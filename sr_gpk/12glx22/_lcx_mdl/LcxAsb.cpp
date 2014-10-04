// Implementation of the CLcxAsb class.
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
#include "LcxAsb.h"


CLcxAsb::CLcxAsb()
{
	m_pMtl	= NULL;
	m_pGeo	= NULL;

	m_TimeC		= 0;
	m_FrameC	= 0;
	m_FrameB	= 0;
	m_FrameE	= 0;

	m_mtWld.Identity();
}


CLcxAsb::~CLcxAsb()
{
	Destroy();
}


INT CLcxAsb::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxAsb::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


INT CLcxAsb::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	strcpy(m_sFile, (char*)p1);

	if(LC_FAILED(Load()))
		return LC_EFAIL;

	m_FrameB = m_Info.nFrmF;		// Begin Frame of Animation Loop
	m_FrameE = m_Info.nFrmL;		// End Frame of Animation Loop

	return LC_OK;
}


void CLcxAsb::Destroy()
{
	SAFE_DELETE_ARRAY(	m_pMtl	);
	SAFE_DELETE_ARRAY(	m_pGeo	);
}



INT CLcxAsb::FrameMove()
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

		LCX_Ase::GetAniMatrix(&pGeo->mtL, m_FrameC, pGeo);


		// bring to completion the World matrix to multiple Parent world matrix.
		if(0 <= pGeo->nPrn)
			pGeo->mtW = pGeo->mtL * m_pGeo[pGeo->nPrn].mtW;
		else
			pGeo->mtW = pGeo->mtL;	// * Identity Matrix
	}


	return LC_OK;
}




void CLcxAsb::Render()
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

		pDev->SetTexture(0, 0);
	}

	pDev->SetTexture(0, NULL);
	pDev->SetFVF(0);
	pDev->SetTransform(LC_TS_WORLD_IDENTITY, NULL);
}



INT CLcxAsb::SetAttrib(UINT dCmd, LC_HANDLE pData)
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


INT CLcxAsb::GetAttrib(UINT dCmd, LC_HANDLE pData)
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

		t->mtT = m_pGeo[n].mtW;
		return LC_OK;
	}
	else if( dCmd == LCXM_GEOMETRY_MATRIX_PT)
	{
		struct _TT { INT n;	LCXMATRIX* mtT; } *t = (_TT*)pData;
		INT		n = t->n;

		if(n<0 || n>=this->m_Info.nGeo)
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


INT CLcxAsb::Load()
{
	FILE*	fp= NULL;
	INT		n=0;
	INT		nRead = 0;


	char	sMedia[LC_MAX_PATH2]={0};
	if(LC_FAILED(LcSys_ResourcePath(sMedia, m_sFile)))
		return LC_CANNT_FIND_RESOURCE;


	fp = fopen(sMedia, "rb");
	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;



	//1. Reading Header
	nRead = fread(&m_Info, sizeof(LCX_Ase::TInfo), 1, fp);	// Scene Info
	if(0>=nRead)
		return LC_INVALID_RESOURCE;


	fseek(fp, LC_RSC_HEADER, SEEK_SET);	// chunck 만큼 이동


	m_pMtl = new LCX_Ase::Tmtl[m_Info.nMtl];
	m_pGeo = new LCX_Ase::Tgeo[m_Info.nGeo];


	//2. Reading Material
	for(n=0; n<m_Info.nMtl; ++n)
	{
		LCX_Ase::Tmtl*	pDest	= &m_pMtl[n];
		nRead = fread(pDest->sTex, 1, LCXM_TEXNAME, fp);
	}


	//3. Reading Geometry Info
	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::Tgeo* pDest = &m_pGeo[n];

		nRead = fread(pDest->sName , sizeof(char ), 32, fp);		// Current Node Name
		nRead = fread(&pDest->nPrn , sizeof(INT  ),  1, fp);		// Parent Node Index
		nRead = fread(&pDest->nMtl , sizeof(INT  ),  1, fp);		// Material Index

		nRead = fread(&pDest->iFce , sizeof(INT  ),  1, fp);		// Number of Index
		nRead = fread(&pDest->iNvx , sizeof(INT  ),  1, fp);		// Number of Vertex
		nRead = fread(&pDest->dFVF , sizeof(UINT ),  1, fp);		// FVF
		nRead = fread(&pDest->dVtx , sizeof(UINT ),  1, fp);		// Zero Stride

		nRead = fread(&pDest->mtW  , sizeof(FLOAT), 16, fp);		// World Matrix
		nRead = fread(&pDest->mtL  , sizeof(FLOAT), 16, fp);		// Local Matrix
		nRead = fread(&pDest->mtTmL, sizeof(FLOAT), 16, fp);		// Transform and Movement

		nRead = fread(&pDest->nAni , sizeof(INT  ),  1, fp);		// Animation Matrix Number of Animation
	}


	//4.1 Memory Alloc
	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::Tgeo* pDest = &m_pGeo[n];

		if(pDest->iFce)
			pDest->pIdx = new LCX_Ase::Tidx[pDest->iFce];

		if(pDest->iNvx)
		{
			if(pDest->dFVF == (UINT)LCX_Ase::Tvtx::FVF)
				pDest->pVtx = new LCX_Ase::Tvtx[pDest->iNvx];

			else if(pDest->dFVF == (UINT)LCX_Ase::TvtxUV::FVF)
				pDest->pVtx = new LCX_Ase::TvtxUV[pDest->iNvx];
		}
	}


	//4.2 Reading Vertex.
	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::Tgeo* pDest = &m_pGeo[n];

		// Reading Index Buffer
		if(pDest->iFce)
			nRead = fread(pDest->pIdx, sizeof(LCX_Ase::Tidx), pDest->iFce, fp);

		if(pDest->iNvx)
			nRead = fread(pDest->pVtx, pDest->dVtx, pDest->iNvx, fp);
	}

	//6. Reading Animation Matrix
	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::Tgeo*	pDst= &m_pGeo[n];

		INT				nAni = pDst->nAni;
		LCX_Ase::Ttrack* vAni = NULL;

		if(0 >= nAni)
			continue;

		vAni = new LCX_Ase::Ttrack[nAni];
		fread(vAni, sizeof(LCX_Ase::Ttrack), nAni, fp);

		pDst->vAni = vAni;
	}


	fclose(fp);


	return LC_OK;
}


INT CLcxAsb::LoadTexture(LC_HANDLE pOption)
{
	// Setup Texture

	for(INT i=0; i<m_Info.nMtl; ++i)
	{
		LCX_Ase::Tmtl* pMtl = &m_pMtl[i];

		if(strlen(pMtl->sTex) <3)
			continue;

		char sFile	[LC_MAX_PATH ]={0};
		char drive	[LC_MAX_DRIVE]={0};
		char dir	[LC_MAX_DIR  ]={0};
		char fname	[LC_MAX_FNAME]={0};
		char ext	[LC_MAX_EXT  ]={0};

		LcStr_SplitPath( m_sFile, drive, dir, fname, ext );

		sprintf(sFile, "%s%s%s", drive,dir, pMtl->sTex);


		PLC_TEXTURE	pTex = NULL;
		if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &pTex, sFile)))
		{
			printf("Cannot Read Model Texture: %s\n", sFile);
			return LC_CANNT_FIND_RESOURCE;
		}

		pMtl->pTex = pTex;
	}

	return LC_OK;
}


