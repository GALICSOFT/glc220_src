
#define _WIN32_WINNT			0x0400

#include <windows.h>

#include "DataGrid.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)		{ if(p){ delete (p); (p) = NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p){ if(p){ delete [] (p); (p) = NULL; } }
#endif


#ifndef SAFE_FREE
#define SAFE_FREE(p)		{ if(p){ free (p); (p) = NULL; } }
#endif

class CDataGrid : public IDataGrid
{
public:
	// Custom callback procedure
	typedef INT(CALLBACK *DGCOMPARE)(char* item1, char* item2, INT column);


	// DataGrid column definition structure
	struct _Tcol
	{
		char		columnText[0x400];
		INT			columnWidth;
		INT			textAlign;
		BOOL		pressed;

	};


	// DataGrid row definition structure
	struct _Trow
	{
		char**		rowText;
		INT*		textAlign;
		BOOL		selected;
		BOOL*		readOnly;
		COLORREF	bgColor;

		_Trow();
	};

	// DataGrid item information structure
	struct _Titem
	{
		INT			dgMask;
		INT			dgItem;
		INT			dgSubitem;
		char*		dgText;
		INT			dgTextLen;
		INT			dgTextAlign;
		BOOL		dgSelected;
		BOOL		dgReadOnly;
		COLORREF	dgBgColor;
	};


	// DataGrid definition
	struct _Tlist
	{
		HWND		dg_hWnd;
		HWND		dg_hParent;
		_Tcol*		dg_Columns;
		_Trow*		dg_Rows;
		INT			dg_ColumnNumber;
		INT			dg_RowNumber;
		INT			dg_Column;
		INT			dg_Row;
		INT			dg_SelectedColumn;
		INT			dg_SelectedRow;
		_Trow*		dg_Selected;
		BOOL		dg_Cursor;
		BOOL		dg_Resize;
		BOOL		dg_Click;
		HFONT		dg_hColumnFont;
		HFONT		dg_hRowFont;
		LOGFONT		dg_LFColumnFont;
		LOGFONT 	dg_LFRowFont;
		HBRUSH		dg_hBgBrush;
		HPEN		dg_hCellPen;
		HBITMAP		dg_hMemBitmap;
		HDC			dg_hMemDC;
		HBITMAP		dg_hOldMemBitmap;
		HWND		dg_hEditWnd;
		RECT		dg_EditRect;
		BOOL		dg_Edit;
		BOOL		dg_EnableEdit;
		BOOL		dg_EnableSort;
		BOOL		dg_EnableResize;
		BOOL		dg_EnableGrid;
		DGCOMPARE	dg_CompareFunc;
		COLORREF	dg_ColumnTextColor;
		COLORREF	dg_RowTextColor;

		_Tlist*	next;

		_Tlist();
	};

private:
	// DataGrid members
	HWND	m_hWnd;
	HWND	m_hParentWnd;
	RECT	m_DGRect;

public:
	// Basic DataGrid methods
	CDataGrid();
	virtual ~CDataGrid();
	virtual BOOL	Create(HWND hParent, RECT wndRect, INT numCols);
	virtual void	Destroy();
	virtual HWND	GetWindowHandle();

	virtual BOOL	SetColumnInfo(INT columnIndex, char* columnText, INT columnWidth, INT textAlign);
	virtual BOOL	SetItemInfo(INT rowIndex, INT columnIndex, char* itemText, INT textAlign, BOOL readOnly=FALSE);
	virtual BOOL	InsertItem(char* itemText, INT textAlign);
	virtual BOOL	RemoveItem(INT row);
	virtual BOOL	GetItemText(INT row, INT column, char* buf, INT buf_size);

	virtual void	Resize();
	virtual void	Update();
	

	// General DataGrid methods
	void	RemoveAllItems();
	void	EnableSort(BOOL enable);
	void	EnableEdit(BOOL enable);
	void	EnableResize(BOOL enable);
	void	EnableGrid(BOOL enable);
	void	EnsureVisible(INT row, INT column);
	void	SelectItem(INT row, INT column);
	INT		GetResizedColumn();
	INT		GetRowNumber();
	INT		GetSelectedRow();
	INT		GetSelectedColumn();
	void	SetCompareFunction(DGCOMPARE CompareFunction);

	// DataGrid SET attribute methods
	void	SetColumnTxtColor(COLORREF txtColor);
	void	SetColumnFont(LOGFONT* lf);
	BOOL	SetItemInfo(_Titem* itemInfo);
	void	SetItemBgColor(INT rowIndex, COLORREF bgColor);
	void	SetRowFont(LOGFONT* lf);
	void	SetRowTxtColor(COLORREF txtColor);

	// DataGrid GET attribute methods
	void	GetColumnFont(LOGFONT* lf);
	void	GetItemInfo(_Titem* itemInfo);
	void	GetRowFont(LOGFONT* lf);

	COLORREF	GetColumnTxtColor();
	COLORREF	GetRowTxtColor();
};






class DGManager
{
public:
	/* DataGrid global variables */
	static CDataGrid::_Tlist *g_DGList;
	static INT g_DGGridNumber;

	/* DataGrid global procedures */
	static LRESULT CALLBACK DataGridProc(HWND, UINT, WPARAM, LPARAM);

	static void	DrawColumns(HWND hWnd);
	static void	DrawRows(HWND hWnd);
	static BOOL	CheckColumnResize(HWND hWnd, INT x, INT y, INT* col, RECT* colRect);
	static BOOL	CheckColumnClick(HWND hWnd, INT x, INT y, INT* col);
	static BOOL	CheckRows(HWND hWnd, INT x, INT y, INT* row);
	static void	GetColumnRect(HWND hWnd, INT col, RECT* colRect);
	static void	GetRowRect(HWND hWnd, INT row, RECT* rowRect);
	static void	RecalcWindow(HWND hWnd);
	static void	SortDGItems(HWND hWnd, INT column);
	static void	Sort(HWND hWnd, INT col, INT size);
	static void	SetDGSelection(HWND hWnd, INT rowIndex, INT columnIndex);
	static void	SelectNextItem(HWND hWnd, CDataGrid::_Trow* item);
	static void	SelectPreviousItem(HWND hWnd, CDataGrid::_Trow* item);
	static void	SelectNextSubitem(HWND hWnd);
	static void	SelectPreviousSubitem(HWND hWnd);
	static void	EnsureRowVisible(HWND hWnd, INT rowIndex);
	static void	EnsureColumnVisible(HWND hWnd, INT columnIndex);
	static void	EnsureVisible(HWND hWnd, INT rowIndex, INT colIndex);
	static void	GetCellRect(HWND hWnd, RECT* cellRect);
	static BOOL	GetDGItemText(HWND hWnd, INT rowIndex, INT columnIndex, char* buffer, INT buffer_size);
	static BOOL	SetDGItemText(HWND hWnd, INT rowIndex, INT columnIndex, char* buffer);
	static void	InitDGGlobals(HWND hParent, HWND hWnd);
	static BOOL	AddDGGrid(HWND hWnd, HWND hParent);
	static void	DestroyDGGrid(HWND hWnd);
	static CDataGrid::_Tlist* DGManager::GetDGGrid(HWND hWnd);
	static CDataGrid::_Tlist* DetachDGGrid(HWND hWnd);
	static void	SiftDown( HWND hWnd, INT root, INT bottom, INT col );
};






CDataGrid::_Trow::_Trow()
{
	rowText		= NULL;
	textAlign	= NULL;
	selected	= NULL;
	readOnly	= NULL;
}


CDataGrid::_Tlist::_Tlist()
{
	next = NULL;	
}





/* DataGrid global variables */
CDataGrid::_Tlist* DGManager::g_DGList = NULL;
INT DGManager::g_DGGridNumber = 0;


CDataGrid::CDataGrid()
{
	// Init DataGrid variables
	m_hWnd = NULL;
	m_hParentWnd = NULL;
}


CDataGrid::~CDataGrid()
{
}


void CDataGrid::Resize()
{
	// Recalculate DataGrid relative size
	RECT rectParent;
	GetWindowRect( m_hParentWnd, &rectParent );
	SetWindowPos( m_hWnd, HWND_NOTOPMOST, INT((double(m_DGRect.left)/100.0)*(rectParent.right-rectParent.left)), INT((double(m_DGRect.top)/100.0)*(rectParent.bottom-rectParent.top)), 
		INT((double(m_DGRect.right)/100.0)*(rectParent.right-rectParent.left)), INT((double(m_DGRect.bottom)/100.0)*(rectParent.bottom-rectParent.top)), SWP_NOZORDER | SWP_SHOWWINDOW );
	RECT clientRect;
	GetClientRect( m_hWnd, &clientRect );
	DGManager::RecalcWindow(m_hWnd);
	InvalidateRect( m_hWnd, NULL, TRUE );
	UpdateWindow(m_hWnd);
}


BOOL CDataGrid::Create(HWND hParent, RECT wndRect, INT numCols)
{
	BOOL result = FALSE;
	
	// Set DatGrid parent window handle
	m_hParentWnd = hParent;
	
	// Calculate DataGrid relative offset and size
	RECT rectParent;
	GetWindowRect( hParent, &rectParent );
	m_DGRect.left = INT(100*double(wndRect.left)/double(rectParent.right-rectParent.left));
	m_DGRect.top = INT(100*double(wndRect.top)/double(rectParent.right-rectParent.left));
	m_DGRect.right = INT(100*double(wndRect.right-wndRect.left)/double(rectParent.right-rectParent.left));
	m_DGRect.bottom = INT(100*double(wndRect.bottom-wndRect.top)/double(rectParent.bottom-rectParent.top));
	
	WNDCLASSEX wincl;
	wincl.hInstance = GetModuleHandle(NULL);
	wincl.lpszClassName = "DATAGRID";
	wincl.lpfnWndProc = DGManager::DataGridProc;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof (WNDCLASSEX);
	wincl.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wincl.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
	wincl.hCursor = LoadCursor( NULL, IDC_ARROW );
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	
	ATOM reg = RegisterClassEx(&wincl);
	DWORD error = GetLastError();
	
	// Register DataGrid window class
	if ( ( reg ) || ( error == ERROR_CLASS_ALREADY_EXISTS ) )
	{
		// Create DataGrid window
		m_hWnd = CreateWindowEx( WS_EX_CLIENTEDGE, "DATAGRID", "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL | WS_HSCROLL, 
			wndRect.left, wndRect.top, wndRect.right-wndRect.left, wndRect.bottom-wndRect.top, hParent, NULL, GetModuleHandle(NULL), NULL );
		
		// Hide DataGrid window scroll bars
		ShowScrollBar( m_hWnd, SB_BOTH, FALSE );
		
		if ( m_hWnd )
		{
			DGManager::AddDGGrid( m_hWnd, m_hParentWnd );
			result = TRUE;
		}

		CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

		if(dgList)
		{
			HDC hDC = GetDC(dgList->dg_hWnd);
			RECT rectClient;
			
			GetClientRect( dgList->dg_hWnd, &rectClient );
			
			dgList->dg_hMemBitmap = CreateCompatibleBitmap( hDC, (rectClient.right-rectClient.left), (rectClient.bottom-rectClient.top) );
			dgList->dg_hMemDC = CreateCompatibleDC(hDC);
			dgList->dg_hOldMemBitmap = (HBITMAP)SelectObject( dgList->dg_hMemDC, dgList->dg_hMemBitmap );
			SetFocus(dgList->dg_hWnd);
			ReleaseDC( dgList->dg_hWnd, hDC );
		}
		
		// Init DataGrid GDI objects
		DGManager::InitDGGlobals( hParent, m_hWnd );
		
		dgList = DGManager::GetDGGrid(m_hWnd);

		if(dgList)
		{
			// Init DataGrid columns info
			if ( numCols < DG_MAXCOLUMN )
				dgList->dg_ColumnNumber = numCols;
			else
				dgList->dg_ColumnNumber = DG_MAXCOLUMN;
			
			dgList->dg_Columns = new _Tcol[dgList->dg_ColumnNumber];
			for ( INT i=0; i<dgList->dg_ColumnNumber; i++ )
			{
				dgList->dg_Columns[i].columnWidth = 50;
				dgList->dg_Columns[i].textAlign = DG_LEFT;
				strcpy( dgList->dg_Columns[i].columnText, " " );
			}
			
			// Create EDIT control
			dgList->dg_hEditWnd = CreateWindow( "EDIT", "", WS_CHILD | WS_CLIPSIBLINGS | ES_AUTOHSCROLL, 0, 0, 100, 20, m_hWnd, NULL, GetModuleHandle(NULL), NULL );
			SendMessage( dgList->dg_hEditWnd, WM_SETFONT, (WPARAM)dgList->dg_hRowFont, MAKELPARAM(TRUE,0) );
		}
	}
	
	return result;
}


