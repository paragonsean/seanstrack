/***************************************************
Ultimate Grid 97 for SDK
Dundas Software Ltd. 1994-1997

  Standard Defines
****************************************************/

//UGID structure defines
#define LPCUGID const UGID *

//movement defines
#define UG_LINEUP   0
#define UG_LINEDOWN 1
#define UG_PAGEUP   2
#define UG_PAGEDOWN 3
#define UG_TOP      4
#define UG_BOTTOM   5

//common return codes  - error and warning codes are positive values
#define UG_SUCCESS      0
#define UG_ERROR        1
#define UG_NA           -1
#define UG_NO_INDEX     -1

#define UG_PROCESSED    1

//data source
#define UG_MAXDATASOURCES 5

//scrolling
#define UG_SCROLLNORMAL     0
#define UG_SCROLLTRACKING   1
#define UG_SCROLLJOYSTICK   2

//alignment defines
#define UG_ALIGNLEFT        1
#define UG_ALIGNRIGHT       2
#define UG_ALIGNCENTER      4
#define UG_ALIGNTOP         8
#define UG_ALIGNBOTTOM      16
#define UG_ALIGNVCENTER     32

//border style defines
#define UG_BDR_LTHIN        1
#define UG_BDR_TTHIN        2
#define UG_BDR_RTHIN        4
#define UG_BDR_BTHIN        8
#define UG_BDR_LMEDIUM      16
#define UG_BDR_TMEDIUM      32
#define UG_BDR_RMEDIUM      64
#define UG_BDR_BMEDIUM      128
#define UG_BDR_LTHICK       256
#define UG_BDR_TTHICK       512
#define UG_BDR_RTHICK       1024
#define UG_BDR_BTHICK       2048
#define UG_BDR_RECESSED     4096
#define UG_BDR_RAISED       8192

//sorting
#define UG_SORT_ASCENDING       1
#define UG_SORT_DESCENDING      2

//finding
#define UG_FIND_PARTIAL         1
#define UG_FIND_CASEINSENSITIVE 2
#define UG_FIND_UP              4
#define UG_FIND_ALLCOLUMNS      8

//printing defines
#define UG_PRINT_TOPHEADING     1
#define UG_PRINT_SIDEHEADING    2
#define UG_PRINT_LEFTMARGIN     3
#define UG_PRINT_TOPMARGIN      4
#define UG_PRINT_RIGHTMARGIN    5
#define UG_PRINT_BOTTOMMARGIN   6

#define UG_PRINT_LANDSCAPE      7
#define UG_PRINT_SMALL          8
#define UG_PRINT_LARGE          9
#define UG_PRINT_FRAME          10

//best fit defines
#define UG_BESTFIT_TOPHEADINGS  1
#define UG_BESTFIT_AVERAGE      2


//bit defines
#define BIT0    1
#define BIT1    2
#define BIT2    4
#define BIT3    8
#define BIT4    16
#define BIT5    32
#define BIT6    64
#define BIT7    128
#define BIT8    256
#define BIT9    512
#define BIT10   1024
#define BIT11   2048
#define BIT12   4096
#define BIT13   8192
#define BIT14   16384
#define BIT15   32768
#define BIT16   65536l
#define BIT17   131072l
#define BIT18   262144l
#define BIT19   524288l
#define BIT20   1048576l
#define BIT21   2097152l
#define BIT22   4194304l
#define BIT23   8388608l
#define BIT24   16777216l
#define BIT25   33554432l
#define BIT26   67108864l
#define BIT27   134217728l
#define BIT28   268435456l
#define BIT29   536870912l
#define BIT30   1073741824l
#define BIT31   2147483648l

//Normal CellType/CellType Ex Values - most other cell types
//also use these values, so bits 1-8 are reserved generally reserved
#define UGCT_NORMAL             0
#define UGCT_NORMALSINGLELINE   BIT0
#define UGCT_NORMALMULTILINE    BIT1
#define UGCT_NORMALELLIPSIS     BIT2
#define UGCT_NORMALLABELTEXT    BIT3


//Droplist CellType/CellTypeEx values
#define UGCT_DROPLIST           1
#define UGCT_DROPLISTHIDEBUTTON BIT9
//Droplist OnCellType notifications
#define UGCT_DROPLISTSTART  100
#define UGCT_DROPLISTSELECT 101

//Arrow CellType/CellTypeEx values
#define UGCT_ARROW          3
#define UGCT_ARROWRIGHT     BIT4
#define UGCT_ARROWLEFT      BIT5
#define UGCT_ARROWDRIGHT    BIT6
#define UGCT_ARROWDLEFT     BIT7


//SpinButton CellType/CellTypeEx Values
//Spinbutton OnCellType notifications
#define UGCT_SPINBUTTONUP   16
#define UGCT_SPINBUTTONDOWN 17


//Grid Sections - used by menu commands, etc
#define UG_GRID             (WM_USER+201)
#define UG_TOPHEADING       (WM_USER+202)
#define UG_SIDEHEADING      (WM_USER+203)
#define UG_CORNERBUTTON     (WM_USER+204)
#define UG_HSCROLL          (WM_USER+205)
#define UG_VSCROLL          (WM_USER+206)
#define UG_TAB              (WM_USER+207)

//control define
#define UG_EDITID           (WM_USER+211)
#define UG_LISTBOXID        (WM_USER+212)
#define UG_TOOLTIPID        (WM_USER+213)


#define UG_THIS_PTR_CTRL        (WM_USER+240)
#define UG_THIS_PTR_GRID        (WM_USER+241)
#define UG_THIS_PTR_TOPHDG      (WM_USER+242)
#define UG_THIS_PTR_SIDEHDG     (WM_USER+243)
#define UG_THIS_PTR_CNRBTN      (WM_USER+244)
#define UG_THIS_PTR_TAB         (WM_USER+245)
#define UG_THIS_PTR_VSCROLL     (WM_USER+246)
#define UG_THIS_PTR_HSCROLL     (WM_USER+247)
#define UG_THIS_PTR_EDIT        (WM_USER+248)
#define UG_THIS_PTR_LISTBOX     (WM_USER+249)
#define UG_THIS_PTR_HINT        (WM_USER+250)
