// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"


#define MEDIA_DIR	"../../media/"


CMain::CMain()
{
	m_pInput	= NULL;
	m_pCam3D	= NULL;
	m_pCam2D	= NULL;
	m_pXYZ		= NULL;
	m_pGrid		= NULL;

	m_pTdm		= NULL;
}


INT	CMain::Init()
{
	INT	hr = LC_OK;


	// Create Input
	if(LC_FAILED(LcxEnt_CreateInput(NULL, &m_pInput, m_hWnd)))
		return LC_EFAIL;


	// Create 3D Camera
	if(LC_FAILED(LcxEnt_CreateCamera(LCXCMD_CAMERA_3D, &m_pCam3D)))
		return LC_EFAIL;

	LCXVECTOR3	vcEye ( 0, -100, 0);
	LCXVECTOR3	vcLook( 0,   0,  0);
	LCXVECTOR3	vcUp  ( 0,   0,  1);
	m_pCam3D->SetupParamView(&vcEye, &vcLook, &vcUp);

	FLOAT	fFov = LCXToRadian(45.0F);
	FLOAT	fScnW= FLOAT(m_nScnW);
	FLOAT	fScnH= FLOAT(m_nScnH);
	FLOAT	fNear= 1.0F;
	FLOAT	fFar = 5000.0F;
	m_pCam3D->SetupParamProj(fFov, fScnW, fScnH, fNear, fFar);





	// Create 2D Camera
	if(LC_FAILED(LcxEnt_CreateCamera(LCXCMD_CAMERA_2D, &m_pCam2D)))
		return LC_EFAIL;


	LCXVECTOR3	vcEye2 ( 0,  0,  -1);
	LCXVECTOR3	vcLook2( 0,  0,  0);
	LCXVECTOR3	vcUp2  ( 0,  1,  0);
	m_pCam2D->SetupParamView(&vcEye2, &vcLook2, &vcUp2);




	if(LC_FAILED(LcxEnt_CreateEntity(LCXCMD_ENTITY_AXIS, &m_pXYZ)))
		return LC_EFAIL;

	if(LC_FAILED(LcxEnt_CreateEntity(LCXCMD_ENTITY_GRID, &m_pGrid)))
		return LC_EFAIL;



	m_pTdm = Setup2dModel((char*)(MEDIA_DIR"model2d/boss01.tdt"));
	if(NULL == m_pTdm)
		return LC_EFAIL;



	LCXVECTOR2	vcT(0.0F, 0.F);
	hr = m_pTdm->SetAnimation(0,"attack");
	hr = m_pTdm->SetPosition(&vcT);


	return LC_OK;
}



INT	CMain::Destroy()
{
	ItTex _F = m_vTex.begin();
	ItTex _L = m_vTex.end();

	for(; _F != _L; ++_F )
	{
		PLC_TEXTURE pTex = (*_F).second;

		if(pTex)
			pTex->Release();
	}

	m_vTex.clear();

	m_pTdm->Release();


	SAFE_RELEASE(	m_pXYZ		);
	SAFE_RELEASE(	m_pGrid		);
	SAFE_RELEASE(	m_pCam2D	);
	SAFE_RELEASE(	m_pCam3D	);
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
	speed *=0.1f;


	// Translation
	if(pKey[LCIK_DOWN])
		m_pCam2D->MoveForward(+speed);

	if(pKey[LCIK_UP])
		m_pCam2D->MoveForward(-speed);

	if(pKey[LCIK_W])
		m_pCam2D->MoveUpward(speed);

	if(pKey[LCIK_S])
		m_pCam2D->MoveUpward(-speed);

	if(pKey[LCIK_A])
		m_pCam2D->MoveSideward(-speed);

	if(pKey[LCIK_D])
		m_pCam2D->MoveSideward(+speed);


	// Rotation
	float fAngle_degree = .5f;

	if(pKey[LCIK_LEFT])
		m_pCam2D->RotateYaw(fAngle_degree, speed);

	if(pKey[LCIK_RIGHT])
		m_pCam2D->RotateYaw(fAngle_degree, -speed);


	m_pCam2D->Update();
	m_pCam3D->Update();


	DOUBLE fElapsedTime = this->GetElapsedTime();
	DOUBLE fTime = fElapsedTime;

	m_pTdm->SetAttrib(LCXM_ADVANCE_TIME, &fTime);
	m_pTdm->FrameMove();


	return LC_OK;
}


