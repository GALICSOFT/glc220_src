// Implementation of the CLcMdlSM class.
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
#include "LnMdlSM.h"															// Model Object


CLcMdlSM::CLcMdlSM()
{
	memset(m_sName, 0, sizeof(m_sName));

	m_nIdx	= 0	;
	m_nVtx	= 0	;

	m_pIdx	= NULL	;
	m_pVtx	= NULL	;

	memset(m_sTex, 0, sizeof(m_sTex));
	m_pTex	= NULL	;
}



CLcMdlSM::~CLcMdlSM()
{
	Destroy();
}



INT CLcMdlSM::Create(void* p1, void* p2, void* p3, void* p4)
{
	char*	sFile= (char*)p1;

	if(FAILED(LoadFromFile(sFile)))
		return -1;

	char fname[128]={0};
	char fext [ 32]={0};
	LcStr_SplitPath(sFile, NULL, NULL, fname, fext);
	sprintf(m_sName, "%s%s", fname, fext);


	D3DXVECTOR3 vcMin;
	D3DXVECTOR3 vcMax;

	// Set Max and Min vertex Position
	D3DXComputeBoundingBox(	(D3DXVECTOR3*)m_pVtx
							, m_nVtx
							, D3DXGetFVFVertexSize(VtxNDUV1::FVF)
							, &vcMin
							, &vcMax);

	m_BndInf.Set(vcMin, vcMax);
	
	return 0;
}


void CLcMdlSM::Destroy()
{
	SAFE_DELETE(	m_pIdx	);
	SAFE_DELETE(	m_pVtx	);
	SAFE_DELETE(	m_pTex	);
}


INT CLcMdlSM::FrameMove()
{
	return 0;
}


void CLcMdlSM::Render()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	pDev->SetTransform(D3DTS_WORLD, &m_mtWld);

	RenderMesh();
}



void CLcMdlSM::RenderMesh()
{
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


	PDTX	pDXTex	= (PDTX)m_pTex->GetPointer();

	pDev->SetTexture(0, pDXTex);
	pDev->SetFVF(VtxNDUV1::FVF);
	pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0
									, m_nVtx
									, m_nIdx
									, m_pIdx
									, D3DFMT_INDEX16
									, m_pVtx
									, sizeof(VtxNDUV1)
									);

	LcD3D_SetWorldIdentity(pDev);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	pDev->SetRenderState( D3DRS_LIGHTING, dMnLgt);
}


INT CLcMdlSM::Query(char* sCmd, void* pVal)
{
	if(0==_stricmp("Load Texture", sCmd))
	{
		char*	sFile=(char*)pVal;

		if(FAILED(LoadTexture(sFile)))
			return -1;

		return 0;
	}
	
	return CLcMdl::Query(sCmd, pVal);
}



INT CLcMdlSM::LoadTexture(char* sTexPath)
{
	int iLen = strlen(sTexPath);
	if(iLen<4)
		return -1;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	char	sPath[MAX_PATH]={0};
	char	sTex[MAX_PATH];

	strcpy(sPath, sTexPath);

	if( '/' == sPath[iLen-1])
		sPath[iLen-1] = 0;
	
	sprintf(sTex, "%s/%s", sPath, m_sTex);

	if(FAILED(LcDev_CreateTex("File", &m_pTex, sTex, 0x00FFFFFF, (D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR))))
		return -1;

	return 0;
}



INT CLcMdlSM::LoadFromFile(char* sPath)
{
	FILE*	fp = NULL;
	INT		i;
	char	sLine[180]={0};

	// Text로 읽는다.
	fp= fopen(sPath, "rt");

	if(!fp)
		return -1;

	while(!feof(fp))
	{
		fgets(sLine, 180, fp);

		if(0==_strnicmp(sLine, "NumFce*", strlen("NumFce*")))
		{
			sscanf(sLine, "%*s %d", &m_nIdx);
			m_pIdx = new VtxIdx[m_nIdx];
		}

		else if(0==_strnicmp(sLine, "NumVtx*", strlen("NumVtx*")))
		{
			sscanf(sLine, "%*s %d", &m_nVtx);
			m_pVtx = new VtxNDUV1[m_nVtx];
		}

		else if(0==_strnicmp(sLine, "Texture*", strlen("Texture*")))
			sscanf(sLine, "%*s %s", m_sTex);

		else if(0==_strnicmp(sLine, "FceLst {", strlen("FceLst {")))
		{
			for(i=0;i<m_nIdx; ++i)
			{
				fgets(sLine, 180, fp);

				INT n, a, b, c;
				sscanf(sLine, "%d	%d %d %d", &n, &a, &b, &c);

				m_pIdx[n] = VtxIdx(a, b, c);
			}
		}

		else if(0==_strnicmp(sLine, "VtxLst {", strlen("VtxLst {")))
		{
			for(i=0;i<m_nVtx; ++i)
			{
				fgets(sLine, 180, fp);
				
				INT	n;
				FLOAT X, Y, Z, nX, nY, nZ, U, V;
				DWORD	D;
				sscanf(sLine
						, "%d   %f %f %f     %f %f %f  %f %f         %x"
						, &n,   &X, &Y, &Z,  &nX, &nY, &nZ, &U, &V,  &D);

				// for OpenGL
				//V = 1.0 - V;

				m_pVtx[n].p = D3DXVECTOR3(X,Y,Z);
				m_pVtx[n].n = D3DXVECTOR3(nX, nY, nZ);
				m_pVtx[n].u = U;
				m_pVtx[n].v = V;
				m_pVtx[n].d = D;
			}
		}
	}

	fclose(fp);

	return 0;
}



