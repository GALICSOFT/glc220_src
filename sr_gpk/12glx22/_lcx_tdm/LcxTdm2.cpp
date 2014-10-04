// Implementation of the CLcxTdmT class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <string>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tinyxml.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
  #include "ILcxTdm.h"
#else
  #include "../LcxEntity.h"
#endif


#include "LcxTdm.h"

#define MAX_PARSE_LINE	512



// Sprite Record
struct TPrsTdmRec
{
	std::string	sTda;			// Tda Name
	std::string	sSpt;			// Sprite Name
	INT			Time;			// Animation Name

	TPrsTdmRec() : Time(-1){}
	TPrsTdmRec(INT _Time, char* tda, char* spt) : Time(_Time), sTda(tda), sSpt(spt){}

	~TPrsTdmRec()
	{
		Destroy();
	}

	void Destroy()
	{
		Time	= -1;			// Animation Name
		sTda.clear();
		sSpt.clear();
	}
};

// Sprite List
struct TPrsTdmAni
{
	std::string			sName;	// Animation Name
	std::string			sNext;	// Next Animation after animation finished.
	INT					nRpt;	// Repeat
	vector<TPrsTdmRec*>	vRec;	// Sprite Frame Number

	TPrsTdmAni() : nRpt(0){}
	TPrsTdmAni(char* s) : nRpt(0), sName(s){}
	TPrsTdmAni(char* _name, char* _next, INT _rpt) : nRpt(_rpt), sName(_name), sNext(_next){}

	~TPrsTdmAni()
	{
		Destroy();
	}

	void Destroy()
	{
		nRpt =  0;
		sName.clear();
		sNext.clear();

		vRec.clear();
	}
};

typedef vector<TPrsTdmAni*>		lsTsptani;


struct TPrsSptInf
{
	INT			nVer[2]	;					// Version
	lsTsptani	vAni	;

	TPrsSptInf()
	{
		memset(nVer, 0, sizeof(nVer));
	}

	~TPrsSptInf()
	{
		Destroy();
	}

	void Destroy()
	{
		INT iSize = (INT)vAni.size();
		INT n=0;

		for(n=0; n<iSize; ++n)
			delete vAni[n];

		vAni.clear();

		memset(nVer, 0, sizeof(nVer));
	}
};


