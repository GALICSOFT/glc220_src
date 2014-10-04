// Implementation of the LscFont class.
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
#include <LcAux.h>
#include <LcxEntity.h>

#if !defined(_LUA_SRC_ORG)
#include "../LctLua.h"
#else
#include <LctLua.h>
#endif




////////////////////////////////////////////////////////////////////////////////
// Script

namespace LscFont_t
{
	static LC_HANDLE		m_pSpt  = NULL;		// sprite
	static LscFont::ILhLst*	m_pCont	= NULL;		// local
}


INT LscFont::Create(LC_HANDLE v)
{
	LscFont_t::m_pSpt = v;

	if(NULL == LscFont_t::m_pCont)
		LscFont_t::m_pCont = new LscFont::ILhLst;

	return 0;
}

INT	LscFont::Destroy()
{
	if(LscFont_t::m_pCont)
	{
		delete LscFont_t::m_pCont;
		LscFont_t::m_pCont = NULL;
	}

	return 0;
}


INT	LscFont::Invalid()
{
	if(LscFont_t::m_pCont)
		LscFont_t::m_pCont->Invalid();

	return 0;
}


INT	LscFont::Restore()
{
	if(LscFont_t::m_pCont)
		LscFont_t::m_pCont->Restore();

	return 0;
}


static const luaL_reg regLscFont[] =
{
	{"New"		,	(lua_CFunction)LscFont::Lsc_Alloc		},
	{"Release"	,	(lua_CFunction)LscFont::Lsc_Release		},

	{"Draw"		,	(lua_CFunction)LscFont::Lsc_FontDraw	},
	{"Setup"	,	(lua_CFunction)LscFont::Lsc_FontSetup	},
	{"Position"	,	(lua_CFunction)LscFont::Lsc_FontPos		},
	{"String"	,	(lua_CFunction)LscFont::Lsc_FontString	},
	{"Color"	,	(lua_CFunction)LscFont::Lsc_FontColor	},
	{"Scale"	,	(lua_CFunction)LscFont::Lsc_FontScale	},
	{"Align"	,	(lua_CFunction)LscFont::Lsc_FontAlign	},
	{"Count"	,	(lua_CFunction)LscFont::Lsc_FontCount	},
	{"Rect"		,	(lua_CFunction)LscFont::Lsc_FontRect	},

	{NULL, NULL},
};


INT LscFont::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "Lfont", regLscFont, 0);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////
// For list

LscFont::ILhLst::ILhLst()
{
	memset(vLst, 0, sizeof(LscH*) * MAX_FONT);
}

LscFont::ILhLst::~ILhLst()
{
	for(INT i=0; i<MAX_FONT; ++i)
	{
		if(vLst[i])
		{
			PLC_FONT pDst = (PLC_FONT)(vLst[i]->pIns);
			SAFE_RELEASE(	pDst	);

			delete vLst[i];
			vLst[i] = NULL;
		}
	}
}



INT LscFont::ILhLst::Alloc(LC_HANDLE*	pOut
						,	char*		sName
						,	INT			lHeight
						)
{
	LscH*		pLsH	= NULL;
	PLC_FONT	pObj	= NULL;

	INT			nKey	=-1;
	INT			hr		=-1;

	// 빈공간을 찾는다.
	nKey = FindEmpty();

	// 꽉차있음
	if(-1 == nKey)
		return -1;


	// Create Font
	char* p = strchr( sName, '.');
	if(NULL == p || 0 == LcStr_Stricmp("Default", sName))
	{
		TLC_FONT	tFont(LCCMD_FONT_DEFAULT, NULL, 0, lHeight);
		hr = LcDev_CreateFontFromMemory(NULL, &pObj, LscFont_t::m_pSpt, &tFont);
	}
	else
	{
		TLC_FONT	tFont(sName, lHeight);
		hr = LcDev_CreateFontFromFile(NULL, &pObj, LscFont_t::m_pSpt, &tFont);
	}


	if(LC_FAILED(hr))
		return -1;

	pLsH	= new LscH;
	pLsH->pIns	= pObj;

	vLst[nKey] = pLsH;
	return (nKey+1);
}


