// Implementation of the CLcmSptD class.
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

#include "LnmSptB.h"
#include "LnmSptD.h"



CLcmSptD::_Inf::_Inf()
{
	nI = -1;
	memset(sN, 0, sizeof(sN));
}


CLcmSptD::_Fog::_Fog()
{
	m_dFt	= D3DFOG_LINEAR;
	m_dFc	= 0;														// Fog Color
	m_fFb	= 0;														// Fog Begin
	m_fFe	= 0;														// Fog End
	m_fFd	= 1.f;
}

inline DWORD	__F2DW( FLOAT f )	{ return *((DWORD*)&f); }

void CLcmSptD::_Fog::SetFog(void* _pDev)
{
	LPDIRECT3DDEVICE9	pDev = (LPDIRECT3DDEVICE9)_pDev;

//	pDev->SetRenderState(D3DRS_FOGENABLE, TRUE);
	pDev->SetRenderState(D3DRS_FOGTABLEMODE, m_dFt);
	pDev->SetRenderState(D3DRS_FOGCOLOR,	 m_dFc);					// Set Fog
	pDev->SetRenderState(D3DRS_FOGSTART,	__F2DW(m_fFb));
	pDev->SetRenderState(D3DRS_FOGEND,		__F2DW(m_fFe));
	pDev->SetRenderState(D3DRS_FOGDENSITY,	__F2DW(m_fFd));
}


CLcmSptD::_Env::_Env()
{
	m_dAb	= 0;														// Ambient color
	memset(&m_Mtl, 0, sizeof(m_Mtl));									// Material
	m_iLg	= 0;														// Lighting Num

	memset(m_pLg, 0, sizeof(m_pLg));
}


CLcmSptD::_VecT::_VecT()
: iZ(-1)
, iX(-1)
, iK(-1)
, p(0,0,0)
, iE(-1)
{
}


CLcmSptD::_VecT::_VecT(INT Z, INT X, INT K, const D3DXVECTOR3& P, INT E)
: iZ(Z)
, iX(X)
, iK(K)
, p(P)
, iE(E)
{
}


CLcmSptD::_Pck::_Pck()
: vcPk(0,0,0)
, p0(0,0,0)
, p1(0,0,0)
, p2(0,0,0)
, fStlSrtR(FLT_MAX){}

CLcmSptD::_Pck::_Pck(D3DXVECTOR3& vcP, D3DXVECTOR3& P0, D3DXVECTOR3& P1, D3DXVECTOR3& P2, FLOAT& _fD)
: vcPk(vcP)
, p0(P0)
, p1(P1)
, p2(P2)
, fStlSrtR(_fD)
{
}


CLcmSptD::CLcmSptD()
{
	m_ppLcl 	= NULL;
	m_pLcT		= NULL;

	m_pPkLc 	= NULL;				// Pick Local
	m_bClck 	= FALSE;
	m_pPkObj	= NULL;				// Picking Object

	m_vcPck		= D3DXVECTOR3(0,0,0);

	m_fCull		= 6000;

	m_bCtrlCull	= TRUE;
	m_bCtrlPck	= FALSE;

	m_bCtrlHeight	= FALSE;
	m_bCtrlSpoid	= FALSE;
	m_dFillColor	= 0x0;

	m_bCtrlBrush	= FALSE;
	m_bCtrlModel	= FALSE;
	m_bCtrlTile		= FALSE;
	m_nCtrlTile		= -1;
	m_bCtrlBndBox	= FALSE;
	m_bCtrlLclLine	= FALSE;

	m_pMdB			= NULL;
	m_pCam			= NULL;

	m_pTbTex		= NULL;
	m_pTbMdl		= NULL;

	m_pEft			= NULL;
	m_pFVF			= NULL;

	m_fRangeMax		= 5.f;
}


CLcmSptD::~CLcmSptD()
{
	Destroy();
}


void CLcmSptD::Clear()
{
	SAFE_DELETE(	m_pLcT		);

	if(m_ppLcl)
	{
		for(int z=0; z<m_Inf.iNfZ; ++z)
			SAFE_DELETE_ARRAY(	m_ppLcl[z]	);

		SAFE_DELETE_ARRAY(	m_ppLcl );
	}

	m_pLcT	= NULL;
	m_pPkLc	= NULL;
	m_pPkObj = NULL;
	m_vObj2D.clear();
	m_vObj3D.clear();
	m_vObj.clear();
}


