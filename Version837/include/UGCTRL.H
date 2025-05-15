/***********************************************************************
* Ultimate Grid 97                                                     *
* Copyright 1994 - 1997 Dundas Software Ltd.                           *
*                                                                      *
* CUGCtrl                                                              *
*                                                                      *
* Purpose                                                              *
*                                                                      *
* This is the main grid class. It contains and/or controls all         *
* of the other grid clases. CUGCtrl - stands for C++ Ultimate          *
* Grid Control.                                                        *
*                                                                      *
* This is the class that other grids are usually derived from,         *
* since all of the grid's notifications are contained in this          *
* class.                                                               *
*                                                                      *
* For simple and many complex grid applications this is the only       *
* class that needs to be used and worked with directly.                *
*                                                                      *
* Details                                                              *
*                                                                      *
* Generaly a derived class is created from this class and used         *
* in other applications. A derived class is neccessary if any of       *
* the grid's notifications are to be used                              *
*                                                                      *
* Being derived from CWnd allows the grid to be used anywhere a        *
* CWnd can be used, plus all CWnd functions are available as           *
* well.                                                                *
*                                                                      *
* This class automatically creates the grids child components          *
*  (grid, top heading, side heading, scrollbars, tabs, and             *
*  corner button)                                                      *
*                                                                      *
* This class also creates a default memory manager datasource          *
* and registers the grids standard cell types                          *
*                                                                      *
* When an instance of this class (or a class derived from this)        *
* is created, a windows still needs to be created (this is the         *
* same for any CWnd derived class).                                    *
*                                                                      *
* Call the classes CreateGrid to create the window. Or call            *
* AttachGrid to attach the grid to a control in a dialog box.          *
*                                                                      *
* When this class creates or attaches to a window it's OnSetup         *
* member is automatically called. This is a good spot for              *
* initial setup of the grid.                                           *
*                                                                      *
***********************************************************************/
#include <tchar.h>
#include <math.h>
#include <commctrl.h>
#include <time.h>
#include <stdio.h>

#ifndef CUGCTRLINC
#define CUGCTRLINC

#ifdef STRICT
#define FARPROC WNDPROC
#endif

/*
------------------------------------------------------------------
Grid feature enable defines - rem out one or more of these defines
to remove one or more features
------------------------------------------------------------------ */
#define UG_ENABLE_MOUSEWHEEL
#define UG_ENABLE_PRINTING
#define UG_ENABLE_FINDDIALOG
#define UG_ENABLE_HINTS
#define UG_ENABLE_SCROLLHINTS

#define UG_CELLMODE_FOCUS     1
#define UG_CELLMODE_HIGHLIGHT 2

/*
---------------------
Standard grid classes
--------------------- */
class CUGGridInfo;
class CUGGrid;
class CUGTopHdg;
class CUGSideHdg;
class CUGCnrBtn;
class CUGVScroll;
class CUGHScroll;
class CUGCtrl;
class CUGCell;
class CUGCellType;
class CUGEdit;
class CUGCellFormat;

/*
----------------------
Standard grid includes
---------------------- */
#include "UGDefine.h"
#include "UGStruct.h"
#include "UGPtrLst.h"
#include "UGDrwHnt.h"
#include "UGCell.h"
#include "UGCnrBtn.h"
#include "UGGrid.h"
#include "UGHScrol.h"
#include "UGSideHd.h"
#include "UGTopHdg.h"
#include "UGVScrol.h"
#include "UGDtaSrc.h"
#include "UGMemMan.h"
#include "UGCelTyp.h"
#include "UGMultiS.h"
#include "UGEdit.h"
#include "UGTab.h"
#include "UGGdInfo.h"
#include "UGFormat.h"

#ifdef UG_ENABLE_PRINTING
#include "UGPrint.h"
#endif

#ifdef UG_ENABLE_SCROLLHINTS
#include "UGHint.h"
#endif

