// Implementation of the CLcxGui class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcAux.h>
#include <LcBase.h>
#include <LcMath.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
  #include "../ILCX_Gui.h"
#else
  #include "../LcxEntity.h"
#endif


#include "LcxGui.h"



////////////////////////////////////////////////////////////////////////////////
//

TLCX_GUI_INF::TLCX_GUI_INF()
{
	nId = -1;
	sName = NULL;
	sText = NULL;
	nType = -1;
	rEvnt = NULL;
	rTitle= NULL;
	rBrcs = NULL;
	rRscs = NULL;
	pPrnt = NULL;
}


TLCX_GUI_EVENT::TLCX_GUI_EVENT(PLCX_GUI G, INT T, INT M) : _o(G), _t(T), _m(M)
{
	memset(_sb, 0, 64 );
}

TLCX_GUI_EVENT::TLCX_GUI_EVENT(const TLCX_GUI_EVENT& r)
{
	_o = r._o; _t = r._t; _m = r._m;
	memcpy(_sb, r._sb, 64 );
}

TLCX_GUI_EVENT& TLCX_GUI_EVENT::operator=(const TLCX_GUI_EVENT& r)
{
	_o = r._o; _t = r._t; _m = r._m;
	memcpy(_sb, r._sb, 64 );
	return *this;
}

void TLCX_GUI_EVENT::SetUp(PLCX_GUI G,INT T,INT M){ _o = G; _t = T; _m = M;		}
void TLCX_GUI_EVENT::SetObject(PLCX_GUI v)	{	_o = v;							}
void TLCX_GUI_EVENT::SetType(UINT v)		{	_t = v;							}
void TLCX_GUI_EVENT::SetMessage(UINT v)		{	_m = v;							}
void TLCX_GUI_EVENT::SetVal(FLOAT* v,int N){	memcpy(_sf,v,sizeof(FLOAT)*N);	}
void TLCX_GUI_EVENT::SetVal(BYTE* v,int N)	{	memcpy(_sf,v,N);				}
void TLCX_GUI_EVENT::SetVal(DINT v)			{	_sn = v;						}
void TLCX_GUI_EVENT::ClearValue()			{	memset(_sb,0,64);				}
void TLCX_GUI_EVENT::ClearAll()
{
	_o=0; _t=0; _m=0;
	memset(_sb,0,64);
}



TLCX_GUI_EXTERN::TLCX_GUI_EXTERN() : txInf(0), rEvnt(0), rRcts(0){}


////////////////////////////////////////////////////////////////////////////////
PLC_TEXTURE CLcxGui::m_pDtxDlg  = NULL;	//	Default Dialog Texture
INT		     CLcxGui::m_pDtxDlgN = 0	;	//	Default Dialog Texture Use

PLC_TEXTURE CLcxGui::m_pDtxBtn  = NULL;	//	Default Button Texture
INT		     CLcxGui::m_pDtxBtnN = 0	;	//	Default Button Texture Use

PLC_TEXTURE CLcxGui::m_pDtxStt  = NULL;	//	Default Static Texture
INT		     CLcxGui::m_pDtxSttN = 0	;	//	Default Static Texture Use


CLcxGui::CLcxGui() : m_Chld(0)
{
	m_pTexE	= NULL;

	m_nId	= 0;
	memset(m_sName, 0, sizeof(char) * LCXGUI_MAX_NAME);

	m_nWidth	= 0;
	m_nHeight	= 0;

	memset(&m_rEvent,  0, sizeof(m_rEvent) );
	memset(&m_rTitle,  0, sizeof(m_rTitle) );
	memset( m_rBrcts,  0, sizeof(m_rBrcts) );
	memset(&m_rRscts,  0, sizeof(m_rRscts) );


	m_Active	= LCXGUI_DISABLE;
	m_State		= LCXGUI_SHOW;

	m_Prnt		= NULL;

	memset(&m_Evnt, 0, sizeof(TLCX_GUI_EVENT));


	m_vcScl		= LCXVECTOR2(1,1);
	m_vcRot		= LCXVECTOR3(0,0,0);
	m_vcPosRef	= LCXVECTOR2(0,0);
	m_dcColor	= LCXCOLOR(1, 1, 1, 1);
	m_dcColorA	= 1.0F;

	m_pFont		= NULL;
}



CLcxGui::~CLcxGui()
{
	Destroy();
}


INT CLcxGui::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}

