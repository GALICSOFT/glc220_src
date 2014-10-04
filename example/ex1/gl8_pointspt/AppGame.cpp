// Implementation of the CAppGame class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1200
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcMath.h>
#include <LcAux.h>


#include "AppGame.h"


CAppGame* CAppGame::m_pAppGame = NULL;


CAppGame::CAppGame()
{
	CAppGame::m_pAppGame = this;

	m_hWnd		= NULL;
	m_pDev		= NULL;

	memset(m_sCls, 0, sizeof(m_sCls));
	strcpy(m_sCls,"LC_Tutorial");


	m_nScnW		= 0;
	m_nScnH		= 0;

	m_fElapsed	= 0.0;
	m_fFPS		= 30.0F;
}


INT CAppGame::Create(LC_HANDLE p1, LC_HANDLE, LC_HANDLE, LC_HANDLE)
{
	UINT	dClearColor = 0xFF006699;

	// Init GLC Library
	if(LC_FAILED(LcSys_Init()))
		return LC_OK;

	// Set Main procedure Function Pointer
	if(LC_FAILED(LcSys_SetMainRunFunc(CAppGame::Run3D)))
		goto End;


	// Create Window
	m_hWnd = LcSys_CreateWindow(NULL, m_sCls);
	if(NULL == m_hWnd)
		goto End;


	LcVar_WindowSize(&m_nScnW, &m_nScnH);

	// Create GLC Rendering Device
	if(LC_FAILED(LcDev_CreateDevice(NULL, &m_pDev, m_hWnd, NULL)))
		goto End;

	//save the main application instance
	LcVar_WriteP(LC_VAR_APP_MAIN, this);


	// setup the rendering state
	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, dClearColor);
	m_pDev->SetRenderState(LC_RS_SHADE_MODEL, LC_ST_SMOOTH);
	m_pDev->SetRenderState(LC_RS_DEPTH_TEST, LC_ST_ENABLE);
	m_pDev->SetRenderState(LC_RS_CULL_FACE, LC_ST_ENABLE);


	// init game data
	if(LC_FAILED(Init()))
		goto End;


	return LC_OK;


End:
	Cleanup();
	return LC_OK;
}


INT CAppGame::Run(LC_HANDLE)
{
	while(1)	// Run Program
	{
		if(LC_FAILED(LcSys_Run()))
			break;

	}

	Cleanup();

	return LC_OK;
}


INT	CAppGame::Proc(LC_HANDLE)
{
	if(LC_FAILED(FrameMove()))
		return LC_EFAIL;


	if(LC_FAILED(Render()))
		return LC_EFAIL;

	m_pDev->Present(0,0,0,0);

	UpdateStatus();


	return LC_OK;
}


INT	LC_CALLBACK CAppGame::Run3D(LC_HANDLE v)
{
	if(CAppGame::m_pAppGame)
		return CAppGame::m_pAppGame->Proc(v);

	return LC_OK;
}


INT CAppGame::Cleanup()
{
	int hr = 0;


	Destroy();

	// Release Device
	hr = m_pDev->Release();
	if(0 != hr)
		printf(
				"The non-zero reference count means that\n"
				"there is something object not cleaned up.\n"
				);
	m_pDev = NULL;

	// Release Window
	LcSys_DestroyWindow(&m_hWnd);

	// Release GLC
	LcSys_Close();

	return LC_OK;
}



void CAppGame::UpdateStatus()
{
#if defined(_WIN32)
	static INT		MaxCount = 10;
#else
	static INT		MaxCount = 10;
#endif

	static INT     iCnt = 0;
	static DOUBLE   dBgn = ::LcSys_GetTickCount();
	DOUBLE		   dCur = ::LcSys_GetTickCount();  // GetTickCount()

	++iCnt;

	if(iCnt>=MaxCount)
	{
		m_fElapsed = (dCur-dBgn)/DOUBLE(iCnt);

		m_fFPS = 1000./m_fElapsed;
		iCnt = 0;
		dBgn = dCur;
	}
}




