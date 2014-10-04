// Implementation of the CLcxEftPtc2d class.
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
  #include <FGraphicsOpengl.h>
  using namespace Osp::Graphics::Opengl;

#else
  #include <gles/gl.h>

#endif

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
  #include "../ILCX_Effect.h"
#else
  #include "../LcxEntity.h"
#endif

#include "LcxEffect.h"
#include "LcxEftPtc2d.h"


CLcxEftPtc2d::CLcxEftPtc2d()
{
	m_Ptc	= NULL;
	m_VtxN	= 0;
	m_Vtx	= NULL;
}

CLcxEftPtc2d::~CLcxEftPtc2d()
{
	Destroy();
}

void CLcxEftPtc2d::Destroy()
{
	SAFE_DELETE_ARRAY(	m_Ptc	);
	SAFE_DELETE_ARRAY(	m_Vtx	);
}



INT	CLcxEftPtc2d::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT hr = 0;
	int	 n;

	if(!p1)
		return LC_EFAIL;



	hr = CLcxEffect::Create(p1, p2, p3, p4);
	if(LC_FAILED(hr))
		return hr;


	memcpy(&m_Inf, p1, sizeof(TLCX_PRT2dINF) );

	//m_Ptc = new TLCX_PRT2d[m_Inf.nPrtNum];
	m_Ptc = m_Inf.pPrtList;

	if(LCXEFT_REN_TIANGLE == m_Inf.bPrtRen)
		m_Vtx	= new LC3D::Vx2DUV[m_Inf.nPrtNum*3];

	else if(LCXEFT_REN_QUAD == m_Inf.bPrtRen)
		m_Vtx	= new LC3D::Vx2DUV[m_Inf.nPrtNum*4];



	TLCX_PRT2d*	ptc = NULL;
	char*		Prt = (char*)m_Ptc;

	for(n=0; n<m_Inf.nPrtNum; ++n)
	{
		char* pTmp = Prt + m_Inf.nPrtSize * n;
		ptc = (TLCX_PRT2d*)pTmp;

		ptc->idx = n;
		ptc->Init(&m_Inf);
	}


	return LC_OK;
}


INT CLcxEftPtc2d::FrameMove()
{
	int	 n;
	TLCX_PRT2d* ptc = NULL;
	char*		  Prt = (char*)m_Ptc;

	for(n=0; n<m_Inf.nPrtNum; ++n)
	{
		char* pTmp = Prt + m_Inf.nPrtSize * n;
		ptc = (TLCX_PRT2d*)pTmp;
		ptc->Update(&m_Inf);
	}


	m_VtxN = 0;
	for(n=0; n<m_Inf.nPrtNum; ++n)
	{
		char* pTmp = Prt + m_Inf.nPrtSize * n;
		ptc = (TLCX_PRT2d*)pTmp;
		if(0.0F >= ptc->Life)
			continue;

		AssembleVertex(&m_VtxN, ptc);
	}


	return LC_OK;
}


void CLcxEftPtc2d::Render()
{
	if(0 == m_VtxN || NULL == m_Inf.pTex)
		return;


	UINT nTex = m_Inf.pTex->GetId();


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, nTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glEnableClientState(GL_COLOR_ARRAY);


	//Setup point sprites coordinate generation
	if(LCXEFT_REN_POINT == m_Inf.bPrtRen)
	{
		char* pVtx = (char*)(&m_Ptc->pos.x);

		glEnable(GL_POINT_SPRITE_OES);
		glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE);
		glPointParameterf(GL_POINT_SIZE_MAX , 255);
		glPointSize(m_Inf.nPntL);

		glVertexPointer(2, GL_FLOAT, m_Inf.nPrtNum, pVtx);	pVtx += sizeof(LCXVECTOR2);
		glColorPointer (4, GL_FLOAT, m_Inf.nPrtNum, pVtx);

		glDrawArrays(GL_POINTS, 0, m_VtxN);
	}
	else if(LCXEFT_REN_TIANGLE == m_Inf.bPrtRen)
	{
		char* pVtx = (char*)(&m_Vtx->p.x);

		glVertexPointer(2, GL_FLOAT,         sizeof(LC3D::Vx2DUV), pVtx);	pVtx += sizeof(LCXVECTOR2);
		glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(LC3D::Vx2DUV), pVtx);	pVtx += sizeof(LCXCOLORB);
		glTexCoordPointer(2, GL_FLOAT,       sizeof(LC3D::Vx2DUV), pVtx);

		glDrawArrays(GL_TRIANGLES, 0, m_VtxN);
	}

	else if(LCXEFT_REN_QUAD == m_Inf.bPrtRen)
	{
		WORD* pIdx = LcSys_QuadIndex();
		char* pVtx = (char*)(&m_Vtx->p.x);

		glVertexPointer(2, GL_FLOAT,         sizeof(LC3D::Vx2DUV), pVtx);	pVtx += sizeof(LCXVECTOR2);
		glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(LC3D::Vx2DUV), pVtx);	pVtx += sizeof(LCXCOLORB);
		glTexCoordPointer(2, GL_FLOAT,       sizeof(LC3D::Vx2DUV), pVtx);

		glDrawElements(GL_TRIANGLES, m_VtxN*6, GL_UNSIGNED_SHORT, pIdx);
	}


	if(LCXEFT_REN_POINT == m_Inf.bPrtRen)
	{
		glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glDisable(GL_POINT_SPRITE_OES);
	}


	glDisableClientState(GL_COLOR_ARRAY);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}



