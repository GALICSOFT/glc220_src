// Implementation of the LscNet class.
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

#include <LcxNetwork.h>


#if !defined(_LUA_SRC_ORG)
#include "../LctLua.h"
#else
#include <LctLua.h>
#endif




////////////////////////////////////////////////////////////////////////////////
// Script

namespace LscNet_t
{
	static LscNet::ILhLst*	m_pCont = NULL;		// global
}


INT LscNet::Create(LC_HANDLE v)
{
	if(NULL == LscNet_t::m_pCont)
		LscNet_t::m_pCont = new LscNet::ILhLst;

	return 0;
}


INT	LscNet::Destroy()
{
	if(LscNet_t::m_pCont)
	{
		delete LscNet_t::m_pCont;
		LscNet_t::m_pCont = NULL;
	}

	return 0;
}


static const luaL_reg regLaNet[] =
{
	{"New"		,	(lua_CFunction)LscNet::Lsc_Alloc		},
	{"Release"	,	(lua_CFunction)LscNet::Lsc_Release		},

	{"Connect"	,	(lua_CFunction)LscNet::Lsc_Connect		},
	{"Listen"	,	(lua_CFunction)LscNet::Lsc_Listen		},
	{"Update"	,	(lua_CFunction)LscNet::Lsc_Update		},
	{"Send"		,	(lua_CFunction)LscNet::Lsc_Send			},
	{"Recv"		,	(lua_CFunction)LscNet::Lsc_Recv			},
	{"State"	,	(lua_CFunction)LscNet::Lsc_State		},
	{"Option"	,	(lua_CFunction)LscNet::Lsc_Option		},
	{"Ctrl"		,	(lua_CFunction)LscNet::Lsc_Ctrl			},
	{NULL, NULL},
};


INT LscNet::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;
	luaL_openlib(pL, "Lnet", regLaNet, 0);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////
// For list

LscNet::ILhLst::ILhLst()
{
	memset(vLst, 0, sizeof(LscH*) * MAX_NET);
}

LscNet::ILhLst::~ILhLst()
{
	PLCX_NET pObj;

	for(INT i=0; i<MAX_NET; ++i)
	{
		if(vLst[i])
		{
			pObj = (PLCX_NET)(vLst[i]->pIns);
			SAFE_RELEASE(	pObj	);

			delete vLst[i];
			vLst[i] = NULL;
		}
	}
}


INT LscNet::ILhLst::Alloc(LC_HANDLE inf)
{
	LscH*		pLsH = NULL;
	PLCX_NET	pObj = NULL	;
	TLC_SOCKET*	psc = (TLC_SOCKET*)inf;

	INT		nKey =-1;
	INT		hr	 =-1;


	// 빈공간을 찾는다.
	nKey = FindEmpty();

	// 꽉차있음
	if(-1 == nKey)
		return -1;


	hr = LcxEnt_CreateNet(NULL, &pObj, psc);
	if(LC_FAILED(hr))
	{
		LOGI("LscNet::ILhLst::Alloc Failed::%s :%u ----\n", psc->uIp, psc->uPt);
		return -1;
	}


	pLsH	= new LscH;
	pLsH->pIns	= pObj;

	vLst[nKey] = pLsH;
	return (nKey+1);
}


INT LscNet::ILhLst::Release(INT nKey)
{
	LscH*		pLsH = NULL;
	PLCX_NET	pObj = NULL;

	if(MAX_NET <= nKey || nKey<1)
		return -1;

	pLsH = vLst[nKey-1];
	if(NULL == pLsH)
		return -1;

	pObj = (PLCX_NET)( pLsH->pIns);

	SAFE_RELEASE(	pObj	);

	delete pLsH;

	vLst[nKey-1] = NULL;

	return 0;
}


LscH* LscNet::ILhLst::Find(INT nKey)
{
	if(MAX_NET <=nKey || nKey<1)
		return NULL;

	return vLst[nKey-1];
}


INT LscNet::ILhLst::FindEmpty()
{
	for(INT i=0; i<MAX_NET; ++i)
	{
		if(NULL == vLst[i])
			return i;
	}

	return -1;
}



////////////////////////////////////////////////////////////////////////////////
// for glue

INT	LscNet::Lsc_Alloc(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;
	TLC_SOCKET	psc = {0};
	INT			nKey=-1;

	char*		proto	= NULL;
	char*		cs_act	= NULL;
	char*		io_mdl	= NULL;
	char*		ip		= NULL;
	BYTE		uIp[16]	= {0};
	USHORT		uPt		= 0;

	INT		n = lua_gettop(pL);

	TLC_SOCKET	tsc={0};


	if(4 > n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	tsc.prot  = LCNET_TCP;
	tsc.type  = LCNET_CLIENT;
	tsc.io    = LCNET_SELECT;

	proto	= (char* )lua_tostring(pL, 1);
	cs_act	= (char* )lua_tostring(pL, 2);
	io_mdl	= (char* )lua_tostring(pL, 3);
	ip		= (char* )lua_tostring(pL, 4);
	uPt		= (USHORT)lua_tonumber(pL, 5);


	LcNet_InetAddr32(uIp, ip);

	if( 0 == LcStr_Stricmp( "udp",		proto) )	tsc.prot  = LCNET_UDP;


	tsc.uPt   = uPt;
	tsc.uIp   = uIp;


	nKey = LscNet_t::m_pCont->Alloc(&tsc);

	lua_pushnumber(pL, nKey);
	return 1;
}


INT LscNet::Lsc_Release(LC_HANDLE _L)
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

	iSize = LscNet_t::m_pCont->Release(nKey);

	// 남아 있는 사이즈를 돌려준다.
	lua_pushnumber(pL, iSize);
	return 1;
}



INT LscNet::Lsc_Connect(LC_HANDLE _L)
{
	INT			hr =-1;
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_NET	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscNet_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_NET)pLsH->pIns;

	hr = pObj->Query("Connect", NULL);


	lua_pushnumber(pL, hr);
	return 1;
}



INT LscNet::Lsc_Listen(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_NET	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscNet_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_NET)pLsH->pIns;



	lua_pushnumber(pL, 0);
	return 1;
}


INT LscNet::Lsc_Update(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_NET	pObj = NULL;
	INT			hr = 0;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscNet_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_NET)pLsH->pIns;

	hr = pObj->Query("Update", NULL);

	lua_pushnumber(pL, hr);
	return 1;
}


INT LscNet::Lsc_Send(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_NET	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);
	pLsH	= LscNet_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_NET)pLsH->pIns;


	lua_pushnumber(pL, 0);
	return 1;
}



INT LscNet::Lsc_Recv(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_NET	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);
	pLsH	= LscNet_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_NET)pLsH->pIns;


	lua_pushnumber(pL, 0);
	return 1;
}



INT LscNet::Lsc_State(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_NET	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey	= (INT)lua_tonumber(pL, 1);
	pLsH	= LscNet_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_NET)pLsH->pIns;

	INT nState = 0;
	pObj->GetAttrib(LCNET_STATE, &nState);

	lua_pushnumber(pL, nState);
	return 1;
}



INT LscNet::Lsc_Option(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_NET	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);
	pLsH	= LscNet_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_NET)pLsH->pIns;


	lua_pushnumber(pL, 0);
	return 1;
}



INT LscNet::Lsc_Ctrl(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_NET	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);
	pLsH	= LscNet_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCX_NET)pLsH->pIns;


	lua_pushnumber(pL, 0);
	return 1;
}


