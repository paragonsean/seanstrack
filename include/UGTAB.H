/***********************************************
    Ultimate Grid 97
    Copyright 1994 - 1997 Dundas Software Ltd.


    class
        CUGTab
    Purpose
        This is a general purose tab control
        that is built into the grid. It is
        most commonly used to switch grid sheets,
        but can be used for other purposes.
    Details
        -This control can contain upto 64 tabs
        -A notification is sent when a tab is
         selected, it does not change grid sheets
         automatically. Although grid sheets can
         be changed during this notification.
         This method gives the tab control greater
         abilities.
        -Fonts and colors can be setup for the
         tab control
***********************************************/

#define UTABSCROLLID 100

#define UTMAXTABS   64


class CUGTab
{

    public:

    CUGTab();
    virtual ~CUGTab();

    HWND m_tabWnd;
    private:
    void MakeCurrentTabVisible();

    protected:

    void OnPaint();
    int OnCreate();
    void OnLButtonDown(UINT nFlags, POINT point);
    void OnSize(UINT nType, int cx, int cy);
    void OnHScroll(UINT nSBCode, UINT nPos, HWND pScrollBar);

    void OnMouseMove(UINT nFlags, POINT point);
    BOOL OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
    void OnLButtonUp(UINT nFlags, POINT point);
    void OnRButtonDown(UINT nFlags, POINT point);

    LPTSTR      m_tabStrings[UTMAXTABS];
    int         m_tabWidths[UTMAXTABS];
    int         m_tabIDs[UTMAXTABS];
    COLORREF    m_tabTextColors[UTMAXTABS];
    COLORREF    m_tabBackColors[UTMAXTABS];

    int   m_tabCount;
    int   m_tabOffset;
    int   m_maxTabOffset;

    int   m_currentTab;

    int   m_scrollWidth;

    HWND  m_ScrollBarWnd;

    HFONT m_defFont;
    HFONT m_font;

    int   m_resizeReady;
    int   m_resizeInProgress;

    friend class CUGCtrl;
    CUGCtrl     * m_ctrl;
    CUGGridInfo * m_GI;           //pointer to the grid information

    int     m_bestWidth;

    int GetTabItemWidth(LPCTSTR string);

    private:

    friend void CopyString(   LPTSTR & dest, LPCTSTR src );
    friend void CopyString(   LPTSTR & dest, LPCTSTR src, int len );
    friend void AddTwoString( LPTSTR & dest, LPCTSTR src );

    public:

    int AddTab( LPCTSTR string, long ID );
    int InsertTab( int pos, LPCTSTR string, long ID );
    int DeleteTab( long ID );

    int SetCurrentTab( long ID );
    int GetCurrentTab();

    int GetTabCount();

    int AdjustScrollBars();

    int SetTabBackColor( long ID,COLORREF color );
    int SetTabTextColor( long ID,COLORREF color );
    int SetTabFont( HFONT font);

    void OnTabLClicked( long ID );
    void OnTabRClicked( long ID );
    void OnTabDClicked( long ID );
    void OnTabSizing( int width );

    void Update();

    BOOL CreateTabWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID);
    static long CALLBACK TabWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
};


