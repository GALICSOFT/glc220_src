// Implementation of the CLcMdlX class.
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
#include "LnMdlX.h"


CLcMdlX::LcMdlX::LcMdlX()
{
	nFce	= 0;
	nVtx	= 0;
	memset(sTex, 0, sizeof sTex);

	pVtx		= NULL;
	pIdx		= NULL;
	pTex		= NULL;
}

CLcMdlX::LcMdlX::~LcMdlX()
{
	SAFE_DELETE_ARRAY(		pVtx	);
	SAFE_DELETE_ARRAY(		pIdx	);
	SAFE_DELETE(		pTex	);
}


CLcMdlX::CLcMdlX()
{
	m_fmFc	= 0;
	m_fmVx	= 0;

	m_stFc	= 0;
	m_stVx	= 0;

	m_nFce	= 0;
	m_nVtx	= 0;

	m_nGeo	= 0;
	m_pGeo	= NULL;
}

CLcMdlX::~CLcMdlX()
{
	Destroy();
}

void CLcMdlX::Destroy()
{
	SAFE_DELETE_ARRAY(	m_pGeo	);

	m_fmFc	= 0;
	m_fmVx	= 0;

	m_stFc	= 0;
	m_stVx	= 0;

	m_nFce	= 0;
	m_nVtx	= 0;

	m_nGeo	= 0;
}


INT CLcMdlX::Create(void* p1, void* p2, void* p3, void* p4)
{
	INT		hr=-1;
	char*	sFile;

	sFile	= (char*)p1;

	if(!strlen(sFile))
		return -1;


	hr  = LoadFromFile(sFile);

	if(FAILED(hr))
		return -1;

	// 입력 받은 파일 이름으로 이름을 설정
	char fname[LCM_FILE_NAME]={0};
	char fext [ 16]={0};
	_splitpath(sFile, NULL, NULL, fname, fext);
	sprintf(m_sName, "%s%s", fname, fext);

	return 0;
}


INT CLcMdlX::FrameMove()
{
	return 0;
}


void CLcMdlX::Render()
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


	pDev->SetFVF(m_fmVx);
	pDev->SetTransform(D3DTS_WORLD, &m_mtWld);

	for(int i = 0; i < m_nGeo; ++i)
	{
		PDTX pTex = NULL;

		if(m_pGeo[i].pTex)
			pTex = (PDTX)m_pGeo[i].pTex->GetPointer();

		pDev->SetTexture(0, pTex);

		pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST
										, 0
										, m_pGeo[i].nVtx
										, m_pGeo[i].nFce
										, m_pGeo[i].pIdx
										, D3DFMT_INDEX16
										, m_pGeo[i].pVtx
										, m_stVx
										);
	}

	pDev->SetTexture(0, NULL);

	LcD3D_SetWorldIdentity(pDev);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	pDev->SetRenderState( D3DRS_LIGHTING, dMnLgt);
}



INT CLcMdlX::Query(char* sCmd, void* pVal)
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

INT CLcMdlX::SetAttrib(char* sCmd, void* pVal)
{
	return CLcMdl::SetAttrib(sCmd, pVal);
}

INT CLcMdlX::GetAttrib(char* sCmd, void* pVal)
{
	if(0==_stricmp("Number Geometry", sCmd))
	{
		*((INT*)pVal)	= m_nGeo;
		return 0;
	}

	else if(0==_stricmp("Geometry Vertex", sCmd))
	{
		struct _T{ INT n; INT v; void* p; } *t = (_T*)pVal;
		
		t->v = m_pGeo[t->n].nVtx;
		t->p = m_pGeo[t->n].pVtx;

		return 0;
	}
	
	else if(0==_stricmp("Get FVF", sCmd))
	{
		*((DWORD*)pVal)	= 0;

		if(!m_pGeo)
			return -1;

		*((DWORD*)pVal)	= m_stVx;

		return 0;
	}

	return CLcMdl::GetAttrib(sCmd, pVal);
}


