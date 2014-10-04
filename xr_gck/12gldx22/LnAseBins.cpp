// Implementation of the CLcAseBins class.
//
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <stdio.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "../11gldc22/LnD3Type.h"

#include <LnUtil.h>
#include <LnUtilDx.h>
#include <LnBase.h>


#include "LnxEntity.h"

#include "LnMdl.h"
#include "LnAseB.h"
#include "LnAseBins.h"


CLcAseBins::TlinkTm::TlinkTm()
{
	nPrn	= -1;

	D3DXMatrixIdentity(&mtW);
	D3DXMatrixIdentity(&mtL);
}


CLcAseBins::CLcAseBins()
{
	m_pOrg	= NULL;

	m_pHead	= NULL;
	m_pMtl	= NULL;
	m_pGeo	= NULL;

	m_dFrmCur = 0;
	m_dTimeCur= 0;

	D3DXMatrixIdentity(&m_mtWld);

	m_nActM		= 0;
	m_pFrame	= NULL;
}

CLcAseBins::~CLcAseBins()
{
	Destroy();
}


INT CLcAseBins::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_pOrg	= (CLcAseB*)( ((CLcMdl*)p2)->GetOriginal() );

	m_pHead	= m_pOrg->GetHeader();
	m_pMtl	= m_pOrg->GetMtrl();
	m_pGeo	= m_pOrg->GetGeometry();
	m_pFrame= m_pOrg->GetFrame();

	m_pTM = new TlinkTm[ m_pHead->nGeo]	;	// World and Animation Matrix


	// Original의 데이터 복사
	for(INT i=0; i< m_pHead->nGeo; ++i)
	{
		m_pTM[i].mtL = m_pGeo[i].mtL;
		m_pTM[i].mtW = m_pGeo[i].mtW;

		m_pTM[i].nPrn= m_pGeo[i].nPrn;
	}

	// Bound Box Setup
	TbndAB* pBnd  = (TbndAB*)m_pOrg->GetBndInf();
	if(pBnd)
		m_BndInf.Set( *pBnd->GetMin(), *pBnd->GetMax());

	return 0;
}


void CLcAseBins::Destroy()
{
	m_pOrg = NULL;

	SAFE_DELETE_ARRAY(	m_pTM	);
}






INT CLcAseBins::FrameMove()
{
	INT		hr		= 0;
	INT		dFrmB	= 0;						// Begin Frame
	INT		dFrmE	= 0;						// End Frame
	INT		i		= 0;

	
	if(NULL == m_pFrame || m_pFrame->empty())
	{
		dFrmB	= m_pHead->nFrmF	;
		dFrmE	= m_pHead->nFrmL	;
	}
	else
	{
		vector<_Tframe >::iterator	p = m_pFrame->begin() + m_nActM;
		dFrmB	= p->nB;
		dFrmE	= p->nE;
	}

	// 현재 프레임 계산 : Frame = FrameSpeed(n/s) * Time(s) + Begin Frame
	// 시간 단위: 초
	m_dFrmCur = m_pHead->nFrmS * m_dTimeCur + dFrmB;

	// End Frame보다 크면 재조정
	if(m_dFrmCur>= dFrmE)
	{
		// 나머지 시간을 현재시간으로 설정
		FLOAT t = FLOAT(dFrmE-dFrmB) /m_pHead->nFrmS;
		m_dTimeCur = m_dTimeCur - t;

		m_dFrmCur = m_pHead->nFrmS * m_dTimeCur + dFrmB;
		hr		= 1;		// Return 값을 애니메이션 한 번 수행 것으로한다.
	}

	for(i=0; i< m_pHead->nGeo; ++i)
	{
		TlinkTm*	pTM = &m_pTM[i];
		
		m_pOrg->GetAniTrack(&pTM->mtL, i, m_dFrmCur);

		// 부모의 월드 행렬을 곱해 자신의 월드 행렬을 완성한다.
		D3DXMATRIX	mtPrn =D3DXMATRIX(1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1);

		if(0 <= pTM->nPrn)
			mtPrn	= m_pTM[pTM->nPrn].mtW;

		pTM->mtW = pTM->mtL * mtPrn;
	}


	return hr;
}




