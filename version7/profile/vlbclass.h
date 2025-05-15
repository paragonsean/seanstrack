class VIRTUAL_LISTBOX_CLASS
    {
    private:

    int   nof_lines;
    int   top_line;
    int   current_line;
    int   lines_per_page;

    DB_TABLE t;
    LISTBOX_CLASS lb;
    HWND          sbw;

    TCHAR * (*lbline)( DB_TABLE & t );

    public:

    VIRTUAL_LISTBOX_CLASS();
    int32   count() { return nof_lines; }
    BOOLEAN startup( TCHAR * fname, short reclen, HWND parent, UINT listbox_id, UINT scrollbar_id, TCHAR * (*lbline_func)( DB_TABLE & t ) );
    void    shutdown();
    void    do_select();
    void    do_vscroll( WPARAM wParam );
    int     do_keypress( int id, int cmd );
    void    fill_listbox();
    void    goto_end();
    void    goto_start();
    void    set_scrollbar();
    void    set_scrollbar_pos();
    void    scroll_down_one_line();
    void    scroll_down_one_page();
    void    scroll_up_one_line();
    void    scroll_up_one_page();
    };