INT CLcMdlX::LoadTexture(char* sPath)
{
	if( m_nGeo <=0)
		return -1;

	int iLen = strlen(sPath);
	if(iLen<4)
		return -1;


	DWORD	color		= 0x00FFFFFF;
	DWORD	Filter		= 0;

	char	sFile[MAX_PATH]={0};
	char	sTex[MAX_PATH];

	strcpy(sFile, sPath);


	if( '/' == sFile[iLen-1])
		sFile[iLen-1] = 0;



	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();


	for(int n=0; n<m_nGeo; ++n)
	{
		LcMdlX*		pGeo = &m_pGeo[n];

		if(strlen(pGeo->sTex)<3)
			continue;


		sprintf(sTex, "%s/%s", sFile, pGeo->sTex);

		//File Load
		PLC_TEXTURE		pTex = NULL;
		if(FAILED(LcDev_CreateTex("File", &pTex, sTex, 0x00FFFFFF, (D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR))))
			return -1;

		pGeo->pTex = pTex;		
	}

	return 0;
}



void* CLcMdlX::GetVertexBuffer()
{
	INT	n=0;
	INT	nVtx = 0;
	void*	pVtx = NULL;
	char*	pVtxS = NULL;
	char*	pVtxD = NULL;


	for(n=0; n<m_nGeo; ++n)
		nVtx += m_pGeo[n].nVtx;
	
	pVtx = malloc(m_stVx * nVtx);

	INT	nPos=0;
	INT iSize=0;
		
	for(n=0; n<m_nGeo; ++n)
	{
		pVtxS = (char*)m_pGeo[n].pVtx;
		iSize = m_pGeo[n].nVtx * m_stVx;

		memcpy(pVtxD + nPos, pVtxS, iSize);		// Vertex Copy

		nPos += iSize;
	}		

	return pVtx;
}


INT CLcMdlX::LoadFromFile(char* sPath)
{
	FILE*	fp = NULL;

	fp= fopen(sPath, "rb");

	if(!fp)
		return -1;

	INT		n = 0;
	char*	pIdxS = NULL;
	char*	pVtxS = NULL;

	char	sType[16]={0};
	char	sVer [16]={0};

	fread(sType, 1, 16, fp);
	fread(sVer,  1, 16, fp);

	fread(&m_fmFc, sizeof(DWORD), 1, fp);
	fread(&m_fmVx, sizeof(DWORD), 1, fp);

	fread(&m_stFc, sizeof(DWORD), 1, fp);
	fread(&m_stVx, sizeof(DWORD), 1, fp);

	fread(&m_nFce, sizeof(DWORD), 1, fp);
	fread(&m_nVtx, sizeof(DWORD), 1, fp);
	fread(&m_nGeo, sizeof(INT  ), 1, fp);
	
	fseek(fp, LCM_HEADER_SIZE, SEEK_SET);

	
	m_pGeo	= new CLcMdlX::LcMdlX[m_nGeo];

	// Reading Vertex, Face, and Material Info
	for(n=0; n<m_nGeo; ++n)
	{
		fread(m_pGeo[n].sTex, sizeof(char), LCM_TEX_NAME, fp);			// Texture FileName

		fread(&m_pGeo[n].nFce, sizeof(DWORD), 1, fp);					// face number
		fread(&m_pGeo[n].nVtx, sizeof(DWORD), 1, fp);					// vertex numbers
	}
	
	// Reading Vertex, and Index Buffer
	for(n=0; n<m_nGeo; ++n)
	{
		if(m_pGeo[n].nFce>0 && m_pGeo[n].nVtx>0)
		{
			m_pGeo[n].pIdx = new VtxIdx[m_pGeo[n].nFce];
			m_pGeo[n].pVtx = new VtxUV1[m_pGeo[n].nVtx];

			fread(m_pGeo[n].pIdx, m_stFc, m_pGeo[n].nFce, fp);			// read faces
			fread(m_pGeo[n].pVtx, m_stVx, m_pGeo[n].nVtx, fp);			// read vertices
		}
	}

	fclose(fp);




	// For D3DX
	for(n=0; n<m_nGeo; ++n)
	{
		LcMdlX*	pGeo = &m_pGeo[n];
		VtxUV1*	pVtx = pGeo->pVtx;

		if(pVtx)
		{
			for(int j=0; j<pGeo->nVtx; ++j)
			{
				pVtx[j].v = 1.f - pVtx[j].v;
			}
		}
	}

	return 0;
}


