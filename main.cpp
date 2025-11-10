//Main header file for the XDK
#include <xtl.h>
//#include <D3DX8.h> //used for DirectX rendering

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

    
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);            // Turn off lighting because we have vertex colors
    g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);        // Turn on z-buffering
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);     //Turn off culling - so we can see the back of the sphere
}

static void CleanUpD3D()
{
    g_pD3DDevice->Release();
    g_pD3D->Release();
}

static void DrawSphereTriangleList()
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
    int nRings = 4;
    int nSegments = 4;
    DWORD dwNumOfVertices = (nRings + 1) * (nSegments + 1);     // These follow logically from the parameters above
    DWORD dwNumOfPolygons = dwNumOfVertices * 3;

    // Create our Vertex buffer
    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer = NULL;
    CUSTOMVERTEX pVertex[25 * 3] = { 0 };


    D3DXVECTOR3 vNormal;
    int nCurrentRing = 0;

    // Set up some angles, where the loops will be "cut" in the sphere
    float rDeltaRingAngle = (D3DX_PI / nRings);
    float rDeltaSegAngle = (2.0f * D3DX_PI / nSegments);


    // Part 1: Read in all the vertices that make up the shape
    // Generate the group of rings for the sphere
    for (nCurrentRing = 0; nCurrentRing < nRings + 1; nCurrentRing++)
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
            int i = nCurrentRing * nSegments + nCurrentSegment;

            pVertex[i].x = x0;
            pVertex[i].y = y0;
            pVertex[i].z = z0;
            pVertex[i].nx = vNormal.x;
            pVertex[i].ny = vNormal.y;
            pVertex[i].nz = vNormal.z;
            pVertex[i].color = 0xff00ff00; // 0 Red, 255 Green, 0 Blue
        }
    }

    // Part 2: Arrange them in list order
    // This part puts all of our vertices into a nice tidy order of triangles.
    //CUSTOMVERTEX pVertexList[dwNumOfVertices*3];
    CUSTOMVERTEX pVertexList[50 * 3] = { 0 };

    int index = 0;

    for (nCurrentRing = 0; nCurrentRing < nRings + 1; nCurrentRing++)
    {
        for (int nCurrentSegment = 0; nCurrentSegment < nSegments + 1; nCurrentSegment++)
        {
            if (nCurrentRing != nRings)
            {
                // These lines specify the triangles into the pVertexList for drawing.
                // Those in part 1 aren't in order of triangles (e.g. points 123 make triangle 1,
                // points 456 make triangle 2, etc.
                // This section takes the points and generates a list of triangles
                // which we can draw using D3DPT_TRIANGLELIST
                
                // Extremely messy. Surely this could be condensed?

                int i = nCurrentRing * nSegments;

                pVertexList[index].x = pVertex[i + nCurrentSegment].x;
                pVertexList[index].y = pVertex[i + nCurrentSegment].y;
                pVertexList[index].z = pVertex[i + nCurrentSegment].z;

                pVertexList[index].nx = pVertex[i + nCurrentSegment].nx;
                pVertexList[index].ny = pVertex[i + nCurrentSegment].ny;
                pVertexList[index].nz = pVertex[i + nCurrentSegment].nz;

                pVertexList[index].color = pVertex[i + nCurrentSegment].color;

                index++;
                ///////////////////////////////////////////////////////////////

                pVertexList[index].x = pVertex[i + nCurrentSegment + nSegments].x;
                pVertexList[index].y = pVertex[i + nCurrentSegment + nSegments].y;
                pVertexList[index].z = pVertex[i + nCurrentSegment + nSegments].z;

                pVertexList[index].nx = pVertex[i + nCurrentSegment + nSegments].nx;
                pVertexList[index].ny = pVertex[i + nCurrentSegment + nSegments].ny;
                pVertexList[index].nz = pVertex[i + nCurrentSegment + nSegments].nz;

                pVertexList[index].color = pVertex[i + nCurrentSegment + nSegments].color;

                index++;
                //////////////////////////////////////////////////////////////

                pVertexList[index].x = pVertex[i + nCurrentSegment + 1].x;
                pVertexList[index].y = pVertex[i + nCurrentSegment + 1].y;
                pVertexList[index].z = pVertex[i + nCurrentSegment + 1].z;

                pVertexList[index].nx = pVertex[i + nCurrentSegment + 1].nx;
                pVertexList[index].ny = pVertex[i + nCurrentSegment + 1].ny;
                pVertexList[index].nz = pVertex[i + nCurrentSegment + 1].nz;

                pVertexList[index].color = pVertex[i + nCurrentSegment + 1].color;

                index++;

                /////////////////////////////////////////////////////////////
                /////////////////////////////////////////////////////////////
                /////////////////////////////////////////////////////////////

                pVertexList[index].x = pVertex[i + nCurrentSegment + nSegments].x;
                pVertexList[index].y = pVertex[i + nCurrentSegment + nSegments].y;
                pVertexList[index].z = pVertex[i + nCurrentSegment + nSegments].z;

                pVertexList[index].nx = pVertex[i + nCurrentSegment + nSegments].nx;
                pVertexList[index].ny = pVertex[i + nCurrentSegment + nSegments].ny;
                pVertexList[index].nz = pVertex[i + nCurrentSegment + nSegments].nz;

                pVertexList[index].color = pVertex[i + nCurrentSegment + nSegments].color;

                index++;
                ///////////////////////////////////////////////////////////////

                pVertexList[index].x = pVertex[i + nCurrentSegment + nSegments + 1].x;
                pVertexList[index].y = pVertex[i + nCurrentSegment + nSegments + 1].y;
                pVertexList[index].z = pVertex[i + nCurrentSegment + nSegments + 1].z;

                pVertexList[index].nx = pVertex[i + nCurrentSegment + nSegments + 1].nx;
                pVertexList[index].ny = pVertex[i + nCurrentSegment + nSegments + 1].ny;
                pVertexList[index].nz = pVertex[i + nCurrentSegment + nSegments + 1].nz;

                pVertexList[index].color = pVertex[i + nCurrentSegment + nSegments].color;

                index++;
                //////////////////////////////////////////////////////////////

                pVertexList[index].x = pVertex[i + nCurrentSegment + 1].x;
                pVertexList[index].y = pVertex[i + nCurrentSegment + 1].y;
                pVertexList[index].z = pVertex[i + nCurrentSegment + 1].z;

                pVertexList[index].nx = pVertex[i + nCurrentSegment + 1].nx;
                pVertexList[index].ny = pVertex[i + nCurrentSegment + 1].ny;
                pVertexList[index].nz = pVertex[i + nCurrentSegment + 1].nz;

                pVertexList[index].color = pVertex[i + nCurrentSegment + 1].color;

                index++;
            }
        }
    }

    g_pD3DDevice->CreateVertexBuffer(index * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &pVertexBuffer);

    VOID* pV = nullptr;
    pVertexBuffer->Lock(0, 0, (BYTE**)&pV, 0);
    memcpy(pV, pVertexList, sizeof(CUSTOMVERTEX)* index);
    pVertexBuffer->Unlock();

    g_pD3DDevice->SetStreamSource(0, pVertexBuffer, sizeof(CUSTOMVERTEX));
    g_pD3DDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, index / 3); //dwNumOfPolygons

    pVertexBuffer->Release();
}

