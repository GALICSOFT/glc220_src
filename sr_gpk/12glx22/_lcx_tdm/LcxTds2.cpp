// Implementation of the CLcxTds class.
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
#include "LcxTds.h"

#define MAX_PARSE_LINE	512


// Sprite List
struct TPrsTdsPts
{
	string			sName;	// base name
	vector<string>	vRec;	// record name list

	TPrsTdsPts()
	{
	}

	TPrsTdsPts(char* s)
	{
		sName = s;
	}

	~TPrsTdsPts()
	{
		Destroy();
	}

	void Destroy()
	{
		sName.clear();
		vRec.clear();
	}
};

struct TPrsSptInf
{
	INT						nVer[2]	;		// Version
	vector<TPrsTdsPts*>		vPts	;

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
		for(INT n=0; n< (INT)vPts.size(); ++n)
			delete vPts[n];

		vPts.clear();

		memset(nVer, 0, sizeof(nVer));
	}
};


INT CLcxTds::LoadFromTxt(char* sFile)
{
	enum Eparse
	{
		ELCXM_REC	= 0	,
		ELCXM_PTSVER	,
		ELCXM_PTSLST	,
	};

	// Parsing Keyword
	static struct TsptPrs
	{
		char*	key;
		INT		len;
	} Keywords[] = {
		{	"Rec*"		, 	strlen("Rec*"	)	},	// ELCXM_REC
		{	"PtsVer"	,	strlen("PtsVer"	)	},	// ELCXM_PTSVER
		{	"PtsLst"	,	strlen("PtsLst"	)	},	// ELCXM_PTSLST
	};



	INT			hr = 0;

	FILE*		fp = fopen(sFile, "rt");
	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;


	char		sPathSrc[LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sPathSrc, sFile, NULL, NULL, "", "");


	TPrsSptInf	SrcInf;
	INT			nSrcPts = 0;
	TPrsTdsPts*	pSrcPts = NULL;


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
		if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXM_PTSVER].key, Keywords[ELCXM_PTSVER].len) )
		{
			sscanf(sLine, "%*s %d.%d", &SrcInf.nVer[0], &SrcInf.nVer[1]);
		}

		// Read the sprite animation
		else if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXM_PTSLST].key, Keywords[ELCXM_PTSLST].len) )
		{
			char	s_tdm[LCXM_TEXNAME]={0};
			sscanf(sLine, "%*s %s", s_tdm);

			SrcInf.vPts.push_back(new TPrsTdsPts(s_tdm));

			nSrcPts = SrcInf.vPts.size() -1;
			pSrcPts = SrcInf.vPts[nSrcPts];

			while(!feof(fp))
			{
				nRead = fgets(sLine, MAX_PARSE_LINE, fp);
				if(NULL == nRead)	// error or EOF
					break;

				LcStr_Trim(sLine);
				if('}' == sLine[0])
					break;

				// read record
				if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXM_REC].key, Keywords[ELCXM_REC].len) )
				{
					char s_tdm[LCXM_TEXNAME]={0};
					sscanf(sLine, "%*s %s", s_tdm);

					pSrcPts->vRec.push_back(s_tdm);
				}
			} // while(!feof(fp))

		}
	}

	fclose(fp);


	// build member data

	m_pInf->nVer[0]	= 0x4B435047;					// 0x4B435047;	// "GPACK"
	m_pInf->nVer[1]	= LCXM_SRC_TDS | 0x01010000;	// VERSION + TDS File

	m_pInf->nPts	= (INT)SrcInf.vPts.size();
	m_pInf->vPts	= new TdsPts[m_pInf->nPts];


	TdsPts*		pDstPts = NULL;
	INT			n=0, j=0;

	for(n=0; n<m_pInf->nPts; ++n)
	{
		pDstPts = &m_pInf->vPts[n];
		pSrcPts = SrcInf.vPts[n];

		pDstPts->nRec	= (INT)pSrcPts->vRec.size();
		pDstPts->vRec	= new PLCX_TDM[pDstPts->nRec];

		for(j=0; j<pDstPts->nRec; ++j)
		{
			char* pSrcRec = (char*)pSrcPts->vRec[j].c_str();

			PLCX_TDM	pTdm = NULL;
			char		sPathFull[LC_MAX_PATH2]={0};

			sprintf(sPathFull, "%s%s.tdm", sPathSrc, pSrcRec);
			hr = ::LcxEnt_CreateTdm(NULL, &pTdm, sPathFull, m_pSpl);

			pDstPts->vRec[j] = pTdm;
		}
	}


	// clear temp data
	for(n=0; n< (INT)SrcInf.vPts.size(); ++n)
		pSrcPts->vRec.clear();

	SrcInf.vPts.clear();


	// save to binary
	char	sBin[LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sBin, sFile, NULL, NULL, NULL, "");
	strcat(sBin, ".gtds");

	hr = SaveToBin(sBin);

	return hr;
}