INT	CMain::Render()
{
	m_pDev->BeginScene();

	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);

	m_pCam3D->SetTransformPrj();
	m_pCam3D->SetTransformViw();


	m_pDev->SetRenderState(LC_RS_CULL_FACE, LC_ST_DISABLE);
	m_pDev->SetRenderState(LC_RS_LIGHTING, LC_ST_DISABLE);


	m_pXYZ->Render();
	m_pGrid->Render();



	// 2D Model Rendering
	{
		m_pSpt->Begin();

		LC_MATRIX* pmtViw = (LC_MATRIX*)m_pCam2D->GetMatrixViw();
		LC_MATRIX* pmtPrj = (LC_MATRIX*)m_pCam2D->GetMatrixPrj();

		m_pSpt->SetTransform(LC_TS_VIEW_EXT, pmtViw);

//		m_pCam2D->SetTransformPrj();
//		m_pCam2D->SetTransformViw();
//
//		m_pCam2D->SetTransformPrj();
//		m_pCam2D->SetTransformViw();

		LCXVECTOR2	vcPos(0, 0);
		m_pTdm->SetPosition(&vcPos);
		m_pTdm->Render();

		m_pSpt->End();
	}



	m_pDev->EndScene();

	return LC_OK;
}



PLC_TEXTURE CMain::FindTex(char* sName)
{
	PLC_TEXTURE	pTex = NULL;

	ItTex it = m_vTex.find( sName );

	if( it == m_vTex.end() )
	{
		char sFile	[LC_MAX_PATH ]={0};
		char drive	[LC_MAX_DRIVE]={0};
		char dir	[LC_MAX_DIR  ]={0};
		char fname	[LC_MAX_FNAME]={0};
		char ext	[LC_MAX_EXT  ]={0};

		LcStr_SplitPath( sName, drive, dir, fname, ext );

		sprintf(sFile, MEDIA_DIR"model2d/%s%s", fname, ext);

		if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &pTex, sFile)))
		{
			printf("Cannot Read Model Texture: %s\n", sFile);
			return NULL;
		}

		if(pTex)
			m_vTex.insert(ItTex::value_type(sName, pTex) );
	}

	else
	{
		pTex = (*it).second;
	}


	return pTex;
}



PLCX_TDM CMain::Setup2dModel(char* sModel)
{
	INT				hr;
	INT				nTexList = 0;
	INT				i = 0;

	char**			s = NULL;
	PLC_TEXTURE*	t = NULL;
	PLCX_TDM		pTdm = NULL;

	hr = LcxEnt_CreateTdm(NULL, &pTdm, (void*)sModel);

	if(LC_FAILED(hr))
		return NULL;


	nTexList = 0;
	pTdm->GetAttrib(LCXM_TEXTURE_LIST_NUMBER, &nTexList);


	s = (char**)malloc( (nTexList+1) * sizeof(char*) );
	t = (PLC_TEXTURE*)malloc((nTexList+1) * sizeof(PLC_TEXTURE*) );
	memset(s, 0, (nTexList+1) * sizeof(char*));
	memset(t, 0, (nTexList+1) * sizeof(PLC_TEXTURE*));


	pTdm->GetAttrib(LCXM_TEXTURE_LIST_NAME, &s);
	for(i=0; i<nTexList; ++i)
	{
		//1. Find Texture
		char*	sName = s[i];
		PLC_TEXTURE pTex = this->FindTex(sName);

		if(NULL == pTex)
			return NULL;

		t[i] = pTex;
	}

	pTdm->SetAttrib(LCXM_TEXTURE_LIST_POINTER, t);

	free(s);
	free(t);

	return pTdm;
}