void CDataGrid::Destroy()
{
	if ( DGManager::g_DGGridNumber <= 0)
		return;

	
	CDataGrid::_Tlist*	pCur= NULL;
	CDataGrid::_Tlist*	pNxt= NULL;
	
	pCur = DGManager::g_DGList;

	DGManager::g_DGList = NULL;
	DGManager::g_DGGridNumber = 0;

	while (pCur)
	{
		pNxt = pCur->next;

		if ( pCur->dg_RowNumber)
		{
			for(INT i=0; i<pCur->dg_RowNumber; ++i)
			{
				for ( INT j=0; j<pCur->dg_ColumnNumber; ++j)
				{
					SAFE_DELETE_ARRAY(	pCur->dg_Rows[i].rowText[j]	);
				}

				SAFE_DELETE_ARRAY(	pCur->dg_Rows[i].rowText	);
				SAFE_DELETE_ARRAY(	pCur->dg_Rows[i].textAlign	);
				SAFE_DELETE_ARRAY(	pCur->dg_Rows[i].readOnly	);
			}

			SAFE_FREE(	pCur->dg_Rows	);
		}

		if ( pCur->dg_Columns )
		{
			delete pCur->dg_Columns;
			pCur->dg_Columns = NULL;
		}
		// Delete rows
		free(pCur->dg_Rows);
		pCur->dg_Rows = NULL;
		// Delete column font
		if ( pCur->dg_hColumnFont )
		{
			DeleteObject(pCur->dg_hColumnFont);
			pCur->dg_hColumnFont = NULL;
		}
		// Delete row font
		if ( pCur->dg_hRowFont )
		{
			DeleteObject(pCur->dg_hRowFont);
			pCur->dg_hRowFont = NULL;
		}
		// Delete background brush
		if ( pCur->dg_hBgBrush )
		{
			DeleteObject(pCur->dg_hBgBrush);
			pCur->dg_hBgBrush = NULL;
		}
		// Delete cell pen
		if ( pCur->dg_hCellPen )
		{
			DeleteObject(pCur->dg_hCellPen);
			pCur->dg_hCellPen = NULL;
		}
		// Delete memory device context
		if ( pCur->dg_hMemDC )
		{
			SelectObject( pCur->dg_hMemDC, pCur->dg_hOldMemBitmap );
			DeleteDC(pCur->dg_hMemDC);
			pCur->dg_hMemDC = NULL;
		}
		// Delete memory bitmap
		if ( pCur->dg_hMemBitmap )
		{
			DeleteObject(pCur->dg_hMemBitmap);
			pCur->dg_hMemBitmap = NULL;
		}

		delete pCur;

		pCur = pNxt;
	}

}


HWND CDataGrid::GetWindowHandle()
{
	// Return DataGrid window handle
	return m_hWnd;
}


BOOL CDataGrid::SetColumnInfo(INT columnIndex, char* columnText, INT columnWidth, INT textAlign)
{
	BOOL result = FALSE;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Check column index
		if ( columnIndex < dgList->dg_ColumnNumber )
		{
			// Set new DataGrid column info
			strncpy( dgList->dg_Columns[columnIndex].columnText, columnText, 1024 );
			dgList->dg_Columns[columnIndex].columnWidth = columnWidth;
			dgList->dg_Columns[columnIndex].textAlign = textAlign;
			dgList->dg_Columns[columnIndex].pressed = false;
			
			result = TRUE;
		}
	}
	
	return result;
}


BOOL CDataGrid::SetItemInfo(INT rowIndex, INT columnIndex, char* itemText, INT textAlign, BOOL readOnly)
{
	BOOL result = FALSE;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		if ( DGManager::SetDGItemText( m_hWnd, rowIndex, columnIndex, itemText ) )
		{
			dgList->dg_Rows[rowIndex].textAlign[columnIndex] = textAlign;
			if ( dgList->dg_EnableEdit )
				dgList->dg_Rows[rowIndex].readOnly[columnIndex] = readOnly;
			
			result = TRUE;
		}
	}
	
	return result;
}


BOOL CDataGrid::SetItemInfo(_Titem* itemInfo)
{
	BOOL result = FALSE;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		if( itemInfo && ( itemInfo->dgItem < dgList->dg_RowNumber ) && ( itemInfo->dgSubitem < dgList->dg_ColumnNumber ) )
		{
			switch ( itemInfo->dgMask )
			{
			case DG_TEXTEDIT:
				{
					// Set DataGrid item text
					DGManager::SetDGItemText( m_hWnd, itemInfo->dgItem, itemInfo->dgSubitem, itemInfo->dgText );
				}
				break;
				
			case DG_TEXTALIGN:
				{
					// Set DataGrid item text align
					dgList->dg_Rows[itemInfo->dgItem].textAlign[itemInfo->dgSubitem] = itemInfo->dgTextAlign;
				}
				break;
				
			case DG_TEXTHIGHLIGHT:
				{
					// Select DataGrid item
					SelectItem( itemInfo->dgItem, itemInfo->dgSubitem );
				}
				break;
				
			case DG_TEXTRONLY:
				{
					// Set DataGrid item edit mode
					dgList->dg_Rows[itemInfo->dgItem].readOnly[itemInfo->dgSubitem] = itemInfo->dgReadOnly;
				}
				break;
				
			case DG_TEXTBGCOLOR:
				{
					// Set DataGrid row background color
					dgList->dg_Rows[itemInfo->dgItem].bgColor = itemInfo->dgBgColor;
				}
				break;
			}
		}
	}
	
	return result;
}


BOOL CDataGrid::GetItemText(INT rowIndex, INT columnIndex, char* buffer, INT buffer_size)
{
	BOOL result = DGManager::GetDGItemText( m_hWnd, rowIndex, columnIndex, buffer, buffer_size );
	return result;
}



INT CDataGrid::GetResizedColumn()
{
	INT result;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
		result = dgList->dg_Column;
	
	// Return resized column
	return result;
}


BOOL CDataGrid::InsertItem(char* itemText, INT textAlign)
{
	BOOL result = FALSE;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		if ( dgList->dg_RowNumber < DG_MAXROW )
		{
			dgList->dg_RowNumber++;
			
			if ( dgList->dg_Rows == NULL )
				dgList->dg_Rows = (_Trow*)malloc(dgList->dg_RowNumber*sizeof(_Trow));
			else
				dgList->dg_Rows = (_Trow*)realloc(dgList->dg_Rows, dgList->dg_RowNumber*sizeof(_Trow));
			
			dgList->dg_Rows[dgList->dg_RowNumber-1].rowText = new char*[dgList->dg_ColumnNumber];
			dgList->dg_Rows[dgList->dg_RowNumber-1].textAlign = new INT[dgList->dg_ColumnNumber];
			dgList->dg_Rows[dgList->dg_RowNumber-1].readOnly = new BOOL[dgList->dg_ColumnNumber];
			
			for ( INT j=0; j<dgList->dg_ColumnNumber; j++ )
			{
				dgList->dg_Rows[dgList->dg_RowNumber-1].rowText[j] = new char[strlen(itemText)+1];
				strcpy( dgList->dg_Rows[dgList->dg_RowNumber-1].rowText[j], " " );
				dgList->dg_Rows[dgList->dg_RowNumber-1].textAlign[j] = textAlign;
				dgList->dg_Rows[dgList->dg_RowNumber-1].readOnly[j] = false;
			}

			strcpy( dgList->dg_Rows[dgList->dg_RowNumber-1].rowText[0], itemText );
			dgList->dg_Rows[dgList->dg_RowNumber-1].selected = false;
			dgList->dg_Rows[dgList->dg_RowNumber-1].bgColor = DG_BGR_COLOR;
			
			result = TRUE;
			
			// Send notification to the parent window
			SendMessage( dgList->dg_hParent, WM_COMMAND, MAKEWPARAM(0,DGM_ITEMADDED), (LPARAM)m_hWnd );
		}
	}
	
	return result;
}


BOOL CDataGrid::RemoveItem(INT row)
{
	BOOL result = FALSE;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);
	if ( ( row >= 0 ) && ( row < dgList->dg_RowNumber ) )
	{
		dgList->dg_RowNumber--;
		if ( row <= dgList->dg_RowNumber-1 )
		{
			for ( INT i=row; i<dgList->dg_RowNumber; i++ )
				memcpy( &dgList->dg_Rows[i], &dgList->dg_Rows[i+1], sizeof(_Trow) );
			dgList->dg_Rows = (_Trow*)realloc(dgList->dg_Rows, dgList->dg_RowNumber*sizeof(_Trow));
		}
		else
			dgList->dg_Rows = (_Trow*)realloc(dgList->dg_Rows, dgList->dg_RowNumber*sizeof(_Trow));
		
		// Mark selection
		dgList->dg_Selected = NULL;
		dgList->dg_SelectedRow = -1;
		dgList->dg_SelectedColumn = -1;
		
		result = TRUE;
	}
	
	// Send notification to the parent window
	SendMessage( dgList->dg_hParent, WM_COMMAND, MAKEWPARAM(0,DGM_ITEMREMOVED), (LPARAM)m_hWnd );
	
	return result;
}


void CDataGrid::RemoveAllItems()
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);
	
	// Delete all items
	if(dgList->dg_Rows)
	{
		dgList->dg_Edit = FALSE;
		ShowWindow( dgList->dg_hEditWnd, SW_HIDE );
		
		free(dgList->dg_Rows);
		dgList->dg_Rows = NULL;
		dgList->dg_RowNumber = 0;
		dgList->dg_Row = -1;
		SetScrollPos( m_hWnd, SB_HORZ, 0, TRUE );
		SetScrollPos( m_hWnd, SB_VERT, 0, TRUE );
		DGManager::RecalcWindow(m_hWnd);
	}
	
	InvalidateRect( m_hWnd, NULL, TRUE );
	UpdateWindow(m_hWnd);
}


