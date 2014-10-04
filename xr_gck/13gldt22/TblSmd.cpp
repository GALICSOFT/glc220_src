// Implementation of the CTblSmd class.
//
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "../11gldc22/LnD3Type.h"

#include <LnUtil.h>
#include <LnUtilDx.h>
#include <LnBase.h>

#include "LntEntity.h"

#include "TblSmd.h"


CTblSmd::TbS::TbS()
{
	nT	= 0;
	pT	= NULL;
}

CTblSmd::TbS::~TbS()
{
	if(pT)
	{
		pT = NULL;
	}
}



CTblSmd::TbM::TbM()
{
}

CTblSmd::TbM::~TbM()
{
	int iSize = vS.size();

	for(int i=0; i<iSize; ++i)
	{
		TbS*	t = vS[i];

		delete t;
	}

	vS.clear();
}


	
	
	
char* sCmdSmd[] =
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



CTblSmd::CTblSmd()
{
}


CTblSmd::~CTblSmd()
{
	Destroy();
}

void CTblSmd::Destroy()
{
	int iSize = (INT)m_vM.size();

	for(int i=0; i<iSize; ++i)
	{
		TbM* t = m_vM[i];

		delete t;
	}

	m_vM.clear();
}


INT CTblSmd::Create(void* p1, void* p2, void* p3, void* p4)
{
	//	char*	sFile = "Data/TblMdl.dat";
	strcpy(m_sF, (char*)p1);


	if(FAILED(Load(m_sF)))
		return -1;
	
	return 0;
}


INT CTblSmd::CreateSub(INT nM, INT nS, void* pEtc)
{
	char sFile[512] ="\0";
	
	sprintf(sFile, "%s%s", m_vM[nM]->sP, m_vM[nM]->vS[nS]->sF);

	// Create
	
	return 0;
}


INT CTblSmd::Delete(INT nM, INT nS)
{
	if(! m_vM[nM]->vS[nS]->pT)
		return -1;
	
	if(	m_vM[nM]->vS[nS]->pT	)
	{
		// delete ...
		m_vM[nM]->vS[nS]->pT = NULL;
	}
	
	return 0;
}





INT CTblSmd::Select(void** pOut, INT nM, INT nS)
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
	
	*pOut = m_vM[nM]->vS[nS]->pT;
	return 0;
}



INT CTblSmd::SelectInfo(void** pOut, INT nM, INT nS)
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
	
	memcpy(pOut, &(m_vM[nM]->vS[nS]->nT), sizeof( m_vM[nM]->vS[nS]->nT ));
	return 0;
}


INT CTblSmd::SelectName(void** pOut, INT nM, INT nS)
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



INT	CTblSmd::Select(void** pOut, INT* nM/*Out*/, INT* nS/*Out*/, char* sFile)
{
	INT i, j;

	int iM = 0;
	int iS = 0;

	iM = (INT)m_vM.size();
	
	for(i=0 ; i< iM; ++i)
	{
		TbM* pM = m_vM[i];

		iS = (INT)pM->vS.size();
		
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



INT CTblSmd::Load(char * pcFileName)
{
	FILE * fp = NULL;
	char sLine[512]="\0";
	
	fp = fopen(pcFileName, "rt");
	
	if(NULL == fp)
		return -1;
	
#define	READ_TEXT_LINE(index)	\
	if(!strncmp(sLine, sCmdSmd[index], strlen(sCmdSmd[index])))
	
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
						

						//(E) 2008-01-18 Editor: AFEW
						// Read.. sub...data
						READ_TEXT_LINE(7)
						{
//							sscanf(sLine, "%*s %*s %X %s"
//								,	&m_vM[nM]->vS[nS]->dF
//								,	m_vM[nM]->vS[nS]->sF);
							
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


void CTblSmd::Confirm()
{
	INT i, j;
	FILE * fp;


	INT iM = (INT)m_vM.size();
	INT iS = 0;
	
	fp = fopen("Data/TxConf.dat", "wt");
	
	fprintf(fp, "////////////////////////////////////////////////////\n//\n\n");
	fprintf(fp, "%s	%s\n\n", sCmdSmd[0], m_sV);
	
	fprintf(fp, "%s\n",	sCmdSmd[1]);
	fprintf(fp, "{\n");
	fprintf(fp, "	%s	%d\n", sCmdSmd[2], iM);
	fprintf(fp, "\n");




	for(i=0 ; i< iM; ++i)
	{
		TbM* pM = m_vM[i];

		iS = (INT)pM->vS.size();


		fprintf(fp, "	%s\n", sCmdSmd[3]);
		fprintf(fp, "	{\n");
		fprintf(fp, "		%s	%d\n", sCmdSmd[4], i);
		fprintf(fp, "		%s	%d\n", sCmdSmd[5], iS);
		fprintf(fp, "		%s	%s\n", sCmdSmd[6], pM->sP);
		fprintf(fp, "\n");

		
		for(j=0 ; j< iS; ++j)
		{
			TbS* pS = pM->vS[j];
			
			//(E) 2008-01-18 Editor: AFEW
			// printf sub data...
			//fprintf(fp, "		%s	%d	0x%x	%s\n",sCmdSmd[7], j,	pS->dF,	pS->sF);
		}
	
		fprintf(fp, "	}\n\n");
	}

	fprintf(fp, "}");
	
	fclose(fp);
}



