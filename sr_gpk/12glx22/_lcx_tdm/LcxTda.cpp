// Implementation of the CLcxTda class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <vector>
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
  #include "ILcxTda.h"
#else
  #include "../LcxEntity.h"
#endif

#include "LcxTda.h"


TLC_TDAREC::TLC_TDAREC()
{
	memset(sName, 0, sizeof(sName));							// Sequence Name
	memset(pos  , 0, sizeof(pos));
	memset(tex  , 0, sizeof(tex));
}

LCXVECTOR2 TLC_TDAREC::GetWH()
{
	FLOAT w = fabsf(pos[2].x - pos[0].x);
	FLOAT h = fabsf(pos[2].x - pos[0].y);

	return LCXVECTOR2(w, h);
}

LCXVECTOR2	TLC_TDAREC::GetXY()
{
	float w = pos[2].x - pos[0].x;
	float h = pos[2].x - pos[0].y;

	LCXVECTOR2 r = pos[0] + LCXVECTOR2(w, h) * 0.5F;
	return r;
}


////////////////////////////////////////////////////////////////////////////////

CLcxTda::CLcxTda()
{
	memset(m_sId, 0, LCXM_TEXNAME);
	memset(m_sTex, 0, LCXM_TEXNAME);

	m_ver[0]	= 0x4B435047;					// 0x4B435047;	// "GPACK"
	m_ver[1]	= LCXM_SRC_TDA | 0x01010000;	// VERSION + TDA File

	m_nLst		= 0;
	m_imgW		= 0;
	m_imgH		= 0;
	m_pRec		= NULL;
	m_pTex		= NULL;
}


CLcxTda::~CLcxTda()
{
	Destroy();
}


