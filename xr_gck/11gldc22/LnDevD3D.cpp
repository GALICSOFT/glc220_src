// Implementation of the LcDevD3D class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

// Need Platform SDK
#define _WIN32_WINNT	(0x0500)


#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>

// Platform SDK와 DirectX의 인클루드와 라이브러리 경로가 충돌할 수 있다.
// 만약 Platform SDK 2003의 인클루드 파일의 우선순위가 비주얼 스튜디오 보다 낮다면
// 비주얼 스투디오의 WinCon.h파일의 마지막에 
// WINBASEAPI HWND APIENTRY GetConsoleWindow(VOID);
// 이 함수를 추가해야 된다.
// 이 함수의 정의와 구현은 2003 버전 WinCon.h 파일에 있다.


#include <io.h>				// _open_osfhandle()
#include <WinCon.h>			// GetConsoleWindow()


#include <tchar.h>
#include <stdio.h>


#include <D3D9.h>
#include <d3dx9.h>
#include <dxerr9.h>



#include "LnBase.h"
#include "LnDev.h"



#include "DXUtil.h"
#include "D3DEnum.h"

#include "LnDevD3D.h"


// Error codes
enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOREF };

#define D3DAPPERR_NODIRECT3D          0x82000001
#define D3DAPPERR_NOWINDOW            0x82000002
#define D3DAPPERR_NOCOMPATIBLEDEVICES 0x82000003
#define D3DAPPERR_NOWINDOWABLEDEVICES 0x82000004
#define D3DAPPERR_NOHARDWAREDEVICE    0x82000005
#define D3DAPPERR_HALNOTCOMPATIBLE    0x82000006
#define D3DAPPERR_NOWINDOWEDHAL       0x82000007
#define D3DAPPERR_NODESKTOPHAL        0x82000008
#define D3DAPPERR_NOHALTHISMODE       0x82000009
#define D3DAPPERR_NONZEROREFCOUNT     0x8200000a
#define D3DAPPERR_MEDIANOTFOUND       0x8200000b
#define D3DAPPERR_RESETFAILED         0x8200000c
#define D3DAPPERR_NULLREFDEVICE       0x8200000d




static CLcDevD3D*	g_pD3DApp;



CLcDevD3D::CLcDevD3D()
{
	g_pD3DApp		= this;

	_tcscpy(m_sCls, "LcSDK1.0.0 Application");
	_tcscpy(m_sWin, "LcSDK1.0.0 Application");


	m_dStyle	= WS_VISIBLE | WS_CAPTION | WS_SYSMENU;					// Window Style
	m_dIcon		= 32512;												// <=IDC_ICON
	m_dAccl		= 0;													// Accelator ID
	m_dTggl		= 0;													// Toggle Key
	m_dExit		= 0;													// Exit Key

	m_iScnW		= 0;													// Screen Width
	m_iScnH		= 0;													// Screen Height
	m_iPosX		= 0;													// Screen Position X
	m_iPosY		= 0;													// Screen Position Y

	m_bStartFull= 0;													// Is Start Full Mode
	m_bFull		= m_bStartFull;											// 0: window, 1: Full

	m_bRender	= FALSE;
	m_bActive	= FALSE;
	m_bCusor	= FALSE;
	m_bShowStats= TRUE;

	m_dbFPS		= 24.;

	m_dClrMod	= D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
	m_dClrClr	= 0xFF006699;


	m_fTime							= 0.;
	m_fTimeNoPause					= 0.;
	m_fElapsedTime					= 0.;

	m_fTimeAv						= 1.f/m_dbFPS;

	m_sStatsDevice[0]				= _T('\0');
	m_sStatsFrame[0]				= _T('\0');




	m_hWndFocus						= NULL;
	m_hMenu							= NULL;
	m_bWindowed						= TRUE;
	m_bActive						= FALSE;
	m_bDeviceLost					= FALSE;
	m_bMinimized					= FALSE;
	m_bMaximized					= FALSE;
	m_bIgnoreSizeChange				= FALSE;
	m_bDeviceInited					= FALSE;
	m_bDeviceRestored				= FALSE;


	m_bDragFullUse					= FALSE;
	m_bDragFullWin					= FALSE;
	
	m_dwCreateFlags					= 0;
	m_bLoadingApp					= TRUE;
	m_bAntialias					= TRUE;

	m_bFrameMoving					= TRUE;
	m_bSingleStep					= FALSE;

	m_bShowCursorWhenFullscreen		= TRUE;

	m_d3dEnum.AppUsesDepthBuffer	= TRUE;
	m_d3dEnum.AppMinStencilBits		= 8;
	m_d3dEnum.AppMinDepthBits		= 24;

	m_bLoadingRnd					= 0;
	m_fMaxFrame						= 20.f;


	// When m_bClipCursorWhenFullscreen is TRUE, the cursor is limited to
	// the device window when the app goes fullscreen.  This prevents users
	// from accidentally clicking outside the app window on a multimon system.
	// This flag is turned off by default for debug builds, since it makes
	// multimon debugging difficult.
#if defined(_DEBUG) || defined(DEBUG)
	m_bClipCursorWhenFullscreen = FALSE;
#else
	m_bClipCursorWhenFullscreen = TRUE;
#endif

	Pause( TRUE ); // Pause until we're ready to render

}


CLcDevD3D::~CLcDevD3D()
{
}



INT CLcDevD3D::Init()
{
	if(m_pFncInit)
		return m_pFncInit();

	return 0;
}


void CLcDevD3D::Destroy()
{
	if(m_pFncDestroy)
		m_pFncDestroy();
}


INT CLcDevD3D::Restore()
{
	if(m_pFncInit)
		return m_pFncRestore();

	return 0;
}

void CLcDevD3D::Invalidate()
{
	if(m_pFncInvalidate)
		m_pFncInvalidate();
}


INT	CLcDevD3D::FrameMove()
{
	if(m_pFncFrameMove)
		return m_pFncFrameMove();
	
	return 0;
}

INT CLcDevD3D::Render()
{
	if(m_pFncRender)
		return m_pFncRender();

	return 0;
}






