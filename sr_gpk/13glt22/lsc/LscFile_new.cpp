// Implementation of the LscFile class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1200
  #pragma warning(disable: 4311)
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#if defined(__BADA__)
  #include <FBase.h>	// for AppLog
#endif

#include <LcType.h>
#include <LcMath.h>
#include <LcBase.h>
#include <LcAux.h>

#if !defined(_LUA_SRC_ORG)
#include "../LctLua.h"
#else
#include <LctLua.h>
#endif


// For Cont
////////////////////////////////////////////////////////////////////////////////

LscFile::ILhCont::ILhCont()
{
	memset(vLscH, 0, sizeof(LscH*) * MAX_FILE);
}

LscFile::ILhCont::~ILhCont()
{
	FILE* pObj;

	for(INT i=0; i<MAX_FILE; ++i)
	{
		if(vLscH[i])
		{
			pObj = (FILE*)(vLscH[i]->pIns);
			SAFE_FCLOSE(	pObj	);

			delete vLscH[i];
			vLscH[i] = NULL;
		}
	}
}


INT LscFile::ILhCont::Alloc(char* fname, char* sMode)
{
	LscH*		pLsH = NULL;
	PLC_FILE	pObj = NULL;

	INT		nKey	=-1;
	INT		hr		=-1;

	pLsH	= this->Find(&nKey, fname);

	// 같은 텍스처가 있으면 리턴
	if(pLsH && 0 <= nKey)
		return nKey;


	nKey	= FindEmpty();

	// 꽉차있음
	if(0 > nKey)
		return -1;



	// 만약 파일 이름 안에 "save" 문자열이나 쓰기 file 있으면 Doc 폴더에서 파일을 찾고 Doc 폴더에
	// 없으면 Resource 안에 있는 파일을 Document 폴더로  옮긴 후 다시 연다.
	char  sMedia[LC_MAX_PATH2]={0};
	char* tsave = strstr(fname, (char*)"save");

	hr = LcSys_FindResource(sMedia, fname);


	if(NULL == tsave)
		tsave = strstr(sMode, (char*)"w");

	if(tsave)
	{
		char*	sApp = NULL;
		char	File[LC_MAX_PATH2]={0};
		char	Ext [LC_MAX_EXT  ]={0};
		char	Src [LC_MAX_PATH2]={0};
		FILE*	fps = NULL;

		strcpy(Src, sMedia);								// Copy file name

		LcStr_SplitPath(fname, NULL, NULL, File, Ext);


#if defined(__APPLE__) || defined(__IPHONE__)

		sApp = (char*)LcSys_IphoneDocDir();					// find document folder
		sprintf(sMedia, "%s/%s%s", sApp, File, Ext);		// rename dest file

		fps = fopen(sMedia, "r");
		if(NULL==fps)										// is there dest file?
		{
			hr = LcFile_XcopyFileToFile(sMedia, Src);
			if(LC_FAILED(hr))
				LOGE("LscFile::ILhCont::Alloc:: File Copy Err: %s\n", sMedia);
		}
		else
			fclose(fps);


#elif defined(__ANDROID__)

		sApp = (char*)LcSys_AppFolder();				// find in App folder
		sprintf(sMedia, "%s/%s%s", sApp, File, Ext);		// rename dest file

		fps = fopen(sMedia, "r");
		if(NULL==fps)										// is there dest file?
		{
			BYTE*		buf = NULL;
			ULONG		len = 0;
			const char*	sApkFile = LcSys_AndroidApkFile(NULL);

			hr = Lzip_UnzipbyName(&buf, &len, sApkFile, Src, LC_TRUE);
			hr = LcFile_XcopyBufferToFile(sMedia, buf, len);
			if(LC_FAILED(hr))
				LOGE("LscFile::ILhCont::Alloc:: File Copy Err: %s\n", sMedia);
		}
		else
			fclose(fps);
#endif
	}


	hr = LcFile_CreateFile(NULL, &pObj, sMedia, 0, sMode);

	if(NULL == pObj)
	{
		LOGE("LscFile::fopen::Err: %s, %s\n", sMode, sMedia);
		return -1;
	}


	pLsH	= new LscH;
	pLsH->pIns	= pObj;
	strcpy(pLsH->sName, fname);

	vLscH[nKey] = pLsH;
	return (nKey+1);
}


INT LscFile::ILhCont::Release(INT nKey)
{
	LscH*	pLsH = NULL;
	FILE*	pObj = NULL;

	if(MAX_FILE <=nKey || nKey<1)
		return -1;

	pLsH = vLscH[nKey-1];

	if(NULL == pLsH)
		return -1;

	pObj = (FILE*)(pLsH->pIns);

	SAFE_FCLOSE(	pObj	);

	delete pLsH;

	vLscH[nKey-1] = NULL;

	return 0;
}

LscH* LscFile::ILhCont::Find(INT nKey)
{
	if(MAX_FILE <=nKey || nKey<1)
		return NULL;

	return vLscH[nKey-1];
}

