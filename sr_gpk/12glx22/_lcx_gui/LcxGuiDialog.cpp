// Implementation of the CLcxGuiDialog class.
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
#include "LcxGuiDialog.h"


struct Tsrc2d
{
	LCXRECT	r;			// rect
	LCXVECTOR2	s;		// scaling
	LCXVECTOR2	p;		// position

	Tsrc2d() : r(0,0,0,0), s(1,1), p(0,0){}
	Tsrc2d(const Tsrc2d& rhs) : r(rhs.r), s(rhs.s), p(rhs.p){}

	Tsrc2d(const LCXRECT& R) : r(R), s(1,1), p(0,0){}
	Tsrc2d(const LCXRECT& R, const LCXVECTOR2& S, const LCXVECTOR2& P) : r(R), s(S), p(P){}
};


CLcxGuiDialog::CLcxGuiDialog()
{
	m_nWidth	= 320;
	m_nHeight	= 240;

	m_BgVtx		= 0;
	m_BgPos		= NULL;
	m_BgTex		= NULL;

	m_SysBtn	= NULL;
	m_SysPos	= LCXVECTOR2(0,0);
}


CLcxGuiDialog::~CLcxGuiDialog()
{
	Destroy();
}


void CLcxGuiDialog::Destroy()
{
	// decrease the dialog counter
	--CLcxGui::m_pDtxDlgN;

	if(0 > CLcxGui::m_pDtxDlgN)
	{
		LOGE("Error: The reference count of dialog texture is underflow.\n");
		return;
	}

	if(0 == CLcxGui::m_pDtxDlgN)
	{
		if(CLcxGui::m_pDtxDlg)
		{
			CLcxGui::m_pDtxDlg->Release();
			CLcxGui::m_pDtxDlg = NULL;
		}
	}


	SAFE_DELETE_ARRAY(	m_BgPos	);
	SAFE_DELETE_ARRAY(	m_BgTex	);


	// Release Child
	itPLCXGUI F = m_Chld.begin();
	itPLCXGUI L = m_Chld.end();

	for(; F != L; ++F)
	{
		(*F)->Release();
	}

	m_Chld.clear();
}