INT CLcDevD3D::Query(char* sCmd, void* pData)
{
	if     (0==_stricmp("Set FuncMsgProc"	, sCmd)){	m_pFncMsgProc	= (ULONG_PTR (__cdecl*)(HWND,UINT,WPARAM,LPARAM))pData;	return 0;	}
	else if(0==_stricmp("Set FuncInit"		, sCmd)){	m_pFncInit		= (ULONG_PTR (__cdecl*)(void))pData;	return 0;	}
	else if(0==_stricmp("Set FuncDestoy"	, sCmd)){	m_pFncDestroy	= (ULONG_PTR (__cdecl*)(void))pData;	return 0;	}
	else if(0==_stricmp("Set FuncRestore"	, sCmd)){	m_pFncRestore	= (ULONG_PTR (__cdecl*)(void))pData;	return 0;	}
	else if(0==_stricmp("Set FuncInvalidate", sCmd)){	m_pFncInvalidate= (ULONG_PTR (__cdecl*)(void))pData;	return 0;	}
	else if(0==_stricmp("Set FuncFrameMove"	, sCmd)){	m_pFncFrameMove	= (ULONG_PTR (__cdecl*)(void))pData;	return 0;	}
	else if(0==_stricmp("Set FuncRender"	, sCmd)){	m_pFncRender	= (ULONG_PTR (__cdecl*)(void))pData;	return 0;	}
	else if(0==_stricmp("Set FuncNetCreate"	, sCmd)){	m_pFncNetCreate	= (ULONG_PTR (__cdecl*)(void*))pData;	return 0;	}

	else if(0==_stricmp("Set Style Code", sCmd))
	{
		DWORD	dVal = *((DWORD*)pData);
		m_dStyle = dVal;
		return 0;
	}
	else if(0==_stricmp("Set Icon Code", sCmd))
	{
		
		DWORD	dVal = *((DWORD*)pData);
		m_dIcon	= dVal;
		return 0;
	}
	else if(0==_stricmp("Set Accelator Code", sCmd))
	{
		DWORD	dVal = *((DWORD*)pData);
		m_dAccl	= dVal;
		return 0;
	}
	else if(0==_stricmp("Set Toggle Code", sCmd))
	{		
		DWORD	dVal = *((DWORD*)pData);
		m_dTggl	= dVal;
		return 0;
	}

	else if(0==_stricmp("Set Exit Code", sCmd))
	{
		DWORD	dVal = *((DWORD*)pData);
		m_dExit	= dVal;
		return 0;
	}

	else if(0==_stricmp("Get Device State"	, sCmd))
	{
		sprintf((char*)pData, m_sStatsDevice);
		return 0;
	}


	else if(0==_stricmp("Get Frame State"	, sCmd))
	{
		sprintf((char*)pData, m_sStatsFrame);
		return 0;
	}

	else if(0==_stricmp("TimeGetTime", sCmd))
	{
		*((DOUBLE*)pData) = TimeGetTime();
		return 0;
	}

	else if(0==_stricmp("Get FPS", sCmd))
	{
		*((DOUBLE*)pData) = m_dbFPS;
		return 0;
	}

	else if(0==_stricmp("Get Elapsed Average Time", sCmd))
	{
		*((DOUBLE*)pData) = m_fTimeAv;
		return 0;
	}


	else if(0==_stricmp("Set Screen Pos X"	, sCmd))	{	INT	dVal = *((INT*)pData);	m_iPosX = dVal;		return 0;	}
	else if(0==_stricmp("Set Screen Pos Y"	, sCmd))	{	INT	dVal = *((INT*)pData);	m_iPosY = dVal;		return 0;	}
	else if(0==_stricmp("Set Screen Width"	, sCmd))	{	INT	dVal = *((INT*)pData);	m_iScnW = dVal;		return 0;	}
	else if(0==_stricmp("Set Screen Height"	, sCmd))	{	INT	dVal = *((INT*)pData);	m_iScnH = dVal;		return 0;	}
	
	else if(0==_stricmp("Get Screen Width"	, sCmd))	{	*((INT*)pData) = m_iScnW;	return 0;	}
	else if(0==_stricmp("Get Screen Height"	, sCmd))	{	*((INT*)pData) = m_iScnH;	return 0;	}

	else if(0==_stricmp("Set Class Name"	, sCmd))
	{
		char*	sVal = (char*)pData;
		strcpy(m_sCls, sVal);
		return 0;
	}

	else if(0==_stricmp("Set Window Title"	, sCmd))
	{
		char*	sVal = (char*)pData;
		strcpy(m_sWin, sVal);
		return 0;
	}
	
	else if(0==_stricmp("Set FullMode Start", sCmd))
	{
		BOOL bVal = *((BOOL*)pData);
		m_bStartFull = bVal;
		m_bFull		= m_bStartFull;
		return 0;
	}

	else if(0==_stricmp("Set Script File"	, sCmd))
	{
		INT		nIdx = 0;
		char	sFile[MAX_PATH]={0};
		char*	sVal = (char*)pData;
		
		memcpy(&nIdx, sVal, sizeof nIdx);
		strcpy(sFile, sVal+ 4);


		INT iSize = (INT)m_vScFile.size();

		if(nIdx>=iSize)
		{
			for(int i=0; i< (nIdx-iSize+1); ++i)
			{
				char* sTmp = new char[MAX_PATH];

				memset(sTmp, 0, sizeof(char) * MAX_PATH);
				m_vScFile.push_back(sTmp);
			}
		}

		vector<char* >::iterator pDst = m_vScFile.begin() + nIdx;
		strcpy(*pDst, sFile);

		return 0;
	}

	else if(0==_stricmp("Get Script File"	, sCmd))
	{
		INT		nIdx = 0;
		char	sFile[MAX_PATH]={0};
		char*	sVal = (char*)pData;
		INT		iSize= (INT)m_vScFile.size();;
		
		memcpy(&nIdx, sVal, sizeof nIdx);

		if(nIdx>=iSize)
			return -1;

		vector<char* >::iterator pDst = m_vScFile.begin() + nIdx;
		_tcscpy(sVal, *pDst);

		return 0;
	}

	else if(0==_stricmp("Get Script File Number", sCmd))
	{
		INT iSize = (INT)m_vScFile.size();
		*((INT*)pData) = iSize;

		return 0;
	}

	else if(0==_stricmp("Console Alloc", sCmd))
	{
		if(m_ConsoleH)
			return 0;

//		#if defined(_DEBUG) || defined(DEBUG)
//			AllocConsole();
//			freopen("CONIN$", "r", stdin);
//			freopen("CONOUT$", "w", stdout); 
//			freopen("CONOUT$", "w", stderr);
//		#endif


		m_ConsoleB	=	::AllocConsole();
		m_ConsoleH	=	::GetStdHandle(STD_OUTPUT_HANDLE);

		if(INVALID_HANDLE_VALUE == m_ConsoleH)
			return -1;


		HWND	ConsoleHwnd = GetConsoleWindow();
		RECT	rcC={0};
		GetWindowRect(ConsoleHwnd, &rcC);

		INT	nConW = rcC.right  - rcC.left;
		INT	nConH = rcC.bottom - rcC.top;
		MoveWindow(ConsoleHwnd, 0, 0, nConW, INT(nConH*1.5), TRUE);

		m_ConsoleR	=	_open_osfhandle((int)m_ConsoleH, 0x4000);	// _O_TEXT=>0x4000
		m_ConsoleF	=	fdopen(m_ConsoleR, "w");
		*stdout = *m_ConsoleF;
		setvbuf(stdout, NULL, _IONBF, 0);

		if(m_hWnd)
			::SetFocus(m_hWnd);

		return 0;
	}
	
	else if(0==_stricmp("Set Console"		, sCmd))
	{
		BOOL bCon = *((INT*)pData);
	
		if(bCon)
		{
			if(m_ConsoleH)
				return 0;
			
			m_ConsoleB	=	::AllocConsole();
			m_ConsoleH	=	::GetStdHandle(STD_OUTPUT_HANDLE);

			if(INVALID_HANDLE_VALUE == m_ConsoleH)
				return -1;


			HWND	ConsoleHwnd = GetConsoleWindow();
			RECT	rcC={0};
			GetWindowRect(ConsoleHwnd, &rcC);

			INT	nConW = rcC.right  - rcC.left;
			INT	nConH = rcC.bottom - rcC.top;
			MoveWindow(ConsoleHwnd, 0, 0, nConW, INT(nConH*1.5), TRUE);


			m_ConsoleR	=	::_open_osfhandle((int)m_ConsoleH, 0x4000);	// _O_TEXT=>0x4000
			m_ConsoleF	=	::fdopen(m_ConsoleR, "w");
			*stdout = *m_ConsoleF;
			::setvbuf(stdout, NULL, _IONBF, 0);

			return 0;
		}
		
		else
		{
			if(m_ConsoleH)
			{
				::fclose( m_ConsoleF);
				::CloseHandle(m_ConsoleH);
				::FreeConsole();
			}
		}

		if(m_hWnd)
			::SetFocus(m_hWnd);

		return 0;
	}
	
	else if(0==_stricmp("Send Console", sCmd))
	{
		if(!m_ConsoleB)
			return -1;

		char* s = (char*)pData;
		
		WriteConsole(m_ConsoleH, s, _tcslen(s), &m_ConsoleW, NULL);
		printf(s);

		return 0;
	}

	else if(0==_stricmp("Set Show State", sCmd))
	{
		BOOL	dVal = *((BOOL*)pData);
		m_bShowStats = dVal;
		return 0;
	}

	else if(0==_stricmp("Set Toggle Full", sCmd))
	{
		INT		dVal =*((INT*)pData);
		if(m_bFull != dVal)
		{
			m_bFull = dVal;
			SendMessage(m_hWnd, WM_COMMAND, m_dTggl, 0);
		}

		return -1;
	}
	
	else if(0==_stricmp("Set Clear Color", sCmd))
	{
		DWORD	dVal = *((DWORD*)pData);
		m_dClrClr= dVal;
		return 0;
	}


	else if(0==_stricmp("Get Time Stored", sCmd))
	{
		DOUBLE fElipse = TimeGetTime();

		fElipse -=m_fTimeSrt;

		*((DOUBLE*)pData) = fElipse;

		return 0;
	}

	else if(0==_stricmp("Get Show State", sCmd))	{	*((BOOL*)pData) = m_bShowStats;		return 0;	}
	else if(0==_stricmp("Get Clear Color", sCmd))	{	*((DWORD*)pData) = m_dClrClr;		return 0;	}

	else if(0==_stricmp("Get Clear Mode", sCmd))	{	*((DWORD*)pData) = m_dClrMod;		return 0;	}

	else if(0==_stricmp("Set 2D Sprite"	, sCmd))	{	m_pDev2D = pData;					return 0;	}
	else if(0==_stricmp("Get 2D Sprite"	, sCmd))	{	*((INT*)(pData)) = (INT)m_pDev2D;	return 0;	}

	else if(0==_stricmp("Set Input"		, sCmd))	{	m_pInput = pData;					return 0;	}
	else if(0==_stricmp("Get Input"		, sCmd))	{	*((INT*)(pData)) = (INT)m_pInput;	return 0;	}

	else if(0==_stricmp("Set Camera"	, sCmd))	{	m_pCam = pData;						return 0;	}
	else if(0==_stricmp("Get Camera"	, sCmd))	{	*((INT*)(pData)) = (INT)m_pCam;		return 0;	}

	else if(0==_stricmp("Set Network"	, sCmd))	{	m_pNet = pData;						return 0;	}
	else if(0==_stricmp("Get Network"	, sCmd))	{	*((INT*)(pData)) = (INT)m_pNet;		return 0;	}

	else if(0==_stricmp("Set Ime"		, sCmd))	{	m_pIme = pData;						return 0;	}
	else if(0==_stricmp("Get Ime"		, sCmd))	{	*((INT*)(pData)) = (INT)m_pIme;		return 0;	}

	else if(0==_stricmp("Set Command"	, sCmd))	{	m_pCmd = pData;						return 0;	}
	else if(0==_stricmp("Get Command"	, sCmd))	{	*((INT*)(pData)) = (INT)m_pCmd;		return 0;	}


	else if(0==_stricmp("Set User Data 1", sCmd))	{	m_pUser1 = pData;					return 0;	}
	else if(0==_stricmp("Get User Data 1", sCmd))	{	*((INT*)(pData)) = (INT)m_pUser1;	return 0;	}

	else if(0==_stricmp("Set User Data 2", sCmd))	{	m_pUser2 = pData;					return 0;	}
	else if(0==_stricmp("Get User Data 2", sCmd))	{	*((INT*)(pData)) = (INT)m_pUser2;	return 0;	}

	else if(0==_stricmp("Set User Data 3", sCmd))	{	m_pUser3 = pData;					return 0;	}
	else if(0==_stricmp("Get User Data 3", sCmd))	{	*((INT*)(pData)) = (INT)m_pUser3;	return 0;	}

	else if(0==_stricmp("Set User Data 4", sCmd))	{	m_pUser4 = pData;					return 0;	}
	else if(0==_stricmp("Get User Data 4", sCmd))	{	*((INT*)(pData)) = (INT)m_pUser4;	return 0;	}

	return CLcDev::Query(sCmd, pData);
}


INT CLcDevD3D::BeginScene()
{
	if(!m_pDev)
		return -1;
	
	return m_pDev->BeginScene();
}

INT CLcDevD3D::EndScene()
{
	if(!m_pDev)
		return -1;

	return m_pDev->EndScene();
}


INT CLcDevD3D::Clear(DWORD Count,CONST RECT* pRects,DWORD Flags, DWORD Color,float Z,DWORD Stencil)
{
	return m_pDev->Clear(Count, (CONST D3DRECT*)pRects,Flags,Color,Z,Stencil);
}





