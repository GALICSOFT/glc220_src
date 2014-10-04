// Implementation of the CLcMdl2D class.
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
#include "LnMdl2D.h"															// Model Object



CLcMdl2D::CLcMdl2D()
{
	memset(m_sName, 0, sizeof m_sName);
	memset(m_sTex, 0, sizeof m_sTex);
	m_pTex	= NULL;
}



CLcMdl2D::~CLcMdl2D()
{
	Destroy();
}



INT CLcMdl2D::Create(void* p1, void* p2, void* p3, void* p4)
{
	char*	sFile = (char*)p1;

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
							, 4
							, D3DXGetFVFVertexSize(VtxDUV1::FVF)
							, &vcMin
							, &vcMax);

	m_BndInf.Set(vcMin, vcMax);

	return 0;
}


void CLcMdl2D::Destroy()
{
	SAFE_DELETE(	m_pTex	);
}



INT CLcMdl2D::FrameMove()
{
	return 0;
}

void CLcMdl2D::Render()
{
	DWORD dMnLgt;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->GetRenderState( D3DRS_LIGHTING, &dMnLgt);

	pDev->SetRenderState( D3DRS_LIGHTING, FALSE);

	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDev->SetRenderState(D3DRS_ALPHAREF, 0x40);
	pDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	
	pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );


	D3DXMATRIX	mtWld;
	D3DXMATRIX	mtViw;
	D3DXMATRIX	mtViwI;

	pDev->GetTransform(D3DTS_VIEW, &mtViw);
	D3DXMatrixInverse(&mtViwI, NULL, &mtViw);

	mtViwI._41 = 0;
	mtViwI._42 = 0;
	mtViwI._43 = 0;

	mtWld = mtViwI * m_mtWld;
	pDev->SetTransform(D3DTS_WORLD, &mtWld);


	PDTX pTex = (PDTX)m_pTex->GetPointer();

	pDev->SetTexture(0, pTex);
	pDev->SetFVF(VtxDUV1::FVF);
	pDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_pVtx, sizeof(VtxDUV1));


	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
	pDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	pDev->SetRenderState( D3DRS_LIGHTING, dMnLgt);

	LcD3D_SetWorldIdentity(pDev);
}



INT CLcMdl2D::Query(char* sCmd, void* pVal)
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



INT CLcMdl2D::LoadTexture(char* sTexPath)
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


INT CLcMdl2D::LoadFromFile(char* sPath)
{
	char	sLine[180]={0};
	INT		i=0;

	FILE*	fp = NULL;
	// Text로 읽는다.
	fp= fopen(sPath, "rt");

	if(!fp)
		return -1;

	
	while(!feof(fp))
	{
		fgets(sLine, 180, fp);

		if(0==_strnicmp(sLine, "Texture*", strlen("Texture*")))
			sscanf(sLine, "%*s %s", m_sTex);

		else if(0==_strnicmp(sLine, "VtxLst {", strlen("VtxLst {")))
		{
			for(i=0;i<4; ++i)
			{
				fgets(sLine, 180, fp);
				
				INT	n;
				FLOAT X, Y, Z, U, V;
				DWORD	D;
				sscanf(sLine
						, "%d   %f %f %f     %f %f     %x"
						, &n,   &X, &Y, &Z,  &U, &V,  &D);

				// for OpenGL
				//V = 1.0 - V;

				m_pVtx[n].p = D3DXVECTOR3(X,Y,Z);
				m_pVtx[n].u = U;
				m_pVtx[n].v = V;
				m_pVtx[n].d = D;
			}

			break;
		}
	}

	fclose(fp);

	return 0;
}

