// Implementation of the CLcAcmIns class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <stdio.h>
#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"

#include "LnMdl.h"
#include "LnAcm.h"
#include "LnAcmIns.h"


#define SAFE_DELETE(p)      { if(p){ delete (p); (p) = NULL;	} }
#define SAFE_DELETE_ARRAY(p){ if(p){ delete [] (p);	(p) = NULL;	} }
#define SAFE_RELEASE(p)     { if(p){ (p)->Release(); (p) = NULL;} }


//
///////////////////////////////////////////////////////////////////////////////

CLcAcmIns::CLcAcmIns()
{
	m_pOrg		= NULL;

	m_pLcHead	= NULL;
	m_pLcMtl	= NULL;
	m_pLcGeo	= NULL;
	m_pFrame	= NULL;
	m_mpTxOrg	= NULL;

	m_mtPrn		= NULL;
	m_pmtWld	= NULL;


	m_dFrmCur	= 0;		// Current Frame
	m_dTimeCur	= 0;		// Current Time
	m_nActM		= 0;

	m_mtPrn		= NULL;

	m_pmtWld	= NULL;

	m_bBone		= FALSE;
	m_bModel	= TRUE;
	m_bAnima	= TRUE;
}


CLcAcmIns::~CLcAcmIns()
{
	Destroy();
}


void CLcAcmIns::Destroy()
{
	SAFE_DELETE_ARRAY(	m_pLcGeo	);
	SAFE_DELETE_ARRAY(	m_pmtWld	);
}


INT CLcAcmIns::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_pOrg	= (CLcAcm*)( ((CLcMdl*)p2)->GetOriginal() );

	int i=0;
	CLcAcm::LcGeo* pGeoOrg = NULL;

	m_pLcHead	= m_pOrg->GetHeader();
	m_pLcMtl	= m_pOrg->GetMtrl();
	m_mpTxOrg	= m_pOrg->GetTextures();
	m_pFrame	= m_pOrg->GetFrame();
	pGeoOrg		= m_pOrg->GetGeometry();

	m_pLcGeo	= new CLcAcm::LcGeo[m_pLcHead->iNgeo];
	m_pmtWld	= new MATA[m_pLcHead->iNgeo];

	memset(m_pmtWld, 0,			sizeof(MATA) * m_pLcHead->iNgeo);
	memcpy(m_pLcGeo, pGeoOrg,	sizeof(CLcAcm::LcGeo)* m_pLcHead->iNgeo);


	// Bound Box Setup
	TbndAB* pBnd  = (TbndAB*)m_pOrg->GetBndInf();
	if(pBnd)
		m_BndInf.Set( *pBnd->GetMin(), *pBnd->GetMax());

	return 0;
}


