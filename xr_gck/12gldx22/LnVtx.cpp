// Implementation of the CLcVtx classes.
// PC Game Vertex
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"
#include "LnVtx.h"


CLcVtx::CLcVtx()
{
	m_dFVF		= 0;
	m_dStrd		= 0;
	m_nNum		= 0;

	m_pVB		= NULL;
	m_dPtType	= D3DPT_TRIANGLELIST;
	m_dPtNum	= 0;

	m_pTx[0]	= NULL;	m_pTx[4]	= NULL;
	m_pTx[1]	= NULL;	m_pTx[5]	= NULL;
	m_pTx[2]	= NULL;	m_pTx[6]	= NULL;
	m_pTx[3]	= NULL;	m_pTx[7]	= NULL;
}


CLcVtx::~CLcVtx()
{
	CLcVtx::Destroy();
}


void CLcVtx::Destroy()
{
	if(m_pVB)
	{
		free(m_pVB);
		m_pVB	= NULL;
	}
}


INT CLcVtx::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_nNum			= (INT)p1;
	void*	pVtxS	= p2;
	INT		iSize	= (INT)p3;

	m_pVB = malloc(m_nNum * m_dStrd);

	if(pVtxS)
	{
		memcpy(m_pVB, pVtxS, iSize);
	}

	return 0;
}


void CLcVtx::Render()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	switch(m_dFVF)
	{
		case LcM::VtxRHWD::FVF :
			pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
			pDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

			pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
			pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
			break;

		case LcM::VtxD::FVF :
			pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
			pDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

			pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
			pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
			break;

		case LcM::VtxDUV1::FVF :
			pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			pDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

			pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
			pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
			break;


		default:
			pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			pDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

			pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
			pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
			break;
	}

	for(int i=0; i<4; ++i)
	{
		if(m_pTx[i])
			pDev->SetTexture( i, m_pTx[i]);
	}

	pDev->SetFVF(m_dFVF);
	pDev->DrawPrimitiveUP( (D3DPRIMITIVETYPE)m_dPtType, m_dPtNum, m_pVB, m_dStrd);
}


void CLcVtx::DrawPrimitive(DWORD PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->SetFVF(m_dFVF);
	pDev->DrawPrimitiveUP( (D3DPRIMITIVETYPE)m_dPtType, m_dPtNum, m_pVB, m_dStrd);
}



void* CLcVtx::GetVertex()
{
	return m_pVB;
}


DWORD CLcVtx::GetFVF()
{
	return m_dFVF;
}


UINT CLcVtx::GetStrid()
{
	return m_dStrd;
}


INT CLcVtx::GetVtxNum()
{
	return m_nNum;
}



INT CLcVtx::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
	if(!m_pVB)
		return -1;

	if(ppbData)
		*ppbData = m_pVB;

	return 0;
}

INT CLcVtx::Unlock()
{
	if(!m_pVB)
		return -1;

	return 0;
}


void CLcVtx::SetTexture(INT nStage, void* pTx)
{
	m_pTx[nStage] = (LPDIRECT3DTEXTURE9)pTx;
}






void CLcVtx::SetFVF(DWORD dFVF)
{
	m_dFVF = dFVF;
}

void CLcVtx::SetStrdSize(UINT iStrd)
{
	m_dStrd = iStrd;
}


void CLcVtx::SetPtType(DWORD dPtType)
{
	m_dPtType = dPtType;
}

void CLcVtx::SetPtNum(UINT iPtNum)
{
	m_dPtNum = iPtNum;
}




