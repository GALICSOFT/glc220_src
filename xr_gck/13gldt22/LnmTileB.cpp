// Implementation of the CLcmTileB class.
//
////////////////////////////////////////////////////////////////////////////////


#include <Windows.h>
#include <stdio.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "../11gldc22/LnD3Type.h"

#include <LnUtil.h>
#include <LnUtilDx.h>
#include <LnBase.h>
#include <LnxEntity.h>

#include "LntEntity.h"

#include "LnmTileB.h"
#include "LnmTileD.h"






CLcmTileB::CLcmTileB()
{
	D3DXMatrixIdentity(&m_mtW);
	m_vcP		= VEC3(0,0,0);

	m_nCell		= 0;
	m_pTile		= NULL;

	m_fWx		= 0;
	m_iVs		= 0;
	m_FVF		= 0;
	m_nVtx		= 0;
	m_pVtx		= NULL;

	m_bRend		= TRUE;
	m_pMdLst	= NULL;
	m_pDst		= NULL;

	m_pDst		= NULL;
}



CLcmTileB::~CLcmTileB()
{
	Destroy();
}



void CLcmTileB::Destroy()
{
	D3DXMatrixIdentity(&m_mtW);

	m_nCell	= 0;
	SAFE_DELETE_ARRAY(	m_pTile		);
	m_fWx	= 0;
	m_nVtx	= 0;
	m_iVs	= 0;
	m_FVF	= 0;

	SAFE_DELETE_ARRAY(	m_pVtx	);

	SAFE_DELETE(	m_pMdLst	);
}


// Create: Texture, Model Table과 Device를 복사
INT CLcmTileB::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_pDst	= p1;

	if(FAILED(LcMdl_CreatePack(	NULL, &m_pMdLst, this)))
		return -1;

	return 0;
}


INT CLcmTileB::FrameMove()
{
	INT		i=0;
	INT		iSize = 0;
	ILcMdl*	pObj  = NULL;


	ILcCam* pCam = (ILcCam*)( ((CLcmTileD*)m_pDst)->GetCam() );

	FLOAT	fRadiusCam = pCam->GetFar() *0.7f;
	struct T
	{
		VEC3*	vcCam;
		FLOAT	fRadiusCam;
		DPLN*	plan;
	} t =
	{
		(VEC3*)pCam->GetEye(),
		fRadiusCam,
		(DPLN*)pCam->GetFrustumPlanes()
	};

	i=0;
	iSize = m_pMdLst->Size();

	// Setup Rendering Level
	for(i=0; i<iSize; ++i)
	{
		pObj  = m_pMdLst->GetObj(i);
		pObj->SetAttrib("Rendering Level", &t);
	}

	// FrameMove and Add Object
	for(i=0; i<iSize; ++i)
	{
		INT	 nRndLvl = 0;
		pObj  = m_pMdLst->GetObj(i);
		pObj->GetAttrib("Rendering Level", &nRndLvl);

		if(ILcMdl::LCM_RND_UPDATE > nRndLvl)
			continue;

		pObj->FrameMove();
		((CLcmTileD*)m_pDst)->AddObj(pObj);
	}

	return 0;
}


void CLcmTileB::RenderBlc()
{
	if(!m_pVtx)
		return;

	if(!m_bRend)
		return;


	HRESULT hr	= -1;


	CLcmTileD*	pDst = (CLcmTileD*)m_pDst;
	ILcCam*		pCam = (ILcCam*)( pDst->GetCam() );
	PDVD		pFVF = (PDVD)((CLcmTileD*)m_pDst)->GetVertexDeclarator();
	PDEF		pEft = (PDEF)((CLcmTileD*)m_pDst)->GetD3DXEffect();
	PDTX		pTex = (PDTX)((CLcmTileD*)m_pDst)->GetTexture();
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	MATA	mtWld;
	MATA	mtViw;
	MATA	mtPrj;


	D3DXMatrixIdentity(&mtWld);
	mtViw = *pCam->GetMatrixViw();
	mtPrj = *pCam->GetMatrixPrj();


//	pDev->SetFVF(m_FVF);
//	pDev->SetTexture(0, m_pTex.pTex);

	hr = pDev->SetVertexDeclaration( pFVF );			// 정점선언

	hr = pEft->SetMatrix( "m_mtWld", &mtWld);
	hr = pEft->SetMatrix( "m_mtViw", &mtViw);			// 프로젝션 행렬
	hr = pEft->SetMatrix( "m_mtPrj", &mtPrj);			// 뷰 행렬
	hr = pEft->SetTexture("m_Tex0", pTex);

	hr = pEft->SetTechnique( "Tech0" );
	hr = pEft->Begin(NULL, 0);
	hr = pEft->Pass(0);

	hr = pDev->DrawPrimitiveUP(
						D3DPT_TRIANGLELIST
						, m_nCell * m_nCell * 2
						, m_pVtx
						, m_iVs
						);

	pEft->End();

	hr = pDev->SetVertexDeclaration(NULL);
	hr = pDev->SetVertexShader(NULL);
	hr = pDev->SetPixelShader(NULL);
}