INT	CLcxGuiDialog::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT hr = LC_OK;

	UINT	ImgFormat= LC_FMT_RGBA;
	UINT	ImgType  = LC_FMT_WORD_5551;
	UINT	ImgW = 64;
	UINT	ImgH = 64;
	UINT	ImgD = 2;
	UINT	ImgC = 0x0;
	BYTE*	pPxl = (BYTE*)UsGui::_LcxGuiGetRscDialog();


	TLCX_GUI_EXTERN* ext = (TLCX_GUI_EXTERN*)p2;


	if(LC_FAILED(CLcxGui::Create(p1, p2, p3, p4)))
		return LC_CANNT_FIND_RESOURCE;


	// increase the reference counter of default dialog texture
	++CLcxGui::m_pDtxDlgN;
	if(0 > CLcxGui::m_pDtxDlgN)
	{
		LOGE("Error: The reference count of dialog texture is overflow.\n");
		return LC_CANNT_FIND_RESOURCE;
	}

	if(1 == CLcxGui::m_pDtxDlgN)
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

		hr = LcDev_CreateTextureFromBuffer(NULL, &CLcxGui::m_pDtxDlg, &t);

		if(LC_FAILED(hr))
			--CLcxGui::m_pDtxDlgN;
	}



	// Create system button
	PLCX_GUI		SysBtn;
	TLCX_GUI_INF	bt_inf;
	bt_inf.sName = (char*)"system button";
	bt_inf.pPrnt = this;
	hr = LcxEnt_CreateGui(LCXCMD_GUI_SYSBTN, &SysBtn, &bt_inf);
	m_SysBtn = (CLcxGui*)SysBtn;


	// extern data
	if(ext)
		return SetupWithExtern(ext);




	std::vector<Tsrc2d >	vRscImg;

	// Title
	LCXRECT	rTtlL (  0, 0,  7, 23);		// Left
	LCXRECT	rTtlC (  7, 0,  8, 23);		// Center
	LCXRECT	rTtlR (  9, 0, 16, 23);		// Right

	// Body
	LCXRECT	rBdyL (  0,23,  3, 24);		// Body Left
	LCXRECT	rBdyC (  7,24,  8, 25);		// Body Center
	LCXRECT	rBdyR ( 13,23, 16, 24);		// Body Right
	LCXRECT	rBdyBL(  0,24,  3, 27);		// Body Bottom Left
	LCXRECT	rBdyBC(  3,24,  4, 27);		// Body Bottom Center
	LCXRECT	rBdyBR( 13,24, 16, 27);		// Body Bottom Right

	LCXVECTOR2	vTtL;		// title left position
	LCXVECTOR2	vTtC;		// title center position
	LCXVECTOR2	vTtCs;		// title center scaling
	LCXVECTOR2	vTtR;		// title right position

	LCXVECTOR2	vBdL;		// body left position
	LCXVECTOR2	vBdLs;		// body scaling
	LCXVECTOR2	vBdC;		// body center position
	LCXVECTOR2	vBdCs;		// body center scaling
	LCXVECTOR2	vBdR;		// body right position
	LCXVECTOR2	vBdRs;		// body right scaling

	LCXVECTOR2	vBdBL;		// body bottom left position
	LCXVECTOR2	vBdBC;		// body bottom center position
	LCXVECTOR2	vBdBCs;		// body bottom center scaling
	LCXVECTOR2	vBdBR;		// body bottom right position

	rTtlL.ToRegion();
	rTtlR.ToRegion();
	rTtlC.ToRegion();

	rBdyL.ToRegion();
	rBdyC.ToRegion();
	rBdyR.ToRegion();

	rBdyBL.ToRegion();
	rBdyBC.ToRegion();
	rBdyBR.ToRegion();

	vRscImg.push_back(rTtlL);
	vRscImg.push_back(rTtlC);
	vRscImg.push_back(rTtlR);

	vRscImg.push_back(rBdyL );
	vRscImg.push_back(rBdyC );
	vRscImg.push_back(rBdyR );
	vRscImg.push_back(rBdyBL);
	vRscImg.push_back(rBdyBC);
	vRscImg.push_back(rBdyBR);



	m_BgVtx = 0;
	m_BgPos = NULL;
	m_BgTex = NULL;

	// setup title
	vTtL = LCXVECTOR2(0, 0);
	vTtC = vTtL + LCXVECTOR2(rTtlL.w, 0);
	vTtCs= LCXVECTOR2(m_nWidth - rTtlL.w - rTtlR.w, 1);

	vTtR = vTtC + LCXVECTOR2(vTtCs.x, 0);


	// close button position
	LCXVECTOR2 SysPos;
	SysPos = vTtL + LCXVECTOR2(vTtCs.x, 0) + LCXVECTOR2(-6, 5);
	m_SysPos = SysPos;

	// setup Title region
	m_rTitle = LCXRECT(0,0, m_SysPos.x - 5, rTtlC.h);	// w: close position-5, h: title height



	// body
	FLOAT dcW = m_nWidth - rBdyL.w - rBdyR.w;
	FLOAT dcH = m_nHeight - rTtlL.h - rBdyBL.h;

	//
	vBdL = vTtL + LCXVECTOR2(0, rTtlL.h);
	vBdLs= LCXVECTOR2( 1, dcH);

	vBdC = vBdL + LCXVECTOR2(rBdyL.w, 0);
	vBdCs= LCXVECTOR2(dcW , dcH);

	vBdR = vBdC + LCXVECTOR2(vBdCs.x, 0);
	vBdRs= LCXVECTOR2( 1, dcH);


	// body bottom
	vBdBL= vTtL + LCXVECTOR2(0, m_nHeight - rBdyBL.h);

	vBdBR = vTtL;
	vBdBR+= LCXVECTOR2(m_nWidth, m_nHeight);
	vBdBR-= LCXVECTOR2(rBdyBR.w, rBdyBR.h);

	vBdBC = vBdBL + LCXVECTOR2(rBdyBL.w, 0);
	vBdBCs= LCXVECTOR2(m_nWidth -rBdyBL.w-rBdyBR.w, 1);



	INT iSize = vRscImg.size();




	vRscImg[0].p = vTtL;		// title left position
	vRscImg[1].p = vTtC;		// title center position
	vRscImg[1].s = vTtCs;		// title center scaling
	vRscImg[2].p = vTtR;		// title right position

	vRscImg[3].p = vBdL;		// body left position
	vRscImg[3].s = vBdLs;		// body scaling
	vRscImg[4].p = vBdC;		// body center position
	vRscImg[4].s = vBdCs;		// body center scaling
	vRscImg[5].p = vBdR;		// body right position
	vRscImg[5].s = vBdRs;		// body right scaling

	vRscImg[6].p = vBdBL;		// body bottom left position
	vRscImg[7].p = vBdBC;		// body bottom center position
	vRscImg[7].s = vBdBCs;		// body bottom center scaling
	vRscImg[8].p = vBdBR;		// body bottom right position




	m_BgVtx = iSize;
	m_BgVtx *= 6;

	m_BgPos = new LCXVECTOR2[m_BgVtx];
	m_BgTex = new LCXVECTOR2[m_BgVtx];



	LCXVECTOR2	vcMax(-10000, -10000);

	for(INT i=0; i<iSize; ++i)
	{
		LCXVECTOR2* OutPos = &m_BgPos[i*6];
		LCXVECTOR2* OutTex = &m_BgTex[i*6];

		LCXVECTOR2  oP[4];
		LCXVECTOR2  oT[4];

		LCXRECT*	r = &vRscImg[i].r;
		LCXVECTOR2* s = &vRscImg[i].s;
		LCXVECTOR2* p = &vRscImg[i].p;

		LcxEnt_VtxFromDrawingImage(oP, oT, ImgW, ImgH, r, p, s, NULL, 0, LC_FALSE);

		OutPos[0] = oP[0];
		OutPos[1] = oP[1];
		OutPos[2] = oP[3];
		OutPos[3] = oP[2];
		OutPos[4] = oP[3];
		OutPos[5] = oP[1];

		OutTex[0] = oT[0];
		OutTex[1] = oT[1];
		OutTex[2] = oT[3];
		OutTex[3] = oT[2];
		OutTex[4] = oT[3];
		OutTex[5] = oT[1];


		for(INT k=0; k<4; ++k)
		{
			if(vcMax.x<oP[k].x) vcMax.x = oP[k].x;
			if(vcMax.y<oP[k].y) vcMax.y = oP[k].y;
		}
	}


	if(vcMax.x<0) vcMax.x = 0;
	if(vcMax.y<0) vcMax.y = 0;

	m_rEvent.w = vcMax.x;
	m_rEvent.h = vcMax.y;


	vRscImg.clear();

	// setup button position
	m_SysBtn->SetPosition(&m_SysPos);



	if(NULL == m_pTexE)
		m_pTexE = CLcxGui::m_pDtxDlg;


	return hr;
}


