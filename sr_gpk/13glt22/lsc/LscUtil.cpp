// Implementation of the LscUtil class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1200
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <LcType.h>
#include <LcMath.h>
#include <LcBase.h>
#include <LcAux.h>

#if !defined(_LUA_SRC_ORG)
#include "../LctLua.h"
#else
#include <LctLua.h>
#endif


#if defined(__APPLE__) || defined(__IPHONE__)

extern int Sns_SendScoreToGameCenter(char* board_id, int score);
extern int Sns_ShowLeaderBoard(char* board_id);

#else

int Sns_SendScoreToGameCenter(char* v1,int v2)	{	return 0;	}
int Sns_ShowLeaderBoard(char* v1)				{	return 0;	}

#endif



////////////////////////////////////////////////////////////////////////////////
// Script

INT LscUtil::Create(LC_HANDLE v)
{
	return 0;
}


INT LscUtil::Destroy()
{
	return 0;
}


static const luaL_reg regLscUtil[] =
{
	{"Mod"			,	(lua_CFunction)LscUtil::Lsc_Mod				},
	{"Rand"			,	(lua_CFunction)LscUtil::Lsc_Rand			},
	{"GetTickCount"	,	(lua_CFunction)LscUtil::Lsc_GetTickCount	},
	{"TimeGetTime"	,	(lua_CFunction)LscUtil::Lsc_TimeGetTime		},

	{"Int"			,	(lua_CFunction)LscUtil::Lsc_CastInt			},
	{"GetWord"		,	(lua_CFunction)LscUtil::Lsc_GetWord			},
	{"GetWords"		,	(lua_CFunction)LscUtil::Lsc_GetWords		},
	{"GetWordLen"	,	(lua_CFunction)LscUtil::Lsc_GetWordLen		},

	// String
	{"StrLen"		,	(lua_CFunction)LscUtil::Lsc_StrLen			},
	{"StrCmp"		,	(lua_CFunction)LscUtil::Lsc_StrCmp			},
	{"StrToInt"		,	(lua_CFunction)LscUtil::Lsc_StrToInt		},
	{"StrToDouble"	,	(lua_CFunction)LscUtil::Lsc_StrToDouble		},
	{"ToHexa"		,	(lua_CFunction)LscUtil::Lsc_ToHexa			},
	{"ToHexaByte"	,	(lua_CFunction)LscUtil::Lsc_ToHexaByte		},

	// SNS
	{"SendScoreToGameCenter", (lua_CFunction)LscUtil::SendToGameCenter	},
	{"ShowLeaderBoard"		, (lua_CFunction)LscUtil::ShowLeaderBoard	},
	{NULL, NULL},
};


INT LscUtil::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "Lutil", regLscUtil, 0);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////
// for glue

INT	LscUtil::Lsc_Mod(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	DOUBLE v1= (DOUBLE)lua_tonumber(pL, 1);
	DOUBLE v2= (DOUBLE)lua_tonumber(pL, 2);

	DOUBLE v3 = v1 -floor(v1/v2) * v2;

	lua_pushnumber(pL, v3);
	return 1;
}



INT LscUtil::Lsc_Rand(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		v=0, v1=0, v2=0;

	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnil(pL);
		return 1;
	}


	v1 = (INT)lua_tonumber(pL, 1);

	if(1 == n)
	{
		v = 1+ rand() %v1;
		lua_pushnumber(pL, v);
		return 1;
	}

	v2 = (INT)lua_tonumber(pL, 2);

	if(v1 == v2)
	{
		v = v1;
	}

	if(v1>v2)
	{
		v1 ^= v2;
		v2 ^= v1;
		v1 ^= v2;
		v = v1 + rand() %(1+v2 - v1);
	}
	else
		v = v1 + rand() %(1+v2 - v1);

	
	lua_pushnumber(pL, v);
	return 1;
}



INT LscUtil::Lsc_GetTickCount(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	DOUBLE v1= ::LcSys_GetTickCount();
	lua_pushnumber(pL, v1);
	return 1;
}


INT LscUtil::Lsc_TimeGetTime(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	DOUBLE v1= ::LcSys_GetTickCount();
	lua_pushnumber(pL, v1);
	return 1;
}





INT	LscUtil::Lsc_CastInt(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT v1= (INT)lua_tonumber(pL, 1);

	lua_pushnumber(pL, v1);
	return 1;
}



