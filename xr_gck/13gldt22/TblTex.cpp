// Implementation of the CTblTex class.
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

#include "TblTex.h"


CTblTex::TbS::TbS()
{
	 pT	= NULL;
	 dF = 0x00;	 dK = 0x00FFFFFF;
}

CTblTex::TbS::~TbS()
{
	if(pT)
	{
		pT->Release();
		pT = NULL;
	}
}

CTblTex::TbM::TbM()
{
}

CTblTex::TbM::~TbM()
{
	int iSize = (INT)vS.size();

	for(int i=0; i<iSize; ++i)
	{
		TbS*	t = vS[i];

		delete t;
	}

	vS.clear();
}






CTblTex::CTblTex()
{
}


CTblTex::~CTblTex()
{
	Destroy();
}

void CTblTex::Destroy()
{
	int iSize = (INT)m_vM.size();

	for(int i=0; i<iSize; ++i)
	{
		TbM* t = m_vM[i];

		delete t;
	}

	m_vM.clear();
}


INT CTblTex::Create(void* p1, void* p2, void* p3, void* p4)
{
	//	char*	sFile = "Data/TblMdl.dat";
	strcpy(m_sF, (char*)p1);

	
	if(FAILED(Load(m_sF)))
		return -1;
	
	return 0;
}


INT CTblTex::CreateSub(INT nM, INT nS, void* pEtc)
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	char sFile[512] ="\0";

	DWORD	dColor = *((DWORD*)pEtc);
	
	sprintf(sFile, "%s%s", m_vM[nM]->sP, m_vM[nM]->vS[nS]->sF);
	
	if(FAILED(LcD3D_TexLoadFile(&m_vM[nM]->vS[nS]->pT, pDev, sFile
		, dColor
		, &m_vM[nM]->vS[nS]->pI
		, m_vM[nM]->vS[nS]->dF
		, m_vM[nM]->vS[nS]->dF )))
	{
		m_vM[nM]->vS[nS]->pT = NULL;
		return -1;
	}
	
	return 0;
}


INT CTblTex::Delete(INT nM, INT nS)
{
	if(! m_vM[nM]->vS[nS]->pT)
		return -1;
	
	if(	m_vM[nM]->vS[nS]->pT	)
	{
		LPDIRECT3DTEXTURE9	pT = m_vM[nM]->vS[nS]->pT;
		pT->Release();

		m_vM[nM]->vS[nS]->pT = NULL;
	}
	
	return 0;
}


INT CTblTex::Select(void** pOut, INT nM, INT nS)
{
	if( nM < 0 ||
		nS < 0 ||
		nM >= (INT)(m_vM.size()) ||
		nS >= (INT)(m_vM[nM]->vS.size()) )
		return -1;

	DWORD	dColor = 0x00FFFFFF;
	if(!m_vM[nM]->vS[nS]->pT)
		CreateSub(nM, nS, &dColor);
	
	if(!m_vM[nM]->vS[nS]->pT)
		return -1;
	
	*pOut = m_vM[nM]->vS[nS]->pT;
	return 0;
}



INT CTblTex::SelectInfo(void** pOut, INT nM, INT nS)
{
	if( nM < 0 ||
		nS < 0 ||
		nM >= (INT)(m_vM.size()) ||
		nS >= (INT)(m_vM[nM]->vS.size()) )
		return -1;
	
	DWORD	dColor = 0x00FFFFFF;
	if(! m_vM[nM]->vS[nS]->pT)
		CreateSub(nM, nS, &dColor);
	
	if(!m_vM[nM]->vS[nS]->pT)
	{
		return -1;
	}
	
	memcpy(pOut, &(m_vM[nM]->vS[nS]->pI), sizeof(D3DXIMAGE_INFO));
	return 0;
}


INT CTblTex::SelectName(void** pOut, INT nM, INT nS)
{
	if( nM < 0 ||
		nS < 0 ||
		nM >= (INT)(m_vM.size()) ||
		nS >= (INT)(m_vM[nM]->vS.size()) )
		return -1;
	
	DWORD	dColor = 0x00FFFFFF;
	if(! m_vM[nM]->vS[nS]->pT)
		CreateSub(nM, nS, &dColor);
	
	if(!m_vM[nM]->vS[nS]->pT)
		return -1;
	
	strcpy((char*)pOut, m_vM[nM]->vS[nS]->sF);
	return 0;
}



INT	CTblTex::Select(void** pOut, INT* nM/*Out*/, INT* nS/*Out*/, char* sFile)
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



INT CTblTex::Load(char * pcFileName)
{
	FILE * fp = NULL;
	char sLine[512]="\0";
	
	fp = fopen(pcFileName, "rt");
	
	if(NULL == fp)
		return -1;



	char* sCmdTx[] =
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
	
#define	READ_TEXT_LINE(index)	\
	if(!strncmp(sLine, sCmdTx[index], strlen(sCmdTx[index])))
	
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
							sscanf(sLine, "%*s %*s %X %s"
								,	&m_vM[nM]->vS[nS]->dF
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


void CTblTex::Confirm()
{
	INT i, j;
	FILE * fp;


	INT iM = (INT)m_vM.size();
	INT iS = 0;



	char* sCmdTx[] =
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
	
	fp = fopen("Data/TxConf.dat", "wt");
	
	fprintf(fp, "////////////////////////////////////////////////////\n//\n\n");
	fprintf(fp, "%s	%s\n\n", sCmdTx[0], m_sV);
	
	fprintf(fp, "%s\n",	sCmdTx[1]);
	fprintf(fp, "{\n");
	fprintf(fp, "	%s	%d\n", sCmdTx[2], iM);
	fprintf(fp, "\n");




	for(i=0 ; i< iM; ++i)
	{
		TbM* pM = m_vM[i];

		iS = (INT)pM->vS.size();


		fprintf(fp, "	%s\n", sCmdTx[3]);
		fprintf(fp, "	{\n");
		fprintf(fp, "		%s	%d\n", sCmdTx[4], i);
		fprintf(fp, "		%s	%d\n", sCmdTx[5], iS);
		fprintf(fp, "		%s	%s\n", sCmdTx[6], pM->sP);
		fprintf(fp, "\n");

		
		for(j=0 ; j< iS; ++j)
		{
			TbS* pS = pM->vS[j];
			
			fprintf(fp, "		%s	%d	0x%x	%s\n",sCmdTx[7], j,	pS->dF,	pS->sF);
		}
	
		fprintf(fp, "	}\n\n");
	}

	fprintf(fp, "}");
	
	fclose(fp);
}



