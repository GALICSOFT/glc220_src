// Implementation of the CTblMdl class.
//
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "../11gldc22/LnD3Type.h"

#include <LnUtil.h>
#include <LnUtilDx.h>
#include <LnBase.h>

#include <LnxEntity.h>

#include "LntEntity.h"

#include "TblMdl.h"


CTblMdl::TbS::TbS()
{
	pT	= NULL;
	nT	= 0;
}


CTblMdl::TbS::~TbS()
{
	if(pT)
	{
		ILcMdl*	p = (ILcMdl*)pT;
		delete p;
		pT = NULL;
	}
}
	


CTblMdl::TbM::TbM()
{
}

CTblMdl::TbM::~TbM()
{
	int iSize = (INT)vS.size();

	for(int i=0; i<iSize; ++i)
	{
		TbS*	t = vS[i];

		delete t;
	}

	vS.clear();
}
	
	
	
	
	
char* sCmdMd[] =
{
	"Version",			//0
		"TbMst",		//1
		"NumC",			//2
		
		"TbSub",		//3
		"Index",		//4
		"NumS",			//5
		"Path",			//6
		"Rec*",			//7
};



CTblMdl::CTblMdl()
{
}


CTblMdl::~CTblMdl()
{
	Destroy();
}

void CTblMdl::Destroy()
{
	int iSize = (INT)m_vM.size();

	for(int i=0; i<iSize; ++i)
	{
		TbM* t = m_vM[i];

		delete t;
	}

	m_vM.clear();
}


INT CTblMdl::Create(void* p1, void* p2, void* p3, void* p4)
{
	//	char*	sFile = "Data/TblMdl.dat";
	strcpy(m_sF, (char*)p1);

	if(FAILED(Load(m_sF)))
		return -1;
	
	return 0;
}


INT CTblMdl::CreateSub(INT nM, INT nS, void* pEtc)
{
	char sFile[512] ="\0";

	char*	sName = m_vM[nM]->vS[nS]->sF;
	
	sprintf(sFile, "%s%s", m_vM[nM]->sP, sName);
	
	INT		nTm=-1, nTs=-1;
	ILcMdl*	pObj = NULL;

	// Model Load...
	if(FAILED(LcMdl_Create(NULL, &pObj, sFile)))
		return -1;

	// Texture Load...
	char path_tex[_MAX_PATH]={0};
	char drive[_MAX_DRIVE]={0};
	char dir[_MAX_DIR]={0};

	LcStr_SplitPath(sFile, drive, dir, NULL, NULL );

	if(strlen(drive)>2)
		sprintf(path_tex, "%s/%s", drive, dir);
	else
		sprintf(path_tex, "%s", dir);

	if(FAILED(pObj->Query( "Load Texture", path_tex)))
		return -1;

	m_vM[nM]->vS[nS]->pT = pObj;
	return 0;
}


INT CTblMdl::Delete(INT nM, INT nS)
{
	return -1;
}


INT CTblMdl::Select(void** pOut, INT nM, INT nS)
{
	if( nM < 0 ||
		nS < 0 ||
		nM >= (INT)(m_vM.size()) ||
		nS >= (INT)(m_vM[nM]->vS.size() ) )
		return -1;
	
	if(!m_vM[nM]->vS[nS]->pT)
		CreateSub(nM, nS);
	
	if(!m_vM[nM]->vS[nS]->pT)
		return -1;
		
	(*pOut) = m_vM[nM]->vS[nS]->pT;
	return 0;
}


INT	CTblMdl::Select(void** pOut, INT* nM/*Out*/, INT* nS/*Out*/, char* sFile)
{
	INT i, j;

	int iM = 0;
	int iS = 0;

	iM = (INT)m_vM.size();
	
	for(i=0 ; i< iM; ++i)
	{
		TbM* pM = m_vM[i];

		iS = (INT)(pM->vS.size());
		
		for(j=0 ; j< iS; ++j)
		{
			TbS* pS = pM->vS[j];
			
			if ( !_stricmp( pS->sF, sFile) )
			{
				*nM = i;
				*nS = j;

				if(pOut)
				{
					if(FAILED(Select(pOut, i, j)))
						return -1;

					return 0;
				}
				
				return 0;
			}
		}
	}

	*nM = -1;
	*nS = -1;
	
	return -1;
}

INT CTblMdl::SelectInfo(void** pOut, INT nM, INT nS)
{
	if( nM < 0 ||
		nS < 0 ||
		nM >= (INT)(m_vM.size()) ||
		nS >= (INT)(m_vM[nM]->vS.size()) )
		return -1;
	
	if(!m_vM[nM]->vS[nS]->pT)
		CreateSub(nM, nS);
	
	if(!m_vM[nM]->vS[nS]->pT)
		return -1;
		
	memcpy(pOut, &(m_vM[nM]->vS[nS]->nT), sizeof(INT));
	return 0;
}


