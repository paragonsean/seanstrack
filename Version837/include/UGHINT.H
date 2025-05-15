#ifdef UG_ENABLE_SCROLLHINTS

class CUGHint {

public:
    CUGHint();
    virtual ~CUGHint();

protected:
    void OnPaint();
    void OnCreate();

public:
    HWND            m_hintWnd;

    CUGCtrl *       m_ctrl;     //pointer to the main class
    LPTSTR          m_text;
    COLORREF        m_textColor;
    COLORREF        m_backColor;
    int             m_windowAlign;
    int             m_textAlign;
    HFONT           m_defaultFont;
    HFONT           m_hFont;
    int             m_fontHeight;

    int SetWindowAlign(int align);
    int SetTextAlign(int align);
    int SetTextColor(COLORREF color);
    int SetBackColor(COLORREF color);
    int SetFont( HFONT font );

    int SetText( LPCTSTR string,int update );
    int MoveHintWindow( int x,int y, int width );

    int Hide();
    int Show();

    BOOL CreateHintWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID);
    static long CALLBACK HintWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
};
#endif