INT CLcxTda::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxTda::Release()
{
	INT hr = 0;

	CLcxTda::PopFromList(this);

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcxTda::Destroy()
{
	// release .. just only this is original instance
	SAFE_DELETE_ARRAY(	m_pRec		);		// Frame Name
	SAFE_RELEASE	(	m_pTex		);		// Texture

	m_nLst		= 0;
	m_imgW		= 0;
	m_imgH		= 0;
	m_pRec		= NULL;
	m_pTex		= NULL;
}


INT CLcxTda::CreateFromFile(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT		hr    = 0;
	char*	sFile = (char*)p1;
	char	sBin  [LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sBin, sFile, NULL, NULL, NULL, ".gtda");

#if defined(__ANDROID__)

	BYTE*		buf = NULL;
	ULONG		len = 0;
	const char*	sApkFile = LcSys_AndroidApkFile(NULL);

	hr = Lzip_UnzipbyName(&buf, &len, sApkFile, sBin, LC_TRUE);
	LOGI("Create Tda Unzip: %x %ld %d %s", (UINT)buf, len, hr, sFile);

	if(LC_FAILED(hr))
		return LC_CANNT_ALLOC;

	hr = CreateFromMemory(sBin, buf, &len);
	SAFE_FREE(	buf	);

	return hr;
#endif


	char	sMedia[LC_MAX_PATH2]={0};
	char	ext   [LC_MAX_EXT  ]={0};
	INT		nType = LC_OBJ_UNKNOWN;

	char*	sId	  = m_sId + sizeof(INT);
	*((INT*)m_sId) = LcStr_ReplacePath(sId, sFile, NULL, NULL, NULL, "");



	LcStr_SplitPath(sFile, NULL, NULL, NULL, ext);
	if(strstr(ext, ".tda"))
		nType = LC_OBJ_XML;

	else if(strstr(ext, ".csv"))
		nType = LC_OBJ_CSV;

	else
		goto END;


	if(LC_FAILED(LcSys_ResourcePath(sMedia, sBin)))
	{
#if defined(__APPLE__) || defined(__IPHONE__)
		return LC_CANNT_FIND_RESOURCE;
#endif

		hr = LC_CANNT_ALLOC;

		if     (LC_OBJ_XML == nType)
			hr = LoadFromXML(sFile);				// Load Tda File for xml

		else if(LC_OBJ_CSV == nType)
			hr = LoadFromCsv(sFile);				// Load Tda File for csv


		if(LC_FAILED(hr))
			goto END;


		// release record
		SAFE_DELETE_ARRAY(	m_pRec	);				// Frame Name

		memset(m_sTex, 0, LCXM_TEXNAME);
		m_ver[0] = 0;	m_ver[1] = 0;
		m_nLst   = 0;	m_imgW   = 0;	m_imgH = 0;
	}

	// load from binary
	hr = LoadFromBin(sMedia);
	if(LC_FAILED(hr))
		goto END;


	CLcxTda::Add(this);
	return LC_OK;

END:
	return hr;
}




// PLC_ARGS option
//	p0 : sName	Model File Name
//	p1 : pMem	starting memory pointer
//	p2 : pSize	size(unsigned int) pointer

INT CLcxTda::CreateFromMemory(PLC_ARGS pArg)
{
	if(L_NIL == pArg->p0 || L_NIL == pArg->p1 || L_NIL == pArg->p2)
		return LC_CANNT_ALLOC;


	char*	sFile = (char*  )pArg->p0;
	BYTE*	buf   = (BYTE*  )pArg->p1;
	ULONG	len   = (ULONG  )pArg->p2;

	INT		hr    = 0;


	char*	sId	  = m_sId + sizeof(INT);
	*((INT*)m_sId) = LcStr_ReplacePath(sId, sFile, NULL, NULL, NULL, "");


	hr = LoadFromMemory(buf, &len);
	LOGI("Create Tda From Memory hr: %d", hr);
	if(LC_FAILED(hr))
		return hr;

	CLcxTda::Add(this);

	return hr;
}


INT CLcxTda::SetAttrib(UINT dCmd, LC_HANDLE pData)
{
	INT hr = 0;
	if(LCXM_EXEC_TEXTURE_LOAD == dCmd)
	{
		char sFile[LC_MAX_PATH] ={0};
		char* sPath = (char*)pData;

		if(m_pTex)
			return LC_OK;

		if(sPath)
		{
			INT len;

			strcpy(sFile, sPath);
			len = strlen(sFile);

			if(len)
			{
				if( '/' != sFile[len-1] && '\\' != sFile[len-1])
					strcat(sFile, "/");
			}

			strcat(sFile, this->m_sTex);

			hr = LcDev_CreateTextureFromFile(NULL, &m_pTex, sFile);
		}
		else
		{
			INT len = LcStr_ReplacePath(sFile, m_sId + sizeof(INT)
											, NULL, NULL, "", "");

			if(len)
			{
				if( '/' != sFile[len-1] && '\\' != sFile[len-1])
					strcat(sFile, "/");
			}

			strcat(sFile, this->m_sTex);

			hr = LcDev_CreateTextureFromFile(NULL, &m_pTex, sFile);
		}

		return hr;
	}
	else
		return LC_EFAIL;

	return LC_OK;
}


INT CLcxTda::GetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if(LCXM_TEXTURE_LIST_NAME == dCmd)
	{
		char* str = (char*)pData;
		strcpy(str, this->m_sTex);
	}

	else if(LCXM_SRC_ID == dCmd)
	{
		char* sId = this->m_sId + sizeof(INT);
		char* str = (char*)pData;
		strcpy(str, sId);
	}
	else
		return LC_EFAIL;

	return LC_OK;
}


INT CLcxTda::GetRecIdx(char* recName)
{
	TLC_TDAREC* pRec = NULL;
	INT	n   = -1;

	if(NULL == recName)
		return m_nLst;

	for(n=0; n<m_nLst; ++n)
	{
		pRec = &m_pRec[n];
		if( 0 == LcStr_Strcmp(pRec->sName, recName))
			return n;
	}

	return -1;
}


TLC_TDAREC* CLcxTda::GetRec(char* recName)
{
	INT recIdx = GetRecIdx(recName);

	if(0>recIdx || recIdx >= m_nLst)
		return NULL;

	return &m_pRec[recIdx];
}


TLC_TDAREC* CLcxTda::GetRec(INT recIdx)
{
	if(0>recIdx || recIdx >= m_nLst)
		return NULL;

	return &m_pRec[recIdx];
}


