// DataGrid Control
// http://www.codeguru.com/cpp/controls/controls/gridcontrol/article.php/c10319/
// Created by darkoman	August 17, 2005
// Update Heesung Oh 2009/02/10
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DataGrid_darkoman_August_17_2005_codeguru_com_H_
#define DataGrid_darkoman_August_17_2005_codeguru_com_H_

#ifndef interface
#define interface struct
#endif


#ifndef LC_CLASS_DESTROYER
#define LC_CLASS_DESTROYER( CLASS_NAME )	\
virtual ~CLASS_NAME(){}
#endif

interface IDataGrid
{
	LC_CLASS_DESTROYER(	IDataGrid	);

	// DataGrid definitions
	enum
	{
		DG_MAXGRID		= 20,
		DG_MAXCOLUMN	= 1000,
		DG_MAXROW		= 32000,
		
		DG_BGR_COLOR	= RGB(255,255,255),
		DG_TXT_COLOR	= RGB(0,0,0),
		DG_RONLY_COLOR	= RGB(220,220,220),

		DG_LEFT			= DT_LEFT,
		DG_CENTER		= DT_CENTER,
		DG_RIGHT		= DT_RIGHT,
	};

	// DataGrid messages
	enum
	{
		DGM_ITEMCHANGED		= 0x0001,
		DGM_ITEMTEXTCHANGED	= 0x0002,
		DGM_ITEMADDED		= 0x0003,
		DGM_ITEMREMOVED		= 0x0004,
		DGM_COLUMNRESIZED	= 0x0005,
		DGM_COLUMNCLICKED	= 0x0006,
		DGM_STARTSORTING	= 0x0007,
		DGM_ENDSORTING		= 0x0008,
	};

	// DataGrid enumerations
	enum
	{
		DG_TEXTEDIT,
		DG_TEXTALIGN,
		DG_TEXTHIGHLIGHT,
		DG_TEXTRONLY,
		DG_TEXTBGCOLOR
	};


	virtual BOOL	Create(HWND hParent, RECT wndRect, INT numCols)=0;
	virtual void	Destroy()=0;
	virtual void	Resize()=0;
	virtual void	Update()=0;
	virtual HWND	GetWindowHandle()=0;

	virtual BOOL	SetColumnInfo(INT columnIndex, char* columnText, INT columnWidth, INT textAlign)=0;
	virtual BOOL	SetItemInfo(INT rowIndex, INT columnIndex, char* itemText, INT textAlign, BOOL readOnly=FALSE)=0;
	virtual BOOL	InsertItem(char* itemText, INT textAlign)=0;
	virtual BOOL	RemoveItem(INT row)=0;

	
	// General DataGrid methods
	virtual void	RemoveAllItems()=0;
	virtual INT		GetRowNumber()=0;
	virtual INT		GetSelectedRow()=0;
	virtual INT		GetSelectedColumn()=0;
	virtual BOOL	GetItemText(INT row, INT column, char* buf, INT buf_size)=0;
};


INT		LcUtil_CreateDataGrid(char* sCmd
							 , IDataGrid** pData
							 , HWND p1			// Parent HWND
							 , RECT* p2			// RECT
							 , INT	p3			// numCols
							 );

#endif


