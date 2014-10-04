// Implementation of the CLcAseB class.
//
////////////////////////////////////////////////////////////////////////////////


//#define USE_OPENGL	1

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



CLcAseB::Tmtl::Tmtl()
{
	memset(sTex, 0, sizeof sTex);
	pTex = NULL;
}

CLcAseB::Tmtl::~Tmtl()
{
	SAFE_DELETE(	pTex	);
}


CLcAseB::Tgeo::Tgeo()
{
	nMtl	= -1;

	memset(sName, 0, sizeof sName);

	nPrn	= -1;
	iNix	= 0;
	iNvx	= 0;
	pIdx	= NULL;
	pVtx	= NULL;
	dFVF	= 0;
	dVtx	= 0;

	D3DXMatrixIdentity(&mtTmL);
	D3DXMatrixIdentity(&mtW);
	D3DXMatrixIdentity(&mtL);
	
	nRot	= 0;
	nTrs	= 0;
	vRot	= NULL;
	vTrs	= NULL;
}

CLcAseB::Tgeo::~Tgeo()
{
	SAFE_DELETE_ARRAY(	pIdx	);

	if(pVtx)
	{
		if(dFVF == (DWORD)Vtx::FVF)
		{
			Vtx* p = (Vtx*)pVtx;
			delete [] p;
		}

		else if(dFVF == (DWORD)VtxUV1::FVF)
		{
			VtxUV1* p = (VtxUV1*)pVtx;
			delete [] p;
		}

		pVtx = NULL;
	}

	nPrn	= -1;
	iNix	= 0;
	iNvx	= 0;
	dFVF	= 0;
	dVtx	= 0;

	SAFE_DELETE_ARRAY(	vRot	);
	SAFE_DELETE_ARRAY(	vTrs	);
}


CLcAseB::CLcAseB()
{
	memset(m_sName, 0, sizeof m_sName);

	m_pMtl	= NULL;
	m_pGeo	= NULL;

	m_dFrmCur = 0;
	m_dTimeCur= 0;

	m_nActM		= 0;
	m_pFrame	= NULL;
}

CLcAseB::~CLcAseB()
{
	Destroy();
}


INT CLcAseB::Create(void* p1, void* p2, void* p3, void* p4)
{
	INT		hr=-1;
	char*	sFile=NULL;

	sFile	=(char*)p1;

	if(!strlen(sFile))
		return -1;


//	TCHAR stFile[MAX_PATH]={0};	
//	LcUtil_GetFullPath(stFile, sFile);
//	hr  = LoadFromFile(stFile);

	hr  = LoadFromFile(sFile);

	if(FAILED(hr))
		return -1;

	// 입력 받은 파일 이름으로 이름을 설정
	char fname[128]={0};
	char fext [ 32]={0};
	LcStr_SplitPath(sFile, NULL, NULL, fname, fext);
	sprintf(m_sName, "%s%s", fname, fext);



	// Load Animation
	if(0xFFFFFFFD == LoadAtc((void**)&m_pFrame, sFile))
		return -1;

	return 0;
}


void CLcAseB::Destroy()
{
	SAFE_DELETE_ARRAY(	m_pMtl	);	
	SAFE_DELETE_ARRAY(	m_pGeo	);
	SAFE_DELETE(	m_pFrame	);
}






INT CLcAseB::FrameMove()
{
	INT		hr		= 0;
	INT		dFrmB	= 0;						// Begin Frame
	INT		dFrmE	= 0;						// End Frame
	INT		i		= 0;

	
	if(NULL == m_pFrame || m_pFrame->empty())
	{
		dFrmB	= m_Head.nFrmF	;
		dFrmE	= m_Head.nFrmL	;
	}
	else
	{
		vector<_Tframe >::iterator	p = m_pFrame->begin() + m_nActM;
		dFrmB	= p->nB;
		dFrmE	= p->nE;
	}

	// 현재 프레임 계산 : Frame = FrameSpeed(n/s) * Time(s) + Begin Frame
	// 시간 단위: 초
	m_dFrmCur = m_Head.nFrmS * m_dTimeCur + dFrmB;

	// End Frame보다 크면 재조정
	if(m_dFrmCur>= dFrmE)
	{
		// 나머지 시간을 현재시간으로 설정
		FLOAT t = FLOAT(dFrmE-dFrmB) /m_Head.nFrmS;
		m_dTimeCur = m_dTimeCur - t;

		m_dFrmCur = m_Head.nFrmS * m_dTimeCur + dFrmB;
		hr		= 1;		// Return 값을 애니메이션 한 번 수행 것으로한다.
	}

	for(i=0; i< m_Head.nGeo; ++i)
	{
		Tgeo*		pGeo = &m_pGeo[i];
		
		this->GetAniTrack(&pGeo->mtL, i, m_dFrmCur);

		// 부모의 월드 행렬을 곱해 자신의 월드 행렬을 완성한다.
		D3DXMATRIX	mtPrn =D3DXMATRIX(1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1);

		if(0 <= pGeo->nPrn)
			mtPrn	= m_pGeo[pGeo->nPrn].mtW;

		pGeo->mtW = pGeo->mtL * mtPrn;
	}


	return hr;
}