INT CLcxGuiDialog::FrameMove()
{
	if(LCXGUI_HIDE == m_State)
		return LC_OK;


	INT				hr = 0;
	CLcxGuiDialog* pDragDlg = NULL;

	LCXVECTOR2*	EvPos;
	INT			EvMsg =0;

	// Child window
	itPLCXGUI F;
	itPLCXGUI L;

	pDragDlg = (CLcxGuiDialog*)UsGui::_LcxGuiGetDragDialog();
	if( pDragDlg == this)
		goto END;


	EvMsg = m_Evnt._m ;						// event message
	EvPos = (LCXVECTOR2*)&m_Evnt._sf[0];	// event value


	// Drag Event Process
	hr = LCXIsInRect(*EvPos, m_rEvent.x, m_rEvent.y, m_rTitle.w, m_rTitle.h);

	hr = LC_SUCCEEDED(hr) &&
		(EvMsg & LCXGUI_EVNT) &&
		(EvMsg & 0x0F) == LCXLOWORD(LCXGUI_EVNT_PRESS);

	if(hr)
	{
		UsGui::_LcxGuiSetDragDialog(this);
		m_vcPosRef.x = EvPos->x - m_rEvent.x;
		m_vcPosRef.y = EvPos->y - m_rEvent.y;

		goto END;
	}


	// Event Process System Button
	m_SysBtn->SetAttrib(LCXGUI_EVNT, &m_Evnt);
	hr = m_SysBtn->FrameMove();

	if(m_State && LCXGUI_EVNT_CLOSE == hr)	// enable일 때만 이벤트 처리
	{
		TLCX_GUI_EVENT Event(this, LCXGUI_DIALOG, LCXGUI_EVNT_CLOSE);
		UsGui::_LcxGuiSetEvent(&Event);
		return hr;
	}

	// update Child window
	F = m_Chld.begin();
	L = m_Chld.end();

	if(m_SysBtn)
		++F;

	for(; F != L; ++F)
	{
		CLcxGui* pGui = (*F);

		// enable일 때만 이벤트 처리
		if(m_State)
			pGui->SetAttrib(LCXGUI_EVNT, &m_Evnt);

		pGui->FrameMove();
	}

END:

	// reset event
	memset(&m_Evnt, 0, sizeof(TLCX_GUI_EVENT));

	return LC_OK;
}


