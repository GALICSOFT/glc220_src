// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"

#define MEDIA_DIR	"../../media/"


CMain::CMain()
{
	m_pStr1 = NULL;
	m_pStr2 = NULL;
	m_pStr3 = NULL;
}


int	CMain::Init()
{
	int	hr = 0;

	{
		CTexString*	pStr;
		pStr = new CTexString;

		if(LC_FAILED( hr = pStr->Create( (char*)MEDIA_DIR"tex/font256x128.tga", 12)))
		{
			delete pStr;
			return LC_EFAIL;
		}

		m_pStr1 = pStr;
	}

	{
		CTexString*	pStr;
		pStr = new CTexString;

		if(LC_FAILED( hr = pStr->Create( (char*)MEDIA_DIR"tex/font256x128.tga", 14)))
		{
			delete pStr;
			return LC_EFAIL;
		}

		m_pStr2 = pStr;
	}

	{
		CTexString*	pStr;
		pStr = new CTexString;

		if(LC_FAILED( hr = pStr->Create( (char*)MEDIA_DIR"tex/font256x128.tga", 18)))
		{
			delete pStr;
			return LC_EFAIL;
		}

		m_pStr3 = pStr;
	}


	return LC_OK;
}


int	CMain::Destroy()
{
	SAFE_DELETE(	m_pStr1	);
	SAFE_DELETE(	m_pStr2	);
	SAFE_DELETE(	m_pStr3	);

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



	m_pStr1->SetString((char*)"Hello world !!!");
	m_pStr1->SetPosition(10, 50);
	m_pStr1->SetColor(1, 1, 0, 1);

	m_pStr2->SetString((char*)"This is a sample texture string");
	m_pStr2->SetPosition(15, 80);
	m_pStr2->SetColor(1, 0, 1, 1);

	m_pStr3->SetString((char*)"Using font256x128.tga");
	m_pStr3->SetPosition(20, 110);
	m_pStr3->SetColor(0, 1, 1, 1);


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

	m_pStr1->DrawTxt();
	m_pStr2->DrawTxt();
	m_pStr3->DrawTxt();

	return LC_OK;
}





