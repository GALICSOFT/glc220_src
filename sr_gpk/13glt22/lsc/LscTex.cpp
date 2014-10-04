// Implementation of the LscTex class.
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

namespace LscTex_t
{
	static LC_HANDLE		m_pSpt	= NULL;		// sprite
	static LscTex::ILhLst*	m_pCont	= NULL;		// local
}


LscTex::ILhLst*	L_GetTextureList()
{
	return LscTex_t::m_pCont;
}


INT LscTex::Create(LC_HANDLE v)
{
	LscTex_t::m_pSpt = v;
	if(NULL == LscTex_t::m_pCont)
		LscTex_t::m_pCont = new LscTex::ILhLst;

	return 0;
}


INT LscTex::Destroy()
{
	if(LscTex_t::m_pCont)
	{
		delete LscTex_t::m_pCont;
		LscTex_t::m_pCont = NULL;
	}

	return 0;
}


INT	LscTex::Invalid()
{
	if(LscTex_t::m_pCont)
		LscTex_t::m_pCont->Invalid();

	return 0;
}


INT	LscTex::Restore()
{
	if(LscTex_t::m_pCont)
		LscTex_t::m_pCont->Restore();

	return 0;
}


static const luaL_reg regLscTex[] =
{
	{"New"		,	(lua_CFunction)LscTex::Lsc_Alloc			},
	{"ReAlloc"	,	(lua_CFunction)LscTex::Lsc_ReAlloc			},
	{"Release"	,	(lua_CFunction)LscTex::Lsc_Release			},

	{"Width"	,	(lua_CFunction)LscTex::Lsc_TextureWidth		},
	{"Height"	,	(lua_CFunction)LscTex::Lsc_TextureHeight	},
	{"Pointer"	,	(lua_CFunction)LscTex::Lsc_TexturePointer	},
	{"Draw"		,	(lua_CFunction)LscTex::Lsc_TextureDraw		},
	{"DrawOne"	,	(lua_CFunction)LscTex::Lsc_TextureDrawOne	},
	{"DrawEx"	,	(lua_CFunction)LscTex::Lsc_TextureDrawEx	},
	{"DrawMask"	,	(lua_CFunction)LscTex::Lsc_TextureDrawMask	},

	{"SetTexture",	(lua_CFunction)LscTex::Lsc_SetTexture		},
	{"Blend"	,	(lua_CFunction)LscTex::Lsc_SetBlending		},
	{"ZBias"	,	(lua_CFunction)LscTex::Lsc_ZBias			},

	{NULL, NULL},
};


INT LscTex::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "Ltex", regLscTex, 0);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////
// For list

LscTex::ILhLst::ILhLst()
{
	memset(vLst, 0, sizeof(LscH*) * MAX_TEX);
}

LscTex::ILhLst::~ILhLst()
{
	PLC_TEXTURE pObj;

	for(INT i=0; i<MAX_TEX; ++i)
	{
		if(vLst[i])
		{
			pObj = (PLC_TEXTURE)(vLst[i]->pIns);
			SAFE_RELEASE(	pObj	);

			delete vLst[i];
			vLst[i] = NULL;
		}
	}
}


INT LscTex::ILhLst::Alloc(char* sFile, INT b16, UINT dColorKey, UINT dFilter)
{
	LscH*		pLsH = NULL;
	PLC_TEXTURE	pObj = NULL;

	INT		nKey	=-1;
	INT		hr		=-1;

	pLsH	= this->Find(&nKey, sFile);
	if(pLsH && 0 <= nKey)											// if same file exists.
		return nKey+1;												// lua index starts from 1


	nKey	= FindEmpty();

	// 꽉차있음
	if(0 > nKey)
		return -1;


	if(b16)
		b16= LC_FMT_INDEX16;
	else
		b16 = 0;


	hr = LcDev_CreateTextureFromFile(NULL, &pObj, (LC_HANDLE)sFile, &dColorKey, NULL, &b16);
	if(LC_FAILED(hr))
		return -1;

	pLsH	= new LscH;
	pLsH->pIns	= pObj;
	strcpy(pLsH->sName, sFile);

	vLst[nKey] = pLsH;
	return (nKey+1);
}


INT LscTex::ILhLst::ReAlloc(INT nKey, char* sFile, INT b16, UINT dColorKey, UINT dFilter)
{
	LscH*		pLsH = NULL;
	PLC_TEXTURE	pObj = NULL;

	INT				hr   =-1;


	pLsH = this->Find(nKey);


	// 텍스처가 있다면 이전 Object를 해제하고, 새로운 Object를 할당한다.
	if(pLsH && pLsH->pIns)
	{
		hr = LcDev_CreateTextureFromFile(NULL, &pObj, (LC_HANDLE)sFile, &dColorKey);
		if(LC_FAILED(hr))
			return -1;

		PLC_TEXTURE	pTmp = (PLC_TEXTURE)pLsH->pIns;
		SAFE_RELEASE(	pTmp	);

		pLsH->pIns = pObj;
		strcpy(pLsH->sName, sFile);
	}
	else
	{
		// 없으면 메모리를 할당한다.
		nKey = this->Alloc(sFile);

		if(0>nKey)
			printf("File Alloc Failed: %s\n", sFile);
	}

	return nKey;
}