INT CLcmSptD::Query(char* sCmd, void* pData)
{
	if(0==_stricmp("Get Pick Object", sCmd))
	{
		ILcMdl* pObj = NULL;

		pObj= (ILcMdl*)this->GetPckObj();

		*((ILcMdl**)pData) = pObj;
		return 0;
	}

	else if(0==_stricmp("Get Pick Position",	sCmd))
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

		INT 	x, z, i=0;

		for(z=0; z<m_Inf.iNfZ; ++z)
		{
			for(x=0; x<m_Inf.iNfX; ++x)
			{
				CLcmSptB* pLc	= &m_ppLcl[z][x];
				int		  iSize	= pLc->m_pMdb2D->Size();

				for(i=0; i<iSize; ++i)
				{
					ILcMdl*	pMdB = pLc->m_pMdb2D->GetObj(i);
					pMdB->Query("Set Camera", m_pCam);
				}


				iSize	= pLc->m_pMdb3D->Size();
				for(i=0; i<iSize; ++i)
				{
					ILcMdl*	pMdB = pLc->m_pMdb3D->GetObj(i);
					pMdB->Query("Set Camera", m_pCam);
				}
			}
		}

		return 0;
	}

	else if(0==_stricmp("Get Camera", sCmd))
	{
		*((ILcCam**)pData) = m_pCam;

		return 0;
	}

	else if(0==_stricmp("Set Ray Direction", 	sCmd))
	{
		m_vcRayDir = *((D3DXVECTOR3*)pData);
		return 0;
	}

	else if(0==_stricmp("Set Camera Position", 	sCmd))
	{
		m_vcCamPos = *((D3DXVECTOR3*)pData);
		return 0;
	}

	else if(0==_stricmp("Erase Pick Object", 	sCmd))
	{
		this->EraseObj();
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

	else if(0==_stricmp("Tiling Enable", 		sCmd))
	{
		m_bCtrlTile = *((BOOL*)pData);
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

	else if(0==_stricmp("Set Tile Index", 		sCmd))
	{
		m_nCtrlTile = *((INT*)pData);
		return 0;
	}

	else if(0==_stricmp("Set Field Info",	 	sCmd))
	{
		memcpy(&m_Hgt, pData, sizeof(CLcmSptD::MpHgt));
		return 0;
	}

	else if(0==_stricmp("Clear Layer List", 	sCmd))
	{
		return 0;
	}

	else if(0==_stricmp("Set Layer List", 		sCmd))
	{
		struct _T	{	INT n;	char* s;	};
		_T*	t = (_T*)pData;

		return 0;
	}

	else if(0==_stricmp("Get Fill Color", 		sCmd))
	{
		*((DWORD*)pData) = m_dFillColor;
		return 0;
	}

	else if(0==_stricmp("Get Model Pointer", 	sCmd))
	{
		*((ILcMdl**)pData) = m_pMdB;
		return 0;
	}

	else if(0==_stricmp("File Load", 			sCmd))
	{
		char*	sFile = (char*)pData;
		INT hr = 0;

		this->Clear();
		hr = this->FileLoad(sFile);
		return hr;
	}

	else if(0==_stricmp("File Save", 			sCmd))
	{
		char*	sFile = (char*)pData;
		INT hr = this->FileSave(sFile);
		return hr;
	}

	else if(0==_stricmp("Set Info", 			sCmd))
	{
		memcpy(&m_Inf, pData, sizeof(CLcmSptD::_Inf));
		return 0;
	}

	else if(0==_stricmp("Get Info", 			sCmd))
	{
		memcpy(pData, &m_Inf, sizeof(CLcmSptD::_Inf));
		return 0;
	}

	else if(0==_stricmp("Set Environment", 		sCmd))
	{
		memcpy(&m_Env, pData, sizeof(CLcmSptD::_Env));
		return 0;
	}

	else if(0==_stricmp("Get Environment", 		sCmd))
	{
		memcpy(pData, &m_Env, sizeof(CLcmSptD::_Env));
		return 0;
	}


	else if(0==_stricmp("Create Field", 		sCmd))
	{
		struct _T{	CLcmSptD::_Inf* I; CLcmSptD::_Env* E; CLcmSptB::lsMpTex* L;};
		_T* t = (_T*)pData;

		CreateField(t->I, t->E, t->L);
		return 0;
	}

	else if(0==_stricmp("Set Fog", 				sCmd))
	{
		memcpy(&m_Env.m_Fog, pData, sizeof(CLcmSptD::_Fog));
		return 0;
	}

	else if(0==_stricmp("Get Fog", 				sCmd))
	{
		memcpy(pData, &m_Env.m_Fog, sizeof(CLcmSptD::_Fog));
		return 0;
	}


	else if(0==_stricmp("Set Light", 			sCmd))
	{
		struct _T
		{
			INT		n;							// Number of Lighting
			void*	p;							// Lighting Data
		};

		_T*	t =(_T*)pData;
		m_Env.m_iLg = t->n;
		memcpy(m_Env.m_pLg, t->p, sizeof(CLcmSptD::_Lgt) * t->n);
		return 0;
	}

	else if(0==_stricmp("Set Material", 		sCmd))
	{
		memcpy( &m_Env.m_Mtl, pData, sizeof(CLcmSptD::_Mtl) );
		return 0;
	}

	else if(0==_stricmp("Set Ambient Light", 	sCmd))
	{
		DWORD dAmLgt =*((DWORD*)pData);
		m_Env.m_dAb	= dAmLgt;
		return 0;
	}

	else if(0==_stricmp("Get Field Name", 		sCmd))
	{
		strcpy((char*)pData, m_Inf.sN);
		return 0;
	}

	else if(0==_stricmp("Set HLSL", 		sCmd))
	{
		m_pEft = (LPD3DXEFFECT)pData;
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

	return -1;
}


INT CLcmSptD::Create(void* p1, void* p2, void* p3, void* p4)
{
	INT	hr	= 0;
	char* sFile	= NULL;

	sFile		= (char*  )p1;
	m_pTbTex	= (ILcTbl*)p2;
	m_pTbMdl	= (ILcTbl*)p3;


	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	DWORD				dFVF = CLcmSptB::VtxSpl::FVF;
	D3DVERTEXELEMENT9	pDcl[MAX_FVF_DECL_SIZE]={0};

	D3DXDeclaratorFromFVF(dFVF, pDcl);

	if(FAILED(pDev->CreateVertexDeclaration( pDcl, &m_pFVF)))
		return -1;

	if(sFile)
		hr = FileLoad(sFile);

	return hr;
}


void CLcmSptD::Destroy()
{
	SAFE_DELETE(	m_pLcT		);

	if(m_ppLcl)
	{
		for(int z=0; z<m_Inf.iNfZ; ++z)
			SAFE_DELETE_ARRAY(	m_ppLcl[z]	);

		SAFE_DELETE_ARRAY(	m_ppLcl );
	}

	SAFE_RELEASE(	m_pFVF	);

	m_pLcT	= NULL;
	m_pPkLc	= NULL;
	m_pPkObj = NULL;
	m_vObj2D.clear();
	m_vObj3D.clear();
	m_vObj.clear();
}


INT CLcmSptD::FrameMove()
{
	INT		z, x;
	VEC3	vcEye;

	m_vObj2D.clear();
	m_vObj3D.clear();

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
				fRangeView	*= fRangeView;

				VEC3	vcCenter = m_ppLcl[z][x].m_vcP + VEC3(1,0,1) * fBlockWidth/2.F;
				VEC3	vcT = vcCenter- vcEye;
				FLOAT fLenSq = D3DXVec3LengthSq(&vcT);

				if(fLenSq>= fRangeView)
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

	SAFE_FRMOV( m_pLcT	);

	SAFE_UPDATE(this);

	m_bCtrlPck = FALSE;

	return 0;
}


INT CLcmSptD::Update()
{
	INT i;
	INT iSize;

	m_vObj.clear();

	iSize = (INT)m_vObj2D.size();

	for(i=0; i<iSize; ++i)
	{
		ILcMdl*	pObj = m_vObj2D[i];

		if(this->m_bCtrlCull)
		{
			TbndAB*		pBnd = NULL;
			ILcCam*	pCam = NULL;

			pObj->Query("Get Camera", &pCam);
			pObj->Query("Get Bound", &pBnd);


			VEC3	vcEye = *pCam->GetEye();
			VEC3	vcBnd = *pBnd->GetCenter();
			VEC3	vcT = vcBnd - vcEye;
			FLOAT	fL = D3DXVec3Length(&vcT);

			FLOAT	fR = pBnd->GetRadius();

			if(fL>(m_fCull+fR))
				continue;

			const DPLN* pFrsm  = pCam->GetFrustumPlanes();

			INT j =0;
			for(j=0; j<6; ++j)
			{
				fL = pFrsm[j].a * vcBnd.x + pFrsm[j].b * vcBnd.y + pFrsm[j].c * vcBnd.z + pFrsm[j].d - fR;

				if(fL>0)
					break;
			}

			if(j<6)
				continue;
		}

		m_vObj.push_back(pObj);
	}


	iSize = (INT)m_vObj3D.size();

	for(i=0; i<iSize; ++i)
	{
		ILcMdl*	pObj = m_vObj3D[i];

		if(this->m_bCtrlCull)
		{
			TbndAB*		pBnd = NULL;
			ILcCam*	pCam = NULL;

			pObj->Query("Get Camera", &pCam);
			pObj->Query("Get Bound", &pBnd);


			VEC3	vcEye = *pCam->GetEye();
			VEC3	vcBnd = *pBnd->GetCenter();
			VEC3	vcT = vcBnd - vcEye;

			FLOAT	fL = D3DXVec3Length(&vcT);
			FLOAT	fR = pBnd->GetRadius();

			if(fL>(m_fCull+fR))
				continue;

			const DPLN* pFrsm = pCam->GetFrustumPlanes();

			INT j=0;

			for(j=0; j<6; ++j)
			{
				fL = pFrsm[j].a * vcBnd.x + pFrsm[j].b * vcBnd.y + pFrsm[j].c * vcBnd.z + pFrsm[j].d - fR;

				if(fL>0)
					break;
			}

			if(j<6)
				continue;
		}

		m_vObj.push_back(pObj);
	}


	std::sort(m_vObj.begin(), m_vObj.end(), TsrtGpf<ILcMdl* >());


	// Picking Object
	PickObj();

	return 0;
}



void CLcmSptD::AddObj2D(ILcMdl* pMdB)
{
	m_vObj2D.push_back(pMdB);
}


void CLcmSptD::AddObj3D(ILcMdl* pMdB)
{
	m_vObj3D.push_back(pMdB);
}






INT CLcmSptD::PickField()
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


INT CLcmSptD::PickPosition(	CLcmSptB**			pBlock
						,	CLcmSptD::lsTlPkT*	pPickInf	// Picking Information
						,	VEC3*				pvcOut		// Picking position
						,	const VEC3*			pvcRayPos	// Ray Begin Position
						,	const VEC3*			pvcRayDir	// Ray Direction
							)
{
	INT		z;
	INT		x;
	INT		i;

	D3DXVECTOR3		vcPick	;
	CLcmSptD::lsTlPkT	vPkLc	;			// Picking Triangle from Local

	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			CLcmSptB* pLcl = &m_ppLcl[z][x];

			for(i=0; i<pLcl->m_iNi; ++i)
			{
				WORD	a, b, c;

				a = pLcl->m_pFce[i].a;
				b = pLcl->m_pFce[i].b;
				c = pLcl->m_pFce[i].c;

				VEC3	vcPk;
				VEC3	p0 = pLcl->m_pVtx[a].p;
				VEC3	p1 = pLcl->m_pVtx[b].p;
				VEC3	p2 = pLcl->m_pVtx[c].p;
				FLOAT	fD;

				if(SUCCEEDED(LcMath_LineToTriangle(&vcPk, &fD, pvcRayPos, pvcRayDir, &p0, &p1, &p2)))
					vPkLc.push_back(CLcmSptD::_Pck(vcPk, p0, p1, p2, fD));
			}
		}
	}

	if(vPkLc.empty())
		return -1;


	std::sort(vPkLc.begin(), vPkLc.end(), TsrtL<CLcmSptD::_Pck >());
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


INT CLcmSptD::PickLocal()
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

	if(m_bCtrlTile)
	{
		SetupTile();
	}

	return 0;
}


