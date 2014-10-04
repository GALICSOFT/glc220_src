// Implementation of the CGpApp class.
//
//  Created by Heesung Oh on 2011. 01. 19.
//  Copyright GALIC SOFT Inc. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1200
  #pragma warning(disable: 4996)
#endif


#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>
#include <LcStdev.h>
#include <LcxEntity.h>
#include <LcxMedia.h>

#include "GpApp.h"


CGpApp* CGpApp::m_pApp = NULL;


CGpApp::CGpApp()
{
	LOGI("CGpApp::CGpApp()\n");
	
	std::vector<int > t_vector;		// for importing std::vector
	t_vector.push_back(0);
	t_vector.clear();


	memset(m_sCls, 0, sizeof m_sCls);
	strcpy(m_sCls, "GaLic G-pack");

	m_nScnW		= 800		;
	m_nScnH		= 480		;
	m_nScnD		= 16		;
	m_nScnX		= -1		;
	m_nScnY		= -1		;
	m_bShowSt	= LC_TRUE	;
	m_dClearCol	= 0xFF006699;

	m_hWnd		= NULL		;
	m_pDev		= NULL		;
	m_pSpt		= NULL		;
	m_pInput	= NULL		;
	m_TimeFPS	= 120		;
	m_TimeElps	= 1000./m_TimeFPS;		// Ellapsed time
	m_pFont		= NULL		;


	// game data
	m_pGameFnt	= NULL		;
	m_pGameTex	= NULL		;
	m_pGameSnd	= NULL		;
}



INT CGpApp::Create(PLC_ARG pArg)
{
	INT	hr	= LC_OK;


	// Create Sound Device
	if(LC_FAILED(LcxSmd_DeviceOpen()))
	{
		LOGE("Create Sound Device Failed. -------------\n");
		return LC_EFAIL;
	}


	// Set Main procedure Function Pointer
	if(LC_FAILED(LcSys_SetMainRunFunc(CGpApp::Run3D)))
		return LC_EFAIL;


	// Create Window
	m_hWnd = LcSys_CreateWindow(CGpApp::WndProc, m_sCls, m_nScnW, m_nScnH, m_nScnX, m_nScnY);
	if(NULL == m_hWnd)
	{
		LOGE("Create Window Failed. -------------------\n");
		return LC_EFAIL;
	}


	// Create GLC Rendering Device
#if defined(__ANDROID__)
	INT bUseNative = LC_TRUE;
	hr = LcDev_CreateDevice(NULL, &m_pDev, m_hWnd, &bUseNative);
#else
	hr = LcDev_CreateDevice(NULL, &m_pDev, m_hWnd, NULL);
#endif

	if(LC_FAILED(hr))
	{
		LOGE("Create Device Failed. -------------------\n");
		return LC_EFAIL;
	}

	//save the main application instance
	LcVar_WriteP(LC_VAR_APP_MAIN, this);


	// Create 2D Rendering Sprite
	if(LC_FAILED(LcDev_CreateSprite(NULL, &m_pSpt)))
	{
		LOGE("Create Sprite Failed. -------------------\n");
		return LC_EFAIL;
	}

	LcVar_WriteP(LC_VAR_SPRITE0, m_pSpt);

	// Create Input
	if(LC_FAILED(LcxEnt_CreateInput(NULL, &m_pInput, m_hWnd)))
	{
		LOGE("Create Input Failed. --------------------\n");
		return LC_EFAIL;
	}


	// Create Font Library
	if(LC_FAILED(LcDev_TFFontOpen()))
	{
		LOGE("Create Font Library Failed. -------------\n");
		return LC_EFAIL;
	}


	////////////////////////////////////////////////////////////////////////////
	// Test Default Font...
	// Create Font
	TLC_FONT	tFont(LCCMD_FONT_DEFAULT, NULL, NULL, 12);
	LCXVECTOR2	vcPos = LCXVECTOR2(3, 450);
	LCXCOLOR	dCol  = LCXCOLOR(1.0F, 1.0F, 0.0F, 1);


	if(LC_FAILED(LcDev_CreateFontFromMemory(NULL, &m_pFont, m_pSpt, &tFont)))
	{
		LOGE("Create System Font Failed. --------------\n");
		return LC_EFAIL;
	}


	m_pFont->SetString("System Starting...");
	m_pFont->SetPosition(&vcPos);
	m_pFont->SetColor(&dCol);

	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, m_dClearCol);
	m_pDev->SetRenderState(LC_RS_SHADE_MODEL, LC_ST_SMOOTH);
	m_pDev->SetRenderState(LC_RS_DEPTH_TEST, LC_ST_ENABLE);
	m_pDev->SetRenderState(LC_RS_CULL_FACE, LC_ST_ENABLE);


	// Init Game Data
	if(LC_FAILED(Init()))
	{
		LOGE("Game Data Init Failed. ------------------\n");
		return LC_EFAIL;
	}

	LOGI("CGpApp::Create::Succeeded\n");

	return LC_OK;
}



