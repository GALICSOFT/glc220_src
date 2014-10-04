// Implementation of the CGpApp class.
//
//  Created by Heesung Oh on 2011. 01. 19.
//  Copyright GALIC SOFT Inc. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "_StdAfx.h"

#include "GpApp.h"


INT CGpApp::InitScript(char* slua)
{
	LOGI("CGpApp::InitScript()\n");

	INT			hr = 0;
	LC_HANDLE	pL	= NULL;


	pL	= Lua_Open();			// open the lua
	if(NULL == pL)
		return LC_EFAIL;


	LscSys	::Create(this);		// Lua System
	LscUtil	::Create(this);		// Lua Utility

	LscSys	::OpenLib(pL);		// load the glue for system
	LscUtil	::OpenLib(pL);		// load the glue for util


#if defined(_MSC_VER)

	if(slua)
	{
		LOGE("LuaFile::%s\n", slua);
		hr = Lua_Load(pL, slua);
	}
	else
	{
		hr = Lua_Load(pL, (char*)"_main.lua");
		if(LC_FAILED(hr))
			hr = Lua_Load(pL, (char*)"main.lua");
	}

#elif defined(__ANDROID__)
	hr = Lua_Load(pL, (char*)"main.lua");

#else
	hr = Lua_Load(pL, (char*)"_main.lua");
	if(LC_FAILED(hr))
		hr = Lua_Load(pL, (char*)"main.lua");
#endif


	if(LC_FAILED(hr))
	{
		const char*	s = Lua_GetString(pL, -1);
		LcLog_Out(LC_LOG_ERROR, "Script Err: %s\n\n", s);
		return LC_EFAIL;
	}

	LscLua_Create(pL);

	Lua_Close(&pL);

	return LC_OK;
}



INT CGpApp::Init()
{
	LOGI("CGpApp::Init::Begin\n");

	INT hr = 0;


	// Set Start Phase..
	hr = this->ChangePhase(1);
	if(LC_FAILED(hr))
		return LC_EFAIL;


	LOGI("CGpApp::Init::End\n");


	return LC_OK;
}


INT CGpApp::Destroy()
{
	LOGI("CGpApp::Destroy\n");

	SAFE_DELETE(	m_Phase	);

	return LC_OK;
}


INT CGpApp::FrameMove()
{
	INT hr =0;

	// Input Update
	hr = m_pInput->Update();

	if (LC_FAILED(hr))
		return LC_EFAIL;


	LCXVECTOR3	vcMtsPos = m_pInput->GetMtsPos();
	//LC_STATE	nMtsState= m_pInput->MtsState();
	const BYTE*	pKey = m_pInput->GetKeyMap();
	const BYTE*	pMts = m_pInput->GetMtsMap();

	// Home Key
	if(pKey[LCIK_ESCAPE])
		return LC_EFAIL;


	//LOGI("CGpApp::FrameMove(): Read MtsMap ------------------------------\n");


	if(m_Phase)
	{
		hr = m_Phase->FrameMove();

		if(LC_FAILED(hr))
			return LC_EFAIL;
	}


	//LOGI("m_Phase->FrameMove(): Update Input ---------------------------\n");

	
	if(m_bShowSt)
	{
		char sbuf[128]={0};
		DOUBLE fFPS		   = m_TimeFPS;
		DOUBLE fElasedTime = m_TimeElps;

		sprintf(sbuf, "갈릭: m: %.f %.f %d, %.f(%.1f)F"
				, vcMtsPos.x, vcMtsPos.y, pMts[0], fFPS, fElasedTime);

		m_pFont->SetString(sbuf);
	}

	return LC_OK;
}



INT CGpApp::Render()
{
	// Phase 교환이 있으면 한번은 렌더링하지 않는다.
	if( m_PhaseO != m_PhaseC)
	{
		m_PhaseO = m_PhaseC;
		return LC_OK;
	}

	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, m_dClearCol);
	
	m_pDev->BeginScene();	// Prepare Rendering...
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);	// Clear Backbuffer

	m_pSpt->Begin();		// Sprite Begin

	if(m_Phase)
		m_Phase->Render();


	if(m_bShowSt)
		m_pFont->DrawTxt();

//	LCXRECT	rc;
//	LCXVECTOR2 vcPos;
//
//	vcPos = m_pFont->GetPosition();
//	m_pFont->GetAttrib(LC_FONT_RECT_SCALEUP, &rc);
//	vcPos.x += rc.w;
//
//	m_pFont->GetRect(&rc, 10);
//	vcPos.x = rc.x;
//	vcPos.y = rc.y;
//	m_pFont->DrawTxt(NULL, 0, -1, &vcPos);


	m_pSpt->End();				// Sprite End

	m_pDev->EndScene();			// Post Rendering...


	return LC_OK;
}

