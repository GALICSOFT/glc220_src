// Implementation of the LscSys class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1200
  #pragma warning(disable: 4996)
#endif


#if defined(_WIN32)
  #include <Windows.h>
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

namespace LscSys_t
{
	static LC_HANDLE m_pAppMain	= NULL	;
}



INT LscSys::Create(LC_HANDLE v)
{
	LscSys_t::m_pAppMain = v;
	return 0;
}


INT LscSys::Destroy()
{
	return 0;
}


static const luaL_reg regLscSys[] =
{
	{"CreateWindow"		,	(lua_CFunction)LscSys::Lsc_CreateWindow		},
	{"ScriptFile"		,	(lua_CFunction)LscSys::Lsc_ScriptFile		},
	{"Release"			,	(lua_CFunction)LscSys::Lsc_Release			},

	{"ConsoleAlloc"		,	(lua_CFunction)LscSys::Lsc_ConsoleAlloc		},
	{"ConsoleSet"		,	(lua_CFunction)LscSys::Lsc_ConsoleSet		},
	{"ConsoleSend"		,	(lua_CFunction)LscSys::Lsc_ConsoleSend		},

	{"Sleep"			,	(lua_CFunction)LscSys::Lsc_Sleep			},
	{"ShowCursor"		,	(lua_CFunction)LscSys::Lsc_ShowCursor		},

	{"ShowState"		,	(lua_CFunction)LscSys::Lsc_ShowState		},
	{"ChangeMode"		,	(lua_CFunction)LscSys::Lsc_ChangeMode		},
	{"SetClearColor"	,	(lua_CFunction)LscSys::Lsc_SetClearColor	},
	{"GetClearColor"	,	(lua_CFunction)LscSys::Lsc_GetClearColor	},

	{"GetTickCount"		,	(lua_CFunction)LscSys::Lsc_GetTickCount		},
	{"TimeGetTime"		,	(lua_CFunction)LscSys::Lsc_TimeGetTime		},
	{"GetTime"			,	(lua_CFunction)LscSys::Lsc_GetTime			},

	{"WindowTitle"		,	(lua_CFunction)LscSys::Lsc_WindowTitle		},
	{"ModulePath"		,	(lua_CFunction)LscSys::Lsc_ModulePath		},
	{"DoFile"			,	(lua_CFunction)LscSys::Lsc_DoFile			},
	{"Open"				,	(lua_CFunction)LscSys::Lsc_OpenFile			},

	{"Log"				,	(lua_CFunction)LscSys::Lsc_LogOut			},

	{NULL, NULL},
};


INT LscSys::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "Lsys", regLscSys, 0);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////
// for glue


INT LscSys::Lsc_CreateWindow(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_APPFRAME pApp = (PLC_APPFRAME)(LscSys_t::m_pAppMain);

	INT		iScnX	= 0;
	INT		iScnY	= 0;
	INT		iScnW	= 0;
	INT		iScnH	= 0;
	char	sClsName[LC_MAX_PATH]={0};
	INT		bFull	= 0;

	INT		hr=-1;
	INT		n = lua_gettop(pL);

	if(n<4)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	iScnX	= (INT)lua_tonumber(pL, 1);
	iScnY	= (INT)lua_tonumber(pL, 2);
	iScnW	= (INT)lua_tonumber(pL, 3);
	iScnH	= (INT)lua_tonumber(pL, 4);

	strcpy(sClsName, (char*)lua_tostring(pL, 5));
	bFull = (INT)lua_tonumber(pL, 6)? 1: 0;


	hr = pApp->Query((char*)"Set Screen Position X"	, &iScnX	);
	hr = pApp->Query((char*)"Set Screen Position Y"	, &iScnY	);
	hr = pApp->Query((char*)"Set Screen Width"		, &iScnW	);
	hr = pApp->Query((char*)"Set Screen Height"		, &iScnH	);
	hr = pApp->Query((char*)"Set Class Name"		, sClsName	);
	hr = pApp->Query((char*)"Set Screen Mode Full"	, &bFull	);

	return 0;
}


INT LscSys::Lsc_ScriptFile(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_APPFRAME	pApp = (PLC_APPFRAME)(LscSys_t::m_pAppMain);

	char	sFile[LC_MAX_PATH+4]={0};
	INT		nIdx = 0;

	INT		hr=-1;
	INT		n = lua_gettop(pL);

	if(n<2)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nIdx = (INT)lua_tonumber(pL, 1);
	strcpy(sFile+ 4, (char*)lua_tostring(pL, 2) );

	memcpy(sFile, &nIdx, sizeof nIdx);

	hr = pApp->Query((char*)"Set Script File with Index", sFile	);

	return 0;
}