INT CLcAcmIns::FrameMove()
{
	INT		hr		= 0;
	INT		iNgeo	= m_pLcHead->iNgeo;

	INT		i		= 0;
	INT		dFrmB	= 0;						// Begin Frame
	INT		dFrmE	= 0;						// End Frame

	if(!m_bAnima)
		return 0;


	if(NULL == m_pFrame || m_pFrame->empty())
	{
		dFrmB	= m_pLcHead->nFrmF	;
		dFrmE	= m_pLcHead->nFrmL	;
	}
	else
	{
		vector<_Tframe >::iterator	p = m_pFrame->begin() + m_nActM;
		dFrmB	= p->nB;
		dFrmE	= p->nE;
	}

	// 현재 프레임 계산 : Frame = FrameSpeed(n/s) * Time(s) + Begin Frame
	// 시간 단위: 초
	m_dFrmCur = m_pLcHead->nFrmS * m_dTimeCur + dFrmB;

	// End Frame보다 크면 재조정
	if(m_dFrmCur>= dFrmE)
	{
		// 나머지 시간을 현재시간으로 설정
		FLOAT t = FLOAT(dFrmE-dFrmB) /m_pLcHead->nFrmS;
		m_dTimeCur = m_dTimeCur - t;

		m_dFrmCur = m_pLcHead->nFrmS * m_dTimeCur + dFrmB;

		hr		= 1;		// Return 값을 애니메이션 한 번 수행 것으로한다.
	}

	// 1. 현재 프레임, 다음 프레임, 비중을 구한다.
	// 시간에서 프레임을 구한다.
	INT		nFrmC	= INT(m_dFrmCur);			// End Frame
	FLOAT	fFrmW	= m_dFrmCur - nFrmC;		// 소수점은 비중이 된다.
	INT		nFrmN	= nFrmC+1;					// Next Frame

//	// End Frame보다 크면 재조정
//	if(1 == hr)
//		nFrmN = nFrmC;


	// 2. 애니메이션에 관련된 행렬들을 갱신한다.
	// 월드 행렬 = 지역행렬 * 부모의 월드 행렬 * 전체 월드 행렬
	// 전체 월드 행렬의 곱셈 적용은 쉐이더에서 한다.

	for(i=0; i<m_pLcHead->iNgeo; ++i)
	{
		CLcAcm::LcGeo*	pDst	= &m_pLcGeo[i];

		MATA	mtL(1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1);
		MATA	mtP(1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1);

		if(pDst->nPrn>=0)
		{
			CLcAcm::LcGeo*	pPrn = &m_pLcGeo[pDst->nPrn];
			mtP	= pPrn->mtWld;
		}

		// 프레임에 대한 행렬이 있는 경우 월드 행렬 = 지역행렬 * 부모의 월드 행렬
		// 을 적용하지 않고 바로 월드 행렬을 구한다.
		// 여기서 구한 월드 행렬은 맥스에서 구성한 장면의 기준 좌표계가 된다.
		// 각각의 Geometry의 행렬 = 해당 방향 행렬 * 지역행렬
		if(!pDst->vAniRot->empty())
		{	
			VEC3	p(0,0,0);
			QUAT	q(0,0,0,1);

			VEC3	p1 = *(pDst->vAniTrn->begin() + nFrmC);
			VEC3	p2 = *(pDst->vAniTrn->begin() + nFrmN);

			QUAT	q1 = *(pDst->vAniRot->begin() + nFrmC);
			QUAT	q2 = *(pDst->vAniRot->begin() + nFrmN);
			

			p = p1 * (1.f - fFrmW) + p2 * fFrmW;		// 위치를 선형 보간
			D3DXQuaternionSlerp(&q, &q1, &q2, fFrmW);	// 회전을 선형 보간. slerp을 이용

			D3DXMatrixRotationQuaternion(&mtL, &q);

			mtL._41 = p.x;
			mtL._42 = p.y;
			mtL._43 = p.z;

			pDst->mtWld = pDst->mtOrn * mtL;			// 맥스에서 지정된 방향 행렬을 곱해야 한다.

			if(NULL == m_mtPrn)
				pDst->mtPvt	= mtL * m_mtWld;
			else
				pDst->mtPvt	= mtL * (*m_mtPrn);
		}

		else											// 계층구조와 동일
		{
			pDst->mtWld = mtL * mtP;

			if(NULL == m_mtPrn)
				pDst->mtPvt	= pDst->mtOrI * pDst->mtWld * m_mtWld;
			else
				pDst->mtPvt	= pDst->mtOrI * pDst->mtWld * (*m_mtPrn);
		}

		m_pmtWld[i] = pDst->mtWld;						// 행렬 팔레트에 복사.
	}


//	// Texture Animation
//	int iSize = (INT)m_vIfl.size();
//
//	dTimeC = ::timeGetTime();
//	if(dTimeC >m_dIflTime+m_dIflIntv)
//	{
//		m_dIflTime = dTimeC;
//
//		for(i=0; i<iSize; ++i)
//		{
//			CLcAcm::TaniTx*	pTani = &m_vIfl[i];
//			CLcAcm::LcMtl*	pMtl = pTani->pM;
//
//			++pTani->nF;
//
//			INT	nTot = pMtl->pvIfl->size();
//
//			pTani->nF %= nTot;
//
//			vector<TaniIfl>::iterator it = pMtl->pvIfl->begin()  + pTani->nF;
//
//			pTani->pT = it->pT;
//		}
//	}



	// For Sort...(optional)
	MATA mtViwI;
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->GetTransform( D3DTS_VIEW,  &mtViwI );
	D3DXMatrixInverse(&mtViwI, NULL, &mtViwI);
	
	VEC3 vcCam	= VEC3(mtViwI._41, mtViwI._42, mtViwI._43);
	VEC3 vcZ	= VEC3(mtViwI._31, mtViwI._32, mtViwI._33);
	VEC3 vcTmp	= m_vcTrn - vcCam;

	m_fStlSrtR = D3DXVec3Dot(&vcZ, &vcTmp);
	
	return hr;
}