void CLcVtx::SetPos(INT nIdx, FLOAT X, FLOAT Y, FLOAT Z, FLOAT W)
{
	if(!m_pVB)
		return;

	if(LcM::VtxD::FVF == m_dFVF)
	{
		LcM::VtxD* pVtx = (LcM::VtxD*)m_pVB;
		pVtx[nIdx].p[0] = X;
		pVtx[nIdx].p[1] = Y;
		pVtx[nIdx].p[2] = Z;
	}

	else if(LcM::VtxDUV1::FVF == m_dFVF)
	{
		LcM::VtxDUV1* pVtx = (LcM::VtxDUV1*)m_pVB;
		pVtx[nIdx].p[0] = X;
		pVtx[nIdx].p[1] = Y;
		pVtx[nIdx].p[2] = Z;
	}

	else if(LcM::VtxN::FVF == m_dFVF)
	{
		LcM::VtxN* pVtx = (LcM::VtxN*)m_pVB;
		pVtx[nIdx].p[0] = X;
		pVtx[nIdx].p[1] = Y;
		pVtx[nIdx].p[2] = Z;
	}
	
	else if(LcM::VtxUV1::FVF == m_dFVF)
	{
		LcM::VtxUV1* pVtx = (LcM::VtxUV1*)m_pVB;
		pVtx[nIdx].p[0] = X;
		pVtx[nIdx].p[1] = Y;
		pVtx[nIdx].p[2] = Z;
	}

	else if(LcM::VtxNUV1::FVF == m_dFVF)
	{
		LcM::VtxNUV1* pVtx = (LcM::VtxNUV1*)m_pVB;
		pVtx[nIdx].p[0] = X;
		pVtx[nIdx].p[1] = Y;
		pVtx[nIdx].p[2] = Z;
	}

	else if(LcM::VtxNDUV1::FVF == m_dFVF)
	{
		LcM::VtxNDUV1* pVtx = (LcM::VtxNDUV1*)m_pVB;
		pVtx[nIdx].p[0] = X;
		pVtx[nIdx].p[1] = Y;
		pVtx[nIdx].p[2] = Z;
	}


	else if(LcM::VtxNDUV3::FVF == m_dFVF)
	{
		LcM::VtxNDUV3* pVtx = (LcM::VtxNDUV3*)m_pVB;
		pVtx[nIdx].p[0] = X;
		pVtx[nIdx].p[1] = Y;
		pVtx[nIdx].p[2] = Z;
	}


	else if(LcM::VtxRHWD::FVF == m_dFVF)
	{
		LcM::VtxRHWD* pVtx = (LcM::VtxRHWD*)m_pVB;
		pVtx[nIdx].p[0] = X;
		pVtx[nIdx].p[1] = Y;
		pVtx[nIdx].p[2] = Z;
		pVtx[nIdx].p[3] = W;
	}
}

void CLcVtx::SetNormal(INT nIdx, FLOAT X, FLOAT Y, FLOAT Z)
{
	if(!m_pVB)
		return;

	if(LcM::VtxN::FVF == m_dFVF)
	{
		LcM::VtxN* pVtx = (LcM::VtxN*)m_pVB;
		pVtx[nIdx].n[0] = X;
		pVtx[nIdx].n[1] = Y;
		pVtx[nIdx].n[2] = Z;
	}
	
	else if(LcM::VtxNUV1::FVF == m_dFVF)
	{
		LcM::VtxNUV1* pVtx = (LcM::VtxNUV1*)m_pVB;
		pVtx[nIdx].n[0] = X;
		pVtx[nIdx].n[1] = Y;
		pVtx[nIdx].n[2] = Z;
	}

	else if(LcM::VtxNDUV1::FVF == m_dFVF)
	{
		LcM::VtxNDUV1* pVtx = (LcM::VtxNDUV1*)m_pVB;
		pVtx[nIdx].n[0] = X;
		pVtx[nIdx].n[1] = Y;
		pVtx[nIdx].n[2] = Z;
	}

	else if(LcM::VtxNDUV3::FVF == m_dFVF)
	{
		LcM::VtxNDUV3* pVtx = (LcM::VtxNDUV3*)m_pVB;
		pVtx[nIdx].n[0] = X;
		pVtx[nIdx].n[1] = Y;
		pVtx[nIdx].n[2] = Z;
	}
}


void CLcVtx::SetDiffuse(INT nIdx, FLOAT R, FLOAT G, FLOAT B, FLOAT A)
{
	DWORD dColor = D3DXCOLOR(R,G, B, A);

	if(!m_pVB)
		return;

	if(LcM::VtxD::FVF == m_dFVF)
	{
		LcM::VtxD* pVtx = (LcM::VtxD*)m_pVB;
		pVtx[nIdx].d = dColor;
	}

	else if(LcM::VtxDUV1::FVF == m_dFVF)
	{
		LcM::VtxDUV1* pVtx = (LcM::VtxDUV1*)m_pVB;
		pVtx[nIdx].d = dColor;
	}

	else if(LcM::VtxNDUV1::FVF == m_dFVF)
	{
		LcM::VtxNDUV1* pVtx = (LcM::VtxNDUV1*)m_pVB;
		pVtx[nIdx].d = dColor;
	}

	else if(LcM::VtxNDUV3::FVF == m_dFVF)
	{
		LcM::VtxNDUV3* pVtx = (LcM::VtxNDUV3*)m_pVB;
		pVtx[nIdx].d = dColor;
	}

	else if(LcM::VtxRHWD::FVF == m_dFVF)
	{
		LcM::VtxRHWD* pVtx = (LcM::VtxRHWD*)m_pVB;
		pVtx[nIdx].d = dColor;
	}
}


