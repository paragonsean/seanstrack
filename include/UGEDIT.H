/***********************************************************************
*   Ultimate Grid 97                                                   *
*   Copyright 1994 - 1997 Dundas Software Ltd.                         *
*                                                                      *
*   CUGEdit                                                            *
*                                                                      *
*   This is the standard edit control for the grid. The grid can       *
*   use any control derived from CWnd as a edit control.               *
*                                                                      *
*   This control automatically expands to the right as text is         *
*   entered, once the control reaches the right side of the grid       *
*   then it expands downward until it reaches the bottom. Once         *
*   it reaches the bottom then it will start scrolling text as         *
*   it is entered.                                                     *
*                                                                      *
*   When editing first starts the control also automatically           *
*   expands to fit the inital text.                                    *
*                                                                      *
*   When editing finishes it is the responsability of the edit         *
*   control to call the grid's CUGCtrl::EditCtrlFinished method        *
*   to notify the grid that editing has finished.                      *
*                                                                      *
*   The edit control can also call the grid's OnEditVerify             *
*   during editing as well to give the programmer greater              *
*   control over what is being entered.                                *
*                                                                      *
*   New edit controls can be used by the grid by calling               *
*   CUGCtrl::SetNewEditClass. This function can be called just         *
*   before editing starts, thus allowing different cells to use        *
*   different edit controls.                                           *
*                                                                      *
***********************************************************************/

class CUGEdit
    {
    public:

    CUGEdit();
    virtual ~CUGEdit();

    protected:

    void OnKillFocus(HWND hNewWnd);
    void OnSetFocus(HWND hOldWnd);
    void OnKeyDown(UINT nChar, UINT nFlags);
    BOOL OnChar(UINT nChar, UINT nFlags);
    int OnMouseActivate(HWND pDesktopWnd, UINT nHitTest, UINT message);
    BOOL OnSetCursor(HWND pWnd, UINT nHitTest, UINT message);
    UINT OnGetDlgCode();


    BOOL m_cancel;
    BOOL m_continueFlag;
    int  m_continueCol;
    int  m_continueRow;

    BOOL m_autoSize;

    int m_origHeight;
    int m_currentHeight;

    void CalcEditRect();

    private:

    friend void CopyString(LPTSTR & dest, LPCTSTR src);
    friend void CopyString(LPTSTR & dest, LPCTSTR src, int len);
    friend void AddTwoString(LPTSTR & dest, LPCTSTR src);

    public:

    HWND    m_editWnd;

    CUGCtrl * m_ctrl;

    BOOL SetAutoSize(BOOL state);
    void OnUpdate();

    BOOL CreateEditWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID);
    static long CALLBACK EditWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
    };
