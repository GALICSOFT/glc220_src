// Implementation of the CLcmTileD class.
//
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>

#include <windows.h>
#include <d3d9.h>
#include <d3dX9.h>

#include "../11gldc22/LnD3Type.h"

#include <LnUtil.h>
#include <LnUtilDx.h>
#include <LnEuclid.h>

#include <LnBase.h>
#include <LnxEntity.h>

#include "LntEntity.h"

#include "LnmTileB.h"
#include "LnmTileD.h"


CLcmTileD::_VecT::_VecT()
: iZ(-1)
, iX(-1)
, iK(-1)
, p(0,0,0)
, iE(-1)
{
}


CLcmTileD::_VecT::_VecT(INT Z, INT X, INT K, const VEC3& P, INT E)
: iZ(Z)
, iX(X)
, iK(K)
, p(P)
, iE(E)
{
}


CLcmTileD::_Pck::_Pck()
: vcPk(0,0,0)
, p0(0,0,0)
, p1(0,0,0)
, p2(0,0,0)
, fStlSrtR(FLT_MAX){}

CLcmTileD::_Pck::_Pck(VEC3& vcP, VEC3& P0, VEC3& P1, VEC3& P2, FLOAT& _fD)
: vcPk(vcP)
, p0(P0)
, p1(P1)
, p2(P2)
, fStlSrtR(_fD)
{
}


CLcmTileD::_Inf::_Inf()
{
	nI = -1;
	memset(sN, 0, sizeof(sN));
}


CLcmTileD::_Tex::_Tex()
{
	pTex	= NULL;
	memset(&Image, 0, sizeof Image);
	memset(sName, 0, sizeof sName);
}


CLcmTileD::_Tex::_Tex(const _Tex& r)
{
	pTex	= r.pTex;
	memcpy(&Image, &r.Image, sizeof Image);
	strcpy(sName, r.sName);
}

CLcmTileD::_Tex::_Tex(const _Tex* r)
{
	if(r)
	{
		pTex	= r->pTex;
		memcpy(&Image, &r->Image, sizeof Image);
		strcpy(sName, r->sName);
	}
	else
	{
		pTex	= NULL;
		memset(&Image, 0, sizeof Image);
		memset(sName, 0, sizeof sName);
	}
}

void CLcmTileD::_Tex::operator =(const _Tex& r)
{
	pTex	= r.pTex;
	memcpy(&Image, &r.Image, sizeof Image);
	strcpy(sName, r.sName);
}

void CLcmTileD::_Tex::operator =(const _Tex* r)
{
	if(r)
	{
		pTex	= r->pTex;
		memcpy(&Image, &r->Image, sizeof Image);
		strcpy(sName, r->sName);
	}
	else
	{
		pTex	= NULL;
		memset(&Image, 0, sizeof Image);
		memset(sName, 0, sizeof sName);
	}
}






CLcmTileD::CLcmTileD()
{
	m_pEft		= NULL;
	m_pFVF		= NULL;

	m_ppLcl 	= NULL;

	m_pPkLc 	= NULL;				// Pick Local
	m_bClck 	= FALSE;

	m_vcPck		= VEC3(0,0,0);


	m_fCull		= 6000;

	m_bCtrlCull	= TRUE;
	m_bCtrlPck	= FALSE;

	m_bCtrlHeight	= FALSE;
	m_bCtrlSpoid	= FALSE;
	m_dFillColor	= 0x0;

	m_bCtrlBrush	= FALSE;
	m_bCtrlModel	= FALSE;
	m_bCtrlTileFront= FALSE;
	m_bCtrlTileBack	= FALSE;

	m_bCtrlBndBox	= FALSE;
	m_bCtrlLclLine	= FALSE;

	m_pMdB			= NULL;
	m_pCam			= NULL;
	m_pTbMdl		= NULL;

	m_fRangeMax		= 6.f;


	m_nTile			= -1;
}


CLcmTileD::~CLcmTileD()
{
	Destroy();
}


void CLcmTileD::Destroy()
{
	if(m_ppLcl)
	{
		for(int z=0; z<m_Inf.iNfZ; ++z)
			SAFE_DELETE_ARRAY(	m_ppLcl[z]	);

		SAFE_DELETE_ARRAY(	m_ppLcl );
	}

	SAFE_RELEASE(	m_pFVF	);

	m_vObj.clear();



	SAFE_RELEASE(	m_pTex.pTex	);
}


INT CLcmTileD::Create(void* p1, void* p2, void* p3, void* p4)
{
	INT	hr	= 0;
	char* sFile	= NULL;

	sFile		= (char*  )p1;
	m_pCam		= (ILcCam*)p2;
	m_pTbMdl	= (ILcTbl*)p3;

	if(sFile)
	{
		hr = FileLoad(sFile);

		if(FAILED(hr))
			return -1;


		char sPath[MAX_PATH]={0};
		char drive[_MAX_DRIVE]={0};
		char dir[_MAX_DIR]={0};

		_splitpath(sFile, drive, dir, NULL, NULL);
		sprintf(sPath, "%s%s", drive, dir);

		hr = LoadTexture(sPath);

		if(FAILED(hr))
			return -1;
	}


	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	DWORD				dFVF = CLcmTileB::_VtxDX::FVF;
	D3DVERTEXELEMENT9	pDcl[MAX_FVF_DECL_SIZE]={0};

	D3DXDeclaratorFromFVF(dFVF, pDcl);

	if(FAILED(pDev->CreateVertexDeclaration( pDcl, &m_pFVF)))
		return -1;

	return 0;
}


