#ifndef _STATICS_
#define _STATICS_

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

void set_statics_font( HWND hWnd, UINT id );
void statics_startup();
void statics_shutdown();
void update_statics( HWND hWnd, RESOURCE_TEXT_ENTRY * rlist, int n );
void set_setup_strings( HWND hWnd, RESOURCE_TEXT_ENTRY * rsn, int nof_rsn, UINT * box, int nof_boxes, TCHAR * title );
#ifdef _WINDOW_CLASS_
void set_statics_font( WINDOW_CLASS & w );
#endif

#endif
