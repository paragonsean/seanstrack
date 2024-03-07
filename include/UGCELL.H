/***********************************************************************
* Ultimate Grid 97                                                     *
* Copyright 1994 - 1997 Dundas Software Ltd.                           *
*                                                                      *
* CUGCell                                                              *
*                                                                      *
* This class hold all the properties of a cell. It is used             *
* internally and externally to set and retrieve cell properties.       *
*                                                                      *
* Although this is the native format for cell information              *
* datasouces may keep their own data in any format, as long as         *
* they convert the data into CUGCell format when the grid needs        *
* the information.                                                     *
*                                                                      *
* The grid has many functions which allow cell properties to be        *
* set and retrieved without the need to use one of these object        *
* directly (QuickSet and QuickGet functions) But to have the           *
* greatest control, and performance the use of these objects           *
* directly is required.                                                *
*                                                                      *
* Each cell object has over 20 properties which can be set and         *
* retrieved. Each property also has two states SET and NOT-SET.        *
*                                                                      *
* When a property is set a flag in the cell is set to indicate         *
* that the property is SET. The reason for this SET/NOT-SET            *
* design is to allow default styles for a cell. This way when          *
* the grid is gathering cell information for display it can  get       *
* the cells default style(s) first (ie. column defaults, cell          *
* defaults, grid default, heading defaults), then retrieve the         *
* cell properties next, and only the SET properties are                *
* retrieved. This way the a cell can have default values but can       *
* easily over-ride these values as well.                               *
*                                                                      *
* The cells text (or string) property is also its data property.       *
* This means that numbers, times, currency, etc. are all stored        *
* in text format in this property. Even though the cell stores         *
* all data as text, there is a flag which indecates how the text       *
* should be treaded (text, time, currency, numbers etc.).              *
*                                                                      *
***********************************************************************/

#define UGCELL_TEXT_SET         1
#define UGCELL_STRING_SET       1
#define UGCELL_MASK_SET         2
#define UGCELL_LABEL_SET        4
#define UGCELL_DATATYPE_SET     8
#define UGCELL_PARAM_SET        16
#define UGCELL_CELLTYPE_SET     32
#define UGCELL_CELLTYPEEX_SET   64
#define UGCELL_TEXTCOLOR_SET    128
#define UGCELL_BACKCOLOR_SET    256
#define UGCELL_HTEXTCOLOR_SET   512
#define UGCELL_HBACKCOLOR_SET   1024
#define UGCELL_BORDERSTYLE_SET  2048
#define UGCELL_BORDER_SET       2048
#define UGCELL_BORDERCOLOR_SET  4096
#define UGCELL_FONT_SET         8192
#define UGCELL_BITMAP_SET       16384
#define UGCELL_ALIGNMENT_SET    32768
#define UGCELL_JOIN_SET         131072l
#define UGCELL_FORMAT_SET       2097152l
#define UGCELL_NOTUSED          4194304l
#define UGCELL_STYLE_SET        8388608l
#define UGCELL_READONLY_SET     16777216l
#define UGCELL_NUMBERDEC_SET    33554432l

#define UGCELLDATA_STRING       1

class CUGCtrl;

class CUGCell
    {
    protected:

    unsigned long m_propSetFlags;  // One bit is used as a set/unset flag for each cell property

    CUGCellFormat * m_format;      // Pointer to a format class to validate editing and general setting
                                   // also used for diplay formating

    CUGCell * m_cellStyle;         // Pointer to a property style cell object
    TCHAR   * m_string;
    TCHAR   * m_mask;
    TCHAR   * m_label;

    short   m_dataType;     // bit level flag - string,date,time,
                            // time&date,number,bool,binary, currency

    BOOL    m_isReadOnly;
    long    m_param;        // general purpose param
    int     m_cellType;     // cell type index to use
    long    m_cellTypeEx;   // extended cell type param

    COLORREF m_textColor;
    COLORREF m_backColor;
    COLORREF m_hTextColor;
    COLORREF m_hBackColor;

    short   m_borderStyle;
    HPEN    m_borderPen;
    HFONT   m_font;
    HBITMAP m_bitmap;

    short   m_alignment;    // Text alignment

    short   m_joinOrigin;   // Joined cells
    long    m_joinRow;      // Relative position
    long    m_joinCol;

    private:

    friend void CopyString(   LPTSTR & dest, LPCTSTR * src );
    friend void CopyString(   LPTSTR & dest, LPCTSTR * src, int len );
    friend void AddTwoString( LPTSTR & dest, LPCTSTR * src );

    public:

    CUGCell();
    ~CUGCell();

    void ClearAll();

    /*
    ----------------------------------
    Cell information copying functions
    ---------------------------------- */
    int     CopyInfoTo( CUGCell *dest );
    int     CopyInfoFrom( CUGCell *source );
    int     CopyCellInfo( CUGCell *src, CUGCell *dest );

    /*
    ----------------------------------------------------------------------
    Cell information appending funcitons - only updates items that are set
    ---------------------------------------------------------------------- */
    int     AddInfoTo( CUGCell * dest );
    int     AddInfoFrom( CUGCell * source );
    int     AddCellInfo( CUGCell * src, CUGCell * dest );


    BOOL    IsPropertySet( unsigned long propertyFlag );
    int     ClearPropertySetFlag( unsigned long propertyFlag );
    int     ClearProperty( unsigned long propertyFlag );
    unsigned long GetPropertyFlags();
    int     SetPropertyFlags( unsigned long flags );

    /*
    ------------------------
    Text data type functions
    ------------------------ */
    int     SetText( const TCHAR * sorc );
    TCHAR * GetText();
    int     AppendText( const TCHAR * sorc );
    int     GetTextLength();

    int     SetMask( const TCHAR * text );
    int     GetMask( const TCHAR * text, int len);
    TCHAR * GetMask();

    int     SetLabelText( const TCHAR * text );
    TCHAR * GetLabelText();

    /*
    ----------------------
    Set and get data types
    ---------------------- */
    int     SetDataType( short type );
    int     GetDataType();

    int         SetTextColor( COLORREF color );
    COLORREF    GetTextColor();
    int         SetBackColor( COLORREF color );
    COLORREF    GetBackColor();

    int         SetHTextColor( COLORREF color );
    COLORREF    GetHTextColor();
    int         SetHBackColor( COLORREF color );
    COLORREF    GetHBackColor();

    int     SetAlignment( short align );
    short   GetAlignment();

    int     SetBorder( short style );
    short   GetBorder();
    int     SetBorderColor( HPEN pen );
    HPEN    GetBorderColor();

    int     SetFont(HFONT font);
    HFONT   GetFont();

    int     SetBitmap( HBITMAP bitmap );
    HBITMAP GetBitmap();

    /*
    --------------
    Read-only flag
    -------------- */
    int     SetReadOnly( BOOL state );
    BOOL    GetReadOnly();

    /*
    -------------------
    Style type settings
    ------------------- */
    int     SetCellType( int index );
    int     GetCellType();

    int     SetCellTypeEx( long type );
    long    GetCellTypeEx();

    /*
    ----------------
    Join information
    ---------------- */
    int SetJoinInfo( BOOL origin, int col, long row );
    int GetJoinInfo( BOOL * origin, int * col, long * row );

    /*
    ------------------------
    Formating and validation
    ------------------------ */
    int     SetFormatClass(CUGCellFormat * format);
    CUGCellFormat*  GetFormatClass();

    int     SetCellStyle(CUGCell *cell);
    CUGCell * GetCellStyle();

    /*
    --------
    Defaults
    -------- */
    int SetDefaultInfo();

};