INT CDataGrid::GetSelectedRow()
{
	INT result = -1;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Return selected row
		result  = dgList->dg_SelectedRow;
	}
	
	return result;
}


INT CDataGrid::GetSelectedColumn()
{
	INT result = -1;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Return selected column
		result = dgList->dg_SelectedColumn;
	}
	
	return result;
}


void CDataGrid::Update()
{
	// Update DataGrid window
	DGManager::RecalcWindow(m_hWnd);
	InvalidateRect( m_hWnd, NULL, TRUE );
	UpdateWindow(m_hWnd);
}


void CDataGrid::SetCompareFunction(DGCOMPARE CompareFunction)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Set custom comparison function
		dgList->dg_CompareFunc = (DGCOMPARE)CompareFunction;
	}
}


void CDataGrid::SetItemBgColor(INT rowIndex, COLORREF bgColor)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);
	
	if ( ( rowIndex >= 0 ) && ( rowIndex < dgList->dg_RowNumber ) )
	{
		dgList->dg_Rows[rowIndex].bgColor = bgColor;
		InvalidateRect( m_hWnd, NULL, TRUE );
		UpdateWindow(m_hWnd);
	}
}


void CDataGrid::EnableSort(BOOL enable)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Set sort flag
		dgList->dg_EnableSort = enable;
	}
}


void CDataGrid::EnableEdit(BOOL enable)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Set edit flag
		dgList->dg_EnableEdit = enable;
	}
}


void CDataGrid::EnableResize(BOOL enable)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Set resize flag
		dgList->dg_EnableResize = enable;
	}
}


void CDataGrid::EnableGrid(BOOL enable)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Set grid flag
		dgList->dg_EnableGrid = enable;
	}
}


void CDataGrid::SetRowTxtColor(COLORREF txtColor)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Set DataGrid Text color
		dgList->dg_RowTextColor = txtColor;
		InvalidateRect( m_hWnd, NULL, TRUE );
		UpdateWindow(m_hWnd);
	}
}


void CDataGrid::SetColumnTxtColor(COLORREF txtColor)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Set DataGrid Text color
		dgList->dg_ColumnTextColor = txtColor;
		InvalidateRect( m_hWnd, NULL, TRUE );
		UpdateWindow(m_hWnd);
	}
}


INT CDataGrid::GetRowNumber()
{
	INT result;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Get DataGrid row number
		result = dgList->dg_RowNumber;
	}
	
	return result;
}


void CDataGrid::GetColumnFont(LOGFONT* lf)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Get DataGrid column font
		memcpy( lf, &dgList->dg_LFColumnFont, sizeof(LOGFONT) );
	}
}


void CDataGrid::SetColumnFont(LOGFONT* lf)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Set DataGrid column font
		memcpy( &dgList->dg_LFColumnFont, lf, sizeof(LOGFONT) );
		
		if ( dgList->dg_hColumnFont )
			DeleteObject(dgList->dg_hColumnFont);
		dgList->dg_hColumnFont = CreateFontIndirect(lf);
	}
}


void CDataGrid::GetRowFont(LOGFONT* lf)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Get DataGrid row font
		memcpy( lf, &dgList->dg_LFRowFont, sizeof(LOGFONT) );
	}
}


void CDataGrid::SetRowFont(LOGFONT* lf)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Set DataGrid row font
		memcpy( &dgList->dg_LFRowFont, lf, sizeof(LOGFONT) );
		
		if ( dgList->dg_hRowFont )
			DeleteObject(dgList->dg_hRowFont);
		dgList->dg_hRowFont = CreateFontIndirect(lf);
	}
}


COLORREF CDataGrid::GetColumnTxtColor()
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Get DataGrid column text color
		return dgList->dg_ColumnTextColor;
	}
	else
		return RGB(0,0,0);
}


void CDataGrid::GetItemInfo(_Titem* itemInfo)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Get DataGrid item info
		if(itemInfo && ( itemInfo->dgItem < dgList->dg_RowNumber ) && ( itemInfo->dgSubitem < dgList->dg_ColumnNumber ) )
		{
			switch ( itemInfo->dgMask )
			{
			case DG_TEXTEDIT:
				{
					// Get DataGrid item text
					DGManager::GetDGItemText( m_hWnd, itemInfo->dgItem, itemInfo->dgSubitem, itemInfo->dgText, itemInfo->dgTextLen );
				}
				break;
				
			case DG_TEXTALIGN:
				{
					// Get DataGrid item text alignment
					itemInfo->dgTextAlign = dgList->dg_Rows[itemInfo->dgItem].textAlign[itemInfo->dgSubitem];
				}
				break;
				
			case DG_TEXTHIGHLIGHT:
				{
					// Get DataGrid row selection state
					itemInfo->dgSelected = dgList->dg_Rows[itemInfo->dgItem].selected;
				}
				break;
				
			case DG_TEXTRONLY:
				{
					// Get DataGrid item edit mode
					itemInfo->dgReadOnly = dgList->dg_Rows[itemInfo->dgItem].readOnly[itemInfo->dgSubitem];
				}
				break;
				
			case DG_TEXTBGCOLOR:
				{
					// Get DataGrid row background color
					itemInfo->dgBgColor = dgList->dg_Rows[itemInfo->dgItem].bgColor;
				}
				break;
			}
		}
	}
}


COLORREF CDataGrid::GetRowTxtColor()
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Get DataGrid row text color
		return dgList->dg_RowTextColor;
	}
	else
		return RGB(0,0,0);
}


void CDataGrid::EnsureVisible(INT row, INT column)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Ensure DataGrid item is full visible
		DGManager::EnsureVisible( m_hWnd, row, column );
	}
}


void CDataGrid::SelectItem(INT row, INT column)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(m_hWnd);

	if(dgList)
	{
		// Set DataGrid selection
		for ( INT j=0; j<dgList->dg_RowNumber; j++ )
		{
			if ( j == row )
			{
				// Mark selection
				dgList->dg_Rows[j].selected = true;
				dgList->dg_Selected = &dgList->dg_Rows[j];
				dgList->dg_SelectedRow = row;
				dgList->dg_SelectedColumn = column;
			}
			else
				dgList->dg_Rows[j].selected = false;
		}
	}
}




////////////////////////////////////////////////////////////////////////////////

void DGManager::InitDGGlobals(HWND hParent, HWND hWnd)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);

	if(dgList)
	{
		// Create fonts
		HDC hParentDC = GetDC(hParent);
		LOGFONT lf;
		lf.lfHeight = -MulDiv( 10, GetDeviceCaps(hParentDC,LOGPIXELSY), 72 );
		lf.lfWidth = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfWeight = FW_NORMAL;
		lf.lfItalic = FALSE;
		lf.lfUnderline = FALSE;
		lf.lfStrikeOut = FALSE;
		lf.lfCharSet = ANSI_CHARSET;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		strcpy( lf.lfFaceName, "Arial" );
		dgList->dg_hColumnFont = CreateFontIndirect(&lf);
		memcpy( &dgList->dg_LFColumnFont, &lf, sizeof(LOGFONT) );
		lf.lfHeight = -MulDiv( 9, GetDeviceCaps(hParentDC,LOGPIXELSY), 72 );
		dgList->dg_hRowFont = CreateFontIndirect(&lf);
		memcpy( &dgList->dg_LFRowFont, &lf, sizeof(LOGFONT) );
		ReleaseDC( hParent, hParentDC );
		
		// Create background brush
		LOGBRUSH lb;
		lb.lbColor = IDataGrid::DG_BGR_COLOR;
		lb.lbStyle = BS_SOLID;
		dgList->dg_hBgBrush = CreateBrushIndirect(&lb);
		
		// Create cell pen
		LOGPEN lp;
		lp.lopnColor = RGB(210,210,210);
		lp.lopnStyle = PS_SOLID;
		lp.lopnWidth.x = 1;
		dgList->dg_hCellPen = CreatePenIndirect(&lp);
	}
}



BOOL DGManager::SetDGItemText(HWND hWnd, INT rowIndex, INT columnIndex, char* buffer)
{
	BOOL result = FALSE;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);

	if(dgList)
	{
		// Check column and row index
		if( buffer && ( columnIndex < dgList->dg_ColumnNumber ) && ( rowIndex < dgList->dg_RowNumber ))
		{
			// Set DataGrid row info
			if ( strlen(dgList->dg_Rows[rowIndex].rowText[columnIndex]) < strlen(buffer) )
			{
				delete dgList->dg_Rows[rowIndex].rowText[columnIndex];
				dgList->dg_Rows[rowIndex].rowText[columnIndex] = new char[strlen(buffer)+1];
			}
			if ( strlen(buffer) == 0 )
				strcpy( dgList->dg_Rows[rowIndex].rowText[columnIndex], " " );
			else
				strcpy( dgList->dg_Rows[rowIndex].rowText[columnIndex], buffer );
			
			result = TRUE;
		}
	}
	
	return result;
}



BOOL DGManager::GetDGItemText(HWND hWnd, INT rowIndex, INT columnIndex, char* buffer, INT buffer_size)
{
	BOOL result = FALSE;
	
	// Clear return buffer
	strcpy( buffer, "" );
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	// Check column and row index
	if ( ( columnIndex < dgList->dg_ColumnNumber ) && ( rowIndex < dgList->dg_RowNumber ) )
	{
		// Get DataGrid item text
		strncpy( buffer, dgList->dg_Rows[rowIndex].rowText[columnIndex], buffer_size );
		
		result = TRUE;
	}
	
	return result;
}


LRESULT CALLBACK DGManager::DataGridProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);