void CLcAseBins::Render()
{
	if(!m_pGeo)
		return;

	INT			i=0;
	D3DXMATRIX	mtW;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDev->SetRenderState(D3DRS_ALPHAREF, 0x40);
	pDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	pDev->SetFVF(VtxUV1::FVF);


	for(i=0; i< m_pHead->nGeo; ++i)
	{
		CLcAseB::Tgeo*	pGeo = &m_pGeo[i];
		TlinkTm*		pTM	 = &m_pTM[i];	// World and Animation Matrix

		if(pGeo->dFVF != (DWORD)(VtxUV1::FVF) )
			continue;

		if(NULL == pGeo->pVtx)
			continue;

		PLC_TEXTURE	pTx=NULL;

		if( 0<= pGeo->nMtl)
			pTx = m_pMtl[pGeo->nMtl].pTex;

		if(NULL == pTx)
			continue;

		PDTX pTex = (PDTX)pTx->GetPointer();

		mtW = pTM->mtW * m_mtWld;

		pDev->SetTransform(D3DTS_WORLD, &mtW);
		pDev->SetTexture(0, pTex);

		pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST
									, 0
									, pGeo->iNvx
									, pGeo->iNix
									, pGeo->pIdx
									, (D3DFORMAT)(VtxIdx::FVF)
									, pGeo->pVtx
									, sizeof(VtxUV1)
									);

	}


	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	pDev->SetTexture(0, NULL);
	LcD3D_SetWorldIdentity(pDev);
}


INT CLcAseBins::SetAttrib(char* sCmd, void* pVal)
{
	if(SUCCEEDED(CLcMdl::SetAttrib(sCmd, pVal)))
		return 0;


	if( 0 ==_stricmp("Advance Time", sCmd))
	{
		float	fElapsedTime = *((float*)pVal);

		m_dTimeCur += fElapsedTime;
	}

	else if(0==_stricmp("Frame Index", sCmd))
	{
		INT dVal = *((INT*)pVal);

		if(dVal == m_nActM)
			return 0;

		if(NULL == m_pFrame || m_pFrame->empty())
			return 0;

		INT iSize = m_pFrame->size();

		if(dVal>=iSize || dVal<0)
			return -1;

		m_nActM = dVal;
		m_dTimeCur	= 0;
	}

	else if(0==_stricmp("Frame Index Absolute", sCmd))
	{
		INT dVal = *((INT*)pVal);

		if(NULL == m_pFrame || m_pFrame->empty())
			return 0;

		INT iSize = m_pFrame->size();

		if(dVal>=iSize || dVal<0)
			return -1;

		m_nActM = dVal;
		m_dTimeCur	= 0;
	}

	else
	{
		return -1;
	}


	return 0;
}

INT CLcAseBins::GetAttrib(char* sCmd, void* pVal)
{
	if(SUCCEEDED(CLcMdl::GetAttrib(sCmd, pVal)))
		return 0;


	if(0==_stricmp("Total Frame", sCmd))
	{
		*((INT*)pVal)= m_pHead->nFrmL - m_pHead->nFrmF;
	}

	else if(0==_stricmp("Current Frame", sCmd))
	{
		*((INT*)pVal)= INT(m_dFrmCur);
	}

	else if(0==_stricmp("Bone List", sCmd))
	{
		vector<_Tattach>*	p = ((vector<_Tattach>*)pVal);
		p->clear();


		// Original의 데이터 복사
		for(INT i=0; i< m_pHead->nGeo; ++i)
		{
			CLcAseB::Tgeo*	pDst = &m_pGeo[i];

			_Tattach	v;
			v.sN	= pDst->sName;
			v.tm	= (FLOAT*)&pDst->mtW;
			v.nM	= pDst->nMtl;
			p->push_back(v);
		}
	}

	else if(0==_stricmp("Material List", sCmd))
	{
		vector<_Tmtl>*	p = ((vector<_Tmtl>*)pVal);
		p->clear();

		for(int i=0; i< m_pHead->nMtl; ++i)
		{
			CLcAseB::Tmtl* pDst	= &m_pMtl[i];

			_Tmtl	v;
			
			v.sDif	= pDst->sTex;
			p->push_back(v);
		}
	}

	else if(0==_stricmp("Bone Matrix", sCmd))
	{
	}

	else if(0==_stricmp("Frame List", sCmd))
	{
		vector<_Tframe>*	p = ((vector<_Tframe>*)pVal);
		p->clear();

		if(NULL == m_pFrame || m_pFrame->empty())
			return 0;

		vector<_Tframe>::iterator	_F = m_pFrame->begin();
		vector<_Tframe>::iterator	_L = m_pFrame->end();

		for(; _F != _L; ++_F)
			p->push_back( _Tframe(_F->sN, _F->nB, _F->nE));

	}

	else
	{
		return -1;
	}

	return 0;
}

INT CLcAseBins::Query(char* sCmd, void* pVal)
{
	return CLcMdl::Query(sCmd, pVal);
}

