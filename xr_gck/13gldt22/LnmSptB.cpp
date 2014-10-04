// Implementation of the CLcmSptB class.
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

#include "LnmSptB.h"
#include "LnmSptD.h"


CLcmSptB::MpTex::_Tex::_Tex()
{
	pTex	= NULL;
	memset(&Image, 0, sizeof Image);
	memset(sName, 0, sizeof sName);
}

CLcmSptB::MpTex::MpTex()
{
}



CLcmSptB::CLcmSptB()
{
	D3DXMatrixIdentity(&m_mtW);

	m_vcP		= D3DXVECTOR3(0,0,0);

	m_iNx		= 0;
	m_fWx		= 0.f;
	m_fUV		= 0;

	m_iVs		= 0;
	m_FVF		= 0x0;
	m_iNi		= 0;
	m_iNv		= 0;

	m_pFce		= NULL;
	m_pVtx		= NULL;
	m_pVtt		= NULL;
	m_bRend		= TRUE;

	m_pMdb2D	= NULL;
	m_pMdb3D	= NULL;

	m_pDst		= NULL;
}



CLcmSptB::~CLcmSptB()
{
	Destroy();
}



void CLcmSptB::Destroy()
{
	D3DXMatrixIdentity(&m_mtW);

	m_iNx	= 0;
	m_fWx	= 0;
	m_iNi	= 0;
	m_iNv	= 0;
	m_fUV	= 1.f;
	m_iVs	= 0;
	m_FVF	= 0;

	SAFE_DELETE_ARRAY(	m_pFce	);
	SAFE_DELETE_ARRAY(	m_pVtx	);
	SAFE_DELETE_ARRAY(	m_pVtt	);

	m_vTex.clear();

	SAFE_DELETE(	m_pMdb2D	);
	SAFE_DELETE(	m_pMdb3D	);
}



INT CLcmSptB::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_pDst	= p1;

	if(FAILED(LcMdl_CreatePack(	NULL, &m_pMdb2D, this)))
		return -1;

	if(FAILED(LcMdl_CreatePack(	NULL, &m_pMdb3D, this)))
		return -1;

	return 0;
}


INT CLcmSptB::FrameMove()
{
	if(	m_pMdb2D	)
		m_pMdb2D->FrameMove();

	INT		i;
	INT		iSize = 0;
	ILcMdl*	pMdB  = NULL;

	iSize = m_pMdb2D->Size();

	for(i=0; i<iSize; ++i)
	{
		pMdB  = m_pMdb2D->GetObj(i);
		((CLcmSptD*)m_pDst)->AddObj2D(pMdB);
	}

	SAFE_FRMOV(	m_pMdb3D	);

	iSize = m_pMdb3D->Size();

	for(i=0; i<iSize; ++i)
	{
		pMdB  = m_pMdb3D->GetObj(i);
		((CLcmSptD*)m_pDst)->AddObj3D(pMdB);
	}

	return 0;
}


