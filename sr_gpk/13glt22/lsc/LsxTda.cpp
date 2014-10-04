// Implementation of the LsxTda class.
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

#include <LcType.h>
#include <LcMath.h>
#include <LcBase.h>
#include <LcAux.h>

#include <LcxEntity.h>

#if !defined(_LUA_SRC_ORG)
#include "../LctLua.h"
#else
#include <LctLua.h>
#endif




////////////////////////////////////////////////////////////////////////////////
// Script

namespace LsxTda_t
{
	static LC_HANDLE		m_pSpt	= NULL;		// sprite
	static LsxTda::ILhLst*	m_pCont = NULL;		// local
}


INT LsxTda::Create(LC_HANDLE v)
{
	LsxTda_t::m_pSpt = v;

	if(NULL == LsxTda_t::m_pCont)
		LsxTda_t::m_pCont = new LsxTda::ILhLst;

	return 0;
}


INT LsxTda::Destroy()
{
	if(LsxTda_t::m_pCont)
	{
		delete LsxTda_t::m_pCont;
		LsxTda_t::m_pCont = NULL;
	}

	return 0;
}


static const luaL_reg regLsxTda[] =
{
	{"New"			,	(lua_CFunction)LsxTda::Lsc_Alloc		},
	{"Release"		,	(lua_CFunction)LsxTda::Lsc_Release		},

	{"DrawOneFrame"	,	(lua_CFunction)LsxTda::Lsc_DrawOneFrame	},
	{"SetAnimation"	,	(lua_CFunction)LsxTda::Lsc_SetAnimation	},
	{"FrameMove"	,	(lua_CFunction)LsxTda::Lsc_FrameMove	},
	{"Render"		,	(lua_CFunction)LsxTda::Lsc_Render		},

	{NULL, NULL},
};


INT LsxTda::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "LsxTda", regLsxTda, 0);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////
// For list

LsxTda::ILhLst::ILhLst()
{
	memset(vLst, 0, sizeof(LscH*) * MAX_TSA);
	memset(vLstO, 0, sizeof(LscH*) * MAX_TSA_ORG);
}


LsxTda::ILhLst::~ILhLst()
{
	INT	i;
	PLCX_TDA pObj;

	for(i=0; i<MAX_TSA; ++i)
	{
		if(vLst[i])
		{
			pObj = (PLCX_TDA)(vLst[i]->pIns);
			SAFE_RELEASE(	pObj	);

			delete vLst[i];
			vLst[i] = NULL;
		}
	}


	for(i=0; i<MAX_TSA_ORG; ++i)
	{
		if(vLstO[i])
		{
			pObj = (PLCX_TDA)(vLstO[i]->pIns);
			SAFE_RELEASE(	pObj	);

			delete vLstO[i];
			vLstO[i] = NULL;
		}
	}
}


INT LsxTda::ILhLst::Alloc(char* sFile)
{
	LscH*		pLsH = NULL;
	LscH*		pLsO = NULL;
	PLCX_TDA	pObj = NULL;

	INT		nKey	=-1;
	INT		hr		=-1;

	nKey	= FindEmpty();

	// 꽉차있음
	if(0 > nKey)
		return -1;

	// Find In Original List
	pLsO	= this->FindOrg(sFile);

	// 원본이 없으면
	if(NULL == pLsO)
		return -1;


	hr = LcxEnt_CreateTda(NULL, &pObj, NULL, pLsO->pIns);
	if(LC_FAILED(hr))
		return -1;


	int rpt = 3;
//	pObj->SetAttrib(LCXM_REPEATE_COUNT, &rpt);

	int efr = 10;
//	pObj->SetAttrib(LCXM_END_FREEZE_FRAME, &efr);


	pLsH	= new LscH;
	pLsH->pIns	= pObj;
	strcpy(pLsH->sName, sFile);

	vLst[nKey] = pLsH;
	return (nKey+1);
}


INT LsxTda::ILhLst::Release(INT nKey)
{
	LscH*		pLsH = NULL;
	PLCX_TDA	pObj = NULL;

	if(MAX_TSA <=nKey || nKey<1)
		return -1;

	pLsH = vLst[nKey-1];

	if(NULL == pLsH)
		return -1;

	pObj = (PLCX_TDA)(pLsH->pIns);

	SAFE_RELEASE(	pObj	);

	delete pLsH;

	vLst[nKey-1] = NULL;

	return 0;
}


LscH* LsxTda::ILhLst::Find(INT nKey)
{
	if(MAX_TSA <=nKey || nKey<1)
		return NULL;

	return vLst[nKey-1];
}


