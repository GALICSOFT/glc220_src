// Implementation of the LstDB class.
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
#include "../LctEntity.h"
#include "../LctLua.h"
#else
#include <LctEntity.h>
#include <LctLua.h>
#endif




////////////////////////////////////////////////////////////////////////////////
// Script

namespace LstDB_t
{
	static LstDB::ILhLst*	m_pCont = NULL;		// global
}


INT	LstDB::Create(LC_HANDLE v)
{
	if(NULL == LstDB_t::m_pCont)
		LstDB_t::m_pCont = new LstDB::ILhLst;

	return 0;
}


INT	LstDB::Destroy()
{
	if(LstDB_t::m_pCont)
	{
		delete LstDB_t::m_pCont;
		LstDB_t::m_pCont = NULL;
	}

	return 0;
}


static const luaL_reg regLstDB[] =
{
	{"New"			,	(lua_CFunction)LstDB::Lsc_Alloc			},
	{"Release"		,	(lua_CFunction)LstDB::Lsc_Release		},

	{"Select"		,	(lua_CFunction)LstDB::Lsc_Select		},
	{"FieldCount"	,	(lua_CFunction)LstDB::Lsc_FieldCount	},
	{"RecordCount"	,	(lua_CFunction)LstDB::Lsc_RecordCount	},

	{NULL, NULL},
};