INT LscSys::Lsc_Release(LC_HANDLE _L)
{
	//lua_State*	pL = (lua_State*)_L;

	//	printf("La Release\n");

	return 0;
}




INT LscSys::Lsc_ConsoleAlloc(LC_HANDLE _L)
{
	//lua_State*	pL = (lua_State*)_L;

	PLC_APPFRAME	pApp = (PLC_APPFRAME)(LscSys_t::m_pAppMain);

	pApp->Query((char*)"Console Alloc", NULL);

	return 0;
}

INT LscSys::Lsc_ConsoleSet(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_APPFRAME	pApp = (PLC_APPFRAME)(LscSys_t::m_pAppMain);

	INT v1= (INT)lua_tonumber(pL, 1);

	pApp->Query((char*)"Console Enable", &v1);

	return 0;
}


INT LscSys::Lsc_ConsoleSend(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_APPFRAME	pApp = (PLC_APPFRAME)(LscSys_t::m_pAppMain);

	char* v1= (char*)lua_tostring(pL, 1);

	pApp->Query((char*)"Send to Console", v1);

	return 0;
}





INT LscSys::Lsc_Sleep(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT nMilliSecond = (INT)lua_tonumber(pL, 1);

	LcSys_Sleep(nMilliSecond);

	return 0;
}

INT LscSys::Lsc_ShowCursor(LC_HANDLE _L)
{
#if defined(_WIN32)
	lua_State*	pL = (lua_State*)_L;

	INT		n = lua_gettop(pL);
	INT nMilliSecond = (INT)lua_tonumber(pL, 1);

#endif

	return 0;
}




INT	LscSys::Lsc_ShowState(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_APPFRAME	pApp = (PLC_APPFRAME)(LscSys_t::m_pAppMain);

	INT			bShowState= LC_FALSE;
	INT			n = lua_gettop(pL);

	if(n>0)
		bShowState = (INT)lua_tonumber(pL, 1);

	pApp->Query((char*)"Set Show Status", &bShowState);

	return 0;
}


INT	LscSys::Lsc_ChangeMode(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_APPFRAME	pApp = (PLC_APPFRAME)(LscSys_t::m_pAppMain);

	INT			bFull = 0;
	INT			n = lua_gettop(pL);

	if(n>0)
		bFull = (INT)lua_tonumber(pL, 1);


	pApp->Query((char*)"Toggle Screen Mode", &bFull);
	return 0;
}


INT	LscSys::Lsc_SetClearColor(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_APPFRAME	pApp = (PLC_APPFRAME)(LscSys_t::m_pAppMain);

	INT			hr=-1;

	char		s[16]={0};
	UINT		dCol = 0x0;

	strcpy(s, (char*)lua_tostring(pL, 1) );
	sscanf(s, "%x", &dCol);

	hr = pApp->Query((char*)"Set Clear Color", &dCol);

	if(LC_FAILED(hr))
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	lua_pushnumber(pL, 0);
	return 1;
}

INT	LscSys::Lsc_GetClearColor(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_APPFRAME	pApp = (PLC_APPFRAME)(LscSys_t::m_pAppMain);
	UINT			dColor = 0x0;

	pApp->Query((char*)"Get Clear Color", &dColor);

	lua_pushnumber(pL, dColor);
	return 1;
}



INT	LscSys::Lsc_GetTickCount(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	//DOUBLE v1= ::LcSys_TimeGetTime();
	DOUBLE v1= ::LcSys_GetTickCount();

	lua_pushnumber(pL, v1);
	return 1;
}


INT LscSys::Lsc_TimeGetTime(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	//DOUBLE v1= ::LcSys_TimeGetTime();
	DOUBLE v1= ::LcSys_GetTickCount();

	lua_pushnumber(pL, v1);
	return 1;
}


INT	LscSys::Lsc_GetTime(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	//DOUBLE v1= ::LcSys_TimeGetTime();
	DOUBLE v1= ::LcSys_GetTickCount();

	lua_pushnumber(pL, v1);
	return 1;
}



INT LscSys::Lsc_WindowTitle(LC_HANDLE _L)
{
#if defined(_WIN32)
	lua_State*	pL = (lua_State*)_L;

	PLC_APPFRAME	pApp = (PLC_APPFRAME)(LscSys_t::m_pAppMain);

	char	sMsg[512]={0};
	LC_HWND hWnd = NULL;

	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	for(INT i=1; i<=n; ++i)
	{
		char sRcv[512]={0};

		if (!lua_isnumber(pL, i))
		{
			sprintf(sRcv, " %s", lua_tostring(pL, i));
			strcat(sMsg, sRcv);
		}

		else
		{
			double fRcvNumber = lua_tonumber(pL, i);
			sprintf(sRcv, " %6.3f", fRcvNumber);
			strcat(sMsg, sRcv);
		}
	}


	pApp->Query((char*)"Get Window Handle", &hWnd);
	::SetWindowText((HWND)hWnd, sMsg);
#endif


	return 0;
}