// Window Create
INT CLcDevD3D::Create(void* p1, void*, void*, void*)
{
	HRESULT hr=-1;

	LcDev*	pLnDev = (LcDev*)p1;

	m_hInst	= GetModuleHandle(NULL);




	m_bDragFullWin = 0;
	SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, FALSE, &m_bDragFullWin, 0);
	SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, 0,0);
	
	// Create the Direct3D object
	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	if( m_pD3D == NULL )
		return DisplayErrorMsg( D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT );

	// Build a list of Direct3D adapters, modes and devices. The
	// ConfirmDevice() callback is used to confirm that only devices that
	// meet the app's requirements are considered.
	m_d3dEnum.SetD3D( m_pD3D );
	m_d3dEnum.ConfirmDeviceCallback = ConfirmDeviceHelper;
	if( FAILED( hr = m_d3dEnum.Enumerate() ) )
	{
		SAFE_RELEASE( m_pD3D );
		return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
	}

	
	

	if(pLnDev)
	{
		m_hInst	= (pLnDev->hInst)? pLnDev->hInst: GetModuleHandle(NULL);

		if( _tcslen(pLnDev->sCls))
		{
			_tcscpy(m_sCls, pLnDev->sCls);
			_tcscpy(m_sWin, pLnDev->sCls);
		}

		if(pLnDev->hWnd)
			m_hWnd = pLnDev->hWnd;

		
		pLnDev->hInst = m_hInst;

		m_iScnW	= pLnDev->ScnX;
		m_iScnH	= pLnDev->ScnY;

		m_iPosX	= pLnDev->PosX;
		m_iPosY	= pLnDev->PosY;	
	}




	// Register the windows class
	WNDCLASS wc;

	wc.style			= CS_CLASSDC|CS_DBLCLKS;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;

	wc.hInstance		= m_hInst;
	wc.hIcon			= LoadIcon( m_hInst, MAKEINTRESOURCE( m_dIcon ) );
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground	=(HBRUSH)GetStockObject(WHITE_BRUSH);
	
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= m_sCls;


	RegisterClass( &wc );




	if(0==m_iScnW)	m_iScnW = ::GetSystemMetrics(SM_CXSCREEN);
	if(0==m_iScnH)	m_iScnH = ::GetSystemMetrics(SM_CYSCREEN);

	RECT rc={0,0,m_iScnW, m_iScnH};

	AdjustWindowRect( &rc, m_dStyle, m_hMenu? TRUE: FALSE );


	if(-1==m_iPosX)
	{
		m_iPosX = ::GetSystemMetrics(SM_CXSCREEN)/2;
		m_iPosX -= (rc.right  - rc.left)/2;
	}

	if(-1==m_iPosY)
	{
		m_iPosY = ::GetSystemMetrics(SM_CYSCREEN)/2;
		m_iPosY -=(rc.bottom - rc.top)/2;
	}

	if(pLnDev)
	{
		pLnDev->ScnX= m_iScnW;
		pLnDev->ScnY= m_iScnH;

		pLnDev->PosX= m_iPosX;
		pLnDev->PosY= m_iPosY;
	}


	m_hWnd = CreateWindow(
		m_sCls
		, m_sWin
		, m_dStyle
		, m_iPosX
		, m_iPosY
		, rc.right  - rc.left
		, rc.bottom - rc.top
		, GetDesktopWindow()
		, NULL
		, m_hInst
		, NULL );








	// The focus window can be a specified to be a different window than the
	// device window.  If not, use the device window as the focus window.
	if( m_hWndFocus == NULL )
		m_hWndFocus = m_hWnd;

	// Save window properties
	m_dStyle = GetWindowLong( m_hWnd, GWL_STYLE );
	GetWindowRect( m_hWnd, &m_rcWindowBounds );
	GetClientRect( m_hWnd, &m_rcWindowClient );

	if( FAILED( hr = ChooseInitialD3DSettings() ) )
	{
		SAFE_RELEASE( m_pD3D );
		return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
	}

	// Initialize the application timer
	DXUtil_Timer( TIMER_START );

	// Initialize the app's custom scene stuff
	if( FAILED( hr = OneTimeSceneInit() ) )
	{
		SAFE_RELEASE( m_pD3D );
		return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
	}

	// Initialize the 3D environment for the app
	if( FAILED( hr = Initialize3DEnvironment() ) )
	{
		SAFE_RELEASE( m_pD3D );
		return -1;
	}

	// The app is ready to go
	Pause( FALSE );

	return S_OK;
}







BOOL CLcDevD3D::FindBestWindowedMode( BOOL bRequireHAL, BOOL bRequireREF )
{
	// Get display mode of primary adapter (which is assumed to be where the window
	// will appear)
	D3DDISPLAYMODE primaryDesktopDisplayMode;
	m_pD3D->GetAdapterDisplayMode(0, &primaryDesktopDisplayMode);

	CD3DEnum::D3DAdapterInfo* pBestAdapterInfo = NULL;
	CD3DEnum::D3DDeviceInfo* pBestDeviceInfo = NULL;
	CD3DEnum::D3DDeviceCombo* pBestDeviceCombo = NULL;

	for( UINT iai = 0; iai < m_d3dEnum.m_pAdapterInfoList->Count(); iai++ )
	{
		CD3DEnum::D3DAdapterInfo* pAdapterInfo = (CD3DEnum::D3DAdapterInfo*)m_d3dEnum.m_pAdapterInfoList->GetPtr(iai);

		for( UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++ )
		{
			CD3DEnum::D3DDeviceInfo* pDeviceInfo = (CD3DEnum::D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);

			if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
				continue;

			if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
				continue;

			for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
			{
				CD3DEnum::D3DDeviceCombo* pDeviceCombo = (CD3DEnum::D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
				BOOL bAdapterMatchesBB = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);

				if (!pDeviceCombo->IsWindowed)
					continue;

				if (pDeviceCombo->AdapterFormat != primaryDesktopDisplayMode.Format)
					continue;

				// If we haven't found a compatible DeviceCombo yet, or if this set
				// is better (because it's a HAL, and/or because formats match better),
				// save it

				if( pBestDeviceCombo == NULL ||
					pBestDeviceCombo->DevType != D3DDEVTYPE_HAL && pDeviceCombo->DevType == D3DDEVTYPE_HAL ||
					pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB )
				{
					pBestAdapterInfo = pAdapterInfo;
					pBestDeviceInfo = pDeviceInfo;
					pBestDeviceCombo = pDeviceCombo;
				
					if( pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB )
					{
						// This windowed device combo looks great -- take it
						goto EndWindowedDeviceComboSearch;
					}
					// Otherwise keep looking for a better windowed device combo
				}
			}
		}
	}

EndWindowedDeviceComboSearch:
	
	if (pBestDeviceCombo == NULL )
		return FALSE;

	m_d3dSettings.pWindowed_AdapterInfo = pBestAdapterInfo;
	m_d3dSettings.pWindowed_DeviceInfo = pBestDeviceInfo;
	m_d3dSettings.pWindowed_DeviceCombo = pBestDeviceCombo;
	m_d3dSettings.IsWindowed = TRUE;
	m_d3dSettings.Windowed_DisplayMode = primaryDesktopDisplayMode;
	m_d3dSettings.Windowed_Width = m_rcWindowClient.right - m_rcWindowClient.left;
	m_d3dSettings.Windowed_Height = m_rcWindowClient.bottom - m_rcWindowClient.top;
	
	if (m_d3dEnum.AppUsesDepthBuffer)
		m_d3dSettings.Windowed_DepthStencilBufferFormat = *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);
	
//	m_d3dSettings.Windowed_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
//	m_d3dSettings.Windowed_MultisampleQuality = 0;

	
	if(m_bAntialias)
	{
		int nCount = pBestDeviceCombo->pMultiSampleTypeList->Count();
		m_d3dSettings.Windowed_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(nCount-1);

		nCount = pBestDeviceCombo->pMultiSampleQualityList->Count();
		m_d3dSettings.Windowed_MultisampleQuality = *(DWORD*)pBestDeviceCombo->pMultiSampleQualityList->GetPtr(nCount-1);

		if(m_d3dSettings.Windowed_MultisampleQuality>0)
			m_d3dSettings.Windowed_MultisampleQuality -=1;
	}
	else
	{
		m_d3dSettings.Windowed_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
		m_d3dSettings.Windowed_MultisampleQuality = 0;
	}




	m_d3dSettings.Windowed_VertexProcessingType = *(INT*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
	m_d3dSettings.Windowed_PresentInterval = *(UINT*)pBestDeviceCombo->pPresentIntervalList->GetPtr(0);
	
	return TRUE;
}



BOOL CLcDevD3D::FindBestFullscreenMode( BOOL bRequireHAL, BOOL bRequireREF )
{
	// For fullscreen, default to first HAL DeviceCombo that supports the current desktop
	// display mode, or any display mode if HAL is not compatible with the desktop mode, or
	// non-HAL if no HAL is available
	D3DDISPLAYMODE adapterDesktopDisplayMode;
	D3DDISPLAYMODE bestAdapterDesktopDisplayMode;
	D3DDISPLAYMODE bestDisplayMode;
	bestAdapterDesktopDisplayMode.Width = 0;
	bestAdapterDesktopDisplayMode.Height = 0;
	bestAdapterDesktopDisplayMode.Format = D3DFMT_UNKNOWN;
	bestAdapterDesktopDisplayMode.RefreshRate = 0;

	CD3DEnum::D3DAdapterInfo* pBestAdapterInfo = NULL;
	CD3DEnum::D3DDeviceInfo* pBestDeviceInfo = NULL;
	CD3DEnum::D3DDeviceCombo* pBestDeviceCombo = NULL;

	for( UINT iai = 0; iai < m_d3dEnum.m_pAdapterInfoList->Count(); iai++ )
	{
		CD3DEnum::D3DAdapterInfo* pAdapterInfo = (CD3DEnum::D3DAdapterInfo*)m_d3dEnum.m_pAdapterInfoList->GetPtr(iai);

		m_pD3D->GetAdapterDisplayMode( pAdapterInfo->AdapterOrdinal, &adapterDesktopDisplayMode );

		for( UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++ )
		{
			CD3DEnum::D3DDeviceInfo* pDeviceInfo = (CD3DEnum::D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);

			if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
				continue;

			if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
				continue;

			for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
			{
				CD3DEnum::D3DDeviceCombo* pDeviceCombo = (CD3DEnum::D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
				BOOL bAdapterMatchesBB = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);
				BOOL bAdapterMatchesDesktop = (pDeviceCombo->AdapterFormat == adapterDesktopDisplayMode.Format);

				if (pDeviceCombo->IsWindowed)
					continue;

				// If we haven't found a compatible set yet, or if this set
				// is better (because it's a HAL, and/or because formats match better),
				// save it
				if (pBestDeviceCombo == NULL ||
					pBestDeviceCombo->DevType != D3DDEVTYPE_HAL && pDeviceInfo->DevType == D3DDEVTYPE_HAL ||
					pDeviceCombo->DevType == D3DDEVTYPE_HAL && pBestDeviceCombo->AdapterFormat != adapterDesktopDisplayMode.Format && bAdapterMatchesDesktop ||
					pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop && bAdapterMatchesBB )
				{
					bestAdapterDesktopDisplayMode = adapterDesktopDisplayMode;
					pBestAdapterInfo = pAdapterInfo;
					pBestDeviceInfo = pDeviceInfo;
					pBestDeviceCombo = pDeviceCombo;
					if (pDeviceInfo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop && bAdapterMatchesBB)
					{
						// This fullscreen device combo looks great -- take it
						goto EndFullscreenDeviceComboSearch;
					}
					// Otherwise keep looking for a better fullscreen device combo
				}
			}
		}
	}
EndFullscreenDeviceComboSearch:

	if (pBestDeviceCombo == NULL)
		return FALSE;

	// Need to find a display mode on the best adapter that uses pBestDeviceCombo->AdapterFormat
	// and is as close to bestAdapterDesktopDisplayMode's res as possible
	bestDisplayMode.Width = 0;
	bestDisplayMode.Height = 0;
	bestDisplayMode.Format = D3DFMT_UNKNOWN;
	bestDisplayMode.RefreshRate = 0;
	
	
	D3DDISPLAYMODE* pdmDst = NULL;
	
	int idm =0;
	INT nCntDisplayMode = pBestAdapterInfo->pDisplayModeList->Count();
	INT nHz = MonitorMaxHz(this->m_iScnW, this->m_iScnH, 32);
	
	
	for(idm=0; idm < nCntDisplayMode; ++idm)
	{
		D3DDISPLAYMODE* pdm = (D3DDISPLAYMODE*)pBestAdapterInfo->pDisplayModeList->GetPtr(idm);
		
		if( pdm->Format != pBestDeviceCombo->AdapterFormat )
			continue;
		
		if( pdm->Width == this->m_iScnW &&
			pdm->Height == this->m_iScnH &&
			pdm->RefreshRate > bestDisplayMode.RefreshRate &&
			pdm->RefreshRate <= DWORD(nHz))
		{
			bestDisplayMode = *pdm;
			pdmDst			= pdm;
		}
	}
	
	
	if(NULL == pdmDst)
	{
		for(idm=0; idm < nCntDisplayMode; ++idm)
		{
			D3DDISPLAYMODE* pdm = (D3DDISPLAYMODE*)pBestAdapterInfo->pDisplayModeList->GetPtr(idm);
			
			if( pdm->Format != pBestDeviceCombo->AdapterFormat )
				continue;
			
			if( pdm->Width == this->m_iScnW &&
				pdm->Height == this->m_iScnH &&
				pdm->RefreshRate == bestAdapterDesktopDisplayMode.RefreshRate )
			{
				// found a perfect match, so stop
				bestDisplayMode = *pdm;
				break;
			}
			
			else if( pdm->Width == this->m_iScnW &&
				pdm->Height == this->m_iScnH &&
				pdm->RefreshRate > bestDisplayMode.RefreshRate )
			{
				// refresh rate doesn't match, but width/height match, so keep this
				// and keep looking
				bestDisplayMode = *pdm;
			}
			else if( pdm->Width == bestAdapterDesktopDisplayMode.Width )
			{
				// width matches, so keep this and keep looking
				bestDisplayMode = *pdm;
			}
			else if( bestDisplayMode.Width == 0 )
			{
				// we don't have anything better yet, so keep this and keep looking
				bestDisplayMode = *pdm;
			}
		}
	}
	
	m_d3dSettings.pFullscreen_AdapterInfo = pBestAdapterInfo;
	m_d3dSettings.pFullscreen_DeviceInfo = pBestDeviceInfo;
	m_d3dSettings.pFullscreen_DeviceCombo = pBestDeviceCombo;
	m_d3dSettings.IsWindowed = FALSE;
	m_d3dSettings.Fullscreen_DisplayMode = bestDisplayMode;

	if (m_d3dEnum.AppUsesDepthBuffer)
		m_d3dSettings.Fullscreen_DepthStencilBufferFormat = *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);

