// Implementation of the CLcxAcmIns class.
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
#else
  #include "../LcxEntity.h"
#endif

#include "LcxAcm.h"
#include "LcxAcmIns.h"


CLcxAcmIns::TlinkTm::TlinkTm()
{
	pVdP	= NULL;		// Vertex Dest Position

	mtW.Identity();
}

CLcxAcmIns::TlinkTm::~TlinkTm()
{
	SAFE_DELETE_ARRAY(	pVdP	);

	mtW.Identity();
}


CLcxAcmIns::CLcxAcmIns()
{
	m_nMtl	= 0;
	m_pMtl	= NULL;
	m_nGeo	= 0;
	m_pGeo	= NULL;

	m_pOrg	= NULL;


	m_nFrmF		= 0;
	m_nFrmL		= 0;
	m_nFrmS		= 0;
	m_nFrmT		= 0;

	m_TimeC		= 0;
	m_FrameC	= 0;
	m_FrameB	= 0;
	m_FrameE	= 0;

	m_mtWld.Identity();

	m_BlndNum = 1;
}


CLcxAcmIns::~CLcxAcmIns()
{
	Destroy();
}


INT CLcxAcmIns::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxAcmIns::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}



INT CLcxAcmIns::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	CLcxAcm::TInfo*	pInfo;
	m_pOrg	= (CLcxAcm*)p1;

	pInfo	= (CLcxAcm::TInfo*)m_pOrg->GetInfo();


	m_nFrmF	= pInfo->nFrmF;		// First Frame
	m_nFrmL	= pInfo->nFrmL;		// Last Frame
	m_nFrmS	= pInfo->nFrmS;		// Frames per one scecond
	m_nFrmT	= pInfo->nFrmT;		// Tick per Frame

	m_FrameB = m_nFrmF;			// Begin Frame of Animation Loop
	m_FrameE = m_nFrmL;			// End Frame of Animation Loop

	m_nMtl	= pInfo->nMtl;
	m_nGeo	= pInfo->nGeo;

	m_pMtl	= m_pOrg->GetMtrl();
	m_pGeo	= m_pOrg->GetGeometry();


	m_pTM = new TlinkTm[m_nGeo]	;	// World and Animation Matrix


	// Copy from Original
	for(INT n=0; n<m_nGeo; ++n)
	{
		CLcxAcm::Tgeo*			pGeoS = &m_pGeo[n];
		CLcxAcmIns::TlinkTm*	pGeoD = &m_pTM[n];

		pGeoD->mtW = pGeoS->mtW;

		if(CLcxAcm::LCX_MESH == pGeoS->nType)
			pGeoD->pVdP= new VEC3[pGeoS->nVtx];
	}

	return LC_OK;
}


void CLcxAcmIns::Destroy()
{
	m_pOrg = NULL;

	SAFE_DELETE_ARRAY(	m_pTM	);
}




INT CLcxAcmIns::FrameMove()
{
	INT		n=0, j=0;
	INT		nFrame=0;

	// Frame = FrameSpeed * Time;
	m_FrameC = FLOAT(m_nFrmS * m_TimeC);

	nFrame	= INT(m_FrameC);

	if(nFrame>= m_nFrmL)
	{
		// The Current time shall be determined bye the rest time
		INT d = nFrame/ m_nFrmL;
		m_TimeC = (m_FrameC - d * nFrame)/DOUBLE(m_nFrmS);
	}

	for(n=0; n<m_nGeo; ++n)
	{
		CLcxAcm::Tgeo*	pGeoS = &m_pGeo[n];
		TlinkTm*		pGeoD = &m_pTM[n];

		m_pOrg->GetAniMatrix(&pGeoD->mtW, m_FrameC, pGeoS );
	}



	// Update Rendering Vertex Position
	MATA	mtW;
	MATA*	pmtW;
	FLOAT*	Wgx = NULL;		// BLEND WEIGHT
	INT  *	Idx = NULL;		// MATRIX Index

	for(n=0; n<m_nGeo; ++n)
	{
		CLcxAcm::Tgeo*			pGeoS = &m_pGeo[n];
		CLcxAcmIns::TlinkTm*	pGeoD = &m_pTM[n];

		VEC3*	pVsP = NULL;
		VEC4*	pVsW = NULL;
		INT4*	pVsB = NULL;

		VEC3*	pVdP = NULL;


		if(pGeoS->nType != CLcxAcm::LCX_MESH || 0>=pGeoS->nVtx)
			continue;


		pVsP = pGeoS->pVsP;
		pVsW = pGeoS->pVsW;
		pVsB = pGeoS->pVsB;

		pVdP = pGeoD->pVdP;


		for(j=0; j<pGeoS->nVtx; ++j)
		{
			if(1 == m_BlndNum)
			{
				Idx = (INT  *)&pVsB[j];
				pmtW = &m_pTM[ Idx[0] ].mtW;
				pmtW->TransformCoord(&pVdP[j], &pVsP[j]);
			}

			else
			{
				Wgx = (FLOAT*)&pVsW[j];
				Idx = (INT  *)&pVsB[j];

				// Transform Matrix = ¥ÒMi * Wi
				memset(&mtW, 0, sizeof mtW);

				for(int i=0; i<m_BlndNum; ++i)
					mtW += Wgx[i] * m_pGeo[ Idx[i] ].mtW;

				mtW.TransformCoord(&pVdP[j], &pVsP[j]);
			}
		}
	}


	return LC_OK;
}




