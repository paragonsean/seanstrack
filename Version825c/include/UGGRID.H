/***********************************************************************
* Ultimate Grid 97                                                     *
* Copyright 1994 - 1997 Dundas Software Ltd.                           *
*                                                                      *
* CUGGrid                                                              *
*                                                                      *
* This class handles the drawing of the main grid area, plus           *
* handles and forwards events that took place inside of the main       *
* grid area.                                                           *
*                                                                      *
* NOTE: The whole grid is made up of several parts, top heading,       *
* side heading, corner button, grid area, scroll bars and a tab        *
* area.                                                                *
*                                                                      *
* This class handles the drawing of the main grid area and is          *
* responsibile for drawing the grid in an optomized manner.            *
*                                                                      *
* This class handles all mouse and keyboard events and forwards        *
* the events to the CUGCtrl class and other appropriate classes        *
*                                                                      *
***********************************************************************/

class CUGGrid
    {
    public:

    CUGGrid();
    virtual ~CUGGrid();

    void OnPaint();
    void OnKeyDown(UINT nChar, UINT nFlags);
    void OnLButtonDown(UINT nFlags, POINT point);
    void OnLButtonUp(UINT nFlags, POINT point);
    void OnLButtonDblClk(UINT nFlags, POINT point);
    void OnMouseMove(UINT nFlags, POINT point);
    void OnRButtonDown(UINT nFlags, POINT point);
    void OnRButtonUp(UINT nFlags, POINT point);
    void OnChar(UINT nChar, UINT nFlags);
    void OnKillFocus(HWND hNewWnd);
    void OnSetFocus(HWND hOldWnd);
    int  OnMouseActivate(HWND hDesktopWnd, UINT nHitTest, UINT message);
    void OnSize(UINT nType, int cx, int cy);
    BOOL OnMouseWheel(UINT nFlags, short zDelta, POINT point);
    BOOL OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
    void OnKeyUp(UINT nChar, UINT nFlags);
    UINT OnGetDlgCode();
    int OnCreate();
    void OnDestroy(HWND hWnd,WPARAM wParam,LPARAM lParam);

    /*
    -------------
    Internal data
    ------------- */
    HWND            m_gridWnd;
    HWND            m_tooltipWnd;

    CUGCtrl     *   m_ctrl;         //pointer to the main class
    CUGGridInfo *   m_GI;           //pointer to the grid information

    CUGDrawHint     m_drawHint;     //cell drawing hints

    HBITMAP         m_bitmap;       //double buffering

    RECT            m_clientRect;
    BOOL            m_paintOverRide;
    BOOL            m_doubleBufferMode;

    CUGCell         m_cell;         //general purpose cell object

    long            m_keyRepeatCount; //key ballistics repeat counter
    BOOL            m_hasFocus;
    BOOL            m_cellTypeCapture;

    void Update();
    void Moved();
    void DrawCellsIntern( HDC * dc, HDC db_dc );
    void RedrawCell(int col,long row);
    void RedrawRow(long row);
    void RedrawCol(int col);
    void RedrawRange(int startCol, long startRow, int endCol, long endRow);

    int SetDoubleBufferMode( BOOL mode );

    BOOL CreateGridWindow( DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID );
    static long CALLBACK GridWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

    BOOL CreateToolTipWindow(HWND hWndParent);
    void ToolTipMessage(HWND hWnd,UINT message, WPARAM wParam,LPARAM lParam);
    int OnToolTip(HWND hWnd, WPARAM wParam, LPARAM lParam);
};

