//Main header file for the XDK
#include <xtl.h>

LPDIRECT3D8 g_pD3D = NULL;                      // DirectX Object
LPDIRECT3DDEVICE8 g_pD3DDevice = NULL;          // Screen Object

static void InitialiseD3D()
{
    //First of all, create the main D3D object. If it is created successfully we
    //should get a pointer to an IDirect3D8 interface.
    g_pD3D = Direct3DCreate8(D3D_SDK_VERSION);

    //Create a structure to hold the settings for our device
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    //Fill the structure.
    // Set fullscreen 640x480x32 mode
    d3dpp.BackBufferWidth = 640;
    d3dpp.BackBufferHeight = 480;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;

    // Create one backbuffer
    d3dpp.BackBufferCount = 1;

    // Set up how the backbuffer is "presented" to the frontbuffer each time
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

    //Create a Direct3D device.
    g_pD3D->CreateDevice(0, D3DDEVTYPE_HAL, NULL, 
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp, &g_pD3DDevice);
}

static void CleanUpD3D()
{
    g_pD3DDevice->Release();
    g_pD3D->Release();
}

//Application entry point
void __cdecl main()
{
    InitialiseD3D();
    while (true)
    {
        //Clear the backbuffer to blue
        //                                                          r   g   b
        g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

        //Begin the scene
        g_pD3DDevice->BeginScene();

        //Draw stuff here

        //End the scene
        g_pD3DDevice->EndScene();

        //Filp the back and front buffers so that whatever has been rendered on the back buffer
        //will now be visible on screen (front buffer).
        g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
    }
    CleanUpD3D();
}