INT CLcxTdm::LoadFromTxt(char* sFile)
{
	enum Eparse
	{
		ELCXM_REC	= 0	,
		ELCXM_SPTVER	,
		ELCXM_SPTANI	,
		ELCXM_REPEAT	,
		ELCXM_NEXT		,
	};

	// Parsing Keyword
	static struct TsptPrs
	{
		char*	key;
		INT		len;
	} Keywords[] = {
		{	"Rec*"		, 	strlen("Rec*"	)	},	// ELCXM_REC
		{	"SptVer"	,	strlen("SptVer"	)	},	// ELCXM_SPTVER
		{	"SptAni"	,	strlen("SptAni"	)	},	// ELCXM_SPTANI
		{	"repeat"	,	strlen("repeat"	)	},	// ELCXM_REPEAT
		{	"next"		,	strlen("next"	)	},	// ELCXM_NEXT
	};

	INT hr = 0;

	FILE* fp = fopen(sFile, "rt");
	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;


	TPrsSptInf	SrcInf;
	INT			nSrcAni = 0;
	TPrsTdmAni*	pSrcAni = NULL;

	INT			nSrcRec = 0;
	TPrsTdmRec*	pSrcRec = NULL;


	char		sPathSrc[LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sPathSrc, sFile, NULL, NULL, "", "");


	// SceneÆÄ½Ì
	char	sLine[MAX_PARSE_LINE];
	char*	nRead = NULL;

	while(!feof(fp))
	{
		nRead = fgets(sLine, MAX_PARSE_LINE, fp);
		if(NULL == nRead)	// error or EOF
			break;

		LcStr_Trim(sLine);
		if('}' == sLine[0])
			break;


		// Read the version
		if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXM_SPTVER].key, Keywords[ELCXM_SPTVER].len) )
		{
			sscanf(sLine, "%*s %d.%d", &SrcInf.nVer[0], &SrcInf.nVer[1]);
		}

		// Read the sprite animation
		else if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXM_SPTANI].key, Keywords[ELCXM_SPTANI].len) )
		{
			char	sAni[LCXM_ANINAME]={0};
			sscanf(sLine, "%*s %s", sAni);

			SrcInf.vAni.push_back(new TPrsTdmAni(sAni));

			nSrcAni = SrcInf.vAni.size() -1;
			pSrcAni = SrcInf.vAni[nSrcAni];

			nSrcRec = 0;
			pSrcRec = NULL;

			while(!feof(fp))
			{
				nRead = fgets(sLine, MAX_PARSE_LINE, fp);
				if(NULL == nRead)	// error or EOF
					break;

				LcStr_Trim(sLine);
				if('}' == sLine[0])
					break;


				// read repeat
				if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXM_REPEAT].key, Keywords[ELCXM_REPEAT].len) )
				{
					INT rpt=0;

					sscanf(sLine, "%*s %d", &rpt);
					pSrcAni->nRpt = rpt;
				}

				// read next
				else if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXM_NEXT].key, Keywords[ELCXM_NEXT].len) )
				{
					char	sNext[LCXM_ANINAME]={0};
					sscanf(sLine, "%*s %s", sNext);
					pSrcAni->sNext = sNext;
				}

				// read record
				else if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXM_REC].key, Keywords[ELCXM_REC].len) )
				{
					INT		nTime = 0;
					char	tda[LCXM_ANINAME]={0};
					char	spt[LCXM_ANINAME]={0};

					sscanf(sLine, "%*s %d %s %s", &nTime, tda, spt);

					pSrcAni->vRec.push_back(new TPrsTdmRec(nTime, tda, spt) );
					nSrcRec = pSrcAni->vRec.size() -1;
					pSrcRec = pSrcAni->vRec[nSrcRec];

				}
			} // while(!feof(fp))

		}
	}

	fclose(fp);


	// build member data

	m_pInf->nVer[0]	= 0x4B435047;					// 0x4B435047;	// "GPACK"
	m_pInf->nVer[1]	= LCXM_SRC_TDM | 0x01010000;	// VERSION + TDM File

	m_pInf->nAni	= (INT)SrcInf.vAni.size();
	m_pInf->vAni	= new TsptAni[m_pInf->nAni];


	TsptAni*	pDstAni = NULL;
	TsptRec*	pDstRec = NULL;
	INT			n=0, j=0;

	for(n=0; n<m_pInf->nAni; ++n)
	{
		pDstAni = &m_pInf->vAni[n];
		pSrcAni = SrcInf.vAni[n];


		strcpy(pDstAni->sName, pSrcAni->sName.c_str() );
		pDstAni->nRpt	= pSrcAni->nRpt;
		pDstAni->nRec	= (INT)pSrcAni->vRec.size();
		pDstAni->vRec	= new TsptRec[pDstAni->nRec];

		for(j=0; j<pDstAni->nRec; ++j)
		{
			pDstRec = &pDstAni->vRec[j];
			pSrcRec = pSrcAni->vRec[j];

			PLCX_TDA	pTda = NULL;
			INT			nTsc = -1;
			TLC_TDAREC*	pTsc = NULL;
			char		sPathFull[LC_MAX_PATH2]={0};


			sprintf(sPathFull, "%s%s.tda", sPathSrc, pSrcRec->sTda.c_str() );
			::LcxEnt_CreateTda(NULL, &pTda, sPathFull);


			char* recName = (char*)pSrcRec->sSpt.c_str();
			nTsc = pTda->GetRecIdx(recName );
			pTsc = pTda->GetRec(nTsc);

			pDstRec->Time = pSrcRec->Time;
			pDstRec->pTda = pTda;
			pDstRec->pTsc = pTsc;
		}


		if(1< pSrcAni->sNext.size() )
		{
			for(j=0; j<m_pInf->nAni; ++j)
			{
				if(0== LcStr_Stricmp(SrcInf.vAni[n]->sNext.c_str(), pSrcAni->sNext.c_str() ))
				{
					pDstAni->nNxt = j;
					break;
				}
			}
		}

	}


	// clear temp data
	for(n=0; n< (INT)SrcInf.vAni.size(); ++n)
	{
		pSrcAni = SrcInf.vAni[n];

		for(j=0; j< (INT)pSrcAni->vRec.size(); ++j)
		{
			pSrcRec = pSrcAni->vRec[j];
			delete pSrcRec;
		}
		pSrcAni->vRec.clear();

		delete pSrcAni;
	}

	SrcInf.vAni.clear();



	// save to binary
	char	sBin[LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sBin, sFile, NULL, NULL, NULL, "");
	strcat(sBin, ".gtdm");

	hr = SaveToBin(sBin);

	return hr;
}