////////////////////////////////////////////////////////////////////////////////

/*
<glc_tds ver="1.0.0" name="boss01">
	<glc_pts name="boss01_leg">
	</glc_pts>

	<glc_pts  name="boss01_head">
	</glc_pts>

	<glc_pts name="boss01_torso">
		<!--
		<rec name="boss01_shield">
		<rec name="boss01_sword">
		-->

	</glc_pts>
</glc_tds>
*/


INT CLcxTds::LoadFromXML(char* sFile)
{
	TiXmlDocument	XmlDoc;				// create the tiny xml instance
	INT				hr = 0;

	TPrsSptInf		SrcInf;
	INT				nSrcPts = 0;
	TPrsTdsPts*		pSrcPts = NULL;


	if(false == XmlDoc.LoadFile(sFile))	// Load xml file
		return LC_CANNT_FIND_RESOURCE;


	char			sPathSrc[LC_MAX_PATH]={0};
	LcStr_ReplacePath(sPathSrc, sFile, NULL, NULL, "", "");


	// parse xml
	TiXmlNode* pNodeRoot = NULL;
	TiXmlNode* pNodePts  = NULL;
	TiXmlNode* pNodeRec  = NULL;

	// get the root node
	pNodeRoot = XmlDoc.FirstChild("glc_tds");
	if(NULL == pNodeRoot)
	{
		XmlDoc.Clear();
		return LC_CANNT_FIND_RESOURCE;
	}

	// get the version
	char* ver = (char*)pNodeRoot->ToElement()->Attribute("ver");


	pNodePts = pNodeRoot->FirstChild("glc_pts");
	while(pNodePts)
	{
		char* s_tdm = (char*)pNodePts->ToElement()->Attribute("name");

		SrcInf.vPts.push_back(new TPrsTdsPts(s_tdm));

		nSrcPts = SrcInf.vPts.size() -1;
		pSrcPts = SrcInf.vPts[nSrcPts];


		pNodeRec = pNodePts->FirstChild("rec");
		while(pNodeRec)
		{
			char* s_tdm = (char*)pNodeRec->ToElement()->Attribute("name");

			pSrcPts->vRec.push_back(s_tdm);

			pNodeRec = pNodeRec->NextSibling();			// next record node
		}

		pNodePts = pNodePts->NextSibling();				// next ani node
	}

	XmlDoc.Clear();




	// build member data

	m_pInf->nVer[0]	= 0x4B435047;					// 0x4B435047;	// "GPACK"
	m_pInf->nVer[1]	= LCXM_SRC_TDS | 0x01010000;	// VERSION + TDS File

	m_pInf->nPts	= (INT)SrcInf.vPts.size();
	m_pInf->vPts	= new TdsPts[m_pInf->nPts];


	TdsPts*		pDstPts = NULL;
	INT			n=0, j=0;

	for(n=0; n<m_pInf->nPts; ++n)
	{
		pDstPts = &m_pInf->vPts[n];
		pSrcPts = SrcInf.vPts[n];


		// Load
		PLCX_TDM	pTdm = NULL;
		char		sPathFull[LC_MAX_PATH2]={0};


		sprintf(sPathFull, "%s%s.tdm", sPathSrc, pSrcPts->sName.c_str());
		hr = ::LcxEnt_CreateTdm(NULL, &pTdm, sPathFull, m_pSpl);


		pDstPts->pTdm	= pTdm;
		pDstPts->nRec	= (INT)pSrcPts->vRec.size();

		if(0>=pDstPts->nRec)
			continue;


		pDstPts->vRec	= new PLCX_TDM[pDstPts->nRec];

		for(j=0; j<pDstPts->nRec; ++j)
		{
			PLCX_TDM	pTdm = NULL;
			char		sName    [LC_MAX_PATH2]={0};
			char		sPathFull[LC_MAX_PATH2]={0};

			strcpy(sName, (char*)pSrcPts->vRec[j].c_str());
			sprintf(sPathFull, "%s%s.tdm", sPathSrc, sName);
			hr = ::LcxEnt_CreateTdm(NULL, &pTdm, sPathFull, m_pSpl);


			pDstPts->vRec[j] = pTdm;
		}
	}


	// clear temp data
	for(n=0; n< (INT)SrcInf.vPts.size(); ++n)
	{
		pSrcPts = SrcInf.vPts[n];

		pSrcPts->vRec.clear();
		delete pSrcPts;
	}

	SrcInf.vPts.clear();




	// save to binary
	char	sBin[LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sBin, sFile, NULL, NULL, NULL, "");
	strcat(sBin, ".gtds");

	hr = SaveToBin(sBin);

	return hr;
}


