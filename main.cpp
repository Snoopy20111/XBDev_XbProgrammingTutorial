#include "init.h"

#include <stdio.h>
#include <fstream>

// Here are a couple of structures to store our data, its so much tidier to use
// structures, as we can store an XYZ value in the stXYZ stucture, an if we need
// to access it we can just go stXYZ myP;  myP.x = 0;  etc.
struct stXYZ
{
    float x, y, z;
};

struct stFace
{
    int p1, p2, p3;
};

struct CUSTOMVERTEX
{
    FLOAT x, y, z;
    DWORD color;
};

// Global variables.  These will hold the data that contains our shapes.
stXYZ* pVerts;
stFace* pFaces;
int iNum_vectors = 0;
int iNum_polys = 0;

// Loader function. Should work on pretty much anything, because it only depends
// on the standard libraries.
static int loadshape(char* filename)
{
    long int color;
    int i;
    char temp[16];

    // Temp var used to determine if the polygon is made up of 3 sides (triangle or more)
    // Error if iNum_vectors = 0 when returning from function
    int iNum_points = 0; 

    FILE* fin = fopen(filename, "r");

    if (!fin) { return 0; } // If we're here, an error occured opening the file.

    // Read in number of points & polygons in object
    if (fscanf(fin, "%15s %d %d", temp, &iNum_vectors, &iNum_polys) != 3)
    {
        fclose(fin);
        return 0;
    }

    // Initialize our arrays
    pVerts = new stXYZ[iNum_vectors];
    pFaces = new stFace[iNum_polys];

    // Read in all the vectors used in this object
    // Also includes a little error checking
    for (i = 0; i < iNum_vectors; i++)
    {
        if (fscanf(fin, "%f %f %f", &pVerts[i].x, &pVerts[i].y, &pVerts[i].z) != 3)
        {
            fclose(fin);
            return 0;
        }
    }

    // Read in all of the polygon data
    for (i = 0; i < iNum_polys; i++)
    {
        // We could use the color in some way here, but we'll just set all the faces to the same color for simplicity.
        // Also includes a little error checking
        if (fscanf(fin, "%ld", &color) != 1)
        {
            fclose(fin);
            return 0;
        }
        if (fscanf(fin, "%ld", &iNum_points) != 1)
        {
            fclose(fin);
            return 0;
        }
        if (iNum_points > 3)    // Our code only supports triangles
        {
            fclose(fin);
            return 0;
        }
        if (fscanf(fin, "%d %d %d", &pFaces[i].p1, &pFaces[i].p2, &pFaces[i].p3) != 3)
        {
            fclose(fin);
            return 0;
        }
    }
    fclose(fin);
    return 1;
}

// Up until now, there's been no DirectX. Just StdLibs and the 3d file.
// For demonstration, we'll just load the vertex points into DirectX buffers
// and display it; very inefficient but easy to follow.
static void DrawShape()
{

    CUSTOMVERTEX* myVertices = new CUSTOMVERTEX[iNum_polys * 3];
    // Total number of triangles is iNum_polys
    int index = 0;
    for (int i = 0; i < iNum_polys; i++)
    {
        myVertices[index].x = pVerts[pFaces[i].p1].x;
        myVertices[index].y = pVerts[pFaces[i].p1].y;
        myVertices[index].z = pVerts[pFaces[i].p1].z;
        myVertices[index].color = 0xff00ffff;
        index++;

        myVertices[index].x = pVerts[pFaces[i].p2].x;
        myVertices[index].y = pVerts[pFaces[i].p2].y;
        myVertices[index].z = pVerts[pFaces[i].p2].z;
        myVertices[index].color = 0xff0ff00f;
        index++;

        myVertices[index].x = pVerts[pFaces[i].p3].x;
        myVertices[index].y = pVerts[pFaces[i].p3].y;
        myVertices[index].z = pVerts[pFaces[i].p3].z;
        myVertices[index].color = 0xff00ffff;
        index++;
    }

    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer = NULL;
    UINT D3DFVF_CUSTOMVERTEX = (D3DFVF_XYZ | D3DFVF_DIFFUSE);

    g_pD3DDevice->CreateVertexBuffer(3 * iNum_polys * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &pVertexBuffer);
    VOID* pV = nullptr;
    pVertexBuffer->Lock(0, 0, (BYTE**)&pV, 0);
    memcpy(pV, myVertices, sizeof(CUSTOMVERTEX) * iNum_polys * 3);
    pVertexBuffer->Unlock();

    g_pD3DDevice->SetStreamSource(0, pVertexBuffer, sizeof(CUSTOMVERTEX));
    g_pD3DDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, iNum_polys);

    pVertexBuffer->Release();
    delete[] myVertices;
}

static void Render()
{
    // Clear the backbuffer to blue, and clear the Z Buffer
    //                                                                             r  g   b
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

    // Begin the scene
    g_pD3DDevice->BeginScene();

    // Camera. DirectX handles a lot of the matrix math here.
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

    // Scary matrix math, but basically it rotates our shape
    D3DXMATRIX matWorld;
    D3DXMATRIX trans_matrix;
    D3DXMATRIX y_rot_matrix;
    static float y_rot = 0;
    y_rot -= 0.005f;
    //Set up the rotation matrix
    D3DXMatrixRotationY(&y_rot_matrix, y_rot);
    //Set up the translation matrix (move it over to the left a bit)
    D3DXMatrixTranslation(&trans_matrix, -1.0f, 0.0f, 0.0f);
    //Combine out matrices (basically making the rotation and translation one "action")
    D3DXMatrixMultiply(&matWorld, &y_rot_matrix, &trans_matrix);
    //Set our World Matrix (apply the adjustments above)
    g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

    DrawShape();

    // End the scene
    g_pD3DDevice->EndScene();

    // Flip the back and front buffers so that whatever has been rendered on the back buffer
    // will now be visible on screen (front buffer).
    g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

// Application entry point
void __cdecl main()
{
    InitialiseD3D();

    loadshape("D:\\Shape.model");

    while (true)
    {
        Render();
    }
    CleanUpD3D();
}