void CLcmSptD::SetupHeight()
{
	INT x, z, k;

	INT 	iFlat= m_Hgt.iFlat;
	INT 	iRng= m_Hgt.iRng;
	INT 	bAc = m_Hgt.bAc;
	FLOAT	fH	= m_Hgt.fH;
	FLOAT	fDH = m_Hgt.fDH;

	vector<CLcmSptD::_VecT >	vVtxT;

	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			for(k=0; k<m_ppLcl[z][x].m_iNv; ++k)
			{
				VEC3	pVc = m_vcPck - m_ppLcl[z][x].m_pVtx[k].p;
				INT 	iRT = INT(sqrtf(pVc.x * pVc.x + pVc.z * pVc.z)/m_ppLcl[z][x].m_fWx);

				if( iRT<iRng)
				{
					vVtxT.push_back(CLcmSptD::_VecT(z, x, k, m_ppLcl[z][x].m_pVtx[k].p));
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
			int idx = Z * m_ppLcl[Z][X].m_iNx + X;
			m_ppLcl[Z][X].SetDiffuse(K);
		}
	}


	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			INT Z = vVtxT[k].iZ;
			INT X = vVtxT[k].iX;
			INT K = vVtxT[k].iK;

			m_ppLcl[z][x].SetNormal();

			if(m_ppLcl[z][x].m_pMdb2D)
			{
				CLcmSptB*	pLcl = NULL;
				ILcmPack*	pPack = m_ppLcl[z][x].m_pMdb2D;

				if(SUCCEEDED(pPack->Query("local", &pLcl)))
				{
					INT		i;
					INT		iSize;

					iSize = pPack->Size();

					for(i=0; i<iSize; ++i)
					{
						ILcMdl*	pMdB = pPack->GetObj(i);
						VEC3	vcPos;

						pMdB->GetPosition((FLOAT*)&vcPos);
						FLOAT	fY   = pLcl->GetHeight(vcPos);

						vcPos.y = fY;
						pMdB->SetPosition((FLOAT*)&vcPos);
					}

				}
			}

			if(m_ppLcl[z][x].m_pMdb3D)
			{
				CLcmSptB*	pLcl = NULL;
				ILcmPack*	pPack = m_ppLcl[z][x].m_pMdb3D;

				if(SUCCEEDED(pPack->Query("local", &pLcl)))
				{
					INT		i;
					INT		iSize;

					iSize = pPack->Size();

					for(i=0; i<iSize; ++i)
					{
						ILcMdl* pMdB = pPack->GetObj(i);
						VEC3	 vcPos;

						pMdB->GetPosition((FLOAT*)&vcPos);
						FLOAT	 fY   = pLcl->GetHeight(vcPos);

						vcPos.y = fY;
						pMdB->SetPosition((FLOAT*)&vcPos);
					}

				}
			}
		}
	}
}



