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

	m_pTex1		= NULL;

	m_pEft1		= NULL;
	m_pEft2		= NULL;
	m_pEft3		= NULL;
	m_pEft4		= NULL;
}


struct MyEft2D1 : public TLCX_PRT2d
{
	LCXVECTOR2	vel;		// Velocity
	LCXVECTOR2	acc;		// Accelerator
	LCXCOLOR	clv;		// Color Velocity

	MyEft2D1() : vel(0,0), acc(0,0), clv(0,0,0,0){}

	virtual INT Init(TLCX_PRT2dINF*);
	virtual INT Reset(TLCX_PRT2dINF*);
	virtual INT Update(TLCX_PRT2dINF*);
};


struct MyEft2D2 : public TLCX_PRT2d
{
	LCXVECTOR2	vel;
	LCXVECTOR2	acc;
	LCXCOLOR	clv;

	MyEft2D2() : vel(0,0), acc(0,0), clv(0,0,0,0){}

	virtual INT Init(TLCX_PRT2dINF*);
	virtual INT Reset(TLCX_PRT2dINF*);
	virtual INT Update(TLCX_PRT2dINF*);
};

struct MyEft2D3 : public TLCX_PRT2d
{
	LCXVECTOR2	vel;		// Velocity
	LCXVECTOR2	acc;		// Accelerator
	LCXCOLOR	clv;		// Color Velocity

	MyEft2D3() : vel(0,0), acc(0,0), clv(0,0,0,0){}

	virtual INT Init(TLCX_PRT2dINF*);
	virtual INT Reset(TLCX_PRT2dINF*);
	virtual INT Update(TLCX_PRT2dINF*);
};


INT MyEft2D1::Init(TLCX_PRT2dINF* pInf)
{
	float	width = float( (50 + rand()%100) * 0.1);

	float	Theta = float(-90 + -10 + rand()%21);
	float	Cos   = 0;
	float	Sin   = 0;

	float	Speed  = float( (50 + rand()%200) * 0.025);

	pos   = pInf->IntlPos;
	pos.x+= rand()%20;

	Theta = LCXToRadian(Theta);
	Cos   = LCXMathCos(Theta);
	Sin   = LCXMathSin(Theta);


	float r = pInf->IntlCol.r;
	float g = pInf->IntlCol.g;
	float b = pInf->IntlCol.b;

	if(r >g && r >b)
	{
		col.r = (200 + rand()%56)/255.F	;
		col.g = ( 50 + rand()%50)/255.F	;
		col.b = 0;
	}
	else
	{
		col.b = (200 + rand()%56)/255.F	;
		col.g = ( 50 + rand()%50)/255.F	;
		col.r = 0;
	}

	col.a = (128 + rand()%128)/255.F	;

	vel.x = Speed * Cos;
	vel.y = Speed * Sin;

	acc.y = 0.04F;

	clv.a= -float((100 + rand()%101) * 0.0001);

	mag   = LCXVECTOR2(width, width);
	Life  = 1.0F;

	return 0;
}

INT MyEft2D1::Reset(TLCX_PRT2dINF* pInf)
{
	float	width = float( (50 + rand()%100) * 0.1);

	float	Theta = float(-90 + -10 + rand()%21);
	float	Cos   = 0;
	float	Sin   = 0;

	float	Speed  = float( (50 + rand()%200) * 0.025);

	pos   = pInf->IntlPos;
	pos.x+= rand()%20;

	Theta = LCXToRadian(Theta);
	Cos   = LCXMathCos(Theta);
	Sin   = LCXMathSin(Theta);

	float r = pInf->IntlCol.r;
	float g = pInf->IntlCol.g;
	float b = pInf->IntlCol.b;


	if(r >g && r >b)
	{
		col.r = (200 + rand()%56)/255.F	;
		col.g = ( 50 + rand()%50)/255.F	;
		col.b = 0;
	}
	else
	{
		col.b = (200 + rand()%56)/255.F	;
		col.g = ( 50 + rand()%50)/255.F	;
		col.r = 0;
	}

	col.a = (128 + rand()%128)/255.F	;

	vel.x = Speed * Cos;
	vel.y = Speed * Sin;

	acc.y = 0.04F;

	clv.a= -float((100 + rand()%101) * 0.0001);

	mag   = LCXVECTOR2(width, width);
	Life  = 1.0F;

	return 0;
}


INT MyEft2D1::Update(TLCX_PRT2dINF* pInf)
{
	vel  += acc;
	pos  += vel;
	col  += clv;

	// boundary
	LCXRECT* pRct = (LCXRECT*)&pInf->IntlRect;

	if( LC_OK != pRct->IsInRect((FLOAT*)&pos) || 0.F >= col.a)
	{
		Life  = 0.0F;
		Reset(pInf);
	}

	return 0;
}