//	m_d3dSettings.Fullscreen_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
//	m_d3dSettings.Fullscreen_MultisampleQuality = 0;


	if(m_bAntialias)
	{
		INT nCount = pBestDeviceCombo->pMultiSampleTypeList->Count();
		m_d3dSettings.Fullscreen_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(nCount-1);

		nCount = pBestDeviceCombo->pMultiSampleQualityList->Count();
		m_d3dSettings.Fullscreen_MultisampleQuality = *(DWORD*)pBestDeviceCombo->pMultiSampleQualityList->GetPtr(nCount-1);

		if(m_d3dSettings.Fullscreen_MultisampleQuality>0)
			m_d3dSettings.Fullscreen_MultisampleQuality -=1;
	}
	else
	{
		m_d3dSettings.Fullscreen_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
		m_d3dSettings.Fullscreen_MultisampleQuality =0;
	}

	


	m_d3dSettings.Fullscreen_VertexProcessingType = *(INT*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
	m_d3dSettings.Fullscreen_PresentInterval = D3DPRESENT_INTERVAL_DEFAULT;

	return TRUE;
}




INT	CLcDevD3D::MonitorMaxHz(INT nScnW, INT nScnH, INT nBitsPerPel)
{
	int nMax = -1;

	DEVMODE dmModCur={0};
	DEVMODE dmModNew={0};

//	dmModCur.dmSize = sizeof(DEVMODE);
	
	if(0 ==EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmModCur))
		return -1;


	memcpy(&dmModNew, &dmModCur, sizeof dmModNew);
	dmModNew.dmPelsWidth	= nScnW;
	dmModNew.dmPelsHeight	= nScnH;
	dmModNew.dmBitsPerPel	= nBitsPerPel;

	for(int i=60; i<=75; ++i)
	{
		DWORD hr = -1;
		
		dmModNew.dmDisplayFrequency = i;
		hr = ChangeDisplaySettings(&dmModNew, CDS_TEST);

		if(DISP_CHANGE_SUCCESSFUL == hr)
			nMax = i;
	}


	DEVMODE dmModOld;
	memcpy(&dmModOld, &dmModCur, sizeof dmModOld);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmModOld);

	
	if( 0!=memcmp(&dmModOld, &dmModCur, sizeof(DEVMODE)) )
		ChangeDisplaySettings(&dmModCur, 0);


	return nMax;
}




HRESULT CLcDevD3D::ChooseInitialD3DSettings()
{
	BOOL bFoundFullscreen = FindBestFullscreenMode( FALSE, FALSE );
	BOOL bFoundWindowed = FindBestWindowedMode( FALSE, FALSE );

	if( m_bStartFull && bFoundFullscreen )
		m_d3dSettings.IsWindowed = FALSE;
	if( !bFoundWindowed && bFoundFullscreen )
		m_d3dSettings.IsWindowed = FALSE;

	if( !bFoundFullscreen && !bFoundWindowed )
		return D3DAPPERR_NOCOMPATIBLEDEVICES;

	return S_OK;
}



// Name: HandlePossibleSizeChange()
// Reset the device if the client area size has changed.
HRESULT CLcDevD3D::HandlePossibleSizeChange()
{
	HRESULT hr = S_OK;
	RECT rcClientOld;
	rcClientOld = m_rcWindowClient;

	if( m_bIgnoreSizeChange )
		return S_OK;

	// Update window properties
	GetWindowRect( m_hWnd, &m_rcWindowBounds );
	GetClientRect( m_hWnd, &m_rcWindowClient );

	if( rcClientOld.right - rcClientOld.left !=
		m_rcWindowClient.right - m_rcWindowClient.left ||
		rcClientOld.bottom - rcClientOld.top !=
		m_rcWindowClient.bottom - m_rcWindowClient.top)
	{
		// A new window size will require a new backbuffer
		// size, so the 3D structures must be changed accordingly.
		Pause( TRUE );

		m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
		m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

		if( m_pDev != NULL )
		{
			// Reset the 3D environment
			if( FAILED( hr = Reset3DEnvironment() ) )
			{
				if( hr != D3DERR_OUTOFVIDEOMEMORY )
					hr = D3DAPPERR_RESETFAILED;
				DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
			}
		}
		Pause( FALSE );
	}
	return hr;
}