INT CLcmTileD::FrameMove()
{
	INT		z, x;
	VEC3	vcEye;

	m_vObj.clear();


	vcEye = *m_pCam->GetEye();

	if(m_ppLcl)
	{
		// 1. 카메라의 위치에 따른 렌더링 블록을 선택, 렌더링 결정
		for(z=0; z<m_Inf.iNfZ; ++z)
		{
			for(x=0; x<m_Inf.iNfX; ++x)
			{
				INT	iNx	= m_ppLcl[z][x].GetTileN();			// Number of tile for Width
				INT	iWx	= m_ppLcl[z][x].GetTileW();			// Width of tile for x;

				FLOAT fBlockWidth = iNx * iWx;
				FLOAT fRangeView;							// View Range

				fRangeView	= m_fRangeMax* fBlockWidth;
//				fRangeView	*= fRangeView;

				VEC3	vcCenter = m_ppLcl[z][x].m_vcP + VEC3(1,0,1) * fBlockWidth/2.F;
				VEC3	vcT = vcCenter- vcEye;
				FLOAT fLen = D3DXVec3Length(&vcT);

				if(fLen>= fRangeView)
					m_ppLcl[z][x].m_bRend = FALSE;

				else
					m_ppLcl[z][x].m_bRend = TRUE;
			}
		}

		// 2. Field와 Local의 Picking처리
		PickField();
		PickLocal();


		// 3. Data 갱신
		for(z=0; z<m_Inf.iNfZ; ++z)
		{
			for(x=0; x<m_Inf.iNfX; ++x)
			{
				if(FALSE == m_ppLcl[z][x].m_bRend)
					continue;

				m_ppLcl[z][x].FrameMove();
			}
		}
	}

	m_bCtrlPck = FALSE;
	std::sort(m_vObj.begin(), m_vObj.end(), TsrtGpf<ILcMdl* >());


//	int iSize = (INT)m_vObj.size();
//	TCHAR sBuf[128]={0};
//	_tprintf(sBuf, _T("%d"), iSize);
//	SetWindowTextW(GetActiveWindow(), sBuf);

	return 0;
}



void CLcmTileD::Render()
{
	INT x, z;
	INT	i;
	INT iSize;

	if(!m_ppLcl)
		return;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	LcD3D_SetWorldIdentity(pDev);

	pDev->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE);


	if(m_ppLcl)
	{
		for(z=0; z<m_Inf.iNfZ; ++z)
		{
			for(x=0; x<m_Inf.iNfX; ++x)
			{
				m_ppLcl[z][x].RenderBlc();
			}
		}


		for(z=0; z<m_Inf.iNfZ; ++z)
		{
			for(x=0; x<m_Inf.iNfX; ++x)
			{
				m_ppLcl[z][x].RenderTline();
			}
		}
	}


	iSize = (INT)m_vObj.size();
	for(i=0; i<iSize; ++i)
	{
		ILcMdl*	pMdl = m_vObj[i];

		if(m_bCtrlBndBox)
			pMdl->Query("bound box render", NULL);

		pMdl->Render();
	}


	RenderPck();
}


