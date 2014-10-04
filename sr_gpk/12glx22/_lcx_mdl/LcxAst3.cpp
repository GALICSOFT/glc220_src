// Implementation of the CLcxAst class.
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

#include "LcxAse.h"
#include "LcxAst.h"


// 1. save Header
//    version
//    Scene Info

// 2. Save Material

// 3. Save Geometry

// 4. Animation Set


void CLcxAst::SaveToAsmBin()
{
	FILE*	fp=NULL;
	INT		n=0;


	char drive[LC_MAX_DRIVE]={0};
	char dir  [LC_MAX_DIR  ]={0};
	char fname[LC_MAX_FNAME]={0};
	char ext  [LC_MAX_EXT  ]=".asb";
	char sFile[LC_MAX_PATH ]={0};

	LcStr_SplitPath( m_sFile, drive, dir, fname, NULL );



	sprintf(sFile, "%s%s%s%s", drive, dir, fname, ext);

	fp = fopen(sFile, "wb");




	//1. save Header
	fwrite(&m_Info, sizeof(LCX_Ase::TInfo), 1, fp);	// Info
	fseek(fp, LC_RSC_HEADER, SEEK_SET);			// chunck 만큼 이동


	//2. Writing Material
	for(n=0; n<m_Info.nMtl; ++n)
	{
		LCX_Ase::Tmtl* pMtl = &m_pMtl[n];

		fwrite(pMtl->sTex, 1, LCXM_TEXNAME, fp);
	}


	//3. Writing Geometry Info
	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::Tgeo* pDst = &m_pGeo[n];

		fwrite(pDst->sName , sizeof(char ), 32, fp);		// Current Node Name
		fwrite(&pDst->nPrn , sizeof(INT  ),  1, fp);		// Parent Node Index
		fwrite(&pDst->nMtl , sizeof(INT  ),  1, fp);		// Material Index

		fwrite(&pDst->iFce , sizeof(INT  ),  1, fp);		// Number of Index
		fwrite(&pDst->iNvx , sizeof(INT  ),  1, fp);		// Number of Vertex
		fwrite(&pDst->dFVF , sizeof(UINT ),  1, fp);		// FVF
		fwrite(&pDst->dVtx , sizeof(UINT ),  1, fp);		// Zero Stride

		fwrite(&pDst->mtW  , sizeof(FLOAT), 16, fp);		// World Matrix
		fwrite(&pDst->mtL  , sizeof(FLOAT), 16, fp);		// Local Matrix
		fwrite(&pDst->mtTmL, sizeof(FLOAT), 16, fp);		// Transform and Movement

		fwrite(&pDst->nAni , sizeof(INT  ),  1, fp);		// Animation Matrix Number of Animation
	}


	//4. Writing Index, Vertex Buffer
	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::Tgeo* pDst = &m_pGeo[n];

		if(pDst->iFce)	fwrite(pDst->pIdx, sizeof(LCX_Ase::Tidx), pDst->iFce, fp);
		if(pDst->iNvx)	fwrite(pDst->pVtx, pDst->dVtx, pDst->iNvx, fp);
	}


	//6. Writing Animation. Matrix
	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::Tgeo* pDst = &m_pGeo[n];

		INT	nAni = pDst->nAni;

		if(0>=nAni)
			continue;

		//LCX_Ase::Ttrack* t = &pDst->vAni[j];
		fwrite(pDst->vAni, sizeof(LCX_Ase::Ttrack), nAni, fp);
	}

	fclose(fp);
}