// Name: Initialize3DEnvironment()
//
HRESULT CLcDevD3D::Initialize3DEnvironment()
{
	HRESULT hr;

	CD3DEnum::D3DAdapterInfo* pAdapterInfo = m_d3dSettings.PAdapterInfo();
	CD3DEnum::D3DDeviceInfo* pDeviceInfo = m_d3dSettings.PDeviceInfo();

	m_bWindowed = m_d3dSettings.IsWindowed;

	// Prepare window for possible windowed/fullscreen change
	AdjustWindowForChange();

	// Set up the presentation parameters
	BuildPresentParamsFromSettings();

	if( pDeviceInfo->Caps.PrimitiveMiscCaps & D3DPMISCCAPS_NULLREFERENCE )
	{
		// Warn user about null ref device that can't render anything
		DisplayErrorMsg( D3DAPPERR_NULLREFDEVICE, 0 );
	}

	DWORD behaviorFlags;

	if (m_d3dSettings.GetVertexProcessingType() == CD3DEnum::SOFTWARE_VP)
		behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	else if (m_d3dSettings.GetVertexProcessingType() == CD3DEnum::MIXED_VP)
		behaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;

	else if (m_d3dSettings.GetVertexProcessingType() == CD3DEnum::HARDWARE_VP)
		behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	else if (m_d3dSettings.GetVertexProcessingType() == CD3DEnum::PURE_HARDWARE_VP)
		behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;

	else
		behaviorFlags = 0; // TODO: throw exception


	behaviorFlags |= (D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED);

	// Create the device
//	m_d3dpp.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	hr = m_pD3D->CreateDevice( m_d3dSettings.AdapterOrdinal(), pDeviceInfo->DevType,
		m_hWndFocus, behaviorFlags, &m_d3dpp,
		&m_pDev );

	if( SUCCEEDED(hr) )
	{
		if(m_bDragFullUse)
		{
			if(FAILED(m_pDev->SetDialogBoxMode(TRUE)))
				return -1;
		}

		// When moving from fullscreen to windowed mode, it is important to
		// adjust the window size after recreating the device rather than
		// beforehand to ensure that you get the window size you want.  For
		// example, when switching from 640x480 fullscreen to windowed with
		// a 1000x600 window on a 1024x768 desktop, it is impossible to set
		// the window size to 1000x600 until after the display mode has
		// changed to 1024x768, because windows cannot be larger than the
		// desktop.
		if( m_bWindowed )
		{
			SetWindowPos( m_hWnd, HWND_NOTOPMOST,
				m_rcWindowBounds.left, m_rcWindowBounds.top,
				( m_rcWindowBounds.right - m_rcWindowBounds.left ),
				( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
				SWP_SHOWWINDOW );
		}

		// Store device Caps
		m_pDev->GetDeviceCaps( &m_d3dCaps );
		m_dwCreateFlags = behaviorFlags;

		// Store device description
		if( pDeviceInfo->DevType == D3DDEVTYPE_REF )
			lstrcpy( m_sStatsDevice, TEXT("REF") );
		else if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
			lstrcpy( m_sStatsDevice, TEXT("HAL") );
		else if( pDeviceInfo->DevType == D3DDEVTYPE_SW )
			lstrcpy( m_sStatsDevice, TEXT("SW") );

		if( behaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
			behaviorFlags & D3DCREATE_PUREDEVICE )
		{
			if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
				lstrcat( m_sStatsDevice, TEXT(" (pure hw vp)") );
			else
				lstrcat( m_sStatsDevice, TEXT(" (simulated pure hw vp)") );
		}
		else if( behaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		{
			if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
				lstrcat( m_sStatsDevice, TEXT(" (hw vp)") );
			else
				lstrcat( m_sStatsDevice, TEXT(" (simulated hw vp)") );
		}
		else if( behaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING )
		{
			if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
				lstrcat( m_sStatsDevice, TEXT(" (mixed vp)") );
			else
				lstrcat( m_sStatsDevice, TEXT(" (simulated mixed vp)") );
		}
		else if( behaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING )
		{
			lstrcat( m_sStatsDevice, TEXT(" (sw vp)") );
		}

		if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
		{
			// Be sure not to overflow m_sStatsDevice when appending the adapter
			// description, since it can be long.  Note that the adapter description
			// is initially CHAR and must be converted to TCHAR.
			lstrcat( m_sStatsDevice, TEXT(": ") );
			const int cchDesc = sizeof(pAdapterInfo->AdapterIdentifier.Description);
			TCHAR szDescription[cchDesc];
			
			DXUtil_ConvertAnsiStringToGenericCch( szDescription,
				pAdapterInfo->AdapterIdentifier.Description, cchDesc );

			int maxAppend = sizeof(m_sStatsDevice) / sizeof(TCHAR) - lstrlen( m_sStatsDevice ) - 1;
			
			_tcsncat( m_sStatsDevice, szDescription, maxAppend );
		}

		// Store render target surface desc
		m_pDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBcBuf );
		m_pBcBuf->GetDesc( &m_d3dBcDsc );

		// Set up the fullscreen cursor
		if( m_bShowCursorWhenFullscreen && !m_bWindowed )
			m_pDev->ShowCursor( TRUE );

		// Confine cursor to fullscreen window
		if( m_bClipCursorWhenFullscreen )
		{
			if (!m_bWindowed )
			{
				RECT rcWindow;
				GetWindowRect( m_hWnd, &rcWindow );
				ClipCursor( &rcWindow );
			}
			else
			{
				ClipCursor( NULL );
			}
		}



		if(m_d3dEnum.AppMinStencilBits>0)
			m_dClrMod = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER| D3DCLEAR_STENCIL;

		else
			m_dClrMod = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER;



		//(E) 2008-01-09 Editor: AFEW
		D3DFrameStatsToString();

		// Initialize the app's device-dependent objects
		hr = Init();

		if( FAILED(hr) )
		{
			SAFE_RELEASE(	m_pBcBuf		);
			SAFE_RELEASE(	m_pDev	);
			return -1;
		}
		else
		{
			m_bDeviceInited = TRUE;
			hr = Restore();

			if( FAILED(hr) )
			{
				Invalidate();
			}
			else
			{
				m_bDeviceRestored = TRUE;
				return S_OK;
			}
		}

		// Cleanup before we try again
		Cleanup3DEnvironment();
}

// If that failed, fall back to the reference rasterizer
if( hr != D3DAPPERR_MEDIANOTFOUND &&
   hr != HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) &&
   pDeviceInfo->DevType == D3DDEVTYPE_HAL )
{
	if (FindBestWindowedMode(FALSE, TRUE))
	{
		m_bWindowed = TRUE;
		AdjustWindowForChange();
		// Make sure main window isn't topmost, so error message is visible
		SetWindowPos( m_hWnd, HWND_NOTOPMOST,
			m_rcWindowBounds.left, m_rcWindowBounds.top,
			( m_rcWindowBounds.right - m_rcWindowBounds.left ),
			( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
			SWP_SHOWWINDOW );

		// Let the user know we are switching from HAL to the reference rasterizer
		DisplayErrorMsg( hr, MSGWARN_SWITCHEDTOREF );

		hr = Initialize3DEnvironment();
	}
}
return hr;
}










// Name: BuildPresentParamsFromSettings()
//
void CLcDevD3D::BuildPresentParamsFromSettings()
{
	m_d3dpp.Windowed               = m_d3dSettings.IsWindowed;
	m_d3dpp.BackBufferCount        = 1;
	m_d3dpp.MultiSampleType        = m_d3dSettings.MultisampleType();
	m_d3dpp.MultiSampleQuality     = m_d3dSettings.MultisampleQuality();
	m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.EnableAutoDepthStencil = m_d3dEnum.AppUsesDepthBuffer;
	m_d3dpp.hDeviceWindow          = m_hWnd;

	if( m_d3dEnum.AppUsesDepthBuffer )
	{
		m_d3dpp.Flags              = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		m_d3dpp.AutoDepthStencilFormat = m_d3dSettings.DepthStencilBufferFormat();
	}
	else
	{
		m_d3dpp.Flags              = 0;
	}

	if( m_bWindowed )
	{
		m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
		m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;
		m_d3dpp.BackBufferFormat = m_d3dSettings.PDeviceCombo()->BackBufferFormat;
		m_d3dpp.FullScreen_RefreshRateInHz = 0;
		m_d3dpp.PresentationInterval = m_d3dSettings.PresentInterval();
	}
	else
	{
		m_d3dpp.BackBufferWidth  = m_d3dSettings.DisplayMode().Width;
		m_d3dpp.BackBufferHeight = m_d3dSettings.DisplayMode().Height;
		m_d3dpp.BackBufferFormat = m_d3dSettings.PDeviceCombo()->BackBufferFormat;
		m_d3dpp.FullScreen_RefreshRateInHz = m_d3dSettings.Fullscreen_DisplayMode.RefreshRate;
		m_d3dpp.PresentationInterval = m_d3dSettings.PresentInterval();
	}

}




HRESULT CLcDevD3D::Reset3DEnvironment()
{
	HRESULT hr;

	// Release all vidmem objects
	if( m_bDeviceRestored )
	{
		m_bDeviceRestored = FALSE;
		Invalidate();

		SAFE_RELEASE(m_pBcBuf);
	}

//	m_d3dpp.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	
	// Reset the device
	if( FAILED( hr = m_pDev->Reset( &m_d3dpp ) ) )
		return hr;

	if(m_bDragFullUse)
	{
		if(FAILED(m_pDev->SetDialogBoxMode(TRUE)))
			return -1;
	}

	if(m_d3dEnum.AppMinStencilBits>0)
		m_dClrMod = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER| D3DCLEAR_STENCIL;

	else
		m_dClrMod = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER;



	// Store render target surface desc
	m_pDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBcBuf );
	m_pBcBuf->GetDesc( &m_d3dBcDsc );

	// Set up the fullscreen cursor
	if( m_bShowCursorWhenFullscreen && !m_bWindowed )
		m_pDev->ShowCursor( TRUE );


	// Confine cursor to fullscreen window
	if( m_bClipCursorWhenFullscreen )
	{
		if (!m_bWindowed )
		{
			RECT rcWindow;
			GetWindowRect( m_hWnd, &rcWindow );
			ClipCursor( &rcWindow );
		}
		else
		{
			ClipCursor( NULL );
		}
	}


	//(E) 2008-01-09 Editor: AFEW
	D3DFrameStatsToString();
	
	// Initialize the app's device-dependent objects
	hr = Restore();

	if( FAILED(hr) )
	{
		Invalidate();
		return hr;
	}
	m_bDeviceRestored = TRUE;

	// If the app is paused, trigger the rendering of the current frame
	if( FALSE == m_bFrameMoving )
	{
		m_bSingleStep = TRUE;
		DXUtil_Timer( TIMER_START );
		DXUtil_Timer( TIMER_STOP );
	}

	return S_OK;
}




// Called when user toggles between fullscreen mode and windowed mode
HRESULT CLcDevD3D::ToggleFullscreen()
{
	HRESULT hr;
	int AdapterOrdinalOld = m_d3dSettings.AdapterOrdinal();
	D3DDEVTYPE DevTypeOld = m_d3dSettings.DevType();

	Pause( TRUE );
	m_bIgnoreSizeChange = TRUE;

	// Toggle the windowed state
	m_bWindowed = !m_bWindowed;
	m_d3dSettings.IsWindowed = m_bWindowed;

	// Prepare window for windowed/fullscreen change
	AdjustWindowForChange();

	// If AdapterOrdinal and DevType are the same, we can just do a Reset().
	// If they've changed, we need to do a complete device teardown/rebuild.
	if (m_d3dSettings.AdapterOrdinal() == AdapterOrdinalOld &&
		m_d3dSettings.DevType() == DevTypeOld)
	{
		// Reset the 3D device
		BuildPresentParamsFromSettings();
		hr = Reset3DEnvironment();
	}
	else
	{
		Cleanup3DEnvironment();
		hr = Initialize3DEnvironment();
	}
	if( FAILED( hr ) )
	{
		if( hr != D3DERR_OUTOFVIDEOMEMORY )
			hr = D3DAPPERR_RESETFAILED;
		m_bIgnoreSizeChange = FALSE;
		if( !m_bWindowed )
		{
			// Restore window type to windowed mode
			m_bWindowed = !m_bWindowed;
			m_d3dSettings.IsWindowed = m_bWindowed;
			AdjustWindowForChange();
			SetWindowPos( m_hWnd, HWND_NOTOPMOST,
				m_rcWindowBounds.left, m_rcWindowBounds.top,
				( m_rcWindowBounds.right - m_rcWindowBounds.left ),
				( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
				SWP_SHOWWINDOW );
		}
		return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
	}

	m_bIgnoreSizeChange = FALSE;

	// When moving from fullscreen to windowed mode, it is important to
	// adjust the window size after resetting the device rather than
	// beforehand to ensure that you get the window size you want.  For
	// example, when switching from 640x480 fullscreen to windowed with
	// a 1000x600 window on a 1024x768 desktop, it is impossible to set
	// the window size to 1000x600 until after the display mode has
	// changed to 1024x768, because windows cannot be larger than the
	// desktop.
	if( m_bWindowed )
	{
		SetWindowPos( m_hWnd, HWND_NOTOPMOST,
			m_rcWindowBounds.left, m_rcWindowBounds.top,
			( m_rcWindowBounds.right - m_rcWindowBounds.left ),
			( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
			SWP_SHOWWINDOW );
	}

	Pause( FALSE );
	return S_OK;
}



HRESULT CLcDevD3D::ForceWindowed()
{
	HRESULT hr;

	if( m_bWindowed )
		return S_OK;

	if( !FindBestWindowedMode(FALSE, FALSE) )
	{
		return E_FAIL;
	}
	m_bWindowed = TRUE;

	// Now destroy the current 3D device objects, then reinitialize

	Pause( TRUE );

	// Release all scene objects that will be re-created for the new device
	Cleanup3DEnvironment();

	// Create the new device
	if( FAILED(hr = Initialize3DEnvironment() ) )
		return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );

	Pause( FALSE );
	return S_OK;
}



HRESULT CLcDevD3D::AdjustWindowForChange()
{
	if( m_bWindowed )
	{
		// Set windowed-mode style
		SetWindowLong( m_hWnd, GWL_STYLE, m_dStyle );
		if( m_hMenu != NULL )
		{
			SetMenu( m_hWnd, m_hMenu );
			m_hMenu = NULL;
		}
	}
	else
	{
		// Set fullscreen-mode style
		SetWindowLong( m_hWnd, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE );
		if( m_hMenu == NULL )
		{
			m_hMenu = GetMenu( m_hWnd );
			SetMenu( m_hWnd, NULL );
		}
	}
	return S_OK;
}



HRESULT CLcDevD3D::UserSelectNewDevice()
{
	return S_OK;
}






// Name: Run()
//
INT CLcDevD3D::Run()
{
	// Load keyboard accelerators
	HACCEL hAccel = LoadAccelerators( NULL, MAKEINTRESOURCE( m_dAccl ) );

	// Now we're ready to recieve and process Windows messages.
	BOOL bGotMsg;
	MSG  msg;
	msg.message = WM_NULL;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	while( WM_QUIT != msg.message  )
	{
		// Use PeekMessage() if the app is active, so we can use idle time to
		// render the scene. Else, use GetMessage() to avoid eating CPU time.
		if( m_bActive )
			bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );
		else
			bGotMsg = ( GetMessage( &msg, NULL, 0U, 0U ) != 0 );

		if( bGotMsg )
		{
			// Translate and dispatch the message

//			if( hAccel == NULL || m_hWnd == NULL || 0 == TranslateAccelerator( m_hWnd, hAccel, &msg ) )
//			{
//				TranslateMessage( &msg );
//				DispatchMessage( &msg );
//
//				if( ((msg.lParam >> 16) & 0xff) == 0xf1)						// 한자Key 막음.
//				{
//					continue;
//				}
//			}

			if(m_hWnd && hAccel)
				TranslateAccelerator( m_hWnd, hAccel, &msg );

			TranslateMessage( &msg );
			DispatchMessage( &msg );

			if( ((msg.lParam >> 16) & 0xff) == 0xf1)						// 한자Key 막음.
				continue;
		}
		else
		{
			if( m_bDeviceLost )
			{
				// Yield some CPU time to other processes
				Sleep( 100 ); // 100 milliseconds
			}
			// Render a frame during idle time (no messages are waiting)
			if( m_bActive )
			{
				if( FAILED( Render3DEnvironment() ) )
					SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
			}
		}
	}
	if( hAccel != NULL )
		DestroyAcceleratorTable( hAccel );

	return (INT)msg.wParam;
}







