// Implementation of the ILcMdl class.
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

#include "LnMdl2D.h"
#include "LnMdl2Dins.h"
#include "LnMdlSM.h"
#include "LnMdlSMins.h"

#include "LnAseB.h"
#include "LnAseBins.h"

#include "LnMdlX.h"
#include "LnMdlXins.h"

#include "LnXSkin.h"
#include "LnXSkinAlloc.h"
#include "LnXSkinSrc.h"
#include "LnXSkinIns.h"



ILcMdl::_Tframe::_Tframe()
: nB(0)
, nE(0)
{
	memset(sN, 0, sizeof sN);
}


ILcMdl::_Tframe::_Tframe(char* s, INT B, INT E)
: nB(B)
, nE(E)
{
	memset(sN, 0, sizeof sN);
	strcpy(sN, s);
}



INT LcMdl_Create(char* sCmd
				 , ILcMdl** pData
				 , void* p1			// Original Source: Original Source가 NULL이면 새로 만듦
				 , void* p2			// Model File Name
				 , void* p3			// No Use
				 , void* p4			// No Use
				 )
{
	ILcMdl* pObj = NULL;

	char		Ext[_MAX_EXT]={0};
	const char*	sFile = NULL;
	char*		p  = NULL;


	(*pData) = NULL;


	if(p2)							// 파일 이름이 주어지면 Original을 만든다.
	{
		sFile = (char*)p2;
	}
	else							// 파일 이름 대신 Original이 주어지면 복사를 한다.
	{
		ILcMdl* pOrg = (ILcMdl*)p3;
		sFile  = pOrg->GetName();
	}

	p  = strrchr(sFile, '.');

	if(!p)
		return -1;

	strcpy(Ext, p+1);

	

	if(0==_stricmp(Ext, "acm"))
	{
		if(NULL == p3)
			pObj = new CLcAcm;
		else
			pObj = new CLcAcmIns;
	}
	else if(0==_stricmp(Ext, "lcb"))
	{
		if(NULL == p3)
			pObj = new CLcMdl2D;
		else
			pObj = new CLcMdl2Dins;
	}
	else if(0==_stricmp(Ext, "lcs"))
	{
		if(NULL == p3)
			pObj = new CLcMdlSM;
		else
			pObj = new CLcMdlSMins;
	}
	else if(0==_stricmp(Ext, "asb"))
	{
		if(NULL == p3)
			pObj = new CLcAseB;
		else
			pObj = new CLcAseBins;
	}
	else if(0==_stricmp(Ext, "xln"))
	{
		if(NULL == p3)
			pObj = new CLcMdlX;
		else
			pObj = new CLcMdlXins;
	}
	else if(0==_stricmp(Ext, "skn"))
	{
		if(NULL == p3)
			pObj = new CLcXSkinSrc;
		else
			pObj = new CLcXSkinIns;
	}
	else
	{
		return -1;
	}


	// 객체를 초기화 한다.
	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;
	return 0;
}
















//		Idx[ 0] = 0;	Idx[ 1] = 1;
//		Idx[ 2] = 1;	Idx[ 3] = 3;
//		Idx[ 4] = 3;	Idx[ 5] = 2;
//		Idx[ 6] = 2;	Idx[ 7] = 0;
//
//		Idx[ 8] = 0;	Idx[ 9] = 4;
//		Idx[10] = 1;	Idx[11] = 5;
//		Idx[12] = 3;	Idx[13] = 7;
//		Idx[14] = 2;	Idx[15] = 6;
//
//		Idx[16] = 4;	Idx[17] = 5;
//		Idx[18] = 5;	Idx[19] = 7;
//		Idx[20] = 7;	Idx[21] = 6;
//		Idx[22] = 6;	Idx[23] = 4;


static WORD	TBndAABB_nIdxLine[24]=
{
	0,	1,
	1,	3,
	3,	2,
	2,	0,

	0,	4,
	1,	5,
	3,	7,
	2,	6,

	4,	5,
	5,	7,
	7,	6,
	6,	4,
};


//					 
//		5------------7(Max)
//	y  /|           /|
//	| / |   z      / |
//	|/  |  /      /  |
//	1------------3   |
//	|   |/       |   |
//	|   4--------|---6
//	|  /         |  /
//	| /          | /
//	|/           |/
//	0(Min)-------2----- x

const static int TBndAABB_nIdxTri[][3] =
{
	{ 0, 1, 2},
	{ 3, 2, 1},

	{ 2, 3, 6},
	{ 7, 6, 3},

	{ 6, 7, 4},
	{ 5, 4, 7},

	{ 4, 5, 0},
	{ 1, 0, 5},

	{ 1, 5, 3},
	{ 7, 3, 5},

	{ 4, 0, 6},
	{ 2, 6, 0},
};



TbndAB::TbndAB()
{
	pOwn	= NULL;
}

TbndAB::TbndAB(const D3DXVECTOR3& _Min, const D3DXVECTOR3& _Max, void* _pOwn)
{
	pOwn	= NULL;
	Set(_Min, _Max, _pOwn);		
}