INT MyEft2D2::Init(TLCX_PRT2dINF* pInf)
{
	int		PtcN = pInf->nPrtNum;
	float	width = (200 + rand()%(PtcN+1)) * 0.02F;
	float	scnW = pInf->IntlRect.w;

	pos.x = (rand()%(PtcN+1)) * scnW/PtcN;
	pos.y = -300.F + rand()%(301);

	col.r = 1.F	;
	col.g = 1.F	;
	col.b = 1.F	;
	col.a = (128 + rand()%128)/255.F	;

	vel.x = 0;
	vel.y = (100 + rand()%(PtcN+1)) * 0.01F;
	clv  = LCXCOLOR(0,0,0,-0.004F);

	mag   = LCXVECTOR2(width, width);

	return 0;
}


INT MyEft2D2::Reset(TLCX_PRT2dINF* pInf)
{
	pos.y = -300.F + rand()%(301);
	col.a = (128 + rand()%128)/255.F	;

	Life  = 1.0F;

	return 0;
}


INT MyEft2D2::Update(TLCX_PRT2dINF* pInf)
{
	pos  += vel;
	col  += clv;

	// boundary
	if(500 < pos.y || 0.F >= col.a)
	{
		Life  = 0.0F;
		Reset(pInf);
	}

	return 0;
}





INT MyEft2D3::Init(TLCX_PRT2dINF* pInf)
{
	float	width = float( (50 + rand()%100) * 0.05);

	float	Theta = float(-90 + -30 + rand()%61);
	float	Cos   = 0;
	float	Sin   = 0;

	float	Speed  = float( (50 + rand()%200) * 0.05);

	pos   = pInf->IntlPos;
	pos.x+= rand()%20;

	Theta = LCXToRadian(Theta);
	Cos   = LCXMathCos(Theta);
	Sin   = LCXMathSin(Theta);

	col.r = (100 + rand()%56 )/255.F	;
	col.g = (200 + rand()%56 )/255.F	;
	col.b = (200 + rand()%56 )/255.F	;
	col.a = (128 + rand()%128)/255.F	;

	vel.x = Speed * Cos;
	vel.y = Speed * Sin;

	acc.y = 0.1F;

	clv.a= -float((100 + rand()%101) * 0.0001);

	mag   = LCXVECTOR2(width, width);
	Life  = 1.0F;

	return 0;
}

INT MyEft2D3::Reset(TLCX_PRT2dINF* pInf)
{
	float	width = float( (50 + rand()%100) * 0.05);

	float	Theta = float(-90 + -30 + rand()%61);
	float	Cos   = 0;
	float	Sin   = 0;

	float	Speed  = float( (50 + rand()%200) * 0.05);

	pos   = pInf->IntlPos;
	pos.x+= rand()%20;

	Theta = LCXToRadian(Theta);
	Cos   = LCXMathCos(Theta);
	Sin   = LCXMathSin(Theta);

	col.r = (100 + rand()%56 )/255.F	;
	col.g = (200 + rand()%56 )/255.F	;
	col.b = (200 + rand()%56 )/255.F	;
	col.a = (128 + rand()%128)/255.F	;

	vel.x = Speed * Cos;
	vel.y = Speed * Sin;

	acc.y = 0.1F;

	clv.a= -float((100 + rand()%101) * 0.0001);

	mag   = LCXVECTOR2(width, width);
	Life  = 1.0F;

	return 0;
}