INT CGpApp::Cleanup()
{
	INT	hr = 0;

	if(NULL == m_pDev)
		return LC_OK;


	Destroy();


	SAFE_RELEASE(	m_pFont		);	// Release Font

	LcDev_TFFontClose();			// Release Font Library
	LcxSmd_DeviceClose();			// Sound Device Release

	SAFE_RELEASE(	m_pInput	);	// Release Input
	SAFE_RELEASE(	m_pSpt		);	// Release Sprite

	// Release Device
	hr = m_pDev->Release();
	if(hr)
	{
		LOGE("Remain unrelease instance : %d\n", hr);
		LOGE(
				"The non-zero reference count means that\n"
				"there is something object not cleaned up.\n"
				);
	}
	
	m_pDev = NULL;

	LcSys_DestroyWindow(&m_hWnd);		// Release Window
	LOGI("CGpApp::Cleanup()\n");

	return LC_OK;
}


LRESULT LC_WINAPI CGpApp::WndProc(LC_HWND, UINT, WPARAM, LPARAM)
{
	return LC_TRUE;
}


void CGpApp::UpdateStatus()
{
	static INT		MaxCnt = 4;
	static INT		dFrame = 0;
	static DOUBLE	dBgn = ::LcSys_GetTickCount();
	DOUBLE			dCur = ::LcSys_GetTickCount();  // GetTickCount()


	LcSys_StoredTime(&dCur, 1);						// Save Stored Time

	++dFrame;

	if(dFrame>=MaxCnt)
	{
		m_TimeElps = (dCur-dBgn)/dFrame;

		if(1>m_TimeElps)
			m_TimeElps = 1;

		m_TimeFPS = 1000./m_TimeElps;
		dFrame = 0;
		dBgn = dCur;


		LcSys_ElapseTime(&m_TimeElps, 1);			// Save Elapsed Time
		LcSys_FramePerSecond(&m_TimeFPS, 1);		// Save Frames

		if     (1024< m_TimeFPS)	MaxCnt = 256;
		else if( 512< m_TimeFPS)	MaxCnt = 128;
		else if( 256< m_TimeFPS)	MaxCnt =  64;
		else if( 128< m_TimeFPS)	MaxCnt =  32;
		else if(  64< m_TimeFPS)	MaxCnt =  16;
		else if(  32< m_TimeFPS)	MaxCnt =   8;
		else						MaxCnt =   4;
	}
}


// Run Program
INT CGpApp::Run(LC_HANDLE)
{
#if defined(__APPLE__) || defined(__IPHONE__)

	return LcSys_Run();

#else

	while(1)
	{
		if(LC_FAILED(LcSys_Run()))
			break;
	}

	Cleanup();

#endif

	return LC_OK;
}



INT	CGpApp::Proc(LC_HANDLE)
{
	//LOGE("Proc-----------------------------------------------------------\n");

	if(LC_FAILED(FrameMove()))
		return LC_EFAIL;


	if(LC_FAILED(Render()))
		return LC_EFAIL;


#if !defined(__ANDROID__)
	m_pDev->Present(0,0,0,0);
#endif

	UpdateStatus();


	return LC_OK;
}


INT	LC_CALLBACK CGpApp::Run3D(LC_HANDLE pData)
{
	if(CGpApp::m_pApp)
		return CGpApp::m_pApp->Proc(pData);

	return LC_OK;
}



