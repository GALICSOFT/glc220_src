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


#define MAX_PARSE_LINE	512



INT CLcxAst::ParseScene(FILE* fp)
{
	enum	EAseKey
	{
		SCENE,
		FRAME_FIRST,
		FRAME_LAST,
		FRAME_SPEED,
		FRAME_TICK,
	};



	// Parsing Keyword
	LCX_Ase::_AseKey Keywords[] =
	{
		{	"*SCENE {"				,	strlen("*SCENE {"			)	},	// SCENE
		{	"*SCENE_FIRSTFRAME"		,	strlen("*SCENE_FIRSTFRAME"	)	},	// FRAME_FIRST
		{	"*SCENE_LASTFRAME"		,	strlen("*SCENE_LASTFRAME"	)	},	// FRAME_LAST
		{	"*SCENE_FRAMESPEED"		,	strlen("*SCENE_FRAMESPEED"	)	},	// FRAME_SPEED
		{	"*SCENE_TICKSPERFRAME"	,	strlen("*SCENE_TICKSPERFRAME")	},	// FRAME_TICK
	};

	// Scene파싱
	char	sLine[MAX_PARSE_LINE]={0};
	char*	nRead = NULL;

	while(!feof(fp))
	{
		nRead = fgets(sLine, MAX_PARSE_LINE, fp);
		if(NULL == nRead)	// error or EOF
			break;

		LcStr_Trim(sLine);

		if(0 == LcStr_Strnicmp(sLine, Keywords[SCENE].sKey, Keywords[SCENE].iKey) )
		{
			while(!feof(fp))
			{
				nRead = fgets(sLine, MAX_PARSE_LINE, fp);
				if(NULL == nRead)	// error or EOF
					break;

				LcStr_Trim(sLine);

				if('}' == sLine[0])
				{
					return LC_OK;
//					break;
				}


				if(0 == LcStr_Strnicmp(sLine, Keywords[FRAME_FIRST].sKey, Keywords[FRAME_FIRST].iKey) )
				{
					INT		nFirst;
					sscanf(sLine, "%*s %d", &nFirst);
					m_Info.nFrmF = nFirst;
				}

				if(0 == LcStr_Strnicmp(sLine, Keywords[FRAME_LAST].sKey, Keywords[FRAME_LAST].iKey) )
				{
					INT		nLast;
					sscanf(sLine, "%*s %d", &nLast);
					m_Info.nFrmL = nLast;
				}

				if(0 == LcStr_Strnicmp(sLine, Keywords[FRAME_SPEED].sKey, Keywords[FRAME_SPEED].iKey) )
				{
					INT		nSpeed;
					sscanf(sLine, "%*s %d", &nSpeed);
					m_Info.nFrmS = nSpeed;
				}

				if(0 == LcStr_Strnicmp(sLine, Keywords[FRAME_TICK].sKey, Keywords[FRAME_TICK].iKey) )
				{
					INT		nTick;
					sscanf(sLine, "%*s %d", &nTick);
					m_Info.nFrmT = nTick;
				}

			}
		}

	}

	return LC_OK;
}

INT	CLcxAst::ParseMaterial(FILE* fp)
{

	enum	EAseKey
	{
		MTRL_LST	,
		MTRL_CNT	,
		MTRL_INF	,

		MTRL_MAPDIF	,
		MTRL_TEXDIF	,
	};



	// Parsing Keyword
	LCX_Ase::_AseKey Keywords[] =
	{
		{	"*MATERIAL_LIST {"		,	strlen("*MATERIAL_LIST {"	)	},	// MTRL_LST
		{	"*MATERIAL_COUNT"		,	strlen("*MATERIAL_COUNT"	)	},	// MTRL_CNT
		{	"*MATERIAL "			,	strlen("*MATERIAL "			)	},	// MTRL_INF

		{	"*MAP_DIFFUSE {"		,	strlen("*MAP_DIFFUSE {"		)	},	// MTRL_MAPDIF
		{	"*BITMAP"				,	strlen("*BITMAP"			)	},	// MTRL_TEXDIF
	};



	// Material파싱
	char	sLine[MAX_PARSE_LINE];
	char*	nRead = NULL;


	// file 포인터를 처음으로 옮긴다.
	fseek(fp, 0, SEEK_SET);


	// 머티리얼 숫자를 센다.
	m_Info.nMtl	= 0;


	while(!feof(fp))
	{
		nRead = fgets(sLine, MAX_PARSE_LINE, fp);
		if(NULL == nRead)	// error or EOF
			break;

		LcStr_Trim(sLine);

		if(0 == LcStr_Strnicmp(sLine, Keywords[MTRL_LST].sKey, Keywords[MTRL_LST].iKey) )
		{
			while(!feof(fp))
			{
				nRead = fgets(sLine, MAX_PARSE_LINE, fp);
				if(NULL == nRead)	// error or EOF
					break;

				LcStr_Trim(sLine);

				if('}' == sLine[0])
					break;

				if(0 == LcStr_Strnicmp(sLine, Keywords[MTRL_CNT].sKey, Keywords[MTRL_CNT].iKey) )
				{
					INT		iNmtl;
					sscanf(sLine, "%*s %d", &iNmtl);
					m_Info.nMtl = iNmtl;

					if(m_Info.nMtl>0)
						m_pMtlS  = new LCX_Ase::_AseMtl[m_Info.nMtl];
				}

				if(0 == LcStr_Strnicmp(sLine, Keywords[MTRL_INF].sKey, Keywords[MTRL_INF].iKey) )
				{
					INT	nMtl=-1;
					sscanf(sLine, "%*s %d", &nMtl);

					while(!feof(fp))
					{
						nRead = fgets(sLine, MAX_PARSE_LINE, fp);
						LcStr_Trim(sLine);

						if('}' == sLine[0])
							break;

						if(0 == LcStr_Strnicmp(sLine, Keywords[MTRL_MAPDIF].sKey, Keywords[MTRL_MAPDIF].iKey) )
						{
							while(!feof(fp))
							{
								nRead = fgets(sLine, MAX_PARSE_LINE, fp);
								LcStr_Trim(sLine);

								if('}' == sLine[0])
									break;

								if(0 == LcStr_Strnicmp(sLine, Keywords[MTRL_TEXDIF].sKey, Keywords[MTRL_TEXDIF].iKey) )
								{
									char	sFile[MAX_PARSE_LINE]={0};
									LcStr_Quot(sFile, sLine);

									if( 2 < strlen(sFile))
									{
										char sTx[LC_MAX_FNAME]={0};
										char dir[LC_MAX_DIR  ]={0};
										char ext[LC_MAX_EXT  ]={0};

										LcStr_SplitPath(sFile, NULL, dir, sTx, ext);
										sprintf(m_pMtlS[nMtl].sTex, "%s%s", sTx, ext);
									}
								}

							}// while
						}// if

					}// while
				}// if


			}// while
		}//if
	}// while

	return LC_OK;
}