void CLcmSptD::SetupSpoid()
{
	INT 	nIdxZ = INT((-m_pPkLc->m_vcP.z + m_vcPck.z+0.5F*m_pPkLc->m_fWx)/ m_pPkLc->m_fWx);
	INT 	nIdxX = INT((-m_pPkLc->m_vcP.x + m_vcPck.x+0.5F*m_pPkLc->m_fWx)/ m_pPkLc->m_fWx);

	m_dFillColor  = m_pPkLc->m_pVtx[nIdxZ * m_pPkLc->m_iNx + nIdxX].d;
}



void CLcmSptD::SetupBrush()
{
	// 피킹 포지션: m_vcPck
	INT		z1=0;
	INT		x1=0;
	INT		z2=0;
	INT		x2=0;

	INT 	iRng= m_Hgt.iRng;
	DWORD	dC	= m_dFillColor;

	INT x, z, k;

	vector<CLcmSptD::_VecT >	vVtxT;

	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			for(k=0; k<m_ppLcl[z][x].m_iNv; ++k)
			{
				VEC3	pVc = m_vcPck - m_ppLcl[z][x].m_pVtx[k].p;
				INT 	iRT = 100 * INT(sqrtf(pVc.x * pVc.x + pVc.z * pVc.z)/m_ppLcl[z][x].m_fWx);

				INT		E	= iRT- 100 * iRng;

				if( E<0)
				{
					vVtxT.push_back(CLcmSptD::_VecT(z, x, k, m_ppLcl[z][x].m_pVtx[k].p, E));
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





void CLcmSptD::SetupModel()
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

		CLcmSptD*	pField	= this;
		CLcmSptB*	pLocal	= m_pPkLc;
		ILcmPack*	pPack	= NULL;


		m_pMdB->Query("get ids", nIds);
		nI1 = nIds[0];
		nI2 = nIds[1];
		nI3 = nIds[2];


		if(0 == nI1)															// 풀
		{
			pPack	= m_pPkLc->m_pMdb2D;

			if(pPack)
				pPack->AddObj(m_pMdB, pos, m_pCam, pField, pLocal, pPack);
		}

		else if(1 == nI1)														// 3D Tree
		{
			pPack	= m_pPkLc->m_pMdb3D;

			if(pPack)
				pPack->AddObj(m_pMdB, pos, m_pCam, pField, pLocal, pPack);
		}


//		if(pPack)
//		{
//			INT		i;
//			INT		iSize;
//
//			iSize = pPack->Size();
//
//			for(i=0; i<iSize; ++i)
//			{
//				ILcMdl* m_pMdB = pPack->GetObj(i);
//				VEC3	 vcPos;
//
//				m_pMdB->GetPosition((FLOAT*)&vcPos);
//				FLOAT	 fY   = pLocal->GetHeight(vcPos);
//
//				vcPos.y = fY;
//				m_pMdB->SetPosition((FLOAT*)&vcPos);
//			}
//		}

	}// if(GINPUT->ButtonDown(0))


	m_pMdB = NULL;
}



void CLcmSptD::SetupTile()
{
	// 피킹 포지션: m_vcPck
	INT		z1=0;
	INT		x1=0;
	INT		z2=0;
	INT		x2=0;

	INT 	nTile = m_nCtrlTile;
	INT 	iRng  = m_Hgt.iRng;

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
			CLcmSptB*			pLcl = &m_ppLcl[z1][x1];
			CLcmSptB::VtxSpl*	pVtx = pLcl->m_pVtx;
			INT*				pVtt = pLcl->m_pVtt;

			if(NULL == pVtx)
				continue;

			INT 	nIdxZ = INT((-pLcl->m_vcP.z + m_vcPck.z+0.5F*pLcl->m_fWx)/ pLcl->m_fWx);
			INT 	nIdxX = INT((-pLcl->m_vcP.x + m_vcPck.x+0.5F*pLcl->m_fWx)/ pLcl->m_fWx);

			int 	nIdxBgnZ = nIdxZ-iRng;
			int 	nIdxBgnX = nIdxX-iRng;

			int 	nIdxEndZ = nIdxZ+iRng;
			int 	nIdxEndX = nIdxX+iRng;

			if(nIdxBgnZ<0)
				nIdxBgnZ = 0;

			if(nIdxBgnX<0)
				nIdxBgnX = 0;

			if(nIdxEndZ>=pLcl->m_iNx)
				nIdxEndZ = pLcl->m_iNx;

			if(nIdxEndX>=pLcl->m_iNx)
				nIdxEndX = pLcl->m_iNx;


			for(z2= nIdxBgnZ; z2<nIdxEndZ; ++z2)
			{
				for(x2= nIdxBgnX; x2<nIdxEndX; ++x2)
				{
					int idx = z2 * pLcl->m_iNx + x2;

					int r = (x2-nIdxX) * (x2-nIdxX) + (z2-nIdxZ) * (z2-nIdxZ) - (iRng * iRng);

					if(r>0)
						continue;

					pVtt[idx] = nTile;

//					pVtx[idx].ts[nTile] = 1.f;
				}// for(x2= nIdxBgnX; x2<nIdxEndX; ++x2)
			}// for(z2= nIdxBgnZ; z2<nIdxEndZ; ++z2)


			pLcl->CalculateMap();

		}// for(x1=nLclBgnX; x1<=nLclEndX; ++x1)
	}// for(z1=nLclBgnZ; z1<=nLclEndZ; ++z1)
}


