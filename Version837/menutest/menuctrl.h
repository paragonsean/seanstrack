#ifndef _VTW_MENU_CONTROL_CLASS_
#define _VTW_MENU_CONTROL_CLASS_

#define BACKGROUND int(0)
#define FOREGROUND int(1)
#define SELECT_BACKGROUND int(2)
#define SELECT_FOREGROUND int(3)
#define NOF_COLORS        int(4)

#define MAIN_MENU_ITEM_TYPE TEXT('0')
#define SUB_MENU_ITEM_TYPE  TEXT('1')
#define SEPERATOR_TYPE      TEXT('2')
#define COLOR_DEF_TYPE      TEXT('3')

/*
-----------------------------
States for menu_control_entry
----------------------------- */
#define NO_CONTROL_STATE           unsigned( 0 )
#define SELECTED_CONTROL_STATE     unsigned( 1 )
#define WAS_SELECTED_CONTROL_STATE unsigned( 2 )

/*
--------------------------
States for menu_item_entry
-------------------------- */
static const unsigned NO_ITEM_STATE       = 0;
static const unsigned HOVER_ITEM_STATE    = 1;
static const unsigned SELECTED_ITEM_STATE = 2;

class MENU_CONTROL_ENTRY;

class MENU_ITEM_ENTRY
    {
    public:
    UINT                 id;
    int                  password_level;
    unsigned             state;
    RECTANGLE_CLASS      r;
    STRING_CLASS         s;
    COLORREF             color[NOF_COLORS];
    MENU_CONTROL_ENTRY * owner;
    MENU_ITEM_ENTRY    * submenu;
    int                  nof_submenu_items;
    int                  submenu_height; /* Pixels */
    int                  submenu_width;  /* Pixels */
    MENU_ITEM_ENTRY();
    ~MENU_ITEM_ENTRY();
    void    draw( HDC dc );
    void    draw_rect( HDC dc );
    void    draw_submenu_item( HDC dc, UINT id );
    BOOLEAN get( TCHAR * record );
    };

class MENU_CONTROL_ENTRY
    {
    public:
    HWND              parent;
    UINT              id;
    STRING_CLASS      language;
    MENU_ITEM_ENTRY * main;
    int               nof_main_items;
    int               main_index;
    int               sub_index;
    unsigned          state;
    STRING_CLASS      name;
    WINDOW_CLASS      mw;    /* Main Menu */
    WINDOW_CLASS      sw;    /* Popup submenu */
    HFONT             font_handle;
    FONT_CLASS        lf;
    RECTANGLE_CLASS   mr;    /* Client coordinates of mw */
    RECTANGLE_CLASS   sr;    /* Screen coordinates of sw */
    TEXT_LIST_CLASS   t;

    MENU_CONTROL_ENTRY();
    ~MENU_CONTROL_ENTRY();
    static void register_window();
    BOOLEAN create_windows( TCHAR * new_window_name, HWND new_parent );
    void    destroy_windows();
    void    do_left_button_down( WPARAM wParam, LPARAM lParam );
    void    do_left_button_up( WPARAM wParam, LPARAM lParam );
    void    do_mousemove( HWND hWnd, WPARAM wParam, LPARAM lParam );
    void    paint_me( HWND hWnd );
    void    paint_submenu( HWND hWnd );
    BOOLEAN read( NAME_CLASS & s );
    BOOLEAN resize();
    void    show_submenu();
    };

#endif