void CLcAseB::Render()
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
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pDev->SetFVF(VtxUV1::FVF);

	for(i=0; i< m_Head.nGeo; ++i)
	{
		Tgeo*	pGeo = &m_pGeo[i];

		if(pGeo->dFVF != (DWORD)VtxUV1::FVF)
			continue;

		if(NULL == pGeo->pVtx)
			continue;

		PLC_TEXTURE	pTx=NULL;

		if( 0<= pGeo->nMtl)
			pTx = m_pMtl[pGeo->nMtl].pTex;
		
		if(NULL == pTx)
			continue;

		PDTX pTex = (PDTX)pTx->GetPointer();

		mtW = pGeo->mtW * m_mtWld;
		pDev->SetTransform(D3DTS_WORLD, &mtW);

		pDev->SetTexture(0, pTex);

		pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST
									, 0
									, pGeo->iNvx
									, pGeo->iNix
									, pGeo->pIdx
									, (D3DFORMAT)VtxIdx::FVF
									, pGeo->pVtx
									, sizeof(VtxUV1)
									);
	}

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	pDev->SetTexture(0, NULL);
	LcD3D_SetWorldIdentity(pDev);
}



INT CLcAseB::SetAttrib(char* sCmd, void* pVal)
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

INT CLcAseB::GetAttrib(char* sCmd, void* pVal)
{
	if(SUCCEEDED(CLcMdl::GetAttrib(sCmd, pVal)))
		return 0;


	if(0==_stricmp("Total Frame", sCmd))
	{
		*((INT*)pVal)= m_Head.nFrmL - m_Head.nFrmF;
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
		for(INT i=0; i< m_Head.nGeo; ++i)
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

		for(int i=0; i< m_Head.nMtl; ++i)
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



INT CLcAseB::Query(char* sCmd, void* pVal)
{
	if(0==_stricmp("Load Texture", sCmd))
	{
		char*	sFile=(char*)pVal;

		if(FAILED(LoadTexture(sFile)))
			return -1;

		return 0;
	}

	return CLcMdl::Query(sCmd, pVal);
}


INT CLcAseB::GetAniTrack(void* mtOut, INT nGeo, FLOAT dFrame)
{
	INT			i=0;

	Tgeo*		pGeo	= &m_pGeo[nGeo];
	D3DXMATRIX	mtA;

	INT			iSizeR = pGeo->nRot;
	INT			iSizeP = pGeo->nTrs;

	
	D3DXMatrixIdentity(&mtA);

	
	if(iSizeR && pGeo->vRot[0].nF <=dFrame)
	{
		INT nFrm = pGeo->vRot[0].nF;
		INT nIdx;

		if(1 == iSizeR)
		{
			D3DXQUATERNION q;

			nIdx =0;

			q.x = pGeo->vRot[nIdx].x;
			q.y = pGeo->vRot[nIdx].y;
			q.z = pGeo->vRot[nIdx].z;
			q.w = pGeo->vRot[nIdx].w;

			D3DXMatrixRotationQuaternion(&mtA, &q);
		}

		else if(pGeo->vRot[iSizeR-1].nF <=dFrame)
		{
			D3DXQUATERNION q;

			nIdx = iSizeR-1;

			q.x = pGeo->vRot[nIdx].x;
			q.y = pGeo->vRot[nIdx].y;
			q.z = pGeo->vRot[nIdx].z;
			q.w = pGeo->vRot[nIdx].w;

			D3DXMatrixRotationQuaternion(&mtA, &q);
		}

		else
		{
			for(i=0; i<iSizeR-1; ++i)
			{
				if(pGeo->vRot[i].nF <=dFrame && dFrame <pGeo->vRot[i+1].nF)
				{
					D3DXQUATERNION q;
					D3DXQUATERNION q1;
					D3DXQUATERNION q2;

					nIdx = i;

					q1.x = pGeo->vRot[nIdx].x;
					q1.y = pGeo->vRot[nIdx].y;
					q1.z = pGeo->vRot[nIdx].z;
					q1.w = pGeo->vRot[nIdx].w;

					q2.x = pGeo->vRot[nIdx+1].x;
					q2.y = pGeo->vRot[nIdx+1].y;
					q2.z = pGeo->vRot[nIdx+1].z;
					q2.w = pGeo->vRot[nIdx+1].w;

					FLOAT	w = (dFrame - pGeo->vRot[i].nF)/(pGeo->vRot[i+1].nF- pGeo->vRot[i].nF);

					D3DXQuaternionSlerp(&q, &q1, &q2, w);
					D3DXMatrixRotationQuaternion(&mtA, &q);

					break;
				}
			}
		}
	}

	else
	{
		mtA = pGeo->mtTmL;
	}


	if(iSizeP && pGeo->vTrs[0].nF <=dFrame)
	{
		INT nIdx;

		if(1 == iSizeP)
		{
			D3DXVECTOR3 p;

			nIdx =0;
			p.x = pGeo->vTrs[nIdx].x;
			p.y = pGeo->vTrs[nIdx].y;
			p.z = pGeo->vTrs[nIdx].z;

			mtA._41 = p.x;
			mtA._42 = p.y;
			mtA._43 = p.z;
		}

		else if(pGeo->vTrs[iSizeP-1].nF <=dFrame)
		{
			D3DXVECTOR3 p;

			nIdx = iSizeP-1;
			p.x = pGeo->vTrs[nIdx].x;
			p.y = pGeo->vTrs[nIdx].y;
			p.z = pGeo->vTrs[nIdx].z;

			mtA._41 = p.x;
			mtA._42 = p.y;
			mtA._43 = p.z;
		}

		else
		{
			for(i=0; i<iSizeP-1; ++i)
			{
				if(pGeo->vTrs[i].nF <=dFrame && dFrame <pGeo->vTrs[i+1].nF)
				{
					D3DXVECTOR3 p;
					D3DXVECTOR3 p1;
					D3DXVECTOR3 p2;

					nIdx = i;

					p1.x = pGeo->vTrs[nIdx].x;
					p1.y = pGeo->vTrs[nIdx].y;
					p1.z = pGeo->vTrs[nIdx].z;

					p2.x = pGeo->vTrs[nIdx+1].x;
					p2.y = pGeo->vTrs[nIdx+1].y;
					p2.z = pGeo->vTrs[nIdx+1].z;


					FLOAT	w = (dFrame- pGeo->vTrs[i].nF)/(pGeo->vTrs[i+1].nF- pGeo->vTrs[i].nF);

					p = p1  + w * (p2-p1);
					mtA._41 = p.x;	mtA._42 = p.y;	mtA._43 = p.z;
					break;
				}
			}
		}
	}
	else
	{
		mtA._41 = pGeo->mtTmL._41;
		mtA._42 = pGeo->mtTmL._42;
		mtA._43 = pGeo->mtTmL._43;
	}


	*((D3DXMATRIX*) mtOut) = mtA;
	
	return 0;
}






INT CLcAseB::LoadFromFile(char* sPath)
{
	FILE*	fp;
	INT		n=0;
	INT		j=0;

	fp= fopen(sPath, "rb");

	if(NULL == fp)
		return -1;


	//1. Reading Header
	fread(&m_Head, sizeof(Thead), 1, fp);	// Scene Info

	fseek(fp, LCM_HEADER_SIZE, SEEK_SET);		// chunck 만큼 이동


	m_pMtl = new Tmtl[ m_Head.nMtl];
	m_pGeo = new Tgeo[ m_Head.nGeo];


	//2. Reading Material
	for(n=0; n< m_Head.nMtl; ++n)
	{
		Tmtl*	pDest	= &m_pMtl[n];
		fread(pDest->sTex, 1, LCM_TEX_NAME, fp);
	}


	//3. Reading Geometry Info
	for(n=0; n< m_Head.nGeo; ++n)
	{
		Tgeo*	pDest	= &m_pGeo[n];

		INT	nAniRot = 0;
		INT	nAniTrs = 0;


		fread(pDest->sName	, sizeof(char ), 32, fp);		// Current Node Name
		fread(&pDest->nPrn	, sizeof(INT  ),  1, fp);		// Parent Node Index
		fread(&pDest->nMtl	, sizeof(INT  ),  1, fp);		// Material Index

		fread(&pDest->iNix	, sizeof(INT  ),  1, fp);		// Number of Index
		fread(&pDest->iNvx	, sizeof(INT  ),  1, fp);		// Number of Vertex
		fread(&pDest->dFVF	, sizeof(DWORD),  1, fp);		// FVF
		fread(&pDest->dVtx	, sizeof(DWORD),  1, fp);		// Zero Stride

		fread(&pDest->mtW	, sizeof(FLOAT), 16, fp);		// World Matrix
		fread(&pDest->mtL	, sizeof(FLOAT), 16, fp);		// Local Matrix
		fread(&pDest->mtTmL	, sizeof(FLOAT), 16, fp);		// Transform and Movement

		fread(&pDest->nRot	, sizeof(INT  ),  1, fp);		// Rotaion Number of Animation
		fread(&pDest->nTrs	, sizeof(INT  ),  1, fp);		// Translation Number of Animation
	}


	//4. Memory Alloc
	for(n=0; n< m_Head.nGeo; ++n)
	{
		Tgeo*	pDest	= &m_pGeo[n];

		if(pDest->iNix)
			pDest->pIdx = new VtxIdx[pDest->iNix];

		if(pDest->iNvx)
		{
			if(pDest->dFVF == (DWORD)Vtx::FVF)
				pDest->pVtx = new Vtx[pDest->iNvx];

			else if(pDest->dFVF == (DWORD)VtxUV1::FVF)
				pDest->pVtx = new VtxUV1[pDest->iNvx];
		}

		if(pDest->nRot)
			pDest->vRot = new Ttrack[pDest->nRot];

		if(pDest->nTrs)
			pDest->vTrs= new Ttrack[pDest->nTrs];
	}


	//5. Reading Vertex.
	for(n=0; n< m_Head.nGeo; ++n)
	{
		Tgeo*	pDest	= &m_pGeo[n];

		// Reading Index Buffer
		if(pDest->pIdx)
			fread(pDest->pIdx, sizeof(VtxIdx), pDest->iNix, fp);
	
		if(pDest->pVtx)
			fread(pDest->pVtx, pDest->dVtx, pDest->iNvx, fp);
	}


	//6. Reading Animation.
	for(n=0; n< m_Head.nGeo; ++n)
	{
		Tgeo*	pDest	= &m_pGeo[n];

		// Reading Rotation
		if(0< pDest->nRot)
			fread(pDest->vRot, sizeof(Ttrack), pDest->nRot, fp);

		// Reading Translation
		if(0< pDest->nTrs)
			fread(pDest->vTrs, sizeof(Ttrack), pDest->nTrs, fp);
	}

	fclose(fp);


#if defined(USE_OPENGL) && USE_OPENGL

	//7. UV 조정 <== for OpenGL
	for(n=0; n< m_Head.nGeo; ++n)
	{
		Tgeo*	pDest	= &m_pGeo[n];

		if(pDest->dFVF != (DWORD)VtxUV1::FVF || NULL == pDest->pVtx)
			continue;

		VtxUV1*	pVtx = (VtxUV1*)pDest->pVtx;

		for(j=0;j<pDest->iNvx; ++j)
		{
			pVtx[j].v = 1.f - pVtx[j].v;
		}
	}

#endif
	
	return 0;
}




INT CLcAseB::LoadTexture(char* sTexPath)
{
	if(NULL == m_pMtl)
		return 0;

	int iLen = strlen(sTexPath);
	if(iLen<4)
		return -1;


	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	INT		n =0;
	DWORD	color	= 0x00FFFFFF;
	DWORD	Filter	= 0;

	char	sPath[MAX_PATH]={0};
	char	sTex[MAX_PATH];

	strcpy(sPath, sTexPath);

	if( '/' == sPath[iLen-1])
		sPath[iLen-1] = 0;


	for(n=0; n< m_Head.nMtl; ++n)
	{
		Tmtl*		pMtl = &m_pMtl[n];

		char sFile[_MAX_PATH]={0};
		sprintf(sTex, "%s/%s", sPath, pMtl->sTex);


		PLC_TEXTURE		pTex = NULL;
		if(FAILED(LcDev_CreateTex("File", &pTex, sTex, 0x00FFFFFF, (D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR))))
			return -1;

		pMtl->pTex = pTex;
	}

	return 0;
}