INT CLcxTda::LoadFromBin(char* sFile)
{
	FILE*	fp  = NULL;

	fp = fopen(sFile, "rb");
	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;


	// 1. Reading Header
	fread(  m_ver, sizeof(INT), 2, fp)	;					// Version
	fread(&m_nLst, sizeof(INT), 1, fp)	;					// sprite list
	fread(&m_imgW, sizeof(INT), 1, fp)	;					// Image Width
	fread(&m_imgH, sizeof(INT), 1, fp)	;					// Image Height

	if(0>=m_nLst || 0 >=m_imgW || 0 >= m_imgH)
	{
		fclose(fp);
		return LC_CANNT_FIND_RESOURCE;
	}


	fseek(fp, LC_RSC_HEADER, SEEK_SET);						// seek offset


	// 2. Reading the texture file name
	fread(m_sTex, 1, LCXM_TEXNAME, fp);						// texture file name


	// 3. Reading the Record
	m_pRec	= new TLC_TDAREC[m_nLst];

	fread(m_pRec, sizeof(TLC_TDAREC), m_nLst, fp);			// texture file name

	fclose(fp);

	return LC_OK;
}


INT CLcxTda::LoadFromMemory(void* pMem, void* pSize)
{
	char*	bgn  = (char*)pMem;
	char*	Mem  = bgn;

	// 1. Reading Header
	memcpy(  m_ver, Mem, sizeof(INT)*2); Mem += sizeof(INT)*2;	// Version
	memcpy(&m_nLst, Mem, sizeof(INT)*1); Mem += sizeof(INT)*1;	// sprite list
	memcpy(&m_imgW, Mem, sizeof(INT)*1); Mem += sizeof(INT)*1;	// Image Width
	memcpy(&m_imgH, Mem, sizeof(INT)*1); Mem += sizeof(INT)*1;	// Image Height

	if(0>=m_nLst || 0 >=m_imgW || 0 >= m_imgH)
		return LC_CANNT_FIND_RESOURCE;


	Mem = bgn + LC_RSC_HEADER;									// seek offset


	// 2. Reading the texture file name
	memcpy(m_sTex, Mem, LCXM_TEXNAME); Mem += LCXM_TEXNAME;


	// 3. Reading the Record
	m_pRec	= new TLC_TDAREC[m_nLst];

	memcpy(m_pRec, Mem, sizeof(TLC_TDAREC) * m_nLst); Mem += sizeof(TLC_TDAREC) * m_nLst;	// texture file name

	return LC_OK;
}


