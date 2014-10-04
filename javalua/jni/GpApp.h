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


#define MAX_PHASE	260


class CGpApp : public ILC_AppFrame
{
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
	PLC_SPRITE		m_pSpt		;		// 2D Sprite
	PLCX_INPUT		m_pInput	;		// Input
	DOUBLE			m_TimeFPS	;		// Frame Per Second
	DOUBLE			m_TimeElps	;		// Ellapsed time
	PLC_FONT		m_pFont		;		// Test Font

	INT				m_ScFileN	;		// Script Number
	char			m_ScFile[MAX_PHASE][LC_MAX_PATH];

	INT				m_PhaseC	;		// Current Phase
	INT				m_PhaseO	;		// Old Phase
	PLC_APPFRAME	m_Phase		;		// Game Phase Class

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
	DOUBLE		GetElapsedTime(){	return m_TimeElps;		}
	PLC_DEVICE	GetDevice()		{	return m_pDev;			}
	INT			Invalid();
	INT			Restore();
	INT			ChangePhase(INT NewPhase);
	INT			InitScript(char* slua=NULL);

protected:
	void	UpdateStatus();
};


#endif

