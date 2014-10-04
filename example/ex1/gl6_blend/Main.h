// Interface for the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif


#ifndef _CMain_H_
#define _CMain_H_


#define MAX_STAR_NUM 90


class CMain : public CAppGame
{
public:
	struct Tpaticle
	{
		float r, g, b;			// color
		float dist;				// Distance from center
		float angle;			// Angle
	};

protected:
	unsigned int	m_TexID;
	Tpaticle		m_pStar[MAX_STAR_NUM];

public:
	CMain();

	virtual int		Init();
	virtual int		Destroy();
	virtual int		FrameMove();
	virtual int		Render();
	
protected:
	void	SetPerspective();
};

extern	CMain*	g_pApp;
#define	GMAIN	g_pApp

#endif


