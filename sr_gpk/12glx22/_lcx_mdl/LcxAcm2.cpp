// Implementation of the CLcxAcm class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
#else
  #include "../LcxEntity.h"
#endif

#include "LcxAcm.h"



INT CLcxAcm::Load()
{
	INT		n=0;
	INT		j=0;
	INT		k=0;

	FILE*	fp= NULL;

	char	sMedia[LC_MAX_PATH2]={0};
	if(LC_FAILED(LcSys_ResourcePath(sMedia, m_sFile)))
		return LC_EFAIL;


	fp = fopen(sMedia, "rb");
	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;




	fread(&m_Info, sizeof(CLcxAcm::TInfo), 1, fp);
	fseek(fp, LC_RSC_HEADER, SEEK_SET);


	if(m_Info.nGeo<1)
	{
		fclose(fp);
		return LC_EFAIL;
	}



	m_pGeo = new Tgeo[m_Info.nGeo];


	// Read Mesh
	for(n =0; n<m_Info.nGeo; ++n)
	{
		Tgeo* pGeo = &m_pGeo[n];

		fread(pGeo->sName , sizeof(char),32, fp);	// Node Name
		fread(&pGeo->nType, sizeof(INT ), 1, fp);	// Node Type
		fread(&pGeo->nPrn , sizeof(INT ), 1, fp);	// Parent Index

		fread(&pGeo->nFce , sizeof(INT ), 1, fp);	// Index Number
		fread(&pGeo->nVtx , sizeof(INT ), 1, fp);	// Vertex Number
		fread(&pGeo->nAni , sizeof(INT ), 1, fp);	// Animation Number
		fread(&pGeo->nMtl , sizeof(INT ), 1, fp);	// Materila File Index



		// Indicate Parent Node
		if(-1 != pGeo->nPrn)
			pGeo->pPrn = &m_pGeo[pGeo->nPrn];

		if(1 > pGeo->nFce || 1 > pGeo->nVtx)
			continue;


		if(LCX_BONE == pGeo->nType && LCX_MESH == pGeo->nType)
		{
			pGeo->nFce = 0;
			pGeo->nVtx = 0;
			continue;
		}


		// Create Rendering Mesh Buffer
		pGeo->pFce = new Tidx[pGeo->nFce];		// Face List

		pGeo->pVsP = new VEC3[pGeo->nVtx];		// Vertex Source Position
		pGeo->pVdP = new VEC3[pGeo->nVtx];		// Vertex Dest Position

		if(LCX_MESH == pGeo->nType)
		{
			pGeo->pVsW = new VEC4[pGeo->nVtx];		// BLEND WEIGHT
			pGeo->pVsB = new INT4[pGeo->nVtx];		// Blend MATRIX Index
			pGeo->pVsN = new VEC3[pGeo->nVtx];		// Vertex Source Normal vector
			pGeo->pVsT = new VEC2[pGeo->nVtx];		// Vertex Source Texture Coordinate
		}


		// Read Face and Position
		fread(pGeo->pFce, sizeof(Tidx), pGeo->nFce, fp);
		fread(pGeo->pVsP, sizeof(VEC3), pGeo->nVtx, fp);
	}


	// Reading Animation
	for(n =0; n<m_Info.nGeo; ++n)
	{
		Tgeo* pGeo = &m_pGeo[n];

		if(1>pGeo->nAni)
			continue;

		pGeo->vAni = new CLcxAcm::Ttrack[pGeo->nAni];
		fread(pGeo->vAni, sizeof(CLcxAcm::Ttrack), pGeo->nAni, fp);	// Animation Matrix

		//int c;
		//c=0;
	}



	// Reading Vertex Weight Data
	for(n=0; n<m_Info.nGeo; ++n)
	{
		Tgeo*	pGeo	= &m_pGeo[n];

		fread(&pGeo->nBnd, sizeof(INT ), 1, fp);

		if(0>=pGeo->nBnd)
			continue;

		pGeo->pBnd = new LcmBone[pGeo->nVtx];

		for(j=0; j<pGeo->nVtx; ++j)
		{
			LcmBone*	pBlnd = &pGeo->pBnd[j];
			INT			iBone = 0;

			fread(&iBone, sizeof(INT ), 1, fp);

			for(k=0; k<iBone; ++k)
			{
				INT		N = -1;
				FLOAT	W = 0.f;
				fread(&N, sizeof(INT  ), 1, fp);
				fread(&W, sizeof(FLOAT), 1, fp);

				pBlnd->vB.push_back(Tbone(N, W));
			}
		}
	}



	// Reading Normal
	for(n =0; n<m_Info.nGeo; ++n)
	{
		Tgeo* pGeo = &m_pGeo[n];

		if(LCX_MESH	!= pGeo->nType)
			continue;

		fread(pGeo->pVsN, sizeof(VEC3), pGeo->nVtx, fp);
	}



	// read Texture File Index
	if(0<m_Info.nMtl)
	{
		m_pMtl	= new Tmtl[m_Info.nMtl];
		for(n=0; n< m_Info.nMtl; ++n)
			fread(m_pMtl[n].sTex, sizeof(char), LCM_TEX_NAME, fp);
	}

	// Reading Texture Coordinate
	for(n =0; n<m_Info.nGeo; ++n)
	{
		Tgeo* pGeo = &m_pGeo[n];

		if(LCX_MESH	!= pGeo->nType)
			continue;

		fread(pGeo->pVsT, sizeof(VEC2), pGeo->nVtx, fp);
	}

	fclose(fp);


	// Copy Vertex Weight data
	for(n=0; n<m_Info.nGeo; ++n)
	{
		Tgeo* pGeo = &m_pGeo[n];

		if(0>=pGeo->nBnd)
			continue;

		for(j=0; j<pGeo->nVtx; ++j)
		{
			// Copy Blend Data
			LcmBone*	pBlnd= &pGeo->pBnd[j];
			INT			iBone= pBlnd->vB.size();
			FLOAT*		pVsW = NULL;
			INT  *		pVsB = NULL;

			FLOAT		g[8]={0};		// BLEND WEIGHT
			INT			m[8]={0};		// MATRIX Index

			for(k=0; k<iBone; ++k)
			{
				g[k] = pBlnd->vB[k].fW;
				m[k] = pBlnd->vB[k].nB;
			}

			g[3] = fabsf(1.0F - g[0] - g[1] - g[2]);


			pVsW = (FLOAT*)&pGeo->pVsW[j];
			pVsB = (INT  *)&pGeo->pVsB[j];

			pVsW[0] = g[0];
			pVsW[1] = g[1];
			pVsW[2] = g[2];
			pVsW[3] = g[3];

			pVsB[0] = m[0];
			pVsB[1] = m[1];
			pVsB[2] = m[2];
			pVsB[3] = m[3];
		}
	}


	// Delete Vertex Weight Data
	for(n=0; n<m_Info.nGeo; ++n)
	{
		Tgeo*	pGeo	= &m_pGeo[n];

		if(0>=pGeo->nBnd)
			continue;

		SAFE_DELETE_ARRAY(	pGeo->pBnd	);
	}


	return LC_OK;
}



INT CLcxAcm::LoadTexture(LC_HANDLE pOption)
{
	// Setup Texture

	for(INT i=0; i<m_Info.nMtl; ++i)
	{
		Tmtl*	pMtl = &m_pMtl[i];

		if(strlen(pMtl->sTex) <3)
			continue;

		char sFile	[LC_MAX_PATH ]={0};
		char drive	[LC_MAX_DRIVE]={0};
		char dir	[LC_MAX_DIR  ]={0};
		char fname	[LC_MAX_FNAME]={0};
		char ext	[LC_MAX_EXT  ]={0};

		LcStr_SplitPath( m_sFile, drive, dir, fname, ext );
		sprintf(sFile, "%s%s%s", drive,dir, pMtl->sTex);


		PLC_TEXTURE	pTex = NULL;
		if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &pTex, sFile)))
		{
			printf("Cannot Read Model Texture: %s\n", sFile);
			return LC_CANNT_FIND_RESOURCE;
		}

		pMtl->pTex = pTex;
	}

	return LC_OK;
}