void TbndAB::Set(const D3DXVECTOR3& _Min, const D3DXVECTOR3& _Max, void* _pOwn)
{
	vcN	= _Min;
	vcM = _Max;
	vcC = (vcM + vcN) * 0.5f;

	fX = vcM.x -_Min.x;
	fY = vcM.y -_Min.y;
	fZ = vcM.z -_Min.z;

	fR = sqrtf( fX * fX + fY * fY + fZ * fZ);

	vcE[0].p = vcN;
	vcE[1].p = vcN + D3DXVECTOR3( 0, fY,  0);
	vcE[2].p = vcN + D3DXVECTOR3(fX,  0,  0);
	vcE[3].p = vcN + D3DXVECTOR3(fX, fY,  0);
	vcE[4].p = vcN + D3DXVECTOR3( 0,  0, fZ);
	vcE[5].p = vcN + D3DXVECTOR3( 0, fY, fZ);
	vcE[6].p = vcN + D3DXVECTOR3(fX,  0, fZ);
	vcE[7].p = vcM;

	if(_pOwn)
		pOwn = _pOwn;
}


void TbndAB::RenderBox(void* _pDev)
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)_pDev;
	DWORD dFill;

	pDev->GetRenderState(D3DRS_FILLMODE, &dFill);
	pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	pDev->SetTexture(0,0);
	pDev->SetFVF( (D3DFVF_XYZ|D3DFVF_DIFFUSE) );

	pDev->DrawIndexedPrimitiveUP(
				D3DPT_LINELIST, 0, 8, 12
				, TBndAABB_nIdxLine, D3DFMT_INDEX16
				, vcE, sizeof(TbndAB::_Tvtx));

	pDev->SetRenderState(D3DRS_FILLMODE, dFill);
}

void TbndAB::SetOwner(void* _pOwn)
{
	pOwn = _pOwn;
}

void* TbndAB::GetOwner()
{
	return pOwn;
}

void TbndAB::SetColor(DWORD dColor)
{
	for(int i=0 ;i<8; ++i)
		vcE[i].d = dColor;
}

D3DXVECTOR3* TbndAB::GetMin() const
{
	return (D3DXVECTOR3*)&(vcN);
}

D3DXVECTOR3* TbndAB::GetMax() const
{
	return (D3DXVECTOR3*)&(vcM);
}

D3DXVECTOR3* TbndAB::GetCenter() const
{
	return (D3DXVECTOR3*)&(vcC);
}

FLOAT TbndAB::GetWidth()	{	return  fX;	}
FLOAT TbndAB::GetHeight()	{	return	fY;	}
FLOAT TbndAB::GetDepth()	{	return	fZ;	}
FLOAT TbndAB::GetRadius()	{	return	fR;	}
DWORD TbndAB::GetColor()	{	return	vcE[0].d;	}



TbndAABB::TbndAABB()
{
	pBnd=0;
	fStlSrtR = FLT_MAX;
}

TbndAABB::TbndAABB(const D3DXVECTOR3& vcP, TbndAB* _pBnd, FLOAT _fD)
: vPck(vcP), pBnd(_pBnd), fStlSrtR(_fD)
{
}



INT	LcxUtil_PickAABB(FLOAT* fDist												// Distance
					, D3DXVECTOR3* pvcOut										// In, Out: Picking Position
					, D3DXVECTOR3* pvcCamPos									// In: Camera Position
					, D3DXVECTOR3* pvcRayDir									// In: Ray Direction
					, TbndAB* pIn												// In: Bound Box for Test.
					)
{
	D3DXVECTOR3	vcCamPos= *pvcCamPos;
	D3DXVECTOR3	vcRayDir= *pvcRayDir;

	D3DXVECTOR3	vcBndCent= *pIn->GetCenter();
	D3DXVECTOR3	vcT		 = vcBndCent - vcCamPos;

	FLOAT	fRadius = pIn->GetRadius();
	FLOAT	fDot	= D3DXVec3Dot(&vcRayDir, &vcT);
	FLOAT	fL		= 0.F;

	// 디렉션과 반대이면 빠져나감
	if(fDot<0.f)
		return -1;


	// 충돌 바운드 구와 비교도 한다.
	fL		 =D3DXVec3LengthSq(&vcT);
	fDot	*=fDot;
	fRadius *=fRadius;

	if( fL >(fDot + fRadius))
		return -1;


	// 충돌 바운드 구의 반경보다 작으면 12 개의 삼각형 충돌을 진행한다.
	

	FLOAT	fMax= FLT_MAX;
	FLOAT	U;
	FLOAT	V;
	FLOAT	D;
	D3DXVECTOR3*	p0;
	D3DXVECTOR3*	p1;
	D3DXVECTOR3*	p2;
	D3DXVECTOR3		vcPck;

	TbndAB::_Tvtx* vcE = pIn->vcE;

	for(int i=0; i<12; ++i)
	{
		p0 = &vcE[TBndAABB_nIdxTri[i][0] ].p;
		p1 = &vcE[TBndAABB_nIdxTri[i][1] ].p;
		p2 = &vcE[TBndAABB_nIdxTri[i][2] ].p;

		if(D3DXIntersectTri( p0, p1, p2, &vcCamPos, &vcRayDir, &U, &V, &D))
		{
			if(D<fMax)
			{
				if(pvcOut)
					*pvcOut = *p0 + U * (*p1 - *p0) + V * (*p2 - *p0);

				fMax = D;
			}
		}
	}

	// 충돌하면...
	if(FLT_MAX != fMax)
	{
		*fDist = fMax;
		return 0;
	}

	return -1;
}