void CLcmSptB::RenderBlc()
{
	if(!m_pVtx)
		return;

	if(!m_bRend)
		return;


	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();


	HRESULT hr	= -1;
	INT		i;

	CLcmSptD*	pDst = (CLcmSptD*)m_pDst;
	ILcCam*	pCam = pDst->GetCam();
	PDVD		pFVF = (PDVD)((CLcmSptD*)m_pDst)->GetVertexDeclarator();
	PDEF		pEft = (PDEF)((CLcmSptD*)m_pDst)->GetD3DXEffect();

	INT			nL	= (INT)m_vTex.size();

	D3DXMATRIX	mtViw;
	D3DXMATRIX	mtPrj;
	D3DXVECTOR4	vcCamEye;
	D3DXVECTOR4	vcCamZ;

	FLOAT		Fog		= 0;
	float		FogBgn	= 0;
	float		FogEnd	= 0;
	D3DXCOLOR	FogColor(1,1,1,1);

	DWORD	dFt;
	DWORD	dFc;
	DWORD	dFb;
	DWORD	dFe;

	pDev->GetRenderState(D3DRS_FOGENABLE	, &dFt);
	pDev->GetRenderState(D3DRS_FOGSTART	, &dFb);
	pDev->GetRenderState(D3DRS_FOGEND		, &dFe);
	pDev->GetRenderState(D3DRS_FOGCOLOR	, &dFc);

	if(dFt)
	{
		Fog		= 1.f;
		FogBgn	= *((FLOAT*)&dFb);
		FogEnd	= *((FLOAT*)&dFe);
		FogColor= dFc;
		FogColor.a = 1.f;
	}

	mtViw = *pCam->GetMatrixViw();
	mtPrj = *pCam->GetMatrixPrj();

	vcCamEye= (D3DXVECTOR4)*pCam->GetEye();
	vcCamZ	= (D3DXVECTOR4)*pCam->GetAxisZ();

	hr = pDev->SetVertexDeclaration( pFVF );			// Á¤Á¡¼±¾ð

	hr = pEft->SetMatrix( "m_mtViw", &mtViw);			// ÇÁ·ÎÁ§¼Ç Çà·Ä
	hr = pEft->SetMatrix( "m_mtPrj", &mtPrj);			// ºä Çà·Ä

	hr = pEft->SetVector( "m_vcCamEye", &vcCamEye);		// ºä Çà·Ä
	hr = pEft->SetVector( "m_vcCamZ", &vcCamZ);			// ºä Çà·Ä

	hr = pEft->SetFloat( "m_Fog"		, Fog		);		// Fog Enable
	hr = pEft->SetFloat( "m_FogBgn"		, FogBgn	);		// Fog ½ÃÀÛ
	hr = pEft->SetFloat( "m_FogEnd"		, FogEnd	);		// Fog ³¡
	hr = pEft->SetVector( "m_FogColor"	, (D3DXVECTOR4*)&FogColor	);		// Fog »ö»ó


	hr = pEft->SetTechnique( "Tech0" );

	for(i = 0; i<nL; ++i)
	{
		char	sTx[LCXM_TEX_NAME]={0};
		PDTX	pTexB	= m_vTex[i].TexB.pTex;
		sprintf(sTx, "m_TxDif%d", i);

		hr = pEft->SetTexture(sTx, pTexB);
	}

	hr = pEft->Begin(NULL, 0);
	hr = pEft->Pass(0);

	hr = pDev->DrawIndexedPrimitiveUP(
							D3DPT_TRIANGLELIST
							, 0
							, m_iNv
							, m_iNi
							, m_pFce
							, D3DFMT_INDEX16
							, m_pVtx
							, m_iVs
							);

	pEft->End();

	hr = pDev->SetVertexDeclaration(NULL);
	hr = pDev->SetVertexShader(NULL);
	hr = pDev->SetPixelShader(NULL);


//	pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//	pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//	pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
//	pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
//
//	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
//
//
//	pDev->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE);
//	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE,  TRUE);
//	pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR);
//	pDev->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_ZERO);
//
//	pDev->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE);
//
//
//	pDev->SetTexture(0, NULL);
//	pDev->SetTexture(1, NULL);
//	pDev->SetTexture(2, NULL);
//	pDev->SetTexture(3, NULL);
//
//	pDev->SetFVF(CLcmSptD::VtxSpl::FVF);
//
//
//	hr = pDev->DrawIndexedPrimitiveUP(
//							D3DPT_TRIANGLELIST
//							, 0
//							, m_iNv
//							, m_iNi
//							, m_pFce
//							, D3DFMT_INDEX16
//							, m_pVtx
//							, m_iVs
//							);
//
//	pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
//	pDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
//	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE,  FALSE);
//	pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
//	pDev->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA);
//
//	pDev->SetRenderState( D3DRS_LIGHTING,  FALSE);
//	pDev->SetRenderState( D3DRS_SPECULARENABLE,  FALSE);
}


