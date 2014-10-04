// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"

#define MEDIA_DIR	"../../media/"


CMain::CMain()
{
	m_pPtc	= NULL;
	m_pStr	= NULL;
}


int	CMain::Init()
{
	m_pPtc = new CParticle;
	if(LC_FAILED(m_pPtc->Create( (char*)MEDIA_DIR"tex/ball128x128.tga", 120)))
	{
		delete m_pPtc;
		return LC_EFAIL;
	}

	m_pStr = new CTexString;
	if(LC_FAILED(m_pStr->Create( (char*)MEDIA_DIR"tex/font256x128.tga", 12)))
	{
		delete m_pStr;
		return LC_EFAIL;
	}

	return LC_OK;
}


int	CMain::Destroy()
{
	SAFE_DELETE(	m_pPtc	);
	SAFE_DELETE(	m_pStr	);

	return LC_OK;
}


int	CMain::FrameMove()
{
#if defined(_WIN32)
	LcSys_Sleep(10);
#endif


	static int c=0;
	++c;

	//	if(c>20*60*20)
	if(c>8*60*2)
	{
		printf("Game Process Terminated\n");
		return LC_EFAIL;
	}


	static int n = 0;
	++n;

	if(n>5)
	{
		printf("FPS: %3.f\n", m_fFPS);
		n = 0;
	}



	m_pPtc->FrameMove();


	m_pStr->SetString("Point Sprite Example");
	m_pStr->SetPosition(10, 10);
	m_pStr->SetColor(1, 1, 0, 1);

	return LC_OK;
}


int	CMain::Render()
{
	// Setting Rendering pipeline
	glShadeModel(GL_SMOOTH);
	glDisable( GL_DEPTH_TEST);
	glDisable( GL_CULL_FACE );

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Clear Color buffer and Depth Buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	m_pPtc->Render();
	m_pStr->DrawTxt();


	return LC_OK;
}