INT LscTex::ILhLst::Release(INT nKey)
{
	LscH*		pLsH = NULL;
	PLC_TEXTURE	pObj = NULL;

	if(MAX_TEX <=nKey || nKey<1)
		return -1;

	pLsH = vLst[nKey-1];

	if(NULL == pLsH)
		return -1;

	pObj = (PLC_TEXTURE)(pLsH->pIns);

	SAFE_RELEASE(	pObj	);

	delete pLsH;

	vLst[nKey-1] = NULL;

	return 0;
}


LscH* LscTex::ILhLst::Find(INT nKey)
{
	if(MAX_TEX <=nKey || nKey<1)
		return NULL;

	return vLst[nKey-1];
}


LscH* LscTex::ILhLst::Find(INT* nIdx, char* sFile)
{
	if(nIdx)
		*nIdx = -1;

	if(NULL == sFile || strlen(sFile)<1)
		return NULL;


	for(INT i=0; i<MAX_TEX; ++i)
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


INT LscTex::ILhLst::FindEmpty()
{
	for(INT i=0; i<MAX_TEX; ++i)
	{
		if(NULL == vLst[i])
			return i;
	}

	return -1;
}


INT LscTex::ILhLst::Invalid()
{
	LscH*		pLsH = NULL;
	PLC_TEXTURE	pObj = NULL;

	for(INT i=0; i<MAX_TEX; ++i)
	{
		pLsH = vLst[i];

		if(NULL == pLsH)
			continue;

		pObj = (PLC_TEXTURE)(pLsH->pIns);

		if(NULL == pObj)
			continue;

		pObj->Query(LCQUERY_INVALID, NULL);
	}

	return LC_OK;
}


INT LscTex::ILhLst::Restore()
{
	LscH*		pLsH = NULL;
	PLC_TEXTURE	pObj = NULL;

	for(INT i=0; i<MAX_TEX; ++i)
	{
		pLsH = vLst[i];

		if(NULL == pLsH)
			continue;

		pObj = (PLC_TEXTURE)(pLsH->pIns);

		if(NULL == pObj)
			continue;

		pObj->Query(LCQUERY_RESTORE, NULL);
	}

	return LC_OK;
}



////////////////////////////////////////////////////////////////////////////////
// for glue

#define GL_NEAREST                   0x2600
#define GL_LINEAR                    0x2601
#define D3DX_FILTER_NONE             (1 << 0)
#define D3DX_FILTER_LINEAR           (3 << 0)

// Texture
INT	LscTex::Lsc_Alloc(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		nKey=-1;
	char	sFile[240]={0};
	char	sColor[32]={0};
	char	sFilter[32]={0};
	INT		b16 = 0;

	UINT	dc=0x00;
	UINT	dF=GL_NEAREST;

	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	// file name
	strcpy(sFile, lua_tostring(pL, 1));

	// to 16bit
	if(2 <= n)
	{
		b16 = (INT)lua_tonumber(pL, 2);
	}

	// color key
	if(3 <= n)
	{
		strcpy(sColor, lua_tostring(pL, 3));
		sscanf(sColor, "%x", &dc);
	}

	// filter
	if(4 <= n)
	{
		strcpy(sFilter, lua_tostring(pL, 4));

		if(0== LcStr_Strcmp(sFilter, "Linear"))
			dF = GL_LINEAR;
	}


	nKey = LscTex_t::m_pCont->Alloc(sFile, b16, dc, dF);

	// Key를 돌려 준다.
	lua_pushnumber(pL, nKey);
	return 1;
}




// Texture
INT	LscTex::Lsc_ReAlloc(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		nKey=-1;
	char	sFile[LC_MAX_PATH]={0};

	INT		n = lua_gettop(pL);

	if(n<3)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	strcpy(sFile, lua_tostring(pL, 1));

	nKey = LscTex_t::m_pCont->ReAlloc(nKey, sFile);

	// Key를 돌려 준다.
	lua_pushnumber(pL, nKey);
	return 1;
}




INT LscTex::Lsc_Release(LC_HANDLE _L)
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

	iSize = LscTex_t::m_pCont->Release(nKey);

	// 남아 있는 사이즈를 돌려준다.
	lua_pushnumber(pL, iSize);
	return 1;
}


