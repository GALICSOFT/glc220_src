// Implementation of the CLcxGuiSysBtn class.
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
#include "LcxGuiSysBtn.h"


CLcxGuiSysBtn::CLcxGuiSysBtn()
{
	m_rCbA	= 0;
}


CLcxGuiSysBtn::~CLcxGuiSysBtn()
{
	Destroy();
}


void CLcxGuiSysBtn::Destroy()
{
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



INT	CLcxGuiSysBtn::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT hr = LC_OK;

	UINT	ImgFormat= LC_FMT_RGBA;
	UINT	ImgType  = LC_FMT_WORD_5551;
	UINT	ImgW = 64;
	UINT	ImgH = 64;
	UINT	ImgD = 2;
	UINT	ImgC = 0x0;
	BYTE*	pPxl = (BYTE*)UsGui::_LcxGuiGetRscSysBtn();



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


	m_rBrcts[0].ToRegion( 0,  0,  15, 15);	// Normal
	m_rBrcts[1].ToRegion( 0, 15,  15, 30);	// Over
	m_rBrcts[2].ToRegion( 0, 30,  15, 45);	// Pressed


	// set the texture
	if(NULL == m_pTexE)
		m_pTexE = CLcxGui::m_pDtxBtn;


	return hr;
}


INT CLcxGuiSysBtn::FrameMove()
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

	m_rCbA = 0;
	if(LC_SUCCEEDED(bMouseIn))
	{
		m_rCbA = 1;

		if((EvMsg & LCXGUI_EVNT))
		{
			if( (LCXGUI_EVNT_PRESS & 0xF) == (0xF & EvMsg))
				m_rCbA = 2;

			else if( (LCXGUI_EVNT_UP & 0xF) == (0xF & EvMsg))
				return LCXGUI_EVNT_CLOSE;
		}
	}


	// reset event
	memset(&m_Evnt, 0, sizeof(TLCX_GUI_EVENT));

	return LC_OK;
}


void CLcxGuiSysBtn::Render()
{
	if(LCXGUI_HIDE == m_State || NULL == m_pTexE)
		return;

	INT			nTex = m_pTexE->GetId();
	LCXVECTOR2	RefPos(m_rEvent);
	CLcxGui*	Prnt   = (CLcxGui*)m_Prnt;
	LCXCOLOR	dColor = m_dcColor;

	INT			ImgW = m_pTexE->GetImgW();
	INT			ImgH = m_pTexE->GetImgH();

	LCXVECTOR2	BgPos[4];
	LCXVECTOR2	BgTex[4];

	dColor.a *= m_dcColorA;


	if(Prnt)
		RefPos += Prnt->GetPositionAbs();

	LcxEnt_VtxFromDrawingImage(BgPos, BgTex, ImgW, ImgH, &m_rBrcts[m_rCbA], &RefPos, LC_FALSE);	// title


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

	glEnableClientState(GL_COLOR_ARRAY);
	glColor4f(1,1,1,1);
}


INT CLcxGuiSysBtn::SetupWithExtern(TLCX_GUI_EXTERN* ext)
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