//			if(dgList)
//			{
//				HDC hDC = GetDC(dgList->dg_hWnd);
//				RECT rectClient;
//				GetClientRect( dgList->dg_hWnd, &rectClient );
//				dgList->dg_hMemBitmap = CreateCompatibleBitmap( hDC, (rectClient.right-rectClient.left), (rectClient.bottom-rectClient.top) );
//				dgList->dg_hMemDC = CreateCompatibleDC(hDC);
//				dgList->dg_hOldMemBitmap = (HBITMAP)SelectObject( dgList->dg_hMemDC, dgList->dg_hMemBitmap );
//				SetFocus(dgList->dg_hWnd);
//				ReleaseDC( dgList->dg_hWnd, hDC );
//			}
		}
		break;
		
	case WM_DESTROY:
		{
			// Delete DataGrid grid
			DGManager::DestroyDGGrid(hwnd);
		}
		break;
		
	case WM_SIZE:
		{
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);

			if(dgList)
			{
				// Delete memory device context
				if ( dgList->dg_hMemDC )
				{
					SelectObject( dgList->dg_hMemDC, dgList->dg_hOldMemBitmap );
					DeleteDC(dgList->dg_hMemDC);
					dgList->dg_hMemDC = NULL;
				}
				// Delete memory bitmap
				if ( dgList->dg_hMemBitmap )
				{
					DeleteObject(dgList->dg_hMemBitmap);
					dgList->dg_hMemBitmap = NULL;
				}
				
				HDC hDC = GetDC(dgList->dg_hWnd);
				RECT rectClient;
				GetClientRect( dgList->dg_hWnd, &rectClient );
				dgList->dg_hMemBitmap = CreateCompatibleBitmap( hDC, (rectClient.right-rectClient.left), (rectClient.bottom-rectClient.top) );
				dgList->dg_hMemDC = CreateCompatibleDC(hDC);
				dgList->dg_hOldMemBitmap = (HBITMAP)SelectObject( dgList->dg_hMemDC, dgList->dg_hMemBitmap );
				ReleaseDC( dgList->dg_hWnd, hDC );
			}
		}
		
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint( hwnd, &ps );
			
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);

			if(dgList)
			{
				// Draw DataGrid rows
				DGManager::DrawRows(dgList->dg_hWnd);
				// Draw DataGrid columns
				DGManager::DrawColumns(dgList->dg_hWnd);
				
				RECT clientRect;
				GetClientRect( dgList->dg_hWnd, &clientRect );
				BitBlt( ps.hdc, 0, 0, (clientRect.right-clientRect.left), (clientRect.bottom-clientRect.top), dgList->dg_hMemDC, 0, 0, SRCCOPY );
			}
			
			EndPaint( hwnd, &ps );
		}
		break;
		
	case WM_ERASEBKGND:
		{
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);

			if(dgList)
			{
				RECT clientRect;
				GetClientRect( dgList->dg_hWnd, &clientRect );
				FillRect( dgList->dg_hMemDC, &clientRect, dgList->dg_hBgBrush );
			}
		}
		break;
		
	case WM_HSCROLL:
		{
			switch ( LOWORD(wParam) )
			{
			case SB_LINERIGHT:
			case SB_PAGERIGHT:
				{
					INT OldPos = GetScrollPos( hwnd, SB_HORZ );
					SetScrollPos( hwnd, SB_HORZ, OldPos+10, TRUE );
					INT NewPos = GetScrollPos( hwnd, SB_HORZ );
					InvalidateRect( hwnd, NULL, TRUE );
					UpdateWindow(hwnd);
				}
				break;
				
			case SB_LINELEFT:
			case SB_PAGELEFT:
				{
					INT OldPos = GetScrollPos( hwnd, SB_HORZ );
					SetScrollPos( hwnd, SB_HORZ, OldPos-10, TRUE );
					INT NewPos = GetScrollPos( hwnd, SB_HORZ );
					InvalidateRect( hwnd, NULL, TRUE );
					UpdateWindow(hwnd);
				}
				break;
				
			case SB_THUMBTRACK:
				{
					SCROLLINFO si;
					si.cbSize = sizeof(SCROLLINFO);
					si.fMask = SIF_ALL;
					GetScrollInfo( hwnd, SB_HORZ, &si );
					INT OldPos = si.nPos;
					si.nPos = si.nTrackPos;
					INT NewPos = si.nPos;
					SetScrollPos( hwnd, SB_HORZ, si.nPos, TRUE );
					InvalidateRect( hwnd, NULL, TRUE );
					UpdateWindow(hwnd);
				}
				break;
			}
			
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);
			if ( dgList->dg_Edit )
			{
				INT scrollX = GetScrollPos( hwnd, SB_HORZ );
				INT scrollY = GetScrollPos( hwnd, SB_VERT );
				RECT columnRect, clientRect;
				GetClientRect( hwnd, &clientRect );
				DGManager::GetColumnRect( hwnd, 0, &columnRect );
				INT offsetLeft = scrollX + clientRect.left;
				INT offsetRight = scrollX + (clientRect.right-clientRect.left);
				INT offsetTop = scrollY + clientRect.top + (columnRect.bottom-columnRect.top);
				INT offsetBottom = scrollY + (clientRect.bottom - clientRect.top);
				if ( ( dgList->dg_EditRect.top >= offsetTop ) && ( dgList->dg_EditRect.bottom <= offsetBottom ) &&
					( dgList->dg_EditRect.right >= offsetLeft ) && ( dgList->dg_EditRect.left <= offsetRight ) )
				{
					SetWindowPos( dgList->dg_hEditWnd, HWND_NOTOPMOST, dgList->dg_EditRect.left-scrollX, dgList->dg_EditRect.top-scrollY, dgList->dg_EditRect.right-dgList->dg_EditRect.left, dgList->dg_EditRect.bottom-dgList->dg_EditRect.top, SWP_NOZORDER | SWP_SHOWWINDOW );
					SetFocus(dgList->dg_hEditWnd);
				}
				else
					ShowWindow( dgList->dg_hEditWnd, SW_HIDE );
			}
		}
		break;
		
	case WM_VSCROLL:
		{
			switch ( LOWORD(wParam) )
			{
			case SB_LINEUP:
				{
					RECT rowRect;
					DGManager::GetRowRect( hwnd, 0, &rowRect );
					INT OldPos = GetScrollPos( hwnd, SB_VERT );
					INT diff = (rowRect.bottom-rowRect.top) - 1;
					SetScrollPos( hwnd, SB_VERT, OldPos-diff, TRUE );
					InvalidateRect( hwnd, NULL, TRUE );
					UpdateWindow(hwnd);
				}
				break;
				
			case SB_PAGEUP:
				{
					RECT rowRect, clientRect;
					DGManager::GetRowRect( hwnd, 0, &rowRect );
					GetClientRect( hwnd, &clientRect );
					INT OldPos = GetScrollPos( hwnd, SB_VERT );
					INT diff = (clientRect.bottom-clientRect.top) / (rowRect.bottom-rowRect.top-1) - 2;
					diff *= (rowRect.bottom-rowRect.top-1);
					SetScrollPos( hwnd, SB_VERT, OldPos-diff, TRUE );
					InvalidateRect( hwnd, NULL, TRUE );
					UpdateWindow(hwnd);
				}
				break;
				
			case SB_LINEDOWN:
				{
					RECT rowRect;
					DGManager::GetRowRect( hwnd, 0, &rowRect );
					INT OldPos = GetScrollPos( hwnd, SB_VERT );
					INT diff = (rowRect.bottom-rowRect.top) - 1;
					SetScrollPos( hwnd, SB_VERT, OldPos+diff, TRUE );
					InvalidateRect( hwnd, NULL, TRUE );
					UpdateWindow(hwnd);
				}
				break;
				
			case SB_PAGEDOWN:
				{
					RECT rowRect, clientRect;
					DGManager::GetRowRect( hwnd, 0, &rowRect );
					GetClientRect( hwnd, &clientRect );
					INT OldPos = GetScrollPos( hwnd, SB_VERT );
					INT diff = (clientRect.bottom-clientRect.top) / (rowRect.bottom-rowRect.top-1) - 2;
					diff *= (rowRect.bottom-rowRect.top-1);
					SetScrollPos( hwnd, SB_VERT, OldPos+diff, TRUE );
					InvalidateRect( hwnd, NULL, TRUE );
					UpdateWindow(hwnd);
				}
				break;
				
			case SB_THUMBTRACK:
				{
					RECT rowRect, clientRect, columnRect;
					DGManager::GetRowRect( hwnd, 0, &rowRect );
					DGManager::GetColumnRect( hwnd, 0, &columnRect );
					GetClientRect( hwnd, &clientRect );
					clientRect.top = columnRect.bottom;
					SCROLLINFO si;
					si.cbSize = sizeof(SCROLLINFO);
					si.fMask = SIF_ALL;
					GetScrollInfo( hwnd, SB_VERT, &si );
					INT OldPos = si.nPos;
					INT NewPos = si.nTrackPos;
					INT diff = NewPos % (rowRect.bottom-rowRect.top-1);
					NewPos -= diff;
					si.nPos = NewPos;
					si.nTrackPos = NewPos;
					SetScrollPos( hwnd, SB_VERT, NewPos, TRUE );
					InvalidateRect( hwnd, NULL, TRUE );
					UpdateWindow(hwnd);
				}
				break;
			}
			
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);
			if ( dgList->dg_Edit )
			{
				INT scrollX = GetScrollPos( hwnd, SB_HORZ );
				INT scrollY = GetScrollPos( hwnd, SB_VERT );
				RECT columnRect, clientRect;
				GetClientRect( hwnd, &clientRect );
				DGManager::GetColumnRect( hwnd, 0, &columnRect );
				INT offsetLeft = scrollX + clientRect.left;
				INT offsetRight = scrollX + (clientRect.right-clientRect.left);
				INT offsetTop = scrollY + clientRect.top + (columnRect.bottom-columnRect.top);
				INT offsetBottom = scrollY + (clientRect.bottom - clientRect.top);
				if ( ( dgList->dg_EditRect.top >= offsetTop ) && ( dgList->dg_EditRect.bottom <= offsetBottom ) &&
					( dgList->dg_EditRect.right >= offsetLeft ) && ( dgList->dg_EditRect.left <= offsetRight ) )
				{
					SetWindowPos( dgList->dg_hEditWnd, HWND_NOTOPMOST, dgList->dg_EditRect.left-scrollX, dgList->dg_EditRect.top-scrollY, dgList->dg_EditRect.right-dgList->dg_EditRect.left, dgList->dg_EditRect.bottom-dgList->dg_EditRect.top, SWP_NOZORDER | SWP_SHOWWINDOW );
					SetFocus(dgList->dg_hEditWnd);
				}
				else
					ShowWindow( dgList->dg_hEditWnd, SW_HIDE );
			}
		}
		break;
		
	case WM_MOUSEWHEEL:
		{
			SHORT zDelta = (SHORT)HIWORD(wParam);
			if ( zDelta > 0 )
			{
				// Scroll page up
				INT scrollCode = SB_PAGEUP;
				SHORT pos = GetScrollPos( hwnd, SB_VERT );
				SendMessage( hwnd, WM_VSCROLL, MAKEWPARAM(scrollCode,pos), (LPARAM)NULL );
			}
			else
			{
				// Scroll page down
				INT scrollCode = SB_PAGEDOWN;
				SHORT pos = GetScrollPos( hwnd, SB_VERT );
				SendMessage( hwnd, WM_VSCROLL, MAKEWPARAM(scrollCode,pos), (LPARAM)NULL );
			}
		}
		break;
		
	case WM_LBUTTONDOWN:
		{
			INT scrollX = GetScrollPos( hwnd, SB_HORZ );
			INT scrollY = GetScrollPos( hwnd, SB_VERT );
			INT xPos = LOWORD(lParam) + scrollX;
			INT yPos = HIWORD(lParam) + scrollY;
			
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);

			if(dgList)
			{
				if ( dgList->dg_Edit )
				{
					char text[1024];
					SendMessage( dgList->dg_hEditWnd, WM_GETTEXT, (WPARAM)1024, (LPARAM)text );
					ShowWindow( dgList->dg_hEditWnd, SW_HIDE );
					dgList->dg_Edit = FALSE;
					DGManager::SetDGItemText( hwnd, dgList->dg_SelectedRow, dgList->dg_SelectedColumn, text );
					
					// Send notification to the parent window
					SendMessage( dgList->dg_hParent, WM_COMMAND, MAKEWPARAM(0, IDataGrid::DGM_ITEMTEXTCHANGED), (LPARAM)hwnd );
				}
				
				// Check for resized columns
				RECT rect;
				if ( dgList->dg_Resize == FALSE )
					dgList->dg_Resize = DGManager::CheckColumnResize( hwnd, xPos, yPos, &dgList->dg_Column, &rect );
				// Check for clicked columns
				if ( !dgList->dg_Resize )
					dgList->dg_Click = DGManager::CheckColumnClick( hwnd, xPos, yPos, &dgList->dg_Column );
				if ( dgList->dg_Click )
					dgList->dg_Columns[dgList->dg_Column].pressed = true;
				
				// Check for selected rows
				if ( !dgList->dg_Click )
				{
					if ( DGManager::CheckRows( hwnd, xPos, yPos, &dgList->dg_Row ) )
					{
						// Send notification to the parent window
						SendMessage( dgList->dg_hParent, WM_COMMAND, MAKEWPARAM(0, IDataGrid::DGM_ITEMCHANGED), (LPARAM)hwnd );
					}
				}
				InvalidateRect( hwnd, NULL, FALSE );
				UpdateWindow(hwnd);
				
				SetFocus(hwnd);
				
				// Capture mouse
				SetCapture(hwnd);
			}
		}
		break;
		
	case WM_LBUTTONUP:
		{
			INT scrollX = GetScrollPos( hwnd, SB_HORZ );
			INT scrollY = GetScrollPos( hwnd, SB_VERT );
			INT xPos = LOWORD(lParam) + scrollX;
			INT yPos = HIWORD(lParam) + scrollY;
			
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);

			if(dgList)
			{
				for ( INT i=0; i<dgList->dg_ColumnNumber; i++ )
					dgList->dg_Columns[i].pressed = false;
				
				if ( dgList->dg_Resize )
				{
					// Send notification to the parent window
					SendMessage( dgList->dg_hParent, WM_COMMAND, MAKEWPARAM(0, IDataGrid::DGM_COLUMNRESIZED), (LPARAM)hwnd );
				}
				
				RECT rect;
				dgList->dg_Resize = FALSE;
				if ( ( dgList->dg_Click ) && ( dgList->dg_EnableSort ) )
				{
					DGManager::SortDGItems( hwnd, dgList->dg_Column );
					dgList->dg_Click = FALSE;
					
					// Send notification to the parent window
					SendMessage( dgList->dg_hParent, WM_COMMAND, MAKEWPARAM(0, IDataGrid::DGM_COLUMNCLICKED), (LPARAM)hwnd );
				}
				dgList->dg_Cursor = DGManager::CheckColumnResize( hwnd, xPos, yPos, &dgList->dg_Column, &rect );
				InvalidateRect( hwnd, NULL, FALSE );
				UpdateWindow(hwnd);
				
				// Release mouse
				ReleaseCapture();
			}
		}
		break;
		
	case WM_NCLBUTTONUP:
		{
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);

			if(dgList)
			{
				for ( INT i=0; i<dgList->dg_ColumnNumber; i++ )
					dgList->dg_Columns[i].pressed = false;
				
				if ( dgList->dg_Resize )
				{
					// Send notification to the parent window
					SendMessage( dgList->dg_hParent, WM_COMMAND, MAKEWPARAM(0, IDataGrid::DGM_COLUMNRESIZED), (LPARAM)hwnd );
				}
				
				dgList->dg_Resize = FALSE;
				if ( ( dgList->dg_Click ) && ( dgList->dg_EnableSort ) )
				{
					DGManager::SortDGItems( hwnd, dgList->dg_Column );
					dgList->dg_Click = FALSE;
					
					// Send notification to the parent window
					SendMessage( dgList->dg_hParent, WM_COMMAND, MAKEWPARAM(0, IDataGrid::DGM_COLUMNCLICKED), (LPARAM)hwnd );
				}
				InvalidateRect( hwnd, NULL, FALSE );
				UpdateWindow(hwnd);
			}
		}
		break;
		
	case WM_LBUTTONDBLCLK:
		{
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);

			if( dgList && ( dgList->dg_EnableEdit ) )
			{
				INT scrollX = GetScrollPos( hwnd, SB_HORZ );
				INT scrollY = GetScrollPos( hwnd, SB_VERT );
				RECT columnRect, clientRect;
				GetClientRect( hwnd, &clientRect );
				DGManager::GetColumnRect( hwnd, 0, &columnRect );
				INT offsetLeft = scrollX + clientRect.left;
				INT offsetRight = scrollX + (clientRect.right-clientRect.left);
				INT offsetTop = scrollY + clientRect.top + (columnRect.bottom-columnRect.top);
				INT offsetBottom = scrollY + (clientRect.bottom - clientRect.top);
				DGManager::GetCellRect( hwnd, &dgList->dg_EditRect );
				if ( ( dgList->dg_EditRect.top >= offsetTop ) && ( dgList->dg_EditRect.bottom <= offsetBottom ) &&
					( dgList->dg_EditRect.right >= offsetLeft ) && ( dgList->dg_EditRect.left <= offsetRight ) )
				{
					if ( !dgList->dg_Rows[dgList->dg_SelectedRow].readOnly[dgList->dg_SelectedColumn] )
					{
						if ( DGManager::CheckRows( hwnd, LOWORD(lParam) + scrollX, HIWORD(lParam) + scrollY, &dgList->dg_Row ) )
						{
							char text[1024];
							GetDGItemText( hwnd, dgList->dg_SelectedRow, dgList->dg_SelectedColumn, text, 1024 );
							SetWindowPos( dgList->dg_hEditWnd, HWND_NOTOPMOST, dgList->dg_EditRect.left-scrollX, dgList->dg_EditRect.top-scrollY, dgList->dg_EditRect.right-dgList->dg_EditRect.left, dgList->dg_EditRect.bottom-dgList->dg_EditRect.top, SWP_NOZORDER | SWP_SHOWWINDOW );
							SendMessage( dgList->dg_hEditWnd, WM_SETTEXT, (WPARAM)0, (LPARAM)(LPCTSTR)text );
							SetFocus(dgList->dg_hEditWnd);
							dgList->dg_Edit = TRUE;
						}
					}
				}
			}
		}
		break;
		
	case WM_MOUSEMOVE:
		{
			INT scrollX = GetScrollPos( hwnd, SB_HORZ );
			INT scrollY = GetScrollPos( hwnd, SB_VERT );
			INT xPos = LOWORD(lParam) + scrollX;
			INT yPos = HIWORD(lParam) + scrollY;
			
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);

			if(dgList)
			{
				INT oldSize, newSize;
				RECT rect;
				if ( wParam == MK_LBUTTON )
				{
					if ( ( dgList->dg_Resize ) && ( dgList->dg_EnableResize ) )
					{
						// Get column rectangle
						DGManager::GetColumnRect( hwnd, dgList->dg_Column, &rect );
						
						RECT wndRect;
						GetWindowRect( hwnd, &wndRect );
						POINT pt = {LOWORD(lParam), HIWORD(lParam)};
						
						if ( ( pt.x >= 0 ) && ( pt.x <= 2000 ) )
						{
							// Calculate new column size
							oldSize = rect.right - rect.left;
							newSize = xPos - rect.left;
							if ( newSize < 0 )
								newSize = 2;
							
							// Resize DataGrid column
							dgList->dg_Columns[dgList->dg_Column].columnWidth = newSize;
							DGManager::RecalcWindow(hwnd);
							RECT clientRect, columnRect;
							DGManager::GetColumnRect( hwnd, 0, &columnRect );
							GetClientRect( hwnd, &clientRect );
							clientRect.top += (columnRect.bottom-columnRect.top);
							InvalidateRect( hwnd, NULL, TRUE );
							UpdateWindow(hwnd);
							
							dgList->dg_Cursor = TRUE;
						}
					}
					else
						dgList->dg_Cursor = FALSE;
				}
				else if ( dgList->dg_EnableResize )
					dgList->dg_Cursor = DGManager::CheckColumnResize( hwnd, xPos, yPos, &dgList->dg_Column, &rect );
			}
		}
		break;
		
	case WM_SETCURSOR:
		{
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);

			if(dgList)
			{
				// Check cursor flag
				if ( ( dgList->dg_Cursor ) && ( LOWORD(lParam) == HTCLIENT ) )
					SetCursor(LoadCursor(NULL, IDC_SIZEWE));
				else
					SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
		}
		break;
		
	case WM_KEYDOWN:
		{
			CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hwnd);

			if(dgList)
			{
				if ( dgList->dg_Selected )
				{
					/* KEY_DOWN pressed */
					if ( INT(wParam) == 40 )
					{
						// Select next item
						DGManager::SelectNextItem( hwnd, dgList->dg_Selected );
						DGManager::EnsureVisible( hwnd, dgList->dg_SelectedRow, dgList->dg_SelectedColumn );
					}
					/* KEY_UP pressed */
					else if ( INT(wParam) == 38 )
					{
						// Select previous item
						DGManager::SelectPreviousItem( hwnd, dgList->dg_Selected );
						DGManager::EnsureVisible( hwnd, dgList->dg_SelectedRow, dgList->dg_SelectedColumn );
					}
					/* KEY_LEFT pressed */
					else if ( INT(wParam) == 37 )
					{
						// Select previous subitem
						DGManager::SelectPreviousSubitem(hwnd);
						DGManager::EnsureVisible( hwnd, dgList->dg_SelectedRow, dgList->dg_SelectedColumn );
					}
					/* KEY_RIGHT pressed */
					else if ( INT(wParam) == 39 )
					{
						// Select next subitem
						DGManager::SelectNextSubitem(hwnd);
						DGManager::EnsureVisible( hwnd, dgList->dg_SelectedRow, dgList->dg_SelectedColumn );
					}
					/* KEY_PAGEUP pressed */
					else if ( INT(wParam) == 33 )
					{
						// Scroll page up
						INT scrollCode = SB_PAGEUP;
						SHORT pos = GetScrollPos( hwnd, SB_VERT );
						SendMessage( hwnd, WM_VSCROLL, MAKEWPARAM(scrollCode,pos), (LPARAM)NULL );
					}
					/* KEY_PAGEDOWN pressed */
					else if ( INT(wParam) == 34 )
					{
						// Scroll page up
						INT scrollCode = SB_PAGEDOWN;
						SHORT pos = GetScrollPos( hwnd, SB_VERT );
						SendMessage( hwnd, WM_VSCROLL, MAKEWPARAM(scrollCode,pos), (LPARAM)NULL );
					}
					
					// Send notification to the parent window
					SendMessage( dgList->dg_hParent, WM_COMMAND, MAKEWPARAM(0, IDataGrid::DGM_ITEMCHANGED), (LPARAM)hwnd );
					
					InvalidateRect( hwnd, NULL, TRUE );
					UpdateWindow(hwnd);
				}
			}
		}
		break;
		
	default:
		return DefWindowProc( hwnd, message, wParam, lParam );
	}

	return 0;
}