void CLcmSptB::RenderObj()
{
	if(FALSE == m_bRend)
		return;

	SAFE_RENDER(	m_pMdb3D		);
	SAFE_RENDER(	m_pMdb2D		);
}



void CLcmSptB::RenderTline()
{
	if(FALSE == m_bRend)
		return;

	struct _VtxD
	{
		D3DXVECTOR3	p;
		D3DCOLOR	d;

		_VtxD()							: p(0,0,0),d(0XFFFFFFFF){}
		_VtxD(FLOAT X,FLOAT Y,FLOAT Z
			,D3DCOLOR D=0xFFFFFFFF)		: p(X,Y,Z),d(D){}

		enum { FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE) };
	};


	_VtxD	pLine[5];

	FLOAT	fW	= (m_iNx-1) * m_fWx - 4.f;
	FLOAT	Px	= m_vcP.x;
	FLOAT	Py	= m_vcP.y+10;
	FLOAT	Pz	= m_vcP.z;

	pLine[0] = _VtxD(Px+ 0, Py, Pz+ 0, 0xFFFFFF00);
	pLine[1] = _VtxD(Px+ 0, Py, Pz+fW, 0xFFFFFF00);
	pLine[2] = _VtxD(Px+fW, Py, Pz+fW, 0xFFFFFF00);
	pLine[3] = _VtxD(Px+fW, Py, Pz+ 0, 0xFFFFFF00);
	pLine[4] = pLine[0];

	DWORD dMnLgt;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->GetRenderState( D3DRS_LIGHTING, &dMnLgt);

	pDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetTexture(0,0);
	pDev->SetFVF(_VtxD::FVF);
	pDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, pLine, sizeof(_VtxD));

	pDev->SetRenderState( D3DRS_LIGHTING, dMnLgt);
}



void CLcmSptB::CreateMesh(INT iNx, FLOAT fWx, FLOAT fUV)
{
	INT		z, x;

	m_iNx	= iNx;
	m_fWx	= fWx;
	m_fUV	= fUV;

	m_iNv	= m_iNx * m_iNx;
	m_FVF	= VtxSpl::FVF;
	m_iVs	= sizeof(VtxSpl);
	m_pVtx	= new VtxSpl[m_iNv];
	m_pVtt	= new SplIdx[m_iNv];

	memset(m_pVtt, 0, sizeof(INT) * m_iNv);

	for(z=0; z<m_iNx; ++z)
	{
		for(x=0; x<m_iNx; ++x)
		{
			m_pVtx[z * m_iNx + x].p = VEC3(FLOAT(x), 0.f, FLOAT(z)) * m_fWx;

			m_pVtx[z * m_iNx + x].p += m_vcP;
			m_pVtx[z * m_iNx + x].u = x /m_fUV;
			m_pVtx[z * m_iNx + x].v = z /m_fUV;
		}
	}

	SetNormal();
	SetIndex();
}


void CLcmSptB::LayerSetup(lsMpTex* pTxLst)
{
	int iSize = pTxLst->size();

	lsMpTex::iterator _F = pTxLst->begin();
	lsMpTex::iterator _L = pTxLst->end();

	for(; _F != _L; ++_F)
	{
		m_vTex.push_back(*_F);
	}
}


void CLcmSptB::LayerAdd(MpTex* pTex)
{
	INT iSize = (INT)m_vTex.size();

	if(iSize>=6)
		return;

	m_vTex.push_back(*pTex);
}





