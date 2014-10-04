// Implementation of the LscSmd class.
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
#include <LcBase.h>
#include <LcAux.h>
#include <LcxMedia.h>

#if !defined(_LUA_SRC_ORG)
#include "../LctLua.h"
#else
#include <LctLua.h>
#endif




////////////////////////////////////////////////////////////////////////////////
// Script

namespace LscSmd_t
{
	static LscSmd::ILhLst*	m_pCont = NULL;		// local
}


INT LscSmd::Create(LC_HANDLE v)
{
	if(NULL == LscSmd_t::m_pCont)
		LscSmd_t::m_pCont = new LscSmd::ILhLst;

	return 0;
}


INT	LscSmd::Destroy()
{
	if(LscSmd_t::m_pCont)
	{
		delete LscSmd_t::m_pCont;
		LscSmd_t::m_pCont = NULL;
	}

	return 0;
}


INT	LscSmd::Invalid()
{
	if(LscSmd_t::m_pCont)
		LscSmd_t::m_pCont->Invalid();

	return 0;
}

INT	LscSmd::Restore()
{
	if(LscSmd_t::m_pCont)
		LscSmd_t::m_pCont->Restore();

	return 0;
}


static const luaL_reg regLaSnd[] =
{
	{"New"		,	(lua_CFunction)LscSmd::Lsc_Alloc		},
	{"Release"	,	(lua_CFunction)LscSmd::Lsc_Release		},

	{"Play"		,	(lua_CFunction)LscSmd::Lsc_SoundPlay	},
	{"Stop"		,	(lua_CFunction)LscSmd::Lsc_SoundStop	},
	{"Pause"	,	(lua_CFunction)LscSmd::Lsc_SoundPause	},
	{"Reset"	,	(lua_CFunction)LscSmd::Lsc_SoundReset	},
	{"Volume"	,	(lua_CFunction)LscSmd::Lsc_SoundVolume	},
	{"Repeat"	,	(lua_CFunction)LscSmd::Lsc_SetRepeat	},
	{"State"	,	(lua_CFunction)LscSmd::Lsc_GetState		},
	{"Draw"		,	(lua_CFunction)LscSmd::Lsc_Draw			},

	{NULL, NULL},
};

INT LscSmd::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "Lsmd", regLaSnd, 0);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////
// For list

LscSmd::ILhLst::ILhLst()
{
	memset(vLst, 0, sizeof(LscH*) * MAX_SMD);
}


LscSmd::ILhLst::~ILhLst()
{
	PLCX_SOUND pObj;

	for(INT i=0; i<MAX_SMD; ++i)
	{
		if(vLst[i])
		{
			pObj = (PLCX_SOUND)(vLst[i]->pIns);
			SAFE_RELEASE(	pObj	);

			delete vLst[i];
			vLst[i] = NULL;
		}
	}
}


INT LscSmd::ILhLst::Alloc(LC_HANDLE* pOut, char* sFile, UINT dFlag)
{
	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL	;

	INT		nKey =-1;
	INT		hr	 =-1;

	nKey = FindEmpty();	// 빈공간을 찾는다.

	if(-1 == nKey)		// 꽉차있음
		return -1;

	hr = LcxSmd_CreateSoundFromFile(NULL, &pObj, sFile);
	if(LC_FAILED(hr))
	{
		LOGI("LscSmd::ILhLst::Alloc Failed::%s ----\n", sFile);
		return -1;
	}

	pLsH	= new LscH;
	pLsH->pIns	= pObj;

	vLst[nKey] = pLsH;
	return (nKey+1);
}


INT LscSmd::ILhLst::Release(INT nKey)
{
	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL;

	if(MAX_SMD <= nKey || nKey<1)
		return -1;

	pLsH = vLst[nKey-1];
	if(NULL == pLsH)
		return -1;

	pObj = (PLCX_SOUND)( pLsH->pIns);

	SAFE_RELEASE(	pObj	);

	delete pLsH;

	vLst[nKey-1] = NULL;

	return 0;
}


LscH* LscSmd::ILhLst::Find(INT nKey)
{
	if(MAX_SMD <=nKey || nKey<1)
		return NULL;

	return vLst[nKey-1];
}


INT LscSmd::ILhLst::FindEmpty()
{
	for(INT i=0; i<MAX_SMD; ++i)
	{
		if(NULL == vLst[i])
			return i;
	}

	return -1;
}


INT LscSmd::ILhLst::Invalid()
{
	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL;

	for(INT i=0; i<MAX_SMD; ++i)
	{
		pLsH = vLst[i];
		if(NULL == pLsH || NULL == pLsH->pIns)
			continue;

		pObj = (PLCX_SOUND)(pLsH->pIns);
		pObj->Query(LCQUERY_INVALID, NULL);
	}

	return LC_OK;
}


