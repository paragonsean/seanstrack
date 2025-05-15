/***********************************************
    Ultimate Grid 97
    Copyright 1994 - 1997 Dundas Software Ltd.

    Class
        CUGVScroll
    Purpose
        This class controls the grid's verticle
        scrollbar and is responsible for updating
        it and making sure that it is using the
        specified scroll mode
************************************************/

class CUGVScroll
{
// Construction
public:
    CUGVScroll();

// Attributes
public:

    HWND m_vScrollWnd;
//  HWND m_tooltipWnd;

// Operations
public:

// Implementation
public:
    virtual ~CUGVScroll();

protected:

    friend class    CUGCtrl;
    CUGCtrl *       m_ctrl;
    CUGGridInfo *   m_GI;           //pointer to the grid information

    double          m_multiRange;   //scroll bar multiplication factor
                                    //for setting the scroll range
    double          m_multiPos;     //multiplication factor for setting the
                                    //top row during a thumb track
    long            m_lastMaxTopRow;//last max top row

    int             m_lastScrollMode;

    int             m_lastNumLockRow;

    long            m_trackRowPos;

public:

    //internal functions
    void Update();
    void Moved();
    void VScroll(UINT nSBCode, UINT nPos);
    virtual void OnRButtonDown(UINT nFlags, POINT point);

    BOOL CreateVScrollWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID);
    static long CALLBACK VScrollWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
//  virtual BOOL CreateToolTipWindow(HWND hWndParent);
//  virtual void ToolTipMessage(HWND hWnd,UINT message, WPARAM wParam,LPARAM lParam);
//  virtual int OnToolTip(HWND hWnd, WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////