INT LscFont::ILhLst::Release(INT nKey)
{
	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;

	if(MAX_FONT <= nKey || nKey<1)
		return -1;


	pLsH = vLst[nKey-1];
	if(NULL == pLsH)
		return -1;


	pObj = (PLC_FONT)(pLsH->pIns);

	SAFE_RELEASE(	pObj	);

	delete pLsH;

	vLst[nKey-1] = NULL;

	return 0;
}


LscH* LscFont::ILhLst::Find(INT nKey)
{
	if(MAX_FONT<=nKey || nKey<1)
		return NULL;

	return vLst[nKey-1];
}


INT LscFont::ILhLst::FindEmpty()
{
	for(INT i=0; i<MAX_FONT; ++i)
	{
		if(NULL == vLst[i])
			return i;
	}

	return -1;
}


INT LscFont::ILhLst::Invalid()
{
	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;

	for(INT i=0; i<MAX_FONT; ++i)
	{
		pLsH = vLst[i];

		if(NULL == pLsH)
			continue;

		pObj = (PLC_FONT)(pLsH->pIns);

		if(NULL == pObj)
			continue;

		pObj->Query(LCQUERY_INVALID, NULL);
	}

	return LC_OK;
}


INT LscFont::ILhLst::Restore()
{
	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;

	for(INT i=0; i<MAX_FONT; ++i)
	{
		pLsH = vLst[i];

		if(NULL == pLsH)
			continue;

		pObj = (PLC_FONT)(pLsH->pIns);

		if(NULL == pObj)
			continue;

		pObj->Query(LCQUERY_RESTORE, NULL);
	}

	return LC_OK;
}




////////////////////////////////////////////////////////////////////////////////
// for glue

#define MTFNT_DX	0
#define MTFNT_LN	1


// Font
// Name, Height, Font Weight , Italic
INT	LscFont::Lsc_Alloc(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		nKey=-1;

	char	sName[LC_MAX_PATH]={0};
	UINT	lH = 12;
	UINT	lW = 400;
	UINT	iL = 0;

	UINT	dColorStr = 0xFFFFFFFF;
	UINT	dColorBck = 0xFFFFFFFF;
	INT		iThckX = 0;
	INT		iThckY = 0;

	UINT	lWeight = 400;

	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
//		LcUtil_ErrMsgBox("Font Load Error");
		return 1;
	}

	strcpy(sName, lua_tostring(pL, 1));


	if(2<=n)
		lH = (UINT)lua_tonumber(pL, 2);

	if(3<=n)
		lW = (UINT)lua_tonumber(pL, 3);

	if(4<=n)
		iL = (UINT)lua_tonumber(pL, 4);

	if(5 <=n && 8>=n)
	{
		char	sColor[32];
		strcpy(sColor, lua_tostring(pL, 5));
		sscanf(sColor,"%x", &dColorStr);

		strcpy(sColor, lua_tostring(pL, 6));
		sscanf(sColor,"%x", &dColorBck);

		iThckX = (INT)lua_tonumber(pL, 7);
		iThckY = (INT)lua_tonumber(pL, 8);
	}


	if(0 == lWeight)
		lWeight = 100;
	else if(1 == lWeight)
		lWeight = 400;
	else
		lWeight = 700;



	nKey = LscFont_t::m_pCont->Alloc(NULL, sName, lH);

	lua_pushnumber(pL, nKey);
	return 1;
}




INT LscFont::Lsc_Release(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT iSize = 0;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	iSize = LscFont_t::m_pCont->Release(nKey);

	// 남아 있는 사이즈를 돌려준다.
	lua_pushnumber(pL, iSize);
	return 1;
}