INT MyEft2D3::Update(TLCX_PRT2dINF* pInf)
{
	vel  += acc;
	pos  += vel;
	col  += clv;

	// boundary
	LCXRECT* pRct = (LCXRECT*)&pInf->IntlRect;

	if( LC_OK != pRct->IsInRect((FLOAT*)&pos) || 0.F >= col.a)
	{
		Life  = 0.0F;
		Reset(pInf);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////


INT	CMain::Init()
{
	INT	hr = LC_OK;


	// Input 持失
	if(LC_FAILED(LcxEnt_CreateInput(NULL, &m_pInput, m_hWnd)))
		return LC_EFAIL;


	// Camera 持失
	if(LC_FAILED(LcxEnt_CreateCamera(LCXCMD_CAMERA_3D, &m_pCam)))
		return LC_EFAIL;


	LCXVECTOR3	vcEye ( 5,-100, 30);
	LCXVECTOR3	vcLook( 5,   0, 30);
	LCXVECTOR3	vcUp  ( 0,   0,  1);
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



	// create particle image
	LcDev_CreateTextureFromFile(NULL, &m_pTex1, (char*)MEDIA_DIR"tex/ball128x128.tga");

	// create particle effect
	INT ScnW, ScnH;
	LcVar_WindowSize(&ScnW, &ScnH);


	INT	nPartNum = 600;

	TLCX_PRT2dINF inf;

	inf.nPrtNum  = nPartNum;
	inf.bPrtRen  = LCXEFT_REN_QUAD;
	inf.nPrtSize = sizeof(MyEft2D1);
	inf.pPrtList = new MyEft2D1[nPartNum];				// it will be removed automatically when it will be bound with Particle Effect.;
	inf.pTex     = m_pTex1;								// Texture Pointer

	inf.IntlRect = LC_RECTF(0,0, ScnW, ScnH);			// boundary region
	inf.IntlPos  = LCXVECTOR2(100, 420);				// Initial position
	inf.IntlCol	 = LCXCOLOR(1, 0.5F, 0, 1);				// Initial COLOR


	hr = LcxEnt_CreateEffect((char*)LCXCMD_EFT_PARTICLE2D, &m_pEft1, &inf);
	if(LC_FAILED(hr))
		return hr;


	inf.nPrtNum  = nPartNum;
	inf.bPrtRen  = LCXEFT_REN_QUAD;
	inf.nPrtSize = sizeof(MyEft2D2);
	inf.pPrtList = new MyEft2D2[nPartNum];
	inf.pTex     = m_pTex1;

	inf.IntlRect = LC_RECTF(0,0, ScnW, ScnH);

	hr = LcxEnt_CreateEffect((char*)LCXCMD_EFT_PARTICLE2D, &m_pEft2, &inf);
	if(LC_FAILED(hr))
		return hr;



	inf.nPrtNum  = nPartNum;
	inf.bPrtRen  = LCXEFT_REN_QUAD;
	inf.nPrtSize = sizeof(MyEft2D3);
	inf.pPrtList = new MyEft2D3[nPartNum];
	inf.pTex     = m_pTex1;

	inf.IntlRect = LC_RECTF(0,0, ScnW, ScnH);
	inf.IntlPos  = LCXVECTOR2(250, 420);

	hr = LcxEnt_CreateEffect((char*)LCXCMD_EFT_PARTICLE2D, &m_pEft3, &inf);
	if(LC_FAILED(hr))
		return hr;


	inf.nPrtNum  = nPartNum;
	inf.bPrtRen  = LCXEFT_REN_QUAD;
	inf.nPrtSize = sizeof(MyEft2D1);
	inf.pPrtList = new MyEft2D1[nPartNum];
	inf.pTex     = m_pTex1;

	inf.IntlRect = LC_RECTF(0,0, ScnW, ScnH);
	inf.IntlPos  = LCXVECTOR2(400, 420);
	inf.IntlCol	 = LCXCOLOR(0, 0.5F, 1, 1);

	hr = LcxEnt_CreateEffect((char*)LCXCMD_EFT_PARTICLE2D, &m_pEft4, &inf);
	if(LC_FAILED(hr))
		return hr;

	return LC_OK;
}



INT	CMain::Destroy()
{
	SAFE_RELEASE(	m_pEft1		);
	SAFE_RELEASE(	m_pEft2		);
	SAFE_RELEASE(	m_pEft3		);
	SAFE_RELEASE(	m_pEft4		);

	SAFE_RELEASE(	m_pTex1		);

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
		//printf("FPS: %3.f\n", m_fFPS);
		n = 0;
	}



	// Input Update
	m_pInput->Update();

	const BYTE*  pKey = m_pInput->GetKeyMap();
	const BYTE*  pMts = m_pInput->GetMtsMap();
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

	if(pKey[LCIK_LEFT])
		m_pCam->RotateYaw(fAngle_degree, speed);

	if(pKey[LCIK_RIGHT])
		m_pCam->RotateYaw(fAngle_degree, -speed);

	if(pKey[LCIK_UP])
		m_pCam->RotatePitch(fAngle_degree, speed);

	if(pKey[LCIK_DOWN])
		m_pCam->RotatePitch(fAngle_degree, -speed);


	m_pCam->Update();


	if(pMts[0])
	{
		LCXVECTOR2	MtsPos = m_pInput->GetMtsPos();
		TLCX_PRT2dINF	inf;
		m_pEft4->GetAttrib(LCXEFT_INFO, &inf);
		inf.IntlPos	= MtsPos;
		m_pEft4->SetAttrib(LCXEFT_INFO, &inf);
	}


	m_pEft1->FrameMove();
	m_pEft2->FrameMove();
	m_pEft3->FrameMove();
	m_pEft4->FrameMove();


	return LC_OK;
}


INT	CMain::Render()
{
	m_pDev->BeginScene();

	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);

	LCXCOLOR dclear(0,0,0,1);
	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, &dclear);

	m_pCam->SetTransformPrj();
	m_pCam->SetTransformViw();


	m_pDev->SetRenderState(LC_RS_CULL_FACE, LC_ST_DISABLE);
	m_pDev->SetRenderState(LC_RS_LIGHTING, LC_ST_DISABLE);


//	m_pXYZ->Render();
//	m_pGrid->Render();

	m_pSpt->Begin();

		m_pSpt->SetRenderState(LC_RS_BLEND_SRC , LC_ST_SRC_ALPHA);
		m_pSpt->SetRenderState(LC_RS_BLEND_DEST, LC_ST_ONE);

		m_pEft1->Render();
		m_pEft2->Render();
		m_pEft3->Render();
		m_pEft4->Render();

	m_pSpt->End();


	m_pDev->EndScene();

	return LC_OK;
}