INT CLcmTileD::Query(char* sCmd, void* pData)
{
	if(0==_stricmp("Get Pick Position",	sCmd))
	{
		// Picking position, Ray Begin Position, Ray Direction
		struct _T{	VEC3* P;	VEC3* R;	VEC3* D;	};
		_T* t = (_T*)pData;

		if(FAILED(PickPosition(NULL, NULL, t->P, t->R, t->D)))
			return -1;

		return 0;
	}

	else if(0==_stricmp("Set Camera", sCmd))
	{
		m_pCam	= (ILcCam*)pData;
		return 0;
	}

	else if(0==_stricmp("Get Camera", sCmd))
	{
		*((ILcCam**)pData) = m_pCam;

		return 0;
	}

	else if(0==_stricmp("Set Ray Direction", 	sCmd))
	{
		m_vcRayDir = *((VEC3*)pData);
		return 0;
	}

	else if(0==_stricmp("Set Camera Position", 	sCmd))
	{
		m_vcCamPos = *((VEC3*)pData);
		return 0;
	}

	else if(0==_stricmp("Set Culling Enable", 	sCmd))
	{
		m_bCtrlCull	= *((BOOL*)pData);
		return 0;
	}

	else if(0==_stricmp("Set Culling Range", 	sCmd))
	{
		m_fCull	= *((FLOAT*)pData);
		return 0;
	}

	else if(0==_stricmp("Set Picking Enable", 	sCmd))
	{
		m_bCtrlPck = *((BOOL*)pData);
		return 0;
	}


	else if(0==_stricmp("Set Height Enable", 	sCmd))
	{
		m_bCtrlHeight = *((BOOL*)pData);
		return 0;
	}

	else if(0==_stricmp("Spoid Enable", 		sCmd))
	{
		m_bCtrlSpoid = *((BOOL*)pData);
		return 0;
	}

	else if(0==_stricmp("Brush Enable", 		sCmd))
	{
		m_bCtrlBrush = *((BOOL*)pData);
		return 0;
	}

	else if(0==_stricmp("Model Setup Enable", 	sCmd))
	{
		m_bCtrlModel = *((BOOL*)pData);
		return 0;
	}

	else if(0==_stricmp("Set Model Pointer", 	sCmd))
	{
		ILcMdl* pMdl = (ILcMdl*)pData;
		m_pMdB = pMdl;
		return 0;
	}

	else if(0==_stricmp("Front Tiling Enable", 		sCmd))
	{
		m_bCtrlTileFront = *((BOOL*)pData);
		return 0;
	}

	else if(0==_stricmp("Back Tiling Enable", 		sCmd))
	{
		m_bCtrlTileBack = *((BOOL*)pData);
		return 0;
	}

	else if(0==_stricmp("Bound Box Enable", 	sCmd))
	{
		m_bCtrlBndBox = *((BOOL*)pData);
		return 0;
	}

	else if(0==_stricmp("Show Local Enable", 	sCmd))
	{
		m_bCtrlLclLine = *((BOOL*)pData);
		return 0;
	}

	else if(0==_stricmp("Set Fill Color", 		sCmd))
	{
		m_dFillColor = *((DWORD*)pData);
		return 0;
	}

	else if(0==_stricmp("Set Field Info",	 	sCmd))
	{
		memcpy(&m_Hgt, pData, sizeof(CLcmTileD::_Hgt));
		return 0;
	}

	else if(0==_stricmp("Get Fill Color", 		sCmd))
	{
		*((DWORD*)pData) = m_dFillColor;
		return 0;
	}

	else if(0==_stricmp("File Load", 			sCmd))
	{
		char*	sFile = (char*)pData;
		INT hr = this->FileLoad(sFile);
		return hr;
	}

	else if(0==_stricmp("Set Info", 			sCmd))
	{
		memcpy(&m_Inf, pData, sizeof(CLcmTileD::_Inf));
		return 0;
	}

	else if(0==_stricmp("Get Info", 			sCmd))
	{
		memcpy(pData, &m_Inf, sizeof(CLcmTileD::_Inf));
		return 0;
	}

	else if(0==_stricmp("Create Field", 		sCmd))
	{
		struct _T{	CLcmTileD::_Inf* I; CLcmTileD::_Tex* L;};
		_T* t = (_T*)pData;

		CreateField(t->I, t->L);
		return 0;
	}

	else if(0==_stricmp("Get Field Name", 		sCmd))
	{
		strcpy((char*)pData, m_Inf.sN);
		return 0;
	}

	else if(0==_stricmp("Set HLSL", 		sCmd))
	{
		m_pEft = (PDEF)pData;
		return 0;
	}

	else if(0==_stricmp("Set View Range",		sCmd))
	{
		m_fRangeMax =*((FLOAT*)pData);
		return 0;
	}

	else if(0==_stricmp("Get View Range",		sCmd))
	{
		*((FLOAT*)pData) = m_fRangeMax;
		return 0;
	}

	else if(0==_stricmp("Set Tile Index", sCmd))
	{
		m_nTile = *((INT*)pData);
	}

	else if(0==_stricmp("Setup Texture", sCmd))
	{
		this->SetupTexture((CLcmTileD::_Tex*)pData);
		return 0;
	}

	return -1;
}





INT CLcmTileD::PickField()
{
	if(!m_ppLcl)
		return -1;

	if(!m_bCtrlPck)
		return -1;

	// Picking Local Area
	m_pPkLc = NULL;
	m_vPkLc.clear();

	if(FAILED(PickPosition(&m_pPkLc, &m_vPkLc, &m_vcPck, &m_vcCamPos, &m_vcRayDir)))
		return -1;

	m_bClck = TRUE;

	return 0;
}


INT CLcmTileD::PickPosition(CLcmTileB**			pBlock
							, CLcmTileD::lsTlPkT*	pPickInf	// Picking Information
							, VEC3*				pvcOut		// Picking position
							, const VEC3*		pvcRayPos	// Ray Begin Position
							, const VEC3*		pvcRayDir	// Ray Direction
							)
{
	INT		z;
	INT		x;
	INT		i;

	VEC3		vcPick	;
	CLcmTileD::lsTlPkT	vPkLc	;			// Picking Triangle from Local

	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			CLcmTileB* pLcl = &m_ppLcl[z][x];

			for(i=0; i<pLcl->m_nVtx; i+=3)
			{
				VEC3	vcPk;
				VEC3	p0 = pLcl->m_pVtx[i+0].p;
				VEC3	p1 = pLcl->m_pVtx[i+1].p;
				VEC3	p2 = pLcl->m_pVtx[i+2].p;
				FLOAT	fD;

				if(SUCCEEDED(LcMath_LineToTriangle(&vcPk, &fD, pvcRayPos, pvcRayDir, &p0, &p1, &p2)))
					vPkLc.push_back(CLcmTileD::_Pck(vcPk, p0, p1, p2, fD));
			}
		}
	}

	if(vPkLc.empty())
		return -1;


	std::sort(vPkLc.begin(), vPkLc.end(), TsrtL<CLcmTileD::_Pck >());
	vcPick = vPkLc[0].vcPk;

	x = INT(vcPick.x/m_Inf.iExt);
	z = INT(vcPick.z/m_Inf.iExt);

	if(pBlock)
		*pBlock = &m_ppLcl[z][x];

	if(pPickInf)
		pPickInf->push_back( vPkLc[0] );

	if(pvcOut)
		*pvcOut = vcPick;

	return 0;
}


