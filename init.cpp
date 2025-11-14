#include "init.h"

void InitialiseD3D()
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
    g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);       // Turn on z-buffering
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    //Turn off culling - so we can see the back of the sphere
}

void CleanUpD3D()
{
    g_pD3DDevice->Release();
    g_pD3D->Release();
}