INT CGpApp::Query(char* sCmd, LC_HANDLE pData)
{
	INT hr = LC_OK;


	if(0 == LcStr_Stricmp((char*)"Get FPS", sCmd) )
		*((DOUBLE*)pData) = m_TimeFPS;

	else if(0 == LcStr_Stricmp((char*)"Get Time Delta", sCmd) )
		*((DOUBLE*)pData) = m_TimeElps;

	else if(0 == LcStr_Stricmp((char*)"Get Script File Package", sCmd) )
		return LC_EFAIL;

	else if(0 == LcStr_Stricmp((char*)"Window Resize", sCmd) )
		hr = m_pDev->SetRenderState(LC_RS_EAGL_RESIZE, pData);		// Resize Need CAEAGLLayer*

	
	else if(0 == LcStr_Stricmp((char*)"Get Clear Color", sCmd) )
		*((UINT*)pData) = m_dClearCol;



#if defined(_WIN32)
	else if(0 == LcStr_Stricmp((char*)"Get Window Handle", sCmd) )
		*((LC_HWND*)pData) = m_hWnd;
#endif


	else if(0 == LcStr_Stricmp((char*)"Set Clear Color", sCmd) )
	{
		hr = LC_EFAIL;
		m_dClearCol= *((UINT*)pData);
		if(m_pDev)
			m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, m_dClearCol);
	}

	else if(0 == LcStr_Stricmp((char*)"Set Show Status", sCmd) )
		m_bShowSt = *((INT*)pData);

	else if(0 == LcStr_Stricmp((char*)"Set Screen Position X", sCmd) )
		m_nScnX = *((INT*)pData);

	else if(0 == LcStr_Stricmp((char*)"Set Screen Position Y", sCmd) )
		m_nScnY = *((INT*)pData);

	else if(0 == LcStr_Stricmp((char*)"Set Screen Width", sCmd) )
		m_nScnW = *((INT*)pData);

	else if(0 == LcStr_Stricmp((char*)"Set Screen Height", sCmd) )
		m_nScnH = *((INT*)pData);

	else if(0 == LcStr_Stricmp((char*)"Set Class Name", sCmd) )
		strcpy(m_sCls, (char*)pData);

	else
		return LC_EFAIL;


	return hr;
}


INT CGpApp::SetAttrib(UINT nCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}



INT CGpApp::GetAttrib(UINT nCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}



////////////////////////////////////////////////////////////////////////////////


// get the main app
PLC_APPFRAME LcSys_GetMainApp()
{
	return CGpApp::m_pApp;
}

// GameApp Instance
INT LcSys_CreateAppFrame(char* sCmd					// Create  the main application instance. It must be implemented in program start.
					, PLC_APPFRAME* pOut
					, PLC_ARG pArg
					)
{
	PLC_APPFRAME pObj = NULL;

	if(NULL != CGpApp::m_pApp)
	{
		*pOut = CGpApp::m_pApp;
		return LC_FILE_ALREADY_OPEN;
	}


	*pOut = NULL;
	pObj = new CGpApp;

	if(0 > pObj->Create(pArg))
	{
		delete pObj;
		return LC_EFAIL;
	}

	CGpApp::m_pApp = (CGpApp*)pObj;
	*pOut = pObj;
	return LC_OK;
}


INT LcSys_DestroyAppFrame()
{
	if(NULL == CGpApp::m_pApp)
		return LC_OK;

	CGpApp::m_pApp->Cleanup();

	delete (CGpApp::m_pApp);
	CGpApp::m_pApp = NULL;

	return LC_OK;
}


INT LcSys_ProcAppFrame(LC_HANDLE)
{
	INT hr = 0;

	if(NULL == CGpApp::m_pApp)
		return LC_EFAIL;

	hr = CGpApp::m_pApp->Proc(NULL);
	return hr;
}


INT LcSys_ExecAppFrame(char* sCmd, LC_HANDLE pData)
{
	if     (0 == LcStr_Stricmp(sCmd, LCQUERY_INVALID))
	{
		return CGpApp::m_pApp->Invalid();
	}
	else if(0 == LcStr_Stricmp(sCmd, LCQUERY_RESTORE))
	{
		return CGpApp::m_pApp->Restore();
	}

	return LC_EFAIL;
}