void CLcVtx::SetUV(INT nIdx, INT nUV, FLOAT U, FLOAT V)
{
	if(!m_pVB)
		return;

	if(LcM::VtxDUV1::FVF == m_dFVF)
	{
		LcM::VtxDUV1* pVtx = (LcM::VtxDUV1*)m_pVB;
		pVtx[nIdx].u = U;
		pVtx[nIdx].v = V;
	}

	else if(LcM::VtxNDUV1::FVF == m_dFVF)
	{
		LcM::VtxNDUV1* pVtx = (LcM::VtxNDUV1*)m_pVB;
		pVtx[nIdx].u = U;
		pVtx[nIdx].v = V;
	}

	else if(LcM::VtxNDUV3::FVF == m_dFVF)
	{
		LcM::VtxNDUV3* pVtx = (LcM::VtxNDUV3*)m_pVB;

		if(0==nUV)
		{
			pVtx[nIdx].u0 = U;
			pVtx[nIdx].v0 = V;
		}
		else if(1==nUV)
		{
			pVtx[nIdx].u1 = U;
			pVtx[nIdx].v1 = V;
		}
		else if(2==nUV)
		{
			pVtx[nIdx].u2 = U;
			pVtx[nIdx].v2 = V;
		}
	}

}



INT	LcM3d_CreateVtx_PC(
					ILcVtx** pData	//
					, void* p1		// Vertex Structure
					, void* p2		// Need D3D Device
					, void* p3		// Primitive Type
					, void* p4		// Primitive Count
					, void* p5		// Total Vertices
					, void* p6		// Vertex memory copy source.
					, void* p7		// Source Total Size
					, void* p8		// No Use
					, void* p9		// No Use
					)
{
	(*pData)= NULL;

	DWORD	dFVF	= LcM::VtxD::FVF;
	UINT	dStrd	= sizeof(LcM::VtxD);
	CLcVtx* pObj	= NULL;

	char*	sVtx	= (char*)p1;
	void*	pDev	= (void*)p2;
	char*	sPtType	= (char*)p3;
	INT		iPtNum	= (INT  )p4;

	pObj	= new CLcVtx;

	if(0==_stricmp("TRIANGLE LIST", sPtType))
		pObj->SetPtType(D3DPT_TRIANGLELIST);

	else if(0==_stricmp("TRIANGLE STRIP", sPtType))
		pObj->SetPtType(D3DPT_TRIANGLESTRIP);

	else if(0==_stricmp("POINT LIST", sPtType))
		pObj->SetPtType(D3DPT_POINTLIST);

	else if(0==_stricmp("TRIANGLE FAN", sPtType))
		pObj->SetPtType(D3DPT_TRIANGLEFAN);


	pObj->SetPtNum(iPtNum);


	if(0==_stricmp("Diffuse", sVtx))
	{
		dFVF	= LcM::VtxD::FVF;
		dStrd	= sizeof(LcM::VtxD);
	}

	else if(0==_stricmp("Diffuse Texture1", sVtx))
	{
		dFVF	= LcM::VtxDUV1::FVF;
		dStrd	= sizeof(LcM::VtxDUV1);
	}

	else if(0==_stricmp("Normal", sVtx))
	{
		dFVF	= LcM::VtxN::FVF;
		dStrd	= sizeof(LcM::VtxN);
	}
	
	else if(0==_stricmp("Texture1", sVtx))
	{
		dFVF	= LcM::VtxUV1::FVF;
		dStrd	= sizeof(LcM::VtxUV1);
	}

	else if(0==_stricmp("Normal Texture1", sVtx))
	{
		dFVF	= LcM::VtxNUV1::FVF;
		dStrd	= sizeof(LcM::VtxNUV1);
	}

	else if(0==_stricmp("Normal Diffuse Texture1", sVtx))
	{
		dFVF	= LcM::VtxNDUV1::FVF;
		dStrd	= sizeof(LcM::VtxNDUV1);
	}

	else if(0==_stricmp("Normal Diffuse Texture3", sVtx))
	{
		dFVF	= LcM::VtxNDUV3::FVF;
		dStrd	= sizeof(LcM::VtxNDUV3);
	}

	else if(0==_stricmp("RHW Diffuse", sVtx))
	{
		dFVF	= LcM::VtxRHWD::FVF;
		dStrd	= sizeof(LcM::VtxRHWD);
	}

	pObj->SetFVF(dFVF);
	pObj->SetStrdSize(dStrd);

	if(FAILED(pObj->Create(p2, p5, p6, p7)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;

	return 0;
}