INT LstDB::OpenLib(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	luaL_openlib(pL, "Lstdb", regLstDB, 0);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////
// For list

LstDB::ILhLst::ILhLst()
{
	memset(vLst, 0, sizeof(LscH*) * MAX_DB);
}

LstDB::ILhLst::~ILhLst()
{
	PLCT_DBCSV pObj;

	for(INT i=0; i<MAX_DB; ++i)
	{
		if(vLst[i])
		{
			pObj = (PLCT_DBCSV)(vLst[i]->pIns);
			SAFE_RELEASE(	pObj	);

			delete vLst[i];
			vLst[i] = NULL;
		}
	}
}


INT LstDB::ILhLst::Alloc(char* sFile, INT dlm)
{
	LscH*		pLsH = NULL;
	PLCT_DBCSV	pObj = NULL;

	INT			nKey	=-1;
	INT			hr		=-1;

	pLsH = this->Find(&nKey, sFile);
	if(pLsH && 0 <= nKey)											// if same file exists.
		return nKey+1;												// lua index starts from 1


	nKey	= FindEmpty();
	if(0 > nKey)													// all key is used. allowble key does not exist.
		return -1;


	char  sMedia[LC_MAX_PATH2]={0};


	hr = LcSys_ResourcePath(sMedia, sFile);


	BYTE*		buf = NULL;
	ULONG		len = 0;

#if defined(__ANDROID__)
	const char*	sApkFile = LcSys_AndroidApkFile(NULL);

	hr = Lzip_UnzipbyName(&buf, &len, sApkFile, sMedia, LC_TRUE);
	if(LC_FAILED(hr))
		return -1;

#else
	buf = (BYTE*)sMedia;
#endif


	hr = LctDB_CreateCsv(NULL, &pObj, buf, &len, &dlm);
	if(LC_FAILED(hr))
	{
		LOGE("LstDB::Err::Alloc: %s\n", sMedia);
		return -1;
	}

	LOGI("LstDB::Alloc:: %s %ld\n", sMedia, len);

	pLsH	= new LscH;
	pLsH->pIns	= pObj;
	strcpy(pLsH->sName, sFile);

	vLst[nKey] = pLsH;

	return (nKey+1);
}


INT LstDB::ILhLst::Release(INT nKey)
{
	LscH*		pLsH = NULL;
	PLCT_DBCSV	pObj = NULL;

	if(MAX_DB <=nKey || nKey<1)
		return -1;

	pLsH = vLst[nKey-1];

	if(NULL == pLsH)
		return -1;

	pObj = (PLCT_DBCSV)(pLsH->pIns);

	SAFE_RELEASE(	pObj	);

	delete pLsH;

	vLst[nKey-1] = NULL;

	return 0;
}


LscH* LstDB::ILhLst::Find(INT nKey)
{
	if(MAX_DB <=nKey || nKey<1)
		return NULL;

	return vLst[nKey-1];
}


LscH* LstDB::ILhLst::Find(INT* nIdx, char* sFile)
{
	if(nIdx)
		*nIdx = -1;

	if(NULL == sFile || strlen(sFile)<1)
		return NULL;


	for(INT i=0; i<MAX_DB; ++i)
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


INT LstDB::ILhLst::FindEmpty()
{
	for(INT i=0; i<MAX_DB; ++i)
	{
		if(NULL == vLst[i])
			return i;
	}

	return -1;
}




////////////////////////////////////////////////////////////////////////////////
// for glue

INT	LstDB::Lsc_Alloc(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT		nKey=-1;
	char*	sFile = NULL;
	INT		n   = lua_gettop(pL);
	INT		dlm = ',';

	if(1>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	if(2==n)
	{
		char s[4]={0};
		strncpy(s, (char*)lua_tostring(pL, 2), 1);
		dlm = s[0];
	}

	if(3<=n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	sFile= (char*)lua_tostring(pL, 1);
	nKey = LstDB_t::m_pCont->Alloc(sFile, dlm);


	if(0>nKey)
		printf("DB File Alloc Failed: %s\n", sFile);

	// Key를 돌려 준다.
	lua_pushnumber(pL, nKey);
	return 1;
}



INT LstDB::Lsc_Release(LC_HANDLE _L)
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

	iSize = LstDB_t::m_pCont->Release(nKey);

	// 남아 있는 사이즈를 돌려준다.
	lua_pushnumber(pL, iSize);
	return 1;
}


INT LstDB::Lsc_Select(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT			hr = 0;

	LscH*		pLsH = NULL;
	PLCT_DBCSV	pObj = NULL;
	INT			row =-1, col = -1;
	char*		Field= NULL;


	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(2>n)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	// read row
	row  = (INT  )lua_tonumber(pL, 2);


	// read col
	if(2<n)
	{
		 if(!lua_isnumber(pL, 3) )
			Field= (char*)lua_tostring(pL, 3);
		else
			col  = (INT  )lua_tonumber(pL, 3);
	}


	nKey = (INT)lua_tonumber(pL, 1);
	pLsH	= LstDB_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLCT_DBCSV)pLsH->pIns;


	--row;
	--col;

	ILCT_DBcsv::TCSV_CELL	cell;
	ILCT_DBcsv::TCSV_REC	rcd;


	int	bCell = 0;
	if(Field)
	{
		bCell = 1;
		hr = pObj->Select(&cell, row, Field);
	}
	else if( 0<=col && 0<=row)
	{
		bCell = 1;
		hr = pObj->Select(&cell, row, col);
	}
	else
		hr = pObj->Select(&rcd, row);


	// error
	if(LC_FAILED(hr))
	{
		lua_pushnil(pL);
		return 1;
	}

	if(bCell)
	{
		if(     LCTDB_BOOL == cell.t)	lua_pushboolean(pL, cell.b);
		else if(LCTDB_SMALL== cell.t)	lua_pushnumber (pL, cell.n);
		else if(LCTDB_FLOAT== cell.t)	lua_pushnumber (pL, cell.f);
		else if(LCTDB_BIG  == cell.t)	lua_pushnumber (pL, cell.d);
		else							lua_pushstring (pL, cell.s);

		//LOGI("DB[%d,%d]:", (row+1), (col+1));
		//if     (LCTDB_BOOL  == cell.t)	LOGI("%d, ", (int)cell.b);
		//else if(LCTDB_SMALL == cell.t)	LOGI("%d, ", cell.n);
		//else if(LCTDB_FLOAT == cell.t)	LOGI("%f, ", cell.f);
		//else if(LCTDB_BIG   == cell.t)	LOGI("lf, ", cell.d);
		//else if(LCTDB_TEXT  == cell.t)	LOGI("%s, ", cell.s);
		//else							LOGI("not defined:: ");
		//LOGI("\n");

		return 1;
	}


	for(int i=0; i<rcd.cnt; ++i)
	{
		if(     LCTDB_BOOL == rcd.rec[i].t)	lua_pushboolean(pL, rcd.rec[i].b);
		else if(LCTDB_SMALL== rcd.rec[i].t)	lua_pushnumber (pL, rcd.rec[i].n);
		else if(LCTDB_FLOAT== rcd.rec[i].t)	lua_pushnumber (pL, rcd.rec[i].f);
		else if(LCTDB_BIG  == rcd.rec[i].t)	lua_pushnumber (pL, rcd.rec[i].d);
		else								lua_pushstring (pL, rcd.rec[i].s);

		//LOGI("DB[%d]:", (row+1));
		//if     (LCTDB_BOOL  == cell.t)	LOGI("%d, ", (int)cell.b);
		//else if(LCTDB_SMALL == cell.t)	LOGI("%d, ", cell.n);
		//else if(LCTDB_FLOAT == cell.t)	LOGI("%f, ", cell.f);
		//else if(LCTDB_BIG   == cell.t)	LOGI("%lf, ", cell.d);
		//else if(LCTDB_TEXT  == cell.t)	LOGI("%s, ", cell.s);
		//else							LOGI("not defined");
		//LOGI("\n");
	}

	return rcd.cnt;
}


INT LstDB::Lsc_Insert(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT			hr = 0;

	LscH*		pLsH = NULL;
	PLCT_DBCSV	pObj = NULL;

	ILCT_DBcsv::TCSV_REC rcd;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LstDB_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	pObj = (PLCT_DBCSV)pLsH->pIns;


	rcd.cnt = n-1;

	for(int i=0; i<rcd.cnt; ++i)
	{
		ILCT_DBcsv::TCSV_CELL* cell = &rcd.rec[i];

		cell->t = i;
		hr = pObj->GetAttrib(LCTDB_FIELD_TYPE, &cell->t);

		if(0>hr)
			continue;

		if(     LCTDB_BOOL == cell->t)
			cell->b = (INT   )lua_toboolean(pL,i+2);

		else if(LCTDB_SMALL== cell->t)
			cell->n = (INT   )lua_tonumber (pL,i+2);

		else if(LCTDB_FLOAT== cell->t)
			cell->f = (FLOAT )lua_tonumber (pL,i+2);

		else if(LCTDB_BIG  == cell->t)
			cell->d = (DOUBLE)lua_tonumber (pL,i+2);

		else
			cell->s = (char* )lua_tostring (pL,i+2);
	}


	hr = pObj->Insert(&rcd);

	lua_pushnumber(pL, hr);
	return 1;
}


INT LstDB::Lsc_Update(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT			hr = 0;

	LscH*		pLsH = NULL;
	PLCT_DBCSV	pObj = NULL;
	INT			row=-1, col=-1;
	char*		Field= NULL;

	ILCT_DBcsv::TCSV_CELL	cell;


	INT		nKey=-1;
	INT		n = lua_gettop(pL);
	if(n<3)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	row = (INT)lua_tonumber(pL, 2);

	if(!lua_isnumber(pL, 3) )
		Field= (char*)lua_tostring(pL, 3);
	else
		col  = (INT  )lua_tonumber(pL, 3);


	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LstDB_t::m_pCont->Find(nKey);
	if(NULL == pLsH || NULL == pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLCT_DBCSV)pLsH->pIns;


	--row;


	if(Field)
	{
		char sFiled[LCTDB_MAX_NAME]={0};
		strcpy(sFiled, Field);

		hr = pObj->GetAttrib(LCTDB_FIELD_INDEX, sFiled);
		memcpy(&col, sFiled, sizeof(col));
	}

	cell.t = col;

	hr = pObj->GetAttrib(LCTDB_FIELD_TYPE, &cell.t);

	if(0>hr)
	{
		lua_pushnumber(pL, hr);
		return 1;
	}


	if(     LCTDB_BOOL == cell.t)	cell.b = (INT   )lua_toboolean(pL,4);
	else if(LCTDB_SMALL== cell.t)	cell.n = (INT   )lua_tonumber (pL,4);
	else if(LCTDB_FLOAT== cell.t)	cell.f = (FLOAT )lua_tonumber (pL,4);
	else if(LCTDB_BIG  == cell.t)	cell.d = (DOUBLE)lua_tonumber (pL,4);
	else							cell.s = (char* )lua_tostring (pL,4);


	hr = pObj->Update(&cell, row, col);

	lua_pushnumber(pL, hr);
	return 1;

}


INT LstDB::Lsc_Delete(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT			hr = 0;

	LscH*		pLsH = NULL;
	PLCT_DBCSV	pObj = NULL;
	INT			row  = -1;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	if(n<2)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	row  = (INT)lua_tonumber(pL, 2);

	nKey = (INT)lua_tonumber(pL, 1);

	pLsH = LstDB_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}
	pObj = (PLCT_DBCSV)pLsH->pIns;


	--row;

	hr = pObj->Delete(row);

	lua_pushnumber(pL, hr);
	return 1;
}


INT LstDB::Lsc_FieldCount(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT			hr = 0;

	LscH*		pLsH = NULL;
	PLCT_DBCSV	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	INT		nField = -1;

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LstDB_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pObj = (PLCT_DBCSV)pLsH->pIns;

	hr = pObj->GetAttrib(LCTDB_FIELD_NUM, &nField);
	if(0>hr)
	{
		lua_pushnumber(pL, hr);
		return 1;
	}


	lua_pushnumber(pL, nField);
	return 1;
}


INT LstDB::Lsc_RecordCount(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT			hr = 0;

	LscH*		pLsH = NULL;
	PLCT_DBCSV	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	INT		nRecord = -1;

	if(n<1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LstDB_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pObj = (PLCT_DBCSV)pLsH->pIns;

	hr = pObj->GetAttrib(LCTDB_RECORD_NUM, &nRecord);
	if(0>hr)
	{
		lua_pushnumber(pL, hr);
		return 1;
	}


	lua_pushnumber(pL, nRecord);
	return 1;
}


INT LstDB::Lsc_Save(LC_HANDLE _L)
{
	lua_State*	pL = (lua_State*)_L;

	INT			hr = 0;

	LscH*		pLsH = NULL;
	PLCT_DBCSV	pObj = NULL;

	INT		nKey=-1;
	INT		n = lua_gettop(pL);

	char*	sFile = NULL;

	if(n<=1)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}

	if(1<n)
		sFile = (char*)lua_tostring(pL, 2);


	nKey = (INT)lua_tonumber(pL, 1);

	pLsH	= LstDB_t::m_pCont->Find(nKey);
	if(NULL ==pLsH || NULL ==pLsH->pIns)
	{
		lua_pushnumber(pL, -1);
		return 1;
	}


	pObj = (PLCT_DBCSV)pLsH->pIns;

	hr = pObj->Query(LCTDB_QUERY_SAVE_AS, sFile);

	lua_pushnumber(pL, hr);
	return 1;
}