void DGManager::DrawColumns(HWND hWnd)
{
	INT offsetX=0, offsetY=0;
	RECT rect;
	SIZE size;
	TEXTMETRIC tm;
	DRAWTEXTPARAMS dtp;
	dtp.cbSize = sizeof(DRAWTEXTPARAMS);
	dtp.iLeftMargin = 5;
	dtp.iRightMargin = 5;
	dtp.iTabLength = 0;
	
	// Find DataGrid
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	HDC hDC = dgList->dg_hMemDC;
	
	INT scrollX = GetScrollPos( hWnd, SB_HORZ );
	offsetX -= scrollX;
	
	HFONT hOldFont = (HFONT)SelectObject( hDC, dgList->dg_hColumnFont );
	COLORREF oldTxtColor = SetTextColor( hDC, dgList->dg_ColumnTextColor );
	SetBkMode( hDC, TRANSPARENT );
	
	for ( INT i=0; i<dgList->dg_ColumnNumber; i++ )
	{
		// Get column text dimensions
		GetTextExtentPoint( hDC, dgList->dg_Columns[i].columnText, strlen(dgList->dg_Columns[i].columnText), &size );
		GetTextMetrics( hDC, &tm );
		
		// Set column rectangle
		rect.left = offsetX;
		rect.top = offsetY;
		rect.right = rect.left + dgList->dg_Columns[i].columnWidth;
		rect.bottom = rect.top + size.cy + tm.tmInternalLeading;
		offsetX = rect.right;
		
		if ( dgList->dg_EnableSort )
		{
			// Draw sorting column
			if ( !dgList->dg_Columns[i].pressed )
				DrawFrameControl( hDC, &rect, DFC_BUTTON, DFCS_BUTTONPUSH );
			else
				DrawFrameControl( hDC, &rect, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_FLAT );
		}
		else
		{
			// Draw non-sorting column
			rect.bottom += 2;
			DrawEdge( hDC, &rect, EDGE_ETCHED, BF_RECT | BF_MIDDLE | BF_ADJUST );
		}
		
		// Draw column text
		DrawTextEx( hDC, dgList->dg_Columns[i].columnText, strlen(dgList->dg_Columns[i].columnText), &rect, dgList->dg_Columns[i].textAlign | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS, &dtp );
	}
	
	SetTextColor( hDC, oldTxtColor );
	SetBkMode( hDC, OPAQUE );
	SelectObject( hDC, hOldFont );
}


