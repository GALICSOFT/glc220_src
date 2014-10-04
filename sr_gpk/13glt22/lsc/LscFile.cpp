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




////////////////////////////////////////////////////////////////////////////////
// Script

namespace LscFile_t
{
	static LscFile::ILhLst* m_pCont = NULL;
}


INT	LscFile::Create(LC_HANDLE v)
{
	if(NULL == LscFile_t::m_pCont)
		LscFile_t::m_pCont = new LscFile::ILhLst;

	return 0;
}


INT	LscFile::Destroy()
{
	if(LscFile_t::m_pCont)
	{
		delete LscFile_t::m_pCont;
		LscFile_t::m_pCont = NULL;
	}

	return 0;
}


static const luaL_reg regLscFile[] =
{
//	{"Open"		,	(lua_CFunction)LscFile::Lsc_FileOpen		},
//	{"Close"	,	(lua_CFunction)LscFile::Lsc_FileClose		},

	{"New"		,	(lua_CFunction)LscFile::Lsc_FileOpen		},
	{"Release"	,	(lua_CFunction)LscFile::Lsc_FileClose		},
	{"Read"		,	(lua_CFunction)LscFile::Lsc_FileRead		},
	{"ReadLine"	,	(lua_CFunction)LscFile::Lsc_FileReadLine	},
	{"Write"	,	(lua_CFunction)LscFile::Lsc_FileWrite		},

	{NULL, NULL},
};


INT LscFile::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "Lfile", regLscFile, 0);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////
// For list

LscFile::ILhLst::ILhLst()
{
	memset(vLst, 0, sizeof(LscH*) * MAX_FILE);
}

LscFile::ILhLst::~ILhLst()
{
	PLC_FILE pObj = NULL;

	for(INT i=0; i<MAX_FILE; ++i)
	{
		if(vLst[i])
		{
			pObj = (PLC_FILE)(vLst[i]->pIns);
			SAFE_RELEASE(	pObj	);

			delete vLst[i];
			vLst[i] = NULL;
		}
	}
}


INT LscFile::ILhLst::Alloc(char* sFile, char* sMode)
{
	LscH*		pLsH = NULL;
	PLC_FILE	pObj = NULL;

	INT			nKey	=-1;
	INT			hr		=-1;

	pLsH = this->Find(&nKey, sFile);
	if(pLsH && 0 <= nKey)											// if same file exists.
		return nKey+1;												// lua index starts from 1

	nKey	= FindEmpty();
	if(0 > nKey)													// all key is used. allowble key does not exist.
		return -1;

	hr = LcFile_Open(NULL,&pObj, sFile, sMode);
	if(LC_FAILED(hr))
	{
		LOGE("LscFile::ILhLst::Alloc::Err: %s\n", sFile);
		return -1;
	}

	pLsH	= new LscH;
	pLsH->pIns	= pObj;
	strcpy(pLsH->sName, sFile);

	vLst[nKey] = pLsH;

	return (nKey+1);
}


INT LscFile::ILhLst::Release(INT nKey)
{
	LscH*		pLsH = NULL;
	PLC_FILE	pObj = NULL;

	if(MAX_FILE <=nKey || nKey<1)
		return -1;


	pLsH = vLst[nKey-1];

	if(NULL == pLsH)
		return -1;

	pObj = (PLC_FILE)(pLsH->pIns);

	SAFE_RELEASE(	pObj	);

	delete pLsH;

	vLst[nKey-1] = NULL;

	return 0;
}

LscH* LscFile::ILhLst::Find(INT nKey)
{
	if(MAX_FILE <=nKey || nKey<1)
		return NULL;

	return vLst[nKey-1];
}

LscH* LscFile::ILhLst::Find(INT* nIdx, char* sFile)
{
	if(nIdx)
		*nIdx = -1;

	if(NULL == sFile || strlen(sFile)<1)
		return NULL;


	for(INT i=0; i<MAX_FILE; ++i)
	{
		if(NULL == vLst[i])
			continue;

		char* sName = vLst[i]->sName;

		if(0 == ::LcStr_Strcmp(sFile, sName))
		{
			if(nIdx)
				*nIdx = i;

			return vLst[i];
		}
	}

	return NULL;
}



INT LscFile::ILhLst::FindEmpty()
{
	for(INT i=0; i<MAX_FILE; ++i)
	{
		if(NULL == vLst[i])
			return i;
	}

	return -1;
}




////////////////////////////////////////////////////////////////////////////////
// for glue

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

	nKey = LscFile_t::m_pCont->Alloc(sFile, sMode);

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

	iSize = LscFile_t::m_pCont->Release(nKey);

	// 남아 있는 사이즈를 돌려준다.
	lua_pushnumber(pL, iSize);
	return 1;
}


INT LscFile::Lsc_FileRead(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FILE	pObj = NULL;

	INT		nKey=-1;
	INT		nRead=-1;
	INT		n = lua_gettop(pL);

	char	buf[2048]={0};
	char	linecomment[32]="*line";
	INT		nBuf = 0;
	char*	r = 0;
	int		hr= 0;

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscFile_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLC_FILE)pLsH->pIns;


	if(2 <= n)
	{
		if(!lua_isnumber(pL, 2) )
			strcpy(linecomment, (char*)lua_tostring(pL, 2) );
		else
			nBuf  = (INT  )lua_tonumber(pL, 2);
	}


	if(0 == LcStr_Stricmp(linecomment, "*line") || 0 == LcStr_Stricmp(linecomment, "lines") )
	{
		//r = fgets(buf, 2048, pObj);
		hr= pObj->ReadLine(buf, 2048);
		if(LC_FAILED(hr))
			lua_pushnil(pL);
		else
		{
			lua_pushstring(pL, buf);
			//LOGI("LscFile::FileRead::%s\n", buf);
		}

		return 1;
	}
	else if(0 >= nBuf)
	{
		//lua_pushstring(pL, NULL);
		lua_pushnil(pL);
		lua_pushnil(pL);
		return 2;
	}


	nRead = pObj->ReadByte(buf, nBuf);

	lua_pushnumber(pL, nRead);
	lua_pushstring(pL, buf);
	return 2;
}


INT LscFile::Lsc_FileReadLine(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FILE	pObj = NULL;

	INT			nKey=-1;
	INT			n = lua_gettop(pL);

	char		buf[2048]={0};
	int			hr = 0;

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscFile_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLC_FILE)pLsH->pIns;


	hr = pObj->ReadLine(buf, 2048);
	if(LC_FAILED(hr))
		lua_pushnil(pL);
	else
	{
		lua_pushstring(pL, buf);
		//LOGI("LscFile::FileReadLine::%s\n", buf);
	}

	return 1;
}



INT LscFile::Lsc_FileWrite(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FILE	pObj = NULL;

	INT			nKey  =-1;
	INT			nWrite= 0;
	INT			n = lua_gettop(pL);

	if(n<2)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscFile_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLC_FILE)pLsH->pIns;


	// write to buffer
	char	buf[2048]={0};
	int		len =0;
	int		nrec =0;

	for(int l=1; l<n; ++l)
	{
		strcpy(buf, (char*)lua_tostring(pL, l+1) );
		len = strlen(buf);

		nrec = pObj->WriteByte(buf, len);
		if(0>= nrec)
			continue;


		nWrite += nrec;
	}

	lua_pushnumber(pL, nWrite);
	return 1;
}


