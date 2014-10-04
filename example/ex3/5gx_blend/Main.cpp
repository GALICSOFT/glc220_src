// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"


CMain::CMain()
{
	m_pInput	= NULL;
	m_pCam		= NULL;
	m_pXYZ		= NULL;
	m_pGrid		= NULL;

	m_pEft		= NULL;
}


INT	CMain::Init()
{
	// Input 持失
	if(LC_FAILED(LcxEnt_CreateInput(NULL, &m_pInput, m_hWnd)))
		return LC_EFAIL;


	// Camera 持失
	if(LC_FAILED(LcxEnt_CreateCamera(LCXCMD_CAMERA_3D, &m_pCam)))
		return LC_EFAIL;


	LCXVECTOR3	vcEye (0, -130,  0);
	LCXVECTOR3	vcLook(0,    0,  0);
	LCXVECTOR3	vcUp  (0,    0,  1);

	m_pCam->SetupParamView(&vcEye, &vcLook, &vcUp);

	FLOAT	fFov = LCXToRadian(45.0F);
	FLOAT	fScnW= FLOAT(m_nScnW);
	FLOAT	fScnH= FLOAT(m_nScnH);
	FLOAT	fNear= 1.0F;
	FLOAT	fFar = 5000.0F;

	m_pCam->SetupParamProj(fFov, fScnW, fScnH, fNear, fFar);



	if(LC_FAILED(LcxEnt_CreateEntity(LCXCMD_ENTITY_AXIS, &m_pXYZ)))
		return LC_EFAIL;


	if(LC_FAILED(LcxEnt_CreateEntity(LCXCMD_ENTITY_GRID, &m_pGrid)))
		return LC_EFAIL;



	m_pEft = new CParticle;
	if(LC_FAILED(m_pEft->Create(m_pDev)))
	{
		delete m_pEft;
		m_pEft = NULL;

		return LC_EFAIL;
	}

	return LC_OK;
}


INT	CMain::Destroy()
{
	if(m_pEft)
	{
		delete m_pEft;
		m_pEft = NULL;
	}

	SAFE_RELEASE(	m_pXYZ		);
	SAFE_RELEASE(	m_pGrid		);
	SAFE_RELEASE(	m_pCam		);
	SAFE_RELEASE(	m_pInput	);

	return LC_OK;
}

INT	CMain::FrameMove()
{
#if defined(_WIN32)
	LcSys_Sleep(10);
#endif


	static INT c=0;
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


	// Input Update
	m_pInput->Update();

	const BYTE*	pKey = m_pInput->GetKeyMap();

	if(pKey[LCIK_ESCAPE])
		return LC_EFAIL;


	// Move, Rotation Speed
	FLOAT speed = (FLOAT)this->GetElapsedTime();
	speed *=0.1f;


	// Translation
	if(pKey[LCIK_W])
		m_pCam->MoveForward(+speed);

	if(pKey[LCIK_S])
		m_pCam->MoveForward(-speed);

	if(pKey[LCIK_A])
		m_pCam->MoveSideward(-speed);

	if(pKey[LCIK_D])
		m_pCam->MoveSideward(+speed);


	// Rotation
	float fAngle_degree = 2.0f;

	if(m_pInput->KeyPress(LCIK_LEFT))
		m_pCam->RotateYaw(fAngle_degree, speed);

	if(m_pInput->KeyPress(LCIK_RIGHT))
		m_pCam->RotateYaw(fAngle_degree, -speed);

	if(m_pInput->KeyPress(LCIK_UP))
		m_pCam->RotatePitch(fAngle_degree, speed);

	if(m_pInput->KeyPress(LCIK_DOWN))
		m_pCam->RotatePitch(fAngle_degree, -speed);


	m_pCam->Update();



	m_pEft->FrameMove();


	return LC_OK;
}



INT	CMain::Render()
{
	m_pDev->BeginScene();

	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);

	m_pCam->SetTransformPrj();
	m_pCam->SetTransformViw();


//	m_pXYZ->Render();
//	m_pGrid->Render();

	m_pEft->Render();



	m_pDev->EndScene();

	return LC_OK;
}