/*
-------------------
Standard cell types
------------------- */
#include "ugLstBox.h"
#include "ugdltype.h"
#include "ugctarrw.h"

class CUGCtrl
    {

    public:
    CUGCtrl();
    virtual ~CUGCtrl();

    private:

    void clear_default_font();

    /*
    -------------------------------
    Generated message map functions
    ------------------------------- */
    protected:

    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    void OnSize(UINT nType, int cx, int cy);
    void OnPaint();
    void OnSetFocus(HWND hOldWnd);
    void OnSysColorChange();
    void OnHScroll(UINT nSBCode, UINT nPos, HWND hScrollBar);
    void OnVScroll(UINT nSBCode, UINT nPos, HWND hScrollBar);
    UINT OnGetDlgCode();

    protected:

    /*
    ----------------
    Internal classes
    ---------------- */
    TCHAR m_findstring[80];
    TCHAR m_replacestring[80];

    /*
    ----------------
    Data source list
    ---------------- */
    CUGDataSource ** m_dataSrcList;
    int           m_dataSrcListLength;
    int           m_dataSrcListMaxLength;

    CUGPtrList    * m_fontList;
    CUGPtrList    * m_bitmapList;
    CUGPtrList    * m_cellTypeList;
    CUGPtrList    * m_cellStylesList;
    CUGPtrList    * m_validateList;
    CUGPtrList    * m_displayFormatList;

    /*
    -------------------
    Standard cell types
    ------------------- */
    CUGCellType     m_normalCellType;
    CUGDropListType m_dropListType;
    CUGArrowType    m_arrowType;


    #ifdef UG_ENABLE_PRINTING
    CUGPrint * m_CUGPrint;
    #endif

    /*
    ----------
    Popup menu
    ---------- */
    HMENU               m_menu;
    int                 m_menuCol;
    long                m_menuRow;
    int                 m_menuSection;

    CUGCell m_cell;

    /*
    --------------------------------
    Grid info list / sheet variables
    -------------------------------- */
    CUGGridInfo ** m_GIList;
    int m_currentSheet;
    int m_numberSheets;

    /*
    --------------------------
    Update enable/disable flag
    -------------------------- */
    BOOL m_enableUpdate;

    private:
    friend void CopyString(LPTSTR & dest, LPCTSTR src);
    friend void CopyString(LPTSTR & dest, LPCTSTR src, int len);
    friend void AddTwoString(LPTSTR & dest, LPCTSTR src);

    public:

    /*
    -------------
    Current sheet
    ------------- */
    CUGGridInfo * m_GI;

    /*
    --------------------
    Child window classes
    -------------------- */
    CUGGrid    * m_CUGGrid;
    CUGTopHdg  * m_CUGTopHdg;
    CUGSideHdg * m_CUGSideHdg;
    CUGCnrBtn  * m_CUGCnrBtn;
    CUGVScroll * m_CUGVScroll;
    CUGHScroll * m_CUGHScroll;

    /*
    ----
    Tabs
    ---- */
    CUGTab * m_CUGTab;

    /*
    ------------------
    Scroll hint window
    ------------------ */
    #ifdef UG_ENABLE_SCROLLHINTS
    CUGHint             *m_CUGHint;
    #endif

    /*
    ---------------
    Tab sizing flag
    --------------- */
    BOOL m_tabSizing;

    /*
    --------------
    Control window
    -------------- */
    HWND    m_ctrlWnd;

    /*
    -----------------------
    Tracking topmost window
    ----------------------- */
    HWND    m_trackingWnd;

    /*
    --------------------
    Default edit control
    -------------------- */
    CUGEdit m_defEditCtrl;

    /*
    -------
    Editing
    ------- */
    HWND    m_editCtrl;

    int     m_editInProgress;       //TRUE or FALSE
    long    m_editRow;
    int     m_editCol;
    CUGCell m_editCell;
    HWND    m_editParent;

    /*
    ---------------
    Other variables
    --------------- */
    HPEN m_threeDLightPen;
    HPEN m_threeDDarkPen;

    protected:

    /*
    ------------------
    Internal functions
    ------------------ */
    void CalcTopRow();          //Calcs the max top row then adjusts the top row if needed
    void CalcLeftCol();         //Calcs the max left col then adjusts the left col if needed
    void AdjustTopRow();        //moves top row so that the current row is in view
    void AdjustLeftCol();       //move the left col so that the current col is in view
    void Update();              //updates all windows - also performs recalculations
    void Moved();               //this function is called whenever a grid movement is made
                                //this function then notifies the child windows

    BOOL CreateChildWindows();

    void SetLockRowHeight();
    void SetLockColWidth();

    void MoveTrackingWindow();

    public:

    /*
    -----------------------------------------
    Check to see if the new position is valid
    ----------------------------------------- */
    int VerifyTopRow(long* newRow);
    int VerifyCurrentRow(long* newRow);
    int VerifyLeftCol(int* newCol);
    int VerifyCurrentCol(int* newCol);

    /*
    ------------------
    Creation and setup
    ------------------ */
    BOOL CreateGrid( DWORD dwStyle, const RECT& rect, HWND hWndParent, UINT nID );

    /*
    -------------------------
    Dialog resource functions
    ------------------------- */
    BOOL AttachGrid(HWND wnd,UINT ID);

    void AdjustComponentSizes();    //adjusts and positions the child windows

    //*************** editing *****************
    int     StartEdit();
    int     StartEdit(int key);
    int     StartEdit(int col,long row,int key);
    int     ContinueEdit(int adjustCol,long adjustRow);

    //*************** row and column *****************
    int     SetNumberRows( long rows, BOOL redraw = FALSE );
    long    GetNumberRows();

    int     SetNumberCols( int cols, BOOL redraw = FALSE );
    int     GetNumberCols();

    int     SetDefColWidth( int width );
    int     SetColWidth( int col, int width );
    int     GetColWidth( int col, int * width );
    int     GetColWidth( int col );

    int     SetDefRowHeight( int height );
    int     SetUniformRowHeight( int flag );
    int     SetRowHeight( long row, int height );
    int     GetRowHeight( long row, int * height );
    int     GetRowHeight( long row);

    int     GetCurrentCol();
    long    GetCurrentRow();
    int     GetLeftCol();
    long    GetTopRow();
    int     GetRightCol();
    long    GetBottomRow();

    int     InsertCol(int col);
    int     AppendCol();
    int     DeleteCol(int col);
    int     InsertRow(long row);
    int     AppendRow();
    int     DeleteRow(long row);

    //************ heading functions *************
    int     SetTH_NumberRows(int rows);
    int     SetTH_RowHeight(int row,int height);
    int     SetSH_NumberCols(int cols);
    int     SetSH_ColWidth(int col,int width);


    //************* find/replace/sort *********************
    int FindFirst(LPTSTR string,int *col,long *row,long flags);
    int FindNext(LPTSTR string,int *col,long *row,int flags);

    int FindDialog();
    int ReplaceDialog();
    long ProcessFindDialog(UINT, long);
    int FindInAllCols(BOOL state);
    BOOL m_findDialogRunning;
    BOOL m_findDialogStarted;
    BOOL m_findInAllCols;
    FINDREPLACE m_findReplaceDialog;
    //HWND              m_FindReplaceWnd;
    UINT                m_FindReplaceMsg;

    int SortBy(int col,int flag = UG_SORT_ASCENDING);
    int SortBy(int *cols,int num,int flag = UG_SORT_ASCENDING);


    //************* child windows *********************
    int     SetTH_Height(int height);
    int     GetTH_Height();
    int     SetSH_Width(int width);
    int     GetSH_Width();
    int     SetVS_Width(int width);
    int     GetVS_Width();
    int     SetHS_Height(int height);
    int     GetHS_Height();

    //************* default/cell information *********************
    int     GetCell(int col,long row,CUGCell *cell);
    int     GetCellIndirect(int col,long row,CUGCell *cell);
    int     SetCell(int col,long row,CUGCell *cell);
    int     DeleteCell(int col,long row);

    int     SetColDefault(int col,CUGCell *cell);
    int     GetColDefault(int col,CUGCell *cell);
    int     SetGridDefault(CUGCell *cell);
    int     GetGridDefault(CUGCell *cell);
    int     SetHeadingDefault(CUGCell *cell);
    int     GetHeadingDefault(CUGCell *cell);

    //column information and translation
    int     GetColTranslation(int col);
    int     SetColTranslation(int col,int transCol);


    //cell joining
    int     JoinCells(int startcol,long startrow,int endcol,long endrow);
    int     UnJoinCells(int col,long row);
    int     EnableJoins(BOOL state);

    int     QuickSetText(int col,long row,LPCTSTR string);
    int     QuickSetNumber(int col,long row,double number);
    int     QuickSetNumber(int col,long row,long number);
    int     QuickSetNumber(int col,long row,int number);
    int     QuickSetMask(int col,long row,LPCTSTR string);
    int     QuickSetLabelText(int col,long row,LPCTSTR string);
    int     QuickSetTextColor(int col,long row,COLORREF color);
    int     QuickSetBackColor(int col,long row,COLORREF color);
    int     QuickSetAlignment(int col,long row,int align);
    int     QuickSetBorder(int col,long row,int border);
    int     QuickSetBorderColor(int col,long row, HPEN pen);
    int     QuickSetFont(int col,long row, HFONT font);
    int     QuickSetFont(int col,long row,int index);
    int     QuickSetBitmap(int col,long row, HBITMAP bitmap);
    int     QuickSetBitmap(int col,long row,int index);
    int     QuickSetCellType(int col,long row,long type);
    int     QuickSetCellTypeEx(int col,long row,long typeEx);
    int     QuickSetHTextColor(int col,long row,COLORREF color);
    int     QuickSetHBackColor(int col,long row,COLORREF color);

    int     QuickSetRange(int startCol,long startRow,int endCol,long endRow,
                CUGCell* cell);

    int         QuickGetText(int col,long row,LPTSTR *string,int len);
    LPCTSTR     QuickGetText(int col,long row);


    //************* general modes and settings *********************
    int SetCurrentCellMode(int mode);   //focus and highlighting
                                            //bit 1:focus 2:hightlight
    int SetHighlightRow(int mode);      //on,off
    int SetMultiSelectMode(int mode);   //on,off
    int Set3DHeight(int height);        //in pixels
    int SetPaintMode(int mode);         //on,off
    int GetPaintMode();         //on,off
    int SetVScrollMode(int mode);       // 0-normal 2- tracking 3-joystick
    int SetHScrollMode(int mode);       // 0-normal 2- tracking

    int HideCurrentCell();

    int SetDoubleBufferMode(BOOL mode);

    int LockColumns(int numCols);
    int LockRows(int numRows);

    int EnableCellOverLap( BOOL state );
    int EnableColSwapping( BOOL state );


    //************* movement *********************
    int MoveTopRow(int flag);   //0-lineup 1-linedown 2-pageup 3-pagedown 4-top 5-bottom
    int AdjustTopRow(long adjust);
    int MoveCurrentRow(int flag);
    int AdjustCurrentRow(long adjust);
    int GotoRow(long row);
    int SetTopRow(long row);

    int MoveLeftCol(int flag);
    int AdjustLeftCol(int adjust);
    int MoveCurrentCol(int flag);
    int AdjustCurrentCol(int adjust);
    int GotoCol(int col);
    int SetLeftCol(int col);

    int GotoCell(int col,long row);


    //************* finding cells *********************
    int GetCellFromPoint(int x,int y,int *col,long *row);
    int GetCellFromPoint(int x,int y,int *ptcol,long *ptrow,RECT *rect);
    int GetAbsoluteCellFromPoint(int x,int y,int *ptcol,long *ptrow);

    int GetCellRect(int col,long row,RECT *rect);
    int GetCellRect(int *col,long *row,RECT *rect);
    int GetRangeRect(int startCol,long startRow,int endCol,long endRow,RECT *rect);

    int GetJoinStartCell(int *col,long *row);
    int GetJoinStartCell(int *col,long *row,CUGCell *cell);
    int GetJoinRange(int *col,long *row,int *col2,long *row2);


    //************* cell types *********************
    long AddCellType(CUGCellType *);   //returns the cell type ID
    int RemoveCellType(int ID);
    CUGCellType * GetCellType(int type);    //returns the pointer to a cell type
    int GetCellType(CUGCellType * type);

    //************* data sources *********************
    int AddDataSource(CUGDataSource * ds);
    CUGDataSource * GetDataSource(int index);
    int RemoveDataSource(int index);
    int SetDefDataSource(int index);
    int GetDefDataSource();
    int SetGridUsingDataSource(int index);

    //************* fonts *********************
    int AddFont(LPCTSTR fontName,int height,int weight);
    int AddFont(int height,int width,int escapement,int orientation,
            int weight,BYTE italic,BYTE underline,BYTE strikeOut,
            BYTE charSet,BYTE outputPrecision,BYTE clipPrecision,
            BYTE quality,BYTE pitchAndFamily,LPCTSTR fontName);
    int RemoveFont(int index);
    int ClearAllFonts();
    HFONT GetFont(int index);
    int SetDefFont(HFONT font);
    int SetDefFont(int index);

    //************* bitmaps *********************
    int AddBitmap( UINT resourceID,LPCTSTR resourceName= NULL);
    int AddBitmap( LPCTSTR fileName);
    int RemoveBitmap(int index);
    int ClearAllBitmaps();
    HBITMAP GetBitmap(int index);


    //************* redrawing *********************
    int RedrawAll();
    int RedrawCell(int col,long row);
    int RedrawRow(long row);
    int RedrawCol(int col);
    int RedrawRange(int startCol,long startRow,int endCol,long endRow);


    //************* multi-select *********************
    int ClearSelections();
    int Select(int col,long row);
    int SelectRange(int startCol,long startRow,int endCol,long endRow);
    int EnumFirstSelected(int *col,long *row);
    int EnumNextSelected(int *col,long *row);
    int EnumFirstBlock(int *startCol,long *startRow,int *endCol,long *endRow);
    int EnumNextBlock(int *startCol,long *startRow,int *endCol,long *endRow);
    BOOL IsSelected(int col,long row,int *blockNum = NULL);

    //************* clipboard ********************
    int CopySelected();
    int CutSelected();
    int CopySelected(int cutflag);  //TRUE,FALSE
    int Paste();
    int Paste(LPTSTR &string);
    int CopyToClipBoard(LPTSTR string);
    int CopyFromClipBoard(LPTSTR *string);
    void CreateSelectedString(LPTSTR &string,int cutFlag);


    //************* column sizing ********************
    int FitToWindow(int startCol,int endCol);
    int BestFit(int startCol,int endCol,int CalcRange,int flag);


    //************* printing ********************
    #ifdef UG_ENABLE_PRINTING
        int PrintInit(HDC hDC, PRINTDLG pPD, int startCol,long startRow,
            int endCol,long endRow);
        int PrintPage(HDC hDC, int pageNum);
        int PrintSetMargin(int whichMargin,int size);
        int PrintSetScale(double scale);
        int PrintSetOption(int option,long param);
        int PrintGetOption(int option,long *param);
    #endif


    //************* hints ********************
    #ifdef UG_ENABLE_HINTS
    int UseHints(BOOL state);
    #endif

    #ifdef UG_ENABLE_SCROLLHINTS
    int UseVScrollHints(BOOL state);
    int UseHScrollHints(BOOL state);
    #endif

    //************* pop-up menu ********************
    HMENU GetPopupMenu();
    int EmptyMenu();
    int AddMenuItem(int ID,LPCTSTR string);
    int RemoveMenuItem(int ID);
    int EnableMenu(BOOL state);


    //************* tabs ********************
    int AddTab(LPCTSTR label,long ID);
    int DeleteTab(long ID);
    int SetTabBackColor(long ID,COLORREF color);
    int SetTabTextColor(long ID,COLORREF color);
    int SetTabWidth(int width);
    int SetCurrentTab(long ID);
    int GetCurrentTab();


    //************* sheets ********************
    int SetNumberSheets(int numSheets);
    int GetNumberSheets();
    int SetSheetNumber(int index,BOOL update = TRUE);
    int GetSheetNumber();

    //****************************************************
    //*********** Over-ridable Notify Functions **********
    //****************************************************
    virtual void OnSetup();
    virtual void OnSheetSetup(int sheetNumber);

    //movement and sizing
    virtual int  OnCanMove(int oldcol,long oldrow,int & newcol,long & newrow);
    virtual int  OnCanViewMove(int oldcol,long oldrow,int newcol,long newrow);
    virtual void OnHitBottom(long numrows,long rowspast,long rowsfound);
    virtual void OnHitTop(long numrows,long rowspast);

    virtual int  OnCanSizeCol(int col);
    virtual void OnColSizing(int col,int *width);
    virtual void OnColSized(int col,int *width);
    virtual int  OnCanSizeRow(long row);
    virtual void OnRowSizing(long row,int *height);
    virtual void OnRowSized(long row,int *height);

    virtual int  OnCanSizeTopHdg();
    virtual int  OnCanSizeSideHdg();
    virtual int  OnTopHdgSizing(int *height);
    virtual int  OnSideHdgSizing(int *width);
    virtual int  OnTopHdgSized(int *height);
    virtual int  OnSideHdgSized(int *width);

    virtual void OnColChange(int oldcol,int newcol);
    virtual void OnRowChange(long oldrow,long newrow);
    virtual void OnCellChange(int oldcol,int newcol,long oldrow,long newrow);
    virtual void OnLeftColChange(int oldcol,int newcol);
    virtual void OnTopRowChange(long oldrow,long newrow);

    //mouse and key strokes
    virtual void OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);
    virtual void OnRClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);
    virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
    virtual void OnMouseMove(int col,long row,POINT *point,UINT nFlags,BOOL processed=0);
    virtual void OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);
    virtual void OnTH_RClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);
    virtual void OnTH_DClicked(int col,long row,RECT *rect,POINT *point,BOOL processed=0);
    virtual void OnSH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);
    virtual void OnSH_RClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);
    virtual void OnSH_DClicked(int col,long row,RECT *rect,POINT *point,BOOL processed=0);
    virtual void OnCB_LClicked(int updn,RECT *rect,POINT *point,BOOL processed=0);
    virtual void OnCB_RClicked(int updn,RECT *rect,POINT *point,BOOL processed=0);
    virtual void OnCB_DClicked(RECT *rect,POINT *point,BOOL processed=0);

    virtual void OnKeyDown(UINT *vcKey,BOOL processed);
    virtual void OnCharDown(UINT *vcKey,BOOL processed);

    //GetCellIndirect notification
    virtual void OnGetCell(int col,long row,CUGCell *cell);

    //SetCell notification
    virtual void OnSetCell(int col,long row,CUGCell *cell);

    //cell type notifications
    virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

    //editing
    virtual int OnEditStart(int col, long row, HWND *edit);
    virtual int OnEditVerify(int col,long row, HWND edit,UINT *vcKey);
    virtual int OnEditFinish(int col, long row, HWND edit,LPCTSTR string,BOOL cancelFlag);
    virtual int OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow);

    //menu notifications
    virtual void OnMenuCommand(int col,long row,int section,int item);
    virtual int  OnMenuStart(int col,long row,int section);

    //hints
    #ifdef UG_ENABLE_HINTS
    virtual int OnHint(int col,long row,int section,LPTSTR string);
    #endif

    #ifdef UG_ENABLE_SCROLLHINTS
    virtual int OnVScrollHint(long row,LPTSTR string);
    virtual int OnHScrollHint(int col,LPTSTR string);
    #endif

    //sorting
    virtual int  OnSortEvaluate(CUGCell *cell1,CUGCell *cell2,int flags);

    //DC setup
    virtual void OnScreenDCSetup(HDC dc, HDC db_dc,int section);

    virtual void OnAdjustComponentSizes(RECT *grid,RECT *topHdg,RECT *sideHdg,
        RECT *cnrBtn,RECT *vScroll,RECT *hScroll,RECT *tabs);

    virtual void OnTabSelected(int ID);

    virtual COLORREF OnGetDefBackColor(int section);

    //focus rect setup
    virtual void OnDrawFocusRect(HDC dc,RECT *rect);

    virtual void OnSetFocus(int section);
    virtual void OnKillFocus(int section);

    int StartMenu(int col,long row,POINT *point,int section);

    int SetArrowCursor(HCURSOR cursor);
    int SetWESizingCursor(HCURSOR cursor);
    int SetNSSizingCursor(HCURSOR cursor);

    int SetMargin(int pixels);

    /*
    --------------------------------------------
    Functions to be called by grid edit controls
    -------------------------------------------- */
    int EditCtrlFinished(LPCTSTR string,BOOL cancelFlag, BOOL continueFlag,int continueCol,long continueRow);
    int EditCancel();

    CUGCellType * GetCellType(int col,long row);

    virtual BOOL OnColSwapStart(int col);
    virtual BOOL OnCanColSwap(int fromCol,int toCol);

    int MoveColPosition(int fromCol,int toCol,BOOL insertBefore);

    int SetNewTopHeadingClass(CUGTopHdg * topHeading);
    int SetNewSideHeadingClass(CUGSideHdg * sideHeading);
    int SetNewGridClass(CUGGrid * grid);
    int SetNewMultiSelectClass(CUGMultiSelect * multiSelect);
    int SetNewTabClass(CUGTab * tab);
    int SetNewVScrollClass(CUGVScroll * scroll);
    int SetNewHScrollClass(CUGHScroll * scroll);


    int SetNewEditClass(HWND edit);
    HWND GetEditClass();


    /*
    ---------------
    Overlay objects
    --------------- */
    int AddOverlayObject(HWND wnd,int startCol,int startRow,int endCol,int endRow);
    int SetOverlayObjectPos(int index, int startCol,int startRow,int endCol,int endRow, int leftOffset=0,int topOffset=0,int rightOffset=0,int bottomOffset=0);
    int GetOverlayObjectPos(int index, int * startCol, int * startRow, int * endCol,int* endRow, int * leftOffset,int* topOffset,int* rightOffset,int* bottomOffset);
    int GetOverlayObjectPos(int index, int * startCol, int * startRow, int * endCol,int* endRow);
    int RemoveOverlayObject(int index);
    HWND GetOverlayObject(int index);

    /*
    ---------------
    Tracking window
    --------------- */
    int SetTrackingWindow(HWND hWnd);
    int SetTrackingWindowMode(int mode);
    virtual void OnTrackingWindowMoved(RECT *origRect,RECT *newRect);

    int EnableUpdate(BOOL state);

    int SetUserSizingMode(BOOL state);

    int SetColDataSource(int col, int dataSrcIndex);
    int SetColDataSource(int col, CUGDataSource * dataSrc);

    BOOL CreateCtrlWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID);
    static long CALLBACK CtrlWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

    };

#endif
