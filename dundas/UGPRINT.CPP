#include <windows.h>

/***********************************************
	Ultimate Grid 97
	Copyright 1994 - 1997 Dundas Software Ltd.


	class
		CUGPrint
	Purpose
		This class handles the setup and printing
		of the grid to a printer or other printing
		device.
	Details
		-This class has two printing sections,
		 setup, and printing. Setup is required
		 to figure out how many pages it will take
		 to print a range of cells from the grid.
		 Plus which cells to print on which pages.
		 Once the setup is complete a page can
		 now be printed.
		-Printing setup function: PrintInit
		-Page Printing function: PrintPage
		-The rest of the printing functons are optional
		 and are used to setup different print options.	
************************************************/

#include "..\include\UGCtrl.h"

#ifdef UG_ENABLE_PRINTING

/***************************************************
****************************************************/
CUGPrint::CUGPrint(){

	m_pagesWide = 0;
	m_pagesHigh = 0;
	m_printList = NULL;

	m_printLeftMargin	= 25;	//margin in mm
	m_printTopMargin	= 25;	
	m_printRightMargin	= 25;
	m_printBottomMargin = 25;

	m_printFrame = TRUE;
	m_printCenter = FALSE;
	m_fitToPage = FALSE;
	m_printTopHeading = FALSE;
	m_printSideHeading = FALSE;

}

/***************************************************
****************************************************/
CUGPrint::~CUGPrint(){
	ClearPrintList();
}

