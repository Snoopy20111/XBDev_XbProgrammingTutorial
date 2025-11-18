#include "init.h"
#include "sound.h"


CSound g_Madonna;

static void RenderBackground()
{
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    D3DVertexBuffer* g_pVertexBuffer = NULL; // Vertices Buffer
    IDirect3DTexture8* pTexture = NULL;

    VOID* pVertices = nullptr;

    struct CUSTOMVERTEX
    {
        FLOAT x, y, z; // The transformed position for the vertex.
        FLOAT tu, tv;  // The vertex texture coordinates
    };

    DWORD D3DFVF_CUSTOMVERTEX = (D3DFVF_XYZ | D3DFVF_TEX1);

    //Store each point of the triangle together with it's colour
    CUSTOMVERTEX cvVertices[] =
    {
        { -1.0f, -1.0f, 0.0f,    0.0f, 1.0f }, // x, y, z, textures (tu, tv) 
        { -1.0f,  1.0f, 0.0f,    0.0f, 0.0f },
        {  1.0f,  1.0f, 0.0f,    1.0f, 0.0f },

        { -1.0f, -1.0f, 0.0f,    0.0f, 1.0f },
        {  1.0f,  1.0f, 0.0f,    1.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f,    1.0f, 1.0f }
    };

    //Load our texture in
    D3DXCreateTextureFromFile(g_pD3DDevice, "D:\\xfactordev.jpg", &pTexture);

    //Create the vertex buffer from our device
    g_pD3DDevice->CreateVertexBuffer(6 * sizeof(CUSTOMVERTEX),
        0,
        D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT,
        &g_pVertexBuffer);

    //Get a pointer to the vertex buffer vertices and lock the vertex buffer
    g_pVertexBuffer->Lock(0, sizeof(cvVertices), (BYTE**)&pVertices, 0);

    //Copy our stored vertices values into the vertex buffer
    memcpy(pVertices, cvVertices, sizeof(cvVertices));

    //Unlock the vertex buffer
    g_pVertexBuffer->Unlock();

    //Rendering our triangle
    g_pD3DDevice->SetStreamSource(0, g_pVertexBuffer, sizeof(CUSTOMVERTEX));
    g_pD3DDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);
    g_pD3DDevice->SetTexture(0, pTexture);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    g_pVertexBuffer->Release();
    pTexture->Release();
}

// Application entry point
void __cdecl main()
{
    InitialiseD3D();

    g_Madonna.Create("D:\\do_it_with_madonna.wav");
    g_Madonna.playsound();

    while (true)
    {
        //Clear the back buffer to blue
        g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

        // Start, Render image, End
        g_pD3DDevice->BeginScene();
        RenderBackground();
        g_pD3DDevice->EndScene();

        // Flip back buffer to the front
        g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
    }

    g_Madonna.Release();

    CleanUpD3D();
    Reboot();
}