//<?xml version="1.0" encoding="UTF-8"?>
//<!-- Created with TexturePacker http://texturepacker.com-->
//<!-- $TexturePacker:SmartUpdate:4bfed9e26bf150d877a729bd0c3ecbdd$ -->
//<!--Format:
//n  => name of the sprite
//x  => sprite x pos in texture
//y  => sprite y pos in texture
//w  => sprite width (may be trimmed)
//h  => sprite height (may be trimmed)
//oX => sprite's x-corner offset (only available if trimmed)
//oY => sprite's y-corner offset (only available if trimmed)
//oW => sprite's original width (only available if trimmed)
//oH => sprite's original height (only available if trimmed)
//r => 'y' only set if sprite is rotated
//-->
//<TextureAtlas imagePath="work_src.png" width="512" height="1024">
//
//    <sprite n="fire128.tga" x="2" y="426" w="128" h="128"/>
//    <sprite n="star128_org.tga" x="312" y="376" w="128" h="128"/>
//    <sprite n="t00.tga" x="160" y="342" w="150" h="140" oX="37" oY="64" oW="256" oH="256"/>
//    <sprite n="t01.tga" x="378" y="2" w="130" h="170" oX="42" oY="67" oW="256" oH="256" r="y"/>
//    <sprite n="t02.tga" x="2" y="358" w="144" h="66" oX="93" oY="57" oW="256" oH="256" r="y"/>
//    <sprite n="t07.tga" x="192" y="2" w="184" h="98" oX="36" oY="79" oW="256" oH="256"/>
//
//</TextureAtlas>
INT CLcxTda::LoadFromXML(char* sFile)
{
	TiXmlDocument	XmlDoc;				// create the tiny xml instance
	TtdaOrg*		pRec = NULL;

	INT		hr = 0;


	if(false == XmlDoc.LoadFile(sFile))	// Load xml file
		return LC_CANNT_FIND_RESOURCE;


	// parse xml
	TiXmlNode* pNodeRoot = NULL;
	TiXmlNode* pNodeSpt  = NULL;

	// get the node of script_app -> stage
	pNodeRoot = XmlDoc.FirstChild("TextureAtlas");

	if(NULL == pNodeRoot)
	{
		XmlDoc.Clear();
		return LC_CANNT_FIND_RESOURCE;
	}


	char* img_file = (char*)pNodeRoot->ToElement()->Attribute("imagePath");
	strcpy(this->m_sTex, img_file);

	pNodeRoot->ToElement()->Attribute("width",  &m_imgW);
	pNodeRoot->ToElement()->Attribute("height", &m_imgH);


	// read the record
	std::vector<TtdaOrg* > vRec;

	pNodeSpt = pNodeRoot->FirstChild("sprite");

	while(pNodeSpt)
	{
		char* s_name = NULL;
		INT   x=-1,  y=-1,  w=-1,  h=-1;
		INT  oX= 0, oY= 0, oW= 0, oH= 0;
		char* r=NULL;
		
		s_name = (char*)pNodeSpt->ToElement()->Attribute("n");
		pNodeSpt->ToElement()->Attribute( "x", &x );
		pNodeSpt->ToElement()->Attribute( "y", &y );
		pNodeSpt->ToElement()->Attribute( "w", &w );
		pNodeSpt->ToElement()->Attribute( "h", &h );
		pNodeSpt->ToElement()->Attribute("oX", &oX);
		pNodeSpt->ToElement()->Attribute("oY", &oY);
		pNodeSpt->ToElement()->Attribute("oW", &oW);
		pNodeSpt->ToElement()->Attribute("oH", &oH);

		r = (char*)pNodeSpt->ToElement()->Attribute("r");

		pNodeSpt = pNodeSpt->NextSibling();

		pRec = new TtdaOrg;
		pRec->rcD = LCXRECT( FLOAT(x), FLOAT(y), FLOAT(w), FLOAT(h) );
		pRec->rcO = LCXRECT( FLOAT(oX), FLOAT(oY), FLOAT(oW), FLOAT(oH) );

		pRec->rot = (r)? 1.0F: 0;

		char sTmp1[LC_MAX_PATH]={0};
		char sTmp2[LC_MAX_PATH]={0};
		memset(pRec->sName, 0, sizeof(pRec->sName));

		LcStr_Replace(sTmp1, s_name, ".png", "");
		LcStr_Replace(sTmp2, sTmp1 , ".tga", "");	memset(sTmp1, 0, LC_MAX_PATH);
		LcStr_Replace(sTmp1, sTmp2 , ".jpg", "");	memset(sTmp2, 0, LC_MAX_PATH);
		LcStr_Replace(pRec->sName, sTmp1, ".bmp", "");

		vRec.push_back(pRec);
	}

	XmlDoc.Clear();


	m_nLst = (INT)vRec.size();
	if( 0>= m_nLst)
		return LC_CANNT_FIND_RESOURCE;


	// setup the Vertex list
	SetupVtx(&vRec);
	vRec.clear();


	// save to binary
	char	sBin[LC_MAX_PATH2]={0};
	LcStr_Replace(sBin, sFile, ".tda", ".gtda");

	hr = SaveToBin(sBin);

	return hr;
}