INT CLcmTileD::PickLocal()
{
	if(!m_pPkLc)
		return -1;

	if(!m_bClck)
		return 0;

	if(m_bCtrlHeight)
	{
		SetupHeight();
	}

	if(m_bCtrlSpoid)
	{
		SetupSpoid();
	}

	if(m_bCtrlBrush)
	{
		SetupBrush();
	}

	if(m_bCtrlModel)
	{
		SetupModel();
	}


	// Tiling
	if(m_bCtrlTileFront)
	{
		SetupTileFront();
	}

	if(m_bCtrlTileBack)
	{
		SetupTileBack();
	}

	return 0;
}


void CLcmTileD::SetupHeight()
{
	INT x, z, k;

	INT 	iFlat= m_Hgt.iFlat;
	INT 	iRng= m_Hgt.iRng;
	INT 	bAc = m_Hgt.bAc;
	FLOAT	fH	= m_Hgt.fH;
	FLOAT	fDH = m_Hgt.fDH;

	vector<CLcmTileD::_VecT >	vVtxT;

	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			for(k=0; k<m_ppLcl[z][x].m_nVtx; ++k)
			{
				VEC3	pVc = m_vcPck - m_ppLcl[z][x].m_pVtx[k].p;

				// Range 계산
				INT 	iRT = INT(sqrtf(pVc.x * pVc.x + pVc.z * pVc.z)/m_ppLcl[z][x].m_fWx);

				if( iRT<iRng)
				{
					vVtxT.push_back(CLcmTileD::_VecT(z, x, k, m_ppLcl[z][x].m_pVtx[k].p));
				}
			}

		}
	}

	int iSize = (INT)vVtxT.size();

	for(k=0; k<iSize; ++k)
	{
		FLOAT	xSq;
		VEC3	pVc = m_vcPck - vVtxT[k].p;

		xSq = pVc.x * pVc.x + pVc.z * pVc.z;

		xSq /=(m_pPkLc->m_fWx * m_pPkLc->m_fWx * iRng*iRng*0.25);
		fH = fDH * expf( -xSq);

		INT Z = vVtxT[k].iZ;
		INT X = vVtxT[k].iX;
		INT K = vVtxT[k].iK;

		m_ppLcl[Z][X].m_pVtx[K].p.y += fH;

		if(bAc)
		{
			m_ppLcl[Z][X].SetDiffuse(K);
		}
	}


}



void CLcmTileD::SetupSpoid()
{
	INT 	nIdxZ = INT((-m_pPkLc->m_vcP.z + m_vcPck.z+0.5F*m_pPkLc->m_fWx)/ m_pPkLc->m_fWx);
	INT 	nIdxX = INT((-m_pPkLc->m_vcP.x + m_vcPck.x+0.5F*m_pPkLc->m_fWx)/ m_pPkLc->m_fWx);

	INT		nIdx = nIdxZ * m_pPkLc->m_nCell + nIdxX;

	nIdx *=6;

	m_dFillColor  = m_pPkLc->m_pVtx[  nIdx  ].d;
}



void CLcmTileD::SetupBrush()
{
	// 피킹 포지션: m_vcPck
	INT		z1=0;
	INT		x1=0;
	INT		z2=0;
	INT		x2=0;

	INT 	iRng= m_Hgt.iRng;
	DWORD	dC	= m_dFillColor;

	INT x, z, k;

	vector<CLcmTileD::_VecT >	vVtxT;

	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			for(k=0; k<m_ppLcl[z][x].m_nVtx; ++k)
			{
				VEC3	pVc = m_vcPck - m_ppLcl[z][x].m_pVtx[k].p;

				// Range 계산
				INT 	iRT = 100 * INT(sqrtf(pVc.x * pVc.x + pVc.z * pVc.z)/m_ppLcl[z][x].m_fWx);

				INT		E	= iRT- 100 * iRng;

				if( E<0)
				{
					vVtxT.push_back(CLcmTileD::_VecT(z, x, k, m_ppLcl[z][x].m_pVtx[k].p, E));
				}
			}

		}
	}

	int iSize = (INT)vVtxT.size();

	for(k=0; k<iSize; ++k)
	{
		INT Z = vVtxT[k].iZ;
		INT X = vVtxT[k].iX;
		INT K = vVtxT[k].iK;
		INT E = vVtxT[k].iE;

		if( E<-100)
		{
			D3DXCOLOR Dnew = dC;
			Dnew	*=1.f;
			Dnew.a = 1.f;
			m_ppLcl[Z][X].m_pVtx[K].d = Dnew;
		}
		else
		{
			FLOAT	fW = -float(E)/100.f;

			fW	= exp( - fW * fW * 1.f);
			D3DXCOLOR Dnew = dC;
			D3DXCOLOR Dold = m_ppLcl[Z][X].m_pVtx[K].d;
			Dnew	*= fW;
			Dold	*= (1.f - fW);
			Dnew	+= Dold;
			Dnew.a = 1.f;
			m_ppLcl[Z][X].m_pVtx[K].d = Dnew;
		}

	}


}



