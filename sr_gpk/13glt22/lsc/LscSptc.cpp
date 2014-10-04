// Implementation of the LscSpc class.
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

#if !defined(_LUA_SRC_ORG)
#include "../LctLua.h"
#else
#include <LctLua.h>
#endif




////////////////////////////////////////////////////////////////////////////////
// Script

extern LscTex::ILhLst*	L_GetTextureList();

namespace LscSpc_t
{
	static LC_HANDLE		m_pSpt	= NULL;		// sprite
	static LscSpc::ILhLst*	m_pCont	= NULL;		// local
}


INT LscSpc::Create(LC_HANDLE v)
{
	LscSpc_t::m_pSpt = v;

	if(NULL == LscSpc_t::m_pCont)
		LscSpc_t::m_pCont = new LscSpc::ILhLst;

	return 0;
}


INT LscSpc::Destroy()
{
	if(LscSpc_t::m_pCont)
	{
		delete LscSpc_t::m_pCont;
		LscSpc_t::m_pCont = NULL;
	}

	return 0;
}


static const luaL_reg regLscSpc[] =
{
	{"New"		,	(lua_CFunction)LscSpc::Lsc_Alloc		},
	{"Release"	,	(lua_CFunction)LscSpc::Lsc_Release		},

	{"AddRect"	,	(lua_CFunction)LscSpc::Lsc_AddRect		},
	{"Draw"		,	(lua_CFunction)LscSpc::Lsc_Draw			},
	{"Blend"	,	(lua_CFunction)LscSpc::Lsc_Blending		},
	{"ZBias"	,	(lua_CFunction)LscSpc::Lsc_ZBias		},

	{NULL, NULL},
};


INT LscSpc::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "Lspc", regLscSpc, 0);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////
// For list

LscSpc::ILhLst::ILhLst()
{
	memset(vLst, 0, sizeof(LscH*) * MAX_TEX);
}

LscSpc::ILhLst::~ILhLst()
{
	PLC_SPRITE_C pObj;

	for(INT i=0; i<MAX_TEX; ++i)
	{
		if(vLst[i])
		{
			pObj = (PLC_SPRITE_C)(vLst[i]->pIns);
			SAFE_RELEASE(	pObj	);

			delete vLst[i];
			vLst[i] = NULL;
		}
	}
}


INT LscSpc::ILhLst::Alloc(INT texId)
{
	LscH*			pLsH = NULL;
	PLC_SPRITE_C	pObj = NULL;
	PLC_TEXTURE		pTex = NULL;

	INT		hr		=-1;
	INT		nKey	= FindEmpty();

	// 꽉차있음
	if(0 > nKey)
		return -1;


	// find from texture list
	LscTex::ILhLst*	pTexList = L_GetTextureList();
	if(pTexList)
	{
		LscH* pLs = pTexList->Find(texId);

		// texture is not exist
		if(NULL == pLs || NULL == (pTex = (PLC_TEXTURE)pLs->pIns) )
			return -1;
	}


	hr = LcDev_CreateSpriteC(NULL, &pObj, pTex);
	if(LC_FAILED(hr))
		return -1;

	pLsH	= new LscH;
	pLsH->pIns	= pObj;		// object
	sprintf(pLsH->sName, "%d", nKey);

	vLst[nKey] = pLsH;
	return (nKey+1);
}


INT LscSpc::ILhLst::Release(INT nKey)
{
	LscH*			pLsH = NULL;
	PLC_SPRITE_C	pObj = NULL;

	if(MAX_TEX <=nKey || nKey<1)
		return -1;

	pLsH = vLst[nKey-1];

	if(NULL == pLsH)
		return -1;

	pObj = (PLC_SPRITE_C)(pLsH->pIns);

	SAFE_RELEASE(	pObj	);

	delete pLsH;

	vLst[nKey-1] = NULL;

	return 0;
}


LscH* LscSpc::ILhLst::Find(INT nKey)
{
	if(MAX_TEX <=nKey || nKey<1)
		return NULL;

	return vLst[nKey-1];
}


INT LscSpc::ILhLst::FindEmpty()
{
	for(INT i=0; i<MAX_TEX; ++i)
	{
		if(NULL == vLst[i])
			return i;
	}

	return -1;
}


INT LscSpc::ILhLst::Invalid()
{
	LscH*		pLsH = NULL;
	PLC_SPRITE_C	pObj = NULL;

	for(INT i=0; i<MAX_TEX; ++i)
	{
		pLsH = vLst[i];

		if(NULL == pLsH)
			continue;

		pObj = (PLC_SPRITE_C)(pLsH->pIns);

		if(NULL == pObj)
			continue;

		pObj->Query(LCQUERY_INVALID, NULL);
	}

	return LC_OK;
}