void CLcmTileB::RenderTline()
{
	if(FALSE == m_bRend)
		return;



	struct _VtxD
	{
		VEC3	p;
		DWORD	d;

		_VtxD()							: p(0,0,0),d(0XFFFFFFFF){}
		_VtxD(FLOAT X,FLOAT Y,FLOAT Z
			,DWORD D=0xFFFFFFFF)		: p(X,Y,Z),d(D){}

		enum { FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE) };
	};


	_VtxD	pLine[5];

	FLOAT	fW	= m_nCell * m_fWx - 4.f;
	FLOAT	Px	= m_vcP.x;
	FLOAT	Py	= m_vcP.y+10;
	FLOAT	Pz	= m_vcP.z;

	pLine[0] = _VtxD(Px+ 0, Py, Pz+ 0, 0xFFFFFF00);
	pLine[1] = _VtxD(Px+ 0, Py, Pz+fW, 0xFFFFFF00);
	pLine[2] = _VtxD(Px+fW, Py, Pz+fW, 0xFFFFFF00);
	pLine[3] = _VtxD(Px+fW, Py, Pz+ 0, 0xFFFFFF00);
	pLine[4] = pLine[0];

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	DWORD dMnLgt;

	pDev->GetRenderState( D3DRS_LIGHTING, &dMnLgt);

	pDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetTexture(0,0);
	pDev->SetFVF(_VtxD::FVF);
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, pLine, sizeof(_VtxD));

	pDev->SetRenderState( D3DRS_LIGHTING, dMnLgt);
}



void CLcmTileB::CreateFieldMesh(INT nTile, FLOAT fWx)
{
	int x, z;

	m_nCell	= nTile;
	m_fWx	= fWx;


	// Setup Tile
	m_pTile	= new CLcmTileB::_Tile[m_nCell * m_nCell];

	for(z=0; z<m_nCell; ++z)
	{
		for(x=0; x<m_nCell; ++x)
		{
			m_pTile[z* m_nCell + x].nIdS = 160;
			m_pTile[z* m_nCell + x].nIdP = 16;
		}
	}


	SetUpMesh();
	SetUpTexcoord();
}




void CLcmTileB::SetUpMesh()
{
	int x, z;

	// Setup Mesh
	m_nVtx	= m_nCell * m_nCell * 6;
	m_FVF	= CLcmTileB::_VtxDX::FVF;
	m_iVs	= sizeof(CLcmTileB::_VtxDX);
	m_pVtx	= new CLcmTileB::_VtxDX[m_nVtx];

	for(z=0; z<m_nCell; ++z)
	{
		for(x=0; x<m_nCell; ++x)
		{
			INT		n = (z*m_nCell + x)*6;

			m_pVtx[n+0].p = VEC3( x+0.f, 0.01f, z+0.f) * m_fWx + m_vcP;
			m_pVtx[n+1].p = VEC3( x+0.f, 0.01f, z+1.f) * m_fWx + m_vcP;
			m_pVtx[n+2].p = VEC3( x+1.f, 0.01f, z+0.f) * m_fWx + m_vcP;

			m_pVtx[n+3].p = VEC3( x+1.f, 0.01f, z+1.f) * m_fWx + m_vcP;
			m_pVtx[n+4].p = m_pVtx[n+2].p;
			m_pVtx[n+5].p = m_pVtx[n+1].p;
		}
	}


}


void CLcmTileB::SetUpTexcoord()
{
	int i, k;

	// Setup Texture Coord


	for (k=0; k<m_nCell; ++k)
	{
		for (i=0; i<m_nCell; ++i)
		{
			CLcmTileB::_Tile*	pTile = &m_pTile[k*m_nCell + i];
			INT		n = (k*m_nCell + i)*6;

			FLOAT x0, y0, x1, y1, z;

			pTile->GetTexcoordPrim(&x0, &y0, &x1, &y1);

			m_pVtx[n+0].t0 = VEC2(x0, y1);
			m_pVtx[n+1].t0 = VEC2(x0, y0);
			m_pVtx[n+2].t0 = VEC2(x1, y1);

			m_pVtx[n+3].t0 = VEC2(x1, y0);
			m_pVtx[n+4].t0 = VEC2(x1, y1);
			m_pVtx[n+5].t0 = VEC2(x0, y0);


			pTile->GetTexcoordSecond(&x0, &y0, &x1, &y1, &z);

			m_pVtx[n+0].t1 = VEC3(x0, y1, z);
			m_pVtx[n+1].t1 = VEC3(x0, y0, z);
			m_pVtx[n+2].t1 = VEC3(x1, y1, z);

			m_pVtx[n+3].t1 = VEC3(x1, y0, z);
			m_pVtx[n+4].t1 = VEC3(x1, y1, z);
			m_pVtx[n+5].t1 = VEC3(x0, y0, z);
		}// for
	}// for
}


