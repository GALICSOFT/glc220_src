// Implementation of the ILhLsc class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1200
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
#include <LcAux.h>

#if !defined(_LUA_SRC_ORG)
#include "../LctLua.h"
#else
#include <LctLua.h>
#endif


LscH::LscH()
{
	nId		= 0xFFFFFFFF;
	pIns	= NULL;
	pMsc	= NULL;

	memset(sName, 0, sizeof sName);
}


INT LscLua_Init(LC_HANDLE _L)
{
	INT			hr	= -1;
	lua_State*	pL	= (lua_State*)_L;

	// Lua 스크립트 file에 있는 Lua_Init 함수를 호출
	lua_getglobal(pL, "Lua_Init");


	hr = lua_pcall(pL, 0, 1, 0);
	if(LUA_YIELD <= hr && hr <= LUA_ERRERR)
	{
		char msg[512]={0};
		sprintf(msg, "Lua_Init Failed: %s",  lua_tostring(pL, -1) );
		LOGE( msg);

		return -1;
	}

	// get the result
	hr = (HRESULT)lua_tonumber(pL, -1);
	lua_pop(pL, 1);

	if(LC_FAILED(hr))
		return -1;

	return hr;
}


INT LscLua_Destroy(LC_HANDLE _L)
{
	INT			hr	= -1;
	lua_State*	pL	= (lua_State*)_L;

	// Lua 스크립트 file에 있는 Lua_Destroy() 함수를 호출
	lua_getglobal(pL, "Lua_Destroy");


	hr = lua_pcall(pL, 0, 1, 0);
	if(LUA_YIELD <= hr && hr <= LUA_ERRERR)
	{
		char msg[512]={0};
		sprintf(msg, "Lua_Destroy Failed: %s",  lua_tostring(pL, -1) );
		LOGE( msg);
		return -1;
	}

	hr = (INT)lua_tonumber(pL, -1);
	lua_pop(pL, 1);

	if(LC_FAILED(hr))
		return -1;

	return hr;
}


INT	LscLua_FrameMove(LC_HANDLE _L)
{
	INT			hr	= -1;
	lua_State*	pL	= (lua_State*)_L;

	// Lua 스크립트 file에 있는 Lua_FrameMove 함수를 호출
	lua_getglobal(pL, "Lua_FrameMove");


	hr = lua_pcall(pL, 0, 1, 0);
	if(LUA_YIELD <= hr && hr <= LUA_ERRERR)
	{
		char msg[512]={0};
		sprintf(msg, "Lua_FrameMove Failed: %s",  lua_tostring(pL, -1));
		LOGE( msg);
		return -1;
	}

	// get the result
	hr = (INT)lua_tonumber(pL, -1);
	lua_pop(pL, 1);

	if(LC_FAILED(hr))
		return -1;

	return hr;
}


INT LscLua_Render(LC_HANDLE _L)
{
	INT			hr	= -1;
	lua_State*	pL	= (lua_State*)_L;

	// Lua 스크립트 file에 있는 Lua_Render 함수를 호출
	lua_getglobal(pL, "Lua_Render");

	hr = lua_pcall(pL, 0, 1, 0);
	if(LUA_YIELD <= hr && hr <= LUA_ERRERR)
	{
		char msg[512]={0};
		sprintf(msg, "Lua_Render Failed: %s",  lua_tostring(pL, -1));
		LOGE( msg);
		return -1;
	}

	// get the result
	hr = (INT)lua_tonumber(pL, -1);
	lua_pop(pL, 1);

	return hr;
}


INT LscLua_Create(LC_HANDLE _L)
{
	INT			hr	= -1;
	lua_State*	pL	= (lua_State*)_L;

	// Lua 스크립트 file에 있는 Lua_Create 함수를 호출
	lua_getglobal(pL, "Lua_Create");

	hr = lua_pcall(pL, 0, 1, 0);
	if(LUA_YIELD <= hr && hr <= LUA_ERRERR)
	{
		char msg[512]={0};
		sprintf(msg, "Lua_Create Failed: %s\n",  lua_tostring(pL, -1));
		LOGE( msg);
		return -1;
	}

	// get the result
	hr = (INT)lua_tonumber(pL, -1);
	lua_pop(pL, 1);

	if(LC_FAILED(hr))
		return -1;

	return hr;
}


LC_HANDLE Lua_Open()
{
	lua_State* g_pLua = NULL;

	// open the lua
	g_pLua	= lua_open();
	return g_pLua;
}


INT Lua_Load(LC_HANDLE _L, LC_HANDLE Name, INT Size)
{
	lua_State*	pL    = (lua_State*)_L;
	char*		sFile = (char*)Name;

	INT			hr = 0;

	if(NULL == pL || NULL == sFile)
		return LC_EFAIL;

	if( 0 >= Size)
	{
	#if defined(__ANDROID__)
		BYTE*		buf = NULL;
		ULONG		len = 0;
		const char*	sApkFile = LcSys_AndroidApkFile(NULL);

		hr = Lzip_UnzipbyName(&buf, &len, sApkFile, sFile, LC_TRUE);
		if(LC_FAILED(hr))
			return hr;


		hr = luaL_dobuffer(pL, (const char*)buf, len);
		LOGI("Lua do buffer: %d %s\n", hr, sFile);
		SAFE_FREE(	buf	);

	#else

		char sMedia[LC_MAX_PATH2]={0};
		hr = LcSys_ResourcePath(sMedia, sFile);
		if(LC_FAILED(hr))
			return LC_CANNT_ALLOC;

		//FILE* fp = fopen(sMedia, "rb");
		//
		//char*   buf = NULL;
		//UINT	len =0;
		//
		//fseek(fp, 0, SEEK_END);
		//len = ftell(fp);
		//buf = (char*)malloc(len);
		//fseek(fp, 0, SEEK_SET);
		//fread(buf, 1, len, fp);
		//fclose(fp);

		hr = luaL_dofile(pL, (const char*)sMedia);

	#endif
	}

	else
	{
		hr = luaL_dobuffer(pL, (const char*)Name, Size);
	}

	if(0 != hr)
		return LC_CANNT_ALLOC;

	return LC_OK;
}





INT Lua_OpenLibs(LC_HANDLE _L)
{
	lua_State* pL = (lua_State*)_L;

//	luaopen_base(pL);
//	luaopen_io(pL);
//	luaopen_math(pL);
//	luaopen_string(pL);
//	luaopen_table(pL);


	luaL_openlibs(pL);

	return LC_OK;
}

void Lua_Close(LC_HANDLE* _L)
{
	lua_State* pL = NULL;
	if(NULL == _L || NULL == *_L)
		return;

	pL = *((lua_State**)_L);
	lua_close(pL);

	*_L = NULL;
}



char* Lua_GetString(LC_HANDLE _L, INT idx)
{
	lua_State* pL = (lua_State*)_L;
	return (char*)lua_tostring(pL, idx);
}


