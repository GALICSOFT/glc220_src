// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"


#define MEDIA_DIR	"../../media/"


CMain::CMain()
{
	m_pInput	= NULL;
	m_pCam		= NULL;
	m_pXYZ		= NULL;
	m_pGrid		= NULL;

	m_pTex0		= NULL;
	m_pTex1		= NULL;
}


INT	CMain::Init()
{
	INT	hr = 0;

	// Input 持失
	if(LC_FAILED(LcxEnt_CreateInput(NULL, &m_pInput, m_hWnd)))
		return LC_EFAIL;


	// Camera 持失
	if(LC_FAILED(LcxEnt_CreateCamera(LCXCMD_CAMERA_3D, &m_pCam)))
		return LC_EFAIL;


	LCXVECTOR3	vcEye (0,-3.5F,  0);
	LCXVECTOR3	vcLook(0,  0,  0);
	LCXVECTOR3	vcUp  (0,  0,  1);

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



	hr = LcDev_CreateTextureFromFile(NULL, &m_pTex0, (void*)MEDIA_DIR"tex/stones.png");
	if(LC_FAILED(hr))
		return LC_EFAIL;


	hr = LcDev_CreateTextureFromFile(NULL, &m_pTex1, (void*)MEDIA_DIR"tex/env0.png");
	if(LC_FAILED(hr))
		return LC_EFAIL;



	return LC_OK;
}

INT	CMain::Destroy()
{
	m_pTex0->Release();
	m_pTex1->Release();

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
//	FLOAT speed = (FLOAT)this->GetElapsedTime();
//	speed *=40.0F;
//
//
//	// Translation
//	if(pKey[LCIK_W])
//		m_pCam->MoveForward(+speed);
//
//	if(pKey[LCIK_S])
//		m_pCam->MoveForward(-speed);
//
//	if(pKey[LCIK_A])
//		m_pCam->MoveSideward(-speed);
//
//	if(pKey[LCIK_D])
//		m_pCam->MoveSideward(+speed);
//
//
//	// Rotation
//	float fAngle_degree = 2.0f;
//
//	if(m_pInput->KeyPress(LCIK_LEFT))
//		m_pCam->RotateYaw(fAngle_degree, speed);
//
//	if(m_pInput->KeyPress(LCIK_RIGHT))
//		m_pCam->RotateYaw(fAngle_degree, -speed);
//
//	if(m_pInput->KeyPress(LCIK_UP))
//		m_pCam->RotatePitch(fAngle_degree, speed);
//
//	if(m_pInput->KeyPress(LCIK_DOWN))
//		m_pCam->RotatePitch(fAngle_degree, -speed);


	m_pCam->Update();


	return LC_OK;
}




INT	CMain::Render()
{
	m_pDev->BeginScene();


	UINT	dColor = 0xFF006699;
	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, dColor);
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);
//
	m_pCam->SetTransformPrj();
	m_pCam->SetTransformViw();


//	m_pDev->SetRenderState(LC_RS_CULL_FACE, LC_ST_DISABLE);

//	m_pXYZ->Render();
//	m_pGrid->Render();



	// Color
	LCXCOLOR	pColor[3][3];

	pColor[0][0] = LCXCOLOR(1, 0, 0, 1);
	pColor[0][1] = LCXCOLOR(0, 1, 0, 1);
	pColor[0][2] = LCXCOLOR(0, 0, 1, 1);
	pColor[1][0] = LCXCOLOR(0, 1, 0, 1);
	pColor[1][1] = LCXCOLOR(1, 1, 0, 1);
	pColor[1][2] = LCXCOLOR(0, 1, 1, 1);
	pColor[2][0] = LCXCOLOR(1, 1, 1, 1);
	pColor[2][1] = LCXCOLOR(1, 1, 1, 1);
	pColor[2][2] = LCXCOLOR(1, 0, 1, 1);


	INT	nTexOp[3][3][2]=
	{
		{	{ LC_TS_MODULATE, LC_TS_SUBTRACT},	{ LC_TS_DECAL, LC_TS_MODULATE},		{ LC_TS_MODULATE, LC_TS_MODULATE},	},
		{	{ LC_TS_ADD, LC_TS_DECAL},			{ LC_TS_SUBTRACT, LC_TS_MODULATE},	{ LC_TS_ADD_SIGNED, LC_TS_ADD}		},
		{	{ LC_TS_DECAL, LC_TS_DECAL},			{ LC_TS_MODULATE, LC_TS_DECAL},		{ LC_TS_DECAL, LC_TS_ADD},			},
	};

	// Set Vertex Data
	VtxUV1		pVtx[4];

	pVtx[0].t = LCXVECTOR2(0.0F, 0.0F);
	pVtx[1].t = LCXVECTOR2(1.0F, 0.0F);
	pVtx[2].t = LCXVECTOR2(1.0F, 1.0F);
	pVtx[3].t = LCXVECTOR2(0.0F, 1.0F);

	pVtx[0].p = LCXVECTOR3(-1.0F, 0.0F, -1.0F);
	pVtx[1].p = LCXVECTOR3( 1.0F, 0.0F, -1.0F);
	pVtx[2].p = LCXVECTOR3( 1.0F, 0.0F,  1.0F);
	pVtx[3].p = LCXVECTOR3(-1.0F, 0.0F,  1.0F);


	m_pDev->SetTextureAddressS (0, LC_TS_REPEAT);
	m_pDev->SetTextureAddressT (0, LC_TS_REPEAT);
	m_pDev->SetTextureFilterMin(0, LC_TS_LINEAR);
	m_pDev->SetTextureFilterMag(0, LC_TS_LINEAR);

	m_pDev->SetTextureAddressS (1, LC_TS_REPEAT);
	m_pDev->SetTextureAddressT (1, LC_TS_REPEAT);
	m_pDev->SetTextureFilterMin(1, LC_TS_LINEAR);
	m_pDev->SetTextureFilterMag(1, LC_TS_LINEAR);

	m_pDev->SetFVF(VtxUV1::FVF);


	for(int k=0; k<3; ++k)
	{
		for(int i=0; i<3; ++i)
		{
			float	x = (i - 1.5f)*1.333F;
			float	z = (k - 1.5f)*1.0F;


			if(2 == k && 0 == i)
			{
				m_pDev->SetTexture(0, m_pTex0);
				m_pDev->SetTexture(1, NULL);
			}
			else if(2 == k && 1 == i)
			{
				m_pDev->SetTexture(0, m_pTex1);
				m_pDev->SetTexture(1, NULL);
			}
			else
			{
				m_pDev->SetTexture(0, m_pTex0);
				m_pDev->SetTexture(1, m_pTex1);
			}

			pVtx[0].p = LCXVECTOR3(        x, 0.0F, z    );
			pVtx[1].p = LCXVECTOR3( x+1.333F, 0.0F, z    );
			pVtx[2].p = LCXVECTOR3( x+1.333F, 0.0F, z+1.F);
			pVtx[3].p = LCXVECTOR3(        x, 0.0F, z+1.F);

			m_pDev->SetTextureStageOp (0, nTexOp[k][i][0]);
			m_pDev->SetTextureStageOp (0, nTexOp[k][i][1]);

			m_pDev->SetColor((FLOAT*)&pColor[k][i]);
			m_pDev->DrawPrimitiveUP(LC_PT_TRIANGLE_FAN, 0, 4, pVtx, sizeof(VtxUV1) );
		}
	}



	m_pDev->SetTexture(0, NULL);
	m_pDev->SetTexture(1, NULL);

	m_pDev->SetColor(1,1,1,1);



	m_pDev->EndScene();

	return LC_OK;
}





