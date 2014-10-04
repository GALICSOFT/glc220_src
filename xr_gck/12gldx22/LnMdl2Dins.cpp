// Implementation of the CLcMdl2Dins class.
//
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "../11gldc22/LnD3Type.h"

#include <LnUtil.h>
#include <LnUtilDx.h>
#include <LnBase.h>


#include "LnxEntity.h"

#include "LnMdl.h"
#include "LnMdl2D.h"
#include "LnMdl2Dins.h"


CLcMdl2Dins::CLcMdl2Dins()
{
	m_pOrg	= NULL;

	m_pPack	= NULL;
	m_vcEps	= VEC3(0,0,0);
}



CLcMdl2Dins::~CLcMdl2Dins()
{
	Destroy();
}



INT CLcMdl2Dins::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_pOrg = (CLcMdl2D*)( ((CLcMdl*)p2)->GetOriginal() );

	// Bound Box Setup
	TbndAB* pBnd  = (TbndAB*)m_pOrg->GetBndInf();
	if(pBnd)
		m_BndInf.Set( *pBnd->GetMin(), *pBnd->GetMax());

	return 0;
}


void CLcMdl2Dins::Destroy()
{
}


INT CLcMdl2Dins::FrameMove()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();


	WorldMatrixSetup();
	SetupSortValue();


	D3DXMATRIX	mtWld;
	D3DXMATRIX	mtViw;
	D3DXMATRIX	mtViwI;

	pDev->GetTransform(D3DTS_VIEW, &mtViw);
	D3DXMatrixInverse(&mtViwI, NULL, &mtViw);

	mtViwI._41 = 0;
	mtViwI._42 = 0;
	mtViwI._43 = 0;

	mtWld = mtViwI * m_mtWld;


	VtxDUV1* pVtx = m_pOrg->m_pVtx;
	memcpy(m_pVtx, pVtx, sizeof(VtxDUV1) * 4);

	m_vcEps.y = -pVtx[2].p.y*.9f;
	SetColor( D3DXCOLOR( 1, 1, 1,  (1000.f)/fabsf(m_fStlSrtR)  ));

	for(int i=0; i<4; ++i)
	{
		m_pVtx[i].d = m_dC;
		
		D3DXVec3TransformCoord(&m_pVtx[i].p, &pVtx[i].p, &mtWld);
		m_pVtx[i].p += m_vcEps;
	}

	//Transform Bound Box
	TbndAB*	pBndSrc = (TbndAB*)m_pOrg->GetBndInf();
	TbndAB* pBndDst = &m_BndInf;
	Lcm_BoundBoxTransform(pBndDst, pBndSrc, &mtWld, &m_vcEps);

	return 0;
}

void CLcMdl2Dins::Render()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	DWORD dMnLgt;

	pDev->GetRenderState( D3DRS_LIGHTING, &dMnLgt);

	pDev->SetRenderState( D3DRS_LIGHTING, FALSE);

	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDev->SetRenderState(D3DRS_ALPHAREF, 0x40);
	pDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	
	pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	PLC_TEXTURE pTex	= m_pOrg->m_pTex;
	PDTX		pDXTex	= (PDTX)pTex->GetPointer();

	pDev->SetTexture(0, pDXTex);
	pDev->SetFVF(VtxDUV1::FVF);
	pDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_pVtx, sizeof(VtxDUV1));


	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
	pDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	pDev->SetRenderState( D3DRS_LIGHTING, dMnLgt);
}



INT CLcMdl2Dins::Query(char* sCmd, void* pVal)
{
	if(0==_stricmp("Set Package", sCmd))
	{
		m_pPack	= pVal;
		return 0;
	}

	else if(0==_stricmp("Get Package", sCmd))
	{
		ULONG_PTR nAddress = ((ULONG_PTR)m_pPack);
		*((ULONG_PTR*)pVal) = nAddress;
		return 0;
	}

	else if(0==_stricmp("Set Bound", sCmd))
	{
		TbndAB*	pSrc = (TbndAB*)pVal;
		Lcm_BoundBoxSetup(&m_BndInf, pSrc, this);	
		return 0;
	}

	else if(0==_stricmp("Get Bound", sCmd))
	{
		const TbndAB* pBnd = &m_BndInf;
		ULONG_PTR nAddress = ((ULONG_PTR)pBnd);
		*((ULONG_PTR*)pVal) = nAddress;

		return 0;
	}

	else if(0==_stricmp("Bound box render", sCmd))
	{
		LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

		Lcm_BoundBoxRender(pDev, &m_BndInf);
		return 0;
	}

	return CLcMdl::Query(sCmd, pVal);
}