INT CLcmSptD::PickObj()
{
	if(m_bCtrlModel)
		return -1;

	if(!m_bCtrlBndBox)
		return -1;

	int iObjSize = (INT)m_vObj.size();

	if(iObjSize<1)
		return -1;


	INT	i;


	if(m_bCtrlPck)
	{
		VEC3	vcPk;
		FLOAT	fDist;
		TbndAB*	pBndBox=NULL;


		if(m_pPkObj)
		{
			TbndAB*	pBnd = NULL;
			m_pPkObj->Query("get bound", &pBnd);

			pBnd->SetColor(0xFF666666);
		}

		// Picking Object
		m_pPkObj= NULL;
		m_vPkObj.clear();

		int iObjSize = (INT)m_vObj.size();

		for(i=0; i<iObjSize; ++i)
		{
			ILcMdl*	pMdl = m_vObj[i];
			TbndAB* 	pBnd = NULL;

			if(SUCCEEDED(pMdl->Query("get bound", &pBnd)))
				pBndBox = pBnd;

			if(SUCCEEDED(LcxUtil_PickAABB(&fDist, &vcPk, &m_vcCamPos, &m_vcRayDir, pBndBox)))
			{
				TbndAABB Box(vcPk, pBndBox, fDist);
				m_vPkObj.push_back(Box);
			}
		}// for Object


		if(!m_vPkObj.empty())
		{
			m_bClck = TRUE;

			std::sort(m_vPkObj.begin(), m_vPkObj.end(), TsrtL<TbndAABB >());
			m_vcPkObj = m_vPkObj[0].vPck;

			m_pPkObj = (ILcMdl*)m_vPkObj[0].pBnd->GetOwner();					// Picking Object
		}

		if(m_pPkObj)
		{
			ILcMdl*	pMdl = m_pPkObj;
			TbndAB*	pBnd = NULL;

			pMdl->Query("get bound", &pBnd);
			pBnd->SetColor(0xFFFF0000);

			return 0;
		}
	}

	return -1;
}


