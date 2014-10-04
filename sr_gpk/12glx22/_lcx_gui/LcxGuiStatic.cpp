// Implementation of the CLcxGuiStatic class.
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
#include "LcxGuiStatic.h"


CLcxGuiStatic::CLcxGuiStatic()
{
}


CLcxGuiStatic::~CLcxGuiStatic()
{
	Destroy();
}


void CLcxGuiStatic::Destroy()
{
	//decrease the static texture counter
	--CLcxGui::m_pDtxSttN;

	if(0 > CLcxGui::m_pDtxSttN)
	{
		LOGE("Error: The reference count of static texture is underflow.\n");
		return;
	}

	if(0 == CLcxGui::m_pDtxSttN)
	{
		if(CLcxGui::m_pDtxStt)
		{
			CLcxGui::m_pDtxStt->Release();
			CLcxGui::m_pDtxStt = NULL;
		}
	}
}


INT	CLcxGuiStatic::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT hr = LC_OK;

	UINT	ImgFormat= LC_FMT_RGBA;
	UINT	ImgType  = LC_FMT_WORD_5551;
	UINT	ImgW = 64;
	UINT	ImgH = 64;
	UINT	ImgD = 2;
	UINT	ImgC = 0x0;
	BYTE*	pPxl = NULL;


	if(LC_FAILED(CLcxGui::Create(p1, p2, p3, p4)))
		return LC_CANNT_FIND_RESOURCE;


	++CLcxGui::m_pDtxSttN;

	if(0 > CLcxGui::m_pDtxSttN)
	{
		LOGE("Error: The reference count of dialog texture is overflow.\n");
		return LC_CANNT_FIND_RESOURCE;
	}

	if(1 == CLcxGui::m_pDtxSttN)
	{
		pPxl = new BYTE[ImgW * ImgH* ImgD ];
		memset(pPxl, 0xFF, ImgW * ImgH* ImgD);

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

		hr = LcDev_CreateTextureFromBuffer(NULL, &CLcxGui::m_pDtxStt, &t);

		SAFE_DELETE_ARRAY(	pPxl	);

		if(LC_FAILED(hr))
			--CLcxGui::m_pDtxSttN;
	}


	m_rBrcts[0].ToRegion(0,  0,  64, 24);


	// set the texture
	if(NULL == m_pTexE)
		m_pTexE = CLcxGui::m_pDtxStt;

	return hr;
}


INT CLcxGuiStatic::FrameMove()
{
	if(LCXGUI_HIDE == m_State)
		return LC_OK;


	return LC_OK;
}


void CLcxGuiStatic::Render()
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

	LcxEnt_VtxFromDrawingImage(BgPos, BgTex, ImgW, ImgH, m_rBrcts, &RefPos
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




INT CLcxGuiStatic::SetupWithExtern(TLCX_GUI_EXTERN* ext)
{
	if(ext->rEvnt)
		this->SetPosition((LCXVECTOR2*)(ext->rEvnt));

	if(ext->rRcts)
		m_rBrcts[0] = ext->rRcts[0];

	return LC_OK;
}



