// Implementation of the CLcMdlXins class.
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
#include "LnMdlX.h"
#include "LnMdlXins.h"



CLcMdlXins::CLcMdlXins()
{
	m_pOrg	= NULL;
}


CLcMdlXins::~CLcMdlXins()
{
	Destroy();
}


void CLcMdlXins::Destroy()
{
}


INT CLcMdlXins::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_pOrg	= (CLcMdlX*)((ILcMdl*)p2)->GetOriginal();

	return 0;
}


INT CLcMdlXins::FrameMove()
{
	return 0;
}


void CLcMdlXins::Render()
{
	DWORD				fmFc = m_pOrg->GetFmtFce();	
	DWORD				fmVx = m_pOrg->GetFmtVtx();

	DWORD				stFc = m_pOrg->GetStrideFce();
	DWORD				stVx = m_pOrg->GetStrideVtx();

	INT					nGeo = m_pOrg->GetNumGeo();
	CLcMdlX::LcMdlX*	pGeo = m_pOrg->GetGeometry();


	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	DWORD dMnLgt;

	pDev->GetRenderState( D3DRS_LIGHTING, &dMnLgt);
	pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDev->SetRenderState(D3DRS_ALPHAREF, 0x40);
	pDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );


	pDev->SetFVF(fmVx);
	pDev->SetTransform(D3DTS_WORLD, &m_mtWld);

	
	for(int i = 0; i < nGeo; ++i)
	{
		PDTX pTex = NULL;

		if(pGeo[i].pTex)
			pTex = (PDTX)pGeo[i].pTex->GetPointer();

		pDev->SetTexture(0, pTex);

		pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST
										, 0
										, pGeo[i].nVtx
										, pGeo[i].nFce
										, pGeo[i].pIdx
										, D3DFMT_INDEX16
										, pGeo[i].pVtx
										, stVx
										);
	}

	pDev->SetTexture(0, NULL);

	LcD3D_SetWorldIdentity(pDev);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	pDev->SetRenderState( D3DRS_LIGHTING, dMnLgt);
}


INT CLcMdlXins::Query(char* sCmd, void* pVal)
{
	return CLcMdl::Query(sCmd, pVal);
}

INT CLcMdlXins::SetAttrib(char* sCmd, void* pVal)
{
	return CLcMdl::SetAttrib(sCmd, pVal);
}

INT CLcMdlXins::GetAttrib(char* sCmd, void* pVal)
{
	return CLcMdl::GetAttrib(sCmd, pVal);
}