void CLcmSptD::Render()
{
	INT iSize;
	INT i;
	INT x, z;

	if(!m_ppLcl && !m_pLcT)
		return;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	LcD3D_SetWorldIdentity(pDev);

	pDev->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE);


	for(i=0; i<7; ++i)
		pDev->LightEnable( i, FALSE);

	for(i=0; i<m_Env.m_iLg; ++i)
	{
		const D3DLIGHT9*	pLight = m_Env.m_pLg[i];
		pDev->SetLight( i, pLight);
		pDev->LightEnable( i, TRUE );
	}

	pDev->SetRenderState( D3DRS_AMBIENT, m_Env.m_dAb);					// Set Ambient
	pDev->SetMaterial( &m_Env.m_Mtl.m );									// Set Material



	m_Env.m_Fog.SetFog(pDev);


	if(m_bCtrlLclLine && m_ppLcl)
	{
		for(z=0; z<m_Inf.iNfZ; ++z)
		{
			for(x=0; x<m_Inf.iNfX; ++x)
			{
				m_ppLcl[z][x].RenderTline();								// Render Block Line
			}
		}
	}




	if(m_ppLcl)
	{
		// 3. Data 갱신
		for(int z=0; z<m_Inf.iNfZ; ++z)
		{
			for(int x=0; x<m_Inf.iNfX; ++x)
			{
				m_ppLcl[z][x].RenderBlc();
			}
		}
	}


	if(m_pLcT)
		m_pLcT->RenderBlc();



	iSize = (INT)m_vObj.size();
	for(i=0; i<iSize; ++i)
	{
		ILcMdl*	pMdl = m_vObj[i];

		if(m_bCtrlBndBox)
			pMdl->Query("bound box render", NULL);

		pMdl->Render();
	}
}



void CLcmSptD::RenderPck()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	DWORD dMnLgt;

	pDev->GetRenderState( D3DRS_LIGHTING, &dMnLgt);

	pDev->SetRenderState( D3DRS_LIGHTING,	FALSE);
	pDev->SetRenderState(D3DRS_FOGENABLE,	FALSE);
	pDev->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID);
	pDev->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE);

	if(!m_vPkLc.empty())
	{
		struct _VtxD
		{
			D3DXVECTOR3	p;
			D3DCOLOR	d;

			_VtxD()							: p(0,0,0),d(0XFFFFFFFF){}
			_VtxD(FLOAT X,FLOAT Y,FLOAT Z
				,D3DCOLOR D=0xFFFFFFFF)		: p(X,Y,Z),d(D){}

			enum { FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE) };
		};

		_VtxD	pVtx[3];

		pVtx[0].p = m_vPkLc[0].p0;
		pVtx[1].p = m_vPkLc[0].p1;
		pVtx[2].p = m_vPkLc[0].p2;

		pVtx[0].d = 0x9988AAFF;
		pVtx[1].d = 0x9988AAFF;
		pVtx[2].d = 0x9988AAFF;

		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
		pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);

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



void CLcmSptD::CreateField(CLcmSptD::_Inf* Inf, CLcmSptD::_Env* Env, CLcmSptB::lsMpTex* pTxLst)
{
	if(m_ppLcl)
	{
		for(int z=0; z<m_Inf.iNfZ; ++z)
			SAFE_DELETE_ARRAY(	m_ppLcl[z]	);

		SAFE_DELETE_ARRAY(	m_ppLcl );
	}

	memset(&m_Inf, 0, sizeof(CLcmSptD::_Inf));
	memset(&m_Env, 0, sizeof(CLcmSptD::_Env));

	memcpy(&m_Inf, Inf, sizeof(CLcmSptD::_Inf));
	memcpy(&m_Env, Env, sizeof(CLcmSptD::_Env));


	m_ppLcl 	= new CLcmSptB*[m_Inf.iNfZ];

	INT	iWx = m_Inf.iExt/m_Inf.iNx;

	for(int z=0; z<m_Inf.iNfZ; ++z)
	{
		m_ppLcl[z] = new CLcmSptB[m_Inf.iNfX];

		for(int x=0; x<m_Inf.iNfX; ++x)
		{
			VEC3	pos = VEC3( x * m_Inf.iExt, 0, z * m_Inf.iExt);

			m_ppLcl[z][x].Create(this, m_pFVF, m_pEft, NULL);
			m_ppLcl[z][x].SetPos(pos);
			m_ppLcl[z][x].CreateMesh(m_Inf.iNx+1, iWx, m_Inf.fUV);
			m_ppLcl[z][x].LayerSetup(pTxLst);
		}
	}

}


