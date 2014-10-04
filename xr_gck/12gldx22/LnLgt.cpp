// Implementation of the CLcLgt classes.
//
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"
#include "LnLgt.h"


CLcLgt::CLcLgt()
{
	memset(&m_D3Lgt, 0, sizeof m_D3Lgt);
	m_nLgt	= 0;
}


CLcLgt::~CLcLgt()
{
	CLcLgt::Destroy();
}


void CLcLgt::Destroy()
{
}


INT CLcLgt::Create(void* p1, void*, void*, void*)
{
	return 0;
}


void CLcLgt::SetLight(INT nIdx)
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	m_nLgt	= nIdx;
	pDev->SetLight(m_nLgt, &m_D3Lgt);
}



void CLcLgt::SetLightEnable(BOOL nEnable)
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->LightEnable(m_nLgt, nEnable);
}




void CLcLgt::SetLgtType(DWORD dType)
{
	m_D3Lgt.Type = (D3DLIGHTTYPE)dType;
}


void CLcLgt::SetLgtAmbient(FLOAT* pVal/*D3DXCOLOR*/ )
{
	D3DXCOLOR Color(pVal);
	m_D3Lgt.Ambient = Color;
}


void CLcLgt::SetLgtDiffuse(FLOAT* pVal/*D3DXCOLOR*/ )
{
	D3DXCOLOR Color(pVal);
	m_D3Lgt.Diffuse = Color;
}


void CLcLgt::SetLgtSpecular(FLOAT* pVal/*D3DXCOLOR*/ )
{
	D3DXCOLOR Color(pVal);
	m_D3Lgt.Specular = Color;
}




void CLcLgt::SetLgtPosition(FLOAT* pVal/*D3DXVECTOR3*/ )
{
	D3DXVECTOR3 vcVal(pVal);
	m_D3Lgt.Position = vcVal;
}


void CLcLgt::SetLgtDirection(FLOAT* pVal/*D3DXVECTOR3*/ )
{
	D3DXVECTOR3 vcVal(pVal);
	m_D3Lgt.Direction = vcVal;
}


void CLcLgt::SetLgtRange		(FLOAT pVal)	{	m_D3Lgt.Range		 = pVal;	}
void CLcLgt::SetLgtFalloff		(FLOAT pVal)	{	m_D3Lgt.Falloff		 = pVal;	}

void CLcLgt::SetLgtAttenuation0	(FLOAT pVal)	{	m_D3Lgt.Attenuation0 = pVal;	}
void CLcLgt::SetLgtAttenuation1	(FLOAT pVal)	{	m_D3Lgt.Attenuation1 = pVal;	}
void CLcLgt::SetLgtAttenuation2	(FLOAT pVal)	{	m_D3Lgt.Attenuation2 = pVal;	}
void CLcLgt::SetLgtTheta		(FLOAT pVal)	{	m_D3Lgt.Theta		 = pVal;	}
void CLcLgt::SetLgtPhi			(FLOAT pVal)	{	m_D3Lgt.Phi			 = pVal;	}


DWORD	CLcLgt::GetLgtType()		{	return m_D3Lgt.Type;	}

FLOAT*	CLcLgt::GetLgtAmbient()		{	return (FLOAT*)&m_D3Lgt.Ambient.r;		}
FLOAT*	CLcLgt::GetLgtDiffuse()		{	return (FLOAT*)&m_D3Lgt.Diffuse.r;		}
FLOAT*	CLcLgt::GetLgtSpecular()	{	return (FLOAT*)&m_D3Lgt.Specular.r;		}

FLOAT*	CLcLgt::GetLgtPosition()	{	return (FLOAT*)&m_D3Lgt.Position.x;		}
FLOAT*	CLcLgt::GetLgtDirection()	{	return (FLOAT*)&m_D3Lgt.Direction.x;	}

FLOAT	CLcLgt::GetLgtRange()		{	return m_D3Lgt.Range;			}
FLOAT	CLcLgt::GetLgtFalloff()		{	return m_D3Lgt.Falloff;			}

FLOAT	CLcLgt::GetLgtAttenuation0(){	return m_D3Lgt.Attenuation0;	}
FLOAT	CLcLgt::GetLgtAttenuation1(){	return m_D3Lgt.Attenuation1;	}
FLOAT	CLcLgt::GetLgtAttenuation2(){	return m_D3Lgt.Attenuation2;	}
FLOAT	CLcLgt::GetLgtTheta()		{	return m_D3Lgt.Theta;			}
FLOAT	CLcLgt::GetLgtPhi()			{	return m_D3Lgt.Phi;				}