INT LscSpc::ILhLst::Restore()
{
	LscH*		pLsH = NULL;
	PLC_SPRITE_C	pObj = NULL;

	for(INT i=0; i<MAX_TEX; ++i)
	{
		pLsH = vLst[i];

		if(NULL == pLsH)
			continue;

		pObj = (PLC_SPRITE_C)(pLsH->pIns);

		if(NULL == pObj)
			continue;

		pObj->Query(LCQUERY_RESTORE, NULL);
	}

	return LC_OK;
}



////////////////////////////////////////////////////////////////////////////////
// for glue

INT	LscSpc::Lsc_Alloc(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		nKey =-1;
	INT		nTex =-1;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	// texture Id
	nTex = (INT)lua_tonumber(pL, 1);
	nKey = LscSpc_t::m_pCont->Alloc(nTex);

	// Key를 돌려 준다.
	lua_pushnumber(pL, nKey);
	return 1;
}


INT LscSpc::Lsc_Release(LC_HANDLE _L)
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

	iSize = LscSpc_t::m_pCont->Release(nKey);

	// 남아 있는 사이즈를 돌려준다.
	lua_pushnumber(pL, iSize);
	return 1;
}




INT LscSpc::Lsc_AddRect(LC_HANDLE _L)
{
	lua_State*		pL = (lua_State*)_L;

	LscH*			pLsH = NULL;
	PLC_SPRITE_C	pObj = NULL;
	INT				nKey=-1;

	LCXRECT			rct(0,0,0,0);
	LCXVECTOR2		vcPos(0,0);
	LCXVECTOR2		vcScl(1,1);
	LCXVECTOR2		vcRot(0,0);
	FLOAT			fAngle = 0;
	char			sC[16] = {0};
	LCXCOLOR		dC=LCXCOLOR(1,1,1,1);
	INT				nMono = 0;

	INT				n = lua_gettop(pL);

	if(12>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	// 1. ID
	nKey = (INT)lua_tonumber(pL, 1);

	// 2. 영역
	rct.l	= (FLOAT)lua_tonumber(pL, 2);
	rct.t	= (FLOAT)lua_tonumber(pL, 3);
	rct.r	= (FLOAT)lua_tonumber(pL, 4);
	rct.b	= (FLOAT)lua_tonumber(pL, 5);
	rct.ToRegion();

	// 3. 위치
	vcPos.x	= (FLOAT)lua_tonumber(pL, 6);
	vcPos.y	= (FLOAT)lua_tonumber(pL, 7);

	// 4. 스케일
	vcScl.x	= (FLOAT)lua_tonumber(pL, 8);
	vcScl.y	= (FLOAT)lua_tonumber(pL, 9);

	// 5. Rotation Center
	vcRot.x	= (FLOAT)lua_tonumber(pL,10);
	vcRot.y	= (FLOAT)lua_tonumber(pL,11);

	// 6. Angle (Radian)
	fAngle	= (FLOAT)lua_tonumber(pL,12);

	fAngle	= LCXToRadian(fAngle);

	// 7. 색상
	if(13 <=n)
	{
		UINT t;
		strcpy(sC, lua_tostring(pL, 13));
		sscanf(sC, "%x", &t);
		dC= t;
	}

	// 8. 단색
	if(14 <=n)
		nMono = (INT)lua_tonumber(pL, 14);


	pLsH	= LscSpc_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL== pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLC_SPRITE_C)pLsH->pIns;

	pObj->AddRect(&rct, &vcPos, &vcScl, &vcRot, fAngle, &dC);

	lua_pushnumber(pL, 0);
	return 1;
}


INT LscSpc::Lsc_Draw(LC_HANDLE _L)
{
	lua_State*		pL = (lua_State*)_L;

	LscH*			pLsH = NULL;
	PLC_SPRITE_C	pObj = NULL;


	INT				nKey=-1;
	INT				n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey	= (INT)lua_tonumber(pL, 1);
	pLsH	= LscSpc_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL== pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLC_SPRITE_C)pLsH->pIns;
	pObj->Draw();

	lua_pushnumber(pL, 0);
	return 1;
}