void CLcmTileD::SetupTileFront()
{
	// 피킹 포지션: m_vcPck
	INT		z1=0;
	INT		x1=0;
	INT		z2=0;
	INT		x2=0;


	INT 	iRng  = m_Hgt.iRng;

	if(iRng<=1)
	{
		CLcmTileB*			pLcl = NULL;
		CLcmTileB::_Tile*	pTile= NULL;

		// Setup Block Index
		INT		nLclX = INT(m_vcPck.x/m_Inf.iExt);
		INT		nLclZ = INT(m_vcPck.z/m_Inf.iExt);

		pLcl = &m_ppLcl[nLclZ][nLclX];


		// Setup Cell Index
		INT 	nIdxX = INT((m_vcPck.x - pLcl->m_vcP.x)/ pLcl->m_fWx);
		INT 	nIdxZ = INT((m_vcPck.z - pLcl->m_vcP.z)/ pLcl->m_fWx);

		pTile= pLcl->m_pTile;

		int idx = nIdxZ * pLcl->m_nCell + nIdxX;


		pTile[idx].nIdP = m_nTile;
		pLcl->SetUpTexcoord();

		return;
	}



	iRng = INT(iRng/2);


	FLOAT	fWidth = iRng * m_pPkLc->m_fWx;

	VEC3	vcLclMin(m_vcPck.x - fWidth, 0, m_vcPck.z - fWidth);
	VEC3	vcLclMax(m_vcPck.x + fWidth, 0, m_vcPck.z + fWidth);

	INT		nLclBgnX = INT(vcLclMin.x/m_Inf.iExt);
	INT		nLclBgnZ = INT(vcLclMin.z/m_Inf.iExt);

	INT		nLclEndX = INT(vcLclMax.x/m_Inf.iExt);
	INT		nLclEndZ = INT(vcLclMax.z/m_Inf.iExt);


	if(nLclBgnZ<0)
		nLclBgnZ = 0;

	if(nLclBgnX<0)
		nLclBgnX = 0;

	if(nLclBgnZ>=m_Inf.iNfZ)
		nLclBgnZ = m_Inf.iNfZ-1;

	if(nLclBgnX>=m_Inf.iNfX)
		nLclBgnX = m_Inf.iNfX-1;


	if(nLclEndZ<0)
		nLclEndZ = 0;

	if(nLclEndX<0)
		nLclEndX = 0;

	if(nLclEndZ>=m_Inf.iNfZ)
		nLclEndZ = m_Inf.iNfZ-1;

	if(nLclEndX>=m_Inf.iNfX)
		nLclEndX = m_Inf.iNfX-1;


	for(z1=nLclBgnZ; z1<=nLclEndZ; ++z1)
	{
		for(x1=nLclBgnX; x1<=nLclEndX; ++x1)
		{
			CLcmTileB*			pLcl = &m_ppLcl[z1][x1];
			CLcmTileB::_VtxDX*	pVtx = pLcl->m_pVtx;
			CLcmTileB::_Tile*	pTile= pLcl->m_pTile;


			if(NULL == pVtx)
				continue;

			INT 	nIdxZ = INT((-pLcl->m_vcP.z + m_vcPck.z+0.5F*pLcl->m_fWx)/ pLcl->m_fWx);
			INT 	nIdxX = INT((-pLcl->m_vcP.x + m_vcPck.x+0.5F*pLcl->m_fWx)/ pLcl->m_fWx);

			int 	nIdxBgnZ = nIdxZ-iRng;
			int 	nIdxBgnX = nIdxX-iRng;

			int 	nIdxEndZ = INT( (nIdxZ+iRng) );
			int 	nIdxEndX = INT( (nIdxX+iRng) );

			if(nIdxBgnZ<0)
				nIdxBgnZ = 0;

			if(nIdxBgnX<0)
				nIdxBgnX = 0;

			if(nIdxEndZ>=pLcl->m_nCell)
				nIdxEndZ = pLcl->m_nCell;

			if(nIdxEndX>=pLcl->m_nCell)
				nIdxEndX = pLcl->m_nCell;


			for(z2= nIdxBgnZ; z2<nIdxEndZ; ++z2)
			{
				for(x2= nIdxBgnX; x2<nIdxEndX; ++x2)
				{
					int idx = z2 * pLcl->m_nCell + x2;

					int r = (x2-nIdxX) * (x2-nIdxX) + (z2-nIdxZ) * (z2-nIdxZ) - (iRng * iRng);

					if(r>0)
						continue;


					pTile[idx].nIdP = m_nTile;


				}// for(x2= nIdxBgnX; x2<nIdxEndX; ++x2)
			}// for(z2= nIdxBgnZ; z2<nIdxEndZ; ++z2)


			pLcl->SetUpTexcoord();

		}// for(x1=nLclBgnX; x1<=nLclEndX; ++x1)
	}// for(z1=nLclBgnZ; z1<=nLclEndZ; ++z1)
}