LscH* LscFile::ILhCont::Find(INT* nIdx, char* sFile)
{
	if(nIdx)
		*nIdx = -1;

	if(NULL == sFile || strlen(sFile)<1)
		return NULL;


	for(INT i=0; i<MAX_FILE; ++i)
	{
		if(NULL == vLscH[i])
			continue;

		char* sName = vLscH[i]->sName;

		if(0 == ::strcmp(sFile, sName))
		{
			if(nIdx)
				*nIdx = i;

			return vLscH[i];
		}
	}

	return NULL;
}



INT LscFile::ILhCont::FindEmpty()
{
	for(INT i=0; i<MAX_FILE; ++i)
	{
		if(NULL == vLscH[i])
			return i;
	}

	return -1;
}




////////////////////////////////////////////////////////////////////////////////
// Script

LscFile::ILhCont* LscFile::m_pContAuto = NULL;


static const luaL_reg regLscFile[] =
{
	{"Open"		,	(lua_CFunction)LscFile::Lsc_FileOpen	},
	{"Close"	,	(lua_CFunction)LscFile::Lsc_FileClose	},

	{"Read"		,	(lua_CFunction)LscFile::Lsc_FileRead	},
	{"ReadLine"	,	(lua_CFunction)LscFile::Lsc_FileReadLine},
	{"Write"	,	(lua_CFunction)LscFile::Lsc_FileWrite	},

	{NULL, NULL},
};


INT LscFile::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "Lfile", regLscFile, 0);
	return 1;
}


// File
INT	LscFile::Lsc_FileOpen(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		nKey=-1;
	char*	sFile = NULL;
	char	sMode[16]="r";
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	// file name
	sFile = (char*)lua_tostring(pL, 1);

	if(2 <= n)
		strcpy(sMode, (char*)lua_tostring(pL, 2) );

	nKey = LscFile::m_pContAuto->Alloc(sFile, sMode);

	// Key를 돌려 준다.
	lua_pushnumber(pL, nKey);
	return 1;
}


INT LscFile::Lsc_FileClose(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		iSize = 0;
	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	iSize = LscFile::m_pContAuto->Release(nKey);

	// 남아 있는 사이즈를 돌려준다.
	lua_pushnumber(pL, iSize);
	return 1;
}


INT LscFile::Lsc_FileRead(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*	pLsH = NULL;
	FILE*	pObj = NULL;

	INT		nKey=-1;
	INT		nRead=-1;
	INT		n = lua_gettop(pL);

	char	buf[1024]={0};
	char	linecomment[32]="*line";
	INT		nBuf = 0;
	char*	r = 0;

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscFile::m_pContAuto->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (FILE*)pLsH->pIns;


	if(2 <= n)
	{
		if(!lua_isnumber(pL, 2) )
			strcpy(linecomment, (char*)lua_tostring(pL, 2) );
		else
			nBuf  = (INT  )lua_tonumber(pL, 2);
	}


	if(0 == LcStr_Stricmp(linecomment, "*line") || 0 == LcStr_Stricmp(linecomment, "lines") )
	{
		r = fgets(buf, 1024, pObj);

		if(r)
		{
			LcStr_TrimRight(buf, "\n\r");
			lua_pushstring(pL, buf);
		}
		else
			lua_pushstring(pL, NULL);
		
		return 1;
	}
	else if(0 >= nBuf)
	{
		lua_pushstring(pL, NULL);
		return 1;
	}


	nRead = fread(buf, 1, nBuf, pObj);
	lua_pushnumber(pL, nRead);
	lua_pushstring(pL, buf);
	return 2;
}


INT LscFile::Lsc_FileReadLine(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*	pLsH = NULL;
	FILE*	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	char	buf[1024]={0};
	char*	r = 0;

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscFile::m_pContAuto->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (FILE*)pLsH->pIns;


	r = fgets(buf, 1024, pObj);

	if(r)
	{
		LcStr_TrimRight(buf, "\n\r");
		lua_pushstring(pL, buf);
	}
	else
		lua_pushstring(pL, NULL);
	
	return 1;
}












INT LscFile::Lsc_FileWrite(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*	pLsH = NULL;
	FILE*	pObj = NULL;

	INT		nKey  =-1;
	INT		nWrite= 0;
	INT		n = lua_gettop(pL);

	if(n<2)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscFile::m_pContAuto->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (FILE*)pLsH->pIns;


	for(int l=1; l<n; ++l)
	{
		char	buf[520]={0};
		int		len =0;
		strcpy(buf, (char*)lua_tostring(pL, l+1) );
		len = strlen(buf);
		nWrite += fwrite(buf, 1, len, pObj);
	}

	lua_pushnumber(pL, nWrite);
	return 1;
}



























INT	LscFile::ContCreate()
{
	if(NULL == LscFile::m_pContAuto)
		LscFile::m_pContAuto = new LscFile::ILhCont;

	return 0;
}


INT	LscFile::ContDestroy()
{
	if(LscFile::m_pContAuto)
	{
		delete LscFile::m_pContAuto;
		LscFile::m_pContAuto = NULL;
	}

	return 0;
}


