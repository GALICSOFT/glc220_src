// Implementation of the CLcXSkin class.
//
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <stdio.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "LnxEntity.h"

#include "LnMdl.h"
#include "LnXSkin.h"


TexLayer::TexLayer()
{
	for(int i=0; i<TexLayer::MAX_TEXTURE_LAYER; ++i)
	{
		p[i] = NULL;
	}
}

CLcXSkin::CLcXSkin()
{
	m_fElapse	= 0;
	m_dTimeCur	= 0;

	m_vcRot		= VEC4(0,0,0, 1);
	m_vcScl		= VEC3(1,1,1);
	m_vcTrn		= VEC3(0,0,0);

	D3DXMatrixIdentity(&m_mtRot);
	D3DXMatrixIdentity(&m_mtWld);
}


CLcXSkin::~CLcXSkin()
{
	Destroy();
}


INT CLcXSkin::Query(char* sCmd, void* pVal)
{
	if(0==_stricmp("Set Elapsed Time", sCmd))
	{
		DOUBLE t = *((DOUBLE*)pVal);
		m_fElapse = t;
		return 0;
	}

	else if(0==_stricmp("Set World Matrix", sCmd))
	{
		MATA* p = (MATA*)pVal;
		m_mtWld = *p;
		return 0;
	}

	return -1;
}


INT CLcXSkin::SetAttrib(char* sCmd, void* pVal)
{
	return -1;
}

INT CLcXSkin::GetAttrib(char* sCmd, void* pVal)
{
	return -1;
}


