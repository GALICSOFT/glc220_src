// Implementation of the CLcxGuiBtn class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_PC_LINUX_)
  #include <GL/gl.h>

#elif defined(__APPLE__) || defined(__IPHONE__)
  #import <OpenGLES/ES1/gl.h>

#elif defined(__BADA__)
  #include <FBaseSys.h>
  #include <FGraphicsOpengl.h>
  using namespace Osp::Graphics::Opengl;

#else
  #include <gles/gl.h>

#endif

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
#include "LcxGuiBtn.h"


CLcxGuiBtn::CLcxGuiBtn()
{
	m_rCbA		= &m_rBrcts[0];
}


CLcxGuiBtn::~CLcxGuiBtn()
{
	Destroy();
}


void CLcxGuiBtn::Destroy()
{
	//decrease the button texture counter
	--CLcxGui::m_pDtxBtnN;

	if(0 > CLcxGui::m_pDtxBtnN)
	{
		LOGE("Error: The reference count of button texture is underflow.\n");
		return;
	}

	if(0 == CLcxGui::m_pDtxBtnN)
	{
		if(CLcxGui::m_pDtxBtn)
		{
			CLcxGui::m_pDtxBtn->Release();
			CLcxGui::m_pDtxBtn = NULL;
		}
	}
}



INT	CLcxGuiBtn::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT hr = LC_OK;

	UINT	ImgFormat= LC_FMT_RGBA;
	UINT	ImgType  = LC_FMT_WORD_5551;
	UINT	ImgW = 64;
	UINT	ImgH = 64;
	UINT	ImgD = 2;
	UINT	ImgC = 0x0;
	BYTE*	pPxl = (BYTE*)UsGui::_LcxGuiGetRscSysBtn();


	TLCX_GUI_EXTERN* ext = (TLCX_GUI_EXTERN*)p2;


	if(LC_FAILED(CLcxGui::Create(p1, p2, p3, p4)))
		return LC_CANNT_FIND_RESOURCE;


	++CLcxGui::m_pDtxBtnN;

	if(0 > CLcxGui::m_pDtxBtnN)
	{
		LOGE("Error: The reference count of dialog texture is overflow.\n");
		return LC_CANNT_FIND_RESOURCE;
	}

	if(1 == CLcxGui::m_pDtxBtnN)
	{
		TLC_IMAGE t =
		{
			  ImgFormat
			, ImgType
			, ImgW
			, ImgH
			, ImgD
			, ImgC
			, pPxl
		};

		hr = LcDev_CreateTextureFromBuffer(NULL, &CLcxGui::m_pDtxBtn, &t);

		if(LC_FAILED(hr))
			--CLcxGui::m_pDtxBtnN;
	}


	// extern data
	if(ext)
		return SetupWithExtern(ext);


	m_rBrcts[0].ToRegion(16,  0,  64, 19);	// Normal
	m_rBrcts[1].ToRegion(16, 20,  64, 39);	// Over
	m_rBrcts[2].ToRegion(16, 40,  64, 59);	// Pressed


	m_pTexE = CLcxGui::m_pDtxBtn;


	return hr;
}


