// Implementation of the CLcGeoAxis class.
//
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <D3D9.h>
#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"
#include "LnGeoAxis.h"


CLcGeoAxis::CLcGeoAxis()
{
}

CLcGeoAxis::~CLcGeoAxis()
{
	Destroy();
}



void CLcGeoAxis::Destroy()
{
}



INT CLcGeoAxis::Create(void* p1, void*, void*, void*)
{
	FLOAT	fMax = 10000;


	// m_pLine = new CLcGeoAxis::VtxD[ 6*2  + 8* 8];

	m_pLine[ 0] = CLcGeoAxis::VtxD(-fMax,     0,     0, 0xFF770000);
	m_pLine[ 1] = CLcGeoAxis::VtxD(    0,     0,     0, 0xFF770000);
	m_pLine[ 2] = CLcGeoAxis::VtxD(    0,     0,     0, 0xFFFF0000);
	m_pLine[ 3] = CLcGeoAxis::VtxD( fMax,     0,     0, 0xFFFF0000);
	
	m_pLine[ 4] = CLcGeoAxis::VtxD(    0, -fMax,     0, 0xFF007700);
	m_pLine[ 5] = CLcGeoAxis::VtxD(    0,     0,     0, 0xFF007700);
	m_pLine[ 6] = CLcGeoAxis::VtxD(    0,     0,     0, 0xFF00FF00);
	m_pLine[ 7] = CLcGeoAxis::VtxD(    0,  fMax,     0, 0xFF00FF00);
	
	m_pLine[ 8] = CLcGeoAxis::VtxD(    0,     0, -fMax, 0xFF000077);
	m_pLine[ 9] = CLcGeoAxis::VtxD(    0,     0,     0, 0xFF000077);
	m_pLine[10] = CLcGeoAxis::VtxD(    0,     0,     0, 0xFF0000FF);
	m_pLine[11] = CLcGeoAxis::VtxD(    0,     0,  fMax, 0xFF0000FF);

	return 0;
}



INT CLcGeoAxis::FrameMove()
{
	return 0;
}



void CLcGeoAxis::Render()
{
	// Render Lines
	DWORD dMnLgt;
	DWORD dMnFog;

	DWORD dMnAblend;
	DWORD dMnAtest;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->GetRenderState( D3DRS_LIGHTING, &dMnLgt);
	pDev->GetRenderState( D3DRS_FOGENABLE, &dMnFog);
	pDev->GetRenderState( D3DRS_ALPHABLENDENABLE,  &dMnAblend);
	pDev->GetRenderState( D3DRS_ALPHATESTENABLE,  &dMnAtest);

	pDev->SetRenderState( D3DRS_FOGENABLE, FALSE);
	pDev->SetRenderState( D3DRS_LIGHTING, FALSE);
	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	pDev->SetSamplerState(0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER , D3DTEXF_LINEAR);

	
	pDev->SetTexture(0, 0);
	pDev->SetFVF(CLcGeoAxis::VtxD::FVF);
	pDev->DrawPrimitiveUP(D3DPT_LINELIST, 6, m_pLine, sizeof(CLcGeoAxis::VtxD));

	pDev->SetRenderState( D3DRS_LIGHTING, dMnLgt);
	pDev->SetRenderState( D3DRS_FOGENABLE, dMnFog);
	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, dMnAblend);
	pDev->SetRenderState( D3DRS_ALPHATESTENABLE, dMnAtest);
}



INT CLcGeoAxis::Query(char*, void*)
{
	return -1;
}