void CLcxGuiDialog::Render()
{
	if(LCXGUI_HIDE == m_State || NULL == m_pTexE)
		return;


	INT			nTex = m_pTexE->GetId();

	LCXMATRIX	tm;
	LCXVECTOR3	tmPos(m_rEvent.x, m_rEvent.y, 0);
	CLcxGui*	prnt= NULL;
	LCXVECTOR2	vPrn(0,0);
	LCXCOLOR	dColor = m_dcColor;



	dColor.a *= m_dcColorA;

	prnt = (CLcxGui*)this->GetParent();

	if(prnt)
	{
		vPrn = prnt->GetPositionAbs();
		tmPos.x += vPrn.x;
		tmPos.y += vPrn.y;
	}

	tm.SetupTranslation(&tmPos);


	glBindTexture(GL_TEXTURE_2D, nTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glDisableClientState(GL_COLOR_ARRAY);
	glVertexPointer  (2, GL_FLOAT,	0, m_BgPos);
	glTexCoordPointer(2, GL_FLOAT,	0, m_BgTex);

	glColor4f(dColor.r, dColor.g, dColor.b, dColor.a);


	// Draw Background
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	{
		glLoadMatrixf((FLOAT*)&tm);
		glDrawArrays(GL_TRIANGLES, 0, m_BgVtx);
	}

	glPopMatrix();


	// Draw Child window
	itPLCXGUI F = m_Chld.begin();
	itPLCXGUI L = m_Chld.end();

	for(; F != L; ++F)
	{
		CLcxGui* pGui = (*F);
		pGui->Render();
	}

	glColor4f(1,1,1,1);
	glEnableClientState(GL_COLOR_ARRAY);
}


INT CLcxGuiDialog::SetupWithExtern(TLCX_GUI_EXTERN* ext)
{
	INT nVtx = (2 * 3) * 2;

	if(NULL == ext->txInf)
		return LC_EFAIL;

	m_BgVtx = nVtx;

	SAFE_DELETE_ARRAY( m_BgPos	);
	SAFE_DELETE_ARRAY( m_BgTex	);

	m_BgPos = new LCXVECTOR2[nVtx];
	m_BgTex = new LCXVECTOR2[nVtx];


//	Rects[ 0] // title
//	Rects[ 4] // system button position
//	Rects[ 5] // system button normal
//	Rects[ 6] // system button over
//	Rects[ 7] // system button pressed
//	Rects[12] // body


	INT			ImgW = 0;
	INT			ImgH = 0;
	LCXRECT*	rTitle;
	LCXRECT*	rBody;

	LCXVECTOR2  oP[4];
	LCXVECTOR2  oT[4];
	LCXVECTOR2  offSet(0,0);


	// setup the event region
	if(ext->rEvnt)
	{
		m_rEvent = *ext->rEvnt;			// setup the event region
	}



	if(NULL == ext->rRcts)
		return LC_OK;


	ImgW = ext->txInf->ImgW;
	ImgH = ext->txInf->ImgH;

	rTitle = (LCXRECT*)&ext->rRcts[ 0];
	rBody  = (LCXRECT*)&ext->rRcts[12];


	if(0 < rTitle->w && 0 < rTitle->h)
	{
		m_rTitle = *rTitle;					// setup the title region
		m_rTitle.w= ext->rRcts[4].x - 4;	// recalculate the dag region with system position
	}



	// build the vertex list of dialog
	if(0 < rTitle->w && 0 < rTitle->h && 0 < rBody->w && 0 < rBody->h)
	{
		LcxEnt_VtxFromDrawingImage(oP, oT, ImgW, ImgH, rTitle, &offSet, LC_FALSE);	// title
		m_BgPos[ 0] = oP[0];	m_BgTex[ 0] = oT[0];
		m_BgPos[ 1] = oP[1];	m_BgTex[ 1] = oT[1];
		m_BgPos[ 2] = oP[3];	m_BgTex[ 2] = oT[3];
		m_BgPos[ 3] = oP[2];	m_BgTex[ 3] = oT[2];
		m_BgPos[ 4] = oP[3];	m_BgTex[ 4] = oT[3];
		m_BgPos[ 5] = oP[1];	m_BgTex[ 5] = oT[1];

		offSet = LCXVECTOR2(0, rTitle->h);
		LcxEnt_VtxFromDrawingImage(oP, oT, ImgW, ImgH, rBody, &offSet, LC_FALSE);		// body
		m_BgPos[ 6] = oP[0];	m_BgTex[ 6] = oT[0];
		m_BgPos[ 7] = oP[1];	m_BgTex[ 7] = oT[1];
		m_BgPos[ 8] = oP[3];	m_BgTex[ 8] = oT[3];
		m_BgPos[ 9] = oP[2];	m_BgTex[ 9] = oT[2];
		m_BgPos[10] = oP[3];	m_BgTex[10] = oT[3];
		m_BgPos[11] = oP[1];	m_BgTex[11] = oT[1];
	}



	// setup the system button
	TLCX_GUI_EXTERN	t;
	t.txInf   = ext->txInf;
	t.rEvnt = &ext->rRcts[4];	// position
	t.rRcts = &ext->rRcts[5];	// nomal, over, pressed

	m_SysBtn->SetAttrib(LCXGUI_EXTERN, &t);

	return LC_OK;
}


void CLcxGuiDialog::SetExternTexture(PLC_TEXTURE pTex)
{
	m_pTexE = pTex;
	m_SysBtn->SetAttrib(LCXGUI_TEXTURE, pTex);
}