////////////////////////////////////////////////////////////////////////////////

/*
<glc_tdm ver="1.0.0">
	<spt_ani name="move" next="" repeat="-1">
		<rec time=  "0" file="boss01" sprite="move_01" />
		<rec time="100" file="boss01" sprite="move_03" />
	</spt_ani>
	<spt_ani name="attack" next="move" repeat="1">
		<rec time=  "0" file="boss01" sprite="attack_01" />
		<rec time="200" file="boss01" sprite="attack_06" />
	</spt_ani>
</glc_tdm>
*/


INT CLcxTdm::LoadFromXML(char* sFile)
{
	TiXmlDocument	XmlDoc;				// create the tiny xml instance
	INT				hr = 0;

	TPrsSptInf		SrcInf;
	INT				nSrcAni = 0;
	TPrsTdmAni*		pSrcAni = NULL;

	INT				nSrcRec = 0;
	TPrsTdmRec*		pSrcRec = NULL;



	if(false == XmlDoc.LoadFile(sFile))	// Load xml file
		return LC_CANNT_FIND_RESOURCE;


	char			sPathSrc[LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sPathSrc, sFile, NULL, NULL, "", "");


	// parse xml
	TiXmlNode* pNodeRoot = NULL;
	TiXmlNode* pNodeAni  = NULL;
	TiXmlNode* pNodeRec  = NULL;

	// get the root node
	pNodeRoot = XmlDoc.FirstChild("glc_tdm");
	if(NULL == pNodeRoot)
	{
		XmlDoc.Clear();
		return LC_CANNT_FIND_RESOURCE;
	}
	// get the version
	char* ver = (char*)pNodeRoot->ToElement()->Attribute("ver");
	//SrcInf.nVer[0],SrcInf.nVer[1];

	pNodeAni = pNodeRoot->FirstChild("spt_ani");

	while(pNodeAni)
	{
		pSrcAni = NULL;

		char* s_name = NULL;
		char* s_next = NULL;
		INT   rpt    = -1;
		
		s_name = (char*)pNodeAni->ToElement()->Attribute("name");
		s_next = (char*)pNodeAni->ToElement()->Attribute("next");
		pNodeAni->ToElement()->Attribute( "repeat", &rpt);


		pSrcAni = new TPrsTdmAni(s_name, s_next, rpt);		// setup the ani info
		SrcInf.vAni.push_back(pSrcAni);

		pNodeRec = pNodeAni->FirstChild("rec");
		while(pNodeRec)
		{
			pSrcRec = NULL;

			char* s_tda = NULL;
			char* s_spt = NULL;
			INT   t_time= 0;

			pNodeRec->ToElement()->Attribute( "time", &t_time);
			s_tda = (char*)pNodeRec->ToElement()->Attribute("file");
			s_spt = (char*)pNodeRec->ToElement()->Attribute("sprite");

			pSrcRec = new TPrsTdmRec(t_time, s_tda, s_spt);
			pSrcAni->vRec.push_back(pSrcRec);

			pNodeRec = pNodeRec->NextSibling();			// next record node
		}

		pNodeAni = pNodeAni->NextSibling();				// next ani node
	}

	XmlDoc.Clear();




	// build member data

	m_pInf->nVer[0]	= 0x4B435047;					// 0x4B435047;	// "GPACK"
	m_pInf->nVer[1]	= LCXM_SRC_TDM | 0x01010000;	// VERSION + TDM File

	m_pInf->nAni	= (INT)SrcInf.vAni.size();
	m_pInf->vAni	= new TsptAni[m_pInf->nAni];


	TsptAni*	pDstAni = NULL;
	TsptRec*	pDstRec = NULL;
	INT			n=0, j=0;

	for(n=0; n<m_pInf->nAni; ++n)
	{
		pDstAni = &m_pInf->vAni[n];
		pSrcAni = SrcInf.vAni[n];


		strcpy(pDstAni->sName, pSrcAni->sName.c_str() );
		pDstAni->nRpt	= pSrcAni->nRpt;
		pDstAni->nRec	= (INT)pSrcAni->vRec.size();
		pDstAni->vRec	= new TsptRec[pDstAni->nRec];

		for(j=0; j<pDstAni->nRec; ++j)
		{
			pDstRec = &pDstAni->vRec[j];
			pSrcRec = pSrcAni->vRec[j];

			PLCX_TDA	pTda = NULL;
			INT			nTsc = -1;
			TLC_TDAREC*	pTsc = NULL;
			char		sPathFull[LC_MAX_PATH2]={0};


			sprintf(sPathFull, "%s%s.tda", sPathSrc, pSrcRec->sTda.c_str() );
			::LcxEnt_CreateTda(NULL, &pTda, sPathFull);


			char* recName = (char*)pSrcRec->sSpt.c_str();
			nTsc = pTda->GetRecIdx(recName);
			pTsc = pTda->GetRec(nTsc);

			pDstRec->Time = pSrcRec->Time;
			pDstRec->pTda = pTda;
			pDstRec->pTsc = pTsc;
		}


		if(1< pSrcAni->sNext.size() )
		{
			for(j=0; j<m_pInf->nAni; ++j)
			{
				if(0== LcStr_Stricmp(SrcInf.vAni[n]->sNext.c_str(), pSrcAni->sNext.c_str() ))
				{
					pDstAni->nNxt = j;
					break;
				}
			}
		}

	}


	// clear temp data
	for(n=0; n< (INT)SrcInf.vAni.size(); ++n)
	{
		pSrcAni = SrcInf.vAni[n];

		for(j=0; j< (INT)pSrcAni->vRec.size(); ++j)
		{
			pSrcRec = pSrcAni->vRec[j];
			delete pSrcRec;
		}
		pSrcAni->vRec.clear();

		delete pSrcAni;
	}

	SrcInf.vAni.clear();




	// save to binary
	char	sBin[LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sBin, sFile, NULL, NULL, NULL, "");
	strcat(sBin, ".gtdm");

	hr = SaveToBin(sBin);

	return hr;
}


