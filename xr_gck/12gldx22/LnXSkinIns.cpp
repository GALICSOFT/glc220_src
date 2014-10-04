// Implementation of the CLcXSkinSrc class.
//
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"

#include "LnMdl.h"
#include "LnXSkin.h"

#include "LnXSkinAlloc.h"
#include "LnXSkinSrc.h"
#include "LnXSkinIns.h"


// Mesh Animation Instance 생성
// 원본 메쉬는 그대로 두고 애니메이션 컨트롤러만 새로 생성
//
////////////////////////////////////////////////////////////////////////////////



CLcXSkinIns::CLcXSkinIns()
{
	m_pAC		= NULL;
	m_pFrameOrg	= NULL;
}


CLcXSkinIns::~CLcXSkinIns()
{
}


void CLcXSkinIns::Destroy()
{
	if( m_pAC )
	{
		m_pAC->Release();
		m_pAC = NULL;
	}
}



INT CLcXSkinIns::Query(char* sCmd, void* pData)
{
	return CLcXSkin::Query(sCmd, pData);
}


INT CLcXSkinIns::Create(void* p1, void* p2, void* p3, void* p4)
{
	XTrack*	pInst	= NULL;

	m_pOrg	= (CLcXSkinSrc*)p1;
	pInst	=(XTrack*)p2;

	LPD3DXANIMATIONCONTROLLER	pAcOrg	= (LPD3DXANIMATIONCONTROLLER)m_pOrg->GetAniController();

	m_pFrameOrg					= (LPD3DXFRAME)m_pOrg->GetFrameRoot();

	HRESULT	hr=-1;
	DWORD i, dwTracks;
	LPD3DXANIMATIONCONTROLLER pNewAC = NULL;



	if(pAcOrg)
	{
		if(m_pAC)
			m_pAC->Release();

		UINT MaxNumAnimationOutputs	= pAcOrg->GetMaxNumAnimationOutputs();
		UINT MaxNumAnimationSets	= pAcOrg->GetMaxNumAnimationSets();
		UINT MaxNumTracks			= pAcOrg->GetMaxNumTracks();
		UINT MaxNumEvents			= pAcOrg->GetMaxNumEvents();

		hr = pAcOrg->CloneAnimationController(MaxNumAnimationOutputs
			,	MaxNumAnimationSets
			,	MaxNumTracks
			,	MaxNumEvents
			,	&pNewAC);

		if(FAILED(hr))
			return -1;

		m_pAC = pNewAC;

		//	LPD3DXTRACK_DESC;
		//LPD3DXANIMATIONSET pAS= NULL;


		FLOAT	fTrackPosition	= pInst->fTrackPosition;
		FLOAT	fTrackSpeed		= pInst->fTrackSpeed;

		// Start with all tracks disabled
		dwTracks = m_pAC->GetMaxNumTracks();

		for( i = 0; i < dwTracks; ++ i )
		{
			m_pAC->SetTrackEnable( i, TRUE );
			m_pAC->SetTrackWeight( i, 1.0f );
			m_pAC->SetTrackSpeed( i, 1);
			m_pAC->SetTrackPosition(i, fTrackPosition);
		}

	}

	return 0;
}



INT CLcXSkinIns::FrameMove()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	m_dTimeCur += m_fElapse;

	// For Sort...(optional)
	D3DXMATRIX	mtViwI;

	pDev->GetTransform( D3DTS_VIEW,  &mtViwI );
	D3DXMatrixInverse(&mtViwI, NULL, &mtViwI);
	
	D3DXVECTOR3	vcCam	= D3DXVECTOR3(mtViwI._41, mtViwI._42, mtViwI._43);
	D3DXVECTOR3	vcZ		= D3DXVECTOR3(mtViwI._31, mtViwI._32, mtViwI._33);
	D3DXVECTOR3	vcTmp	= m_vcTrn - vcCam;

	m_fStlSrtR = D3DXVec3Dot(&vcZ, &vcTmp);

	return 0;
}


void CLcXSkinIns::Render()
{
	HRESULT hr=-1;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();


	CLcXSkinSrc*	pOrg = (CLcXSkinSrc*)m_pOrg;

	ID3DXEffect*	pEft	= pOrg->GetEffect();

	// Setup the projection matrix
	D3DXMATRIX matProj;
	pDev->GetTransform(D3DTS_PROJECTION, &matProj);




	D3DLIGHT9 light;
	D3DXVECTOR3 vecLightDirUnnormalized(0.0f, -1.0f, 1.0f);
	ZeroMemory( &light, sizeof(D3DLIGHT9) );
	light.Type        = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r   = 1.0f;
	light.Diffuse.g   = 1.0f;
	light.Diffuse.b   = 1.0f;
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecLightDirUnnormalized );
	light.Position.x   = 0.0f;
	light.Position.y   = -1.0f;
	light.Position.z   = 1.0f;
	light.Range        = 1000.0f;

	pDev->SetLight(0, &light );
	pDev->LightEnable(0, TRUE );
	
	
	// Set Light for vertex shader
	D3DXVECTOR4 vLightDir( 0.0f, 1.0f, -1.0f, 0.0f );
	D3DXVec4Normalize( &vLightDir, &vLightDir );
	
	
	// for HLSL
	{
		pEft->SetMatrix( "mViewProj", &matProj);
		pEft->SetVector( "lhtDir", &vLightDir);
	}
	
	
	
	// for shader
	{
		// set the projection matrix for the vertex shader based skinning method
		D3DXMatrixTranspose(&matProj, &matProj);
		pDev->SetVertexShaderConstantF(2, (float*)&matProj, 4);
		pDev->SetVertexShaderConstantF(1, (float*)&vLightDir, 1);
	}

	if(m_pAC)
		m_pAC->AdvanceTime(m_fElapse, NULL);

	pOrg->UpdateFrameMatrices(m_pFrameOrg, &m_mtWld);	
	pOrg->DrawFrame(m_pFrameOrg);

	static	D3DXMATRIX	mtI(1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1);
	pDev->SetTransform(D3DTS_WORLD, &mtI);
}



