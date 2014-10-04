// Implementation of the CLcAcm class.
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

#define SAFE_DELETE(p)      { if(p){ delete (p); (p) = NULL;	} }
#define SAFE_DELETE_ARRAY(p){ if(p){ delete [] (p);	(p) = NULL;	} }
#define SAFE_RELEASE(p)     { if(p){ (p)->Release(); (p) = NULL;} }


CLcAcm::LcmHeader::LcmHeader()
{
	memset(sVer, 0, sizeof sVer);
	strcpy(sVer, "00.01");
	iNmtl	= 0;			// Merterial Num
	iNgeo	= 0;			// Merterial Num
	
	nFrmF	= 0;
	nFrmL	= 0;
	nFrmS	= 0;
	nFrmT	= 0;
}

CLcAcm::TaniIfl::TaniIfl()
{
	nF	= 0;
	pT	= NULL;
}



CLcAcm::TaniIfl::TaniIfl(INT n, PDTX p)
{
	nF	= n;
	pT	= p;
}


CLcAcm::TaniTx::TaniTx()
{
	nF	= 0;
	pM	= NULL;
	pT	= NULL;
}



CLcAcm::TaniTx::TaniTx(INT n, LcMtl* pMtl)
{
	nF	= n;
	pM	= pMtl;
	pT	= NULL;
}


CLcAcm::LcMtl::LcMtl()
{
	nIdx	= -1;
	memset(&dMtl, 0, sizeof dMtl);

	dMtl.Ambient.a = 1.f;
	dMtl.Diffuse.a = 1.f;
	dMtl.Specular.a= 1.f;
	dMtl.Emissive.a= 1.f;
	dMtl.Power	   =10.f;

	memset(sDif, 0, sizeof sDif);
	memset(sNor, 0, sizeof sNor);
	memset(sSpc, 0, sizeof sSpc);
	memset(sEms, 0, sizeof sEms);

	pDif	= NULL;
	pNor	= NULL;
	pSpc	= NULL;
	pEms	= NULL;

	pvIfl	= NULL;
}



CLcAcm::LcGeo::LcGeo()
{
	memset(sName, 0, sizeof sName);
	memset(sPrnt, 0, sizeof sPrnt);

	nIdx	= -1;					// Index;
	nPrn	= -1;					// Parent Index
	nMtl	= -1;					// Material Index
	nMtlS	= 0;					// Material Type

	D3DXMatrixIdentity(&mtLcl);		// 항등 행렬로 만든다.
	D3DXMatrixIdentity(&mtWld);		// 단위행렬은 항등행렬
	D3DXMatrixIdentity(&mtOrn);
	D3DXMatrixIdentity(&mtOrI);
	D3DXMatrixIdentity(&mtPvt);

	dFVF	= 0;
	iNvtx	= 0;
	iNfce	= 0;

	pFce	= NULL;
	pVxD	= NULL;
	pVxA	= NULL;

	nBsrc	= 0;					// Bone에 영향받는 버텍스 수
	pBdst	= NULL;

	vAniRot	= NULL;
	vAniTrn	= NULL;
	vFceRnd	= NULL;

	nBlend	= 0;
}


//
///////////////////////////////////////////////////////////////////////////////

CLcAcm::CLcAcm()
{
	memset(m_sName, 0, sizeof(m_sName));

	m_pLcMtl	= NULL;
	m_pLcGeo	= NULL;

	m_dFrmCur	= 0;		// Current Frame
	m_dTimeCur	= 0;		// Current Time
	m_nActM		= 0;

	m_mtPrn		= NULL;
	m_pmtWld	= NULL;
	m_pEffct	= NULL;

	m_bTex		= TRUE;
	m_bBone		= FALSE;
	m_bModel	= TRUE;
	m_bAnima	= TRUE;

	m_dIflTime	= ::timeGetTime();
	m_dIflIntv	= (10+rand()%11)*10;

	m_pFrame	= NULL;
}


CLcAcm::~CLcAcm()
{
	Destroy();
}


