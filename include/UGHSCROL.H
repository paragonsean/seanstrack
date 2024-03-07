/***********************************************
    Ultimate Grid 97
    Copyright 1994 - 1997 Dundas Software Ltd.

    Class
        CUGHScroll
    Purpose
        This class controls the grid's horizontal
        scrollbar and is responsible for updating
        it and making sure that it is using the
        specified scroll mode
************************************************/

class CUGHScroll
{
public:
    CUGHScroll();
    virtual ~CUGHScroll();

    HWND m_hScrollWnd;

protected:

    friend class CUGCtrl;
    CUGCtrl     *   m_ctrl;     //pointer to the main class
    CUGGridInfo *   m_GI;       //pointer to the grid information

    int m_lastMaxLeftCol;
    int m_lastNumLockCols;

    int m_trackColPos;

public:

    //internal functions
    void Update();
    void Moved();

    void HScroll(UINT nSBCode, UINT nPos);
    virtual void OnRButtonDown(UINT nFlags, POINT point);

    BOOL CreateHScrollWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID);
    static long CALLBACK HScrollWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

};