INT CLcxAst::ParseGeometry(FILE* fp)
{
	enum EAseKey
	{
		GEOOBJECT	,

		NODE_CUR	,
		NODE_PRN	,

		MTRL_REF	,

		NODE_TM		,
		TM_ROW0		,
		TM_ROW1		,
		TM_ROW2		,
		TM_ROW3		,
		TM_POS		,
		TM_ROT		,
		TM_RTA		,
		TM_SCL		,

		MESH_INFOS ,
		MESH_NUMVTX,
		MESH_NUMFCE,
		MESH_VTXLST,
		MESH_VTXREC,
		MESH_FCELST,
		MESH_FCEREC,

		MESH_NUMTVTX,
		MESH_TVTXLST,
		MESH_TVTXREC,

		MESH_NUMTFCE,
		MESH_TFCELST,
		MESH_TFCEREC,

		MESH_NORMAL,
		MESH_FCENRL,

		TM_ANIMATION,
		ROT_TRACK	,
		ROT_SAMPLE	,
		SCALE_TRACK	,
		SCALE_SAMPLE,
		POS_TRACK	,
		POS_SAMPLE	,

		ROT_TCB		,
		ROT_TCB_KEY	,
	};



	// Parsing Keyword
	LCX_Ase::_AseKey Keywords[] =
	{
		{	"*GEOMOBJECT {"			,	strlen("*GEOMOBJECT {"		)	},	// GEOOBJECT

		{	"*NODE_NAME"			,	strlen("*NODE_NAME"			)	},	// NODE_CUR
		{	"*NODE_PARENT"			,	strlen("*NODE_PARENT"		)	},	// NODE_PRN

		{	"*MATERIAL_REF"			,	strlen("*MATERIAL_REF"		)	},	// MTRL_REF

		{	"*NODE_TM {"			,	strlen("*NODE_TM {"			)	},	// NODE_TM
		{	"*TM_ROW0"				,	strlen("*TM_ROW0"			)	},	// TM_ROW0
		{	"*TM_ROW1"				,	strlen("*TM_ROW1"			)	},	// TM_ROW1
		{	"*TM_ROW2"				,	strlen("*TM_ROW2"			)	},	// TM_ROW2
		{	"*TM_ROW3"				,	strlen("*TM_ROW3"			)	},	// TM_ROW3
		{	"*TM_POS"				,	strlen("*TM_POS"			)	},	// TM_POS
		{	"*TM_ROTAXIS"			,	strlen("*TM_ROTAXIS"		)	},	// TM_ROT
		{	"*TM_ROTANGLE"			,	strlen("*TM_ROTANGLE"		)	},	// TM_RTA
		{	"*TM_SCALE"				,	strlen("*TM_SCALE"			)	},	// TM_SCL

		{	"*MESH {"				,	strlen("*MESH {"			)	},	// MESH_INFOS
		{	"*MESH_NUMVERTEX"		,	strlen("*MESH_NUMVERTEX"	)	},	// MESH_NUMVTX
		{	"*MESH_NUMFACES"		,	strlen("*MESH_NUMFACES"		)	},	// MESH_NUMFCE
		{	"*MESH_VERTEX_LIST {"	,	strlen("*MESH_VERTEX_LIST {")	},	// MESH_VTXLST
		{	"*MESH_VERTEX"			,	strlen("*MESH_VERTEX"		)	},	// MESH_VTXREC
		{	"*MESH_FACE_LIST {"		,	strlen("*MESH_FACE_LIST {"	)	},	// MESH_FCELST
		{	"*MESH_FACE"			,	strlen("*MESH_FACE"			)	},	// MESH_FCEREC

		{	"*MESH_NUMTVERTEX"		,	strlen("*MESH_NUMTVERTEX"	)	},	// MESH_NUMTVTX
		{	"*MESH_TVERTLIST {"		,	strlen("*MESH_TVERTLIST {"	)	},	// MESH_TVTXLST
		{	"*MESH_TVERT"			,	strlen("*MESH_TVERT"		)	},	// MESH_TVTXREC

		{	"*MESH_NUMTVFACES"		,	strlen("*MESH_NUMTVFACES"	)	},	// MESH_NUMTFCE
		{	"*MESH_TFACELIST {"		,	strlen("*MESH_TFACELIST {"	)	},	// MESH_TFCELST
		{	"*MESH_TFACE"			,	strlen("*MESH_TFACE"		)	},	// MESH_TFCEREC

		{	"*MESH_NORMALS {"		,	strlen("*MESH_NORMALS {"	)	},	// MESH_NORMAL
		{	"*MESH_FACENORMAL"		,	strlen("*MESH_FACENORMAL"	)	},	// MESH_FCENRL

		{	"*TM_ANIMATION {"		,	strlen("*TM_ANIMATION {"		)	},	// TM_ANIMATION
		{	"*CONTROL_ROT_TRACK {"	,	strlen("*CONTROL_ROT_TRACK {"	)	},	// ROT_TRACK
		{	"*CONTROL_ROT_SAMPLE"	,	strlen("*CONTROL_ROT_SAMPLE"	)	},	// ROT_SAMPLE
		{	"*CONTROL_SCALE_TRACK {",	strlen("*CONTROL_SCALE_TRACK {"	)	},	// SCALE_TRACK
		{	"*CONTROL_SCALE_SAMPLE"	,	strlen("*CONTROL_SCALE_SAMPLE"	)	},	// SCALE_SAMPLE
		{	"*CONTROL_POS_TRACK {"	,	strlen("*CONTROL_POS_TRACK {"	)	},	// POS_TRACK
		{	"*CONTROL_POS_SAMPLE"	,	strlen("*CONTROL_POS_SAMPLE"	)	},	// POS_SAMPLE

		{	"*CONTROL_ROT_TCB {"	,	strlen("*CONTROL_ROT_TCB {"		)	},	// ROT_TCB
		{	"*CONTROL_TCB_ROT_KEY"	,	strlen("*CONTROL_TCB_ROT_KEY"	)	},	// ROT_TCB_KEY
	};



	// Scene 파싱
	char	sLine[MAX_PARSE_LINE];
	char*	nRead = NULL;

	// file 포인터를 처음으로 옮긴다.
	fseek(fp, 0, SEEK_SET);

	//Geometry의 숫자를 센다.
	m_Info.nGeo	= 0;

	while(!feof(fp))
	{
		nRead = fgets(sLine, MAX_PARSE_LINE, fp);
		LcStr_Trim(sLine);

		if(0 == LcStr_Strnicmp(sLine, Keywords[GEOOBJECT].sKey, Keywords[GEOOBJECT].iKey) )
			++m_Info.nGeo;
	}

	if(0==m_Info.nGeo)
		return LC_CANNT_FIND_RESOURCE;

	// file 포인터를 처음으로 옮긴다.
	fseek(fp, 0, SEEK_SET);


	m_pGeoS = new LCX_Ase::_AseGeo[m_Info.nGeo];

	INT	nGeoIdx = -1;

	while(!feof(fp))
	{
		nRead = fgets(sLine, MAX_PARSE_LINE, fp);
		if(NULL == nRead)	// error or EOF
			break;

		LcStr_Trim(sLine);


		if(0 == LcStr_Strnicmp(sLine, Keywords[GEOOBJECT].sKey, Keywords[GEOOBJECT].iKey) )
		{
			++nGeoIdx;
			LCX_Ase::_AseGeo* pSrc = &m_pGeoS[nGeoIdx];

			while(!feof(fp))
			{
				nRead = fgets(sLine, MAX_PARSE_LINE, fp);
				if(NULL == nRead)	// error or EOF
					break;

				LcStr_Trim(sLine);

				if('}' == sLine[0])
					break;


				if(0 == LcStr_Strnicmp(sLine, Keywords[MTRL_REF].sKey, Keywords[MTRL_REF].iKey) )
				{
					INT		nMtl;
					sscanf(sLine, "%*s %d", &nMtl);
					pSrc->nMtlRef = nMtl;
				}

				if(0 == LcStr_Strnicmp(sLine, Keywords[NODE_CUR].sKey, Keywords[NODE_CUR].iKey) )
				{
					char	sName[64]={0};
					LcStr_Quot(sName, sLine);
					strcpy(pSrc->sNodeCur, sName);
				}

				if(0 == LcStr_Strnicmp(sLine, Keywords[NODE_PRN].sKey, Keywords[NODE_PRN].iKey) )
				{
					char	sName[64]={0};
					LcStr_Quot(sName, sLine);
					strcpy(pSrc->sNodePrn, sName);
				}


				if(0 == LcStr_Strnicmp(sLine, Keywords[NODE_TM].sKey, Keywords[NODE_TM].iKey) )
				{
					// Node TM
					while(!feof(fp))
					{
						nRead = fgets(sLine, MAX_PARSE_LINE, fp);
						LcStr_Trim(sLine);

						if('}' == sLine[0])
							break;


						if(0 == LcStr_Strnicmp(sLine, Keywords[TM_ROW0].sKey, Keywords[TM_ROW0].iKey) )
						{
							FLOAT	x=0.F, y=0.F, z=0.F, w=0.F;
							sscanf(sLine, "%*s %f %f %f", &x, &y, &z);

							pSrc->TmInf.mtW._11  = x;
							pSrc->TmInf.mtW._12  = y;
							pSrc->TmInf.mtW._13  = z;
							pSrc->TmInf.mtW._14  = w;
						}

						if(0 == LcStr_Strnicmp(sLine, Keywords[TM_ROW1].sKey, Keywords[TM_ROW1].iKey) )
						{
							FLOAT	x=0.F, y=0.F, z=0.F, w=0.F;
							sscanf(sLine, "%*s %f %f %f", &x, &y, &z);

							pSrc->TmInf.mtW._21  = x;
							pSrc->TmInf.mtW._22  = y;
							pSrc->TmInf.mtW._23  = z;
							pSrc->TmInf.mtW._24  = w;
						}

						if(0 == LcStr_Strnicmp(sLine, Keywords[TM_ROW2].sKey, Keywords[TM_ROW2].iKey) )
						{
							FLOAT	x=0.F, y=0.F, z=0.F, w=0.F;
							sscanf(sLine, "%*s %f %f %f", &x, &y, &z);

							pSrc->TmInf.mtW._31  = x;
							pSrc->TmInf.mtW._32  = y;
							pSrc->TmInf.mtW._33  = z;
							pSrc->TmInf.mtW._34  = w;
						}

						if(0 == LcStr_Strnicmp(sLine, Keywords[TM_ROW3].sKey, Keywords[TM_ROW3].iKey) )
						{
							FLOAT	x=0.F, y=0.F, z=0.F;
							FLOAT	w=1.F;
							sscanf(sLine, "%*s %f %f %f", &x, &y, &z);

							pSrc->TmInf.mtW._41  = x;
							pSrc->TmInf.mtW._42  = y;
							pSrc->TmInf.mtW._43  = z;
							pSrc->TmInf.mtW._44  = w;
						}

						if(0 == LcStr_Strnicmp(sLine, Keywords[TM_POS].sKey, Keywords[TM_POS].iKey) )
						{
							FLOAT	x=0.F, y=0.F, z=0.F;
							sscanf(sLine, "%*s %f %f %f", &x, &y, &z);

							pSrc->TmInf.Px  = x;
							pSrc->TmInf.Py  = y;
							pSrc->TmInf.Pz  = z;
						}


						if(0 == LcStr_Strnicmp(sLine, Keywords[TM_ROT].sKey, Keywords[TM_ROT].iKey) )
						{
							FLOAT	x=0.F, y=0.F, z=0.F;
							sscanf(sLine, "%*s %f %f %f", &x, &y, &z);

							pSrc->TmInf.Rx  = x;
							pSrc->TmInf.Ry  = y;
							pSrc->TmInf.Rz  = z;
						}

						if(0 == LcStr_Strnicmp(sLine, Keywords[TM_RTA].sKey, Keywords[TM_RTA].iKey) )
						{
							FLOAT	w=0.F;
							sscanf(sLine, "%*s %f", &w);

							pSrc->TmInf.Rw  = w;
						}

						if(0 == LcStr_Strnicmp(sLine, Keywords[TM_SCL].sKey, Keywords[TM_SCL].iKey) )
						{
							FLOAT	x=0.F, y=0.F, z=0.F;
							sscanf(sLine, "%*s %f %f %f", &x, &y, &z);

							pSrc->TmInf.Sx  = x;
							pSrc->TmInf.Sy  = y;
							pSrc->TmInf.Sz  = z;
						}

					}// while NODE_TM
				}


				if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_INFOS].sKey, Keywords[MESH_INFOS].iKey) )
				{
					while(!feof(fp))
					{
						nRead = fgets(sLine, MAX_PARSE_LINE, fp);
						LcStr_Trim(sLine);

						if('}' == sLine[0])
							break;

						if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_NUMVTX].sKey, Keywords[MESH_NUMVTX].iKey) )
						{
							INT		iNVx;
							sscanf(sLine, "%*s %d", &iNVx);

							pSrc->iNumVtx = iNVx;

							if(iNVx>0)
								pSrc->pLstVtx = new LCX_Ase::_AsePos[iNVx];
						}

						if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_NUMFCE].sKey, Keywords[MESH_NUMFCE].iKey) )
						{
							INT		iNIx;
							sscanf(sLine, "%*s %d", &iNIx);

							pSrc->iNumFce = iNIx;

							if(iNIx>0)
								pSrc->pLstFce = new LCX_Ase::Tidx[iNIx];
						}


						if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_VTXLST].sKey, Keywords[MESH_VTXLST].iKey) )
						{
							while(!feof(fp))
							{
								nRead = fgets(sLine, MAX_PARSE_LINE, fp);
								LcStr_Trim(sLine);

								if('}' == sLine[0])
									break;

								if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_VTXREC].sKey, Keywords[MESH_VTXREC].iKey) )
								{
									INT		nIdx=0;
									FLOAT	x=0.F, y=0.F, z=0.F;
									sscanf(sLine, "%*s %d %f %f %f", &nIdx, &x, &y, &z);

									pSrc->pLstVtx[nIdx].x = x;
									pSrc->pLstVtx[nIdx].y = y;
									pSrc->pLstVtx[nIdx].z = z;
								}

							}
						}

						if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_FCELST].sKey, Keywords[MESH_FCELST].iKey) )
						{
							while(!feof(fp))
							{
								nRead = fgets(sLine, MAX_PARSE_LINE, fp);
								LcStr_Trim(sLine);

								if('}' == sLine[0])
									break;

								if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_FCEREC].sKey, Keywords[MESH_FCEREC].iKey) )
								{
									INT		nIdx=0;
									INT		a=0, b=0, c=0;

									//            *MESH_FACE    0:    A:    0   B:    1   C:    2
									sscanf(sLine, "%*s         %d:   %*s   %d  %*s   %d  %*s   %d"
												, &nIdx, &a, &b, &c);

									pSrc->pLstFce[nIdx].a = a;
									pSrc->pLstFce[nIdx].b = b;
									pSrc->pLstFce[nIdx].c = c;
								}

							}// while
						}// if == MESH_FACE_LIST


						if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_NUMTVTX].sKey, Keywords[MESH_NUMTVTX].iKey) )
						{
							INT		iNtvtx;
							sscanf(sLine, "%*s %d", &iNtvtx);
							pSrc->iNumTvtx = iNtvtx;
							pSrc->pLstTvtx = new LCX_Ase::_AseTvtx[iNtvtx];

						}// if == *MESH_NUMTVERTEX


						if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_TVTXLST].sKey, Keywords[MESH_TVTXLST].iKey) )
						{
							while(!feof(fp))
							{
								nRead = fgets(sLine, MAX_PARSE_LINE, fp);
								LcStr_Trim(sLine);

								if('}' == sLine[0])
									break;

								if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_TVTXREC].sKey, Keywords[MESH_TVTXREC].iKey) )
								{
									INT		nIdx=0;
									FLOAT	u=0.F, v=0.F, w=0.F;
									sscanf(sLine, "%*s %d %f %f %f", &nIdx, &u, &v, &w);

									pSrc->pLstTvtx[nIdx].u = u;


									//(E) 2009-07-27 Editor: AFEW
									pSrc->pLstTvtx[nIdx].v = v;
									pSrc->pLstTvtx[nIdx].w = w;
								}


							}// while
						}// if == *MESH_TVERTLIST {


						if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_NUMTFCE].sKey, Keywords[MESH_NUMTFCE].iKey) )
						{
							INT		iNtfce;
							sscanf(sLine, "%*s %d", &iNtfce);
							pSrc->iNumTfce = iNtfce;
							pSrc->pLstTfce = new LCX_Ase::Tidx[iNtfce];

						}// if == *MESH_NUMTVFACES


						if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_TFCELST].sKey, Keywords[MESH_TFCELST].iKey) )
						{
							while(!feof(fp))
							{
								nRead = fgets(sLine, MAX_PARSE_LINE, fp);
								LcStr_Trim(sLine);

								if('}' == sLine[0])
									break;

								if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_TFCEREC].sKey, Keywords[MESH_TFCEREC].iKey) )
								{
									INT		nIdx=0;
									INT		a=0, b=0, c=0;

									sscanf(sLine, "%*s %d %d %d %d", &nIdx, &a, &b, &c);

									pSrc->pLstTfce[nIdx].a = a;
									pSrc->pLstTfce[nIdx].b = b;
									pSrc->pLstTfce[nIdx].c = c;
								}


							}// while
						}// if == *MESH_TFACELIST {


						// Normal Vector
						if(0 == LcStr_Strnicmp(sLine, Keywords[MESH_NORMAL].sKey, Keywords[MESH_NORMAL].iKey) )
						{
//							pSrc->pLstNor = new LCX_Ase::_AseNor[pSrc->iNumVtx];
//
							while(!feof(fp))
							{
								nRead = fgets(sLine, MAX_PARSE_LINE, fp);
								LcStr_Trim(sLine);

								if('}' == sLine[0])
									break;

							}// while

						}// if == *MESH_NORMALS {



					}// while
				}// if



				// TM_ANIMATION: Do not read. read Parse Animation
				if(0 == LcStr_Strnicmp(sLine, Keywords[TM_ANIMATION].sKey, Keywords[TM_ANIMATION].iKey) )
				{
					while(!feof(fp))
					{
						nRead = fgets(sLine, MAX_PARSE_LINE, fp);
						LcStr_Trim(sLine);

						if('}' == sLine[0])
							break;

						if(0 == LcStr_Strnicmp(sLine, Keywords[ROT_TRACK].sKey, Keywords[ROT_TRACK].iKey) )
						{
							while(!feof(fp))
							{
								nRead = fgets(sLine, MAX_PARSE_LINE, fp);
								LcStr_Trim(sLine);

								if('}' == sLine[0])
									break;

							}// while
						}// if == *CONTROL_ROT_TRACK {


						if(0 == LcStr_Strnicmp(sLine, Keywords[POS_TRACK].sKey, Keywords[POS_TRACK].iKey) )
						{
							while(!feof(fp))
							{
								nRead = fgets(sLine, MAX_PARSE_LINE, fp);
								LcStr_Trim(sLine);

								if('}' == sLine[0])
									break;

							}// while
						}// if == *CONTROL_SCALE_TRACK {

						if(0 == LcStr_Strnicmp(sLine, Keywords[SCALE_TRACK].sKey, Keywords[SCALE_TRACK].iKey) )
						{
							while(!feof(fp))
							{
								nRead = fgets(sLine, MAX_PARSE_LINE, fp);
								LcStr_Trim(sLine);

								if('}' == sLine[0])
									break;

							}// while
						}// if == *CONTROL_POS_TRACK {


						if(0 == LcStr_Strnicmp(sLine, Keywords[ROT_TCB].sKey, Keywords[ROT_TCB].iKey) )
						{
							while(!feof(fp))
							{
								nRead = fgets(sLine, MAX_PARSE_LINE, fp);
								LcStr_Trim(sLine);

								if('}' == sLine[0])
									break;

							}// while
						}// if == *CONTROL_ROT_TCB {


					}// while
				}// if == *TM_ANIMATION {



			}// while
		}// if


	}// while

	return LC_OK;
}