INT LscSpc::Lsc_Blending(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_SPRITE  pSpt = (PLC_SPRITE)LscSpc_t::m_pSpt;

	INT			hr=-1;
	char*		Src= NULL;
	char*		Dst= NULL;
	INT			nSrc= LC_ST_SRC_ALPHA;
	INT			nDst= LC_ST_ONE_MINUS_SRC_ALPHA;

	INT			n = lua_gettop(pL);

	if(2 <=n)
	{
		Src = (char*)lua_tostring(pL, 1);
		Dst = (char*)lua_tostring(pL, 2);

		if     (0 == LcStr_Stricmp(Src, "ZERO"))				nSrc = LC_ST_ZERO;
		else if(0 == LcStr_Stricmp(Src, "ONE"))					nSrc = LC_ST_ONE;
		else if(0 == LcStr_Stricmp(Src, "SRC_COLOR"))			nSrc = LC_ST_SRC_COLOR;
		else if(0 == LcStr_Stricmp(Src, "ONE_MINUS_SRC_COLOR"))	nSrc = LC_ST_ONE_MINUS_SRC_COLOR;
		else if(0 == LcStr_Stricmp(Src, "SRC_ALPHA"))			nSrc = LC_ST_SRC_ALPHA;
		else if(0 == LcStr_Stricmp(Src, "ONE_MINUS_SRC_ALPHA"))	nSrc = LC_ST_ONE_MINUS_SRC_ALPHA;
		else if(0 == LcStr_Stricmp(Src, "DST_ALPHA"))			nSrc = LC_ST_DST_ALPHA;
		else if(0 == LcStr_Stricmp(Src, "ONE_MINUS_DST_ALPHA"))	nSrc = LC_ST_ONE_MINUS_DST_ALPHA;
		else if(0 == LcStr_Stricmp(Src, "DST_COLOR"))			nSrc = LC_ST_DST_COLOR;
		else if(0 == LcStr_Stricmp(Src, "ONE_MINUS_DST_COLOR"))	nSrc = LC_ST_ONE_MINUS_DST_COLOR;
		else if(0 == LcStr_Stricmp(Src, "SRC_ALPHA_SATURATE"))	nSrc = LC_ST_SRC_ALPHA_SATURATE;

		if     (0 == LcStr_Stricmp(Dst, "ZERO"))				nDst = LC_ST_ZERO;
		else if(0 == LcStr_Stricmp(Dst, "ONE"))					nDst = LC_ST_ONE;
		else if(0 == LcStr_Stricmp(Dst, "SRC_COLOR"))			nDst = LC_ST_SRC_COLOR;
		else if(0 == LcStr_Stricmp(Dst, "ONE_MINUS_SRC_COLOR"))	nDst = LC_ST_ONE_MINUS_SRC_COLOR;
		else if(0 == LcStr_Stricmp(Dst, "SRC_ALPHA"))			nDst = LC_ST_SRC_ALPHA;
		else if(0 == LcStr_Stricmp(Dst, "ONE_MINUS_SRC_ALPHA"))	nDst = LC_ST_ONE_MINUS_SRC_ALPHA;
		else if(0 == LcStr_Stricmp(Dst, "DST_ALPHA"))			nDst = LC_ST_DST_ALPHA;
		else if(0 == LcStr_Stricmp(Dst, "ONE_MINUS_DST_ALPHA"))	nDst = LC_ST_ONE_MINUS_DST_ALPHA;
		else if(0 == LcStr_Stricmp(Dst, "DST_COLOR"))			nDst = LC_ST_DST_COLOR;
		else if(0 == LcStr_Stricmp(Dst, "ONE_MINUS_DST_COLOR"))	nDst = LC_ST_ONE_MINUS_DST_COLOR;
		else if(0 == LcStr_Stricmp(Dst, "SRC_ALPHA_SATURATE"))	nDst = LC_ST_SRC_ALPHA_SATURATE;
	}


	hr = pSpt->SetRenderState(LC_RS_BLEND_SRC , nSrc);
	hr = pSpt->SetRenderState(LC_RS_BLEND_DEST, nDst);

	lua_pushnumber(pL, hr);
	return 1;
}


INT LscSpc::Lsc_ZBias(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_SPRITE  pSpt = (PLC_SPRITE)LscSpc_t::m_pSpt;

	INT			hr=-1;
	FLOAT		zbias= 0;
	INT			n = lua_gettop(pL);

	if(1 >n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	zbias = (FLOAT)lua_tonumber(pL, 1);
	
	hr = pSpt->SetRenderState(LC_RS_ZBIAS, *((INT*)&zbias));
	lua_pushnumber(pL, hr);
	return 1;
}