void CLcmSptB::SetDiffuse(int nIdx)
{
	INT		x,z;

	if(-1 == nIdx)
	{
		for(z=0; z<m_iNx; ++z)
		{
			for(x=0; x<m_iNx; ++x)
			{
				FLOAT fH = m_pVtx[z * m_iNx + x].p.y;
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

				m_pVtx[z * m_iNx + x].d = c;
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

void CLcmSptB::SetIndex()
{
	INT		x, z;
	INT		iN;


	iN = m_iNx-1;

	m_iNi = 2 * (m_iNx-1) * (m_iNx-1);

	m_pFce = new VtxIdx[m_iNi];

	INT i=0;

	WORD index;
	WORD f[9];

	for(z=0; z< iN/2;++z)														// Index¸¦ Ã¤¿î´Ù.
	{
		for(x=0;x<iN/2;++x)
		{
			index = 2*m_iNx*z + m_iNx+1 + 2*x;

			f[6] = index +m_iNx-1;	f[5] = index + m_iNx;	f[4] = index +m_iNx	+1;
			f[7] = index	   -1;	f[8] = index		;	f[3] = index		+1;
			f[0] = index -m_iNx-1;	f[1] = index - m_iNx;	f[2] = index -m_iNx	+1;


			i = z * iN/2 + x;
			i *=8;

			for(int m=0; m<8; ++m)
				m_pFce[i+m] = VtxIdx( f[8], f[(m+1)%8], f[(m+0)%8]);
		}
	}
}


void CLcmSptB::SetNormal()
{
	INT		x,z;
	VEC3	n(0,0,0);
	VEC3	a;
	VEC3	b;
	VEC3	nT;

	for(z=0; z<m_iNx; ++z)
	{
		for(x=0; x<m_iNx; ++x)
		{
			m_pVtx[m_iNx*z + x].n = -NormalVec(z, x);
		}
	}
}






VEC3 CLcmSptB::NormalVec(int z, int x)
{
	VEC3	n(0,0,0);
	VEC3	a;
	VEC3	b;
	VEC3	nT;
	INT		i;

	INT		index = m_iNx*z + x;
	INT		iVtx[10];

	iVtx[9] = index;
	iVtx[0] = iVtx[9];
	iVtx[1] = iVtx[9];
	iVtx[2] = iVtx[9];
	iVtx[3] = iVtx[9];
	iVtx[4] = iVtx[9];
	iVtx[5] = iVtx[9];
	iVtx[6] = iVtx[9];
	iVtx[7] = iVtx[9];
	iVtx[8] = iVtx[9];

	if(0==z && 0==x)
	{
		iVtx[0] = iVtx[9] + 1;
		iVtx[1] = iVtx[0] + m_iNx;
		iVtx[2] = iVtx[9] + m_iNx;
	}

	else if(0==z && (m_iNx-1) == x)
	{
		iVtx[0] = iVtx[9] + m_iNx;
		iVtx[1] = iVtx[0] - 1;
		iVtx[2] = iVtx[9] - 1;
	}

	else if(0==z)
	{
		if(index%2)
		{
			iVtx[0] = iVtx[9] + 1;
			iVtx[1] = iVtx[0] + m_iNx;
			iVtx[2] = iVtx[9] - 1;
		}
		else
		{
			iVtx[0] = iVtx[9] + 1;
			iVtx[1] = iVtx[0] + m_iNx;
			iVtx[2] = iVtx[9] + m_iNx;
			iVtx[3] = iVtx[2] - 1;
			iVtx[4] = iVtx[9] - 1;
		}
	}

	else if( (m_iNx-1) == z && 0==x)
	{
		iVtx[0] = iVtx[9] - m_iNx;
		iVtx[1] = iVtx[0] + 1;
		iVtx[2] = iVtx[9] + 1;
	}

	else if( (m_iNx-1) == z && (m_iNx-1) == x)
	{
		iVtx[0] = iVtx[9] - 1;
		iVtx[1] = iVtx[0] - m_iNx;
		iVtx[2] = iVtx[9] - m_iNx;

	}

	else if((m_iNx-1) == z)
	{
		if(index%2)
		{
			iVtx[0] = iVtx[9] - 1;
			iVtx[1] = iVtx[9] - m_iNx;
			iVtx[2] = iVtx[9] + 1;
		}
		else
		{
			iVtx[0] = iVtx[9] - 1;
			iVtx[1] = iVtx[0] - m_iNx;
			iVtx[2] = iVtx[9] - m_iNx;
			iVtx[3] = iVtx[2] + 1;
			iVtx[4] = iVtx[9] + 1;
		}
	}

	else if(0 == x)
	{
		if(index%2)
		{
			iVtx[0] = iVtx[9] - m_iNx;
			iVtx[1] = iVtx[9] + 1;
			iVtx[2] = iVtx[9] + m_iNx;
		}
		else
		{
			iVtx[0] = iVtx[9] - m_iNx;
			iVtx[1] = iVtx[0] + 1;
			iVtx[2] = iVtx[9] + 1;
			iVtx[3] = iVtx[2] + m_iNx;
			iVtx[4] = iVtx[9] + m_iNx;
		}
	}

	else if((m_iNx-1) == x)
	{
		if(index%2)
		{
			iVtx[0] = iVtx[9] + m_iNx;
			iVtx[1] = iVtx[9] - 1;
			iVtx[2] = iVtx[9] - m_iNx;
		}
		else
		{
			iVtx[0] = iVtx[9] + m_iNx;
			iVtx[1] = iVtx[0] - 1;
			iVtx[2] = iVtx[9] - 1;
			iVtx[3] = iVtx[2] - m_iNx;
			iVtx[4] = iVtx[9] - m_iNx;
		}
	}


	else
	{
		if(index%2)																// È¦ ¼ö
		{
			iVtx[0] = iVtx[9] - 1;
			iVtx[1] = iVtx[9] - m_iNx;
			iVtx[2] = iVtx[9] + 1;
			iVtx[3] = iVtx[9] + m_iNx;
			iVtx[4] = iVtx[0];
		}
		else																	// Â¦ ¼ö
		{
			iVtx[6] = index +m_iNx	-1;		iVtx[5] = iVtx[6] + 1;	iVtx[4] = iVtx[5] + 1;
			iVtx[7] = index			-1;		iVtx[9] = iVtx[7] + 1;	iVtx[3] = iVtx[9] + 1;
			iVtx[0] = index -m_iNx	-1;		iVtx[1] = iVtx[0] + 1;	iVtx[2] = iVtx[1] + 1;
			iVtx[8] = iVtx[0];
		}
	}

	for(i=0; i<8; ++i)
	{
		a = m_pVtx[iVtx[i+0] ].p - m_pVtx[iVtx[9] ].p;
		b = m_pVtx[iVtx[i+1] ].p - m_pVtx[iVtx[9] ].p;
		D3DXVec3Cross(&nT, &a, &b);
		D3DXVec3Normalize(&nT, &nT);
		n +=nT;
	}


	D3DXVec3Normalize(&n, &n);

	return n;
}




void CLcmSptB::CalculateMap()
{
	INT i;

	INT	nL	= (INT)m_vTex.size();

	for(i=0; i<nL; ++i)
	{
		CalculateMapTile(i);
	}
}


void CLcmSptB::CalculateMapTile(int nTx)
{
	INT		x, z, m, n;
	INT		nXBgn, nXEnd;
	INT		nZBgn, nZEnd;

	FLOAT	fAlpha;
	FLOAT	fN;
	FLOAT	fEpsilon= 4;

	for (z=0; z<m_iNx; ++z)
	{
		for (x=0; x<m_iNx; ++x)
		{
			fAlpha = 0.0f;

			if(z==0)
			{
				if(x==0)
				{
					nXBgn	= 0;	nXEnd	= 1;
					nZBgn	= 0;	nZEnd	= 1;
				}

				else if(x==(m_iNx-1))
				{
					nXBgn	= -1;	nXEnd	= 0;
					nZBgn	= 0;	nZEnd	= 1;
				}

				else
				{
					nXBgn	= -1;	nXEnd	= 1;
					nZBgn	= 0;	nZEnd	= 1;
				}
			}

			else if(z==(m_iNx-1))
			{
				if(x==0)
				{
					nXBgn	= 0;	nXEnd	= 1;
					nZBgn	= -1;	nZEnd	= 0;
				}

				else if(x==(m_iNx-1))
				{
					nXBgn	= -1;	nXEnd	= 0;
					nZBgn	= -1;	nZEnd	= 0;
				}

				else
				{
					nXBgn	= -1;	nXEnd	= 1;
					nZBgn	= -1;	nZEnd	= 0;
				}
			}

			else
			{
				if(x==0)
				{
					nXBgn	= 0;	nXEnd	= 1;
					nZBgn	= -1;	nZEnd	= 1;
				}

				else if(x==(m_iNx-1))
				{
					nXBgn	= -1;	nXEnd	= 0;
					nZBgn	= -1;	nZEnd	= 1;
				}

				else
				{
					nXBgn	= -1;	nXEnd	= 1;
					nZBgn	= -1;	nZEnd	= 1;
				}
			}

			for(m=nZBgn; m<=nZEnd; ++m)
			{
				for(n=nXBgn; n<=nXEnd; ++n)
				{
					int idx = (z+m) * m_iNx + x+n;

					if(m_pVtt[idx] != nTx)
						continue;

					if(0==m && 0==n)
						fAlpha +=fEpsilon;
					else
						fAlpha +=1.f;
				}
			}

			fN = 2 * (abs(nXEnd-nXBgn) + abs(nZEnd-nZBgn)) -1 + fEpsilon;

			fAlpha /= fN;


			VEC6 _color;

			memcpy(&_color, &m_pVtx[m_iNx*z + x].ts[0], sizeof(VEC6));

			_color[nTx] = fAlpha;

			memcpy(&m_pVtx[m_iNx*z + x].ts[0], &_color, sizeof(VEC6));
		}// for
	}// for
}



FLOAT CLcmSptB::GetHeight(VEC3& pos)
{
	INT	x;
	INT	z;

	z = INT((-m_vcP.z + pos.z)/ m_fWx);
	x = INT((-m_vcP.x + pos.x)/ m_fWx);


	INT index = m_iNx*z + x;

	VEC3 pos0 = m_pVtx[index].p;
	VEC3 posX;
	VEC3 posZ;
	VEC3 posT;


	float dX = pos.x - pos0.x;
	float dZ = pos.z - pos0.z;

	if( (x+z)%2)					// È¦¼ö
	{
		if(dZ >(m_fWx-dX))			// À§ÂÊ »ï°¢Çü
		{
			index += (m_iNx+1);

			pos0 = m_pVtx[index      ].p;
			posX = m_pVtx[index-1    ].p;
			posZ = m_pVtx[index-m_iNx].p;
		}

		else
		{
			pos0 = m_pVtx[index      ].p;
			posX = m_pVtx[index+1    ].p;
			posZ = m_pVtx[index+m_iNx].p;
		}
	}
	else					// Â¦¼ö
	{
		if(dZ > dX)			// À§ÂÊ »ï°¢Çü
		{
			index += (m_iNx);

			pos0 = m_pVtx[index      ].p;
			posX = m_pVtx[index+1    ].p;
			posZ = m_pVtx[index-m_iNx].p;
		}

		else
		{
			index += 1;

			pos0 = m_pVtx[index      ].p;
			posX = m_pVtx[index-1    ].p;
			posZ = m_pVtx[index+m_iNx].p;
		}
	}

	dX = fabsf(pos.x - pos0.x);
	dZ = fabsf(pos.z - pos0.z);

	posT  = pos0 + (posX - pos0) * dX/m_fWx + (posZ - pos0) * dZ/m_fWx;


	return posT.y;
}


