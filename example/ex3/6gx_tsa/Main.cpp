// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"


#define MEDIA_DIR	"../../media/model2d/"


CMain::CMain()
{
	m_pLyr		= NULL;
	m_pInput	= NULL;

	m_pXYZ		= NULL;

	m_nMaxRec	= 0   ;
	m_nTestRec	= 0   ;
	m_pTsa		= NULL;
}


INT	CMain::Init()
{
	INT	hr = LC_OK;

	// Create Sprite Layer
	INT	 spt_type = LC_OBJ_MODEL2D;
	if(LC_FAILED(LcDev_CreateSpriteL(NULL, &m_pLyr, &spt_type)))
		return LC_EFAIL;


	// Input »ý¼º
	if(LC_FAILED(LcxEnt_CreateInput(NULL, &m_pInput, m_hWnd)))
		return LC_EFAIL;


	if(LC_FAILED(LcxEnt_CreateEntity(LCXCMD_ENTITY_AXIS, &m_pXYZ)))
		return LC_EFAIL;


	char	sFile[] = MEDIA_DIR"boss01.gtsa";

	FILE* fp = fopen(sFile, "rb");
	char* buf = NULL;

	ULONG len = 0;
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buf = (char*)malloc(len);
	fread(buf, 1, len, fp);

	TLC_ARGS	pArg={0};

	MAKE_ARG3(pArg, sFile, buf, len );

	hr = LcxEnt_CreateTsaFromMemory(NULL, &m_pTsa, &pArg);
	//hr = LcxEnt_CreateTsa(NULL, &m_pTsa, sFile);
	//hr = LcxEnt_CreateTsa(NULL, &m_pTsa, sFile);
	if(LC_FAILED(hr))
		return LC_EFAIL;

	SAFE_FREE(buf);

	hr = m_pTsa->SetAttrib(LCXM_EXEC_TEXTURE_LOAD, NULL);
	if(LC_FAILED(hr))
		return LC_EFAIL;

	m_nMaxRec = m_pTsa->GetRecIdx(NULL);

	return LC_OK;
}



INT	CMain::Destroy()
{
	//SAFE_RELEASE(	m_pTsa		);
	ILCX_Tsa::ListRelease();			//release all tsa list


	SAFE_RELEASE(	m_pXYZ		);
	SAFE_RELEASE(	m_pInput	);
	SAFE_RELEASE(	m_pLyr		);

	return LC_OK;
}


INT	CMain::FrameMove()
{
	INT hr = 0;


	
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

	if(n>25)
	{
		printf("FPS: %3.f\n", m_fFPS);
		n = 0;


		++m_nTestRec %= m_nMaxRec;
	}

	// Input Update
	m_pInput->Update();

	const BYTE*	pKey = m_pInput->GetKeyMap();

	if(pKey[LCIK_ESCAPE])
		return LC_EFAIL;



	// Move, Rotation Speed
	FLOAT speed = (FLOAT)this->GetElapsedTime();
	speed *=0.1f;



	DOUBLE fElapsedTime = this->GetElapsedTime();
	DOUBLE fTime = fElapsedTime;


	LcSys_ElapseTime(&fTime);


	return LC_OK;
}



INT	CMain::Render()
{
	m_pDev->BeginScene();

	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);

	m_pDev->SetRenderState(LC_RS_CULL_FACE, LC_ST_DISABLE);
	m_pDev->SetRenderState(LC_RS_LIGHTING, LC_ST_DISABLE);


	m_pXYZ->Render();


	LCXVECTOR2	vcPos[4];
	LCXVECTOR2	vcTex[4];

	vcPos[0] = LCXVECTOR2(-100, 100);
	vcPos[1] = LCXVECTOR2( 100, 100);
	vcPos[2] = LCXVECTOR2( 100,-100);
	vcPos[3] = LCXVECTOR2(-100,-100);

	vcTex[0] = LCXVECTOR2( 0, 1);
	vcTex[1] = LCXVECTOR2( 1, 1);
	vcTex[2] = LCXVECTOR2( 1, 0);
	vcTex[3] = LCXVECTOR2( 0, 0);

	// 2D Tsa Rendering
	m_pLyr->Begin();
	{
		//INT nIdx = m_pTsa->GetRecIdx("t01");
		INT nIdx = m_nTestRec;

		TLC_TSAREC* pRec = m_pTsa->GetRec(nIdx);

		FLOAT* pVtxPos = (FLOAT*)pRec->pos;
		FLOAT* pVtxTex = (FLOAT*)pRec->tex;
		FLOAT* pVtxCol = NULL;

		//pVtxPos = (FLOAT*)vcPos;
		//pVtxTex = (FLOAT*)vcTex;


		PLC_TEXTURE pTex = m_pTsa->GetTexture();


		m_pLyr->Draw(pTex, pVtxPos, pVtxTex, pVtxCol);

	}
	m_pLyr->End();


	m_pDev->EndScene();

	return LC_OK;
}