void CLcAcm::Destroy()
{
	SAFE_DELETE_ARRAY(	m_pLcMtl	);
	SAFE_DELETE_ARRAY(	m_pmtWld	);

	if(m_pLcGeo)
	{
		for(INT i=0; i<m_pLcHead.iNgeo; ++i)
		{
			LcGeo* pGeo	= &m_pLcGeo[i];

			SAFE_DELETE_ARRAY(	pGeo->pFce	);
			SAFE_DELETE_ARRAY(	pGeo->pVxD	);
			SAFE_DELETE_ARRAY(	pGeo->pVxA	);
			SAFE_DELETE_ARRAY(	pGeo->pBdst	);

			pGeo->vAniRot->clear();
			pGeo->vAniTrn->clear();
			pGeo->vFceRnd->clear();

			SAFE_DELETE(	pGeo->vAniRot	);
			SAFE_DELETE(	pGeo->vAniTrn	);
			SAFE_DELETE(	pGeo->vFceRnd	);
		}

		delete [] m_pLcGeo;
		m_pLcGeo = NULL;
	}	


	map<string, PDTX>::iterator	_F = m_mpTxOrg.begin();
	map<string, PDTX>::iterator	_L = m_mpTxOrg.end();

	for(; _F != _L; ++_F)
	{
		PDTX pTex = (*_F).second;

		if(pTex)
			pTex->Release();
	}


	m_mpTxOrg.clear();
	
	SAFE_DELETE(	m_pFrame	);
}


INT CLcAcm::Create(void* p1, void* p2, void* p3, void* p4)
{
	char*	sMdl	= (char*)p1;
	PDEF	pEft	= (PDEF)p3;
	INT		i = 0;

	m_pEffct = pEft;

	if(FAILED( LoadFromFile(sMdl)))
		return -1;


	char fname[128]={0};
	char fext [ 32]={0};
	_splitpath(sMdl, NULL, NULL, fname, fext);
	sprintf(m_sName, "%s%s", fname, fext);

	if(0xFFFFFFFD == ::LoadAtc((void**)&m_pFrame, sMdl))
		return -1;


	// Rendering Geometry
	for(i=0; i<m_pLcHead.iNgeo; ++i)
	{
		LcGeo*	pDst	= &m_pLcGeo[i];

		if(!pDst->iNvtx)
			continue;

		if(VtxNDUV1::FVF != pDst->dFVF)
			continue;


		if(0 == pDst->nBsrc)
			continue;

		// Animation Setup
		VtxNDUV1i*	pVtx = pDst->pVxA;

		for(int j=0; j<pDst->iNvtx; ++j)
		{
			LcmBoneW*	pBdst = &pDst->pBdst[j];

			if(NULL == pBdst)
				continue;
			
			INT	iBone = (INT)(pBdst->vB.size());

			pDst->nBlend+=(iBone);

			for(int k=0; k<iBone && k<4; ++k)
			{
				INT		ix = pBdst->vB[k].n;
				FLOAT	fW = pBdst->vB[k].w;

				pVtx[j].w[k] = fW;
				pVtx[j].g[k] = ix;

				int c;
				c=0;
			}
		}
	}


	for(i=0; i<m_pLcHead.iNmtl; ++i)
	{
		LcMtl*	pDst = &m_pLcMtl[i];

		if(pDst->pvIfl)
		{
			TaniTx	Tani(0, pDst);
			m_vIfl.push_back(Tani);
		}
	}

	return 0;
}


