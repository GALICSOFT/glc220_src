// Implementation of the CLcxAsbIns class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
#else
  #include "../LcxEntity.h"
#endif

#include "LcxAse.h"
#include "LcxAst.h"
#include "LcxAsb.h"
#include "LcxAsbIns.h"


CLcxAsbIns::CLcxAsbIns()
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
}


CLcxAsbIns::~CLcxAsbIns()
{
	Destroy();
}

INT CLcxAsbIns::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxAsbIns::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


INT CLcxAsbIns::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	LCX_Ase::TInfo*	pInfo;
	m_pOrg	= (CLcxAsb*)p1;

	pInfo	= (LCX_Ase::TInfo*)m_pOrg->GetInfo();


	m_nFrmF	= pInfo->nFrmF;		// First Frame
	m_nFrmL	= pInfo->nFrmL;		// Last Frame
	m_nFrmS	= pInfo->nFrmS;		// Frames per one scecond
	m_nFrmT	= pInfo->nFrmT;		// Tick per Frame

	m_FrameB = m_nFrmF;		// 애니메이션 루프의 시작 프레임
	m_FrameE = m_nFrmL;		// 애니메이션 루프의 마지막 프레임

	m_nMtl	= pInfo->nMtl;
	m_nGeo	= pInfo->nGeo;

	m_pMtl	= m_pOrg->GetMtrl();
	m_pGeo	= m_pOrg->GetGeometry();


	m_pTM = new LCX_Ase::TlinkTm[m_nGeo]	;	// World and Animation Matrix


	// Original의 데이터 복사
	for(INT n=0; n<m_nGeo; ++n)
	{
		m_pTM[n].mtL = m_pGeo[n].mtL;
		m_pTM[n].mtW = m_pGeo[n].mtW;

		m_pTM[n].nPrn= m_pGeo[n].nPrn;
	}

	return LC_OK;
}


void CLcxAsbIns::Destroy()
{
	m_pOrg = NULL;

	SAFE_DELETE_ARRAY(	m_pTM	);
}






INT CLcxAsbIns::FrameMove()
{
	INT		n = 0;
	INT		nFrame=0;

	if(NULL == m_pGeo)
		return LC_OK;


	// Modify Frame and Time in  Repeat Region
	// Frame = FrameSpeed * Time;
	if(m_FrameE == m_FrameB)
	{
		m_TimeC = DOUBLE(m_FrameB)/DOUBLE(m_nFrmS);
	}
	else
	{
		nFrame	= INT(m_nFrmS * m_TimeC);
		if( nFrame>= (m_FrameE+1)  ||nFrame< (m_FrameB))
			m_TimeC = DOUBLE(m_FrameB+0.00001)/DOUBLE(m_nFrmS+0.0000001);
	}


	// Current Frame
	m_FrameC = FLOAT(m_nFrmS * m_TimeC);


	for(n=0; n<m_nGeo; ++n)
	{
		LCX_Ase::TlinkTm* pTM = &m_pTM[n];
		LCX_Ase::Tgeo*	 pGeo = &m_pGeo[n];

		LCX_Ase::GetAniMatrix(&pTM->mtL, m_FrameC, pGeo);


		// bring to completion the World matrix to multiple Parent world matrix.
		if(0 <= pTM->nPrn)
			pTM->mtW = pTM->mtL * m_pTM[pTM->nPrn].mtW;
		else
			pTM->mtW = pTM->mtL;	// * Identity Matrix
	}


	return LC_OK;
}




void CLcxAsbIns::Render()
{
	if(!m_pGeo)
		return;

	PLC_DEVICE	pDev = (PLC_DEVICE)LcVar_ReadP(LC_VAR_DEVICE);

	INT			n=0;
	LCXMATRIX	mtW;


	for(n=0; n<m_nGeo; ++n)
	{
		LCX_Ase::Tgeo* pGeo = &m_pGeo[n];

		if(NULL == pGeo->pVtx)
			continue;

		PLC_TEXTURE pTx=NULL;

		if(0 <= pGeo->nMtl)
			pTx = m_pMtl[pGeo->nMtl].pTex;

		if(NULL == pTx)
			continue;


		mtW = m_pTM[n].mtW * m_mtWld;

		pDev->SetTexture(0, pTx);

		pDev->SetTransform(LC_TS_WORLD, &mtW);
		pDev->SetFVF(pGeo->dFVF);
		pDev->DrawIndexedPrimitiveUP(LC_PT_TRIANGLE_LIST, pGeo->iFce*3, pGeo->pIdx, pGeo->pVtx, pGeo->dVtx);

		pDev->SetTexture(0, 0);
	}

	pDev->SetTexture(0, NULL);
	pDev->SetFVF(0);
	pDev->SetTransform(LC_TS_WORLD_IDENTITY, NULL);
}



INT CLcxAsbIns::SetAttrib(UINT dCmd, LC_HANDLE pData)
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

	return LC_EFAIL;
}


INT CLcxAsbIns::GetAttrib(UINT dCmd, LC_HANDLE pData)
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