INT CLcxGui::Release()
{
	INT hr = 0;

	LC_HANDLE pOwn = NULL;
	hr = this->Query(LCQUERY_GET_OWNER, &pOwn);
	if(LC_SUCCEEDED(hr))	// if there is owner then this cannot be released
		return LC_EFAIL;

	// Remove Dialog
	if(LCXGUI_DIALOG == this->GetRscType())
		LcxGui_RemoveDialog(this);

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcxGui::Destroy()
{
	int i;
	int iSize = m_Chld.size();


	for(i=0; i<iSize; ++i)
		m_Chld[i]->Release();

	m_Chld.clear();

	SAFE_RELEASE(	m_pFont	);
}


INT	CLcxGui::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	TLCX_GUI_INF* pInf = NULL;

	pInf		= (TLCX_GUI_INF*)p1;

	LcVar_WindowSize(&m_nWidth, &m_nHeight);			// read the screen and height


	if(pInf)
	{
		// Id
		this->m_nId = pInf->nId;

		// Name
		if(pInf->sName && strlen(pInf->sName))
			strcpy(m_sName, pInf->sName);

		// Text
		if(pInf->sText)
			SetText(pInf->sText);

		// event region
		if(pInf->rEvnt)
		{
			memcpy(&m_rEvent, pInf->rEvnt, sizeof(LCXRECT) );

			// Width
			if(0 < m_rEvent.w && m_rEvent.w < 2048)
				this->m_nWidth = (INT)m_rEvent.w;

			// Height
			if(0 < m_rEvent.h && m_rEvent.h < 2048)
				this->m_nHeight= (int)m_rEvent.h;
		}

		// title region
		if(pInf->rTitle)
			memcpy(&m_rTitle, pInf->rTitle, sizeof(LCXRECT) );

		// system button rects
		if(pInf->rBrcs)
			memcpy( m_rBrcts, pInf->rBrcs, sizeof(m_rBrcts ) );

		if(pInf->rRscs)
			memcpy(&m_rRscts, pInf->rRscs, sizeof(LCXRECT) );

		// Parent
		if(pInf->pPrnt)
			((CLcxGui*)(pInf->pPrnt))->Attach(this);
	}

	return LC_OK;
}


INT CLcxGui::SetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if(LCXGUI_EVNT == dCmd)
	{
		memcpy(&m_Evnt, pData, sizeof(TLCX_GUI_EVENT) );
	}
	else if(LCXGUI_ACTIVE  == dCmd)
	{
		INT active = (INT)pData;
		m_Active   = active;

		if(LCXGUI_DISABLE == m_Active)	m_dcColorA = 0.65F;
		else							m_dcColorA = 1.0F;


		lsPLCXGUI::iterator F = m_Chld.begin();
		lsPLCXGUI::iterator L = m_Chld.end();

		for(; F != L; ++F)
		{
			CLcxGui* pGui = (*F);
			pGui->SetAttrib(LCXGUI_ACTIVE, pData);
		}
	}

	else if(LCXGUI_ENABLE <= dCmd &&  dCmd<= LCXGUI_SHOW)
	{
		this->SetState(dCmd);
	}

	else if(LCXGUI_TEXTURE == dCmd)
	{
		SetExternTexture((PLC_TEXTURE)pData);
	}

	else if(LCXGUI_FONT == dCmd)
	{
		TLC_FONT* tFont = (TLC_FONT*)pData;
		SAFE_RELEASE(	m_pFont	);

		if(LC_FAILED(LcDev_CreateFontFromFile(NULL, &m_pFont, NULL, tFont)))
			return LC_EFAIL;
	}
	else if(LCXGUI_EXTERN== dCmd)
	{
		TLCX_GUI_EXTERN* ext = (TLCX_GUI_EXTERN*)pData;
		return SetupWithExtern(ext);

	}
	else if(LCXGUI_ID == dCmd)
	{
		m_nId = *((UINT*)pData);
	}
	else if(LCXGUI_NAME == dCmd)
	{
		memset(m_sName, 0, sizeof(m_sName) );
		strcpy(m_sName, (char*)pData);
	}
	else
		return LC_EFAIL;


	return LC_OK;
}


INT CLcxGui::GetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if(LCXGUI_EVNT == dCmd)
	{
		CLcxGui* pGui = NULL;
		TLCX_GUI_EVENT* Event = UsGui::_LcxGuiGetEvent();

		pGui = (CLcxGui*)Event->_o;
		if(NULL == pGui)
			return LC_EFAIL;

		if(this != (pGui)->GetRoot())
			return LC_EFAIL;

		memcpy(pData, Event, sizeof(TLCX_GUI_EVENT) );
	}
	else if(LCXGUI_ID == dCmd)
	{
		*((UINT*)pData) = m_nId;
	}
	else if(LCXGUI_NAME == dCmd)
	{
		*((char**)pData) = &m_sName[0];
	}

	else if(LCXGUI_CHILD_GUI == dCmd)
	{
		UINT uId = *((UINT*)pData);

		PLCX_GUI pChild = NULL;

		CLcxGui* p	 = NULL;
		lsPLCXGUI::iterator F = m_Chld.begin();
		lsPLCXGUI::iterator L = m_Chld.end();

		for(; F != L; ++F)
		{
			p = (*F);

			if(uId == p->GetId())
			{
				pChild = p;
				break;
			}
		}

		*((PLCX_GUI*)pData) = pChild;
		if(NULL == pChild)
			return LC_EFAIL;
	}
	else if(LCXGUI_TEXT == dCmd)
	{
		*((char**)pData) = NULL;
		if(!m_pFont)
			return LC_EFAIL;

		*((char**)pData) = (char*)m_pFont->GetString();
	}
	else if(LCXGUI_INFO == dCmd)
	{
		TLCX_GUI_INF* Inf =(TLCX_GUI_INF*)pData;
		Inf->nId	 =  m_nId;
		Inf->sName	 =  m_sName;
		Inf->nType	 =  GetRscType();

		Inf->rEvnt	 = &m_rEvent;
		Inf->rTitle	 = &m_rTitle;
		Inf->rBrcs	 =  m_rBrcts;
		Inf->rRscs	 = &m_rRscts;
		Inf->pPrnt	 =  m_Prnt;
	}
	else
		return LC_EFAIL;

	return LC_OK;
}


