// Implementation of the CLcVtx classes.
//
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"
#include "LnIdx.h"


CLcIdx::CLcIdx()
{
	m_dFMT	= 0;
	m_nNum	= 0;
}

CLcIdx::~CLcIdx()
{
}

void CLcIdx::Destroy()
{
}

INT CLcIdx::Create(void*, void*, void*, void*)
{
	return 0;
}







CLcIdx16::CLcIdx16()
{
	m_dFMT	= D3DFMT_INDEX16;
}

CLcIdx16::~CLcIdx16()
{
	Destroy();
}

void CLcIdx16::Destroy()
{
	if(m_pIdx)
	{
		delete m_pIdx; 	m_pIdx = NULL;
	}
}

INT CLcIdx16::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_nNum			= (INT)p1;
	void*	pSrc	= p2;

	m_pIdx = new LcM::VtxIdx[m_nNum];

	if(pSrc)
		memcpy(m_pIdx, pSrc, m_nNum * sizeof(LcM::VtxIdx) );

	return 0;
}




CLcIdx32::CLcIdx32()
{
	m_dFMT	= D3DFMT_INDEX32;
}

CLcIdx32::~CLcIdx32()
{
	Destroy();
}

void CLcIdx32::Destroy()
{
	if(m_pIdx)
	{
		delete m_pIdx; 	m_pIdx = NULL;
	}
}

INT CLcIdx32::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_nNum			= (INT)p1;
	void*	pSrc	= p2;

	m_pIdx = new LcM::VtxIdx32[m_nNum];

	if(pSrc)
		memcpy(m_pIdx, pSrc, m_nNum * sizeof(LcM::VtxIdx32) );

	return 0;
}





INT	LcM3d_CreateIdx(char* sCmd		// Command
				, ILcIdx** pData	//
				, void* p1			// Need D3D Device
				, void* p2			// Total Indices
				, void* p3			// Index memory copy source
				, void* p4			// No Use
				)
{
	ILcIdx*	pObj = NULL;
	(*pData)= NULL;

	if(0==_stricmp("16", sCmd))
	{
		pObj = new CLcIdx16;
	}

	else if(0==_stricmp("32", sCmd))
	{
		pObj = new CLcIdx32;
	}

	*pData = pObj;
	return 0;
}


