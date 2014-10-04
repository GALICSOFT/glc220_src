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
}


INT	CMain::Init()
{
	// Input 持失
	if(LC_FAILED(LcxEnt_CreateInput(NULL, &m_pInput, m_hWnd)))
		return LC_EFAIL;


	// Camera 持失
	if(LC_FAILED(LcxEnt_CreateCamera(LCXCMD_CAMERA_3D, &m_pCam)))
		return LC_EFAIL;



	LCXVECTOR3	vcEye (30, -100, 30);
	LCXVECTOR3	vcLook(30,   0,  0);
	LCXVECTOR3	vcUp  ( 0,   0,  1);
	m_pCam->SetupParamView( &vcEye, &vcLook, &vcUp);

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

	return LC_OK;
}

INT	CMain::Destroy()
{
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
	printf("FPS: %3.f\n", m_fFPS);



	// Input Update
	m_pInput->Update();

	const BYTE*	pKey = m_pInput->GetKeyMap();
	const BYTE*	pMts = m_pInput->GetMtsMap();

	if(pKey[LCIK_ESCAPE])
		return LC_EFAIL;


	// Move, Rotation Speed
	FLOAT speed = (FLOAT)m_fElapsed;
	speed *=.1f;


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

	if(pKey[LCIK_LEFT])
		m_pCam->RotateYaw(fAngle_degree, speed);

	if(pKey[LCIK_RIGHT])
		m_pCam->RotateYaw(fAngle_degree, -speed);

	if(pKey[LCIK_UP])
		m_pCam->RotatePitch(fAngle_degree, speed);

	if(pKey[LCIK_DOWN])
		m_pCam->RotatePitch(fAngle_degree, -speed);


	m_pCam->Update();

	return LC_OK;
}


INT	CMain::Render()
{
	m_pDev->BeginScene();

	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);

	float mtViw[16];
	float mtPrj[16];

	memcpy((void*)mtViw, (void*)m_pCam->GetMatrixViw(), sizeof(float)*16 );
	memcpy((void*)mtPrj, (void*)m_pCam->GetMatrixPrj(), sizeof(float)*16 );

	m_pCam->SetTransformPrj();
	m_pCam->SetTransformViw();


	m_pDev->SetRenderState(LC_RS_CULL_FACE, LC_ST_ENABLE);

	m_pXYZ->Render();
	m_pGrid->Render();


	m_pDev->EndScene();

	return LC_OK;
}