INT CLcxGui::Attach(PLCX_GUI pNode)
{
	CLcxGui* pChild = (CLcxGui*)pNode;

	if( pChild == this)
		return LC_EFAIL;

	if( pChild == LcxGui_GetActiveDialog() )
	{
		LcxGui_SetActiveDialog(NULL);
	}

	UsGui::_LcxGuiSubstractRootDialog(pChild);
	pChild->SetParent(this);

	if(this == LcxGui_GetActiveDialog() )
	{
		pChild->SetAttrib(LCXGUI_ACTIVE, (LC_HANDLE)LCXGUI_ENABLE);
	}

	m_Chld.push_back(pChild);

	return LC_EFAIL;
}


INT CLcxGui::Detatch(PLCX_GUI pNode)
{
	CLcxGui* pChild = (CLcxGui*)pNode;

	lsPLCXGUI::iterator F = m_Chld.begin();
	lsPLCXGUI::iterator L = m_Chld.end();

	for(; F != L; ++F)
	{
		if(pChild == (*F))
		{
			m_Chld.erase(F);
			break;
		}
	}

	return INT( m_Chld.size() );
}




INT CLcxGui::SetPosition(const LC_FLOAT2* v)
{
	if(NULL == v)
		return LC_EFAIL;

	m_rEvent.x = v->x;
	m_rEvent.y = v->y;

	return LC_OK;
}

INT CLcxGui::SetRotation(const LC_FLOAT3* v)
{
	if(NULL == v)
		return LC_EFAIL;

	m_vcRot = *v;
	return LC_OK;
}


INT CLcxGui::SetScaling(const LC_FLOAT2* v)
{
	if(NULL == v)
		return LC_EFAIL;

	m_vcScl = *v;
	return LC_OK;
}



void CLcxGui::SetState(INT v)
{
	if( LCXGUI_DIALOG == this->GetRscType())
	{
		if(LCXGUI_HIDE == v)
		{
			UsGui::_LcxGuiSortDialog(this, LC_FALSE);

			if(this ==  LcxGui_GetActiveDialog())
				LcxGui_SetActiveDialog(NULL);
		}

	}

	if( LCXGUI_DIALOG == this->GetRscType())
	{
		if(LCXGUI_SHOW == v && NULL == LcxGui_GetActiveDialog())
			LcxGui_SetActiveDialog(this);
	}

	m_State = v;
}



INT CLcxGui::SetText(const char* str, LC_COLORF* color)
{
	if(NULL == str || 0 == strlen(str) || NULL == m_pFont)
		return LC_OK;


	m_pFont->SetColor(color);
	m_pFont->SetString(str);

	return LC_OK;
}



LCXVECTOR2* CLcxGui::GetPosition()
{
	return (LCXVECTOR2*)&(m_rEvent.x);
}


LCXVECTOR2 CLcxGui::GetPositionAbs()		// Absolute position
{
	LCXVECTOR2 t(m_rEvent);
	if(m_Prnt)
		t += *( ((CLcxGui*)m_Prnt)->GetPosition() );

	return t;
}

INT CLcxGui::SetPositionRef(FLOAT* v)
{
	m_rEvent.x = v[0] - m_vcPosRef.x;
	m_rEvent.y = v[1] - m_vcPosRef.y;


	if(LCXGUI_DIALOG == this->GetRscType())
	{
		INT	ScnW=0, ScnH=0;
		LcVar_WindowSize(&ScnW, &ScnH);


		if( m_rEvent.x > ScnW-12.0F)
			m_rEvent.x = ScnW-12.0F;

		if( m_rEvent.x < -m_rTitle.w + 12.0F)
			m_rEvent.x = -m_rTitle.w + 12.0F;


		if( m_rEvent.y > ScnH-m_rTitle.h)
			m_rEvent.y = ScnH-m_rTitle.h;

		if( m_rEvent.y < -m_rTitle.h/2)
			m_rEvent.y = -m_rTitle.h/2;
	}


	return LC_OK;
}



