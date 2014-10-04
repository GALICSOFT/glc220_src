// Interface for the CGpApp class.
//
//  Created by Heesung Oh on 2011. 01. 19.
//  Copyright GALIC SOFT Inc. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif

#ifndef _GpApp_H_
#define _GpApp_H_


class CGpApp : public ILC_AppFrame
{
// for system data
protected:
	char			m_sCls[LC_MAX_PATH];

	INT				m_nScnW		;		// Screen Width
	INT				m_nScnH		;		// Screen Height
	INT				m_nScnD		;		// Depth Buffer bit
	INT				m_nScnX		;		// Screen Position X
	INT				m_nScnY		;		// Screen Position Y
	INT				m_bShowSt	;		// Screen Position Y
	UINT			m_dClearCol	;		// Clear color

	LC_HWND			m_hWnd		;		// Window Handle
	PLC_DEVICE		m_pDev		;		// LC_DEVICE
	PLC_SPRITE	m_pSpt		;		// 2D Sprite
	PLCX_INPUT		m_pInput	;		// Input
	DOUBLE			m_TimeFPS	;		// Frame Per Second
	DOUBLE			m_TimeElps	;		// Ellapsed time
	PLC_FONT		m_pFont		;		// Test Font

// game data
protected:
	INT				m_score		;		// test score
	PLC_FONT		m_pGameFnt	;		// for game application font
	PLC_TEXTURE		m_pGameTex	;		// for game application texture
	PLCX_SOUND		m_pGameSnd	;		// for game application sound

public:
	CGpApp();
	virtual ~CGpApp(){}

	virtual INT		Query(char*, LC_HANDLE);
	virtual INT		Release()	{ return LC_EFAIL;	}

	virtual INT		Create(PLC_ARG =NULL);
	virtual INT		Cleanup();

	virtual INT		Run(LC_HANDLE  =NULL);
	virtual INT		Proc(LC_HANDLE  =NULL);

	virtual INT		SetAttrib(UINT, LC_HANDLE);
	virtual INT		GetAttrib(UINT, LC_HANDLE);

	virtual INT		Init();
	virtual INT		Destroy();
	virtual INT		FrameMove();
	virtual INT		Render();

public:
	static CGpApp*	m_pApp;
	static INT	   LC_CALLBACK Run3D(LC_HANDLE);					// Main Processing
	static LRESULT LC_WINAPI   WndProc(LC_HWND,UINT,WPARAM,LPARAM);	// Window Procedure

public:
	DOUBLE		GetFPS()		{	return m_TimeFPS;		}
	DOUBLE		GetElapsedTime(){	return m_TimeElps;		}		// per millisecond
	DOUBLE		GetElapsedTimeS(){	return m_TimeElps/1000.;}		// per second
	DOUBLE		GetElapsedTimeD(){	return m_TimeElps/10.;	}		// per deci-second
	PLC_DEVICE	GetDevice()		{	return m_pDev;			}
	INT			Invalid();
	INT			Restore();

protected:
	void	UpdateStatus();
};


#endif