////////////////////////////////////////////////////////////////////////////////


INT CLcxTdm::LoadFromBin(char* sFile)
{
	FILE*		fp = NULL;
	INT			n=0, j=0;

	TsptAni*	pAni = NULL;
	TsptRec*	pRec = NULL;


	fp = fopen(sFile, "rb");
	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;


	char		sPathSrc[LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sPathSrc, sFile, NULL, NULL, "", "");


	fread(m_pInf->nVer , sizeof(INT ), 2, fp);
	fread(&m_pInf->nAni, sizeof(INT ), 1, fp);

	if(0>= m_pInf->nAni)
	{
		fclose(fp);
		return LC_CANNT_FIND_RESOURCE;
	}

	m_pInf->vAni	= new TsptAni[m_pInf->nAni];

	fseek(fp, LC_RSC_HEADER, SEEK_SET);						// seek offset


	// read animation list
	for(n=0; n<m_pInf->nAni; ++n)
	{
		pAni = &m_pInf->vAni[n];

		fread(pAni->sName, 1, LCXM_ANINAME, fp);
		fread(&pAni->nRpt, sizeof(INT ), 1, fp);
		fread(&pAni->nRec, sizeof(INT ), 1, fp);
		fread(&pAni->nNxt, sizeof(INT ), 1, fp);

		if( 0>= pAni->nRec)
			continue;

		pAni->vRec	= new TsptRec[pAni->nRec];

		for(j=0; j<pAni->nRec; ++j)
		{
			pRec = &pAni->vRec[j];

			PLCX_TDA	pTda = NULL;
			INT			nTsc = -1;
			TLC_TDAREC*	pTsc = NULL;
			char		sPath    [LCXM_TEXNAME]={0};
			char		sPathFull[LC_MAX_PATH2]={0};

			fread(&pRec->Time, sizeof(INT ), 1, fp);
			fread(sPath      , 1, LCXM_TEXNAME, fp);
			fread(&nTsc      , sizeof(INT ), 1, fp);


			sprintf(sPathFull, "%s%s.tda", sPathSrc, sPath);
			::LcxEnt_CreateTda(NULL, &pTda, sPathFull);
			pTda->SetAttrib(LCXM_EXEC_TEXTURE_LOAD, NULL);


			pRec->pTda = pTda;
			pRec->pTsc = pTda->GetRec(nTsc);
		}
	}

	fclose(fp);

	return LC_OK;
}