void CLcmTileD::SetupTileBack()
{
	// 피킹 포지션: m_vcPck
	INT		z1=0;
	INT		x1=0;
	INT		z2=0;
	INT		x2=0;


	INT 	iRng  = m_Hgt.iRng;

	if(iRng<=1)
	{
		CLcmTileB*			pLcl = NULL;
		CLcmTileB::_Tile*	pTile= NULL;

		// Setup Block Index
		INT		nLclX = INT(m_vcPck.x/m_Inf.iExt);
		INT		nLclZ = INT(m_vcPck.z/m_Inf.iExt);

		pLcl = &m_ppLcl[nLclZ][nLclX];


		// Setup Cell Index
		INT 	nIdxX = INT((m_vcPck.x - pLcl->m_vcP.x)/ pLcl->m_fWx);
		INT 	nIdxZ = INT((m_vcPck.z - pLcl->m_vcP.z)/ pLcl->m_fWx);

		pTile= pLcl->m_pTile;

		int idx = nIdxZ * pLcl->m_nCell + nIdxX;


		pTile[idx].nIdS = m_nTile;
		pLcl->SetUpTexcoord();

		return;
	}



	iRng = INT(iRng/2);


	FLOAT	fWidth = iRng * m_pPkLc->m_fWx;

	VEC3	vcLclMin(m_vcPck.x - fWidth, 0, m_vcPck.z - fWidth);
	VEC3	vcLclMax(m_vcPck.x + fWidth, 0, m_vcPck.z + fWidth);

	INT		nLclBgnX = INT(vcLclMin.x/m_Inf.iExt);
	INT		nLclBgnZ = INT(vcLclMin.z/m_Inf.iExt);

	INT		nLclEndX = INT(vcLclMax.x/m_Inf.iExt);
	INT		nLclEndZ = INT(vcLclMax.z/m_Inf.iExt);


	if(nLclBgnZ<0)
		nLclBgnZ = 0;

	if(nLclBgnX<0)
		nLclBgnX = 0;

	if(nLclBgnZ>=m_Inf.iNfZ)
		nLclBgnZ = m_Inf.iNfZ-1;

	if(nLclBgnX>=m_Inf.iNfX)
		nLclBgnX = m_Inf.iNfX-1;


	if(nLclEndZ<0)
		nLclEndZ = 0;

	if(nLclEndX<0)
		nLclEndX = 0;

	if(nLclEndZ>=m_Inf.iNfZ)
		nLclEndZ = m_Inf.iNfZ-1;

	if(nLclEndX>=m_Inf.iNfX)
		nLclEndX = m_Inf.iNfX-1;


	for(z1=nLclBgnZ; z1<=nLclEndZ; ++z1)
	{
		for(x1=nLclBgnX; x1<=nLclEndX; ++x1)
		{
			CLcmTileB*			pLcl = &m_ppLcl[z1][x1];
			CLcmTileB::_VtxDX*	pVtx = pLcl->m_pVtx;
			CLcmTileB::_Tile*	pTile= pLcl->m_pTile;


			if(NULL == pVtx)
				continue;

			INT 	nIdxZ = INT((-pLcl->m_vcP.z + m_vcPck.z+0.5F*pLcl->m_fWx)/ pLcl->m_fWx);
			INT 	nIdxX = INT((-pLcl->m_vcP.x + m_vcPck.x+0.5F*pLcl->m_fWx)/ pLcl->m_fWx);

			int 	nIdxBgnZ = nIdxZ-iRng;
			int 	nIdxBgnX = nIdxX-iRng;

			int 	nIdxEndZ = INT( (nIdxZ+iRng) );
			int 	nIdxEndX = INT( (nIdxX+iRng) );

			if(nIdxBgnZ<0)
				nIdxBgnZ = 0;

			if(nIdxBgnX<0)
				nIdxBgnX = 0;

			if(nIdxEndZ>=pLcl->m_nCell)
				nIdxEndZ = pLcl->m_nCell;

			if(nIdxEndX>=pLcl->m_nCell)
				nIdxEndX = pLcl->m_nCell;


			for(z2= nIdxBgnZ; z2<nIdxEndZ; ++z2)
			{
				for(x2= nIdxBgnX; x2<nIdxEndX; ++x2)
				{
					int idx = z2 * pLcl->m_nCell + x2;

					int r = (x2-nIdxX) * (x2-nIdxX) + (z2-nIdxZ) * (z2-nIdxZ) - (iRng * iRng);

					if(r>0)
						continue;



					//(E) 2009-08-11 Editor: AFEW
					pTile[idx].nIdS = m_nTile;


				}// for(x2= nIdxBgnX; x2<nIdxEndX; ++x2)
			}// for(z2= nIdxBgnZ; z2<nIdxEndZ; ++z2)


			pLcl->SetUpTexcoord();

		}// for(x1=nLclBgnX; x1<=nLclEndX; ++x1)
	}// for(z1=nLclBgnZ; z1<=nLclEndZ; ++z1)
}