INT CLcxTda::LoadFromCsv(char* sFile)
{
	TtdaOrg*	pRec = NULL;

	INT		hr = 0;
	INT		cnt =0;
	char	sLine[1028];
	char	dst[32][LC_MAX_PATH]={0};

	FILE*	fp  = NULL;
	fp = fopen(sFile, "rt");
	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;


	// 1. Image Path, width, and height
	fgets(sLine, 1024, fp);

	cnt = LcStr_Strtok((char**)dst, LC_MAX_PATH, 12, sLine, ",\n");

	strcpy(this->m_sTex, dst[0]);
	m_imgW = atoi(dst[1]);
	m_imgH = atoi(dst[2]);


	// read the record
	std::vector<TtdaOrg* > vRec;


	while(!feof(fp))
	{
		memset(sLine, 0, 1028);
		fgets(sLine, 1024, fp);
		hr = strlen(sLine);

		if( 4 > hr)
			continue;

		memset(dst, 0, 32 * LC_MAX_PATH);
		cnt = LcStr_Strtok((char**)dst, LC_MAX_PATH, 12, sLine, ",\n");

		char* s_name = NULL;
		INT   x=-1,  y=-1,  w=-1,  h=-1;
		INT  oX= 0, oY= 0, oW= 0, oH= 0;
		FLOAT r= 0.0F;
		
		s_name = dst[0];
		x  = atoi(dst[1]);
		y  = atoi(dst[2]);
		w  = atoi(dst[3]);
		h  = atoi(dst[4]);
		oX = atoi(dst[5]);
		oY = atoi(dst[6]);
		oW = atoi(dst[7]);
		oH = atoi(dst[8]);
		r  = strlen(dst[9])? 1.0F : 0.0F;

		pRec = new TtdaOrg;
		pRec->rcD = LCXRECT( FLOAT(x), FLOAT(y), FLOAT(w), FLOAT(h) );
		pRec->rcO = LCXRECT( FLOAT(oX), FLOAT(oY), FLOAT(oW), FLOAT(oH) );

		pRec->rot = r;

		char sTmp1[LC_MAX_PATH]={0};
		char sTmp2[LC_MAX_PATH]={0};
		memset(pRec->sName, 0, sizeof(pRec->sName));

		LcStr_Replace(sTmp1, s_name, ".png", "");
		LcStr_Replace(sTmp2, sTmp1 , ".tga", "");	memset(sTmp1, 0, LC_MAX_PATH);
		LcStr_Replace(sTmp1, sTmp2 , ".jpg", "");	memset(sTmp2, 0, LC_MAX_PATH);
		LcStr_Replace(pRec->sName, sTmp1, ".bmp", "");

		vRec.push_back(pRec);
	}

	fclose(fp);

	m_nLst = (INT)vRec.size();
	if( 0>= m_nLst)
		return LC_CANNT_FIND_RESOURCE;


	// setup the Vertex list
	SetupVtx(&vRec);
	vRec.clear();


	// save to binary
	char	sBin[LC_MAX_PATH2]={0};
	LcStr_Replace(sBin, sFile, ".csv", ".gtda");

	hr = SaveToBin(sBin);

	return hr;
}


INT CLcxTda::SaveToBin(char* sFile)
{
	FILE*	fp  = NULL;

	fp = fopen(sFile, "wb");
	if(NULL == fp)
		return LC_CANNT_ALLOC;


	// 1. Writing Header
	fwrite(  m_ver, sizeof(INT), 2, fp)	;					// Version
	fwrite(&m_nLst, sizeof(INT), 1, fp)	;					// sprite list
	fwrite(&m_imgW, sizeof(INT), 1, fp)	;					// Image Width
	fwrite(&m_imgH, sizeof(INT), 1, fp)	;					// Image Height

	if(0>=m_nLst || 0 >=m_imgW || 0 >= m_imgH)
	{
		fclose(fp);
		return LC_CANNT_FIND_RESOURCE;
	}


	fseek(fp, LC_RSC_HEADER, SEEK_SET);						// seek offset

	// 2. Writing the texture file name
	fwrite(m_sTex, 1, LCXM_TEXNAME, fp);					// texture file name

	// 3. Writing the Record
	fwrite(m_pRec, sizeof(TLC_TDAREC), m_nLst, fp);			// texture file name

	fclose(fp);

	return LC_OK;
}


