//Main header file for the XDK
#include <xtl.h>
#include <D3DX8.h> //used for DirectX rendering

LPDIRECT3D8 g_pD3D = NULL;                      // DirectX Object
LPDIRECT3DDEVICE8 g_pD3DDevice = NULL;          // Screen Object

static void InitialiseD3D()
{
    // First of all, create the main D3D object. If it is created successfully we
    // should get a pointer to an IDirect3D8 interface.
    g_pD3D = Direct3DCreate8(D3D_SDK_VERSION);

    // Create a structure to hold the settings for our device
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    // Fill the structure.
    // Set fullscreen 640x480x32 mode
    d3dpp.BackBufferWidth = 640;
    d3dpp.BackBufferHeight = 480;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;

    // Create one backbuffer
    d3dpp.BackBufferCount = 1;

    // Tells DirectX to draw only the closest object if one is front of others;
    // Basically, don't draw the back side of the sphere! Very handy for performance.
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    // Set up how the backbuffer is "presented" to the frontbuffer each time
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

    // Create a Direct3D device.
    g_pD3D->CreateDevice(0, D3DDEVTYPE_HAL, NULL, 
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp, &g_pD3DDevice);

    
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);            // Turn off lighting because we have vertex colors
    g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);        // Turn on z-buffering
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);     //Turn off culling - so we can see the back of the sphere
}

static void CleanUpD3D()
{
    g_pD3DDevice->Release();
    g_pD3D->Release();
}

static void DrawSphere()
{
    // Very inefficient, but goes through all the steps necessary
    // Creates and deletes vertex and index buffers, and re-generates a sphere every frame too.
    UINT D3DFVF_CUSTOMVERTEX = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE);
    struct CUSTOMVERTEX
    {
        FLOAT x, y, z;      // Vertex Position
        FLOAT nx, ny, nz;   // Direction the vertex is facing (normal)
        DWORD color;        // Vertex Color
    };

    //These two variables determine the quality (and poly count) of our sphere.
    int nRings = 15;
    int nSegments = 12;
    DWORD dwNumOfVertices = (nRings + 1) * (nSegments + 1);     // These follow logically from the parameters above
    DWORD dwNumOfIndices = 2 * nRings * (nSegments + 1);

    // Create our Vertex and Index buffers
    // An Index buffer is basically a list of our individual meshes
    // In this example, each poly is treated separately, so they can be exploded later
    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer = NULL;
    IDirect3DIndexBuffer8* pIndexBuffer = NULL;
    g_pD3DDevice->CreateVertexBuffer(dwNumOfVertices * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &pVertexBuffer);
    g_pD3DDevice->CreateIndexBuffer(dwNumOfIndices * sizeof(WORD),
        0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
        &pIndexBuffer);

    // Create pointers to our vertex and index buffers
    // These will be used to fill the buffers one value at a time
    CUSTOMVERTEX* pVertex = nullptr;
    WORD* pIndices = nullptr;
    // And then lock the buffers so they don't get accessed by anything else until we're done
    pVertexBuffer->Lock(0, 0, (BYTE**)&pVertex, 0);
    pIndexBuffer->Lock(0, dwNumOfIndices, (BYTE**)&pIndices, 0);


    WORD wVertexIndex = 0;
    D3DXVECTOR3 vNormal;

    // Set up some angles, where the loops will be "cut" in the sphere
    float rDeltaRingAngle = (D3DX_PI / nRings);
    float rDeltaSegAngle = (2.0f * D3DX_PI / nSegments);

    float red = 0.0f, green = 1.0f, blue = 0.0f;
    // Generate the group of rings for the sphere
    for (int nCurrentRing = 0; nCurrentRing < nRings + 1; nCurrentRing++)
    {
        float r0 = sinf(nCurrentRing * rDeltaRingAngle);
        float y0 = cosf(nCurrentRing * rDeltaRingAngle);

        // Generate the group of segments for the current ring
        for (int nCurrentSegment = 0; nCurrentSegment < nSegments + 1; nCurrentSegment++)
        {
            float x0 = r0 * sinf(nCurrentSegment * rDeltaSegAngle);
            float z0 = r0 * cosf(nCurrentSegment * rDeltaSegAngle);

            vNormal.x = x0;
            vNormal.y = y0;
            vNormal.z = z0;

            D3DXVec3Normalize(&vNormal, &vNormal);

            // Add one vector to the strip which makes up the sphere
            pVertex->x = x0;
            pVertex->y = y0;
            pVertex->z = z0;
            pVertex->nx = vNormal.x;
            pVertex->ny = vNormal.y;
            pVertex->nz = vNormal.z;

            // Some colors just to make it more obvious
            pVertex->color = D3DXCOLOR(red, green, blue, 1.0f);
            red += 0.02f;
            blue += 0.01f;
            green -= 0.015f;

            pVertex++;

            // Add two indices except for the last ring
            if (nCurrentRing != nRings)
            {
                *pIndices = wVertexIndex;
                pIndices++;

                *pIndices = wVertexIndex + (WORD)(nSegments + 1);
                pIndices++;

                wVertexIndex++;
            }
        }
    }

    // We're done filling the buffers, so unlock them
    pIndexBuffer->Unlock();
    pVertexBuffer->Unlock();

    g_pD3DDevice->SetStreamSource(0, pVertexBuffer, sizeof(CUSTOMVERTEX));
    g_pD3DDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);
    // Select the index buffer
    g_pD3DDevice->SetIndices(pIndexBuffer, 0);

    DWORD dwNumOfPolygons = dwNumOfIndices - 2;
    // Render the polygons from the index buffer.
    // Note: you can change D3DPT_LINESTRIP to D3DPT_TRIANGLESTRIP to see the sphere shaded in.
    // By using the linestrip, we see the sphere in wireframe mode!
    g_pD3DDevice->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, dwNumOfVertices, 0, dwNumOfPolygons);

    pIndexBuffer->Release();
    pVertexBuffer->Release();
}

// Application entry point
void __cdecl main()
{
    InitialiseD3D();
    while (true)
    {
        // Clear the backbuffer to blue, and clear the Z Buffer
        //                                                                           r   g   b
        g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

        // Begin the scene
        g_pD3DDevice->BeginScene();

        // Camera
        // DirectX will do a lot of the matrix math for us, but it's still a little scary!
        D3DXMATRIX view_matrix, matProj;

        D3DXMatrixLookAtLH(&view_matrix,        // Result Matrix
            &D3DXVECTOR3(0.0f, 0.0f, -5.0f),    // "Eye" (camera) location
            &D3DXVECTOR3(0.0f, 0.0f, 0.0f),     // "Look At" location
            &D3DXVECTOR3(0.0f, 1.0f, 0.0f)      // "Up" direction
        );
        g_pD3DDevice->SetTransform(D3DTS_VIEW, &view_matrix);

        D3DXMatrixPerspectiveFovLH(&matProj,    // Result Matrix
            D3DX_PI / 3,                        // Field of View, in radians (60°)
            (600.0f / 400.0f),                  // Aspect ratio
            1.0f,                               // Near view plane
            1000.0f                             // Far view plane
        );
        g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

        // Finally, draw the sphere
        DrawSphere();

        // End the scene
        g_pD3DDevice->EndScene();

        // Flip the back and front buffers so that whatever has been rendered on the back buffer
        // will now be visible on screen (front buffer).
        g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
    }
    CleanUpD3D();
}