INT CLcxAst::ParseAnimation(FILE* fp)
{
	enum	EAseKey
	{
		GEOOBJECT	,

		NODE_CUR	,

		TM_ANIMATION,
		ROT_TRACK	,
		ROT_SAMPLE	,
		SCALE_TRACK	,
		SCALE_SAMPLE,
		POS_TRACK	,
		POS_SAMPLE	,

		ROT_TCB		,
		ROT_TCB_KEY	,
	};



	// Parsing Keyword
	LCX_Ase::_AseKey Keywords[] =
	{
		{	"*GEOMOBJECT {"			,	strlen("*GEOMOBJECT {"		)	},	// GEOOBJECT

		{	"*NODE_NAME"			,	strlen("*NODE_NAME"			)	},	// NODE_CUR

		{	"*TM_ANIMATION {"		,	strlen("*TM_ANIMATION {"		)	},	// TM_ANIMATION
		{	"*CONTROL_ROT_TRACK {"	,	strlen("*CONTROL_ROT_TRACK {"	)	},	// ROT_TRACK
		{	"*CONTROL_ROT_SAMPLE"	,	strlen("*CONTROL_ROT_SAMPLE"	)	},	// ROT_SAMPLE
		{	"*CONTROL_SCALE_TRACK {",	strlen("*CONTROL_SCALE_TRACK {"	)	},	// SCALE_TRACK
		{	"*CONTROL_SCALE_SAMPLE"	,	strlen("*CONTROL_SCALE_SAMPLE"	)	},	// SCALE_SAMPLE
		{	"*CONTROL_POS_TRACK {"	,	strlen("*CONTROL_POS_TRACK {"	)	},	// POS_TRACK
		{	"*CONTROL_POS_SAMPLE"	,	strlen("*CONTROL_POS_SAMPLE"	)	},	// POS_SAMPLE

		{	"*CONTROL_ROT_TCB {"	,	strlen("*CONTROL_ROT_TCB {"		)	},	// ROT_TCB
		{	"*CONTROL_TCB_ROT_KEY"	,	strlen("*CONTROL_TCB_ROT_KEY"	)	},	// ROT_TCB_KEY
	};


	// Scene 파싱
	char	sLine[MAX_PARSE_LINE];
	char*	nRead = NULL;

	// file 포인터를 처음으로 옮긴다.
	fseek(fp, 0, SEEK_SET);

	while(!feof(fp))
	{
		nRead = fgets(sLine, MAX_PARSE_LINE, fp);
		if(NULL == nRead)	// error or EOF
			break;

		LcStr_Trim(sLine);

		// TM_ANIMATION
		if(0 == LcStr_Strnicmp(sLine, Keywords[TM_ANIMATION].sKey, Keywords[TM_ANIMATION].iKey) )
		{
			LCX_Ase::_AseGeo* pSrc = NULL;

			while(!feof(fp))
			{
				nRead = fgets(sLine, MAX_PARSE_LINE, fp);
				if(NULL == nRead)	// error or EOF
					break;

				LcStr_Trim(sLine);

				if('}' == sLine[0])
					break;

				// NODE_NAME
				if(0 == LcStr_Strnicmp(sLine, Keywords[NODE_CUR].sKey, Keywords[NODE_CUR].iKey) )
				{
					char	sName[64]={0};
					LcStr_Quot(sName, sLine);

					// Node의 이름과 같은 지오메트리를 찾는다.
					for(INT i=0; i<m_Info.nGeo; ++i)
					{
						LCX_Ase::_AseGeo*	pGeoCur = &m_pGeoS[i];

						if(0==LcStr_Stricmp(pGeoCur->sNodeCur, sName))
						{
							pSrc = pGeoCur;
							break;
						}
					}


				}

				// CONTROL_POS_TRACK
				if(0 == LcStr_Strnicmp(sLine, Keywords[POS_TRACK].sKey, Keywords[POS_TRACK].iKey) )
				{
					while(!feof(fp))
					{
						nRead = fgets(sLine, MAX_PARSE_LINE, fp);
						LcStr_Trim(sLine);

						if('}' == sLine[0])
							break;


						if(0 == LcStr_Strnicmp(sLine, Keywords[POS_SAMPLE].sKey, Keywords[POS_SAMPLE].iKey) )
						{
							INT		nTrck;
							FLOAT	x=0.F, y=0.F, z=0.F;
							sscanf(sLine, "%*s %d %f %f %f", &nTrck, &x, &y, &z);

							nTrck /= m_Info.nFrmT;

							LCX_Ase::Ttrack4 trck(nTrck, x, y, z, 0);
							pSrc->vTrs.push_back(trck);

						}// if

					}// while CONTROL_POS_TRACK
				}// if

				// CONTROL_ROT_TRACK
				if(0 == LcStr_Strnicmp(sLine, Keywords[ROT_TRACK].sKey, Keywords[ROT_TRACK].iKey) )
				{
					while(!feof(fp))
					{
						nRead = fgets(sLine, MAX_PARSE_LINE, fp);
						LcStr_Trim(sLine);

						if('}' == sLine[0])
							break;


						if(0 == LcStr_Strnicmp(sLine, Keywords[ROT_SAMPLE].sKey, Keywords[ROT_SAMPLE].iKey) )
						{
							LCXQUATERNION	q1;

							INT		nTrck;
							FLOAT	x=0.F, y=0.F, z=0.F, w=0.F;
							sscanf(sLine, "%*s %d %f %f %f %f", &nTrck, &x, &y, &z, &w);

							nTrck /= m_Info.nFrmT;

							q1.x = sinf(w/2.f) * x;
							q1.y = sinf(w/2.f) * y;
							q1.z = sinf(w/2.f) * z;
							q1.w = cosf(w/2.f);

							INT iSize = pSrc->vRot.size();

							if(0==iSize)
							{
								LCX_Ase::Ttrack4 trck(nTrck, q1.x, q1.y, q1.z, q1.w);
								pSrc->vRot.push_back(trck);
							}
							else
							{
								LCXQUATERNION	q2;
								LCXQUATERNION	q3;

								LCX_Ase::Ttrack4* ptrck = &pSrc->vRot[iSize-1];

								q2.x = ptrck->x;
								q2.y = ptrck->y;
								q2.z = ptrck->z;
								q2.w = ptrck->w;

								//D3DXQuaternionMultiply(&q3, &q2, &q1); ==> q3 = q1 * q2임 D3DXQuaternionMultiply함수는 오른손 법칙을 따름
								q3	= q2 * q1;		// <== Direc3D는 왼손 좌표계로 행렬을 오른쪽 방향으로 곱셈을 수행함. 따라서 q1이 먼저와야 한다.


								LCX_Ase::Ttrack4 trck(nTrck, q3.x, q3.y, q3.z, q3.w);
								pSrc->vRot.push_back( trck );
							}

						}// if

					}// while CONTROL_ROT_TRACK
				}// if


				// CONTROL_SCALE_TRACK
				if(0 == LcStr_Strnicmp(sLine, Keywords[SCALE_TRACK].sKey, Keywords[SCALE_TRACK].iKey) )
				{
					while(!feof(fp))
					{
						nRead = fgets(sLine, MAX_PARSE_LINE, fp);
						LcStr_Trim(sLine);

						if('}' == sLine[0])
							break;


						if(0 == LcStr_Strnicmp(sLine, Keywords[SCALE_SAMPLE].sKey, Keywords[SCALE_SAMPLE].iKey) )
						{
							INT		nTrck;
							FLOAT	x=0.F, y=0.F, z=0.F;
							sscanf(sLine, "%*s %d %f %f %f", &nTrck, &x, &y, &z);

							nTrck /= m_Info.nFrmT;

							LCX_Ase::Ttrack4 trck(nTrck, x, y, z, 0);
							pSrc->vScl.push_back(trck);

						}// if

					}// while CONTROL_SCALE_TRACK
				}// if


			}// while
		}// if TM_ANIMATION


	}// while

	return LC_OK;
}




