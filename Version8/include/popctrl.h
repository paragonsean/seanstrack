#ifndef _VTW_POPUP_CONTROL_CLASS_
#define _VTW_POPUP_CONTROL_CLASS_

#define BACKGROUND             int(0)
#define FOREGROUND             int(1)
#define GRAY_FOREGROUND        int(2)
#define SELECT_BACKGROUND      int(3)
#define SELECT_FOREGROUND      int(4)
#define SELECT_GRAY_FOREGROUND int(5)
#define NOF_MENU_COLORS        int(6)
#define MAIN_MENU_ITEM_TYPE TEXT('0')
#define SUB_MENU_ITEM_TYPE  TEXT('1')
#define SEPERATOR_TYPE      TEXT('2')
#define COLOR_DEF_TYPE      TEXT('3')


/*
--------------------------
States for menu_item_entry
-------------------------- */
#ifndef _VTW_MENU_CONTROL_CLASS_
static const unsigned NO_ITEM_STATE       = 0;
static const unsigned HOVER_ITEM_STATE    = 1;
static const unsigned SELECTED_ITEM_STATE = 2;
#endif

class POPUP_CONTROL_ENTRY;

class POPUP_ITEM_ENTRY
    {
    public:
    UINT                  id;
    unsigned              state;
    BOOLEAN               is_checked;
    BOOLEAN               is_gray;
    RECTANGLE_CLASS       r;
    STRING_CLASS          s;
    COLORREF              color[NOF_MENU_COLORS];
    POPUP_ITEM_ENTRY();
    ~POPUP_ITEM_ENTRY();
    void    draw( HDC dc, POPUP_CONTROL_ENTRY & owner );
    void    draw_rect( HDC dc );
    };

class POPUP_CONTROL_ENTRY
    {
    public:
    HWND              parent;
    POPUP_ITEM_ENTRY * item;
    int               nof_items;
    int               current_index;
    int               menu_height;
    int               menu_width;
    STRING_CLASS      name;
    WINDOW_CLASS      mw;    /* Main Menu */
    HFONT             font_handle;
    FONT_CLASS        lf;
    int               font_height;
    int               checkmark_width;
    TCHAR             checkmark[3];
    RECTANGLE_CLASS   mr;    /* Client coordinates of mw */
    TEXT_LIST_CLASS   t;

    POPUP_CONTROL_ENTRY();
    ~POPUP_CONTROL_ENTRY();
    static void register_window();
    BOOLEAN create_windows( TCHAR * new_window_name, HWND new_parent );
    void    destroy_windows();
    void    do_left_button_down( WPARAM wParam, LPARAM lParam );
    void    do_left_button_up( WPARAM wParam, LPARAM lParam );
    void    do_mousemove( HWND hWnd, WPARAM wParam, LPARAM lParam );
    POPUP_ITEM_ENTRY * find( UINT id_to_find );
    void    paint_me( HWND hWnd );
    BOOLEAN add( TCHAR * s_to_add, UINT id_to_add );
    BOOLEAN calculate_rectangles();
    void    empty();
    BOOLEAN is_checked( UINT sorc_id );
    BOOLEAN is_gray( UINT sorc_id );
    BOOLEAN set_checkmark( UINT dest_id, BOOLEAN new_checked_state );
    void    set_colors( COLORREF * new_color );
    BOOLEAN set_font( FONT_CLASS & new_font );
    BOOLEAN set_gray( UINT dest_id, BOOLEAN new_gray_state );
    void    show_menu( int x, int y );
    };

#endif