void CLcmTileB::SetDiffuse(int nIdx)
{
	INT		x,z;

	if(-1 == nIdx)
	{
		for(z=0; z<m_nCell; ++z)
		{
			for(x=0; x<m_nCell; ++x)
			{
				FLOAT fH = m_pVtx[z * m_nCell + x].p.y;
				DWORD c;

				if( (fH) < -100.f )
					c = D3DCOLOR_XRGB(128, 120, 70);
				else if( (fH) < 100.f )
					c = D3DCOLOR_XRGB(255, 249, 157);
				else if( (fH) < 500.0f )
					c = D3DCOLOR_XRGB(124, 197, 118);
				else if( (fH) <2000.5f )
					c = D3DCOLOR_XRGB(  0, 166,  81);
				else if( (fH) < 3000.0f )
					c = D3DCOLOR_XRGB( 25, 123,  48);
				else if( (fH) < 4000.5f )
					c = D3DCOLOR_XRGB(115, 100,  87);
				else
					c = D3DCOLOR_XRGB(255, 255, 255);

				m_pVtx[z * m_nCell + x].d = c;
			}
		}
	}

	else
	{
		FLOAT fH = m_pVtx[nIdx].p.y;
		DWORD c;

		if( (fH) < -100.f )
			c = D3DCOLOR_XRGB(128, 120, 70);
		else if( (fH) < 100.f )
			c = D3DCOLOR_XRGB(255, 249, 157);
		else if( (fH) < 500.0f )
			c = D3DCOLOR_XRGB(124, 197, 118);
		else if( (fH) <2000.5f )
			c = D3DCOLOR_XRGB(  0, 166,  81);
		else if( (fH) < 3000.0f )
			c = D3DCOLOR_XRGB( 25, 123,  48);
		else if( (fH) < 4000.5f )
			c = D3DCOLOR_XRGB(115, 100,  87);
		else
			c = D3DCOLOR_XRGB(255, 255, 255);

		m_pVtx[nIdx].d = c;
	}
}





INT CLcmTileB::GetBlcHeight(D3DXVECTOR3* pOut, const D3DXVECTOR3* pIn)
{
	INT	nX;
	INT	nZ;

	D3DXVECTOR3	vIn = *pIn;

	// 블록의 시작 위치만큼 뺀다.
	vIn -= m_vcP;

	// 타일의 폭으로 나누어서 타일의 인덱스를 구한다.
	nX = INT(vIn.x/ m_fWx);
	nZ = INT(vIn.z/ m_fWx);


	//구한 타일 인덱스가 블럭 안의 타일 숫자 보다 크거나 작으면 실패
	if(nX<0 || nX>=m_nCell || nZ<0 || nZ>=m_nCell)
		return-1;


//	1  5         3
//	.  .→→→→┐
//	↑↘ ↖      ↓
//	↑  ↘ ↖    ↓
//	↑    ↘ ↖  ↓
//	↑      ↘ ↖↓
//	└←←←←.  .
//	0         2  4

	INT idx= (m_nCell * nZ + nX)*6;
	int _0 = idx + 0;
	int _1 = idx + 1;
	int _2 = idx + 2;
	int _3 = idx + 3;
	int _4 = idx + 4;
	int _5 = idx + 5;
	
	FLOAT dX = vIn.x - nX*m_fWx;
	FLOAT dZ = vIn.z - nZ*m_fWx;

	D3DXVECTOR3 vcX;
	D3DXVECTOR3 vcZ;
	D3DXVECTOR3 vcOut;

	if( (dX+dZ) <=m_fWx )	//아래쪽삼각형
	{
		vcX	= m_pVtx[_2].p - m_pVtx[_0].p;
		vcZ	= m_pVtx[_1].p - m_pVtx[_0].p;

		vcOut = vcX * dX/m_fWx + vcZ * dZ/m_fWx;
		vcOut+= m_pVtx[_0].p;
	}
	else					//위쪽삼각형
	{
		dX = m_fWx - dX;
		dZ = m_fWx - dZ;

		vcX= m_pVtx[_5].p - m_pVtx[_3].p;
		vcZ= m_pVtx[_4].p - m_pVtx[_3].p;

		vcOut  = vcX * dX/m_fWx + vcZ * dZ/m_fWx;
		vcOut += m_pVtx[_3].p;
	}

	*pOut=vcOut;
	return 0;
}


