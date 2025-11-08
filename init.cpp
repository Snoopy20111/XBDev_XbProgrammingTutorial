//Main header file for the XDK
#include <xtl.h>
#include <D3DX8.h>

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

LPDIRECT3D8 g_pD3D = NULL;                      // DirectX Object
LPDIRECT3DDEVICE8 g_pD3DDevice = NULL;          // Screen Object
LPDIRECT3DVERTEXBUFFER8 g_pVertexBuffer = NULL; // Vertices Buffer
LPDIRECT3DTEXTURE8 pTexture = NULL;             // Texture data

struct CUSTOMVERTEX
{
    FLOAT x, y, z, rhw; // The transformed position for the vertex.
    DWORD colour;       // The vertex colour.
    FLOAT tu, tv;       // UV coordinates
};

static void DrawTexturedTriangle()
{
    VOID* pVertices = NULL;

    // Store each point of the triangle together with it's colour and UV coords
    CUSTOMVERTEX cvVertices[] =
    {//      x       y     z     rhw     color     u     v
        { 250.0f, 100.0f, 0.5f, 1.0f, 0x00FF0000, 250.0f, 100.0f }, // Red
        { 400.0f, 350.0f, 0.5f, 1.0f, 0x0000FF00, 400.0f, 350.0f },	// Green
        { 100.0f, 350.0f, 0.5f, 1.0f, 0x000000FF, 100.0f, 350.0f } // Blue
    };

    // Filepath is "D:\\myTexture.bmp"
    D3DXCreateTextureFromFile(g_pD3DDevice, "D:\\myTexture.bmp", &pTexture);

    // Create the vertex buffer from our device
    g_pD3DDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),  //Length = 3, because it's a triangle
        0,
        D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT,
        &g_pVertexBuffer);

    // Copy to vertex buffer
    g_pVertexBuffer->Lock(0, sizeof(cvVertices), (BYTE**)&pVertices, 0);    // Get a pointer to the vertex buffer vertices and lock the vertex buffer
    memcpy(pVertices, cvVertices, sizeof(cvVertices));                      // Copy our stored vertices values into the vertex buffer
    g_pVertexBuffer->Unlock();                                              // Unlock the vertex buffer

    // Rendering our triangle
    g_pD3DDevice->SetStreamSource(0, g_pVertexBuffer, sizeof(CUSTOMVERTEX));
    g_pD3DDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);

    // Set our background to use our texture buffer
    g_pD3DDevice->SetTexture(0, pTexture);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    // Must release textures in the same way as Verticies
    g_pVertexBuffer->Release();
    pTexture->Release();
}

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
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    // Set up how the backbuffer is "presented" to the frontbuffer each time
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

    // Create a Direct3D device.
    g_pD3D->CreateDevice(0, D3DDEVTYPE_HAL, NULL, 
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp, &g_pD3DDevice);

    // Textures!
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

    // Turn off lighting becuase we are specifying that our vertices have textures colour
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
}

static void CleanUpD3D()
{
    g_pD3DDevice->Release();
    g_pD3D->Release();
}

// Application entry point
void __cdecl main()
{
    InitialiseD3D();
    while (true)
    {
        // Clear the backbuffer to blue
        //                                                          r   g   b
        g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

        // Begin the scene
        g_pD3DDevice->BeginScene();

        // Draw stuff here
        DrawTexturedTriangle();

        // End the scene
        g_pD3DDevice->EndScene();

        // Flip the back and front buffers so that whatever has been rendered on the back buffer
        // will now be visible on screen (front buffer).
        g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
    }
    CleanUpD3D();
}