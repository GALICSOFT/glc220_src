// Implementation of the CLcGeoGrid class.
//
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <D3D9.h>
#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"
#include "LnGeoGrid.h"


CLcGeoGrid::CLcGeoGrid()
{
}

CLcGeoGrid::~CLcGeoGrid()
{
	Destroy();
}



void CLcGeoGrid::Destroy()
{
}



INT CLcGeoGrid::Create(void* p1, void*, void*, void*)
{
	INT		i;
	FLOAT	fMax = 10000;

	
	for(i=0; i<8; ++i)
	{
		m_pLine[8*i +0 ] = CLcGeoGrid::VtxD(-128.f, 1.f,  16.f* (i+1), (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +1 ] = CLcGeoGrid::VtxD( 128.f, 1.f,  16.f* (i+1), (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +2 ] = CLcGeoGrid::VtxD(-128.f, 1.f, -16.f* (i+1), (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +3 ] = CLcGeoGrid::VtxD( 128.f, 1.f, -16.f* (i+1), (i%2)? 0xFF999999 : 0xFF666666);

		m_pLine[8*i +4 ] = CLcGeoGrid::VtxD( 16.f* (i+1), 1.f,-128.f, (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +5 ] = CLcGeoGrid::VtxD( 16.f* (i+1), 1.f, 128.f, (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +6 ] = CLcGeoGrid::VtxD(-16.f* (i+1), 1.f,-128.f, (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +7 ] = CLcGeoGrid::VtxD(-16.f* (i+1), 1.f, 128.f, (i%2)? 0xFF999999 : 0xFF666666);
	}

	return 0;
}



INT CLcGeoGrid::FrameMove()
{
	return 0;
}



void CLcGeoGrid::Render()
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
	pDev->SetFVF(CLcGeoGrid::VtxD::FVF);
	pDev->DrawPrimitiveUP(D3DPT_LINELIST, 32, m_pLine, sizeof(CLcGeoGrid::VtxD));

	pDev->SetRenderState( D3DRS_LIGHTING, dMnLgt);
	pDev->SetRenderState( D3DRS_FOGENABLE, dMnFog);
	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, dMnAblend);
	pDev->SetRenderState( D3DRS_ALPHATESTENABLE, dMnAtest);
}



INT CLcGeoGrid::Query(char*, void*)
{
	return -1;
}