void CLcxEftPtc2d::AssembleVertex(INT* VtxN, TLCX_PRT2d* ptc)
{
	int n = *VtxN;

	if(LCXEFT_REN_TIANGLE == m_Inf.bPrtRen)
	{
		m_Vtx[3*n+0].t  = LCXVECTOR2(-0.5F, 0.0F);
		m_Vtx[3*n+1].t  = LCXVECTOR2( 1.5F, 0.0F);
		m_Vtx[3*n+2].t  = LCXVECTOR2( 0.5F, 2.0F);

		m_Vtx[3*n+0].d  = ptc->col;
		m_Vtx[3*n+1].d  = ptc->col;
		m_Vtx[3*n+2].d  = ptc->col;

		m_Vtx[3*n+0].p  = ptc->pos;
		m_Vtx[3*n+1].p  = ptc->pos;
		m_Vtx[3*n+2].p  = ptc->pos;
		m_Vtx[3*n+0].p += LCXVECTOR2(-2.0F * ptc->mag.x, -1.0F * ptc->mag.y);
		m_Vtx[3*n+1].p += LCXVECTOR2( 2.0F * ptc->mag.x, -1.0F * ptc->mag.y);
		m_Vtx[3*n+2].p += LCXVECTOR2( 0.0F * ptc->mag.x,  3.0F * ptc->mag.y);
	}

	else if(LCXEFT_REN_QUAD == m_Inf.bPrtRen)
	{
		m_Vtx[4*n+0].t  = LCXVECTOR2( 0.0F, 0.0F);
		m_Vtx[4*n+1].t  = LCXVECTOR2( 1.0F, 0.0F);
		m_Vtx[4*n+2].t  = LCXVECTOR2( 1.0F, 1.0F);
		m_Vtx[4*n+3].t  = LCXVECTOR2( 0.0F, 1.0F);

		m_Vtx[4*n+0].d  = ptc->col;
		m_Vtx[4*n+1].d  = ptc->col;
		m_Vtx[4*n+2].d  = ptc->col;
		m_Vtx[4*n+3].d  = ptc->col;

		m_Vtx[4*n+0].p  = ptc->pos;
		m_Vtx[4*n+1].p  = ptc->pos;
		m_Vtx[4*n+2].p  = ptc->pos;
		m_Vtx[4*n+3].p  = ptc->pos;
		m_Vtx[4*n+0].p += LCXVECTOR2(-1.0F * ptc->mag.x, -1.0F * ptc->mag.y);
		m_Vtx[4*n+1].p += LCXVECTOR2( 1.0F * ptc->mag.x, -1.0F * ptc->mag.y);
		m_Vtx[4*n+2].p += LCXVECTOR2( 1.0F * ptc->mag.x,  1.0F * ptc->mag.y);
		m_Vtx[4*n+3].p += LCXVECTOR2(-1.0F * ptc->mag.x,  1.0F * ptc->mag.y);
	}

	++(*VtxN);
}





INT CLcxEftPtc2d::SetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if(LCXEFT_INFO == dCmd)
	{
		memcpy(&m_Inf, pData, sizeof(TLCX_PRT2dINF) );
	}
	else
		return LC_EFAIL;


	return LC_OK;
}


INT CLcxEftPtc2d::GetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if(LCXEFT_INFO == dCmd)
	{
		memcpy(pData, &m_Inf, sizeof(TLCX_PRT2dINF) );
	}
	else
		return LC_EFAIL;


	return LC_OK;
}