////////////////////////////////////////////////////////////////////////////////


INT CLcxTds::LoadFromBin(char* sFile)
{
	FILE*		fp = NULL;
	INT			n=0, j=0;
	INT			hr = 0;


	fp = fopen(sFile, "rb");
	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;


	char		sPathSrc[LC_MAX_PATH]={0};
	LcStr_ReplacePath(sPathSrc, sFile, NULL, NULL, "", "");


	fread(m_pInf->nVer , sizeof(INT ), 2, fp);
	fread(&m_pInf->nPts, sizeof(INT ), 1, fp);

	if(0>= m_pInf->nPts)
	{
		fclose(fp);
		return LC_CANNT_FIND_RESOURCE;
	}

	m_pInf->vPts	= new TdsPts[m_pInf->nPts];

	fseek(fp, LC_RSC_HEADER, SEEK_SET);						// seek offset


	// read animation list
	for(n=0; n<m_pInf->nPts; ++n)
	{
		TdsPts*	pPts = &m_pInf->vPts[n];


		char	sName[LC_MAX_PATH2]={0};


		fread(      sName, 1, LCXM_ANINAME, fp);
		fread(&pPts->nRec, sizeof(INT ), 1, fp);



		// Load
		PLCX_TDM	pTdmL = NULL;
		char		sPathFull[LC_MAX_PATH2]={0};


		sprintf(sPathFull, "%s%s.tdm", sPathSrc, sName);
		hr = ::LcxEnt_CreateTdm(NULL, &pTdmL, sPathFull, m_pSpl);


		this->Attach(pTdmL);
		pPts->pTdm	= pTdmL;


		if( 0>= pPts->nRec)
			continue;


		pPts->vRec	= new PLCX_TDM[pPts->nRec];

		for(j=0; j<pPts->nRec; ++j)
		{

			PLCX_TDM	pTdmS = NULL;
			char		sName    [LC_MAX_PATH2]={0};
			char		sPathFull[LC_MAX_PATH2]={0};

			fread(sName, 1, LCXM_TEXNAME, fp);


			sprintf(sPathFull, "%s%s.tdm", sPathSrc, sName);
			hr = ::LcxEnt_CreateTdm(NULL, &pTdmS, sPathFull, m_pSpl);

			this->Attach(pTdmS);
			((CLcxTdm*)pTdmS)->SetTsp(pTdmL);
			pPts->vRec[j] = pTdmS;
		}
	}

	fclose(fp);

	return LC_OK;
}


INT CLcxTds::LoadFromMemory(void* pMem, void* pSize)
{
	char*	bgn  = (char*)pMem;
	char*	Mem  = bgn;

	INT		n=0, j=0;
	INT		hr = 0;


	memcpy(m_pInf->nVer , Mem, sizeof(INT) * 2); Mem += sizeof(INT) * 2;
	memcpy(&m_pInf->nPts, Mem, sizeof(INT) * 1); Mem += sizeof(INT) * 1;

	if(0>= m_pInf->nPts)
		return LC_CANNT_ALLOC;


	char*		sId	  = m_pInf->sId + sizeof(INT);
	char		sPathSrc[LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sPathSrc, sId, NULL, NULL, "", "");


	Mem = bgn + LC_RSC_HEADER;									// seek offset

	m_pInf->vPts	= new TdsPts[m_pInf->nPts];

	// read animation list
	for(n=0; n<m_pInf->nPts; ++n)
	{
		TdsPts*	pPts = &m_pInf->vPts[n];


		char	sName[LCXM_TEXNAME]={0};


		memcpy(      sName, Mem, LCXM_ANINAME); Mem += LCXM_ANINAME;
		memcpy(&pPts->nRec, Mem, sizeof(INT )); Mem += sizeof(INT );


		// Load
		PLCX_TDM	pTdmL = NULL;
		char		sPathFull[LC_MAX_PATH2]={0};


		sprintf(sPathFull, "%s%s.tdm", sPathSrc, sName);
		hr = ::LcxEnt_CreateTdm(NULL, &pTdmL, sPathFull, m_pSpl);


		this->Attach(pTdmL);
		pPts->pTdm	= pTdmL;


		if( 0>= pPts->nRec)
			continue;


		pPts->vRec	= new PLCX_TDM[pPts->nRec];

		for(j=0; j<pPts->nRec; ++j)
		{
			PLCX_TDM	pTdmS = NULL;
			char		sName    [LC_MAX_PATH2]={0};
			char		sPathFull[LC_MAX_PATH2]={0};

			memcpy(sName, Mem, LCXM_TEXNAME); Mem += LCXM_ANINAME;


			sprintf(sPathFull, "%s%s.tdm", sPathSrc, sName);
			hr = ::LcxEnt_CreateTdm(NULL, &pTdmS, sPathFull, m_pSpl);

			this->Attach(pTdmS);
			((CLcxTdm*)pTdmS)->SetTsp(pTdmL);
			pPts->vRec[j] = pTdmS;
		}
	}


	return LC_OK;
}