void CLcxAcmIns::Render()
{
	PLC_DEVICE	pDev = (PLC_DEVICE)LcVar_ReadP(LC_VAR_DEVICE);

	INT		n =0;

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	for(n=0; n<m_nGeo; ++n)
	{
		CLcxAcm::Tgeo*			pGeoS = &m_pGeo[n];
		CLcxAcmIns::TlinkTm*	pGeoD = &m_pTM[n];

		PLC_TEXTURE pTex = NULL;
		UINT		 nTex = 0;

		if(CLcxAcm::LCX_MESH != pGeoS->nType)
			continue;


		if(0<=pGeoS->nMtl)
			pTex = m_pMtl[pGeoS->nMtl].pTex;

		if(NULL == pTex)
			continue;


		nTex = pTex->GetId();
		if(!nTex)
			continue;


		pDev->SetTransform(LC_TS_WORLD, &m_mtWld);

		glVertexPointer  (3, GL_FLOAT,	0, pGeoD->pVdP);


		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glClientActiveTexture(GL_TEXTURE0);

		glBindTexture (GL_TEXTURE_2D, nTex);

		glTexCoordPointer(2, GL_FLOAT,	0, pGeoS->pVsT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, LC_TS_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, LC_TS_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, LC_TS_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, LC_TS_LINEAR);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, LC_TS_MODULATE);

		glDrawElements(GL_TRIANGLES, pGeoS->nFce*3, GL_UNSIGNED_SHORT, pGeoS->pFce);
	}


	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	pDev->SetTransform(LC_TS_WORLD_IDENTITY, NULL);
}




INT CLcxAcmIns::SetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if( dCmd == LCXM_ADVANCE_TIME)
	{
		DOUBLE fElapsed = *((DOUBLE*)pData);
		m_TimeC += fElapsed * 0.001;		// Millisecond -> second

		return LC_OK;
	}

	else if( dCmd == LCXM_CURRENT_TIME)
	{
		DOUBLE fTime = *((DOUBLE*)pData);
		m_TimeC = fTime;
		return LC_OK;
	}

	else if( dCmd == LC_TS_WORLD)
	{
		LCXMATRIX* pTM = (LCXMATRIX*)pData;

		m_mtWld	= *pTM;
		return LC_OK;
	}

	else if( dCmd == LCXM_LOOP_SECTION)
	{
		struct _TT { INT B; INT E; } *t =(_TT*)pData;

		m_FrameB = t->B;
		m_FrameE = t->E;

		if(m_FrameB < m_nFrmF)		m_FrameB = m_nFrmF;
		if(m_FrameB > m_nFrmL)		m_FrameB = m_nFrmL;

		if(m_FrameE < m_FrameB)	m_FrameE = m_FrameB;
		if(m_FrameE > m_nFrmL)		m_FrameE = m_nFrmL;

		return LC_OK;
	}


	else if( dCmd == LCXM_MAX_ANI_BLENDING)
	{
		m_BlndNum = *((INT*)pData);

		if(1>m_BlndNum)
			m_BlndNum = 1;

		else if(4<m_BlndNum)
			m_BlndNum = 4;


		return LC_OK;
	}


	return LC_EFAIL;
}


INT CLcxAcmIns::GetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if( dCmd == LCXM_CURRENT_TIME)
	{
		*((DOUBLE*)pData) = m_TimeC;
		return LC_OK;
	}


	else if( dCmd == LC_TS_WORLD)
	{
		*((LCXMATRIX*)pData) = m_mtWld;
		return LC_OK;
	}
	else if( dCmd == LC_TS_WORLD_PT)
	{
		*((LCXMATRIX**)pData) = &m_mtWld;
		return LC_OK;
	}


	else if( dCmd == LCXM_GEOMETRY_MATRIX)
	{
		struct _TT { INT n;	LCXMATRIX mtT; } *t = (_TT*)pData;
		INT		n = t->n;

		if(n<0 || n>=this->m_nGeo)
			return LC_CANNT_FIND_RESOURCE;

		t->mtT = m_pGeo[n].mtW;
		return LC_OK;
	}
	else if( dCmd == LCXM_GEOMETRY_MATRIX_PT)
	{
		struct _TT { INT n;	LCXMATRIX* mtT; } *t = (_TT*)pData;
		INT		n = t->n;

		if(n<0 || n>=this->m_nGeo)
			return LC_CANNT_FIND_RESOURCE;

		t->mtT = &m_pGeo[n].mtW;
		return LC_OK;
	}


	else if( dCmd == LCXM_LOOP_SECTION)
	{
		struct _TT { INT B; INT E; } t = {m_FrameB, m_FrameE};
		*((_TT*)pData) = t;
		return LC_OK;
	}

	return LC_EFAIL;
}


