/***********************************************
    Ultimate Grid 97
    Copyright 1994 - 1997 Dundas Software Ltd.


    class
        CUGTopHdg
    Purpose
        This class handles the drawing of the top
        heading, plus handles and forwards events
        that took place inside of the top heading.
        NOTE: The whole grid is made up of several
        parts, top heading, side heading, corner button,
        grid area, scroll bars and a tab area.
    Details
        -This class handles the drawing of the
         top heading and is responsibile for drawing
         the heading in an optomized manner.
        -This class handles all mouse and keyboard
         events and forwards the events to the
         CUGCtrl class and other appropriate classes
************************************************/

class CUGTopHdg
{
// Construction
public:
    CUGTopHdg();

// Attributes
public:
    HWND m_topHdgWnd;

// Operations
public:

// Implementation
public:
    virtual ~CUGTopHdg();

    // Generated message map functions
    void OnPaint();
    void OnMouseMove(UINT nFlags, POINT point);
    void OnLButtonDown(UINT nFlags, POINT point);
    void OnLButtonUp(UINT nFlags, POINT point);
    void OnLButtonDblClk(UINT nFlags, POINT point);
    void OnRButtonDown(UINT nFlags, POINT point);
    void OnRButtonUp(UINT nFlags, POINT point);
    BOOL OnSetCursor(HWND pWnd, UINT nHitTest, UINT message);

protected:

    friend class CUGCtrl;
    CUGCtrl *       m_ctrl;
    CUGGridInfo *   m_GI;       //pointer to the grid information

    CUGCell         m_cell;     //general purpose cell class


    BOOL            m_isSizing;         //sizing flag
    BOOL            m_canSize;          //sizing flag
    BOOL            m_colOrRowSizing;   // 0-col 1-row
    int             m_sizingColRow;     //column/row being sized
    int             m_sizingStartSize;  //original size
    int             m_sizingStartPos;   //original start pos
    int             m_sizingStartHeight;//original top heading total height

    RECT            m_focusRect;        //focus rect for column sizing option

    CUGDrawHint     m_drawHint;     //cell drawing hints

    int             m_swapStartCol;
    int             m_swapEndCol;

    int GetCellRect(int *col,int *row,RECT *rect);
    int GetCellRect(int col,int row,RECT *rect);
    int GetCellFromPoint(POINT *point,int *col,int *row,RECT *rect);
    int GetJoinRange(int *col,int *row,int *endCol,int *endRow);
    int GetTHRowHeight(int row);

    void DrawCellsIntern(HDC dc);

    void CheckForUserResize(POINT *point);

public:

    void Update();
    void Moved();
    BOOL CreateTopHdgWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID);
    static long CALLBACK TopHdgWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
};