INT CTblMdl::SelectName(void** pOut, INT nM, INT nS)
{
	if( nM < 0 ||
		nS < 0 ||
		nM >= (INT)(m_vM.size()) ||
		nS >= (INT)(m_vM[nM]->vS.size()) )
		return -1;
	
	if(! m_vM[nM]->vS[nS]->pT)
		CreateSub(nM, nS);
	
	if(!m_vM[nM]->vS[nS]->pT)
		return -1;
	
	strcpy((char*)pOut, m_vM[nM]->vS[nS]->sF);
	return 0;
}


INT CTblMdl::Load(char* pcFileName)
{
	FILE * fp = NULL;
	char sLine[512]="\0";
	
	fp = fopen(pcFileName, "rt");
	
	if(NULL == fp)
		return -1;
	
#define	READ_TEXT_LINE(index)	\
	if(!strncmp(sLine, sCmdMd[index], strlen(sCmdMd[index])))
	
	while(!feof(fp))
	{
		LcFile_ReadFileLine(fp, sLine);
		
		READ_TEXT_LINE(0)
			sscanf(sLine, "%*s %s", m_sV);
		
		READ_TEXT_LINE(1)
		{
			while(!feof(fp))
			{
				LcFile_ReadFileLine(fp, sLine);
				
				if('}' == sLine[0])
					break;
				
				READ_TEXT_LINE(2)
				{
					INT	iM=0;
					sscanf(sLine,"%*s %d", &iM);
					
					if(iM>0)
					{
						for(int n=0; n<iM; ++n)
						{
							TbM* pM = new TbM;
							m_vM.push_back( pM);
						}
					}
				}
				
				
				READ_TEXT_LINE(3)
				{
					INT nM=-1;
					INT nS=0;
					
					while(!feof(fp))
					{
						LcFile_ReadFileLine(fp, sLine);
						
						if('}' == sLine[0])
							break;
						
						READ_TEXT_LINE(4)
						{
							sscanf(sLine, "%*s %d", &nM);
						}
						
						READ_TEXT_LINE(5)
						{
							INT iS=0;
							
							sscanf(sLine, "%*s %d", &iS);
							
							if(iS>0)
							{
								TbM* pM = m_vM[nM];

								for(int s=0; s<iS; ++s)
								{
									TbS* pS = new TbS;
									pM->vS.push_back(pS);
								}
							}
						}
						
						
						READ_TEXT_LINE(6)
						{
							sscanf(sLine, "%*s %s", m_vM[nM]->sP);
							
							INT iLen = strlen(m_vM[nM]->sP);
							
							if('/' != m_vM[nM]->sP[ iLen-1] )
							{
								m_vM[nM]->sP[iLen] ='/';
								m_vM[nM]->sP[ iLen+1] ='\0';
							}
							
						}
						
						
						READ_TEXT_LINE(7)
						{
							sscanf(sLine, "%*s %*s %d %s"
								,	&m_vM[nM]->vS[nS]->nT
								,	m_vM[nM]->vS[nS]->sF);
							
							++nS;
						}// if
					}//while
				}// if
			}// while
		}// if
		
#undef	READ_TEXT_LINE
	}// while
	
	fclose(fp);
	
	return 0;
}


void CTblMdl::Confirm()
{
	INT i, j;
	FILE * fp;


	INT iM = (INT)m_vM.size();
	INT iS = 0;
	
	fp = fopen("Data/TxConf.dat", "wt");
	
	fprintf(fp, "////////////////////////////////////////////////////\n//\n\n");
	fprintf(fp, "%s	%s\n\n", sCmdMd[0], m_sV);
	
	fprintf(fp, "%s\n",	sCmdMd[1]);
	fprintf(fp, "{\n");
	fprintf(fp, "	%s	%d\n", sCmdMd[2], iM);
	fprintf(fp, "\n");




	for(i=0 ; i< iM; ++i)
	{
		TbM* pM = m_vM[i];

		iS = (INT)pM->vS.size();


		fprintf(fp, "	%s\n", sCmdMd[3]);
		fprintf(fp, "	{\n");
		fprintf(fp, "		%s	%d\n", sCmdMd[4], i);
		fprintf(fp, "		%s	%d\n", sCmdMd[5], iS);
		fprintf(fp, "		%s	%s\n", sCmdMd[6], pM->sP);
		fprintf(fp, "\n");

		
		for(j=0 ; j< iS; ++j)
		{
			TbS* pS = pM->vS[j];
			
			fprintf(fp, "		%s	%d	%d	%s\n",sCmdMd[7], j,	LcStr_DWtoStr(pS->nT),	pS->sF);
		}
	
		fprintf(fp, "	}\n\n");
	}

	fprintf(fp, "}");
	
	fclose(fp);
}



