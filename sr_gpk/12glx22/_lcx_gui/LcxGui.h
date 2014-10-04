// Interface for the CLcxGui class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
  #pragma warning(disable: 4530)
  #pragma warning(disable: 4786)
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#ifndef _LcxGui_H_
#define _LcxGui_H_

#include <vector>

class CLcxGui;

typedef	std::vector<CLcxGui* >		lsPLCXGUI;
typedef	lsPLCXGUI::iterator			itPLCXGUI;
typedef	lsPLCXGUI::reverse_iterator	rtPLCXGUI;



class CLcxGui : public ILCX_Gui
{
public:
	static PLC_TEXTURE	m_pDtxDlg	;	//	Default Dialog Texture
	static INT			m_pDtxDlgN	;	//	Default Dialog Texture Use

	static PLC_TEXTURE	m_pDtxBtn	;	//	Default Button Texture
	static INT			m_pDtxBtnN	;	//	Default Button Texture Use

	static PLC_TEXTURE	m_pDtxStt	;	//	Default Static Texture
	static INT			m_pDtxSttN	;	//	Default Static Texture Use


protected:
	PLC_TEXTURE			m_pTexE;		//	Texture

public:
	// Distinguish ID
	UINT			m_nId;
	char			m_sName[LCXGUI_MAX_NAME];	// Gui Name

	//Gui Style and Width and Height
	INT             m_nWidth;		// Gui Width
	INT             m_nHeight;		// Gui Height

	// Event Region
	LCXRECT			m_rEvent;		// Gui Event(or Full Region) Rect(x, y, width, height). it has position
	LCXRECT			m_rTitle;		// Gui Menu(or Title) Rect. just only dialog
	LCXRECT			m_rBrcts[4];	// Gui button or static Rect. if button then 0: Normal 1: Over 2: Pressed
	LCXRECT			m_rRscts;		// Residue or Remain Rect.  for All Windows

	INT				m_Active;		// Active/non-active
	INT				m_State;		// State

	PLCX_GUI		m_Prnt;			// Parent Gui
	lsPLCXGUI		m_Chld;			// Child Gui

	TLCX_GUI_EVENT	m_Evnt;			// input Event List

	LCXVECTOR2		m_vcScl;		// tm Scaling
	//LCXVECTOR2	m_vcPos;		// tm Position is involved in m_rEvent
	LCXVECTOR3		m_vcRot;		// tm Rotation
	LCXVECTOR2		m_vcPosRef;		// tm Reference position
	LCXCOLOR		m_dcColor;		// diffuse color
	FLOAT			m_dcColorA;		// Color Alpha

	PLC_FONT		m_pFont;

public:
	CLcxGui();
	virtual ~CLcxGui();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();

	virtual INT		Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4);
	virtual void	Destroy();


	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);


	// Rendering Position
	virtual INT		SetPosition(const LC_FLOAT2* v);
	virtual INT		SetRotation(const LC_FLOAT3* v);
	virtual INT		SetScaling (const LC_FLOAT2* v);

	// Select Sprite Animation
	virtual INT		SetAnimation(const char*, const char*)	{	return LC_EFAIL;	}
	virtual INT		SetAnimation(const INT  , const INT  )	{	return LC_EFAIL;	}


	virtual INT		 Attach(PLCX_GUI);					// Attatch child Gui
	virtual INT		 Detatch(PLCX_GUI);					// Detatch child Gui
	virtual PLCX_GUI GetParent()	{ return m_Prnt; }	// return Parent Gui
	virtual PLCX_GUI GetRoot()
	{
		if(m_Prnt)
			return ((CLcxGui*)m_Prnt)->GetRoot();

		return this;
	}

	virtual INT		GetState()				{	return m_State;	}
	virtual void	SetState(INT v);
	virtual INT		SetText(const char* str,LC_COLORF* c=NULL);

	virtual	INT		SetupWithExtern(TLCX_GUI_EXTERN* ext){	return LC_EFAIL;	}
	virtual	void	SetExternTexture(PLC_TEXTURE pTex)	 {	m_pTexE = pTex;		}

public:
	void	SetParent(PLCX_GUI v)	{ m_Prnt = v;	 }

	const LCXRECT* GetRectEvent() const {	return &m_rEvent;	};
	const LCXRECT* GetRectTitle() const {	return &m_rTitle;	};
	const LCXRECT* GetRectResidue()const{	return &m_rRscts;	};


	LCXVECTOR2* GetPosition();
	LCXVECTOR2 GetPositionAbs();

	INT		SetPositionRef(FLOAT* pPos);

protected:
	UINT               GetId()				{	return m_nId;	}
	const char* const GetText() const
	{
		if(!m_pFont)	return NULL;
		return m_pFont->GetString();
	}
};



namespace UsGui
{


enum Eparse
{
	MAX_PARSE_LINE = 512,

	ELCXGUI_RSC	=0	,
	ELCXGUI_REC		,
	ELCXGUI_BUTTON	,
	ELCXGUI_SYSBTN	,
	ELCXGUI_DIALOG	,

	ELCXGUI_RCLRBT	,
	ELCXGUI_TEXTURE	,
	ELCXGUI_ID		,
	ELCXGUI_INDEX	,
};


struct TguiParse
{
	char	sKey[40];		// String Keword
	INT		iKey;			// String Size
};

struct TguiRec
{
	int		id;				// Id
	int		ix;				// index
	LCXRECT	rc;				// rect
};

struct TguiRscInf			// Gui Resource Info
{
	char	Name[32];		// Gui Name
	int		Id;
	int		Ix;
	int		Type;
	LCXRECT	Rts[32];		// Rect List

	std::vector<TguiRec > rec;
};


unsigned short* _LcxGuiGetRscDialog();
unsigned short* _LcxGuiGetRscSysBtn();

lsPLCXGUI*      _LcxGuiGetDlgList();
lsPLCXGUI*      _LcxGuiGetDlgRoot();

void            _LcxGuiSetDragDialog(CLcxGui*);
CLcxGui*       _LcxGuiGetDragDialog();

void            _LcxGuiSubstractRootDialog(CLcxGui*);

WORD*           _LcxGuiGetRscDialog();
WORD*           _LcxGuiGetRscSysBtn();

void            _LcxGuiSetEvent(TLCX_GUI_EVENT*);
TLCX_GUI_EVENT* _LcxGuiGetEvent();

void            _LcxGuiSortDialog(CLcxGui* pDlg, INT bFront=LC_TRUE);

INT             _LcxGui_Parse(std::vector<TguiRscInf >& GuiRscLst, char* Texture, char* sFile);
};


#endif