INT CLcAcm::FrameMove()
{
	INT		hr		= 0;
	INT		iNgeo	= m_pLcHead.iNgeo;

	INT		i		= 0;
	INT		dFrmB	= 0;						// Begin Frame
	INT		dFrmE	= 0;						// End Frame

	if(!m_bAnima)
		return 0;


	if(NULL == m_pFrame || m_pFrame->empty())
	{
		dFrmB	= m_pLcHead.nFrmF	;
		dFrmE	= m_pLcHead.nFrmL	;
	}
	else
	{
		vector<_Tframe >::iterator	p = m_pFrame->begin() + m_nActM;
		dFrmB	= p->nB;
		dFrmE	= p->nE;
	}

	// 현재 프레임 계산 : Frame = FrameSpeed(n/s) * Time(s) + Begin Frame
	// 시간 단위: 초
	m_dFrmCur = m_pLcHead.nFrmS * m_dTimeCur + dFrmB;

	// End Frame보다 크면 재조정
	if(m_dFrmCur>= dFrmE)
	{
		// 나머지 시간을 현재시간으로 설정
		FLOAT t = FLOAT(dFrmE-dFrmB) /m_pLcHead.nFrmS;
		m_dTimeCur = m_dTimeCur - t;

		m_dFrmCur = m_pLcHead.nFrmS * m_dTimeCur + dFrmB;

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

	for(i=0; i<m_pLcHead.iNgeo; ++i)
	{
		LcGeo*	pDst	= &m_pLcGeo[i];

		MATA	mtL(1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1);
		MATA	mtPrn(1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1);


		if(pDst->nPrn>=0)
		{
			LcGeo*	pPrn	= &m_pLcGeo[pDst->nPrn];
			mtPrn	= pPrn->mtWld;
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
			pDst->mtWld = mtL * mtPrn;

			if(NULL == m_mtPrn)
				pDst->mtPvt	= pDst->mtOrI * pDst->mtWld * m_mtWld;
			else
				pDst->mtPvt	= pDst->mtOrI * pDst->mtWld * (*m_mtPrn);
		}

		m_pmtWld[i] = pDst->mtWld;						// 행렬 팔레트에 복사.
	}


	// Texture Animation
	int iSize = (INT)m_vIfl.size();

	DWORD dTimeC = ::timeGetTime();
	if(dTimeC >m_dIflTime+m_dIflIntv)
	{
		m_dIflTime = dTimeC;

		for(i=0; i<iSize; ++i)
		{
			TaniTx*	pTani = &m_vIfl[i];
			LcMtl*	pMtl = pTani->pM;

			++pTani->nF;

			INT	nTot = pMtl->pvIfl->size();

			pTani->nF %= nTot;

			vector<TaniIfl>::iterator it = pMtl->pvIfl->begin()  + pTani->nF;

			pTani->pT = it->pT;
		}
	}



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


void CLcAcm::Render()
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
		RederingHlsl(&m_mtRot, &m_mtWld, m_pmtWld, m_mtPrn);

	if(m_bBone)
		RederingBone(&m_mtWld, m_pmtWld, m_mtPrn);

	pDev->SetTransform(D3DTS_WORLD, &mtI);
}



void CLcAcm::RederingHlsl(D3DXMATRIX* pmtRot, D3DXMATRIX* pmtWldBase, D3DXMATRIX* pmtWldLst, D3DXMATRIX* pmtPrnt)
{
	HRESULT hr=-1;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	// 행렬설정
	MATA mtViw;
	MATA mtViwI;
	MATA mtPrj;
	MATA mtViwPrj;

	// 1. 디바이스에 설정된 뷰, 투영 행렬을 가져온다.
	pDev->GetTransform( D3DTS_VIEW,  &mtViw );
	pDev->GetTransform( D3DTS_PROJECTION,  &mtPrj );

	// 2. 뷰의 역행렬을 구해 카메라의 위치를 구한다.
	mtViwI =mtViw;
	D3DXMatrixInverse(&mtViwI, NULL, &mtViwI);
	VEC4	vcCam(mtViwI._41, mtViwI._42, mtViwI._43, 0);


	// 3. 이펙트에 설정할 뷰와 투영행렬의 곱을 설정한다.
	mtViwPrj = mtViw * mtPrj;

	
	hr = m_pEffct->SetTechnique( "TShader");
	hr = m_pEffct->Begin( NULL, 0 );

	// 4. 정점의 법선 벡터는 크기변환 값이 적용이 안된 회전 행렬만 적용되야 한다.
	hr = m_pEffct->SetMatrix( "g_mtRot", pmtRot);					// 회전행렬 설정
	hr = m_pEffct->SetMatrix( "g_mtViwPrj", &(mtViwPrj));			// 월드*뷰*프로젝션 행렬 설정
	hr = m_pEffct->SetVector( "g_vcCam", &vcCam);					// 카메라 위치 설정
	
	if(NULL==m_mtPrn)
		hr = pDev->SetTransform(D3DTS_WORLD, pmtWldBase);				// 고정 파이프라인에서만 필요
	else
		hr = pDev->SetTransform(D3DTS_WORLD, pmtPrnt);				// 고정 파이프라인에서만 필요

	hr = pDev->SetFVF(VtxNDUV1i::FVF);							// 고정 파이프라인에서만 필요
	
	LcGeo*	pDst	= NULL;
	PDTX	pTxDif	= NULL;
	PDTX	pTxNor	= NULL;
	PDTX	pTxSpc	= NULL;
	PDTX	pTxEms	= NULL;

	for(int i=0; i<m_pLcHead.iNgeo; ++i)
	{
		pDst	= &m_pLcGeo[i];

		if(VtxD::FVF ==  pDst->dFVF || 0 == pDst->dFVF)
			continue;

		// 행렬 팔레트를 사용하는 스키닝 애니메이션
		if(pDst->nBlend>0)
		{
			hr = m_pEffct->Pass( 0 );
			hr = m_pEffct->SetMatrixArray( "g_mtBldWld", pmtWldLst, m_pLcHead.iNgeo);

			if(NULL == pmtPrnt)
				hr = m_pEffct->SetMatrix( "g_mtWld", pmtWldBase);
			else
				hr = m_pEffct->SetMatrix( "g_mtWld", pmtPrnt);
		}

		// 계층구조 애니메이션
		else
		{
			hr = m_pEffct->Pass( 1 );

			if(NULL == pmtPrnt)
				hr = m_pEffct->SetMatrix( "g_mtWld", &(pDst->mtWld * *pmtWldBase));

			else
				hr = m_pEffct->SetMatrix( "g_mtWld", &(pDst->mtWld * *pmtPrnt));
		}



		if(	!m_bTex)
			continue;

		if(0 == pDst->nMtlS)
		{
			INT		nMtlGeo = pDst->nMtl;
			
			if(	SUCCEEDED(nMtlGeo) && nMtlGeo < m_pLcHead.iNmtl)
			{
				LcMtl* pMtl = NULL;
				pMtl = &m_pLcMtl[nMtlGeo];
				
				if(!pMtl->pvIfl)
				{
					pTxDif = pMtl->pDif;
					pTxNor = pMtl->pNor;
					pTxSpc = pMtl->pSpc;
					pTxEms = pMtl->pEms;

					// 픽셀 쉐이더에 적용할 텍스처 설정
					hr =m_pEffct->SetTexture( "m_TxDif", pTxDif);				// Diffuse map
					hr =m_pEffct->SetTexture( "m_TxNor", pTxNor);				// Normal map
					hr =m_pEffct->SetTexture( "m_TxSpc", pTxSpc);				// Specular map
					hr =m_pEffct->SetTexture( "m_TxEms", pTxEms);				// Ems map
				}

				else
				{
					hr = m_pEffct->Pass( 2 );
					
					for(int n=0; n< (INT)m_vIfl.size(); ++n)
					{
						TaniTx*	pTani = &m_vIfl[n];
						LcMtl*	pM = pTani->pM;

						if(pMtl == pM)
						{
							pTxDif = pTani->pT;
							break;
						}
					}

					
					hr =m_pEffct->SetTexture( "m_TxDif", pTxDif);				// Diffuse map
				}
			}


			pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST
									, 0, pDst->iNvtx
									, pDst->iNfce
									, pDst->pFce
									, (D3DFORMAT)VtxIdx::FVF
									, pDst->pVxA
									, sizeof(VtxNDUV1i)
									);
		}

		else
		{
			// 맥스의 경우 하나의 오브젝트에 여러 택스처를 적용해서 렌더링한는 경우가 많다.
			// 따라서 각각의 경우에 맞게 텍스처를 설정하고 렌더링
			int iSize = pDst->vFceRnd->size();

			for(int j=0; j<iSize; ++j)
			{
				vector<LcFceR>::iterator pFceR = (pDst->vFceRnd->begin() + j);

				INT		nMtlFce = pFceR->nMtl;
				
				if(	SUCCEEDED(nMtlFce) && nMtlFce < m_pLcHead.iNmtl)
				{
					LcMtl* pMtl = NULL;
					pMtl = &m_pLcMtl[nMtlFce];
					
					if(!pMtl->pvIfl)
					{
						pTxDif = pMtl->pDif;
						pTxNor = pMtl->pNor;
						pTxSpc = pMtl->pSpc;
						pTxEms = pMtl->pEms;

						// 픽셀 쉐이더에 적용할 텍스처 설정
						hr =m_pEffct->SetTexture( "m_TxDif", pTxDif);				// Diffuse map
						hr =m_pEffct->SetTexture( "m_TxNor", pTxNor);				// Normal map
						hr =m_pEffct->SetTexture( "m_TxSpc", pTxSpc);				// Specular map
						hr =m_pEffct->SetTexture( "m_TxEms", pTxEms);				// Ems map
					}

					else
					{
						hr = m_pEffct->Pass( 2 );
						
						for(int n=0; n< (INT)m_vIfl.size(); ++n)
						{
							TaniTx*	pTani = &m_vIfl[n];
							LcMtl*	pM = pTani->pM;

							if(pMtl == pM)
							{
								pTxDif = pTani->pT;
								break;
							}
						}

						
						hr =m_pEffct->SetTexture( "m_TxDif", pTxDif);				// Diffuse map
					}
				}

				pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST
										, 0, pDst->iNvtx
										, pFceR->nNum		//pDst->iNfce
										, pFceR->nBgn + pDst->pFce
										, (D3DFORMAT)VtxIdx::FVF
										, pDst->pVxA
										, sizeof(VtxNDUV1i)
										);
			}
		}


		
	}

	m_pEffct->End();

	m_pEffct->SetTexture( "m_TxDif", NULL);
	m_pEffct->SetTexture( "m_TxNor", NULL);
	m_pEffct->SetTexture( "m_TxSpc", NULL);
	m_pEffct->SetTexture( "m_TxEms", NULL);

	pDev->SetVertexShader( NULL);
	pDev->SetPixelShader( NULL);
}