// Name: Render3DEnvironment()
// Draws the scene.
HRESULT CLcDevD3D::Render3DEnvironment()
{
	HRESULT hr;

	if( m_bDeviceLost )
	{
		// Test the cooperative level to see if it's okay to render
		if( FAILED( hr = m_pDev->TestCooperativeLevel() ) )
		{
			// If the device was lost, do not render until we get it back
			if( D3DERR_DEVICELOST == hr )
				return S_OK;

			// Check if the device needs to be reset.
			if( D3DERR_DEVICENOTRESET == hr )
			{
				// If we are windowed, read the desktop mode and use the same format for
				// the back buffer
				if( m_bWindowed )
				{
					CD3DEnum::D3DAdapterInfo* pAdapterInfo = m_d3dSettings.PAdapterInfo();
					m_pD3D->GetAdapterDisplayMode( pAdapterInfo->AdapterOrdinal, &m_d3dSettings.Windowed_DisplayMode );
					m_d3dpp.BackBufferFormat = m_d3dSettings.Windowed_DisplayMode.Format;
				}

				if( FAILED( hr = Reset3DEnvironment() ) )
					return hr;
			}

			return hr;
		}

		m_bDeviceLost = FALSE;
	}

	// Get the app's time, in seconds. Skip rendering if no time elapsed
	DOUBLE fAppTime        = DXUtil_Timer( TIMER_GETAPPTIME );
	DOUBLE fElapsedAppTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );
	if( ( 0.0 == fElapsedAppTime ) && m_bFrameMoving )
		return S_OK;

	// FrameMove (animate) the scene
	if( m_bFrameMoving || m_bSingleStep )
	{
		// Store the time for the app
		m_fTime        = fAppTime;
		m_fElapsedTime = fElapsedAppTime;

		// Frame move the scene
		if( FAILED( hr = FrameMove() ) )
			return hr;

		m_bSingleStep = FALSE;
	}

	// Render the scene as normal
	if(!m_bLoadingRnd)
	{
		if( FAILED( hr = Render() ) )
			return hr;
	}

	UpdateStats();

	// Show the frame on the primary surface.
	if(!m_bLoadingRnd)
	{
		hr = m_pDev->Present( NULL, NULL, NULL, NULL );

		if( D3DERR_DEVICELOST == hr )
			m_bDeviceLost = TRUE;
	}

	return S_OK;
}




void CLcDevD3D::UpdateStats()
{
	static DWORD	dFrames=0;
	static DOUBLE	dB	= TimeGetTime();
	DOUBLE			dE	= TimeGetTime();

	++dFrames;

	if(dFrames>=32)
	{
		m_dbFPS = dE-dB;

		m_fTimeAv = m_dbFPS/dFrames;
		m_dbFPS	= (dFrames*1000.)/m_dbFPS;

		if(m_fTimeAv>5000)
			m_fTimeAv = 5000;

		dFrames = 0;
		dB = dE;
	}

//	// Keep track of the frame count
//	static DOUBLE dB = 0.;
//	static DWORD dFrames= 0;
//
//	static DOUBLE fLastTimeNoPause= DXUtil_Timer( TIMER_GETABSOLUTETIME );
//	m_fTimeNoPause = DXUtil_Timer( TIMER_GETABSOLUTETIME ) - fLastTimeNoPause;
//
//	DOUBLE dE = DXUtil_Timer( TIMER_GETABSOLUTETIME );
//	++dFrames;
//
//	if( dFrames>=8)
//	{
//		m_dbFPS		= dE - dB;
//		m_fTimeAv	= (m_dbFPS*1000.)/dFrames;
//		m_dbFPS		= (dFrames)/(m_dbFPS);
//
//		if(m_fTimeAv>5000)
//			m_fTimeAv = 5000;
//
//		dB = dE;
//		dFrames  = 0;
//	}
}




// Cleanup scene objects
void CLcDevD3D::Cleanup3DEnvironment()
{
	HRESULT	hr=0;

	SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, &m_bDragFullWin, 0);

	
	if( m_pDev != NULL )
	{
		if( m_bDeviceRestored )
		{
			m_bDeviceRestored = FALSE;
			Invalidate();
		}
		if( m_bDeviceInited )
		{
			m_bDeviceInited = FALSE;
			Destroy();
		}

		if(m_pBcBuf)
		{
			hr = m_pBcBuf->Release();
			m_pBcBuf = 0;
		}

		hr = m_pDev->Release();

//		if(  hr> 1 )
//			DisplayErrorMsg( D3DAPPERR_NONZEROREFCOUNT, MSGERR_APPMUSTEXIT );

		m_pDev = NULL;
	}
}



HRESULT CLcDevD3D::DisplayErrorMsg( HRESULT hr, DWORD dwType )
{
	static BOOL s_bFatalErrorReported = FALSE;
	TCHAR strMsg[512];

	// If a fatal error message has already been reported, the app
	// is already shutting down, so don't show more error messages.
	if( s_bFatalErrorReported )
		return hr;

	switch( hr )
	{
	case D3DAPPERR_NODIRECT3D:
		_tcscpy( strMsg, _T("Could not initialize Direct3D. You may\n")
			_T("want to check that the latest version of\n")
			_T("DirectX is correctly installed on your\n")
			_T("system.  Also make sure that this program\n")
			_T("was compiled with header files that match\n")
			_T("the installed DirectX DLLs.") );
		break;

	case D3DAPPERR_NOCOMPATIBLEDEVICES:
		_tcscpy( strMsg, _T("Could not find any compatible Direct3D\n")
			_T("devices.") );
		break;

	case D3DAPPERR_NOWINDOWABLEDEVICES:
		_tcscpy( strMsg, _T("This sample cannot run in a desktop\n")
			_T("window with the current display settings.\n")
			_T("Please change your desktop settings to a\n")
			_T("16- or 32-bit display mode and re-run this\n")
			_T("sample.") );
		break;

	case D3DAPPERR_NOHARDWAREDEVICE:
		_tcscpy( strMsg, _T("No hardware-accelerated Direct3D devices\n")
			_T("were found.") );
		break;

	case D3DAPPERR_HALNOTCOMPATIBLE:
		_tcscpy( strMsg, _T("This sample requires functionality that is\n")
			_T("not available on your Direct3D hardware\n")
			_T("accelerator.") );
		break;

	case D3DAPPERR_NOWINDOWEDHAL:
		_tcscpy( strMsg, _T("Your Direct3D hardware accelerator cannot\n")
			_T("render into a window.\n")
			_T("Press F2 while the app is running to see a\n")
			_T("list of available devices and modes.") );
		break;

	case D3DAPPERR_NODESKTOPHAL:
		_tcscpy( strMsg, _T("Your Direct3D hardware accelerator cannot\n")
			_T("render into a window with the current\n")
			_T("desktop display settings.\n")
			_T("Press F2 while the app is running to see a\n")
			_T("list of available devices and modes.") );
		break;

	case D3DAPPERR_NOHALTHISMODE:
		_tcscpy( strMsg, _T("This sample requires functionality that is\n")
			_T("not available on your Direct3D hardware\n")
			_T("accelerator with the current desktop display\n")
			_T("settings.\n")
			_T("Press F2 while the app is running to see a\n")
			_T("list of available devices and modes.") );
		break;

	case D3DAPPERR_MEDIANOTFOUND:
	case HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ):
		_tcscpy( strMsg, _T("Could not load required media." ) );
		break;

	case D3DAPPERR_RESETFAILED:
		_tcscpy( strMsg, _T("Could not reset the Direct3D device." ) );
		break;

	case D3DAPPERR_NONZEROREFCOUNT:
		_tcscpy( strMsg, _T("A D3D object has a non-zero reference\n")
			_T("count (meaning things were not properly\n")
			_T("cleaned up).") );
		break;

	case D3DAPPERR_NULLREFDEVICE:
		_tcscpy( strMsg, _T("Warning: Nothing will be rendered.\n")
			_T("The reference rendering device was selected, but your\n")
			_T("computer only has a reduced-functionality reference device\n")
			_T("installed.  Install the DirectX SDK to get the full\n")
			_T("reference device.\n") );
		break;

	case E_OUTOFMEMORY:
		_tcscpy( strMsg, _T("Not enough memory.") );
		break;

	case D3DERR_OUTOFVIDEOMEMORY:
		_tcscpy( strMsg, _T("Not enough video memory.") );
		break;

	default:
		_tcscpy( strMsg, _T("Generic application error. Enable\n")
			_T("debug output for detailed information.") );
	}

	if( MSGERR_APPMUSTEXIT == dwType )
	{
		s_bFatalErrorReported = TRUE;
		_tcscat( strMsg, _T("\n\nThis sample will now exit.") );
		MessageBox( NULL, strMsg, m_sCls, MB_ICONERROR|MB_OK );

		// Close the window, which shuts down the app
		if( m_hWnd )
			SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
	}
	else
	{
		if( MSGWARN_SWITCHEDTOREF == dwType )
			_tcscat( strMsg, _T("\n\nSwitching to the reference rasterizer,\n")
			_T("a software device that implements the entire\n")
			_T("Direct3D feature set, but runs very slowly.") );
		MessageBox( NULL, strMsg, m_sCls, MB_ICONWARNING|MB_OK );
	}

	return hr;
}