INT CLcxGuiBtn::FrameMove()
{
	if(LCXGUI_HIDE == m_State)
		return LC_OK;


	LCXVECTOR2	RefPos(0,0);
	LCXVECTOR2*	EvPos;
	INT			EvMsg = 0;

	INT bMouseIn = -1;

	EvMsg = m_Evnt._m ;						// event message
	EvPos = (LCXVECTOR2*)&m_Evnt._sf[0];	// event value


	// because the parent position can be changed,
	// the button position must be repositioned.
	RefPos = this->GetPositionAbs();


	//	bPress = (GetAsyncKeyState(WNVK_LBUTTON) & 0x8000);
	bMouseIn = LCXIsInRect(*EvPos, RefPos.x, RefPos.y, m_rBrcts[0].w, m_rBrcts[0].h);

	m_rCbA = &m_rBrcts[0];
	if(LC_SUCCEEDED(bMouseIn))
	{
		UINT v = LCXGUI_EVNT_OVER;

		m_rCbA = &m_rBrcts[1];

		if((EvMsg & LCXGUI_EVNT))
		{
			if( (LCXGUI_EVNT_PRESS & 0xF) == (0xF & EvMsg))
			{
				m_rCbA = &m_rBrcts[2];
				v = LCXGUI_EVNT_PRESS;
			}

			else if( (LCXGUI_EVNT_UP & 0xF) == (0xF & EvMsg))
			{
				//m_rCbA = &m_rBrcts[0];
				v = LCXGUI_EVNT_UP;
			}
		}

		// save event object;
		TLCX_GUI_EVENT Event(this, LCXGUI_BUTTON, v);
		UsGui::_LcxGuiSetEvent(&Event);

		return v;
	}


	// reset event
	memset(&m_Evnt, 0, sizeof(TLCX_GUI_EVENT));

	return LC_OK;
}


void CLcxGuiBtn::Render()
{
	if(LCXGUI_HIDE == m_State || NULL == m_pTexE)
		return;

	INT			nTex = m_pTexE->GetId();
	LCXVECTOR2	RefPos(m_rEvent);
	CLcxGui*	Prnt   = (CLcxGui*)m_Prnt;
	LCXCOLOR	dColor = m_dcColor;

	FLOAT		ImgW = (FLOAT)m_pTexE->GetImgW();
	FLOAT		ImgH = (FLOAT)m_pTexE->GetImgH();

	LCXVECTOR2	BgPos[4];
	LCXVECTOR2	BgTex[4];

	dColor.a *= m_dcColorA;


	if(Prnt)
		RefPos += Prnt->GetPositionAbs();

	LcxEnt_VtxFromDrawingImage(BgPos, BgTex, ImgW, ImgH, m_rCbA, &RefPos
								, &m_vcScl, (LCXVECTOR2*)&m_vcRot.x, m_vcRot.z, LC_FALSE);
	
	

	glBindTexture(GL_TEXTURE_2D, nTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glDisableClientState(GL_COLOR_ARRAY);
	glVertexPointer  (2, GL_FLOAT, 0, BgPos);
	glTexCoordPointer(2, GL_FLOAT, 0, BgTex);

	glColor4f(dColor.r, dColor.g, dColor.b, dColor.a);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glColor4f(1,1,1,1);


	RefPos += LCXVECTOR2(5, 3);


	if(m_pFont)
	{
		LCXCOLOR  * col = NULL;
		PLC_TEXTURE	pTex= NULL;
		UINT	    Tex = 0;

		LCXVECTOR2	fntPos[4];
		LCXVECTOR2	fntTex[4];

		m_pFont->SetPosition(&RefPos);
		pTex= m_pFont->GetTexture();

		if(pTex)
		{
			col = (LCXCOLOR  *)m_pFont->GetColor();
			Tex = pTex->GetId();
			m_pFont->SetupVtxLst(fntPos, fntTex);

			glBindTexture(GL_TEXTURE_2D, Tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glVertexPointer  (2, GL_FLOAT,	0, fntPos);
			glTexCoordPointer(2, GL_FLOAT,	0, fntTex);

			glColor4f(col->r, col->g, col->b, col->a);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
	}

	glEnableClientState(GL_COLOR_ARRAY);
}



INT CLcxGuiBtn::SetupWithExtern(TLCX_GUI_EXTERN* ext)
{
	if(ext->rEvnt)
		this->SetPosition((LCXVECTOR2*)(ext->rEvnt));

	if(ext->rRcts)
	{
		m_rBrcts[0] = ext->rRcts[0];
		m_rBrcts[1] = ext->rRcts[1];
		m_rBrcts[2] = ext->rRcts[2];
	}

	return LC_OK;
}