INT CLcmSptD::FileLoad(char* sFile)
{
	FILE*	fp;
	INT 	x, z, i=0, j=0;

	fp = fopen(sFile, "rb");

	if(!fp)
		return -1;


	if(m_ppLcl)
	{
		for(int z=0; z<m_Inf.iNfZ; ++z)
			SAFE_DELETE_ARRAY(	m_ppLcl[z]	);

		SAFE_DELETE_ARRAY(	m_ppLcl );
	}

	memset(&m_Inf, 0, sizeof(m_Inf));
	memset(&m_Env, 0, sizeof(m_Env));


	fread(&m_Inf, 1, sizeof(CLcmSptD::_Inf), fp);
	fread(&m_Env, 1, sizeof(CLcmSptD::_Env), fp);


	m_ppLcl 	= new CLcmSptB*[m_Inf.iNfZ];

	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		m_ppLcl[z] = new CLcmSptB[m_Inf.iNfX];

		for(x=0; x<m_Inf.iNfX; ++x)
			m_ppLcl[z][x].Create(this, m_pFVF, m_pEft, NULL);
	}


	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			CLcmSptB* pLc = &m_ppLcl[z][x];

			fread(&pLc->m_mtW,	1, sizeof(pLc->m_mtW),	fp);					// World Matrix
			fread(&pLc->m_vcP,	1, sizeof(pLc->m_vcP),	fp);					// Position

			// Mesh
			fread(&pLc->m_iNx,	1, sizeof(pLc->m_iNx), fp); 					// Number of tile for Width
			fread(&pLc->m_fWx,	1, sizeof(pLc->m_fWx), fp); 					// Width of tile for x;
			fread(&pLc->m_iVs,	1, sizeof(pLc->m_iVs), fp); 					// Vertex Size
			fread(&pLc->m_FVF,	1, sizeof(pLc->m_FVF), fp);
			fread(&pLc->m_fUV,	1, sizeof(pLc->m_fUV), fp); 					// UV Width
			fread(&pLc->m_iNi,	1, sizeof(pLc->m_iNi), fp); 					// Index Number
			fread(&pLc->m_iNv,	1, sizeof(pLc->m_iNv), fp); 					// Vertex Number

			pLc->m_pFce	= new CLcmSptB::VtxIdx[pLc->m_iNi];
			pLc->m_pVtx	= new CLcmSptB::VtxSpl[pLc->m_iNv];
			pLc->m_pVtt = new CLcmSptB::SplIdx[pLc->m_iNv];
			fread(pLc->m_pFce, pLc->m_iNi, sizeof(CLcmSptB::VtxIdx), fp);
			fread(pLc->m_pVtx, pLc->m_iNv, sizeof(CLcmSptB::VtxSpl), fp);
			fread(pLc->m_pVtt, pLc->m_iNv, sizeof(CLcmSptB::SplIdx), fp);


			// Tiling
			INT iSize = 0;
			fread(&iSize, 1, sizeof(INT), fp);

			for(int n=0; n<iSize; ++n)
			{
				CLcmSptB::MpTex	pLayer;
				fread(pLayer.TexB.sName, 1, CLcmSptB::LCXM_TEX_NAME, fp);
				pLc->m_vTex.push_back(pLayer);
			}

			WORD	nIds[4]={0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
			VEC3	vcR;
			VEC3	vcS;
			VEC3	vcT;
			char	sMdN[CLcmSptB::LCXM_MDL_NAME]={0};

			// Object List 2D
			fread(&iSize, 1, sizeof(INT), fp);

			for(i=0; i<iSize; ++i)
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

				char* p = strrchr(sMdN, '.');
				if(0 == _stricmp(p, ".ini"))
				{
					*p=0;
					strcat(sMdN, ".lcb");
				}

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
					pMdB->Query("Set Package", pLc->m_pMdb2D);

					pLc->m_pMdb2D->Push_Back(pMdB);
				}
			}


			// Object List 3D
			fread(&iSize, 1, sizeof(INT), fp);

			for(i=0; i<iSize; ++i)
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
				void*	pSrc = NULL;

				char* p = strrchr(sMdN, '.');
				if(0 == _stricmp(p, ".ini"))
				{
					*p=0;
					strcat(sMdN, ".lcs");
				}

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
					pMdB->Query("Set Package", pLc->m_pMdb3D);

					pLc->m_pMdb3D->Push_Back(pMdB);
				}
			}
		}
	}

	fclose(fp);


	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			CLcmSptB* pLc = &m_ppLcl[z][x];

			// Tiling
			INT iSize = (INT)(pLc->m_vTex.size());

			for(int n=0; n<iSize; ++n)
			{
				CLcmSptB::MpTex*	pLayer = &pLc->m_vTex[n];

				INT	nM = -1;
				INT nS = -1;
				char* sTexB = pLayer->TexB.sName;

				m_pTbTex->Select((void**)&pLayer->TexB.pTex, &nM, &nS, sTexB);
				m_pTbTex->SelectInfo((void**)&pLayer->TexB.Image, nM, nS);
			}

		}
	}


	return 0;
}