void DGManager::DrawRows(HWND hWnd)
{
	INT offsetX=0, offsetY=0, offY=0;
	RECT rect, colRect, clientRect;
	SIZE size;
	TEXTMETRIC tm;
	DRAWTEXTPARAMS dtp;
	dtp.cbSize = sizeof(DRAWTEXTPARAMS);
	dtp.iLeftMargin = 5;
	dtp.iRightMargin = 5;
	dtp.iTabLength = 0;
	
	// Find DataGrid
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	HDC hDC = dgList->dg_hMemDC;
	
	GetClientRect( hWnd, &clientRect );
	
	INT scrollX = GetScrollPos( hWnd, SB_HORZ );
	offsetX -= scrollX+1;
	INT scrollY = GetScrollPos( hWnd, SB_VERT );
	offY -= scrollY;
	
	// Get column rectangle
	DGManager::GetColumnRect( hWnd, 0, &colRect );
	offsetY += (colRect.bottom - colRect.top) - 1;
	
	HFONT hOldFont = (HFONT)SelectObject( hDC, dgList->dg_hRowFont );
	HPEN hOldPen;
	HPEN hBgPen = CreatePen( PS_SOLID, 1, IDataGrid::DG_BGR_COLOR );
	if ( dgList->dg_EnableGrid )
		hOldPen = (HPEN)SelectObject( hDC, dgList->dg_hCellPen );
	else
		hOldPen = (HPEN)SelectObject( hDC, hBgPen );
	COLORREF oldTxtColor = SetTextColor( hDC, dgList->dg_RowTextColor );
	SetBkMode( hDC, TRANSPARENT );
	
	// Get row clipping boundaries
	RECT rRect, cRect;
	DGManager::GetRowRect( hWnd, 0, &rRect );
	GetClientRect( hWnd, &cRect );
	INT rowOffsetT = scrollY / (rRect.bottom-rRect.top-1);
	if ( ( scrollY % (rRect.bottom-rRect.top-1) != 0 ) && ( rowOffsetT > 0 ) )
		rowOffsetT--;
	offY += rowOffsetT*(rRect.bottom-rRect.top-1);
	INT rowOffsetB = (scrollY + cRect.bottom-cRect.top) / (rRect.bottom-rRect.top-1);
	if ( rowOffsetB > dgList->dg_RowNumber )
		rowOffsetB = dgList->dg_RowNumber;
	
	for ( INT i=rowOffsetT; i<rowOffsetB; i++ )
	{
		for ( INT j=0; j<dgList->dg_ColumnNumber; j++ )
		{
			// Get column rectangle
			DGManager::GetColumnRect( hWnd, j, &colRect );
			
			// Get row text dimensions
			GetTextExtentPoint( hDC, dgList->dg_Rows[i].rowText[j], strlen(dgList->dg_Rows[i].rowText[j]), &size );
			GetTextMetrics( hDC, &tm );
			
			// Set row rectangle
			rect.left = offsetX;
			rect.top = offsetY + offY;
			rect.right = rect.left + (colRect.right-colRect.left+1);
			rect.bottom = rect.top + size.cy + tm.tmInternalLeading;
			offsetX = rect.right - 1;
			// If item is full or partially visible
			if ( ( ( rect.top >= offsetY ) && ( rect.bottom <= clientRect.bottom ) ) || 
				( ( rect.top < clientRect.bottom ) && ( rect.bottom > clientRect.bottom ) ) || 
				( ( rect.top < offsetY ) && ( rect.bottom > offsetY ) ) )
			{
				// Check if row is selected
				if ( dgList->dg_Rows[i].selected == false )
				{
					// Draw row
					HBRUSH hBgBrush;
					if ( !dgList->dg_Rows[i].readOnly[j] )
						hBgBrush = CreateSolidBrush(dgList->dg_Rows[i].bgColor);
					else
						hBgBrush = CreateSolidBrush(IDataGrid::DG_RONLY_COLOR);
					HBRUSH hOldBrush = (HBRUSH)SelectObject( hDC, hBgBrush );
					Rectangle( hDC, rect.left, rect.top, rect.right, rect.bottom );
					
					SelectObject( hDC, hOldBrush );
					DeleteObject(hBgBrush);
					// Draw row text
					DrawTextEx( hDC, dgList->dg_Rows[i].rowText[j], strlen(dgList->dg_Rows[i].rowText[j]), &rect, dgList->dg_Rows[i].textAlign[j] | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS, &dtp );
				}
				else
				{
					// Draw row
					HBRUSH hSelectionBrush = CreateSolidBrush(RGB(220,230,250));
					HBRUSH hOldBrush = (HBRUSH)SelectObject( hDC, hSelectionBrush );
					Rectangle( hDC, rect.left, rect.top, rect.right, rect.bottom );
					SelectObject( hDC, hOldBrush );
					DeleteObject(hSelectionBrush);
					// Draw row text
					COLORREF oldTextColor = SetTextColor( hDC, RGB(130,130,130) );
					DrawTextEx( hDC, dgList->dg_Rows[i].rowText[j], strlen(dgList->dg_Rows[i].rowText[j]), &rect, dgList->dg_Rows[i].textAlign[j] | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS, &dtp );
					SetTextColor( hDC, oldTextColor );
				}
				
				if ( ( j == dgList->dg_SelectedColumn ) && ( i == dgList->dg_SelectedRow ) )
				{
					// Draw focus
					RECT focusRect = rect;
					focusRect.left += 1;
					focusRect.right -= 1;
					focusRect.top += 1;
					focusRect.bottom -= 1;
					DrawFocusRect( hDC, &focusRect );
				}
			}
		}
		
		offsetX = -scrollX-1;
		offY += (rect.bottom - rect.top) - 1;
	}
	
	DeleteObject(hBgPen);
	SetTextColor( hDC, oldTxtColor );
	SetBkMode( hDC, OPAQUE );
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldFont );
}


BOOL DGManager::CheckColumnResize(HWND hWnd, INT x, INT y, INT* col, RECT* colRect)
{
	BOOL result = FALSE;
	
	RECT rect;
	SIZE size;
	INT offsetX=0, offsetY=0;
	TEXTMETRIC tm;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	HDC hDC = GetDC(hWnd);
	
	INT scrollY = GetScrollPos( hWnd, SB_VERT );
	offsetY += scrollY;
	
	HFONT hOldFont = (HFONT)SelectObject( hDC, dgList->dg_hColumnFont );
	dgList->dg_Cursor = FALSE;
	for ( INT i=0; i<dgList->dg_ColumnNumber; i++ )
	{
		GetTextExtentPoint( hDC, dgList->dg_Columns[i].columnText, strlen(dgList->dg_Columns[i].columnText), &size );
		GetTextMetrics( hDC, &tm );
		
		rect.left = offsetX;
		rect.top = offsetY;
		rect.right = rect.left + dgList->dg_Columns[i].columnWidth;
		rect.bottom = rect.top + size.cy + tm.tmInternalLeading;
		offsetX = rect.right;
		
		// Check mouse position
		if ( ( abs(rect.right-x) < 3 ) && ( rect.top <= y ) && ( rect.bottom >= y ) )
		{
			*col = i;
			*colRect = rect;
			result = TRUE;
			break;
		}
	}
	SelectObject( hDC, hOldFont );
	
	ReleaseDC( hWnd, hDC );
	
	return result;
}


BOOL DGManager::CheckColumnClick(HWND hWnd, INT x, INT y, INT* col)
{
	BOOL result = FALSE;
	
	POINT pt = {x, y};
	RECT rect;
	SIZE size;
	INT offsetX=0, offsetY=0;
	TEXTMETRIC tm;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	HDC hDC = GetDC(hWnd);
	*col = -1;
	
	INT scrollY = GetScrollPos( hWnd, SB_VERT );
	offsetY += scrollY;
	
	HFONT hOldFont = (HFONT)SelectObject( hDC, dgList->dg_hColumnFont );
	dgList->dg_Cursor = FALSE;
	for ( INT i=0; i<dgList->dg_ColumnNumber; i++ )
	{
		GetTextExtentPoint( hDC, dgList->dg_Columns[i].columnText, strlen(dgList->dg_Columns[i].columnText), &size );
		GetTextMetrics( hDC, &tm );
		
		rect.left = offsetX;
		rect.top = offsetY;
		rect.right = rect.left + dgList->dg_Columns[i].columnWidth;
		rect.bottom = rect.top + size.cy + tm.tmInternalLeading;
		offsetX = rect.right;
		
		// Check mouse position
		if ( PtInRect( &rect, pt ) )
		{
			*col = i;
			result = TRUE;
			break;
		}
	}
	SelectObject( hDC, hOldFont );
	
	ReleaseDC( hWnd, hDC );
	
	return result;
}


void DGManager::GetColumnRect(HWND hWnd, INT col, RECT* colRect)
{
	SIZE size;
	INT offsetX=0, offsetY=0;
	TEXTMETRIC tm;
	
	// Find DataGrid
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	HDC hDC = GetDC(hWnd);
	
	HFONT hOldFont = (HFONT)SelectObject( hDC, dgList->dg_hColumnFont );
	for ( INT i=0; i<dgList->dg_ColumnNumber; i++ )
	{
		GetTextExtentPoint( hDC, dgList->dg_Columns[i].columnText, strlen(dgList->dg_Columns[i].columnText), &size );
		GetTextMetrics( hDC, &tm );
		
		if ( i == col )
		{
			colRect->left = offsetX;
			colRect->top = offsetY;
			colRect->right = colRect->left + dgList->dg_Columns[i].columnWidth;
			colRect->bottom = colRect->top + size.cy + tm.tmInternalLeading;
			if ( !dgList->dg_EnableSort )
				colRect->bottom += 2;
			break;
		}
		
		offsetX += dgList->dg_Columns[i].columnWidth;
	}
	SelectObject( hDC, hOldFont );
	
	ReleaseDC( hWnd, hDC );
}