void CLcmTileD::SetupModel()
{
	if(!m_pMdB)
		return;

	m_bClck = FALSE;

	if(m_bCtrlPck)
	{
		WORD	nIds[4]={0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
		WORD	nI1 = 0xFFFF;
		WORD	nI2 = 0xFFFF;
		WORD	nI3 = 0xFFFF;

		VEC3	pos		= m_vcPck;

		CLcmTileD*	pField	= this;
		CLcmTileB*	pLocal	= m_pPkLc;
		ILcmPack*	pPack	= NULL;


		m_pMdB->Query("get ids", nIds);
		nI1 = nIds[0];
		nI2 = nIds[1];
		nI3 = nIds[2];


		pPack	= m_pPkLc->m_pMdLst;

		if(pPack)
			pPack->AddObj(m_pMdB, pos, m_pCam, pField, pLocal, pPack);

//		if(pPack)
//		{
//			CLcmTileB*		pLcl = NULL;
//
//			if(SUCCEEDED(pPack->Query("local", &pLcl)))
//			{
//				INT		i;
//				INT		iSize;
//
//				iSize = pPack->Size();
//
//				for(i=0; i<iSize; ++i)
//				{
//					ILcMdl*	m_pMdB = pPack->GetObj(i);
//					VEC3	vcPos;
//					m_pMdB->GetPosition((FLOAT*)&vcPos);
//
//					FLOAT	fY   = pLcl->GetHeight(vcPos);
//
//					vcPos.y = fY;
//					m_pMdB->SetPosition((FLOAT*)&vcPos);
//				}
//
//			}
//		}
	}// if(GINPUT->ButtonDown(0))


	m_pMdB = NULL;
}








void CLcmTileD::RenderPck()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	DWORD dMnLgt;
	VEC3	vcEps(0,1,0);

	pDev->GetRenderState( D3DRS_LIGHTING, &dMnLgt);

	pDev->SetRenderState( D3DRS_LIGHTING,	FALSE);
	pDev->SetRenderState(D3DRS_FOGENABLE,	FALSE);
	pDev->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID);
	pDev->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE);

	if(!m_vPkLc.empty())
	{
		struct _VtxD
		{
			VEC3	p;
			DWORD	d;

			_VtxD()							: p(0,0,0),d(0XFFFFFFFF){}
			_VtxD(FLOAT X,FLOAT Y,FLOAT Z
				,DWORD D=0xFFFFFFFF)		: p(X,Y,Z),d(D){}

			enum { FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE) };
		};

		_VtxD	pVtx[3];

		pVtx[0].p = m_vPkLc[0].p0 + vcEps;
		pVtx[1].p = m_vPkLc[0].p1 + vcEps;
		pVtx[2].p = m_vPkLc[0].p2 + vcEps;

		pVtx[0].d = 0x4488AAFF;
		pVtx[1].d = 0x4488AAFF;
		pVtx[2].d = 0x4488AAFF;

		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);

		pDev->SetRenderState(D3DRS_FOGENABLE,	FALSE);
		pDev->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID);
		pDev->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE);

		pDev->SetTexture(0, 0);
		pDev->SetFVF(_VtxD::FVF);
		pDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, pVtx, sizeof(_VtxD));

		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}

	pDev->SetRenderState( D3DRS_LIGHTING, dMnLgt);
}



void CLcmTileD::CreateField(CLcmTileD::_Inf* Inf, CLcmTileD::_Tex* pTex)
{
	if(m_ppLcl)
	{
		for(int z=0; z<m_Inf.iNfZ; ++z)
		{
			delete [] m_ppLcl[z];
		}

		delete [] m_ppLcl;
	}

	memset(&m_Inf, 0, sizeof(CLcmTileD::_Inf));
	memcpy(&m_Inf, Inf, sizeof(CLcmTileD::_Inf));


	m_ppLcl 	= new CLcmTileB*[m_Inf.iNfZ];

	INT	iWx = m_Inf.iExt/m_Inf.iNx;

	for(int z=0; z<m_Inf.iNfZ; ++z)
	{
		m_ppLcl[z] = new CLcmTileB[m_Inf.iNfX];

		for(int x=0; x<m_Inf.iNfX; ++x)
		{
			VEC3	pos = VEC3( x * m_Inf.iExt, 0, z * m_Inf.iExt);

			m_ppLcl[z][x].Create(this);
			m_ppLcl[z][x].SetPos(pos);
			m_ppLcl[z][x].CreateFieldMesh(m_Inf.iNx, iWx);
		}
	}

}