INT CLcmSptD::FileSave(char* sFile)
{
	FILE*	fp;
	INT 	x, z, i=0, j=0;

	fp = fopen(sFile, "wb");

	fwrite(&m_Inf, 1, sizeof(CLcmSptD::_Inf), fp);
	fwrite(&m_Env, 1, sizeof(CLcmSptD::_Env), fp);

	for(z=0; z<m_Inf.iNfZ; ++z)
	{
		for(x=0; x<m_Inf.iNfX; ++x)
		{
			CLcmSptB* pLc = &m_ppLcl[z][x];

			fwrite(&pLc->m_mtW, 1, sizeof(pLc->m_mtW),	fp);					// World Matrix
			fwrite(&pLc->m_vcP, 1, sizeof(pLc->m_vcP),	fp);					// Position

			// Mesh
			fwrite(&pLc->m_iNx, 1, sizeof(pLc->m_iNx), fp); 					// Number of tile for Width
			fwrite(&pLc->m_fWx, 1, sizeof(pLc->m_fWx), fp); 					// Width of tile for x;
			fwrite(&pLc->m_iVs, 1, sizeof(pLc->m_iVs), fp); 					// Vertex Size
			fwrite(&pLc->m_FVF, 1, sizeof(pLc->m_FVF), fp);
			fwrite(&pLc->m_fUV, 1, sizeof(pLc->m_fUV), fp); 					// UV Width
			fwrite(&pLc->m_iNi, 1, sizeof(pLc->m_iNi), fp); 					// Index Number
			fwrite(&pLc->m_iNv, 1, sizeof(pLc->m_iNv), fp); 					// Vertex Number

			fwrite(pLc->m_pFce, pLc->m_iNi, sizeof(CLcmSptB::VtxIdx), fp);
			fwrite(pLc->m_pVtx, pLc->m_iNv, sizeof(CLcmSptB::VtxSpl), fp);
			fwrite(pLc->m_pVtt, pLc->m_iNv, sizeof(CLcmSptB::SplIdx), fp);

			// Tiling
			INT iSize = (INT)(pLc->m_vTex.size());
			fwrite(&iSize, 1, sizeof(INT), fp);

			for(INT n=0; n<iSize; ++n)
			{
				CLcmSptB::MpTex*	pLayer = &pLc->m_vTex[n];
				fwrite(pLayer->TexB.sName, 1, CLcmSptB::LCXM_TEX_NAME, fp);
			}


			WORD	nIds[4]={0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
			VEC3	vcR;
			VEC3	vcS;
			VEC3	vcT;

			char	sMdN[CLcmSptB::LCXM_MDL_NAME]={0};


			// Object List 2D
			iSize = pLc->m_pMdb2D->Size();
			fwrite(&iSize, 1, sizeof(INT), fp);

			for(i=0; i<iSize; ++i)
			{
				ILcMdl*	pMdB = pLc->m_pMdb2D->GetObj(i);

				memset(sMdN, 0, sizeof(sMdN));
				sprintf(sMdN, "%s", pMdB->GetName());

				pMdB->Query("get ids", nIds);

				pMdB->GetRotation((FLOAT*)&vcR);
				pMdB->GetScaling((FLOAT*)&vcS);
				pMdB->GetPosition((FLOAT*)&vcT);

				fwrite(&nIds[0], 1, sizeof(WORD), fp);
				fwrite(&nIds[1], 1, sizeof(WORD), fp);
				fwrite(&nIds[2], 1, sizeof(WORD), fp);
				fwrite(&nIds[3], 1, sizeof(WORD), fp);

				fwrite(&vcR,	 1, sizeof(VEC3), fp);
				fwrite(&vcS,	 1, sizeof(VEC3), fp);
				fwrite(&vcT,	 1, sizeof(VEC3), fp);

				fwrite(sMdN,	 1, sizeof(sMdN), fp);
			}


			// Object List 3D
			iSize = pLc->m_pMdb3D->Size();

			fwrite(&iSize, 1, sizeof(INT), fp);

			for(i=0; i<iSize; ++i)
			{
				ILcMdl*		pMdB = pLc->m_pMdb3D->GetObj(i);

				memset(sMdN, 0, sizeof(sMdN));
				sprintf(sMdN, "%s", pMdB->GetName());

				pMdB->Query("get ids", nIds);

				pMdB->GetRotation((FLOAT*)&vcR);
				pMdB->GetScaling((FLOAT*)&vcS);
				pMdB->GetPosition((FLOAT*)&vcT);

				fwrite(&nIds[0], 1, sizeof(WORD), fp);
				fwrite(&nIds[1], 1, sizeof(WORD), fp);
				fwrite(&nIds[2], 1, sizeof(WORD), fp);
				fwrite(&nIds[3], 1, sizeof(WORD), fp);

				fwrite(&vcR,	 1, sizeof(VEC3), fp);
				fwrite(&vcS,	 1, sizeof(VEC3), fp);
				fwrite(&vcT,	 1, sizeof(VEC3), fp);

				fwrite(sMdN,	 1, sizeof(sMdN), fp);
			}
		}
	}


	fclose(fp);

	return 0;
}




CLcmSptD::_Inf* CLcmSptD::GetFldInf()
{
	return &m_Inf;
}

char* CLcmSptD::GetFldName()
{
	return m_Inf.sN;
}


void CLcmSptD::EraseObj()
{
	if(!m_ppLcl || !m_pPkObj)
		return;


	CLcmSptB*	pLcl	= NULL;
	ILcMdl*		pObj1	= m_pPkObj;
	ILcMdl*		pObj2	= NULL;
	BOOL		bFind	= FALSE;
	ILcmPack*	pPack	= NULL;


	if(SUCCEEDED(pObj1->Query("Get Package", &pPack)))
	{
		if(SUCCEEDED(pPack->Erase(pObj1)))
		{
			m_pPkObj = NULL;
			return;
		}
	}



}



void* CLcmSptD::GetPckObj()
{
	return m_pPkObj;
}


PDVD CLcmSptD::GetVertexDeclarator()
{
	return m_pFVF;
}


LPD3DXEFFECT CLcmSptD::GetD3DXEffect()
{
	return m_pEft;
}


ILcCam* CLcmSptD::GetCam()
{
	return m_pCam;
}


INT CLcmSptD::GetHeight(float* vcOut, const float* vcIn)
{
	D3DXVECTOR3 vcP	= *((D3DXVECTOR3*)vcIn);
	INT	nX = INT(vcP.x/m_Inf.iExt);
	INT	nZ = INT(vcP.z/m_Inf.iExt);

	CLcmSptB*	pLcl	= NULL;

	if(nX>=0 && nZ>=0)
		pLcl = &m_ppLcl[nZ][nX];
	else
		return -1;


	FLOAT	fY   = pLcl->GetHeight(vcP);
	*((D3DXVECTOR3*)vcOut) = vcP;
	((D3DXVECTOR3*)vcOut)->y =fY;
	return 0;
}