LscH* LsxTda::ILhLst::Find(INT* nIdx, char* sFile)
{
	if(nIdx)
		*nIdx = -1;

	if(NULL == sFile || strlen(sFile)<1)
		return NULL;


	for(INT i=0; i<MAX_TSA; ++i)
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


INT LsxTda::ILhLst::FindEmpty()
{
	for(INT i=0; i<MAX_TSA; ++i)
	{
		if(NULL == vLst[i])
			return i;
	}

	return -1;
}


LscH* LsxTda::ILhLst::FindOrg(char* sFile)
{
	LscH*		pLsO = NULL;
	PLCX_TDA	pObj = NULL;
	INT			hr   = 0;
	INT	i =0;

	if(NULL == sFile || strlen(sFile)<1)
		return NULL;

	// if it exists..
	for(i=0; i<MAX_TSA_ORG; ++i)
	{
		if(NULL == vLstO[i])
			continue;

		char* sName = vLstO[i]->sName;
		if(0 == ::LcStr_Strcmp(sFile, sName))
		{
			pLsO = vLstO[i];
			break;
		}
	}

	if(pLsO)				// return Original Object
		return pLsO;


	if(LC_FAILED(LcxEnt_CreateTda(NULL, &pObj, (LC_HANDLE)sFile)))
		return NULL;

//	hr = pObj->SetAttrib(LCXM_EXEC_TEXTURE_LOAD, NULL);


	pLsO	= new LscH;
	pLsO->pIns	= pObj;
	strcpy(pLsO->sName, sFile);


	for(i=0; i<MAX_TSA_ORG; ++i)
	{
		if(NULL == vLstO[i])
		{
			vLstO[i] = pLsO;			// save original list
			break;
		}
	}
	
	return pLsO;
}




////////////////////////////////////////////////////////////////////////////////
// for glue

INT	LsxTda::Lsc_Alloc(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		nKey=-1;
	char*	sFile = NULL;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	sFile= (char*)lua_tostring(pL, 1);
	nKey = LsxTda_t::m_pCont->Alloc(sFile);


	if(0>nKey)
		printf("Tda File Alloc Failed: %s\n", sFile);

	// Key를 돌려 준다.
	lua_pushnumber(pL, nKey);
	return 1;
}







INT LsxTda::Lsc_Release(LC_HANDLE _L)
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

	iSize = LsxTda_t::m_pCont->Release(nKey);

	// 남아 있는 사이즈를 돌려준다.
	lua_pushnumber(pL, iSize);
	return 1;
}


INT LsxTda::Lsc_DrawOneFrame(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_TDA	pObj = NULL;
	PLC_SPRITE_S  pSpt = (PLC_SPRITE_S)LsxTda_t::m_pSpt;

	char*		FrameName = NULL;
	int			bOffset = 0;
	LCXVECTOR2	vcPos(0,0);
	LCXVECTOR2	vcScl(1,1);
	LCXVECTOR2	vcRot(0,0);
	FLOAT		fAngle=0;

	char		sC[16] = {0};
	LCXCOLOR	dC=LCXCOLOR(1,1,1,1);

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(3>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);
	pLsH	= LsxTda_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLCX_TDA)pLsH->pIns;



	FrameName = (char*)lua_tostring(pL, 2);
	bOffset   = (INT  )lua_tonumber(pL, 3);


	// 위치
	if(5 <= n)
	{
		vcPos.x	= (FLOAT)lua_tonumber(pL, 4);
		vcPos.y	= (FLOAT)lua_tonumber(pL, 5);
	}

	// 스케일
	if(7 <= n)
	{
		vcScl.x	= (FLOAT)lua_tonumber(pL, 6);
		vcScl.y	= (FLOAT)lua_tonumber(pL, 7);
	}

	// 회전
	if(10 <= n)
	{
		vcRot.x	= (FLOAT)lua_tonumber(pL, 8);
		vcRot.y	= (FLOAT)lua_tonumber(pL, 9);
		fAngle	= (FLOAT)lua_tonumber(pL,10);
		fAngle	= LCXToRadian(fAngle);
	}

	// 색상
	if(11 <= n || 6 == n)
	{
		UINT t;
		strcpy(sC, lua_tostring(pL, 11));
		sscanf(sC, "%x", &t);
		dC = t;
	}



	//(E) 2011-09-26 Editor: AFEW
	//ILCX_Tda::Begin();				// if there is not 2d camera it needs..

//	if(6 >= n)
//		pObj->DrawOneFrame(pSpt, FrameName, &vcPos,   NULL,   NULL,      0, &dC, bOffset);
//	else
//		pObj->DrawOneFrame(pSpt, FrameName, &vcPos, &vcScl, &vcRot, fAngle, &dC, bOffset);

	//(E) 2011-09-26 Editor: AFEW
	//ILCX_Tda::End();

	return 0;
}



INT LsxTda::Lsc_SetAnimation(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLCX_TDA	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	char*		sAni = NULL;

	if(2>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);
	pLsH	= LsxTda_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLCX_TDA)pLsH->pIns;


	sAni = (char*)lua_tostring(pL, 2);

//	pObj->SetAnimation(sAni);

	return 0;
}


INT LsxTda::Lsc_FrameMove(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT			hr = 0;
	LscH*		pLsH = NULL;
	PLCX_TDA	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);
	pLsH	= LsxTda_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLCX_TDA)pLsH->pIns;


	DOUBLE fTime = 0;
	LcSys_ElapseTime(&fTime);

//	pObj->SetAttrib(LCXM_ADVANCE_TIME, &fTime);

	int rpt = 3;
//	pObj->SetAttrib(LCXM_REPEATE_COUNT, &rpt);


	LCXVECTOR2 vcPos(60.0F, 60.F);
//	pObj->SetPosition(&vcPos);

//	hr = pObj->FrameMove();

	lua_pushnumber(pL, hr);
	return 1;
}


INT LsxTda::Lsc_Render(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT			hr = 0;
	LscH*		pLsH = NULL;
	PLCX_TDA	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);
	pLsH	= LsxTda_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLCX_TDA)pLsH->pIns;


	DOUBLE fTime = 0;
	LcSys_ElapseTime(&fTime);

//	pObj->SetAttrib(LCXM_ADVANCE_TIME, &fTime);



	int rpt = 3;
//	pObj->SetAttrib(LCXM_REPEATE_COUNT, &rpt);


	LCXVECTOR2 vcPos(60.0F, 60.F);
//	pObj->SetPosition(&vcPos);

//	hr = pObj->FrameMove();
//	pObj->Render();

	return 0;
}

