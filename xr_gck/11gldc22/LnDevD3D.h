// Interface for the CLcDevD3D class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcDevD3D_H_
#define _LcDevD3D_H_

#pragma warning(disable : 4786)
#include <vector>
using namespace std;


class CLcDevD3D : public CLcDev
{
public:
	CLcDevD3D();
	virtual ~CLcDevD3D();
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);

	virtual INT		Init();
	virtual void	Destroy();

	virtual INT		Restore();
	virtual void	Invalidate();

	virtual INT		FrameMove();
	virtual INT		Render();

	virtual	INT		Run();

	virtual INT		Query(char* sCmd, void* pData);


	virtual INT		BeginScene();
	virtual INT		EndScene();
	virtual INT		Clear(DWORD Count,CONST RECT* pRects,DWORD Flags,DWORD Color,float Z,DWORD Stencil);

public:
	LRESULT			MsgProc(HWND,UINT,WPARAM,LPARAM);
	static LRESULT	WINAPI WndProc(HWND,UINT,WPARAM,LPARAM);



protected:
	char		m_sCls[260]	;				// Window Class Name
	char		m_sWin[260]	;				// Window Title
	
	DWORD		m_dStyle	;				// Window Style
	DWORD		m_dIcon		;				// Icon ID
	DWORD		m_dAccl		;				// Accelator ID
	DWORD		m_dTggl		;				// Toggle Key
	DWORD		m_dExit		;				// Exit Key

	INT			m_iScnW		;				// Screen Width
	INT			m_iScnH		;				// Screen Height
	INT			m_iPosX		;				// Screen Position X
	INT			m_iPosY		;				// Screen Position Y

	BOOL		m_bFull		;				// Full or Window
	BOOL		m_bStartFull;				// Is Start Full Mode

	BOOL		m_bRender	;				// Is Rendering
	BOOL		m_bActive	;				// FALSE
	BOOL		m_bCusor	;				// Show Cusor
	BOOL		m_bShowStats;				// Show Stats


	// Variables for timing
	DOUBLE		m_dbFPS			;			// Frame Per Second.
	DOUBLE		m_fTime			;			// Current time in seconds
	DOUBLE		m_fTimeNoPause	;			// Current time. no pause
	DOUBLE		m_fElapsedTime	;			// Time elapsed since last frame
	DOUBLE		m_fTimeAv		;			// Time Average
	DOUBLE		m_fMaxFrame		;			// Boundary Frame for Frame Synchronization.


	DWORD			m_dClrMod	;			// Device Clear Mode.
	DWORD			m_dClrClr	;			// Device Clear Color.

	D3DCAPS9		m_d3dCaps	;			// Capabilitys for the device
	D3DSURFACE_DESC	m_d3dBcDsc	;			// Surface desc of the backbuffer

	TCHAR			m_sStatsDevice[128]	;	// String to hold D3D device stats
	TCHAR			m_sStatsFrame[128]	;	// String to hold frame stats



// Source Code from D3DXSamples CD3DApplication Wizard Code.
protected:
	CD3DEnum	m_d3dEnum			;
	CD3DSettings m_d3dSettings		;

	// Internal variables for the state of the app
	BOOL		m_bWindowed			;
	BOOL		m_bDeviceLost		;
	BOOL		m_bMinimized		;
	BOOL		m_bMaximized		;
	BOOL		m_bIgnoreSizeChange	;
	BOOL		m_bDeviceInited		;
	BOOL		m_bDeviceRestored	;

	// Internal variables used for timing
	BOOL		m_bFrameMoving		;
	BOOL		m_bSingleStep		;

	// 기타..
	BOOL		m_bLoadingApp		;											// TRUE, if the app is loading
	BOOL		m_bLoadingRnd		;											// TRUE, if the Loading Scene Rendering

	BOOL		m_bDragFullUse		;											// FALSE, Child창이 마우스로 끄는 동안 창내용 표시. TRUE이면 속도 저하.
	BOOL		m_bDragFullWin		;											// 이전 설정이 Child창이 마우스로 끄는 동안 창내용 표시인가?
	BOOL		m_bAntialias		;											// Antialias default is TRUE


	// Internal functions to manage and render the 3D scene
	static BOOL	ConfirmDeviceHelper(D3DCAPS9*, INT nType, void* );
	HRESULT		ConfirmDevice(D3DCAPS9*,DWORD,D3DFORMAT,D3DFORMAT);

	void		BuildPresentParamsFromSettings();
	BOOL		FindBestWindowedMode( BOOL, BOOL);
	BOOL		FindBestFullscreenMode( BOOL, BOOL);
	HRESULT		ChooseInitialD3DSettings();
	HRESULT		Initialize3DEnvironment();
	HRESULT		HandlePossibleSizeChange();
	HRESULT		Reset3DEnvironment();
	HRESULT		ToggleFullscreen();
	HRESULT		ForceWindowed();
	HRESULT		UserSelectNewDevice();
	void		Cleanup3DEnvironment();
	HRESULT		Render3DEnvironment();
	HRESULT		AdjustWindowForChange();
	void		UpdateStats();

	HRESULT		DisplayErrorMsg( HRESULT, DWORD);								// Internal error handling function
	INT			MonitorMaxHz(INT nScnW, INT nScnH, INT nBits);					// Max Hz for screen width and Height

	TCHAR*		D3DFormatToString( D3DFORMAT format, BOOL bWithPrefix=TRUE);
	void		D3DFrameStatsToString();


	// Main objects used for creating and rendering the 3D scene
	HWND		m_hWndFocus					;									// The D3D focus window (usually same as m_hWnd)
	HMENU		m_hMenu						;									// App menu bar (stored here when fullscreen)

	DWORD		m_dwCreateFlags				;									// Indicate sw or hw vertex processing
	RECT		m_rcWindowBounds			;									// Saved window bounds for mode switches
	RECT		m_rcWindowClient			;									// Saved client area size for mode switches

	BOOL		m_bShowCursorWhenFullscreen	;									// Whether to show cursor when fullscreen
	BOOL		m_bClipCursorWhenFullscreen	;									// Whether to limit cursor pos when fullscreen

public:
	HRESULT		OneTimeSceneInit();
	void		Pause(BOOL bPause);
	
	HRESULT		FinalCleanup();
	TCHAR*		GetDeviceStates();
	TCHAR*		GetFrameStates();
};


#endif