void DGManager::GetRowRect(HWND hWnd, INT row, RECT* rowRect)
{
	INT offsetX=0, offsetY=0;
	RECT colRect;
	SIZE size;
	TEXTMETRIC tm;
	DRAWTEXTPARAMS dtp;
	dtp.cbSize = sizeof(DRAWTEXTPARAMS);
	dtp.iLeftMargin = 5;
	dtp.iRightMargin = 5;
	dtp.iTabLength = 0;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	HDC hDC = GetDC(hWnd);
	
	INT scrollX = GetScrollPos( hWnd, SB_HORZ );
	offsetX -= scrollX;
	
	// Get column rectangle
	GetColumnRect( hWnd, 0, &colRect );
	offsetY += (colRect.bottom - colRect.top) - 1;
	
	HFONT hOldFont = (HFONT)SelectObject( hDC, dgList->dg_hRowFont );
	HPEN hOldPen = (HPEN)SelectObject( hDC, dgList->dg_hCellPen );
	SetBkMode( hDC, TRANSPARENT );
	
	for ( INT i=0; i<dgList->dg_RowNumber; i++ )
	{
		// Get row text dimensions
		GetTextExtentPoint( hDC, dgList->dg_Rows[i].rowText[0], strlen(dgList->dg_Rows[i].rowText[0]), &size );
		GetTextMetrics( hDC, &tm );
		
		if ( i == row )
		{
			// Get row rectangle
			rowRect->left = offsetX;
			rowRect->top = offsetY;
			rowRect->right = rowRect->left + dgList->dg_Columns[0].columnWidth;
			rowRect->bottom = rowRect->top + size.cy + tm.tmInternalLeading;
			break;
		}
		
		offsetY += (rowRect->bottom - rowRect->top) - 1;
	}
	SetBkMode( hDC, OPAQUE );
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldFont );
	
	ReleaseDC( hWnd, hDC );
}


BOOL DGManager::CheckRows(HWND hWnd, INT x, INT y, INT* row)
{
	BOOL result = FALSE;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	HDC hDC = GetDC(hWnd);
	*row = -1;
	
	if ( dgList->dg_Resize )
		return result;
	
	// Clear selection
	dgList->dg_Selected = NULL;
	dgList->dg_SelectedRow = -1;
	dgList->dg_SelectedColumn = -1;
	
	INT offsetX=0, offsetY=0, offY=0;
	RECT rect, colRect;
	SIZE size;
	TEXTMETRIC tm;
	DRAWTEXTPARAMS dtp;
	dtp.cbSize = sizeof(DRAWTEXTPARAMS);
	dtp.iLeftMargin = 5;
	dtp.iRightMargin = 5;
	dtp.iTabLength = 0;
	
	INT scrollX = GetScrollPos( hWnd, SB_HORZ );
	offsetX -= scrollX;
	INT scrollY = GetScrollPos( hWnd, SB_VERT );
	offY -= scrollY;
	
	POINT pt = {x-scrollX, y};
	
	// Get column rectangle
	GetColumnRect( hWnd, 0, &colRect );
	offsetY += (colRect.bottom - colRect.top) - 1;
	
	HFONT hOldFont = (HFONT)SelectObject( hDC, dgList->dg_hRowFont );
	HPEN hOldPen = (HPEN)SelectObject( hDC, dgList->dg_hCellPen );
	SetBkMode( hDC, TRANSPARENT );
	
	BOOL found = false;
	CDataGrid::_Trow *pCur = dgList->dg_Rows;
	INT rowInd = 0;
	for ( INT j=0; j<dgList->dg_RowNumber; j++ )
	{
		// Reset row selection flag
		dgList->dg_Rows[j].selected = false;
		
		for ( INT i=0; i<dgList->dg_ColumnNumber; i++ )
		{
			// Get row text dimensions
			GetTextExtentPoint( hDC, dgList->dg_Rows[j].rowText[i], strlen(dgList->dg_Rows[j].rowText[i]), &size );
			GetTextMetrics( hDC, &tm );
			
			// Set row rectangle
			rect.left = offsetX;
			rect.top = offsetY;
			rect.right = rect.left + dgList->dg_Columns[i].columnWidth;
			rect.bottom = rect.top + size.cy + tm.tmInternalLeading;
			offsetX = rect.right - 1;
			
			// Check if row is selected
			if ( ( PtInRect( &rect, pt ) ) && ( found == false ) )
			{
				*row = rowInd;
				dgList->dg_Rows[j].selected = true;
				found = true;
				
				// Mark selection
				dgList->dg_Selected = &dgList->dg_Rows[j];
				dgList->dg_SelectedRow = j;
				dgList->dg_SelectedColumn = i;
				
				result = TRUE;
			}
		}
		
		offsetX = -scrollX;
		offsetY += (rect.bottom - rect.top) - 1;
		rowInd++;
	}
	
	SetBkMode( hDC, OPAQUE );
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldFont );
	
	ReleaseDC( hWnd, hDC );
	
	return result;
}


void DGManager::RecalcWindow(HWND hWnd)
{
	INT sizeX = 0;
	INT sizeY = 0;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);

	if(dgList)
	{
		// Get DataGrid window rectangle
		RECT rect;
		GetClientRect( hWnd, &rect );
		
		for ( INT i=0; i<dgList->dg_ColumnNumber; i++ )
		{
			// Calculate total columns width
			sizeX += dgList->dg_Columns[i].columnWidth;
		}
		
		// Check horizontal scroll bar visibility
		INT scrollDiff = (rect.right - rect.left) - sizeX;
		if ( scrollDiff < 0 )
		{
			
			// Show horizontal scroll bar
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_ALL;
			si.nPos = GetScrollPos( hWnd, SB_HORZ );
			si.nMin = 0;
			si.nMax = sizeX;
			si.nPage = si.nMax - si.nMin + 1 - abs(scrollDiff);
			si.nTrackPos = 0;
			SetScrollInfo( hWnd, SB_HORZ, &si, TRUE );
			ShowScrollBar( hWnd, SB_HORZ, TRUE );
		}
		else
			ShowScrollBar( hWnd, SB_HORZ, FALSE );
		
		if ( dgList->dg_RowNumber > 0 )
		{
			RECT colRect, rowRect;
			DRAWTEXTPARAMS dtp;
			dtp.cbSize = sizeof(DRAWTEXTPARAMS);
			dtp.iLeftMargin = 5;
			dtp.iRightMargin = 5;
			dtp.iTabLength = 0;
			// Get column rectangle
			GetColumnRect( hWnd, 0, &colRect );
			sizeY += (colRect.bottom - colRect.top) - 1;
			// Get row rectangle
			GetRowRect( hWnd, 0, &rowRect );
			sizeY += dgList->dg_RowNumber * (rowRect.bottom-rowRect.top-1) + 1;
			
			// Check vertical scroll bar visibility
			scrollDiff = (rect.bottom - rect.top) - sizeY;
			if ( scrollDiff < 0 )
			{
				// Show vertical scroll bar
				SCROLLINFO si;
				si.cbSize = sizeof(SCROLLINFO);
				si.fMask = SIF_ALL;
				si.nPos = GetScrollPos( hWnd, SB_VERT );
				si.nMin = 0;
				si.nMax = sizeY;
				si.nPage = si.nMax - si.nMin + 1 - abs(scrollDiff);
				si.nTrackPos = 0;
				SetScrollInfo( hWnd, SB_VERT, &si, TRUE );
				ShowScrollBar( hWnd, SB_VERT, TRUE );
			}
			else
				ShowScrollBar( hWnd, SB_VERT, FALSE );
		}
		else
			ShowScrollBar( hWnd, SB_VERT, FALSE );
	}
}


void DGManager::SortDGItems(HWND hWnd, INT column)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);

	if(dgList && dgList->dg_CompareFunc)
	{
		// Send notification to the parent window
		SendMessage( dgList->dg_hParent, WM_COMMAND, MAKEWPARAM(0, IDataGrid::DGM_STARTSORTING), (LPARAM)hWnd );
		
		// Sort DataGrid items
		DGManager::Sort( hWnd, column, dgList->dg_RowNumber );
		
		// Send notification to the parent window
		SendMessage( dgList->dg_hParent, WM_COMMAND, MAKEWPARAM(0, IDataGrid::DGM_ENDSORTING), (LPARAM)hWnd );
		
		// Set selection focus
		DGManager::SetDGSelection( hWnd, dgList->dg_SelectedRow, dgList->dg_SelectedColumn );
		InvalidateRect( hWnd, NULL, TRUE );
		UpdateWindow(hWnd);
	}
}


void DGManager::Sort(HWND hWnd, INT col, INT size)
{
	INT i;
	CDataGrid::_Trow temp;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	for ( i=(size+1/2); i>=0; i-- )
		DGManager::SiftDown( hWnd, i, size-1, col );
	
	for ( i=size-1; i>=1; i-- )
	{
		memcpy( &temp, &dgList->dg_Rows[0], sizeof(CDataGrid::_Trow) );
		memcpy( &dgList->dg_Rows[0], &dgList->dg_Rows[i], sizeof(CDataGrid::_Trow) );
		memcpy( &dgList->dg_Rows[i], &temp, sizeof(CDataGrid::_Trow) );
		DGManager::SiftDown( hWnd, 0, i-1, col );
	}
}


void DGManager::SiftDown( HWND hWnd, INT root, INT bottom, INT col )
{
	INT done, maxChild;
	CDataGrid::_Trow temp;
	
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	done = 0;
	while ( (root*2 < bottom) && (!done) )
	{
		if ( root*2 == bottom )
			maxChild = root * 2;
		else if ( root*2 < bottom )
		{
			if ( dgList->dg_CompareFunc(dgList->dg_Rows[root*2].rowText[col], dgList->dg_Rows[root*2+1].rowText[col], col) > 0 )
				maxChild = root * 2;
			else
				maxChild = root * 2 + 1;
			
			if ( dgList->dg_CompareFunc(dgList->dg_Rows[root].rowText[col], dgList->dg_Rows[maxChild].rowText[col], col) < 0 )
			{
				memcpy( &temp, &dgList->dg_Rows[root], sizeof(CDataGrid::_Trow) );
				memcpy( &dgList->dg_Rows[root], &dgList->dg_Rows[maxChild], sizeof(CDataGrid::_Trow) );
				memcpy( &dgList->dg_Rows[maxChild], &temp, sizeof(CDataGrid::_Trow) );
				root = maxChild;
			}
			else
				done = 1;
		}
		else
			done = 1;
	}
}


