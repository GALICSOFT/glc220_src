// Implementation of the CGpPhase class.
//
//  Created by Heesung Oh on 2011. 01. 19.
//  Copyright GALIC SOFT Inc. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1200
  #pragma warning(disable: 4996)
#endif


#include "_StdAfx.h"


CGpPhase::CGpPhase()
{
	m_nPhase	= 0		;
	m_pL		= NULL	;
	m_pAppMain	= NULL	;
	memset(m_sF, 0, sizeof m_sF);
}


CGpPhase::~CGpPhase()
{
	CGpPhase::Destroy();
}


INT CGpPhase::Destroy()
{
	if(!m_pL)
		return 0;

	LscLua_Destroy(m_pL);			// Lua_Destroy()함수 호출

	LscInput::Destroy();
	LscTex	::Destroy();
	LscFont	::Destroy();
	LscSmd	::Destroy();
	LscFile	::Destroy();
	LscSpc	::Destroy();
	LsxTsa	::Destroy();
	LstDB	::Destroy();
	LscHw	::Destroy();
	LscNet	::Destroy();

	LscUtil	::Destroy();
	LscSys	::Destroy();

	Lua_Close(&m_pL);				// Lua 해제

	return 0;
}


INT CGpPhase::Create(PLC_ARG pArg)
{
	INT			hr	= -1;
	LC_HANDLE	pL	= NULL;

	m_nPhase	= *((INT*)(pArg->p[0]));
	m_pAppMain	= pArg->p[1];

	PLC_APPFRAME	pApp= (PLC_APPFRAME)m_pAppMain;

	memcpy(m_sF, &m_nPhase, sizeof m_nPhase);
	if(LC_FAILED(pApp->Query((char*)"Get Script File with Index", m_sF)))
		return LC_EFAIL;

	pL= Lua_Open();
	m_pL	= pL;
	if(NULL == pL)
	{
		LOGE("CGpPhase::Create() Lua Open Failed. -------------------\n");
		return LC_EFAIL;
	}


	PLCX_INPUT	pIpt	= (PLCX_INPUT)LcVar_ReadP(LC_VAR_APP_INPUT);
	PLC_SPRITE	pSpt	= (PLC_SPRITE)LcVar_ReadP(LC_VAR_SPRITE0);
	LC_HANDLE	pMain	= (LC_HANDLE )LcVar_ReadP(LC_VAR_APP_MAIN);


	// LuaScript Initialize
	LscInput::Create(pIpt);
	LscTex	::Create(pSpt);
	LscFont	::Create(pSpt);
	LscSmd	::Create(pMain);
	LscFile	::Create(pMain);
	LscSpc	::Create(pSpt);
	LsxTsa	::Create(pSpt);
	LstDB	::Create(pSpt);
	LscHw	::Create(pSpt);
	LscNet	::Create(pSpt);

	Lua_OpenLibs(pL);

	LscSys	::OpenLib(pL);
	LscUtil	::OpenLib(pL);
	LscInput::OpenLib(pL);
	LscTex	::OpenLib(pL);
	LscFont	::OpenLib(pL);
	LscSmd	::OpenLib(pL);
	LscFile	::OpenLib(pL);
	LscSpc	::OpenLib(pL);
	LsxTsa	::OpenLib(pL);
	LstDB	::OpenLib(pL);
	LscHw	::OpenLib(pL);
	LscNet	::OpenLib(pL);

	char*	LuaFile= m_sF;

	hr = Lua_Load(pL, m_sF);
	if(LC_FAILED(hr))
	{
		const char*	s = Lua_GetString(pL, -1);
		LOGE("CGpPhase::Create() Script Err: %s\n\n", s);
		return LC_EFAIL;
	}

	// Lua_Init() 함수 호출
	if(LC_FAILED(LscLua_Init(m_pL)))
		return LC_EFAIL;

	return 0;
}


INT CGpPhase::FrameMove()
{
	PLC_APPFRAME	pApp = NULL;
	INT hr = 0;
	INT	r =0;

	pApp = (PLC_APPFRAME)m_pAppMain;

	hr = LscLua_FrameMove(m_pL);


	if(LC_FAILED(hr))
		return LC_EFAIL;

	else if(0==hr)
		return 0;


	r = pApp->Query((char*)"Set Phase", &hr);

	if(-2 == r)
		return LC_EFAIL;

	return 0;
}


INT CGpPhase::Render()
{
	LscLua_Render(m_pL);

	return 0;
}


INT CGpPhase::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CGpPhase::Release()
{
	return LC_EFAIL;
}