INT CLcxTdm::LoadFromMemory(void* pMem, void* pSize)
{
	char*	bgn  = (char*)pMem;
	char*	Mem  = bgn;

	INT			n=0, j=0;

	TsptAni*	pAni = NULL;
	TsptRec*	pRec = NULL;

	memcpy(m_pInf->nVer , Mem, sizeof(INT) * 2); Mem += sizeof(INT) * 2;
	memcpy(&m_pInf->nAni, Mem, sizeof(INT) * 1); Mem += sizeof(INT) * 1;

	if(0>= m_pInf->nAni)
		return LC_CANNT_ALLOC;


	char*		sId	  = m_pInf->sId + sizeof(INT);
	char		sPathSrc[LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sPathSrc, sId, NULL, NULL, "", "");



	Mem = bgn + LC_RSC_HEADER;									// seek offset

	m_pInf->vAni	= new TsptAni[m_pInf->nAni];

	// read animation list
	for(n=0; n<m_pInf->nAni; ++n)
	{
		pAni = &m_pInf->vAni[n];

		memcpy(pAni->sName, Mem, LCXM_ANINAME); Mem += LCXM_ANINAME;
		memcpy(&pAni->nRpt, Mem, sizeof(INT )); Mem += sizeof(INT );
		memcpy(&pAni->nRec, Mem, sizeof(INT )); Mem += sizeof(INT );
		memcpy(&pAni->nNxt, Mem, sizeof(INT )); Mem += sizeof(INT );

		if( 0>= pAni->nRec)
			continue;

		pAni->vRec	= new TsptRec[pAni->nRec];

		for(j=0; j<pAni->nRec; ++j)
		{
			pRec = &pAni->vRec[j];

			PLCX_TDA	pTda = NULL;
			INT			nTsc = -1;
			TLC_TDAREC*	pTsc = NULL;
			char		sPath    [LCXM_TEXNAME]={0};
			char		sPathFull[LC_MAX_PATH2]={0};

			memcpy(&pRec->Time, Mem, sizeof(INT )); Mem += sizeof(INT );
			memcpy(sPath      , Mem, LCXM_TEXNAME); Mem += LCXM_ANINAME;
			memcpy(&nTsc      , Mem, sizeof(INT )); Mem += sizeof(INT );


			sprintf(sPathFull, "%s%s.tda", sPathSrc, sPath);
			::LcxEnt_CreateTda(NULL, &pTda, sPathFull);
			pTda->SetAttrib(LCXM_EXEC_TEXTURE_LOAD, NULL);


			pRec->pTda = pTda;
			pRec->pTsc = pTda->GetRec(nTsc);
		}
	}


	return LC_OK;
}