INT LscSys::Lsc_LogOut(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	char	sMsg[1024]={0};

	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	for(INT i=1; i<=n; ++i)
	{
		char sRcv[260]={0};

		if (!lua_isnumber(pL, i))
		{
			sprintf(sRcv, " %s", lua_tostring(pL, i));
			strcat(sMsg, sRcv);
		}

		else
		{
			double fRcvNumber = lua_tonumber(pL, i);
			sprintf(sRcv, " %6.3f", fRcvNumber);
			strcat(sMsg, sRcv);
		}
	}


	strcat(sMsg, "\n");

	LcLog_Out(LC_LOG_INFO, sMsg);

	return 0;
}




INT LscSys::Lsc_ModulePath(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	char	s[512]={0};
	char	stPath[LC_MAX_PATH*2];

	strcpy(s, stPath);

	lua_pushstring(pL, s);
	return 1;
}


INT LscSys::Lsc_DoFile(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT			hr = 0;
	const char*	LuaFile = luaL_optstring(pL, 1, NULL);

	hr = Lua_Load(pL, (LC_HANDLE)LuaFile);

	if(LC_FAILED(hr))
		LOGE("Lsc_DoFile error: %s\n", lua_tostring(pL, -1));

	lua_pushnumber(pL, hr);
	return 1;
}


#if defined(_UNICODE) && defined(_WINCE)
#include <tchar.h>
#endif

INT LscSys::Lsc_OpenFile(LC_HANDLE _L)
{
	int			hr = 0;
	lua_State*	pL = (lua_State*)_L;

	const char*	sFile = luaL_checkstring(pL, 1);
	const char*	mode = luaL_optstring(pL, 2, "r");
	FILE**	pf = NULL;


	char  sMedia[LC_MAX_PATH2]={0};
	char* tsave = (char*)strstr(sFile, (const char*)"save");


	hr = LcSys_ResourcePath(sMedia, sFile);

	if(tsave)
	{
		char*	sApp = NULL;
		char	File[LC_MAX_PATH2]={0};
		char	Ext [LC_MAX_EXT  ]={0};
		char	Src [LC_MAX_PATH2]={0};
		FILE*	fps = NULL;

		strcpy(Src, sMedia);								// Copy file name


		LcStr_SplitPath(sFile, NULL, NULL, File, Ext);


#if defined(__APPLE__) || defined(__IPHONE__)

		sApp = (char*)LcSys_AppFolder();					// find document folder
		sprintf(sMedia, "%s/%s%s", sApp, File, Ext);		// rename dest file

		fps = fopen(sMedia, "r");
		if(NULL==fps)										// is there dest file?
		{
			hr = LcFile_XcopyFileToFile(sMedia, Src);
			if(LC_FAILED(hr))
				LOGE("LscFile::ILhLst::Alloc:: File Copy Err: %s\n", sMedia);
		}
		else
			fclose(fps);


#elif defined(__ANDROID__)

		sApp = (char*)LcSys_AppFolder();					// find in App folder
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
				LOGE("LscFile::ILhLst::Alloc:: File Copy Err: %s\n", sMedia);
		}
		else
			fclose(fps);
#endif
	}


	pf = (FILE**)lua_newuserdata(pL, sizeof(FILE *));
	*pf = NULL;  /* file handle is currently `closed' */
	luaL_getmetatable(pL, "FILE*");
	lua_setmetatable(pL, -2);


	*pf = fopen(sMedia, mode);

	if(NULL == *pf)
		return luaL_pushresult2x(pL, 0, sMedia);

	return 1;
}



INT LscSys::Lsc_MessageBox(LC_HANDLE _L)
{
#if defined(_UNICODE) && defined(_WIN32)
	lua_State*	pL = (lua_State*)_L;

	char	sMsg[512]={0};
	char	sCap[128]={0};
	INT		uType = 0;

	INT		n = lua_gettop(pL);
	HWND	hWnd = (HWND)LcDev_GetHwnd(pApp);

	if(1 == n)
	{
		sprintf(sMsg, "%s", lua_tostring(pL, 1));
	}

	else if(2 == n)
	{
		sprintf(sMsg, "%s", lua_tostring(pL, 1));
		sprintf(sCap, "%s", lua_tostring(pL, 2));
	}

	else if(3 <= n)
	{
		sprintf(sMsg, "%s", lua_tostring(pL, 1));
		sprintf(sCap, "%s", lua_tostring(pL, 2));
		uType = lua_tonumber(pL, 3);
	}

	MessageBox(hWnd, sMsg, sCap, uType);
#endif

	return 0;
}