INT LscTex::Lsc_TextureWidth(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_TEXTURE	pObj = NULL;

	INT		nKey=-1;
	INT		iWidth=-1;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscTex_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pObj = (PLC_TEXTURE)pLsH->pIns;
	iWidth = pObj->GetImgW();

	lua_pushnumber(pL, iWidth);
	return 1;
}


INT LscTex::Lsc_TextureHeight(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_TEXTURE	pObj = NULL;

	INT		nKey=-1;
	INT		iHeight=-1;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscTex_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLC_TEXTURE)pLsH->pIns;
	iHeight = pObj->GetImgH();

	lua_pushnumber(pL, iHeight);
	return 1;
}


INT LscTex::Lsc_TexturePointer(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_TEXTURE	pObj = NULL;

	INT		nKey=-1;
	UINT	nPointer=0;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LscTex_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLC_TEXTURE)pLsH->pIns;
	nPointer	= (UINT)(pObj);

	lua_pushnumber(pL, nPointer);
	return 1;
}



INT LscTex::Lsc_TextureDraw(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_TEXTURE	pObj = NULL;

	PLC_SPRITE  pSpt = (PLC_SPRITE)LscTex_t::m_pSpt;

	INT			nKey=-1;

	LCXRECT		rct(0,0,0,0);
	LCXRECT*	pRc= NULL;

	LCXVECTOR2	vcPos(0,0);
	LCXVECTOR2	vcScl(1,1);
	char		sC[16] = {0};
	LCXCOLOR	dC=LCXCOLOR(1,1,1,1);

	INT		n = lua_gettop(pL);

	if(n<3)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}



	nKey = (INT)lua_tonumber(pL, 1);


	if(3 == n)
	{
		vcPos.x	= (FLOAT)lua_tonumber(pL, 2);
		vcPos.y	= (FLOAT)lua_tonumber(pL, 3);
	}
	else
	{
		rct.l	= (FLOAT)lua_tonumber(pL, 2);
		rct.t	= (FLOAT)lua_tonumber(pL, 3);
		rct.r	= (FLOAT)lua_tonumber(pL, 4);
		rct.b	= (FLOAT)lua_tonumber(pL, 5);
		rct.ToRegion();

		pRc = &rct;
	}

	// 위치
	if(7 <= n)
	{
		vcPos.x	= (FLOAT)lua_tonumber(pL, 6);
		vcPos.y	= (FLOAT)lua_tonumber(pL, 7);
	}

	// 스케일
	if(9 <= n)
	{
		vcScl.x	= (FLOAT)lua_tonumber(pL, 8);
		vcScl.y	= (FLOAT)lua_tonumber(pL, 9);
	}

	// 색상
	if(10 <= n)
	{
		UINT t;
		strcpy(sC, lua_tostring(pL, 10));
		sscanf(sC, "%x", &t);

		dC = t;
	}


	pLsH	= LscTex_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL== pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLC_TEXTURE)pLsH->pIns;


	if(1.0F == vcScl.x && 1.0F == vcScl.y)
		pSpt->Draw(pObj, pRc, &vcPos, &dC);
	else
		pSpt->DrawEx(pObj, &rct, &vcPos, &vcScl, NULL, 0, &dC);


	lua_pushnumber(pL, 0);
	return 1;
}




INT LscTex::Lsc_TextureDrawOne(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_TEXTURE	pObj = NULL;

	PLC_SPRITE  pSpt = (PLC_SPRITE)LscTex_t::m_pSpt;

	INT			nKey=-1;

	LCXVECTOR2	vcPos(0,0);
	char		sC[16] = {0};
	LCXCOLOR	dC=LCXCOLOR(1,1,1,1);

	INT		n = lua_gettop(pL);

	if(n<3)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	nKey = (INT)lua_tonumber(pL, 1);

	// position
	if(3 <= n)
	{
		vcPos.x	= (FLOAT)lua_tonumber(pL, 2);
		vcPos.y	= (FLOAT)lua_tonumber(pL, 3);
	}


	// 색상
	if(4 <= n)
	{
		UINT t;
		strcpy(sC, lua_tostring(pL, 4));
		sscanf(sC, "%x", &t);

		dC = t;
	}


	pLsH	= LscTex_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL== pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLC_TEXTURE)pLsH->pIns;

	pSpt->Draw(pObj, NULL, &vcPos, &dC);

	lua_pushnumber(pL, 0);
	return 1;
}




