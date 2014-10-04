// Implementation of the CMain class.
//
// VBO와 관련된 함수
//
//	- glBindBuffer
//	- glBufferData
//	- glBufferSubData
//	- glDeleteBuffers
//	- glGenBuffers
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
	// Input 생성
	if(LC_FAILED(LcxEnt_CreateInput(NULL, &m_pInput, m_hWnd)))
		return LC_EFAIL;


	// Camera 생성
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




	////////////////////////////////////////////////////////////////////////////
	UINT i=0;


	// 시스템 메모리에 정점 데이터 생성
	float vertices[] =
	{
		-10,-10,-10,  +10,-10,-10, +10,+10,-10, -10,+10,-10,
		-10,-10,+10,  +10,-10,+10, +10,+10,+10, -10,+10,+10
	};


	for(i=0; i< sizeof(vertices)/sizeof(vertices[0]); ++i)
		vertices[i] = vertices[i];


	UINT	dFVF = LC_FVF_XYZ;
	INT		nSize = sizeof(vertices);
	if(LC_FAILED(LcDev_CreateVB(NULL, &m_pVB, &dFVF, vertices, &nSize)))
		return LC_EFAIL;




	dFVF = LC_FMT_INDEX16;
	nSize = sizeof(WORD) * 4;

	for(i=0;i<6; ++i)
	{
		if(LC_FAILED(LcDev_CreateIB(NULL, &m_pIB[i], &dFVF)))
			return LC_EFAIL;
	}

	// Index 데이터
	WORD front [] = {2,1,3,0};	// front face
	WORD back  [] = {5,6,4,7};	// back face
	WORD top   [] = {6,2,7,3};	// top face
	WORD bottom[] = {1,5,0,4};	// bottom face
	WORD left  [] = {3,0,7,4};	// left face
	WORD right [] = {6,5,2,1};	// right face

	m_pIB[0]->Fill(front , nSize);
	m_pIB[1]->Fill(back  , nSize);
	m_pIB[2]->Fill(top   , nSize);
	m_pIB[3]->Fill(bottom, nSize);
	m_pIB[4]->Fill(left  , nSize);
	m_pIB[5]->Fill(right , nSize);

	return LC_OK;
}


INT	CMain::Destroy()
{
	INT i=0;

	m_pVB->Release();

	for(i=0; i<6; ++i)
	{
		if(m_pIB[i])
			m_pIB[i]->Release();
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
	FLOAT speed = (FLOAT)m_fElapsed;
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



	return LC_OK;
}


INT	CMain::Render()
{
	m_pDev->BeginScene();


	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);

	m_pCam->SetTransformPrj();
	m_pCam->SetTransformViw();



	m_pDev->SetRenderState(LC_RS_CULL_FACE, LC_ST_DISABLE);


	m_pXYZ->Render();
	m_pGrid->Render();



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

	m_pDev->SetStreamSource(m_pVB);
	m_pDev->SetFVF(LC_FVF_XYZ);

	m_pDev->SetColor(1,0,0,1);		m_pDev->SetIndices(m_pIB[0]);	m_pDev->DrawIndexedPrimitive(LC_PT_TRIANGLE_STRIP, 4);
	m_pDev->SetColor(0,1,0,1);		m_pDev->SetIndices(m_pIB[1]);	m_pDev->DrawIndexedPrimitive(LC_PT_TRIANGLE_STRIP, 4);
	m_pDev->SetColor(0,0,1,1);		m_pDev->SetIndices(m_pIB[2]);	m_pDev->DrawIndexedPrimitive(LC_PT_TRIANGLE_STRIP, 4);
	m_pDev->SetColor(1,1,0,1);		m_pDev->SetIndices(m_pIB[3]);	m_pDev->DrawIndexedPrimitive(LC_PT_TRIANGLE_STRIP, 4);
	m_pDev->SetColor(0,1,1,1);		m_pDev->SetIndices(m_pIB[4]);	m_pDev->DrawIndexedPrimitive(LC_PT_TRIANGLE_STRIP, 4);
	m_pDev->SetColor(1,0,1,1);		m_pDev->SetIndices(m_pIB[5]);	m_pDev->DrawIndexedPrimitive(LC_PT_TRIANGLE_STRIP, 4);

	m_pDev->SetFVF(0);
	m_pDev->SetColor(1,1,1,1);

	m_pDev->SetStreamSource(NULL);
	m_pDev->SetIndices(NULL);

	m_pDev->SetTransform(LC_TS_WORLD_IDENTITY, NULL);


	m_pDev->EndScene();

	return LC_OK;
}




