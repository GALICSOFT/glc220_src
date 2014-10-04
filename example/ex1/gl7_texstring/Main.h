// Interface for the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif


#ifndef _CMain_H_
#define _CMain_H_


class CMain : public CAppGame
{
protected:
	CTexString*		m_pStr1;
	CTexString*		m_pStr2;
	CTexString*		m_pStr3;

public:
	CMain();

	virtual int		Init();
	virtual int		Destroy();
	virtual int		FrameMove();
	virtual int		Render();
};

extern	CMain*	g_pApp;
#define	GMAIN	g_pApp

#endif