void CLcAcmIns::Render()
{
	HRESULT hr=-1;
	static MATA mtI(1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1);

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	pDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA);

	pDev->SetRenderState( D3DRS_ALPHAREF,  0x40 );
	pDev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);

	if(m_bModel)
		m_pOrg->RederingHlsl(&m_mtRot, &m_mtWld, m_pmtWld, m_mtPrn);

	if(m_bBone)
		m_pOrg->RederingBone(&m_mtWld, m_pmtWld, m_mtPrn);

	pDev->SetTransform(D3DTS_WORLD, &mtI);
}


INT CLcAcmIns::Query(char* sCmd, void* pVal)
{
	if(SUCCEEDED(CLcMdl::Query(sCmd, pVal)))
		return 0;

	if(0==_stricmp("Show Bone", sCmd))
	{
		m_bBone = *((INT*)pVal);
	}

	else if(0==_stricmp("Show Model", sCmd))
	{
		m_bModel = *((INT*)pVal);
	}

	else if(0==_stricmp("Animation Update", sCmd))
	{
		m_bAnima = *((INT*)pVal);
	}

	else if(0==_stricmp("Reset Material Index", sCmd))
	{
		struct _T
		{
			int nObj;
			int nMtl;
		} *t = (_T*)pVal;

		this->m_pLcGeo[t->nObj].nMtl = t->nMtl;
	}

	else
	{
		return -1;
	}

	return 0;
}



INT CLcAcmIns::SetAttrib(char* sCmd, void* pVal)
{
	if(SUCCEEDED(CLcMdl::SetAttrib(sCmd, pVal)))
		return 0;

	if(SUCCEEDED(CLcMdl::SetAttrib(sCmd, pVal)))
		return 0;


	if( 0 ==_stricmp("Advance Time", sCmd))
	{
		float	fElapsedTime = *((float*)pVal);

		m_dTimeCur += fElapsedTime;
	}

	else if(0==_stricmp("Current Frame", sCmd))
	{
		// 시간 = Frame * 1/(FPS)
		INT nFrmC = *((INT*)pVal);
		m_dTimeCur	= FLOAT(nFrmC)/m_pLcHead->nFrmS;
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

	else if(0== _stricmp("World TM Pointer", sCmd))
	{
		m_mtPrn = (D3DXMATRIX*)pVal;
	}

	else
	{
		return -1;
	}

	return 0;
}


INT CLcAcmIns::GetAttrib(char* sCmd, void* pVal)
{
	if(SUCCEEDED(CLcMdl::GetAttrib(sCmd, pVal)))
		return 0;

	else if(0==_stricmp("Total Frame", sCmd))
	{
		return m_pOrg->GetAttrib(sCmd, pVal);
	}

	else if(0==_stricmp("Current Frame", sCmd))
	{
		*((INT*)pVal)= INT(m_dFrmCur);
		return 0;
	}

	else if(0==_stricmp("Bone List", sCmd))
	{
		return m_pOrg->GetAttrib(sCmd, pVal);
	}

	else if(0==_stricmp("Material List", sCmd))
	{
		return m_pOrg->GetAttrib(sCmd, pVal);
	}

	else if(0==_stricmp("Bone Matrix", sCmd))
	{
		return 0;
	}

	else if(0==_stricmp("Frame List", sCmd))
	{
		return m_pOrg->GetAttrib(sCmd, pVal);
	}

	else
	{
		return -1;
	}

	return 0;
}