INT	LscUtil::Lsc_GetWord(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	char* s= (char*)lua_tostring(pL, 1);
	INT nIdx = (INT)lua_tonumber(pL, 2);


	static char sWord[4];
	char*	p;
	INT		nCnt =0;
	INT		b2B= 0;

	memset(sWord, 0, sizeof sWord );
	--nIdx;
	p = s;

	INT iLen = (INT)::strlen(s);

	while(p<s+iLen)
	{
		// 2바이트면 0보다 작다.
		b2B = ((*p) &0x80);

		if(nCnt >= nIdx)
		{
			sWord[0]= *p;

			if( b2B)
				sWord[1]= *(p+1);

			break;
		}

		++nCnt;
		++p;

		if(b2B)
			++p;
	}

	lua_pushstring(pL, sWord);

	return 1;
}

static char	sGetWords[65536]={0};

INT LscUtil::Lsc_GetWords(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;


	char* s= (char*)lua_tostring(pL, 1);
	INT nIdx = (INT)lua_tonumber(pL, 2);

	char*	p;
	char*	pEnd;
	INT		nCnt =0;
	INT		b2B= 0;

	memset(sGetWords, 0, sizeof sGetWords);

	p = s;

	INT iLen = (INT)::strlen(s);

	while( p<s+iLen)
	{
		// 2바이트면 0보다 작다.
		b2B = ((*p) &0x80);

		if(nCnt >= nIdx)
		{
			pEnd = p;

			if( b2B)
				pEnd = p+1;

			break;
		}

		++nCnt;
		++p;

		if(b2B)
			++p;
	}

	if(p<s+iLen)
		memcpy(sGetWords, s, pEnd-s);
	else
		strcpy(sGetWords, s);

	lua_pushstring(pL, sGetWords);

	return 1;
}

INT	LscUtil::Lsc_GetWordLen(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	char* s= (char*)lua_tostring(pL, 1);
	char*	p;
	INT		nCnt =0;
	INT		b2B= 0;

	p = s;

	INT iLen = (INT)::strlen(s);

	while( p<s+iLen)
	{
		// 2바이트면 0보다 작다.
		b2B = ((*p) &0x80);
		++nCnt;
		++p;

		if(b2B)
			++p;
	}

	lua_pushnumber(pL, nCnt);
	return 1;
}



INT	LscUtil::Lsc_StrLen(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	char* s= (char*)lua_tostring(pL, 1);

	INT iLen = (INT)::strlen(s);

	lua_pushnumber(pL, iLen);
	return 1;
}


INT	LscUtil::Lsc_StrCmp(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	char* s1= (char*)lua_tostring(pL, 1);
	char* s2= (char*)lua_tostring(pL, 2);

	INT hr = ::LcStr_Strcmp(s1, s2);

	lua_pushnumber(pL, hr);
	return 1;
}


INT	LscUtil::Lsc_StrToInt(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	char* s= (char*)lua_tostring(pL, 1);

	INT hr = atoi(s);

	lua_pushnumber(pL, hr);
	return 1;
}



INT	LscUtil::Lsc_StrToDouble(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	char* s= (char*)lua_tostring(pL, 1);

	double hr = atof(s);

	lua_pushnumber(pL, hr);
	return 1;
}


INT	LscUtil::Lsc_ToHexa(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	char sTmp[32]={0};
	char sDst[32]={0};

	int v = (int)lua_tonumber(pL, 1);

	sprintf(sTmp, "0000000%X", v);

	int len = strlen(sTmp);

	for(int i=len-1, j=8-1; j>=0; )
		sDst[j--] = sTmp[i--];

	lua_pushstring(pL, sDst);
	return 1;
}


INT	LscUtil::Lsc_ToHexaByte(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	char sTmp[32]={0};
	char sDst[32]={0};

	int v = (int)lua_tonumber(pL, 1);

	sprintf(sTmp, "0%X", v);

	int len = strlen(sTmp);

	for(int i=len-1, j=2-1; j>=0; )
		sDst[j--] = sTmp[i--];

	lua_pushstring(pL, sDst);
	return 1;
}



////////////////////////////////////////////////////////////////////////////////
// Implementations SNS

int	LscUtil::SendToGameCenter(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	int hr = 0;
	char* board_id = (char*)lua_tostring(pL, 1);
	int   score    = (int  )lua_tonumber(pL, 2);

	// Action
	hr = Sns_SendScoreToGameCenter(board_id, score);

	return 0;
}


int LscUtil::ShowLeaderBoard(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	int hr = 0;
	char* board_id = (char*)lua_tostring(pL, 1);

	// Action
	hr = Sns_ShowLeaderBoard(board_id);
	return 0;
}