/***************************************************
****************************************************/
int CUGPrint::ClearPrintList(){

	CUGPrintList* next;
	
	//delete all the items in the linked list
	while(m_printList != NULL){
		next = m_printList->next;
		delete m_printList;
		m_printList = next;
	}

	return TRUE;
}
/***************************************************
****************************************************/
int	CUGPrint::AddPageToList(int page,int startCol,int endCol,long startRow,long endRow){

	CUGPrintList * item;	//new print list item

	//if the list is already empty then just return
	if(m_printList == NULL){
		m_printList = new CUGPrintList;
		item = m_printList;
		item->next = NULL;
	}
	else{
		//find the last item in the list
		CUGPrintList* current = m_printList;
		while(current != NULL){
			if(current->page == page){
				item = current;
				break;
			}
			if(current->next==NULL){
				item = new CUGPrintList;
				current->next = item;
				item->next = NULL;
				break;
			}
			current = current->next;
		}
	}

	item->page		= page;
	item->startCol	= startCol;
	item->endCol	= endCol;
	item->startRow	= startRow;
	item->endRow	= endRow;

	return TRUE;
}
/***************************************************
****************************************************/
int	CUGPrint::GetPageFromList(int page,int *startCol,int *endCol,long *startRow,long *endRow){

	//find the page
	CUGPrintList* next = m_printList;
	while(next != NULL){
		if(next->page ==page)
			break;
		next = next->next;
	}

	//if the page was not found then return FALSE
	if(next == NULL)
		return FALSE;
	
	//return the page information
	*startCol	= next->startCol;
	*endCol		= next->endCol;
	*startRow	= next->startRow;
	*endRow		= next->endRow;
	
	return TRUE;
}
/***************************************************
****************************************************/
int CUGPrint::PrintInit(HDC pDC, PRINTDLG pPD, int startCol,long startRow,
						int endCol,long endRow){

	//get a screen device context
	HDC	sDC;
	sDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	//get the metrics for the screen
/*	float ScreenWidthPX		= (float)GetDeviceCaps(sDC, HORZRES);
	float ScreenWidthMM		= (float)GetDeviceCaps(sDC, HORZSIZE);
	float ScreenHeightPX	= (float)GetDeviceCaps(sDC, VERTRES);
	float ScreenHeightMM	= (float)GetDeviceCaps(sDC, VERTSIZE);*/
	DeleteObject(sDC);

	//get the metrics for the printer
	float paperWidthPX		= (float)GetDeviceCaps(pDC, HORZRES);
	float paperHeightPX		= (float)GetDeviceCaps(pDC, VERTRES);
	float paperWidthMM		= (float)GetDeviceCaps(pDC, HORZSIZE);
	float paperHeightMM		= (float)GetDeviceCaps(pDC, VERTSIZE);

	int virtualWidthPX = (int)( 3.79 * (paperWidthMM - m_printLeftMargin - m_printRightMargin));
	int virtualHeightPX = (int)( 3.78 * (paperHeightMM - m_printTopMargin - m_printBottomMargin));


	//check the print styles
	if(m_printTopHeading)
		virtualWidthPX -= m_GI->m_sideHdgWidth;
	if(m_printSideHeading)
		virtualHeightPX -= m_GI->m_topHdgHeight;


	//save the scaling values
	m_printVScale = ( (paperHeightPX/paperHeightMM) *
					(paperHeightMM - m_printTopMargin - m_printBottomMargin) ) /
					virtualHeightPX;
	m_printHScale = ( (paperWidthPX/paperWidthMM) *
					(paperWidthMM - m_printLeftMargin - m_printRightMargin) ) /
					virtualWidthPX;

	//save the margins
	m_printLeftMarginPX = (int)((paperWidthPX/paperWidthMM) * m_printLeftMargin);
	m_printTopMarginPX = (int)((paperHeightPX/paperHeightMM) * m_printTopMargin);
	m_printRightMarginPX = (int)(paperWidthPX - (paperWidthPX/paperWidthMM) * m_printRightMargin);
	m_printBottomMarginPX = (int)(paperHeightPX - (paperHeightPX/paperHeightMM) * m_printBottomMargin);
	m_paperWidthPX = (int)paperWidthPX;
	m_paperHeightPX = (int)paperHeightPX;


	//get the number of pages wide
	int col,width=0,w;
	int scol = startCol;		//start col for a page
	m_pagesWide = 0;
	
	//find the number of pages wide and the number of columns
	//on each page
	for(col =startCol;col <= endCol;col++){
		m_ctrl->GetColWidth(col,&w);		
		width += w;
	
		if(width > virtualWidthPX){
			m_pagesWide++;
			width = 0;
			if(w < virtualWidthPX){
				col--;
			}
			//add the column info to the list
			AddPageToList(m_pagesWide,scol,col,0,0);
			scol = col+1;
		}
	}
	if(width >0){
		m_pagesWide++;
		//add the column info to the list
		if(col > endCol)
			col = endCol;
		AddPageToList(m_pagesWide,scol,col,0,0);
	}
		

	//get the number of pages high
	long row;
	long srow = startRow;		//start row for page
	int height=0,h;
	m_pagesHigh = 0;
	
	//find the number of pages high and the number of rows
	//on each page
	for(row = startRow;row <=endRow;row++){
		m_ctrl->GetRowHeight(row,&h);
		height += h;
		if(height > virtualHeightPX){
			m_pagesHigh ++;
			height =0;
			if(h < virtualHeightPX)
				row--;
			
			//add the page information to the list
			int c1,c2;
			long r1,r2;
			int page;
			for(col =1;col<=m_pagesWide;col++){
				//get the col info
				GetPageFromList(col,&c1,&c2,&r1,&r2);
				//update the page info
				page = (m_pagesHigh-1)*m_pagesWide +col;
				AddPageToList(page,c1,c2,srow,row);
			}
			srow = row+1;
		}
	}
	if(height >0){
		m_pagesHigh++;
		//add the page information to the list
		int c1,c2;
		long r1,r2;
		int page;
		for(col =1;col<=m_pagesWide;col++){
			//get the col info
			GetPageFromList(col,&c1,&c2,&r1,&r2);
			//update the page info
			page = (m_pagesHigh-1)*m_pagesWide +col;
			if(row > endRow)
				row = endRow;
			AddPageToList(page,c1,c2,srow,row);
		}
	}

	//return the number of pages in total
	return (m_pagesWide * m_pagesHigh);
}
/***************************************************
****************************************************/
int CUGPrint::PrintPage(HDC pDC, int pageNum){

	int  x,h,w,col,xx;
	long y,row,yy;
	int startCol,endCol;
	long startRow,endRow;
	RECT cellrect={0,0,0,0};
	RECT origCellRect;
	int leftMargin = m_printLeftMarginPX;
	int topMargin =  m_printTopMarginPX;

	//get the co-ords to print
	if(GetPageFromList(pageNum,&startCol,&endCol,&startRow,&endRow)!=TRUE)
		return FALSE;

	//*********************
	// draw the page
	//*********************

	//draw the grid
	cellrect.top = topMargin;
	cellrect.bottom = topMargin;

	//main grid row drawing loop
	yy = startRow;
	if(m_printTopHeading)
		yy =-1;
	for(y = yy; y <= endRow ; y++){
		
		//set up the rectangle for the row
		if(y == -1){
			//set up the rect for the top heading
			cellrect.bottom += (int)(m_GI->m_topHdgHeight * m_printVScale);
			cellrect.left	= leftMargin;
			cellrect.right  = cellrect.left;
		}
		else{
			if(yy==-1){
				yy=0;
				y = startRow;
			}
			//set up the rect for the row to be drawn
			cellrect.top	= cellrect.bottom;
			m_ctrl->GetRowHeight(y,&h);		
			cellrect.bottom += (int)(h * m_printVScale);
			cellrect.left	= leftMargin;
			cellrect.right  = cellrect.left;
		}	

		//dont draw the row if it is 0 pixels high		
		if(cellrect.top == cellrect.bottom)
			continue;

		//main column drawing loop
		xx =startCol;
		if(m_printSideHeading)
			xx = -1;
		for(x = xx; x <= endCol; x++){
			
			//finish setting up the rect for the cel
			if( x == -1 ){
				//get the side heading co-ords
				cellrect.right += (int)(m_GI->m_sideHdgWidth * m_printHScale);
			}
			else{
				if(xx==-1){
					xx=0;
					x = startCol;
				}
				//get the left,right co-ords
				cellrect.left = cellrect.right;
				m_ctrl->GetColWidth(x,&w);		
				cellrect.right += (int)(w * m_printHScale);
			}
		
			//dont draw the column is it is 0 pixels wide
			if(cellrect.left == cellrect.right)
				continue;

			//store the original cell rect, incase it is modified below
			CopyRect(&origCellRect,&cellrect);
	
			col = x;
			row = y;

			//check to see if the cell is a joined cell
			if(m_ctrl->GetJoinStartCell(&col,&row)==UG_SUCCESS){
				if(col == x && row == y){
					RECT tempRect;
					int col2;
					long row2;
					CopyRect(&tempRect,&cellrect);
					m_ctrl->GetJoinRange(&col,&row,&col2,&row2);
					while(col2 > col){
						m_ctrl->GetColWidth(col2,&w);		
						tempRect.right+=(int)(w* m_printHScale);
						col2--;
					}
					while(row2 > row){
						m_ctrl->GetRowHeight(row,&h);		
						tempRect.bottom+=(int)(h* m_printVScale);
						row2--;
					}
					InternPrintCell(pDC,&tempRect,col,row);
				}
			}
			else{
				InternPrintCell(pDC,&cellrect,x,y);
			}
			
			//draw a section of the print fram if selected
			CopyRect(&cellrect,&origCellRect);
			if(m_printFrame){
				if(y == startRow){
					MoveToEx(pDC, cellrect.left,cellrect.top, NULL);
					LineTo(pDC, cellrect.right+1,cellrect.top);
				}
				else if(y == endRow){
					MoveToEx(pDC, cellrect.left,cellrect.bottom, NULL);
					LineTo(pDC, cellrect.right+1,cellrect.bottom);
				}
				if(x == startCol){
					MoveToEx(pDC, cellrect.left,cellrect.top, NULL);
					LineTo(pDC, cellrect.left,cellrect.bottom+1);
				}
				else if(x == endCol){
					MoveToEx(pDC, cellrect.right,cellrect.top, NULL);
					LineTo(pDC, cellrect.right,cellrect.bottom+1);
				}
			}
		}
	}
	return UG_SUCCESS;
}
/***************************************************
****************************************************/
int CUGPrint::PrintSetMargin(int whichMargin,int size){
	return UG_SUCCESS;
}
/***************************************************
****************************************************/
int CUGPrint::PrintSetScale(double scale){
	return UG_SUCCESS;
}
/***************************************************
****************************************************/
int CUGPrint::PrintSetOption(int option,long param){

	if(option == UG_PRINT_LEFTMARGIN)
		m_printLeftMargin = (int)param;
	if(option == UG_PRINT_RIGHTMARGIN)
		m_printRightMargin =  (int)param;
	if(option == UG_PRINT_TOPMARGIN)
		m_printTopMargin =  (int)param;
	if(option == UG_PRINT_BOTTOMMARGIN)
		m_printBottomMargin =  (int)param;
	if(option == UG_PRINT_FRAME){
		if(param == FALSE)
			m_printFrame =  FALSE;
		else
			m_printFrame =  TRUE;
	}
	if(option == UG_PRINT_TOPHEADING){
		m_printTopHeading = param;
	}
	if(option == UG_PRINT_SIDEHEADING){
		m_printSideHeading = param;
	}
	
	return UG_SUCCESS;
}
/***************************************************
****************************************************/
int CUGPrint::PrintGetOption(int option,long *param){
	

	if(option == UG_PRINT_LEFTMARGIN)
		return m_printLeftMargin;

	else if(option == UG_PRINT_RIGHTMARGIN)
		return m_printRightMargin;
	
	else if(option == UG_PRINT_TOPMARGIN)
		return m_printTopMargin;
	
	else if(option == UG_PRINT_BOTTOMMARGIN)
		return m_printBottomMargin;
	
	else if(option == UG_PRINT_FRAME)
		return m_printFrame;
	
	else
		return UG_ERROR;
	
	
//	return UG_SUCCESS;
}

/***************************************************
****************************************************/
void CUGPrint::InternPrintCell(HDC dc,RECT * rect,int col,long row){

	CUGCellType * cellType;
	int width,height;
	CUGCell cell,cell2;

	//get the cells original height and width
	if(col >=0)
		m_ctrl->GetColWidth(col,&width);
	else
		width = m_ctrl->GetSH_Width();
	if(row >=0)
		m_ctrl->GetRowHeight(row,&height);
	else
		height = m_ctrl->GetTH_Height();

	//get the cell information
	m_ctrl->GetCellIndirect(col,row,&cell);

	//get the cell type to draw the cell
	if(cell.IsPropertySet(UGCELL_CELLTYPE_SET)){
		cellType = m_ctrl->GetCellType(cell.GetCellType());
	}
	else{
		cellType = m_ctrl->GetCellType(-1);
	}

	//paint the cell
	int saveID = SaveDC(dc);
	cellType->OnPrint(dc,rect,col,row,&cell);
	RestoreDC(dc, saveID);
}

#endif


