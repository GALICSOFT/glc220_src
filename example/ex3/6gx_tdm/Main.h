// Interface for the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif


#ifndef _CMain_H_
#define _CMain_H_


typedef		std::map<std::string, PLC_TEXTURE >	LsTex;
typedef		LsTex::iterator							ItTex;


class CMain : public CAppGame
{
protected:
	PLC_SPRITE_L	m_pLyr		;		// sprite Layer
	PLCX_CAMERA		m_pCam2D	;		// 2D camera
	PLCX_INPUT		m_pInput	;		// Input

	PLCX_ENTITY		m_pXYZ		;		// X, Y, Z Axis Line

	PLCX_TDM		m_pTdm[16]	;		// Original 2D Model
	LsTex			m_vTex		;

public:
	CMain();

	virtual INT	Init();
	virtual INT	Destroy();
	virtual INT	FrameMove();
	virtual INT	Render();

protected:
	PLC_TEXTURE	FindTex(char* sTex);
	PLCX_TDM		Setup2dModel(char* sModel);
};

extern	CMain*	g_pApp;
#define	GMAIN	g_pApp

#endif


