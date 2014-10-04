// Interface for the CAppGame class.
//
////////////////////////////////////////////////////////////////////////////////


#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif


#ifndef _AppGame_H_
#define _AppGame_H_


class CAppGame
{
protected:
	LC_HWND			m_hWnd		;				// Window Handle
	PLC_DEVICE		m_pDev		;				// LC_DEVICE

	char			m_sCls[128]	;				// Program Class Name

	INT				m_nScnW		;				// Screen Width
	INT				m_nScnH		;				// Screen Height

	DOUBLE			m_fElapsed	;				// Elapsed Time
	DOUBLE			m_fFPS		;				// Frame Per Second

public:
	CAppGame();

	virtual INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	virtual INT		Cleanup();

	virtual INT		Run(LC_HANDLE);
	virtual INT		Proc(LC_HANDLE);

	virtual INT		Init()		{	return 0;	}
	virtual INT		Destroy()	{	return 0;	}
	virtual INT		FrameMove()	{	return 0;	}
	virtual INT		Render()	{	return 0;	}

public:
	static CAppGame*	m_pAppGame;
	static INT	   LC_CALLBACK Run3D(LC_HANDLE);			// Main Processing

protected:
	void	UpdateStatus();

public:
	DOUBLE	GetFPS()			{	return m_fFPS;		}
	DOUBLE	GetElapsedTime()	{	return m_fElapsed;	}
};


#endif

