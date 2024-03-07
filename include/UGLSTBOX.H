/***********************************************************************
* Ultimate Grid 97                                                     *
* Copyright 1994 - 1997 Dundas Software Ltd.                           *
*                                                                      *
* CUGLstBox                                                            *
*                                                                      *
* This class is used by the cugdroplist class as the listbox           *
* that appears when the droplist is activated.                         *
*                                                                      *
* This class is responsible for returning any selection make in        *
* the listbox, and destroying itself when an item is selected or       *
* when the selection process is canceled.                              *
*                                                                      *
***********************************************************************/

class CUGLstBox
    {
    public:

    CUGLstBox();
    virtual ~CUGLstBox();

    /*
    -------------------------------
    Generated message map functions
    ------------------------------- */
    protected:

    void OnKillFocus(HWND hNewWnd);
    int OnMouseActivate(HWND hDesktopWnd, UINT nHitTest, UINT message);
    void OnLButtonUp(HWND hWnd, UINT nFlags, POINT point);
    void OnKeyDown(HWND hWnd, UINT nChar, UINT nFlags);
    void OnSetFocus(HWND hOldWnd);

    public:

    static long CALLBACK ListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

    HWND        m_listBoxWnd;

    CUGCtrl *   m_ctrl;
    BOOL        m_HasFocus;
    int     *   m_col;
    long    *   m_row;
    BOOL        m_selected;

    int         m_cellTypeId;

    private:

    void Select(HWND hWnd);

};