INT LscFont::Lsc_FontDraw(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;

	INT		nKey=-1;
	INT		n	= lua_gettop(pL);
	INT		hr	= 0;

	INT		Bgn	= 0;
	INT		End	= -1;

	LCXVECTOR2	vPos(0,0);
	LCXCOLOR	vCol(1,1,1,1);
	char		Col[32]={0};
	UINT		Dif;

	LCXVECTOR2*	pPos = NULL;
	LCXCOLOR*	pCol = NULL;

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey =	(INT)lua_tonumber(pL, 1);

	pLsH = LscFont_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLC_FONT)pLsH->pIns;



	n--;

	if(0 == n)		// no argument list
	{
		hr = pObj->DrawTxt();
		lua_pushnumber(pL, hr);
		return 1;
	}


	// 1 <= n
	Bgn = (INT)lua_tonumber(pL, 1 +1)  -1;
	End = Bgn;

	if(2 == n)
	{
		if(lua_isnumber(pL, 2 +1))
		{
			End  = (INT)lua_tonumber(pL, 2 +1) -1;
		}
		else
		{
			strcpy(Col, lua_tostring(pL, 2 +1));
			sscanf(Col,"%x", &Dif);
			vCol = Dif;
			pCol = &vCol;
		}
	}
	else if(3 == n)
	{
		if(lua_isnumber(pL, 3 +1))
		{
			vPos.x = (FLOAT)lua_tonumber(pL, 2 +1);
			vPos.y = (FLOAT)lua_tonumber(pL, 3 +1);
			pPos = &vPos;
		}
		else
		{
			End  = (INT)lua_tonumber(pL, 2 +1)  -1;
			strcpy(Col, lua_tostring(pL, 3 +1));

			sscanf(Col,"%x", &Dif);
			vCol = Dif;
			pCol = &vCol;
		}
	}
	else if(4 == n)
	{
		if(lua_isnumber(pL, 4 +1))
		{
			End    = (INT  )lua_tonumber(pL, 2 +1)  -1;
			vPos.x = (FLOAT)lua_tonumber(pL, 3 +1);
			vPos.y = (FLOAT)lua_tonumber(pL, 4 +1);
			pPos = &vPos;
		}
		else
		{
			vPos.x = (FLOAT)lua_tonumber(pL, 2 +1);
			vPos.y = (FLOAT)lua_tonumber(pL, 3 +1);
			pPos = &vPos;
	
			strcpy(Col, lua_tostring(pL, 4 +1));
			sscanf(Col,"%x", &Dif);
			vCol = Dif;
			pCol = &vCol;
		}
	}
	else if(5 == n)
	{
		End    = (INT  )lua_tonumber(pL, 2 +1)  -1;
		vPos.x = (FLOAT)lua_tonumber(pL, 3 +1);
		vPos.y = (FLOAT)lua_tonumber(pL, 4 +1);
		pPos = &vPos;
	
		strcpy(Col, lua_tostring(pL, 5 +1));
		sscanf(Col,"%x", &Dif);
		vCol = Dif;
		pCol = &vCol;
	}
	else if(6 <= n || n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	hr = pObj->DrawTxt(NULL, Bgn, End, pPos, pCol);

	lua_pushnumber(pL, hr);
	return 1;
}



INT LscFont::Lsc_FontSetup(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;

	INT			nKey=-1;
	char*		sMsg= NULL;
	LCXVECTOR2	vcPos(0,0);
	LCXCOLOR	dColor(1,1,1,1);
	INT			Align = LC_FONT_ALIGN_LT;

	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT	 )lua_tonumber(pL, 1);

	if(2<=n)
		sMsg = (char*)lua_tostring(pL, 2);

	if(NULL== sMsg)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	if(4<=n)
	{
		vcPos.x	= (FLOAT)lua_tonumber(pL, 3);
		vcPos.y	= (FLOAT)lua_tonumber(pL, 4);
	}

	if(5<=n)
	{
		char	sColor[32]={0};
		UINT	fontColor;
		strcpy(sColor, lua_tostring(pL, 5));
		sscanf(sColor,"%x", &fontColor);
		dColor = fontColor;
	}

	if(6<=n)
	{
		Align = (INT)lua_tonumber(pL, 6);
	}


	pLsH = LscFont_t::m_pCont->Find(nKey);

	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pObj = (PLC_FONT)pLsH->pIns;
	pObj->SetPosition(&vcPos);
	pObj->SetColor(&dColor);
	pObj->SetString(sMsg);
	pObj->SetAttrib(LC_FONT_ALIGN, &Align);


	lua_pushnumber(pL, 0);
	return 1;
}



