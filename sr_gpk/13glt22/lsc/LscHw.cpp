// Implementation of the LscHw class.
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
#include <LcStdev.h>

#if !defined(_LUA_SRC_ORG)
#include "../LctLua.h"
#else
#include <LctLua.h>
#endif



////////////////////////////////////////////////////////////////////////////////
// Script

INT LscHw::Create(LC_HANDLE v)
{
	return 0;
}

INT LscHw::Destroy()
{
	return 0;
}


static const luaL_reg regLscHw[] =
{
	{"HapticPlay"		,	(lua_CFunction)LscHw::Lsc_HapticPlay		},
	{"HapticPattern"	,	(lua_CFunction)LscHw::Lsc_HapticPattern		},
	{"HapticStop"		,	(lua_CFunction)LscHw::Lsc_HapticStop		},

	{"Sensor"			,	(lua_CFunction)LscHw::Lsc_Sensor			},

	{"MasterVolumeUp"	,	(lua_CFunction)LscHw::Lsc_MasterVolumeUp	},
	{"MasterVolumeDown"	,	(lua_CFunction)LscHw::Lsc_MasterVolumeDown	},
	{"MasterVolumeMute"	,	(lua_CFunction)LscHw::Lsc_MasterVolumeMute	},

	{NULL, NULL},
};


INT LscHw::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "Lhw", regLscHw, 0);
	return 1;
}


////////////////////////////////////////////////////////////////////////////////
// for glue

// Haptic

INT	LscHw::Lsc_HapticPlay(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		hr = 0;
	INT		sec= 0;

	if(1> lua_gettop(pL))
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	sec = (INT)lua_tonumber(pL, 1);

	hr = LcStv_HapticPlay(sec);

	lua_pushnumber(pL, hr);
	return 1;
}


INT LscHw::Lsc_HapticPattern(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		hr = 0;
	INT		sec= 0;

	if(1> lua_gettop(pL))
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	sec = (INT)lua_tonumber(pL, 1);

	hr = LcStv_HapticPlay(sec);

	lua_pushnumber(pL, hr);
	return 1;
}


INT LscHw::Lsc_HapticStop(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT	hr = LcStv_HapticStop();

	lua_pushnumber(pL, hr);
	return 1;
}


////////////////////////////////////////////////////////////////////////////////
// Sensor

INT LscHw::Lsc_Sensor(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;
	INT	hr = 0;

	INT type = LC_GSN_GRAVITY;
	TLC_SENSOR v={0};


	if(1 <= lua_gettop(pL))
	{
		char*	src = (char*)lua_tostring(pL, 1);

		if     (0 == LcStr_Stricmp(src, "ACCELEROMETER"			))	type = LC_GSN_ACCELEROMETER			;
		else if(0 == LcStr_Stricmp(src, "MAGNETIC_FIELD"		))	type = LC_GSN_MAGNETIC_FIELD		;
		else if(0 == LcStr_Stricmp(src, "ORIENTATION"			))	type = LC_GSN_ORIENTATION			;
		else if(0 == LcStr_Stricmp(src, "GYROSCOPE"				))	type = LC_GSN_GYROSCOPE				;
		else if(0 == LcStr_Stricmp(src, "LIGHT"					))	type = LC_GSN_LIGHT					;
		else if(0 == LcStr_Stricmp(src, "PRESSURE"				))	type = LC_GSN_PRESSURE				;
		else if(0 == LcStr_Stricmp(src, "TEMPERATURE"			))	type = LC_GSN_TEMPERATURE			;
		else if(0 == LcStr_Stricmp(src, "PROXIMITY"				))	type = LC_GSN_PROXIMITY				;
		else if(0 == LcStr_Stricmp(src, "GRAVITY"				))	type = LC_GSN_GRAVITY				;
		else if(0 == LcStr_Stricmp(src, "LINEAR_ACCELERATION"	))	type = LC_GSN_LINEAR_ACCELERATION	;
		else if(0 == LcStr_Stricmp(src, "ROTATION_VECTOR"		))	type = LC_GSN_ROTATION_VECTOR		;
	}

	hr = LcStv_SensorRead(type, &v);

	lua_pushnumber(pL, v.x);
	lua_pushnumber(pL, v.y);
	lua_pushnumber(pL, v.z);
	return 3;
}


////////////////////////////////////////////////////////////////////////////////
// Master Volume

INT LscHw::Lsc_MasterVolumeUp(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;
	INT	hr = 0;

	//hr = LcStv_SensorRead();

	lua_pushnumber(pL, hr);
	return 1;
}

INT LscHw::Lsc_MasterVolumeDown(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;
	INT	hr = 0;

	lua_pushnumber(pL, hr);
	return 1;
}

INT LscHw::Lsc_MasterVolumeMute(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;
	INT	hr = 0;

	lua_pushnumber(pL, hr);
	return 1;
}

