// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include <gles/gl.h>

#include "_StdAfx.h"


#define MEDIA_DIR	"../../media/"


CMain::CMain()
{
	m_pInput	= NULL;
	m_pCam		= NULL;
	m_pXYZ		= NULL;
	m_pGrid		= NULL;

	m_pTex		= NULL;
	m_pMoSqc	= NULL;
}


INT	CMain::Init()
{
	INT	i  = 0;
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



	if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &m_pTex, (char*)MEDIA_DIR"tex/star128.tga")))
		return LC_EFAIL;


	FLOAT w = 128.F/2.F;
	FLOAT h = 128.F/2.F;

	// setup the motion sequence
	TLCX_MOSQIC2D t[7];
	int iSize = sizeof(t)/sizeof(t[0]);

	t[0] = TLCX_MOSQIC2D((char*)"mot0");
	t[1] = TLCX_MOSQIC2D((char*)"mot1", 500);
	t[2] = TLCX_MOSQIC2D((char*)"mot2", 1000);
	t[3] = TLCX_MOSQIC2D((char*)"mot3", 1500);
	t[4] = TLCX_MOSQIC2D((char*)"mot4", 2000);
	t[5] = TLCX_MOSQIC2D((char*)"mot5", 2500);
	t[6] = TLCX_MOSQIC2D((char*)"mot6", 3000);

	t[0].pos = LCXVECTOR2(100, 100);	t[0].scl = LCXVECTOR2(1, 1);	t[0].roc = LCXVECTOR2(1*w, 1*h) + t[0].pos;	t[0].ror = LCXToRadian(  0);
	t[1].pos = LCXVECTOR2(200, 100);	t[1].scl = LCXVECTOR2(2, 1);	t[1].roc = LCXVECTOR2(2*w, 1*h) + t[1].pos;	t[1].ror = LCXToRadian(120);
	t[2].pos = LCXVECTOR2(300, 100);	t[2].scl = LCXVECTOR2(3, 1);	t[2].roc = LCXVECTOR2(3*w, 1*h) + t[2].pos;	t[2].ror = LCXToRadian(240);
	t[3].pos = LCXVECTOR2(300, 200);	t[3].scl = LCXVECTOR2(3, 2);	t[3].roc = LCXVECTOR2(3*w, 2*h) + t[3].pos;	t[3].ror = LCXToRadian(360);
	t[4].pos = LCXVECTOR2(200, 200);	t[4].scl = LCXVECTOR2(2, 2);	t[4].roc = LCXVECTOR2(2*w, 2*h) + t[4].pos;	t[4].ror = LCXToRadian(480);
	t[5].pos = LCXVECTOR2(100, 200);	t[5].scl = LCXVECTOR2(1, 2);	t[5].roc = LCXVECTOR2(1*w, 2*h) + t[5].pos;	t[5].ror = LCXToRadian(600);
	t[6].pos = LCXVECTOR2(100, 100);	t[6].scl = LCXVECTOR2(1, 1);	t[6].roc = LCXVECTOR2(1*w, 1*h) + t[6].pos;	t[6].ror = LCXToRadian(720);


	// create motion sequence
	hr = LcxEnt_CreateMoSqc(LCXCMD_MOSQC_2D, &m_pMoSqc, t, &iSize);
	if(LC_FAILED(hr))
		return hr;


	return LC_OK;
}



INT	CMain::Destroy()
{
	SAFE_RELEASE(	m_pMoSqc	);
	SAFE_RELEASE(	m_pTex		);

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


	int hr = 0;


	// Input Update
	m_pInput->Update();

	const BYTE*  pKey    = m_pInput->GetKeyMap();
	if(pKey[LCIK_ESCAPE])
		return LC_EFAIL;


	m_pCam->Update();


	INT		MotStt = LCXMO_DEFAULT;
	DOUBLE	dTime = this->GetElapsedTime();



	// set the begin time and start motion
	if(pKey[LCIK_DOWN])
		m_pMoSqc->SetAttrib(LCXMO_BEGIN, NULL);



	// setup the time
	m_pMoSqc->SetAttrib(LCXMO_ADVANCE_TIME, &dTime);

	// update the motion sequence. and get the state
	MotStt = m_pMoSqc->FrameMove();


	return LC_OK;
}






INT	CMain::Render()
{
	INT	hr;

	m_pDev->BeginScene();

	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);

	m_pCam->SetTransformPrj();
	m_pCam->SetTransformViw();


	m_pDev->SetRenderState(LC_RS_CULL_FACE, LC_ST_DISABLE);
	m_pDev->SetRenderState(LC_RS_LIGHTING, LC_ST_DISABLE);


	m_pXYZ->Render();
	m_pGrid->Render();


	TLCX_MOSQ2D	ret;

	hr = m_pMoSqc->GetAttrib(LCXMO_RESULT_SQC, &ret);


	m_pSpt->Begin();

	m_pSpt->DrawEx(m_pTex, NULL, &ret.pos, &ret.scl, &ret.roc, ret.ror, NULL);
	m_pSpt->End();



	m_pDev->EndScene();

	return LC_OK;
}