void CLcAcm::RederingBone(D3DXMATRIX* pmtWldBase, D3DXMATRIX* pmtWldLst, D3DXMATRIX* pmtPrnt)
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	LcGeo*	pDst	= NULL;
	PDTX	pTxDif	= NULL;
	DWORD	dLight	=0;
	DWORD	dFill	=0;

	pDev->GetRenderState(D3DRS_LIGHTING, &dLight);
	pDev->GetRenderState(D3DRS_FILLMODE, &dFill);

	pDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	for(int i=0; i<m_pLcHead.iNgeo; ++i)
	{
		pDst	= &m_pLcGeo[i];

		if(VtxD::FVF !=  pDst->dFVF || 0 == pDst->dFVF)
			continue;

		if(NULL== pmtPrnt)
			pDev->SetTransform(D3DTS_WORLD, &(pDst->mtWld * *pmtWldBase));
		else
			pDev->SetTransform(D3DTS_WORLD, &(pDst->mtWld * *pmtPrnt));


		pDev->SetTexture(0, NULL);
		pDev->SetFVF(VtxD::FVF);
		
		pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST
										, 0, pDst->iNvtx
										, pDst->iNfce
										, pDst->pFce
										, (D3DFORMAT)VtxIdx::FVF
										, pDst->pVxD
										, sizeof(VtxD)
										);

	}

	pDev->SetRenderState(D3DRS_LIGHTING, dLight);
	pDev->SetRenderState(D3DRS_FILLMODE, dFill);
}