INT CLcxTdm::SaveToBin(char* sFile)
{
	FILE*		fp = NULL;
	INT			n=0, j=0, k=0;

	TsptAni*	pAni = NULL;
	TsptRec*	pRec = NULL;

	if(0>= m_pInf->nAni)
		return LC_CANNT_FIND_RESOURCE;


	fp = fopen(sFile, "wb");
	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;

	fwrite(m_pInf->nVer , sizeof(INT ), 2, fp);
	fwrite(&m_pInf->nAni, sizeof(INT ), 1, fp);

	fseek(fp, LC_RSC_HEADER, SEEK_SET);						// seek offset


	// write animation list
	for(n=0; n<m_pInf->nAni; ++n)
	{
		pAni = &m_pInf->vAni[n];

		fwrite(pAni->sName, 1, LCXM_ANINAME, fp);
		fwrite(&pAni->nRpt, sizeof(INT ), 1, fp);
		fwrite(&pAni->nRec, sizeof(INT ), 1, fp);
		fwrite(&pAni->nNxt, sizeof(INT ), 1, fp);

		if( 0>= pAni->nRec)
			continue;


		for(j=0; j<pAni->nRec; ++j)
		{
			pRec = &pAni->vRec[j];

			PLCX_TDA	pTda = NULL;
			TLC_TDAREC*	pTsc = NULL;
			char		sPath    [LC_MAX_PATH2]={0};
			char		sPathFull[LC_MAX_PATH2]={0};
			INT			nTsc = -1;

			pTda = pRec->pTda;
			pTsc = pRec->pTsc;
			nTsc = FindRecIdx(pTda, pTsc);

			pTda->GetAttrib(LCXM_SRC_ID, sPathFull);
			LcStr_ReplacePath(sPath, sPathFull, "", "", NULL, NULL);


			fwrite(&pRec->Time, sizeof(INT ), 1, fp);
			fwrite(sPath      , 1, LCXM_TEXNAME, fp);
			fwrite(&nTsc      , sizeof(INT ), 1, fp);
		}
	}

	fclose(fp);

	return LC_OK;
}



INT CLcxTdm::FindRecIdx(PLCX_TDA pTda, TLC_TDAREC* pTsc)
{
	INT maxRec = pTda->GetRecIdx(NULL);

	for(INT k=0; k<maxRec; ++k)
	{
		if(pTsc == pTda->GetRec(k))
			return k;
	}

	return -1;
}




////////////////////////////////////////////////////////////////////////////////

vector<PLCX_TDM>	CLcxTdm::g_lstTdm;


PLCX_TDM ILCX_Tdm::Find(INT nIdx)
{
	if( 0 > nIdx || nIdx >=(INT)CLcxTdm::g_lstTdm.size())
		return NULL;

	return CLcxTdm::g_lstTdm[nIdx];
}


INT ILCX_Tdm::FindIdx(char* sFile)
{
	CLcxTdm* pLst = NULL;
	INT iSize = (INT)CLcxTdm::g_lstTdm.size();

	if(NULL==sFile)
		return iSize;


	for(INT n=0; n<iSize; ++n)
	{
		pLst = (CLcxTdm*)CLcxTdm::g_lstTdm[n];

		INT   len  = *((INT*)pLst->m_pInf->sId);
		char* name = pLst->m_pInf->sId + sizeof(INT);

		if(0 == LcStr_Strncmp(sFile, name, len))
			return n;
	}

	return -1;
}


INT ILCX_Tdm::ListRelease()
{
	INT iSize = (INT)CLcxTdm::g_lstTdm.size();

	for(INT n=0; n<iSize; ++n)
	{
		if(NULL == CLcxTdm::g_lstTdm[n])
			continue;

		CLcxTdm::g_lstTdm[n]->Release();
	}

	CLcxTdm::g_lstTdm.clear();

	return LC_OK;
}


////////////////////////////////////////////////////////////////////////////////
INT CLcxTdm::PopFromList(PLCX_TDM pTdm)
{
	INT iSize = (INT)CLcxTdm::g_lstTdm.size();

	for(INT n=0; n<iSize; ++n)
	{
		if(pTdm != CLcxTdm::g_lstTdm[n] || NULL == CLcxTdm::g_lstTdm[n])
			continue;

		CLcxTdm::g_lstTdm[n] = NULL;
		break;
	}

	return LC_OK;
}


INT CLcxTdm::Add(CLcxTdm* pTdm)
{
	INT n = 0;
	INT iSize = (INT)CLcxTdm::g_lstTdm.size();


	char* sId = pTdm->m_pInf->sId;
	if(LC_SUCCEEDED(ILCX_Tdm::FindIdx(sId)))
		return LC_EFAIL;


	for(n=0; n<iSize; ++n)
	{
		if(NULL == CLcxTdm::g_lstTdm[n])
			break;
	}


	if(n == iSize)
		CLcxTdm::g_lstTdm.push_back(pTdm);
	else
		CLcxTdm::g_lstTdm[n] = pTdm;

	return LC_OK;
}

