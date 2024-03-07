/***********************************************************************
*   Ultimate Grid 97                                                   *
*   Copyright 1994 - 1997 Dundas Software Ltd.                         *
*                                                                      *
*   CUGnrBtn                                                           *
*                                                                      *
*   This class is used to draw the cells corner button. The            *
*   corner button is the region at the top left of the grid            *
*   between the top and side headings. The width and height of         *
*   this object are determined by the width and height of the          *
*   side and top headings.                                             *
*                                                                      *
*   This class uses a cell located at col -1 by row -1 as its          *
*   source of information                                              *
*                                                                      *
*   This object is drawn using the celltype that the above cell        *
*   object points to                                                   *
*                                                                      *
*   This object gives the user the ability to size the width           *
*   and/or height of the side and top headings. This is done           *
*   when the mouse is moved to the edge and depressed, then            *
*   moved. Sizing nofications are make, which lets the                 *
*   programmer allow or dis-allow the sizing, or even set sizing       *
*   limits.                                                            *
*                                                                      *
***********************************************************************/

class CUGCnrBtn
    {
    public:

    CUGCnrBtn();
    virtual ~CUGCnrBtn();

    /*
    -------------------------------
    Generated message map functions
    ------------------------------- */
    protected:

    void OnPaint();
    BOOL OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
    void OnLButtonDblClk(UINT nFlags, POINT point);
    void OnLButtonDown(UINT nFlags, POINT point);
    void OnLButtonUp(UINT nFlags, POINT point);
    void OnMouseMove(UINT nFlags, POINT point);
    void OnRButtonDown(UINT nFlags, POINT point);
    void OnRButtonUp(UINT nFlags, POINT point);

    public:

    /*
    --------------------
    Internal information
    -------------------- */
    HWND m_cnrBtnWnd;           //pointer to corner button window

    friend class CUGCtrl;
    CUGCtrl     * m_ctrl;     //pointer to the main class
    CUGGridInfo * m_GI;       //pointer to the grid information

    BOOL m_isSizing;     //sizing flag
    BOOL m_canSize;      //sizing flag
    BOOL m_sizingHeight; //sizing flag
    BOOL m_sizingWidth;  //sizing flag

    /*
    ------------------
    Internal functions
    ------------------ */
    void Update( void );
    void Moved();

    BOOL CreateCnrBtnWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID);
    static long CALLBACK CnrBtnWndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

};
