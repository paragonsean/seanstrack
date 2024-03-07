/***********************************************
    Ultimate Grid 97
    Copyright 1994 - 1997 Dundas Software Ltd.


    class
        CUGSideHdg
    Purpose
        This class handles the drawing of the side
        heading, plus handles and forwards events
        that took place inside of the side heading.
        NOTE: The whole grid is made up of several
        parts, top heading, side heading, corner button,
        grid area, scroll bars and a tab area.
    Details
        -This class handles the drawing of the
         side heading and is responsibile for drawing
         the heading in an optomized manner.
        -This class handles all mouse and keyboard
         events and forwards the events to the
         CUGCtrl class and other appropriate classes
************************************************/

class CUGSideHdg
{
// Construction
public:
    CUGSideHdg();

// Attributes
public:
    HWND m_sideHdgWnd;

// Operations
public:

// Implementation
public:
    virtual ~CUGSideHdg();

    // Generated message map functions
protected:
    void OnPaint();
    void OnLButtonDown(UINT nFlags, POINT point);
    void OnLButtonUp(UINT nFlags, POINT point);
    void OnLButtonDblClk(UINT nFlags, POINT point);
    void OnMouseMove(UINT nFlags, POINT point);
    void OnRButtonDown(UINT nFlags, POINT point);
    void OnRButtonUp(UINT nFlags, POINT point);
    BOOL OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);


protected:

    friend class CUGCtrl;
    CUGCtrl *       m_ctrl;
    CUGGridInfo *   m_GI;           //pointer to the grid information

    CUGCell         m_cell;         //general purpose cell class

    CUGDrawHint     m_drawHint;     //cell drawing hints

    long            m_bottomRow;    //the row drawn at the bottom

    int             m_isSizing;         //sizing flag
    int             m_canSize;          //sizing flag
    int             m_colOrRowSizing;   // 0 - col 1- row
    long            m_sizingColRow;     // column or row number being sized
    int             m_sizingStartSize;  // original col/row width/height
    int             m_sizingStartPos;   // original starting mouse position
    int             m_sizingStartWidth;//original side heading total width
    int             m_sizingNumRowsDown; //number of rows from top when size started

    RECT            m_focusRect;        //focus rect for column sizing option

    int GetCellRect(int *col,long *row,RECT *rect);
    int GetCellRect(int col,long row,RECT *rect);
    int GetCellFromPoint(POINT *point,int *col,long *row,RECT *rect);
    int GetSHColWidth(int col);

    void DrawCellsIntern(HDC dc);

public:

    //internal functions
    void Update();
    void Moved();

    BOOL CreateSideHdgWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID);
    static long CALLBACK SideHdgWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////