INT LscTex::Lsc_TextureDrawEx(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;
	PLC_TEXTURE	pObj = NULL;

	PLC_SPRITE  pSpt = (PLC_SPRITE)LscTex_t::m_pSpt;

	INT			nKey=-1;

	LCXRECT		rct(0,0,0,0);
	LCXVECTOR2	vcPos(0,0);
	LCXVECTOR2	vcScl(1,1);
	LCXVECTOR2	vcRot(0,0);
	FLOAT	fAngle = 0;
	char	sC[16] = {0};
	LCXCOLOR	dC=LCXCOLOR(1,1,1,1);
	INT		nMono = 0;

	INT		n = lua_gettop(pL);

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


	pLsH	= LscTex_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL== pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLC_TEXTURE)pLsH->pIns;

	pSpt->DrawEx(pObj, &rct, &vcPos, &vcScl, &vcRot, fAngle, &dC);

	lua_pushnumber(pL, 0);
	return 1;
}


INT LscTex::Lsc_TextureDrawMask(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH0 = NULL;
	LscH*		pLsH1 = NULL;
	PLC_TEXTURE	pObj0 = NULL;
	PLC_TEXTURE	pObj1 = NULL;

	PLC_SPRITE  pSpt = (PLC_SPRITE)LscTex_t::m_pSpt;

	INT			nKey0 = -1;
	INT			nKey1 = -1;

	LCXRECT		pRc0;
	LCXRECT		pRc1;
	LCXVECTOR2	vcPos;
	LCXVECTOR2	vcScl(1,1);
	LCXVECTOR2	vcRot(0,0);
	LCXCOLOR	dC=LCXCOLOR(1,1,1,1);
	char	sC[16];

	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey0 = (INT)lua_tonumber(pL, 1);
	nKey1 = (INT)lua_tonumber(pL, 2);

	pRc0.l	= (FLOAT)lua_tonumber(pL,  3);
	pRc0.t	= (FLOAT)lua_tonumber(pL,  4);
	pRc0.r	= (FLOAT)lua_tonumber(pL,  5);
	pRc0.b	= (FLOAT)lua_tonumber(pL,  6);
	pRc0.ToRegion();

	pRc1.l	= (FLOAT)lua_tonumber(pL,  7);
	pRc1.t	= (FLOAT)lua_tonumber(pL,  8);
	pRc1.r	= (FLOAT)lua_tonumber(pL,  9);
	pRc1.b	= (FLOAT)lua_tonumber(pL, 10);
	pRc1.ToRegion();

	// 위치
	vcPos.x	= (FLOAT)lua_tonumber(pL, 11);
	vcPos.y	= (FLOAT)lua_tonumber(pL, 12);

	// 스케일
	if(n>12)
	{
		vcScl.x	= (FLOAT)lua_tonumber(pL, 13);
		vcScl.y	= (FLOAT)lua_tonumber(pL, 14);
	}

	// 색상
	if(n>14)
	{
		UINT t;
		strcpy(sC, lua_tostring(pL, 15));
		sscanf(sC, "%x", &t);
		dC = t;
	}


	pLsH0 = LscTex_t::m_pCont->Find(nKey0);
	pLsH1 = LscTex_t::m_pCont->Find(nKey1);


	if(NULL == pLsH0 || NULL== pLsH0->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj0	= (PLC_TEXTURE)pLsH0->pIns;

	if(pObj1)
		pObj1	= (PLC_TEXTURE)pLsH1->pIns;



	if(1.0F == vcScl.x && 1.0F == vcScl.y)
		pSpt->Draw(pObj0, &pRc0, &vcPos, &dC);
	else
		pSpt->DrawEx(pObj0, &pRc0, &vcPos, &vcScl, NULL, 0, &dC);


	lua_pushnumber(pL, 0);
	return 1;
}



INT LscTex::Lsc_SetTexture(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	LscH*		pLsH = NULL;

	INT		hr=-1;
	INT		nKey=-1;
	INT		nStage=-1;
	INT		n = lua_gettop(pL);

	if(n<2)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey	= (INT)lua_tonumber(pL, 1);
	nStage	= (INT)lua_tonumber(pL, 2);

	pLsH	= LscTex_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL== pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	//PDEV	pDev	= (PDEV)(LscTex_t::);
	//PLC_TEXTURE	pObj	= (PLC_TEXTURE)(p->pIns);
	//PDTX	pTxD3d	= (PDTX)pObj->GetPixel();

	//hr = pDev->SetTexture( nStage, pTxD3d);

	//if(LC_FAILED(hr))
	//	hr =-1;
	//else
	//	hr =0;

	lua_pushnumber(pL, hr);
	return 1;
}


INT LscTex::Lsc_SetBlending(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_SPRITE  pSpt = (PLC_SPRITE)LscTex_t::m_pSpt;

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

INT LscTex::Lsc_ZBias(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	PLC_SPRITE  pSpt = (PLC_SPRITE)LscTex_t::m_pSpt;

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