INT LscFont::Lsc_FontPos(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;

	LCXVECTOR2	vcPos;

	INT		nKey=-1;

	FLOAT	PosX;
	FLOAT	PosY;

	INT		n = lua_gettop(pL);

	if(n<3)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT	 )lua_tonumber(pL, 1);
	PosX = (FLOAT)lua_tonumber(pL, 2);
	PosY = (FLOAT)lua_tonumber(pL, 3);

	pLsH = LscFont_t::m_pCont->Find(nKey);


	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pObj = (PLC_FONT)pLsH->pIns;
	vcPos = LCXVECTOR2(PosX, PosY);

	pObj->SetPosition(&vcPos);

	lua_pushnumber(pL, 0);
	return 1;
}

INT	LscFont::Lsc_FontString(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;
	INT			nKey=-1;
	char*		sMsg;

	INT		n = lua_gettop(pL);

	if(n<2)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT	 )lua_tonumber(pL, 1);
	sMsg = (char*)lua_tostring(pL, 2);

	if(NULL== sMsg || strlen(sMsg)<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pLsH = LscFont_t::m_pCont->Find(nKey);


	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLC_FONT)pLsH->pIns;
	pObj->SetString(sMsg);

	lua_pushnumber(pL, 0);
	return 1;
}


INT LscFont::Lsc_FontColor(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;
	LCXCOLOR	dColor;

	INT		nKey=-1;

	UINT	fontColor= 0xFFFFFFFF;
	char	sColor[32]={0};

	INT		n = lua_gettop(pL);

	if(n<2)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT	 )lua_tonumber(pL, 1);

	strcpy(sColor, lua_tostring(pL, 2));
	sscanf(sColor,"%x", &fontColor);


	pLsH = LscFont_t::m_pCont->Find(nKey);

	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pObj = (PLC_FONT)pLsH->pIns;
	dColor = fontColor;

	pObj->SetColor(&dColor);


	lua_pushnumber(pL, 0);
	return 1;
}



INT LscFont::Lsc_FontScale(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;

	LCXVECTOR2	vcScl;

	INT		nKey=-1;

	FLOAT	SclX;
	FLOAT	SclY;

	INT		n = lua_gettop(pL);

	if(n<3)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT	 )lua_tonumber(pL, 1);
	SclX = (FLOAT)lua_tonumber(pL, 2);
	SclY = (FLOAT)lua_tonumber(pL, 3);

	pLsH = LscFont_t::m_pCont->Find(nKey);


	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pObj = (PLC_FONT)pLsH->pIns;
	vcScl= LCXVECTOR2(SclX, SclY);

	pObj->SetScaling(&vcScl);

	lua_pushnumber(pL, 0);
	return 1;
}


INT LscFont::Lsc_FontAlign(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;
	INT			Align = LC_FONT_ALIGN_LT;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(n<2)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT	 )lua_tonumber(pL, 1);
	Align= (INT	 )lua_tonumber(pL, 2);


	pLsH = LscFont_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLC_FONT)pLsH->pIns;


	pObj->SetAttrib(LC_FONT_ALIGN, &Align);

	lua_pushnumber(pL, 0);
	return 1;
}



INT LscFont::Lsc_FontCount(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);
	INT		count = 0;

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT	 )lua_tonumber(pL, 1);

	pLsH = LscFont_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLC_FONT)pLsH->pIns;


	pObj->GetAttrib(LC_FONT_CHAR_COUNT, &count);

	lua_pushnumber(pL, count);
	return 1;
}



INT LscFont::Lsc_FontRect(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_FONT	pObj = NULL;

	INT		nKey=-1;
	INT		n   = lua_gettop(pL);
	INT		Idx = 0;
	LCXRECT	rc(0,0,0,0);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	if(2 <= n)
		Idx  = (INT)lua_tonumber(pL, 2);


	pLsH = LscFont_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLC_FONT)pLsH->pIns;


	// decrease index: Lua start 1, but c/c++ start 0
	--Idx;

	pObj->GetRect(&rc, Idx);

	lua_pushnumber(pL, rc.x);
	lua_pushnumber(pL, rc.y);
	lua_pushnumber(pL, rc.w);
	lua_pushnumber(pL, rc.h);

	return 4;
}

