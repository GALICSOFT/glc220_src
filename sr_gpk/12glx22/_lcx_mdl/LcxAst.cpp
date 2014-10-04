// Implementation of the CLcxAst class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__BADA__)
  #include <FBaseSys.h>
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

#include "LcxAse.h"
#include "LcxAst.h"


CLcxAst::CLcxAst()
{
	m_pMtl	= NULL;
	m_pGeo	= NULL;

	m_pMtlS	= NULL;
	m_pGeoS	= NULL;

	m_TimeC		= (10 + rand()%100) /100.f;
	m_FrameC	= 0;
	m_FrameB	= 0;
	m_FrameE	= 0;

	m_mtWld.Identity();
}


CLcxAst::~CLcxAst()
{
	Destroy();
}


INT CLcxAst::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxAst::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


INT CLcxAst::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	strcpy(m_sFile, (char*)p1);

	if(LC_FAILED(Load()))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Cannot Read Model.\n");
		return LC_CANNT_FIND_RESOURCE;
	}


	if(LC_FAILED(SetupRenderData()))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Cannot Setup Rendering Data.\n");
		return LC_CANNT_ALLOC;
	}


	if(LC_FAILED(SetupAniMatrix()))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Cannot Setup Rendering Data.\n");
		return LC_CANNT_ALLOC;
	}


#if defined(_WIN32) || defined(_PC_LINUX_)
	SaveToAsmBin();
	SaveToAsmTxt();
#endif

	SAFE_DELETE_ARRAY(	m_pGeoS	);
	SAFE_DELETE_ARRAY(	m_pMtlS	);


	m_FrameB = m_Info.nFrmF;		// Begin Frame of Animation Loop
	m_FrameE = m_Info.nFrmL;		// End Frame of Animation Loop

	return LC_OK;
}


void CLcxAst::Destroy()
{
	SAFE_DELETE_ARRAY(	m_pGeo	);
	SAFE_DELETE_ARRAY(	m_pMtl	);
}



INT CLcxAst::FrameMove()
{
	INT		n = 0;
	INT		nFrame=0;

	if(NULL == m_pGeo)
		return LC_OK;


	// Modify Frame and Time in  Repeat Region
	// Frame = FrameSpeed * Time;
	if(m_FrameE == m_FrameB)
	{
		m_TimeC = DOUBLE(m_FrameB)/DOUBLE(m_Info.nFrmS);
	}
	else
	{
		nFrame	= INT(m_Info.nFrmS * m_TimeC);
		if( nFrame>= (m_FrameE+1)  ||nFrame< (m_FrameB))
			m_TimeC = DOUBLE(m_FrameB+0.00001)/DOUBLE(m_Info.nFrmS+0.0000001);
	}


	// Current Frame
	m_FrameC = FLOAT(m_Info.nFrmS * m_TimeC);


	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::Tgeo* pGeo = &m_pGeo[n];

		LCX_Ase::GetAniMatrix(&pGeo->mtL, (FLOAT)m_FrameC, pGeo);


		// bring to completion the World matrix to multiple Parent world matrix.
		if(0 <= pGeo->nPrn)
			pGeo->mtW = pGeo->mtL * m_pGeo[pGeo->nPrn].mtW;
		else
			pGeo->mtW = pGeo->mtL;	// * Identity Matrix
	}


	return LC_OK;
}




void CLcxAst::Render()
{
	if(!m_pGeo)
		return;

	PLC_DEVICE	pDev = (PLC_DEVICE)LcVar_ReadP(LC_VAR_DEVICE);

	INT			n=0;
	LCXMATRIX	mtW;


	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::Tgeo* pGeo = &m_pGeo[n];

		if(NULL == pGeo->pVtx)
			continue;

		PLC_TEXTURE pTx=NULL;

		if( 0<= pGeo->nMtl)
			pTx = m_pMtl[pGeo->nMtl].pTex;

		if(NULL == pTx)
			continue;


		mtW = pGeo->mtW * m_mtWld;

		pDev->SetTexture(0, pTx);

		pDev->SetTransform(LC_TS_WORLD, &mtW);
		pDev->SetFVF(pGeo->dFVF);
		pDev->DrawIndexedPrimitiveUP(LC_PT_TRIANGLE_LIST, pGeo->iFce*3, pGeo->pIdx, pGeo->pVtx, pGeo->dVtx);
	}

	pDev->SetTexture(0, NULL);
	pDev->SetFVF(0);
	pDev->SetTransform(LC_TS_WORLD_IDENTITY, NULL);
}



INT CLcxAst::SetAttrib(UINT dCmd, LC_HANDLE pData)
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

		if(m_FrameB < m_Info.nFrmF)	m_FrameB = m_Info.nFrmF;
		if(m_FrameB > m_Info.nFrmL)	m_FrameB = m_Info.nFrmL;

		if(m_FrameE < m_FrameB)		m_FrameE = m_FrameB;
		if(m_FrameE > m_Info.nFrmL)	m_FrameE = m_Info.nFrmL;

		return LC_OK;
	}

	else if( dCmd == LCXM_EXEC_TEXTURE_LOAD)
	{
		return LoadTexture(pData);
	}


	return LC_EFAIL;
}


INT CLcxAst::GetAttrib(UINT dCmd, LC_HANDLE pData)
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

		if(n<0 || n>=this->m_Info.nGeo)
			return LC_CANNT_FIND_RESOURCE;

		t->mtT = m_pGeoS[n].mtW;
		return LC_OK;
	}
	else if( dCmd == LCXM_GEOMETRY_MATRIX_PT)
	{
		struct _TT { INT n;	LCXMATRIX* mtT; } *t = (_TT*)pData;
		INT		n = t->n;

		if(n<0 || n>=this->m_Info.nGeo)
			return LC_CANNT_FIND_RESOURCE;

		t->mtT = &m_pGeoS[n].mtW;
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


INT CLcxAst::Load()
{
	FILE*	fp;

	fp = fopen(m_sFile, "rt");

	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;


	ParseScene(fp);
	ParseMaterial(fp);
	ParseGeometry(fp);
	ParseAnimation(fp);

	fclose(fp);

	return LC_OK;
}

