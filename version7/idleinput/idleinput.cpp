#include <windows.h>

#define DLLEXPORT __declspec(dllexport)

HHOOK KeyboardHook = NULL;
HHOOK MouseHook    = NULL;
BOOL  HaveInput    = FALSE;

/***********************************************************************
*                            InputCheck                                *
***********************************************************************/
extern "C" __declspec (dllexport) BOOL InputCheck()
{
BOOL have_input;

have_input = HaveInput;
HaveInput  = FALSE;

return have_input;
}

/***********************************************************************
*                              MYKBDHOOK                               *
***********************************************************************/
LRESULT CALLBACK MyKbdHook( int code, WPARAM wParam, LPARAM lParam )
{
if ( code==HC_ACTION )
    HaveInput = TRUE;

return ::CallNextHookEx(KeyboardHook, code, wParam, lParam);
}

/***********************************************************************
*                              MyMouseHook                             *
***********************************************************************/
LRESULT CALLBACK MyMouseHook( int code, WPARAM wParam, LPARAM lParam )
{
static POINT LastPt = {0, 0};
MOUSEHOOKSTRUCT * m;

if ( code==HC_ACTION )
    {
    if ( wParam == WM_MOUSEMOVE )
        {
        m = (MOUSEHOOKSTRUCT *) lParam;
        if ( (m->pt.x != LastPt.x) || (m->pt.y != LastPt.y) )
            HaveInput = TRUE;
        LastPt.x = m->pt.x;
        LastPt.y = m->pt.y;
        }
    }
return ::CallNextHookEx(MouseHook, code, wParam, lParam);
}

/***********************************************************************
*                                DllMain                               *
***********************************************************************/
int WINAPI DllMain( HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved )
{
if ( fdwReason == DLL_PROCESS_ATTACH )
    {
    if ( KeyboardHook == NULL )
        {
        KeyboardHook = SetWindowsHookEx( WH_KEYBOARD, MyKbdHook, hInstance, 0 );
        }

    if ( MouseHook == NULL )
        {
        MouseHook = SetWindowsHookEx( WH_MOUSE, MyMouseHook, hInstance, 0 );
        }
    }
else if ( fdwReason == DLL_PROCESS_DETACH )
    {
    if ( KeyboardHook != NULL )
        {
        UnhookWindowsHookEx( KeyboardHook );
        KeyboardHook = NULL;
        }

    if ( MouseHook != NULL )
        {
        UnhookWindowsHookEx( MouseHook );
        MouseHook = NULL;
        }
    }

return TRUE;
}