void CLcxAst::SaveToAsmTxt()
{
	INT		n=0;
	INT		j=0;

	char drive[LC_MAX_DRIVE]={0};
	char dir  [LC_MAX_DIR  ]={0};
	char fname[LC_MAX_FNAME]={0};
	char ext  [LC_MAX_EXT  ]=".ast";
	char sFile[LC_MAX_PATH ]={0};


	LcStr_SplitPath( m_sFile, drive, dir, fname, NULL );

	sprintf(sFile, "%s%s%s%s", drive, dir, fname, ext);

	FILE*	fp = fopen(sFile, "wt");

	//1. save Header
	fprintf(fp, "Version: %s\n", m_Info.sVer);
	fprintf(fp, "FrameBeginEndFpsTick: %3d %3d %3d %3d\n\n"
				, m_Info.nFrmF
				, m_Info.nFrmL
				, m_Info.nFrmS
				, m_Info.nFrmT
				);

	fprintf(fp, "MaterialGeometryNum: %3d %3d\n\n", m_Info.nMtl, m_Info.nGeo);



	//2. Writing Material
	for(n=0; n<m_Info.nMtl; ++n)
	{
		LCX_Ase::Tmtl* pMtl = &m_pMtl[n];

		fprintf(fp, "\nMaterial: %3d\n", n);
		fprintf(fp, "	TextureDiffuse: \"%s\"\n", pMtl->sTex);
	}

	fprintf(fp, "\n\n");


	//3. Writing Geometry
	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::_AseGeo* pSrc = &m_pGeoS[n];
		LCX_Ase::Tgeo*	  pDst = &m_pGeo [n];

		fprintf(fp, "\n\n");
		fprintf(fp, "Geometry_Parent_Material: %2d %2d %2d\n", n, pDst->nPrn, pDst->nMtl);
		fprintf(fp, "	Node_Name: \"%s\"\n", pDst->sName);
		fprintf(fp, "	Parent_Name: \"%s\"\n", pSrc->sNodePrn);
		fprintf(fp, "	FaceNum_VtxNum_FVF_Stride: %3d %3d   0x%X %d\n"	, pDst->iFce, pDst->iNvx, pDst->dFVF, pDst->dVtx);

		// Writing Index Buffer
		if(pDst->iFce < 0)
			continue;


		fprintf(fp, "	Faces:\n");

		for(j=0; j<pDst->iFce; ++j)
		{
			fprintf(fp, "	%4d %4d %4d\n", (INT)pDst->pIdx[j].a, (INT)pDst->pIdx[j].b, (INT)pDst->pIdx[j].c);
		}

		fprintf(fp, "	Vertices:\n");


		for(j=0; j<pDst->iNvx; ++j)
		{
			if(LCX_Ase::Tvtx::FVF == pDst->dFVF)
			{
				LCX_Ase::Tvtx* pVtx = (LCX_Ase::Tvtx*)pDst->pVtx;
				fprintf(fp, "	%9.5f %9.5f %9.5f\n", pVtx[j].p.x, pVtx[j].p.y, pVtx[j].p.z);
			}
			else if(LCX_Ase::TvtxUV::FVF == pDst->dFVF)
			{
				LCX_Ase::TvtxUV* pVtx = (LCX_Ase::TvtxUV*)pDst->pVtx;
				fprintf(fp, "	%9.5f %9.5f %9.5f   %9.5f %9.5f\n", pVtx[j].p.x, pVtx[j].p.y, pVtx[j].p.z, pVtx[j].u, pVtx[j].v);
			}
		}
	}

	fprintf(fp, "\n\n");

	//4. Writing Animation. scaling은 제외 <-- biped Animation은 scaling이 없다.
	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::_AseGeo* pSrc	= &m_pGeoS[n];

		INT	nAniRot = pSrc->vRot.size();
		INT	nAniTrs = pSrc->vTrs.size();

		fprintf(fp, "AniGeoIdx_RotTransNum: %2d %3d %3d  ", n, nAniRot, nAniTrs);
		fprintf(fp, "\"%s\"\n", pSrc->sNodeCur);


		fprintf(fp, "	WorldTM:	"
					"%9.5f %9.5f %9.5f"
					"%9.5f %9.5f %9.5f"
					"%9.5f %9.5f %9.5f"
					"%9.5f %9.5f %9.5f %9.5f\n"

					, pSrc->mtW._11, pSrc->mtW._12, pSrc->mtW._13
					, pSrc->mtW._21, pSrc->mtW._22, pSrc->mtW._23
					, pSrc->mtW._31, pSrc->mtW._32, pSrc->mtW._33
					, pSrc->mtW._41, pSrc->mtW._42, pSrc->mtW._43, pSrc->mtW._44);



		fprintf(fp, "	LocalTM:	"
					"%9.5f %9.5f %9.5f"
					"%9.5f %9.5f %9.5f"
					"%9.5f %9.5f %9.5f"
					"%9.5f %9.5f %9.5f %9.5f\n"

					, pSrc->mtL._11, pSrc->mtL._12, pSrc->mtL._13
					, pSrc->mtL._21, pSrc->mtL._22, pSrc->mtL._23
					, pSrc->mtL._31, pSrc->mtL._32, pSrc->mtL._33
					, pSrc->mtL._41, pSrc->mtL._42, pSrc->mtL._43, pSrc->mtL._44);


		fprintf(fp, "	DestTM:		"
					"%9.5f %9.5f %9.5f"
					"%9.5f %9.5f %9.5f"
					"%9.5f %9.5f %9.5f"
					"%9.5f %9.5f %9.5f %9.5f\n"

					, pSrc->TmInf.mtL._11, pSrc->TmInf.mtL._12, pSrc->TmInf.mtL._13
					, pSrc->TmInf.mtL._21, pSrc->TmInf.mtL._22, pSrc->TmInf.mtL._23
					, pSrc->TmInf.mtL._31, pSrc->TmInf.mtL._32, pSrc->TmInf.mtL._33
					, pSrc->TmInf.mtL._41, pSrc->TmInf.mtL._42, pSrc->TmInf.mtL._43, pSrc->TmInf.mtL._44);

		fprintf(fp, "\n");


		fprintf(fp, "	Rotation:\n");

		// Writing Rotation
		for(j=0; j<nAniRot; ++j)
		{
			LCX_Ase::Ttrack4* t = &pSrc->vRot[j];
			fprintf(fp, "	%4d %9.5f %9.5f %9.5f %9.5f", t->nF, t->x, t->y, t->z, t->w);
			fprintf(fp, "\n");
		}


		fprintf(fp, "	Translation:\n");

		// Writing Translation
		for(j=0; j<nAniTrs; ++j)
		{
			LCX_Ase::Ttrack4* t = &pSrc->vTrs[j];
			fprintf(fp, "	%4d %9.5f %9.5f %9.5f", t->nF, t->x, t->y, t->z);
			fprintf(fp, "\n");
		}

		fprintf(fp, "\n");
	}




	fprintf(fp, "\n\n");

	//5. Animation Matrix
	for(n=0; n<m_Info.nGeo; ++n)
	{
		LCX_Ase::Tgeo* pDst = &m_pGeo[n];

		INT	nAni = pDst->nAni;

		fprintf(fp, "Animation Matrix( %2d ) %2d \"%s\"\n", n, nAni, pDst->sName);

		for(j = 0; j<nAni; ++j)
		{
			LCX_Ase::Ttrack* tr = &(pDst->vAni[j]);

			INT				nF = tr->nF;
			LCXMATRIX4X3* pmt = &(tr->tm);


			fprintf(fp, "\t %4d  %4d  "
						"%9.5f %9.5f %9.5f  "
						"%9.5f %9.5f %9.5f  "
						"%9.5f %9.5f %9.5f  "
						"%9.5f %9.5f %9.5f\n"
						, j
						, nF
						, pmt->_11, pmt->_12, pmt->_13
						, pmt->_21, pmt->_22, pmt->_23
						, pmt->_31, pmt->_32, pmt->_33
						, pmt->_41, pmt->_42, pmt->_43);

		}


		fprintf(fp, "\n");
	}


	fprintf(fp, "\n");


	fclose(fp);
}