INT CLcxAst::SetupRenderData()
{
	INT i=0, j=0;


	if(0 >= m_Info.nGeo || 0>m_Info.nMtl)
		return LC_EFAIL;


	m_pMtl = new LCX_Ase::Tmtl[m_Info.nMtl];
	m_pGeo = new LCX_Ase::Tgeo[m_Info.nGeo];


	// Make material
	for(i=0; i<m_Info.nMtl; ++i)
	{
		LCX_Ase::_AseMtl* pSrc = &m_pMtlS[i];
		LCX_Ase::Tmtl   * pDst = &m_pMtl [i];

		strcpy(pDst->sTex, pSrc->sTex);
	}



	// TM Processing
	// TmLocal = TmWorld * TmWorld(parent)-1
	for(i=0; i<m_Info.nGeo; ++i)
	{
		LCX_Ase::_AseGeo* pSrc = &m_pGeoS[i];
		LCX_Ase::_AseGeo* pGeoPrn = NULL;

		// Find Parent
		for(j=0; j<m_Info.nGeo; ++j)
		{
			LCX_Ase::_AseGeo* pGeoT = &m_pGeoS[j];

			if(0==LcStr_Stricmp(pSrc->sNodePrn, pGeoT->sNodeCur))
			{
				pGeoPrn			= pGeoT;

				pSrc->nNodePrn= j;			//Setup Parent Index
				pSrc->pGeoPrn= pGeoT;		//Setup Parent Pointer
				break;
			}
		}

		if(pGeoPrn)
		{
			LCXMATRIX mtPrn = pGeoPrn->TmInf.mtW;
			LCXMATRIX mtPrnI;
			LCXMATRIX mtL;
			mtPrnI.Inverse(NULL, &mtPrn);

			pSrc->TmInf.mtL = pSrc->TmInf.mtW * mtPrnI;
		}
		else
		{
			pSrc->TmInf.mtL = pSrc->TmInf.mtW;
		}
	}



	// Vertex Inversion
	// v(local) = v(world)*TmWorld-1
	for(i=0; i<m_Info.nGeo; ++i)
	{
		LCX_Ase::_AseGeo* pSrc = &m_pGeoS[i];

		LCXMATRIX mtWI;
		mtWI.Inverse(NULL, &pSrc->TmInf.mtW);

		INT		iNvtx = pSrc->iNumVtx;

		for(j=0; j<iNvtx; ++j)
		{
			LCXVECTOR3 vcI = *((LCXVECTOR3*)&(pSrc->pLstVtx[j]));
			LCXVECTOR3 vcO;

			mtWI.TransformCoord(&vcO, &vcI);

			*((LCXVECTOR3*)&(pSrc->pLstVtx[j])) = vcO;
		}
	}



	// setup Dest Name, Parent Index, and Matrix
	for(i=0; i<m_Info.nGeo; ++i)
	{
		LCX_Ase::_AseGeo* pSrc = &m_pGeoS[i];
		LCX_Ase::Tgeo*	  pDst = &m_pGeo[i];

		strcpy(pDst->sName, pSrc->sNodeCur);
		pDst->nMtl	= pSrc->nMtlRef;
		pDst->nPrn	= pSrc->nNodePrn;
		pDst->mtTmL = pSrc->TmInf.mtL;
		pDst->mtW	= pSrc->mtW;
		pDst->mtL	= pSrc->mtL;
	}




	// Setup FVF
	for(i=0; i<m_Info.nGeo; ++i)
	{
		LCX_Ase::_AseGeo* pSrc = &m_pGeoS[i];
		LCX_Ase::Tgeo*	  pDst = &m_pGeo[i];

		INT		iTfce = pSrc->iNumTfce;
		INT		nMtrl = pSrc->nMtlRef;

		if(pSrc->pLstVtx)
			pDst->dFVF = LCX_Ase::Tvtx::FVF;

		if(iTfce >0 && nMtrl>=0)
			pDst->dFVF |= LC_FVF_TEX1;
	}


	// Setup Stride
	for(i=0; i<m_Info.nGeo; ++i)
	{
		LCX_Ase::Tgeo* pDst = &m_pGeo[i];

		if(pDst->dFVF == (UINT)LCX_Ase::Tvtx::FVF)
			pDst->dVtx = sizeof(LCX_Ase::Tvtx);

		else if(pDst->dFVF == (UINT)LCX_Ase::TvtxUV::FVF)
			pDst->dVtx = sizeof(LCX_Ase::TvtxUV);
	}


	// Setup Index Data and Vertex Number
	for(i=0; i<m_Info.nGeo; ++i)
	{
		LCX_Ase::_AseGeo* pSrc = &m_pGeoS[i];
		LCX_Ase::Tgeo*	  pDst = &m_pGeo [i];

		if(pDst->dFVF == (UINT)LCX_Ase::Tvtx::FVF)	// || pDst->dFVF == (UINT)AseVtxN::FVF)
		{
			pDst->iNvx	= pSrc->iNumVtx;	// 정점의 수
			pDst->iFce	= pSrc->iNumFce;
			pDst->pIdx	= new LCX_Ase::Tidx[pDst->iFce];
			memcpy(pDst->pIdx, pSrc->pLstFce, pDst->iFce * sizeof(LCX_Ase::Tidx));
		}
		else if(pDst->dFVF == (UINT)LCX_Ase::TvtxUV::FVF)	// || pDst->dFVF == (UINT)AseVtxNUV::FVF)
		{
			pDst->iNvx	= pSrc->iNumTvtx;	// UV의 수
			pDst->iFce	= pSrc->iNumTfce;
			pDst->pIdx	= new LCX_Ase::Tidx[pDst->iFce];
			memcpy(pDst->pIdx, pSrc->pLstTfce, pDst->iFce * sizeof(LCX_Ase::Tidx));


			// 인덱스 지정
			for(j=0; j<pDst->iFce; ++j)
			{
				// 정점위치의 인덱스
				INT Va = pSrc->pLstFce[j].a;
				INT Vb = pSrc->pLstFce[j].b;
				INT Vc = pSrc->pLstFce[j].c;

				// UV의 인덱스
				INT Ta = pSrc->pLstTfce[j].a;
				INT Tb = pSrc->pLstTfce[j].b;
				INT Tc = pSrc->pLstTfce[j].c;

				// Tvtx인덱스에 있는 정점의 인덱스를 지정
				pSrc->pLstTvtx[Ta].nT = Va;
				pSrc->pLstTvtx[Tb].nT = Vb;
				pSrc->pLstTvtx[Tc].nT = Vc;
			}
		}
	}


	// Setup Vertex Buffer and Optimizing
	for(i=0; i<m_Info.nGeo; ++i)
	{
		LCX_Ase::_AseGeo* pSrc = &m_pGeoS[i];
		LCX_Ase::Tgeo*	  pDst = &m_pGeo [i];

		if(pDst->dFVF == (UINT)LCX_Ase::Tvtx::FVF)
		{
			LCX_Ase::Tvtx* pVtx = new LCX_Ase::Tvtx[pDst->iNvx];

			memcpy( pVtx, pSrc->pLstVtx, pDst->iNvx * pDst->dVtx);
			pDst->pVtx = pVtx;
		}

		else if(pDst->dFVF == (UINT)LCX_Ase::TvtxUV::FVF)
		{

			std::vector<LCX_Ase::_AseTvtx>	tVtx;

			// 인덱스를 통한 정점 설정
			for(j=pDst->iNvx-1; j>=0; --j)
			{
				INT nT = pSrc->pLstTvtx[j].nT ;

				if(-1 == nT)
				{
					for(INT n=0; n<pDst->iFce; ++n)
					{
						int a = pDst->pIdx[n].a;
						int b = pDst->pIdx[n].b;
						int c = pDst->pIdx[n].c;

						if(j == a)						// Invalid Face
							pDst->pIdx[n].a =-1;
						else if(j<a)					//decrease
							pDst->pIdx[n].a--;


						if(j == b)						// Invalid Face
							pDst->pIdx[n].b =-1;
						else if(j<b)					//decrease
							pDst->pIdx[n].b--;


						if(j == c)						// Invalid Face
							pDst->pIdx[n].c =-1;
						else if(j<c)					//decrease
							pDst->pIdx[n].c--;
					}

				}
				else
				{
					FLOAT x = pSrc->pLstVtx[nT].x;
					FLOAT y = pSrc->pLstVtx[nT].y;
					FLOAT z = pSrc->pLstVtx[nT].z;

					FLOAT u = pSrc->pLstTvtx[j].u;
					FLOAT v = pSrc->pLstTvtx[j].v;

					LCX_Ase::_AseTvtx	tv(nT, x, y, z, 0,0,0, 0xFFFFFFFF, u, v, 0);
					tVtx.push_back(tv);
				}
			}


			INT iSizeV = tVtx.size();

			LCX_Ase::TvtxUV* pVtx = new LCX_Ase::TvtxUV[iSizeV];
			for(j=0; j<iSizeV; ++j)
			{
				LCX_Ase::TvtxUV* pDstV = &pVtx[iSizeV -1-j];

				pDstV->p.x = tVtx[j].x;
				pDstV->p.y = tVtx[j].y;
				pDstV->p.z = tVtx[j].z;
				pDstV->u   = tVtx[j].u;
				pDstV->v   = tVtx[j].v;
			}


			pDst->iNvx = iSizeV;
			pDst->pVtx = pVtx;


			// Make to compete Index List
			std::vector<LCX_Ase::Tidx>	tFce;

			for(j=0; j<pDst->iFce; ++j)
			{
				int a = pDst->pIdx[j].a;
				int b = pDst->pIdx[j].b;
				int c = pDst->pIdx[j].c;

				// Invalid Face
				if(-1 == a || -1 == b || -1 == c)
					continue;

				LCX_Ase::Tidx tf(a, b, c);
				tFce.push_back(tf);
			}

			delete [] pDst->pIdx;

			int iSizeF = tFce.size();

			LCX_Ase::Tidx* pIdx = new LCX_Ase::Tidx[iSizeF];

			for(j=0; j<iSizeF; ++j)
			{
				pIdx[j].a = tFce[j].a;
				pIdx[j].b = tFce[j].b;
				pIdx[j].c = tFce[j].c;
			}

			pDst->iFce = iSizeF;
			pDst->pIdx = pIdx;
		}

	}


	// Delete List Data
	for(i=0; i<m_Info.nGeo; ++i)
	{
		LCX_Ase::_AseGeo* pSrc = &m_pGeoS[i];

		SAFE_DELETE_ARRAY(	pSrc->pLstVtx	);
		SAFE_DELETE_ARRAY(	pSrc->pLstFce	);
		SAFE_DELETE_ARRAY(	pSrc->pLstTvtx	);
		SAFE_DELETE_ARRAY(	pSrc->pLstTfce	);

		pSrc->iNumVtx	= 0;
		pSrc->iNumFce	= 0;

		pSrc->iNumTvtx	= 0;
		pSrc->iNumTfce	= 0;
	}



	// Setup Matrix
	for(i=0; i<m_Info.nGeo; ++i)
	{
		LCX_Ase::_AseGeo* pSrc = &m_pGeoS[i];
		pSrc->mtW = pSrc->TmInf.mtW;
		pSrc->mtL = pSrc->TmInf.mtL;
	}


	return LC_OK;

}




