// Implementation of the CLcMdlSMins class.
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
#include "LnMdlSM.h"
#include "LnMdlSMins.h"


CLcMdlSMins::CLcMdlSMins()
{
	m_pOrg	= NULL;

	m_pPack	= NULL;
	m_vcEps	= VEC3(0,0,0);
}



CLcMdlSMins::~CLcMdlSMins()
{
	Destroy();
}



INT CLcMdlSMins::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_pOrg = (CLcMdlSM*)( ((CLcMdl*)p2)->GetOriginal() );


	FLOAT	fRotY = rand()%360;
	FLOAT	fRotZ = -7 + rand()%15;
	FLOAT	fRotX = -7 + rand()%15;

	fRotY = 0;
	fRotZ = 0;
	fRotX = 0;


	SetRotation( (FLOAT*)&VEC3(fRotX, fRotY, fRotZ));

	// Bound Box Setup
	TbndAB* pBnd  = (TbndAB*)m_pOrg->GetBndInf();
	if(pBnd)
		m_BndInf.Set( *pBnd->GetMin(), *pBnd->GetMax());
	
	return 0;
}


void CLcMdlSMins::Destroy()
{
}


INT CLcMdlSMins::FrameMove()
{
	SetupSortValue();

	//Transform Bound Box
	TbndAB*	pBndSrc = (TbndAB*)m_pOrg->GetBndInf();
	TbndAB* pBndDst = &m_BndInf;
	Lcm_BoundBoxTransform(pBndDst, pBndSrc, &m_mtWld, &m_vcEps);

	return 0;
}


void CLcMdlSMins::Render()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->SetTransform(D3DTS_WORLD, &m_mtWld);
	m_pOrg->RenderMesh();
}



INT CLcMdlSMins::Query(char* sCmd, void* pVal)
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


