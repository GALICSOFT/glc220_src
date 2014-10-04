// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"


#define MEDIA_DIR	"./"


CMain::CMain()
{
	m_pInput	= NULL;
	m_pCam		= NULL;
	m_pXYZ		= NULL;
	m_pGrid		= NULL;

	memset(m_pMdl, 0, sizeof(PLCX_MDL) * 16);
}


INT	CMain::Init()
{
	INT	hr = LC_OK;


	// Input 생성
	if(LC_FAILED(LcxEnt_CreateInput(NULL, &m_pInput, m_hWnd)))
		return LC_EFAIL;


	// Camera 생성
	if(LC_FAILED(LcxEnt_CreateCamera(NULL, &m_pCam)))
		return LC_EFAIL;


	LCXVECTOR3	vcEye ( 5, -100, 30);
	LCXVECTOR3	vcLook( 5,   0,  30);
	LCXVECTOR3	vcUp  ( 0,   0,  1);
	m_pCam->SetupParamView(&vcEye, &vcLook, &vcUp);

	FLOAT	fFov = LCXToRadian(45.0F);
	FLOAT	fScnW= FLOAT(m_nScnW);
	FLOAT	fScnH= FLOAT(m_nScnH);
	FLOAT	fNear= 1.0F;
	FLOAT	fFar = 5000.0F;

	m_pCam->SetupParamProj(fFov, fScnW, fScnH, fNear, fFar);




	if(LC_FAILED(LcxEnt_CreateEntity((char*)"Axis", &m_pXYZ)))
		return LC_EFAIL;

	if(LC_FAILED(LcxEnt_CreateEntity((char*)"Grid", &m_pGrid)))
		return LC_EFAIL;



	hr = LcxEnt_CreateMdl(NULL, &m_pMdl[0], (void*)MEDIA_DIR"model/hella_run2.acm");

	if(LC_FAILED(hr))
		return LC_EFAIL;

	// Load Texture
	UINT	ColorKey = 0x0;
	m_pMdl[0]->SetAttrib(LCXM_EXEC_TEXTURE_LOAD, &ColorKey);




	return LC_OK;
}



INT	CMain::Destroy()
{
	for(int i=0; i<16; ++i)
		LCX_DestroyMdl(&m_pMdl[i]);


	m_pXYZ->Release();
	m_pGrid->Release();

	m_pCam->Release();
	m_pInput->Release();

	return LC_OK;
}


INT	CMain::FrameMove()
{
#if defined(_WIN32)
	LcSys_Sleep(10);
#endif


	static INT c=0;
	++c;

	if(c>20*60*120)
	//if(c>8*60*2)
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
	speed *=100.f;


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


	// 애니메이션에 필요한 시간과 행렬 설정
	DOUBLE fElapsedTime = this->GetElapsedTime();
	DOUBLE fTime;

	LCXMATRIX	mtScl;
	LCXMATRIX	mtRotX;
	LCXMATRIX	mtRotZ;
	LCXMATRIX	mtWld;


	mtScl.SetupScaling( 30.F, 30.F, 30.F);



	for(int i=0; i<1; ++i)
	{
		LCXVECTOR3	vcOrg1( i-2.5F, 0, 0);

		vcOrg1 *=15.F;

		mtWld.Identity();
		mtRotZ.SetupRotationZ(LCXToRadian(45));
		mtWld.SetupTranslation( &vcOrg1, LC_FALSE);

		if(2 > i)
			mtWld = mtScl * mtRotX * mtRotZ * mtWld;
		else
			mtWld = mtRotX * mtRotZ * mtWld;

		fTime = fElapsedTime*1.;


		m_pMdl[i]->SetAttrib(LCXM_WORLD_MATRIX, &mtWld);
		m_pMdl[i]->SetAttrib(LCXM_ADVANCE_TIME, &fTime);


	}




	// Update Model Data
	for(i=0; i<16; ++i)
	{
		if(m_pMdl[i])
			m_pMdl[i]->FrameMove();
	}



	return LC_OK;
}


INT	CMain::Render()
{
	m_pDev->BeginScene();

	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);

	m_pCam->SetTransformPrj();
	m_pCam->SetTransformViw();


	m_pDev->SetRenderState(LC_RS_CULL_FACE, LC_ST_DISABLE);
	m_pDev->SetRenderState(LC_RS_LIGHTING, LC_ST_DISABLE);
	m_pDev->SetRenderState(LC_RS_CULL_MODE, LC_ST_CCW);
	m_pDev->SetRenderState(LC_RS_CULL_FACE, LC_ST_BACK);



	m_pXYZ->Render();
	m_pGrid->Render();


	// Rendering Models
	for(int i=0; i<16; ++i)
	{
		if(m_pMdl[i])
			m_pMdl[i]->Render();
	}




	static int c =0;
	++c;

	LCXMATRIX	mtRotX;
	LCXMATRIX	mtRotZ;
	LCXMATRIX	mtWld;
	mtRotZ.SetupRotationZ( LCXToRadian(c*3.5f));
	mtRotX.SetupRotationX( LCXToRadian(c*3.5f));

	mtWld = mtRotZ * mtRotX;

	mtWld._41 = 10;
	mtWld._42 = -10;
	m_pDev->SetTransform(LC_TS_WORLD, &mtWld);

	m_pDev->SetTransformIdentity(LC_TS_WORLD);


	m_pDev->EndScene();

	return LC_OK;
}