INT CLcxAst::SetupAniMatrix()
{
//	INT	F  = m_Info.nFrmF;
//	INT	L  = m_Info.nFrmL;
	INT	G  = m_Info.nGeo;


	INT	B  = 0;
	INT	E  = 0;

	INT n=0, j=0;
	int hr = 0;

	int k = 0;


	for(n=0; n<G; ++n)
	{
		LCX_Ase::_AseGeo* pSrc = &m_pGeoS[n];
		LCX_Ase::Tgeo*	  pDst = &m_pGeo [n];

		int		nAni=0;
		std::vector<LCX_Ase::Ttrack> vAni;

		INT  AniNum=0;
		INT* Pivot=NULL;

		B  = 0;
		E  = 0;

		hr = LCX_Ase::GetAniFrame(&B, &E, &AniNum, &Pivot, pSrc);

		if(LC_SUCCEEDED(hr))
			++k;


		if(0 == AniNum)
		{
			LCXMATRIX		tm;
			LCX_Ase::Ttrack tr;
			INT				nF =0;

			hr = LCX_Ase::GetAniTrack(&tm, (FLOAT)nF, pSrc);

			tr.nF = nF;
			tr.tm.SetupFrom4X4(&tm);
			vAni.push_back(tr);
		}
		else
		{
			for(j= 0; j< AniNum; ++j)
			{
				LCXMATRIX		tm;
				LCX_Ase::Ttrack tr;
				INT				nF = Pivot[j];

				hr = LCX_Ase::GetAniTrack(&tm, (FLOAT)nF, pSrc);

				tr.nF = nF;
				tr.tm.SetupFrom4X4(&tm);
				vAni.push_back(tr);
			}
		}





		nAni  = vAni.size();
		pDst->nAni = nAni;

		if( 0 >= nAni)
			continue;

		pDst->vAni = new LCX_Ase::Ttrack[nAni];

		for(j=0; j<pDst->nAni; ++j)
		{
			memcpy(&pDst->vAni[j], &vAni[j], sizeof(LCX_Ase::Ttrack));
		}


		vAni.clear();
		SAFE_DELETE_ARRAY(	Pivot	);
	}


	return 0;
}



INT CLcxAst::LoadTexture(LC_HANDLE pOption)
{
	// Setup Texture

	if(NULL == m_pMtl)
		return LC_OK;

	for(INT i=0; i<m_Info.nMtl; ++i)
	{
		LCX_Ase::Tmtl* pMtl = &m_pMtl[i];

		if(strlen(pMtl->sTex) <3)
			continue;

		char sFile[LC_MAX_PATH ]={0};
		char drive[LC_MAX_DRIVE]={0};
		char dir  [LC_MAX_DIR  ]={0};
		char fname[LC_MAX_FNAME]={0};
		char ext  [LC_MAX_EXT  ]={0};

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