BOOL CLcDevD3D::ConfirmDeviceHelper( D3DCAPS9* pCaps, INT vertexProcessingType, void* pDeviceCombo)
{
	DWORD dwBehavior;

	if (vertexProcessingType == CD3DEnum::SOFTWARE_VP)
		dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	else if (vertexProcessingType == CD3DEnum::MIXED_VP)
		dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;

	else if (vertexProcessingType == CD3DEnum::HARDWARE_VP)
		dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	else if (vertexProcessingType == CD3DEnum::PURE_HARDWARE_VP)
		dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;

	else
		dwBehavior = 0; // TODO: throw exception

	
	D3DFORMAT FmtBackBuffer	= ((CD3DEnum::D3DDeviceCombo*)pDeviceCombo)->BackBufferFormat;
	D3DFORMAT FmtAdapter	= ((CD3DEnum::D3DDeviceCombo*)pDeviceCombo)->AdapterFormat;

	return SUCCEEDED( g_pD3DApp->ConfirmDevice( pCaps, dwBehavior, FmtAdapter, FmtBackBuffer ) );
}






HRESULT CLcDevD3D::ConfirmDevice(D3DCAPS9* pCaps,DWORD dwBehavior, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
	// GetTransform doesn't work on PUREDEVICE
	if( dwBehavior & D3DCREATE_PUREDEVICE )
		return E_FAIL;

	//	Dot Product3
	if(!(pCaps->TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3))
		return E_FAIL;

	//	able to do bumpmapping
	if( !( pCaps->TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP ) )
		return E_FAIL;


	// Multi Texture Stage
	if(pCaps->MaxTextureBlendStages < 3)
		return E_FAIL;
	
	
	// HardWare가속 중에 쉐이더 지원이 되는 지...
	if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
		(dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
	{
		// 버텍스 쉐이더 버전은 최소 1.1
		if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
			return E_FAIL;

		// 픽셀 쉬이더 버전은 1.4
		if( pCaps->PixelShaderVersion < D3DVS_VERSION(1,4) )
			return E_FAIL;
	}
	

	return S_OK;
}





HRESULT CLcDevD3D::OneTimeSceneInit()
{
	SendMessage( m_hWnd, WM_PAINT, 0, 0 );
	m_bLoadingApp = FALSE;

	return S_OK;
}

HRESULT CLcDevD3D::FinalCleanup()
{
	int iSize = (INT)m_vScFile.size();

	for(int i=0; i<iSize; ++i)
	{
		delete [] m_vScFile[i];
	}

	m_vScFile.clear();

	// 콘솔 창 해제
	if(m_ConsoleH)
	{
		::fclose( m_ConsoleF);
		::CloseHandle(m_ConsoleH);
		::FreeConsole();

		m_ConsoleF = NULL;
		m_ConsoleH = NULL;
	}

	return S_OK;
}


// Returns the string for the given D3DFORMAT.
TCHAR* CLcDevD3D::D3DFormatToString( D3DFORMAT format, BOOL bWithPrefix )
{
    TCHAR* pstr = NULL;
    switch( format )
    {
    case D3DFMT_UNKNOWN:         pstr = TEXT("D3DFMT_UNKNOWN"); break;
    case D3DFMT_R8G8B8:          pstr = TEXT("D3DFMT_R8G8B8"); break;
    case D3DFMT_A8R8G8B8:        pstr = TEXT("D3DFMT_A8R8G8B8"); break;
    case D3DFMT_X8R8G8B8:        pstr = TEXT("D3DFMT_X8R8G8B8"); break;
    case D3DFMT_R5G6B5:          pstr = TEXT("D3DFMT_R5G6B5"); break;
    case D3DFMT_X1R5G5B5:        pstr = TEXT("D3DFMT_X1R5G5B5"); break;
    case D3DFMT_A1R5G5B5:        pstr = TEXT("D3DFMT_A1R5G5B5"); break;
    case D3DFMT_A4R4G4B4:        pstr = TEXT("D3DFMT_A4R4G4B4"); break;
    case D3DFMT_R3G3B2:          pstr = TEXT("D3DFMT_R3G3B2"); break;
    case D3DFMT_A8:              pstr = TEXT("D3DFMT_A8"); break;
    case D3DFMT_A8R3G3B2:        pstr = TEXT("D3DFMT_A8R3G3B2"); break;
    case D3DFMT_X4R4G4B4:        pstr = TEXT("D3DFMT_X4R4G4B4"); break;
    case D3DFMT_A2B10G10R10:     pstr = TEXT("D3DFMT_A2B10G10R10"); break;
    case D3DFMT_A8B8G8R8:        pstr = TEXT("D3DFMT_A8B8G8R8"); break;
    case D3DFMT_X8B8G8R8:        pstr = TEXT("D3DFMT_X8B8G8R8"); break;
    case D3DFMT_G16R16:          pstr = TEXT("D3DFMT_G16R16"); break;
    case D3DFMT_A2R10G10B10:     pstr = TEXT("D3DFMT_A2R10G10B10"); break;
    case D3DFMT_A16B16G16R16:    pstr = TEXT("D3DFMT_A16B16G16R16"); break;
    case D3DFMT_A8P8:            pstr = TEXT("D3DFMT_A8P8"); break;
    case D3DFMT_P8:              pstr = TEXT("D3DFMT_P8"); break;
    case D3DFMT_L8:              pstr = TEXT("D3DFMT_L8"); break;
    case D3DFMT_A8L8:            pstr = TEXT("D3DFMT_A8L8"); break;
    case D3DFMT_A4L4:            pstr = TEXT("D3DFMT_A4L4"); break;
    case D3DFMT_V8U8:            pstr = TEXT("D3DFMT_V8U8"); break;
    case D3DFMT_L6V5U5:          pstr = TEXT("D3DFMT_L6V5U5"); break;
    case D3DFMT_X8L8V8U8:        pstr = TEXT("D3DFMT_X8L8V8U8"); break;
    case D3DFMT_Q8W8V8U8:        pstr = TEXT("D3DFMT_Q8W8V8U8"); break;
    case D3DFMT_V16U16:          pstr = TEXT("D3DFMT_V16U16"); break;
    case D3DFMT_A2W10V10U10:     pstr = TEXT("D3DFMT_A2W10V10U10"); break;
    case D3DFMT_UYVY:            pstr = TEXT("D3DFMT_UYVY"); break;
    case D3DFMT_YUY2:            pstr = TEXT("D3DFMT_YUY2"); break;
    case D3DFMT_DXT1:            pstr = TEXT("D3DFMT_DXT1"); break;
    case D3DFMT_DXT2:            pstr = TEXT("D3DFMT_DXT2"); break;
    case D3DFMT_DXT3:            pstr = TEXT("D3DFMT_DXT3"); break;
    case D3DFMT_DXT4:            pstr = TEXT("D3DFMT_DXT4"); break;
    case D3DFMT_DXT5:            pstr = TEXT("D3DFMT_DXT5"); break;
    case D3DFMT_D16_LOCKABLE:    pstr = TEXT("D3DFMT_D16_LOCKABLE"); break;
    case D3DFMT_D32:             pstr = TEXT("D3DFMT_D32"); break;
    case D3DFMT_D15S1:           pstr = TEXT("D3DFMT_D15S1"); break;
    case D3DFMT_D24S8:           pstr = TEXT("D3DFMT_D24S8"); break;
    case D3DFMT_D24X8:           pstr = TEXT("D3DFMT_D24X8"); break;
    case D3DFMT_D24X4S4:         pstr = TEXT("D3DFMT_D24X4S4"); break;
    case D3DFMT_D16:             pstr = TEXT("D3DFMT_D16"); break;
    case D3DFMT_L16:             pstr = TEXT("D3DFMT_L16"); break;
    case D3DFMT_VERTEXDATA:      pstr = TEXT("D3DFMT_VERTEXDATA"); break;
    case D3DFMT_INDEX16:         pstr = TEXT("D3DFMT_INDEX16"); break;
    case D3DFMT_INDEX32:         pstr = TEXT("D3DFMT_INDEX32"); break;
    case D3DFMT_Q16W16V16U16:    pstr = TEXT("D3DFMT_Q16W16V16U16"); break;
    case D3DFMT_MULTI2_ARGB8:    pstr = TEXT("D3DFMT_MULTI2_ARGB8"); break;
    case D3DFMT_R16F:            pstr = TEXT("D3DFMT_R16F"); break;
    case D3DFMT_G16R16F:         pstr = TEXT("D3DFMT_G16R16F"); break;
    case D3DFMT_A16B16G16R16F:   pstr = TEXT("D3DFMT_A16B16G16R16F"); break;
    case D3DFMT_R32F:            pstr = TEXT("D3DFMT_R32F"); break;
    case D3DFMT_G32R32F:         pstr = TEXT("D3DFMT_G32R32F"); break;
    case D3DFMT_A32B32G32R32F:   pstr = TEXT("D3DFMT_A32B32G32R32F"); break;
    case D3DFMT_CxV8U8:          pstr = TEXT("D3DFMT_CxV8U8"); break;
    default:                     pstr = TEXT("Unknown format"); break;
    }
    if( bWithPrefix || _tcsstr( pstr, TEXT("D3DFMT_") )== NULL )
        return pstr;
    else
        return pstr + lstrlen( TEXT("D3DFMT_") );
}





void CLcDevD3D::D3DFrameStatsToString()
{
	TCHAR strFmt[100]={0};
	TCHAR strDepthFmt[100]={0};


	D3DFORMAT fmtAdapter = m_d3dSettings.DisplayMode().Format;

	if( fmtAdapter == m_d3dBcDsc.Format )
	{
		lstrcpyn( strFmt, D3DFormatToString( fmtAdapter, FALSE ), 100 );
	}
	else
	{
		_sntprintf( strFmt, 100, TEXT("backbuf %s, adapter %s"),
			D3DFormatToString( m_d3dBcDsc.Format, FALSE ),
			D3DFormatToString( fmtAdapter, FALSE ) );
	}

	if( m_d3dEnum.AppUsesDepthBuffer )
	{
		_sntprintf( strDepthFmt, 100, TEXT(" (%s)"), D3DFormatToString( m_d3dSettings.DepthStencilBufferFormat(), FALSE ) );
	}


	TCHAR* pstrMultiSample;

	switch( m_d3dSettings.MultisampleType() )
	{
		case D3DMULTISAMPLE_NONMASKABLE:  pstrMultiSample = TEXT(" (Nonmaskable Multisample)"); break;
		case D3DMULTISAMPLE_2_SAMPLES:  pstrMultiSample = TEXT(" (2x Multisample)"); break;
		case D3DMULTISAMPLE_3_SAMPLES:  pstrMultiSample = TEXT(" (3x Multisample)"); break;
		case D3DMULTISAMPLE_4_SAMPLES:  pstrMultiSample = TEXT(" (4x Multisample)"); break;
		case D3DMULTISAMPLE_5_SAMPLES:  pstrMultiSample = TEXT(" (5x Multisample)"); break;
		case D3DMULTISAMPLE_6_SAMPLES:  pstrMultiSample = TEXT(" (6x Multisample)"); break;
		case D3DMULTISAMPLE_7_SAMPLES:  pstrMultiSample = TEXT(" (7x Multisample)"); break;
		case D3DMULTISAMPLE_8_SAMPLES:  pstrMultiSample = TEXT(" (8x Multisample)"); break;
		case D3DMULTISAMPLE_9_SAMPLES:  pstrMultiSample = TEXT(" (9x Multisample)"); break;
		case D3DMULTISAMPLE_10_SAMPLES: pstrMultiSample = TEXT(" (10x Multisample)"); break;
		case D3DMULTISAMPLE_11_SAMPLES: pstrMultiSample = TEXT(" (11x Multisample)"); break;
		case D3DMULTISAMPLE_12_SAMPLES: pstrMultiSample = TEXT(" (12x Multisample)"); break;
		case D3DMULTISAMPLE_13_SAMPLES: pstrMultiSample = TEXT(" (13x Multisample)"); break;
		case D3DMULTISAMPLE_14_SAMPLES: pstrMultiSample = TEXT(" (14x Multisample)"); break;
		case D3DMULTISAMPLE_15_SAMPLES: pstrMultiSample = TEXT(" (15x Multisample)"); break;
		case D3DMULTISAMPLE_16_SAMPLES: pstrMultiSample = TEXT(" (16x Multisample)"); break;
		default:                        pstrMultiSample = TEXT(""); break;
	}

	const int cchMaxFrameStats = sizeof(m_sStatsFrame) / sizeof(TCHAR);

	_sntprintf( m_sStatsFrame, cchMaxFrameStats
		, _T("(%dx%d), %s%s%s")
		, m_d3dBcDsc.Width, m_d3dBcDsc.Height
		, strFmt, strDepthFmt, pstrMultiSample);
	m_sStatsFrame[cchMaxFrameStats - 1] = TEXT('\0');
}





// Called in to toggle the pause state of the app.
void CLcDevD3D::Pause( BOOL bPause )
{
	static DWORD dwAppPausedCount = 0;

	dwAppPausedCount		+=( bPause ? +1 : -1 );
	m_bActive	= ( dwAppPausedCount ? FALSE : TRUE );

	// Handle the first pause request (of many, nestable pause requests)
	if( bPause && ( 1 == dwAppPausedCount ) )
	{
		// Stop the scene from animating
		if( m_bFrameMoving )
			DXUtil_Timer( TIMER_STOP );
	}

	if( 0 == dwAppPausedCount )
	{
		// Restart the timers
		if( m_bFrameMoving )
			DXUtil_Timer( TIMER_START );
	}
}



LRESULT WINAPI CLcDevD3D::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pD3DApp->MsgProc(hWnd, uMsg, wParam, lParam);
}