INT CLcmTileD::FileLoad(char* sFile)
{
	FILE*	fp;
	INT 	x, z, n;

	fp = fopen(sFile, "rb");

	if(!fp)
		return -1;


	if(m_ppLcl)
	{
		for(int z=0; z<m_Inf.iNfZ; ++z)
		{
			delete [] m_ppLcl[z];
		}

		delete [] m_ppLcl;
	}


	memset(&m_Inf, 0, sizeof(m_Inf));
	fread(&m_Inf, 1, sizeof(CLcmTileD::_Inf), fp);

	fseek(fp, LCXM_FILE_OFFSET, SEEK_SET);


	m_ppLcl 	= new CLcmTileB*[m_Inf.iNfZ];

	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		m_ppLcl[z] = new CLcmTileB[m_Inf.iNfX];

		for(x=0; x<m_Inf.iNfX; ++x)
			m_ppLcl[z][x].Create(this, m_pFVF, m_pEft);
	}

	char	sTexName[MAX_PATH]={0};

	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			CLcmTileB* pLc = &m_ppLcl[z][x];

			fread(&pLc->m_mtW,	1, sizeof(pLc->m_mtW),	fp);					// World Matrix
			fread(&pLc->m_vcP,	1, sizeof(pLc->m_vcP),	fp);					// Position

			// Tile
			fread(&pLc->m_vcP, 1, sizeof(pLc->m_nCell), fp);					// Tile Number

			// Mesh
			fread(&pLc->m_nCell,1, sizeof(pLc->m_nCell),fp); 					// Number of tile for Width
			fread(&pLc->m_fWx,	1, sizeof(pLc->m_fWx ), fp); 					// Width of tile for x;
			fread(&pLc->m_iVs,	1, sizeof(pLc->m_iVs ), fp); 					// Vertex Size
			fread(&pLc->m_FVF,	1, sizeof(pLc->m_FVF ), fp);
			fread(&pLc->m_nVtx,	1, sizeof(pLc->m_nVtx), fp); 					// Vertex Number

			// Tile Texture Name
			//fread(pLc->m_pTex.sName, 1, CLcmTileB::LCXM_TEX_NAME, fp);
			char	sTex[MAX_PATH]={0};
			fread(sTex, 1, LCXM_TEX_NAME, fp);
			strcpy(sTexName, sTex);
		}
	}


	strcpy(m_pTex.sName, sTexName);

	// Tile...
	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			CLcmTileB* pLc = &m_ppLcl[z][x];
			INT		nCell = pLc->m_nCell * pLc->m_nCell;

			pLc->m_pTile= new CLcmTileB::_Tile[nCell];
			pLc->m_pVtx	= new CLcmTileB::_VtxDX[pLc->m_nVtx];

			fread(pLc->m_pTile, nCell		, sizeof(CLcmTileB::_Tile), fp);		// Tile

			for(n=0; n<pLc->m_nVtx; ++n)
			{
				CLcmTileB::_VtxDX	Src;
				CLcmTileB::_VtxDX*	Dst = pLc->m_pVtx+n;

				fread(&Src, 1, sizeof(CLcmTileB::_VtxDX), fp);		// Mesh

				Dst->p  = Src.p;
				Dst->d  = Src.d;
				Dst->t0 = Src.t0;
				Dst->t1 = Src.t1;
//				Dst->t0.y = 1.0 - Dst->t0.y;
//				Dst->t1.y = 1.0 - Dst->t0.y;
			}
		}
	}

	// Object List
	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			CLcmTileB* pLc = &m_ppLcl[z][x];

			WORD	nIds[4]={0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
			VEC3	vcR;
			VEC3	vcS;
			VEC3	vcT;
			char	sMdN[128]={0};
			INT		iSize=0;

			// Object List
			fread(&iSize, 1, sizeof(INT), fp);

			for(n=0; n<iSize; ++n)
			{
				fread(&nIds[0],	 1, sizeof(WORD), fp);
				fread(&nIds[1],	 1, sizeof(WORD), fp);
				fread(&nIds[2],	 1, sizeof(WORD), fp);
				fread(&nIds[3],	 1, sizeof(WORD), fp);

				fread(&vcR,		 1, sizeof(VEC3), fp);
				fread(&vcS,		 1, sizeof(VEC3), fp);
				fread(&vcT,		 1, sizeof(VEC3), fp);

				fread(sMdN,		 1, sizeof(sMdN), fp);

				INT		nM=-1, nS=-1;
				void*	pSrc;

				if(SUCCEEDED(m_pTbMdl->Select(&pSrc, &nM, &nS, sMdN)))
				{
					ILcMdl*	pMdB = NULL;

					if(FAILED(LcMdl_Create(NULL, &pMdB, NULL, pSrc)))
						return -1;

					pMdB->SetRotation((FLOAT*)&vcR);
					pMdB->SetScaling(vcS);
					pMdB->SetPosition((FLOAT*)&vcT);

					pMdB->Query("Set Camera", m_pCam);
					pMdB->Query("Set ids", nIds);
					pMdB->Query("Set Package", pLc->m_pMdLst);

					pLc->m_pMdLst->Push_Back(pMdB);
				}
			}
		}
	}


	fclose(fp);


	return 0;
}


void* CLcmTileD::GetFldInf()
{
	return &m_Inf;
}

char* CLcmTileD::GetFldName()
{
	return m_Inf.sN;
}


void* CLcmTileD::GetVertexDeclarator()
{
	return m_pFVF;
}

void* CLcmTileD::GetD3DXEffect()
{
	return m_pEft;
}

void* CLcmTileD::GetTexture()
{
	return m_pTex.pTex;
}


void* CLcmTileD::GetCam()
{
	return m_pCam;
}



void CLcmTileD::AddObj(ILcMdl* pObj)
{
	m_vObj.push_back(pObj);
}

INT CLcmTileD::GetHeight(float* pOut, const float* pIn)
{
	VEC3 vcP	= *((VEC3*)pIn);

	// 블록의 인덱스를 구해서 해당 블록의 주소를 구한다.
	INT	nX = INT(vcP.x/m_Inf.iExt);
	INT	nZ = INT(vcP.z/m_Inf.iExt);

	CLcmTileB*	pLcl	= NULL;

	if(	nX>=0 && nX<m_Inf.iNfX &&
		nZ>=0 && nZ<m_Inf.iNfZ)
		pLcl = &m_ppLcl[nZ][nX];
	else
		return -1;

	// 해당 블록으로부터 위치를 구한다.
	return pLcl->GetBlcHeight((D3DXVECTOR3*)pOut, (D3DXVECTOR3*)pIn);
}

INT CLcmTileD::LoadTexture(char* sFile)
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	
	HRESULT hr=0;
	PDTX	pTex=NULL;
	DIMG	Image;

	char	sPath[MAX_PATH]={0};

	sprintf(sPath, "%s%s", sFile, m_pTex.sName);

	hr = D3DXCreateTextureFromFileEx(pDev
			, sPath
			, D3DX_DEFAULT
			, D3DX_DEFAULT
			, D3DX_DEFAULT
			, 0
			, D3DFMT_UNKNOWN
			, D3DPOOL_MANAGED
			, D3DX_DEFAULT
			, D3DX_DEFAULT
			, 0X00FFFFFF
			, &Image
			, NULL
			, &pTex);


	if(FAILED(hr))
		return -1;

	m_pTex.pTex	 = pTex;
	memcpy(&m_pTex.Image, &Image, sizeof(DIMG));
//	strcpy(m_pTex.sName, sFile);

	return 0;
}

void CLcmTileD::SetupTexture(CLcmTileD::_Tex* pTex)
{
	m_pTex = pTex;
}