INT CLcxTds::SaveToBin(char* sFile)
{
	FILE*		fp = NULL;
	INT			n=0, j=0, k=0;


	if(0>= m_pInf->nPts)
		return LC_CANNT_FIND_RESOURCE;


	fp = fopen(sFile, "wb");
	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;

	fwrite(m_pInf->nVer , sizeof(INT ), 2, fp);
	fwrite(&m_pInf->nPts, sizeof(INT ), 1, fp);

	fseek(fp, LC_RSC_HEADER, SEEK_SET);						// seek offset


	// write animation list
	for(n=0; n<m_pInf->nPts; ++n)
	{
		TdsPts* pPts = &m_pInf->vPts[n];

		PLCX_TDM	pTdm = pPts->pTdm;
		char		sName    [LC_MAX_PATH2]={0};
		char		sPathFull[LC_MAX_PATH2]={0};


		pTdm->GetAttrib(LCXM_SRC_ID, sPathFull);
		LcStr_ReplacePath(sName, sPathFull, "", "", NULL, "");


		fwrite(sName      , 1, LCXM_ANINAME, fp);
		fwrite(&pPts->nRec, sizeof(INT ), 1, fp);

		if( 0>= pPts->nRec)
			continue;


		for(j=0; j<pPts->nRec; ++j)
		{
			PLCX_TDM	pTdm = pPts->vRec[j];

			char		sName    [LC_MAX_PATH2]={0};
			char		sPathFull[LC_MAX_PATH2]={0};


			pTdm->GetAttrib(LCXM_SRC_ID, sPathFull);
			LcStr_ReplacePath(sName, sPathFull, "", "", NULL, "");


			fwrite(sName      , 1, LCXM_TEXNAME, fp);
		}
	}

	fclose(fp);

	return LC_OK;
}



////////////////////////////////////////////////////////////////////////////////

vector<PLCX_TDS>	CLcxTds::g_lstTds;


PLCX_TDS ILCX_Tds::Find(INT nIdx)
{
	if( 0 > nIdx || nIdx >=(INT)CLcxTds::g_lstTds.size())
		return NULL;

	return CLcxTds::g_lstTds[nIdx];
}


INT ILCX_Tds::FindIdx(char* sFile)
{
	CLcxTds* pLst = NULL;
	INT iSize = (INT)CLcxTds::g_lstTds.size();

	if(NULL==sFile)
		return iSize;


	for(INT n=0; n<iSize; ++n)
	{
		pLst = (CLcxTds*)CLcxTds::g_lstTds[n];

		INT   len  = *((INT*)pLst->m_pInf->sId);
		char* name = pLst->m_pInf->sId + sizeof(INT);

		if(0 == LcStr_Strncmp(sFile, name, len))
			return n;
	}

	return -1;
}


INT ILCX_Tds::ListRelease()
{
	INT iSize = (INT)CLcxTds::g_lstTds.size();

	for(INT n=0; n<iSize; ++n)
	{
		if(NULL == CLcxTds::g_lstTds[n])
			continue;

		CLcxTds::g_lstTds[n]->Release();
	}

	CLcxTds::g_lstTds.clear();

	return LC_OK;
}


////////////////////////////////////////////////////////////////////////////////
INT CLcxTds::PopFromList(PLCX_TDS pTds)
{
	INT iSize = (INT)CLcxTds::g_lstTds.size();

	for(INT n=0; n<iSize; ++n)
	{
		if(pTds != CLcxTds::g_lstTds[n] || NULL == CLcxTds::g_lstTds[n])
			continue;

		CLcxTds::g_lstTds[n] = NULL;
		break;
	}

	return LC_OK;
}


INT CLcxTds::Add(CLcxTds* pTds)
{
	INT n = 0;
	INT iSize = (INT)CLcxTds::g_lstTds.size();


	char* sId = pTds->m_pInf->sId;
	if(LC_SUCCEEDED(ILCX_Tds::FindIdx(sId)))
		return LC_EFAIL;


	for(n=0; n<iSize; ++n)
	{
		if(NULL == CLcxTds::g_lstTds[n])
			break;
	}


	if(n == iSize)
		CLcxTds::g_lstTds.push_back(pTds);
	else
		CLcxTds::g_lstTds[n] = pTds;

	return LC_OK;
}


////////////////////////////////////////////////////////////////////////////////


INT LcxEnt_ReleaseTdmBase()
{
// release the tdm list
	ILCX_Tdm::ListRelease();

	// release the tda list
	ILCX_Tda::ListRelease();
	ILCX_Tds::ListRelease();

	return LC_OK;
}