LRESULT CLcDevD3D::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(m_pFncMsgProc)
	{
		if(m_pFncMsgProc(hWnd, uMsg, wParam, lParam))
			return 0;
	}


	WPARAM	wLo = LOWORD(wParam);

	switch( uMsg )
	{
		case WM_PAINT:
		// Handle paint messages when the app is paused
		if( m_pDev && !m_bActive && m_bWindowed && m_bDeviceInited && m_bDeviceRestored )
		{
			if(!m_bLoadingRnd)
			{
				Render();
			}
			

			if(!m_bLoadingRnd)
				m_pDev->Present( NULL, NULL, NULL, NULL );
		}
		break;

		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
			break;

		case WM_ENTERSIZEMOVE:
			// Halt frame movement while the app is sizing or moving
			Pause( TRUE );
			break;

		case WM_SIZE:
			// Pick up possible changes to window style due to maximize, etc.
			if( m_bWindowed && m_hWnd != NULL )
				m_dStyle = GetWindowLong( m_hWnd, GWL_STYLE );

			if( SIZE_MINIMIZED == wParam )
			{
				if( m_bClipCursorWhenFullscreen && !m_bWindowed )
					ClipCursor( NULL );

				Pause( TRUE ); // Pause while we're minimized
				m_bMinimized = TRUE;
				m_bMaximized = FALSE;
			}
			else if( SIZE_MAXIMIZED == wParam )
			{
				if( m_bMinimized )
					Pause( FALSE ); // Unpause since we're no longer minimized

				m_bMinimized = FALSE;
				m_bMaximized = TRUE;
				HandlePossibleSizeChange();
			}
			else if( SIZE_RESTORED == wParam )
			{
				if( m_bMaximized )
				{
					m_bMaximized = FALSE;
					HandlePossibleSizeChange();
				}
				else if( m_bMinimized)
				{
					Pause( FALSE ); // Unpause since we're no longer minimized
					m_bMinimized = FALSE;
					HandlePossibleSizeChange();
				}
			}
			break;

		case WM_EXITSIZEMOVE:
		{
			Pause( FALSE );
			HandlePossibleSizeChange();
			break;
		}

		case WM_SETCURSOR:
		{
			if( m_bActive && !m_bWindowed )
			{
//				SetCursor( NULL );
				
				if( m_bShowCursorWhenFullscreen )
					m_pDev->ShowCursor( TRUE );

				return TRUE;
			}
		
			break;
		}

		case WM_MOUSEMOVE:
		{
			if( m_bActive && m_pDev)
			{
				POINT ptCursor;
				GetCursorPos( &ptCursor );

				if( !m_bWindowed )
					ScreenToClient( m_hWnd, &ptCursor );
				
				m_pDev->SetCursorPosition( ptCursor.x, ptCursor.y, 0 );
			}
		
			break;
		}

		case WM_ENTERMENULOOP:
		{
			// Pause the app when menus are displayed
			Pause(TRUE);
			break;
		}

		case WM_EXITMENULOOP:
		{
			Pause(FALSE);
			break;
		}

		case WM_NCHITTEST:
		{
			// Prevent the user from selecting the menu in fullscreen mode
			if( !m_bWindowed )
				return HTCLIENT;

			break;
		}

		case WM_POWERBROADCAST:
		{
			switch( wParam )
			{
	#ifndef PBT_APMQUERYSUSPEND
	#define PBT_APMQUERYSUSPEND 0x0000
	#endif
			case PBT_APMQUERYSUSPEND:
				// At this point, the app should save any data for open
				// network connections, files, etc., and prepare to go into
				// a suspended mode.
				return TRUE;

	#ifndef PBT_APMRESUMESUSPEND
	#define PBT_APMRESUMESUSPEND 0x0007
	#endif
			case PBT_APMRESUMESUSPEND:
				// At this point, the app should recover any data, network
				// connections, files, etc., and resume running from when
				// the app was suspended.
				return TRUE;
			}
		
			break;
		}

		case WM_SYSCOMMAND:
		{
			// Prevent moving/sizing and power loss in fullscreen mode
			if(wLo == SC_CLOSE)
			{
				Sleep(100);

				if(0==m_bLoadingRnd)
					break;

				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
		
			break;
		}


		case WM_COMMAND:
		{
			if(wLo == m_dTggl)
			{
				if(0==m_bLoadingRnd)
				{
					// Toggle the fullscreen/window mode
					Pause( TRUE );

					if( FAILED( ToggleFullscreen() ) )
						DisplayErrorMsg( D3DAPPERR_RESETFAILED, MSGERR_APPMUSTEXIT );
					
					Pause( FALSE );
				}
			}
			
			else if(wLo == m_dExit)
			{
				// Recieved key/menu command to exit app
				SendMessage( hWnd, WM_CLOSE, 0, 0 );
				return 0;
			}

			break;
		}


		case WM_CLOSE:
		case WM_DESTROY:
		{
			Cleanup3DEnvironment();
			SAFE_RELEASE( m_pD3D );
			FinalCleanup();


			if(m_hWnd)
			{
//				HMENU hMenu= GetMenu(hWnd);
//				
//				if( hMenu != NULL )
//					DestroyMenu( hMenu );
//				
//				DestroyWindow( hWnd );
				PostQuitMessage(0);
				m_hWnd = NULL;
			}

			return 0;
		}

	}// switch( uMsg )


	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}






INT LcDev_CreateWindowInstance(char* sCmd
					, ILC_Device** pData
					, void* p1
					, void* p2
					, void* p3
					, void* p4
					)
{
	(*pData) = NULL;


	InitCommonControls();


	if(0==_stricmp("Direct 3D PC", sCmd))
	{
		CLcDevD3D* pObj = NULL;

		pObj = new CLcDevD3D;

		pObj->SetMainObject(p1);
		(*pData) = pObj;
		
		return 0;
	}
	

	return -1;
}




INT LcDev_CreateDevice(char* sCmd
					, ILC_Device** pData
					, void* p1
					, void* p2
					, void* p3
					, void* p4
					)
{
	if(NULL == (*pData))
		return -1;

	if(0==_stricmp("Direct 3D PC", sCmd))
	{
		CLcDevD3D* pObj = (CLcDevD3D*)(*pData);

		if(FAILED(pObj->Create(p1, p2, p3, p4)))
			return -1;

		return 0;
	}

	return -1;
}


