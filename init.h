#pragma once
//Main header file for the XDK
#include <xtl.h>

static LPDIRECT3D8 g_pD3D = NULL;                      // DirectX Object
static LPDIRECT3DDEVICE8 g_pD3DDevice = NULL;          // Screen Object

void InitialiseD3D();
void CleanUpD3D();