INT LscSmd::ILhLst::Restore()
{
	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL;

	for(INT i=0; i<MAX_SMD; ++i)
	{
		pLsH = vLst[i];
		if(NULL == pLsH || NULL == pLsH->pIns)
			continue;

		pObj = (PLCX_SOUND)(pLsH->pIns);
		pObj->Query(LCQUERY_RESTORE, NULL);
	}

	return LC_OK;
}




////////////////////////////////////////////////////////////////////////////////
// for glue

INT	LscSmd::Lsc_Alloc(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		nKey=-1;
	char	sFile[LC_MAX_PATH]={0};
	UINT	dFlag= 0;

	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	if(1 >= n)
	{
		strcpy(sFile, lua_tostring(pL, 1));
	}

	if(2 >= n)
	{
		dFlag = (UINT)lua_tonumber(pL, 2);
	}
	else
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	// d: 16 --> DSBCAPS_CTRL3D( 3D Sound )
	nKey = LscSmd_t::m_pCont->Alloc(NULL, sFile, dFlag);

	lua_pushnumber(pL, nKey);
	return 1;
}


INT LscSmd::Lsc_Release(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		nKey=-1;
	INT		iSize = 0;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	iSize = LscSmd_t::m_pCont->Release(nKey);

	// 남아 있는 사이즈를 돌려준다.
	lua_pushnumber(pL, iSize);
	return 1;
}



INT LscSmd::Lsc_SoundPlay(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL;

	INT		nKey=-1;
	INT		Repeat=1;
	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);


	pLsH	= LscSmd_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pObj = (PLCX_SOUND)pLsH->pIns;

	// Loop
	if(2<= n)
		Repeat = (INT)lua_tonumber(pL, 2);

	pObj->Stop();
	pObj->SetRepeat(Repeat);
	pObj->Play();

	lua_pushnumber(pL, 0);
	return 1;
}



INT LscSmd::Lsc_SoundStop(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);


	pLsH = LscSmd_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_SOUND)pLsH->pIns;
	pObj->Stop();

	lua_pushnumber(pL, 0);
	return 1;
}


INT LscSmd::Lsc_SoundPause(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);


	pLsH = LscSmd_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_SOUND)pLsH->pIns;
	pObj->Pause();

	lua_pushnumber(pL, 0);
	return 1;
}




INT LscSmd::Lsc_SoundReset(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscSmd_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pObj = (PLCX_SOUND)pLsH->pIns;
	pObj->Reset();

	lua_pushnumber(pL, 0);
	return 1;
}




INT LscSmd::Lsc_SoundVolume(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);
	FLOAT	lVolume=0;

	if(n<2)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey	= (INT)lua_tonumber(pL, 1);
	lVolume = (FLOAT)lua_tonumber(pL, 2);


	pLsH	= LscSmd_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_SOUND)pLsH->pIns;
	pObj->SetVolume(lVolume);

	lua_pushnumber(pL, 0);
	return 1;
}


INT LscSmd::Lsc_SetRepeat(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL;

	INT		nKey=-1;
	INT		Repeat=0;
	INT		n = lua_gettop(pL);

	if(n<2)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey	= (INT)lua_tonumber(pL, 1);
	Repeat	= (INT)lua_tonumber(pL, 2);

	pLsH	= LscSmd_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_SOUND)pLsH->pIns;
	pObj->SetRepeat(Repeat);

	lua_pushnumber(pL, 0);
	return 1;
}


INT LscSmd::Lsc_GetState(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL;

	INT		nKey=-1;
	INT		nVal=0;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey	= (INT)lua_tonumber(pL, 1);

	pLsH	= LscSmd_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pObj = (PLCX_SOUND)pLsH->pIns;
	nVal = pObj->GetStatus();


	lua_pushnumber(pL, nVal);
	return 1;
}


INT LscSmd::Lsc_Draw(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_SOUND	pObj = NULL;

	INT		nKey=-1;
	INT		nVal=0;
	INT		iScnX;
	INT		iScnY;
	INT		nScnW;
	INT		nScnH;
	char	sColor[32];
	UINT	dColor;

	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey	= (INT)lua_tonumber(pL, 1);
	iScnX	= (INT)lua_tonumber(pL, 2);
	iScnY	= (INT)lua_tonumber(pL, 3);
	nScnW	= (INT)lua_tonumber(pL, 4);
	nScnH	= (INT)lua_tonumber(pL, 5);

	strcpy(sColor, lua_tostring(pL, 6));
	sscanf(sColor,"%x", &dColor);

	pLsH	= LscSmd_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}




	pObj = (PLCX_SOUND)pLsH->pIns;

	pObj->Query((char*)"Set Screen Pos X", &iScnX);
	pObj->Query((char*)"Set Screen Pos Y", &iScnY);
	pObj->Query((char*)"Set Screen Width", &nScnW);
	pObj->Query((char*)"Set Screen Height", &nScnH);
	pObj->Query((char*)"Set Diffuse"	, &dColor);

	pObj->Query((char*)"Draw",	&nVal);

	lua_pushnumber(pL, nVal);
	return 1;
}
