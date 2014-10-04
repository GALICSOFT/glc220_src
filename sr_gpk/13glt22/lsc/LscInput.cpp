// Implementation of the LscInput class.
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

#include <LcType.h>
#include <LcMath.h>
#include <LcBase.h>
#include <LcStdev.h>
#include <LcxMedia.h>
#include <LcxEntity.h>

#if !defined(_LUA_SRC_ORG)
#include "../LctLua.h"
#else
#include <LctLua.h>
#endif



////////////////////////////////////////////////////////////////////////////////
// Script

namespace LscInput_t
{
	static LC_HANDLE m_pInput = NULL;
}


INT LscInput::Create(LC_HANDLE v)
{
	LscInput_t::m_pInput = v;
	return 0;
}

INT LscInput::Destroy()
{
	return 0;
}


static const luaL_reg regLscInput[] =
{
	{"KeyboardAll"	,	(lua_CFunction)LscInput::Lsc_KeyboardAll	},
	{"KeyboardOne"	,	(lua_CFunction)LscInput::Lsc_KeyboardOne	},
	{"MousePos"		,	(lua_CFunction)LscInput::Lsc_MousePos		},
	{"MouseDelta"	,	(lua_CFunction)LscInput::Lsc_MouseDelta		},
	{"MouseEvnt"	,	(lua_CFunction)LscInput::Lsc_MouseEvnt		},
	{"MouseCount"	,	(lua_CFunction)LscInput::Lsc_MouseCount		},

	{NULL, NULL},
};


INT LscInput::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "Lin", regLscInput, 0);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////
// for glue

INT LscInput::Lsc_KeyboardAll(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLCX_INPUT	pInput = (PLCX_INPUT)(LscInput_t::m_pInput);
	BYTE*		pKeyMap= (BYTE*)pInput->GetKeyMap();


	for(INT i=0; i<256; ++i)
		lua_pushnumber(pL, pKeyMap[i]);


	return 256;
}


INT LscInput::Lsc_KeyboardOne(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLCX_INPUT pInput = (PLCX_INPUT)(LscInput_t::m_pInput);

	INT		nKey  = -1;
	INT		KeySt = -1;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		//	->SetShowState(FALSE);
		lua_pushnumber(pL, 0);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);
	if(0 >nKey || nKey >255)
	{
		lua_pushnumber(pL, 0);
		return 1;
	}

	KeySt = pInput->KeyState(nKey);

	lua_pushnumber(pL, KeySt);
	return 1;
}

INT LscInput::Lsc_MousePos(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLCX_INPUT	pInput = (PLCX_INPUT)(LscInput_t::m_pInput);
	FLOAT*		pV = NULL;
	INT			n = lua_gettop(pL);
	INT			idx = 0;

	if(1 <= n)
	{
		idx = (INT)lua_tonumber(pL, 1);
		--idx;
	}


	pV = (FLOAT*)pInput->GetMtsPos(idx);

	INT	mouseX	= INT(pV[0]);
	INT	mouseY	= INT(pV[1]);
	INT	mouseZ	= INT(pV[2]);

	lua_pushnumber(pL, mouseX);
	lua_pushnumber(pL, mouseY);
	lua_pushnumber(pL, mouseZ);
	return 3;
}

INT LscInput::Lsc_MouseDelta(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLCX_INPUT	pInput = (PLCX_INPUT)(LscInput_t::m_pInput);
	FLOAT*		pV = NULL;
	INT			n = lua_gettop(pL);
	INT			idx = 0;

	if(1 <= n)
	{
		idx = (INT)lua_tonumber(pL, 1);
		--idx;
	}


	pV = (FLOAT*)pInput->GetMtsGrd(idx);

	lua_pushnumber(pL, pV[0]);
	lua_pushnumber(pL, pV[1]);
	lua_pushnumber(pL, pV[2]);
	return 3;
}


INT LscInput::Lsc_MouseEvnt(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLCX_INPUT	pInput = (PLCX_INPUT)(LscInput_t::m_pInput);
	INT			n = lua_gettop(pL);
	INT			idx = 0;
	INT			v = 0;

	if(0 < n)
	{
		idx = (INT)lua_tonumber(pL, 1);
		--idx;

		v = pInput->MtsState(idx);
		lua_pushnumber(pL, v);
		return 1;
	}

	// return all events
	for(int i=0; i<LC_MAX_TSB; ++i)
	{
		v = pInput->MtsState(i);
		lua_pushnumber(pL, v);
	}

	return LC_MAX_TSB;
}


INT LscInput::Lsc_MouseCount(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT  v0 = LcStv_InputCountMts();

	lua_pushnumber(pL, v0);
	return 1;
}



