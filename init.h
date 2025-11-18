#pragma once
//Main header file for the XDK
#include <xtl.h>

extern LPDIRECT3D8 g_pD3D;                      // DirectX Object
extern LPDIRECT3DDEVICE8 g_pD3DDevice;          // Screen Object

void InitialiseD3D();
void CleanUpD3D();
void Reboot();