INT CLcxTda::SetupVtx(std::vector<TtdaOrg* >* pOrg)
{
	TLC_TDAREC* pRecD = NULL;
	TtdaOrg*	pRecO = NULL;

	LCXVECTOR2* p = NULL;
	LCXVECTOR2* t = NULL;
	INT			bRot = 0;
	INT			n = 0;


	m_pRec = new TLC_TDAREC[this->m_nLst];


	for(n=0; n<m_nLst; ++n)
	{
		pRecD	= &m_pRec[n];
		pRecO	= *(pOrg->begin() + n);

		bRot = (0<pRecO->rot)? 1: 0;

		p = pRecD->pos;
		t = pRecD->tex;

		strcpy(pRecD->sName, pRecO->sName);
		
		// position:
		if(0 == bRot)
		{
			if(0 < pRecO->rcO.w)
			{
				p[0].x = -pRecO->rcO.w/2 + pRecO->rcO.x;
				p[2].x = p[0].x + pRecO->rcD.w;
			}
			else
			{
				p[0].x = -pRecO->rcD.w/2 + 0;
				p[2].x =  pRecO->rcD.w/2 + 0;
			}


			if(0 < pRecO->rcO.h)
			{
				p[0].y =  pRecO->rcO.h/2 - pRecO->rcO.y;
				p[2].y =  p[0].y - pRecO->rcD.h;
			}
			else
			{
				p[0].y =  pRecO->rcD.h/2 + 0;
				p[2].y = -pRecO->rcD.h/2 + 0;
			}

			p[1].x = p[2].x;
			p[1].y = p[0].y;

			p[3].x = p[0].x;
			p[3].y = p[2].y;

			// uv:
			t[0].x =  pRecO->rcD.x/m_imgW;
			t[0].y = 1 - pRecO->rcD.y/m_imgH;
			t[2].x = (pRecO->rcD.x + pRecO->rcD.w)/m_imgW;
			t[2].y = 1 - (pRecO->rcD.y + pRecO->rcD.h)/m_imgH;

			t[1].x = t[2].x;
			t[1].y = t[0].y;

			t[3].x = t[0].x;
			t[3].y = t[2].y;
		}
		else
		{
			if(0 < pRecO->rcO.w)
			{
				p[0].x = -pRecO->rcO.w/2 + pRecO->rcO.x;
				p[2].x = p[0].x + pRecO->rcD.h;
			}
			else
			{
				p[0].x = -pRecO->rcD.h/2 + 0;
				p[2].x =  pRecO->rcD.h/2 + 0;
			}


			if(0 < pRecO->rcO.h)
			{
				p[0].y =  pRecO->rcO.h/2 - pRecO->rcO.y;
				p[2].y =  p[0].y - pRecO->rcD.w;
			}
			else
			{
				p[0].y =  pRecO->rcD.w/2 + 0;
				p[2].y = -pRecO->rcD.w/2 + 0;
			}

			p[1].x = p[2].x;
			p[1].y = p[0].y;

			p[3].x = p[0].x;
			p[3].y = p[2].y;

			// uv:
			t[0].x =  pRecO->rcD.x/m_imgW;
			t[0].y = 1 - pRecO->rcD.y/m_imgH;
			t[2].x = (pRecO->rcD.x + pRecO->rcD.w)/m_imgW;
			t[2].y = 1 - (pRecO->rcD.y + pRecO->rcD.h)/m_imgH;

			t[1].x = t[2].x;
			t[1].y = t[0].y;

			t[3].x = t[0].x;
			t[3].y = t[2].y;

			LCXVECTOR2 tmp = t[0];
			t[0] = t[1];
			t[1] = t[2];
			t[2] = t[3];
			t[3] = tmp;
		}
	}


	return LC_OK;
}



////////////////////////////////////////////////////////////////////////////////


INT LcxEnt_CreateTda(char* sCmd			// Create TDA
				 , PLCX_TDA* pOut		// Output instance
				 , LC_HANDLE sName		// File Name
				 , LC_HANDLE p2			// No use
				 , LC_HANDLE p3			// No use
				 , LC_HANDLE p4			// No use
				 )
{
	INT			hr = 0;
	INT			nIdx = -1;
	CLcxTda*	pObj = NULL;


	char* s_name = (char*)sName;


	*pOut		= NULL;
	if(NULL == s_name || 4 > strlen(s_name) )
		return LC_CANNT_FIND_RESOURCE;


	nIdx = ILCX_Tda::FindIdx(s_name);
	if(LC_SUCCEEDED(nIdx))
	{
		*pOut = ILCX_Tda::Find(nIdx);
		return LC_OK;
	}

	pObj = new CLcxTda;
	hr = pObj->CreateFromFile(sName, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}

	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}



// Create TDA Instance from memory with name, buffer pointer, and size
// PLC_ARGS option
//	p0 : sName	Model File Name
//	p1 : pMem	starting memory pointer
//	p2 : pSize	size(unsigned int) pointer

INT LcxEnt_CreateTdaFromMemory(char* sCmd, PLCX_TDA* pOut, PLC_ARGS	pArg)
{
	INT			hr = 0;
	INT			nIdx = -1;
	CLcxTda*	pObj = NULL;


	char* s_name = (char*)pArg->p0;


	*pOut		= NULL;
	if(NULL == s_name || 4 > strlen(s_name) )
		return LC_CANNT_FIND_RESOURCE;


	nIdx = ILCX_Tda::FindIdx(s_name);
	if(LC_SUCCEEDED(nIdx))
	{
		*pOut = ILCX_Tda::Find(nIdx);
		return LC_OK;
	}

	pObj = new CLcxTda;
	hr = pObj->CreateFromMemory(pArg);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}

	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}