INT CLcAcm::Query(char* sCmd, void* pVal)
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

	else if(0==_stricmp("Texture Enable", sCmd))
	{
		m_bTex = *((INT*)pVal);
	}

	else if(0==_stricmp("Load Texture", sCmd))
	{
		char*	sFile=(char*)pVal;

		if(FAILED(LoadTexture(sFile)))
			return -1;
	}

	else
	{
		return -1;
	}

	return 0;
}



INT CLcAcm::SetAttrib(char* sCmd, void* pVal)
{
	if(SUCCEEDED( CLcMdl::Query(sCmd, pVal) ))
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
		m_dTimeCur	= FLOAT(nFrmC)/m_pLcHead.nFrmS;
	}

	else if(0==_stricmp("HLSL", sCmd))
	{
		LPD3DXEFFECT pEft = (LPD3DXEFFECT)pVal;
		m_pEffct	= pEft;
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


INT CLcAcm::GetAttrib(char* sCmd, void* pVal)
{
	if(SUCCEEDED(CLcMdl::GetAttrib(sCmd, pVal)))
		return 0;

	if( 0 ==_stricmp("Advance Time", sCmd))
	{
		float	fElapsedTime = *((float*)pVal);

		m_dTimeCur += fElapsedTime;
	}

	else if(0==_stricmp("Total Frame", sCmd))
	{
		*((INT*)pVal)= m_pLcHead.nFrmL- m_pLcHead.nFrmF;
	}

	else if(0==_stricmp("Current Frame", sCmd))
	{
		*((INT*)pVal)= INT(m_dFrmCur);
	}

	else if(0==_stricmp("HLSL", sCmd))
	{
		*((LPD3DXEFFECT*)pVal)= m_pEffct;
	}

	else if(0==_stricmp("Bone List", sCmd))
	{
		vector<_Tattach>*	p = ((vector<_Tattach>*)pVal);
		p->clear();

		for(int i=0; i<m_pLcHead.iNgeo; ++i)
		{
			LcGeo*	pDst	= &m_pLcGeo[i];

			_Tattach	v;
			v.sN	= pDst->sName;
			v.tm	= (FLOAT*)&pDst->mtPvt;
			v.nM	= pDst->nMtl;
			p->push_back(v);
		}
	}

	else if(0==_stricmp("Material List", sCmd))
	{
		vector<_Tmtl>*	p = ((vector<_Tmtl>*)pVal);
		p->clear();

		for(int i=0; i<m_pLcHead.iNmtl; ++i)
		{
			LcMtl*	pDst	= &m_pLcMtl[i];

			_Tmtl	v;
			
			v.dMtl	= &pDst->dMtl;
			v.sDif	= pDst->sDif;
			v.sNor	= pDst->sNor;
			v.sSpc	= pDst->sSpc;
			v.sEms	= pDst->sEms;
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


INT CLcAcm::LoadFromFile(char* sFile)
{
	INT		i=0;

	// 5. 파일에 기록한다.
	FILE* fp= fopen(sFile, "rb");

	if(NULL == fp)
		return -1;

	fread(&m_pLcHead, sizeof(LcmHeader), 1, fp);

	m_pLcMtl	= new LcMtl[m_pLcHead.iNmtl];
	m_pLcGeo	= new LcGeo[m_pLcHead.iNgeo];
	m_pmtWld	= new MATA [m_pLcHead.iNgeo];
	memset(m_pmtWld, 0, sizeof(MATA) * m_pLcHead.iNgeo);


	// Reading Material
	fseek(fp, LCM_HEADER_SIZE, SEEK_SET);
	
	for(i=0; i<m_pLcHead.iNmtl; ++i)
	{
		LcMtl*	pDst	= &m_pLcMtl[i];

		fread(&pDst->nIdx, sizeof(INT ), 1, fp);
		fread(&pDst->dMtl, sizeof(DMTL), 1, fp);

		fread(pDst->sDif, sizeof(char), LCM_TEX_NAME, fp);
		fread(pDst->sNor, sizeof(char), LCM_TEX_NAME, fp);
		fread(pDst->sSpc, sizeof(char), LCM_TEX_NAME, fp);
		fread(pDst->sEms, sizeof(char), LCM_TEX_NAME, fp);
	}
	

	// Reading Geometry
	for(i=0; i<m_pLcHead.iNgeo; ++i)
	{
		LcGeo*	pDst	= &m_pLcGeo[i];

		fread(pDst->sName, sizeof(char), 32, fp);
		fread(pDst->sPrnt, sizeof(char), 32, fp);

		fread(&pDst->nIdx, sizeof(INT ), 1, fp);
		fread(&pDst->nPrn, sizeof(INT ), 1, fp);
		fread(&pDst->nMtl, sizeof(INT ), 1, fp);

		fread(&pDst->dFVF, sizeof(DWORD), 1, fp);
		fread(&pDst->iNfce, sizeof(INT ), 1, fp);
		fread(&pDst->iNvtx, sizeof(INT ), 1, fp);

		if(pDst->iNfce)
		{
			pDst->pFce = new VtxIdx[pDst->iNfce];
			fread(pDst->pFce, sizeof(VtxIdx), pDst->iNfce, fp);
		}


		if(pDst->iNvtx)
		{
			if(VtxD::FVF == pDst->dFVF)
			{
				int iSize= sizeof(VtxD);

				pDst->pVxD = new VtxD[pDst->iNvtx];
				fread(pDst->pVxD, iSize, pDst->iNvtx, fp);
			}

			else if(VtxNDUV1::FVF == pDst->dFVF)
			{
				int iSize= sizeof(VtxNDUV1);

				pDst->pVxA = new VtxNDUV1i[pDst->iNvtx];

				VtxNDUV1* pVtx = new VtxNDUV1[pDst->iNvtx];
				fread(pVtx, iSize, pDst->iNvtx, fp);

				for(int j=0; j<pDst->iNvtx; ++j)
				{
					pDst->pVxA[j].p	= pVtx[j].p;
					pDst->pVxA[j].n	= pVtx[j].n;
					pDst->pVxA[j].d	= pVtx[j].d;
					pDst->pVxA[j].u	= pVtx[j].u;
					pDst->pVxA[j].v	= pVtx[j].v;
				}

				delete [] pVtx;
			}
		}


		int iSize = 0;
		fread(&iSize, sizeof(INT ), 1, fp);

		pDst->vFceRnd = new vector<LcFceR>;

		for(int j=0; j<iSize; ++j)
		{
			LcFceR	vFceRnd(0,0,0);

			fread(&vFceRnd, sizeof(LcFceR), 1, fp);
			pDst->vFceRnd->push_back(vFceRnd);
		}
	}

	// Reading Animation
	for(i=0; i<m_pLcHead.iNgeo; ++i)
	{
		LcGeo*	pDst	= &m_pLcGeo[i];

		fread(&pDst->mtLcl, sizeof(MATA), 1, fp);
		fread(&pDst->mtWld, sizeof(MATA), 1, fp);
		fread(&pDst->mtOrn, sizeof(MATA), 1, fp);
		
		D3DXMatrixInverse(&pDst->mtOrI, NULL, &pDst->mtOrn);

		if(-1 == pDst->nPrn)
		{
			D3DXMatrixIdentity(&pDst->mtLcl);
			D3DXMatrixIdentity(&pDst->mtWld);
		}

		int iSize = 0;
		fread(&iSize, sizeof(INT ), 1, fp);

		pDst->vAniRot = new vector<QUAT>;
		pDst->vAniTrn = new vector<VEC3>;

		for(int k=0; k<iSize; ++k)
		{
			QUAT	vcR(0,0,0,1);
			VEC3	vcT(0,0,0);

			fread(&vcR, sizeof(QUAT), 1, fp);
			fread(&vcT, sizeof(VEC3), 1, fp);

			pDst->vAniRot->push_back(vcR);
			pDst->vAniTrn->push_back(vcT);
		}
	}


	// Reading Vertex Weight Data
	for(i=0; i<m_pLcHead.iNgeo; ++i)
	{
		LcGeo*	pDst	= &m_pLcGeo[i];

		fread(&pDst->nBsrc, sizeof(INT ), 1, fp);

		if(0>=pDst->nBsrc)
			continue;

		pDst->pBdst = new LcmBoneW[pDst->iNvtx];

		for(int j=0; j<pDst->iNvtx; ++j)
		{
			LcmBoneW*	pBdst = &pDst->pBdst[j];
			INT			iBone = 0;

			fread(&iBone, sizeof(INT ), 1, fp);

			for(int k=0; k<iBone; ++k)
			{
				INT		nB = -1;
				FLOAT	fW = 0.f;
				fread(&nB, sizeof(INT  ), 1, fp);
				fread(&fW, sizeof(FLOAT), 1, fp);

				pBdst->vB.push_back(TboneW(nB, fW));
			}
		}
	}

	fclose(fp);

	return 0;
}



INT CLcAcm::LoadTexture(char* sTexPath)
{
	INT	i;
	int iLen = strlen(sTexPath);
	if(iLen<4)
		return -1;

	INT		n =0;
	DWORD	color	= 0x00FFFFFF;
	DWORD	Filter	= 0;

	char	sPath[MAX_PATH]={0};

	strcpy(sPath, sTexPath);

	if( '/' == sPath[iLen-1])
		sPath[iLen-1] = 0;


	for(i=0; i<m_pLcHead.iNmtl; ++i)
	{
		LcMtl*	pDst = &m_pLcMtl[i];

		char sDifPath[_MAX_PATH ]={0};
		char sDifDriv[_MAX_DRIVE]={0};
		char sDifDirc[_MAX_DIR  ]={0};
		char sDifFile[_MAX_FNAME]={0};
		char sDifExec[_MAX_EXT  ]={0};

		if(strlen(pDst->sDif)>4)
		{
			_splitpath( pDst->sDif, sDifDriv, sDifDirc, sDifFile, sDifExec);

			if(0==_stricmp(sDifExec, ".ifl"))
			{
				TextureLoadIfl(pDst, sDifDriv, sPath, sDifFile);
			}

			else
			{
				sprintf(sDifPath, "%s%s/%s", sDifDriv, sPath, pDst->sDif);
				TextureLoad(sDifPath, &pDst->pDif);
			}
		}
		else
			TextureLoad(sTexPath, &pDst->pDif, 0xFFFFFFFF);

		if(strlen(pDst->sNor)>4)
		{
			sprintf(sDifPath, "%s%s/%s", sDifDriv, sPath, pDst->sNor);
			TextureLoad(sDifPath, &pDst->pNor);
		}
		else
			TextureLoad(sTexPath, &pDst->pNor, 0xFFFFFFFF);

		if(strlen(pDst->sSpc)>4)
		{
			sprintf(sDifPath, "%s%s/%s", sDifDriv, sPath, pDst->sSpc);
			TextureLoad(sDifPath, &pDst->pSpc);
		}
		else
			TextureLoad(sTexPath, &pDst->pSpc, 0xFFFFFFFF);

		if(strlen(pDst->sEms)>4)
		{
			sprintf(sDifPath, "%s%s/%s", sDifDriv, sPath, pDst->sEms);
			TextureLoad(sDifPath, &pDst->pEms);
		}
		else
			TextureLoad(sTexPath, &pDst->pEms, 0xFF000000);
	}

	return 0;
}



INT CLcAcm::TextureLoad(char* sFile, LPDIRECT3DTEXTURE9* pTex, DWORD dFault)
{
	int hr = -1;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();


	map<string, PDTX>::iterator	it = m_mpTxOrg.find(sFile);

	if(it != m_mpTxOrg.end() )
	{
		*pTex = (*it).second;
		return 0;
	}

	
	hr = D3DXCreateTextureFromFileEx(pDev
								, sFile
								, D3DX_DEFAULT
								, D3DX_DEFAULT
								, 1
								, 0
								, D3DFMT_UNKNOWN
								, D3DPOOL_MANAGED
								, D3DX_FILTER_NONE
								, D3DX_FILTER_NONE
								, 0x00FFFFFF
								, NULL
								, NULL
								, pTex
								);

	if(FAILED(hr))
	{
		hr = D3DXCreateTexture(pDev
								, 1					// Width
								, 1					// Height
								, 1					// MipLevels
								, 0					// Usage
								, D3DFMT_A8R8G8B8	// D3DFORMAT
								, D3DPOOL_MANAGED	// D3DPOOL
								, pTex				// LPDIRECT3DTEXTURE9
								);

		if(FAILED(hr))
			return hr;

		D3DLOCKED_RECT	rc;
		(*pTex)->LockRect(0, &rc, 0, 0);
		DWORD*	pColor = (DWORD*)rc.pBits;
		*pColor	= dFault;
		(*pTex)->UnlockRect(0);
	}


	m_mpTxOrg.insert( map<string, PDTX>::value_type( sFile, *pTex));

	return hr;
}



INT CLcAcm::TextureLoadIfl(LcMtl* pMtl, char* sDriv, char* sDirc, char* sName)
{
	char sFile[_MAX_PATH]={0};
	char sPath[_MAX_PATH]={0};

	strcpy(sFile, sName);

	int iLen = strlen(sFile);

	for(int t=0; t<iLen; ++t)
	{
		if('0' <= sFile[t] && sFile[t] <= '9')
		{
			sFile[t+0] = 0;
			sFile[t+1] = 0;
			break;
		}
	}

	sprintf(sPath, "%s%s%s/%s"
					, sDriv
					, sDirc
					, sFile
					, pMtl->sDif);


	FILE*	fp=fopen(sPath, "rt");

	if(!fp)
		return -1;

	pMtl->pvIfl = new vector<TaniIfl>;


	while(!feof(fp))
	{
		char	sLine[256]={0};
		fgets(sLine, sizeof sLine, fp);

		int iL	= strlen(sLine);

		if( iL>6)
		{
			if('\n' == sLine[iL-1] || '\r' == sLine[iL-1])
				sLine[iL-1] = 0;

			INT					nI=-1;
			INT					nF=-1;
			LPDIRECT3DTEXTURE9	pTx=NULL;
			char	sTex[MAX_PATH]={0};
			char	sTmp[MAX_PATH]={0};

			sscanf(sLine, "%d %d %s", &nI, &nF, sTmp);

			sprintf(sTex, "%s%s%s/%s"
					, sDriv
					, sDirc
					, sFile
					, sTmp);

			

			if(FAILED(TextureLoad(sTex, &pTx)))
				return -1;


			CLcAcm::TaniIfl	t(nF, pTx);

			pMtl->pvIfl->push_back(t);
		}

	}

	fclose(fp);

	return 0;
}
	