void DGManager::SetDGSelection(HWND hWnd, INT rowIndex, INT columnIndex)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	BOOL found = false;
	for ( INT j=0; j<dgList->dg_RowNumber; j++ )
	{
		if ( ( dgList->dg_Rows[j].selected ) && ( !found ) )
		{
			// Mark selection
			dgList->dg_Selected = &dgList->dg_Rows[j];
			dgList->dg_SelectedRow = j;
			dgList->dg_SelectedColumn = columnIndex;
			found = true;
			break;
		}
		
		if ( found )
			break;
	}
}


void DGManager::SelectNextItem(HWND hWnd, CDataGrid::_Trow* item)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	for ( INT i=0; i<dgList->dg_RowNumber; i++ )
	{
		if ( ( (&dgList->dg_Rows[i]) == dgList->dg_Selected ) && ( i < dgList->dg_RowNumber-1 ) )
		{
			// Set selected item
			dgList->dg_Rows[i].selected = false;
			dgList->dg_Rows[i+1].selected = true;
			dgList->dg_Selected = &dgList->dg_Rows[i+1];
			dgList->dg_SelectedRow = i+1;
			break;
		}
	}
}


void DGManager::SelectPreviousItem(HWND hWnd, CDataGrid::_Trow* item)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	for ( INT i=0; i<dgList->dg_RowNumber; i++ )
	{
		if ( ( (&dgList->dg_Rows[i]) == dgList->dg_Selected ) && ( i > 0 ) )
		{
			// Set selected item
			dgList->dg_Rows[i].selected = false;
			dgList->dg_Rows[i-1].selected = true;
			dgList->dg_Selected = &dgList->dg_Rows[i-1];
			dgList->dg_SelectedRow = i-1;
			break;
		}
	}
}


void DGManager::SelectNextSubitem(HWND hWnd)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	// Set selected subitem
	if ( dgList->dg_SelectedColumn < dgList->dg_ColumnNumber-1 )
		dgList->dg_SelectedColumn++;
	else
		dgList->dg_SelectedColumn = dgList->dg_ColumnNumber-1;   
}


void DGManager::SelectPreviousSubitem(HWND hWnd)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	
	// Set selected subitem
	if ( dgList->dg_SelectedColumn > 0 )
		dgList->dg_SelectedColumn--;
	else
		dgList->dg_SelectedColumn = 0;
}


void DGManager::EnsureVisible(HWND hWnd, INT rowIndex, INT colIndex)
{
	// Ensure DataGrid item is visible
	DGManager::EnsureRowVisible( hWnd, rowIndex );
	DGManager::EnsureColumnVisible( hWnd, colIndex );
}


void DGManager::EnsureRowVisible(HWND hWnd, INT rowIndex)
{
	// Check DataGrid row visibility
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	RECT rowRect, clientRect, columnRect;
	GetClientRect( hWnd, &clientRect );
	GetColumnRect( hWnd, 0, &columnRect );
	INT clientHeight = clientRect.bottom - clientRect.top;
	GetRowRect( hWnd, 0, &rowRect );
	INT rowOffsetT = dgList->dg_SelectedRow*(rowRect.bottom-rowRect.top-1);
	INT rowOffsetB = dgList->dg_SelectedRow*(rowRect.bottom-rowRect.top-1) + (columnRect.bottom-columnRect.top-1);
	INT scrollY = GetScrollPos( hWnd, SB_VERT );
	if ( scrollY > rowOffsetT )
		SetScrollPos( hWnd, SB_VERT, rowOffsetT, TRUE );
	else if ( scrollY+clientHeight < rowOffsetB+(rowRect.bottom - rowRect.top) )
	{
		INT pos = rowOffsetB + (rowRect.bottom - rowRect.top) - (scrollY+clientHeight);
		SetScrollPos( hWnd, SB_VERT, scrollY+pos, TRUE );
	}
}


void DGManager::EnsureColumnVisible(HWND hWnd, INT columnIndex)
{
	// Check DataGrid column visibility
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);
	RECT columnRect, clientRect;
	GetClientRect( hWnd, &clientRect );
	INT scrollX = GetScrollPos( hWnd, SB_HORZ );
	INT columnOffsetL = 0;
	for ( INT i=0; i<dgList->dg_SelectedColumn; i++ )
	{
		GetColumnRect( hWnd, i, &columnRect );
		columnOffsetL += (columnRect.right-columnRect.left);
	}
	GetColumnRect( hWnd, dgList->dg_SelectedColumn, &columnRect );
	INT columnOffsetR = columnOffsetL + (columnRect.right-columnRect.left);
	if ( scrollX + clientRect.left > columnOffsetL )
	{
		INT pos = scrollX + clientRect.left - columnOffsetL;
		SetScrollPos( hWnd, SB_HORZ, columnOffsetL, TRUE );
	}
	else if ( scrollX+clientRect.right < columnOffsetR )
	{
		INT pos = columnOffsetR - (scrollX+clientRect.right);
		SetScrollPos( hWnd, SB_HORZ, scrollX+pos, TRUE );
	}
}


BOOL DGManager::AddDGGrid(HWND hWnd, HWND hParent)
{
	BOOL result = FALSE;
	
	// Check number of created DataGrids
	if ( g_DGGridNumber < IDataGrid::DG_MAXGRID )
	{
		// Create new DataGrid
		CDataGrid::_Tlist* newDGGrid = new CDataGrid::_Tlist;
		newDGGrid->dg_hWnd = hWnd;
		newDGGrid->dg_hParent = hParent;
		newDGGrid->dg_Columns = NULL;
		newDGGrid->dg_Rows = NULL;
		newDGGrid->dg_ColumnNumber = 0;
		newDGGrid->dg_RowNumber = 0;
		newDGGrid->dg_Column = -1;
		newDGGrid->dg_Row = -1;
		newDGGrid->dg_SelectedColumn = -1;
		newDGGrid->dg_SelectedRow = -1;
		newDGGrid->dg_Selected = NULL;
		newDGGrid->dg_Cursor = FALSE;
		newDGGrid->dg_Resize = FALSE;
		newDGGrid->dg_Click = FALSE;
		newDGGrid->dg_hColumnFont = NULL;
		newDGGrid->dg_hRowFont = NULL;
		newDGGrid->dg_hBgBrush = NULL;
		newDGGrid->dg_hCellPen = NULL;
		newDGGrid->dg_hMemBitmap = NULL;
		newDGGrid->dg_hMemDC = NULL;
		newDGGrid->dg_hOldMemBitmap = NULL;
		newDGGrid->dg_Edit = FALSE;
		newDGGrid->dg_hEditWnd = NULL;
		newDGGrid->dg_EnableEdit = TRUE;
		newDGGrid->dg_EnableSort = TRUE;
		newDGGrid->dg_EnableResize = TRUE;
		newDGGrid->dg_EnableGrid = TRUE;
		newDGGrid->dg_CompareFunc = NULL;
		newDGGrid->dg_ColumnTextColor = IDataGrid::DG_TXT_COLOR;
		newDGGrid->dg_RowTextColor = IDataGrid::DG_TXT_COLOR;
		newDGGrid->next = NULL;
		
		CDataGrid::_Tlist* pCur = g_DGList;
		if ( pCur == NULL )
			g_DGList = newDGGrid;
		else
		{
			while ( pCur->next)
				pCur = pCur->next;
			
			pCur->next = newDGGrid;
		}
		
		g_DGGridNumber++;
	}
	
	return result;
}


void DGManager::DestroyDGGrid(HWND hWnd)
{
	CDataGrid::_Tlist* dgList = DGManager::DetachDGGrid(hWnd);

	if(dgList)
	{
		// Delete columns
		if ( dgList->dg_Columns )
		{
			delete dgList->dg_Columns;
			dgList->dg_Columns = NULL;
		}
		// Delete rows
		free(dgList->dg_Rows);
		dgList->dg_Rows = NULL;
		// Delete column font
		if ( dgList->dg_hColumnFont )
		{
			DeleteObject(dgList->dg_hColumnFont);
			dgList->dg_hColumnFont = NULL;
		}
		// Delete row font
		if ( dgList->dg_hRowFont )
		{
			DeleteObject(dgList->dg_hRowFont);
			dgList->dg_hRowFont = NULL;
		}
		// Delete background brush
		if ( dgList->dg_hBgBrush )
		{
			DeleteObject(dgList->dg_hBgBrush);
			dgList->dg_hBgBrush = NULL;
		}
		// Delete cell pen
		if ( dgList->dg_hCellPen )
		{
			DeleteObject(dgList->dg_hCellPen);
			dgList->dg_hCellPen = NULL;
		}
		// Delete memory device context
		if ( dgList->dg_hMemDC )
		{
			SelectObject( dgList->dg_hMemDC, dgList->dg_hOldMemBitmap );
			DeleteDC(dgList->dg_hMemDC);
			dgList->dg_hMemDC = NULL;
		}
		// Delete memory bitmap
		if ( dgList->dg_hMemBitmap )
		{
			DeleteObject(dgList->dg_hMemBitmap);
			dgList->dg_hMemBitmap = NULL;
		}
		
		g_DGGridNumber--;
	}
}


CDataGrid::_Tlist* DGManager::GetDGGrid(HWND hWnd)
{
	CDataGrid::_Tlist* result = NULL;
	
	if ( g_DGGridNumber > 0 )
	{
		// Find DataGrid
		CDataGrid::_Tlist *dgList = g_DGList;

		while ( dgList && ( dgList->dg_hWnd != hWnd ) )
			dgList = dgList->next;   
		
		result = dgList;
	}
	
	return result;
}


CDataGrid::_Tlist* DGManager::DetachDGGrid(HWND hWnd)
{
	CDataGrid::_Tlist* result = NULL;
	
	if ( g_DGGridNumber > 0 )
	{
		// Find DataGrid
		CDataGrid::_Tlist *pCur, *prev;
		pCur = g_DGList;

		while ( pCur && ( pCur->dg_hWnd != hWnd ) )
		{
			prev = pCur;
			pCur = pCur->next;
		}
		
		if ( pCur == g_DGList )
			g_DGList = g_DGList->next;
		else
			prev->next = pCur->next;    
		
		result = pCur;
	}
	
	return result;
}


void DGManager::GetCellRect(HWND hWnd, RECT* cellRect)
{
	CDataGrid::_Tlist* dgList = DGManager::GetDGGrid(hWnd);

	if(dgList)
	{
		RECT rowRect, columnRect;
		GetColumnRect( hWnd, dgList->dg_SelectedColumn, &columnRect );
		GetRowRect( hWnd, 0, &rowRect );
		INT rowOffset = dgList->dg_SelectedRow * (rowRect.bottom-rowRect.top-1) + (columnRect.bottom-columnRect.top-1);
		cellRect->left = columnRect.left;
		cellRect->right = cellRect->left + (columnRect.right-columnRect.left) - 1;
		cellRect->top = rowOffset + 1;
		cellRect->bottom = cellRect->top + (rowRect.bottom-rowRect.top-1) - 1;
	}
}













INT		LcUtil_CreateDataGrid(char* sCmd
							 , IDataGrid** pData
							 , HWND p1			// Parent HWND
							 , RECT* p2			// RECT
							 , INT	p3			// numCols
							 )
{
	CDataGrid* p = new CDataGrid;
	
	if(FALSE == p->Create(p1, *p2, p3))
	{
		delete p;
		return -1;
	}

	*pData = p;

	return 0;
}