static void setLight()
{
    //Set up a light
    D3DLIGHT8 d3dLight;

    //Initialize the light structure
    ZeroMemory(&d3dLight, sizeof(D3DLIGHT8));

    //Set up a white point light at (0, 0, -10)
    d3dLight.Type = D3DLIGHT_POINT;

    d3dLight.Diffuse.r = 1.0f;
    d3dLight.Diffuse.g = 1.0f;
    d3dLight.Diffuse.b = 1.0f;

    d3dLight.Ambient.r = 0.0f;
    d3dLight.Ambient.g = 0.0f;
    d3dLight.Ambient.b = 0.0f;

    d3dLight.Specular.r = 0.0f;
    d3dLight.Specular.g = 0.0f;
    d3dLight.Specular.b = 0.0f;

    d3dLight.Position.x = 0.0f;
    d3dLight.Position.y = 10.0f;    //2 (???)
    d3dLight.Position.z = -10.0f;

    d3dLight.Attenuation0 = 1.0f;
    d3dLight.Attenuation1 = 0.0f;
    d3dLight.Attenuation2 = 0.0f;
    d3dLight.Range = 100.0f;

    // Turn on lighting
    g_pD3DDevice->SetLight(0, &d3dLight);                                       // Set our new light to Light 0
    g_pD3DDevice->LightEnable(0, TRUE);                                         // Enable Light 0
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);                         // Enable lighting  
    g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(5, 100, 32));     // Add ambient light, mostly dark blue-ish
}

static void setMaterial()
{
    D3DMATERIAL8 matMaterial;
    D3DCOLORVALUE rgbaDiffuse = { 1.0, 1.0, 1.0, 0.0 };     //White, no alpha
    D3DCOLORVALUE rgbaAmbient = { 1.0, 1.0, 1.0, 0.0 };
    D3DCOLORVALUE rgbaSpecular = { 0.0, 0.0, 0.0, 0.0 };    //Black, aka won't affect anything
    D3DCOLORVALUE rgbaEmissive = { 0.0, 0.0, 0.0, 0.0 };

    matMaterial.Diffuse = rgbaDiffuse;
    matMaterial.Ambient = rgbaAmbient;
    matMaterial.Specular = rgbaSpecular;
    matMaterial.Emissive = rgbaEmissive;
    matMaterial.Power = 100.0f;

    g_pD3DDevice->SetMaterial(&matMaterial);
}

// Application entry point
void __cdecl main()
{
    InitialiseD3D();

    setLight();
    setMaterial();

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
        // End Camera


        // Scary matrix math, but basically it rotates our sphere
        D3DXMATRIX matWorld;
        D3DXMATRIX trans_matrix;
        D3DXMATRIX y_rot_matrix;
        static float y_rot = 0;
        y_rot -= 0.001f;
        //Set up the rotation matrix for the triangle
        D3DXMatrixRotationY(&y_rot_matrix, y_rot);
        //Set up the translation matrix (move it over to the left a bit)
        D3DXMatrixTranslation(&trans_matrix, -1.0f, 0.0f, 0.0f);
        //Combine out matrices (basically making the rotation and translation one "action")
        D3DXMatrixMultiply(&matWorld, &y_rot_matrix, &trans_matrix);
        //Set our World Matrix (apply the adjustments above
        g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

        // Finally, draw the sphere
        DrawSphereTriangleList();

        // End the scene
        g_pD3DDevice->EndScene();

        // Flip the back and front buffers so that whatever has been rendered on the back buffer
        // will now be visible on screen (front buffer).
        g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
    }
    CleanUpD3D();
}