#include "init.h"

// Does what it says on the tin.
// For homebrew stuff, this may not be instantly obvious that it's working
static void Reboot()
{
	LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
	XLaunchNewImage(NULL, (LAUNCH_DATA*)&LaunchData);
}

static bool Gamepad()
{
	XINPUT_GAMEPAD myGamePad{};

    // What XINPUT_GAMEPAD looks like:
    /*
        struct
        {
                WORD    wButtons;
                BYTE    bAnalogButtons[8];
                SHORT   sThumbLX;
                SHORT   sThumbLY;
                SHORT   sThumbRX;
                SHORT   sThumbRY;
        } XINPUT_GAMEPAD;
    */

    DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals);

    static HANDLE pGamePd;
    // dwInsertion contains the 'bitwise' information of currently inserted gamepads
     //
     //     etc
     //         Gamepad 2
     //         | Gamepad 1
     //         | | Gamepad 0
     //         | | |
     // 0 0 ... 0 0 1

    // If there's a controller in the first slot (farthest left), grab its input
    if (dwInsertions & 1)
    {
        pGamePd = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, XDEVICE_NO_SLOT, NULL);
    }

    // If we have input, get the data from that gamepad
    if (pGamePd)
    {
        XINPUT_STATE myInputStates;
        XInputGetState(pGamePd, &myInputStates);
        memcpy(&myGamePad, &myInputStates.Gamepad, sizeof(XINPUT_GAMEPAD));
    }

    // Press the gamepad up button, and the function returns true, else false
    if (myGamePad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
    {
        return true;
    }
    return false;
}

// Application entry point
void __cdecl main()
{
    // Initialize DirectX and Controllers
    InitialiseD3D();
    XInitDevices(0, 0);

    // Just clear the screen once, so you can tell when the application has started
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

    // We keep checking the gamepad until the up D-Pad button is pressed
    // When that's true, leave the loop and reboot the Xbox!
    while (true)
    {
        bool pressedUp = Gamepad();
        if (pressedUp) { break; }
    }

    // Draw one more time, so the final screen is different,
    // then clean up D3D.
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 0, 0), 1.0f, 0);
    CleanUpD3D();

    // Back to the Dashboard
    Reboot();
}