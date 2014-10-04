// RndObj.cpp: implementation of the CRndObj class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"


CRndObj::CRndObj()
{
	m_pDev	= NULL;
	m_pTex	= NULL;

	m_nFce	= 0;
	m_pIdx	= NULL;
	m_pVtx	= NULL;
}

CRndObj::~CRndObj()
{
	Destroy();
}

void CRndObj::Destroy()
{
	SAFE_RELEASE(	m_pTex	);
}


INT CRndObj::Create(void* p1, void* p2, void*, void*)
{
	INT		hr = 0;
	char*	sFile=NULL;

	if(!p1)
		return LC_EFAIL;

	m_pDev	= (PLC_DEVICE)p1;
	sFile	= (char*)p2;

	hr = LcDev_CreateTextureFromFile(NULL, &m_pTex, sFile);

	if(LC_FAILED(hr))
		return LC_EFAIL;


	return LC_OK;
}

INT CRndObj::FrameMove()
{
	return LC_OK;
}

void CRndObj::Render()
{
	m_pDev->SetTexture(0, m_pTex);
	m_pDev->SetTransform(LC_TS_WORLD, &m_mtWld);
	m_pDev->SetFVF(LC3D::VxfUV1::FVF);
	m_pDev->DrawIndexedPrimitiveUP(LC_PT_TRIANGLE_LIST, m_nFce* 3, m_pIdx, m_pVtx, sizeof(LC3D::VxfUV1));

	m_pDev->SetTransform(LC_TS_WORLD_IDENTITY, NULL);
	m_pDev->SetTexture(0, 0);
}





void CRndObj::SetMatrix(const LCXMATRIX* v)
{
	m_mtWld = *v;
}


void CRndObj::SetIdx(const void* v, INT nFace)
{
	m_nFce = nFace;
	m_pIdx = (LC3D::VtxIdxW*)v;
}

void CRndObj::SetVtx(const void* v)
{
	m_pVtx = (LC3D::VxfUV1*)v;
}



