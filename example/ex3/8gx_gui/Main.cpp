// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"


#define MEDIA_DIR	"../../media/model2d/"


CMain::CMain()
{
	m_pInput	= NULL;
	m_pCam		= NULL;
	m_pXYZ		= NULL;
	m_pGrid		= NULL;

	m_pTex1		= NULL;
	m_pTex2		= NULL;

	m_pGui		= NULL;
}


#define IDC_BUTTON1		0x001
#define IDC_BUTTON2		0x002
#define IDC_BUTTON3		0x003

INT SaveImageto16BitText(TLC_IMAGE* Out, char* sImageFile, UINT InColorKey=0x0, UINT dInFormat=LC_FMT_WORD_5551);




INT	CMain::Init()
{
	INT	i  = 0;
	INT	hr = LC_OK;


	// Input 생성
	if(LC_FAILED(LcxEnt_CreateInput(NULL, &m_pInput, m_hWnd)))
		return LC_EFAIL;


	// Camera 생성
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


	#if 0
	{
		TLC_IMAGE img={0};
		hr = SaveImageto16BitText(&img, (char*)"rsc/win_dialog.png");
		if(LC_FAILED(hr))
			return hr;

		hr = SaveImageto16BitText(&img, (char*)"rsc/win_button.png");

		if(LC_FAILED(hr))
			return hr;
	}
	#endif





	// Create gui Method 1: defaut
	{
		TLC_FONT	tFont((char*)"../../media/font/mwg_b.otf", 9);
		
		TLCX_GUI_INF t;
		LCXRECT		 rEvnt;
		PLCX_GUI     Dlg  = NULL;					// Gui Dialog

		t.sName = (char*)"world";
		rEvnt	= LCXRECT(200, 20, 380, 250);	// Gui x, y, width, Height
		t.rEvnt	= &rEvnt;
		hr = LcxEnt_CreateGui(LCXCMD_GUI_DIALOG, &Dlg, &t);


		t.sName = (char*)"Hi";
		rEvnt	= LCXRECT(150, 150, 360, 260);	// Gui x, y, width, Height
		t.rEvnt	= &rEvnt;
		hr = LcxEnt_CreateGui(LCXCMD_GUI_DIALOG, &Dlg, &t);


		// Create buttons
		PLCX_GUI     Btn  = NULL;					// Gui Button
		PLCX_GUI     Stt  = NULL;					// Gui Static
		LCXVECTOR2   vcPos(0,0);

		vcPos	= LCXVECTOR2(100, 100);
		t.sName = (char*)"button1";
		t.sText	= t.sName;
		t.nId   = IDC_BUTTON1;
		hr = LcxEnt_CreateGui(LCXCMD_GUI_BUTTON, &Btn, &t);

		Btn->SetPosition(&vcPos);
		Btn->SetAttrib(LCXGUI_FONT, &tFont);
		Btn->SetText(t.sName);
		Dlg->Attach(Btn);


		vcPos	= LCXVECTOR2(160, 100);
		t.sName = (char*)"button2";
		t.sText	= t.sName;
		t.nId   = IDC_BUTTON2;
		hr = LcxEnt_CreateGui(LCXCMD_GUI_BUTTON, &Btn, &t);

		Btn->SetPosition(&vcPos);
		Btn->SetAttrib(LCXGUI_FONT, &tFont);
		Btn->SetText(t.sName);
		LCXVECTOR2	vcScl(1.3F,1);
		Btn->SetScaling(&vcScl);
		Dlg->Attach(Btn);


		vcPos	= LCXVECTOR2(100, 150);
		t.sName = (char*)"button3";
		t.sText	= t.sName;
		t.nId   = IDC_BUTTON3;
		hr = LcxEnt_CreateGui(LCXCMD_GUI_BUTTON, &Btn, &t);

		Btn->SetPosition(&vcPos);
		Btn->SetAttrib(LCXGUI_FONT, &tFont);
		Btn->SetText("Hello");
		Dlg->Attach(Btn);



		// Create Static
		vcPos	= LCXVECTOR2(100, 180);
		t.sName = (char*)"static1";
		t.sText	= t.sName;
		t.nId   = IDC_BUTTON3;
		hr = LcxEnt_CreateGui(LCXCMD_GUI_STATIC, &Stt, &t);

		Stt->SetPosition(&vcPos);
		Stt->SetAttrib(LCXGUI_FONT, &tFont);
		Dlg->Attach(Stt);
	}


	// Create gui Method 2: extern value
	{
		LcDev_CreateTextureFromFile(NULL, &m_pTex1, (char*)MEDIA_DIR"login.png");

		// setupt the image info
		TLC_IMAGE	txInf={0};
		txInf.ImgW = m_pTex1->GetImgW();
		txInf.ImgH = m_pTex1->GetImgH();
		txInf.ImgD = m_pTex1->GetImgD();


		// setup the extern value
		TLCX_GUI_EXTERN ext;
		LCXRECT  Evnt(20, 200, 148,   196);
		LCXRECT  Rcts[32];

		Rcts[ 0] = LCXRECT(  0,  0,  148,   23);	// title
		Rcts[ 4] = LCXRECT(125,  4, 9999, 9999);	// system button position(음수면 button의 위치는 변하지 않음.) LCXRect2Region()에서 계산 되므로 음수가 안되도록 아주 큰 값을 선택
		Rcts[ 5] = LCXRECT(150,  0,  166,   13);	// system button normal
		Rcts[ 6] = LCXRECT(170,  0,  186,   13);	// system button over
		Rcts[ 7] = LCXRECT(190,  0,  206,   13);	// system button pressed
		Rcts[12] = LCXRECT(  0, 23,  148,  196);	// body

		for(i=0; i<32; ++i)
			LCXRect2Region(&Rcts[i]);

		ext.txInf = &txInf;
		ext.rEvnt = &Evnt;							// if Evnt is defined then the value of rEvnt in TLCX_GUI_INF is ignored.
		ext.rRcts = Rcts;


		// setup the gui info

		TLCX_GUI_INF t;
		LCXRECT	 rEvnt;
		PLCX_GUI     Dlg  = NULL;					// Gui Dialog

		// Create Dialogs
		t.sName = (char*)"Hello";					// Gui Name
		rEvnt	= LCXRECT(50, 50, 200, 150);		// Gui x, y, width, Height;
		t.rEvnt	= &rEvnt;							// this value will be ignored when the rEvnt value in TLCX_GUI_EXTERN is allready defined.
		hr = LcxEnt_CreateGui(LCXCMD_GUI_DIALOG, &Dlg, &t, &ext);
		Dlg->SetAttrib(LCXGUI_TEXTURE, m_pTex1);
	}



	// Create gui Method 3: gui list from file
	{
		PLCX_GUI* GuiLst  = NULL;									// Gui Dialog
		char	  TextureName[260]={0};

		int	nList = LcxEnt_CreateGuiListFromFile(NULL						// Gether the gui list
											, &GuiLst
											, TextureName
											, (char*)MEDIA_DIR"login.gut"
											);

		if(0 < nList)
		{
			LcDev_CreateTextureFromFile(NULL, &m_pTex2, TextureName);		// create texture
			for(i= 0; i<nList; ++i)										// bind the texture to gui list
				GuiLst[i]->SetAttrib(LCXGUI_TEXTURE, m_pTex2);

			SAFE_DELETE_ARRAY(	GuiLst	);								//release gui list
		}
	}



	return LC_OK;
}



INT	CMain::Destroy()
{
	LcxGui_DestroyAllDialog();


	SAFE_RELEASE(	m_pTex1		);
	SAFE_RELEASE(	m_pTex2		);

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


	DOUBLE fElapsedTime = this->GetElapsedTime();
	DOUBLE fTime = fElapsedTime;


	// Update and get the event Gui
	FLOAT* MtsPos  = (FLOAT*)m_pInput->GetMtsPos();
	FLOAT* MtsGrd  = (FLOAT*)m_pInput->GetMtsGrd();
	INT    MtsState= m_pInput->MtsState() ;

	TLCX_GUI_EVENT	Evnt(0);
	PLCX_GUI		pGui(0);

	// setup event message
	if(LCIK_KEYDOWN == MtsState)
	{
		Evnt._m = LCXGUI_EVNT_DOWN;
		//printf("Input Evnt Down--------------------------\n");
	}

	else if(LCIK_KEYUP == MtsState)
	{
		Evnt._m = LCXGUI_EVNT_UP;
		//printf("Input Evnt Up--------------------------\n");
	}
	else if(LCIK_KEYPRESS == MtsState)
		Evnt._m = LCXGUI_EVNT_PRESS;

	if(0.F != MtsGrd[0] || 0.F != MtsGrd[1])
		Evnt._m |= LCXGUI_EVNT_MOVE;

	// setup event value
	Evnt.SetVal(MtsPos, 2);


	hr = LcxGui_Process(&Evnt);

	// Evnt 처리
	if(0<hr)
	{
		pGui = Evnt._o;

		UINT  nType = 0;
		UINT  nId   = 0;
		char* name  = NULL;
		char* text  = NULL;
		
		if(pGui)
		{
			nType = pGui->GetRscType();
			pGui->GetAttrib(LCXGUI_ID, &nId);
			pGui->GetAttrib(LCXGUI_NAME, &name);
			pGui->GetAttrib(LCXGUI_TEXT, &name);
		}


		// Close event가 있어도 수동으로 dialog를 해제하지 않으면 계속사용할 수있음
		if( LCXGUI_DIALOG     == Evnt._t &&
			LCXGUI_EVNT_CLOSE == Evnt._m)
		{
			pGui->SetAttrib(LCXGUI_HIDE, NULL);

			m_pGui = pGui;
			// W key를 누르면 다시 활성화 함.
			//SAFE_RELEASE(	pGui	);
		}
		else
		{
			//printf("Evnt Recv: %x %x %x\n", Evnt._o, Evnt._t, Evnt._m);
		}


		if(LCXGUI_EVNT_UP == Evnt._m)	// && IDC_BUTTON1 <= nId && nId <= IDC_BUTTON3)
			printf("Evnt button Id: %d, Name: %s\n", nId, name);


		name  = NULL;
		text  = NULL;
	}


	if(m_pGui)
	{
		if(pKey[LCIK_W])
		{
			m_pGui->SetAttrib(LCXGUI_SHOW, NULL);
			m_pGui = NULL;
		}

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


	m_pXYZ->Render();
	m_pGrid->Render();


	m_pSpt->Begin();

		LcxGui_Render();

	m_pSpt->End();
	
	m_pDev->EndScene();

	return LC_OK;
}








INT SaveImageto16BitText(TLC_IMAGE* Out, char* sImageFile, UINT InColorKey, UINT dInFormat)		// LC_FMT_WORD_565, LC_FMT_WORD_5551, LC_FMT_WORD_44444)
{
	INT			hr	= 0;
	PLC_IMAGE	pImg= NULL;

	hr = LcDev_CreateImageFromFile(NULL, &pImg, sImageFile);
	if( LC_FAILED(hr))
	{
		printf("Err: %d, %s \t", __LINE__, __FILE__);
		printf("Load Texture Failed\n");
		return LC_CANNT_FIND_RESOURCE;
	}


	UINT	ImgW = pImg->GetImgW() ;
	UINT	ImgH = pImg->GetImgH() ;
	UINT	ImgD = pImg->GetImgD() ;
	UINT	ImgC = pImg->GetImgC() ;
	BYTE*	pPxl = pImg->GetPixel();


	UINT	oFormat=0;
	UINT	oType=0;
	WORD*	oPxl = new WORD[ImgW * ImgH];
	LcDev_Convert32to16Color(oPxl, &oFormat, &oType, pPxl, ImgW, ImgH, ImgD, 0x0, LC_FMT_WORD_5551);


	char sTxt[260]={0};
	char* p = NULL;

	strcpy(sTxt, sImageFile);
	p = strrchr(sTxt, '.');
	++p;

	strcpy(p, "txt");

	FILE* fp = fopen(sTxt, "wb");

	p = strrchr(sTxt, '.');
	*p = 0;

	for( p= sTxt; *p; ++p)
	{
		if(*p == '/')
			*p ='_';
	}


	fprintf(fp, "static unsigned short %s [] = {\n", sTxt);

	int x, y, n;
	char sbuf[16];
	char sdst[16];
	for(y=0; y<ImgH; ++y)
	{
		for(x=0; x<ImgW; ++x)
		{
			n = y * ImgW + x;

			WORD pxl = oPxl[n];


			if(0 == n%16 && (x || y) )
				fprintf(fp, "\n");


			memset(sbuf, 0, 16);
			memset(sdst, 0, 16);

			sprintf(sbuf, "0000%X", pxl);

			int ilen = strlen(sbuf)-1;

			for(int k=3; k >=0; --k, --ilen)
			{
				sdst[k] = sbuf[ilen];
			}

		
			sprintf(sbuf, "0x%s", sdst);
			fprintf(fp, "%s, ", sbuf);
		}
	}

	fprintf(fp, "\n};\n");

	fclose(fp);


//	Out->Format	= oFormat;
//	Out->Type	= oType;
//	Out->ImgW	= ImgW;
//	Out->ImgH	= ImgH;
//	Out->ImgD	= 2;
//	Out->ImgC	= ImgC;
//	Out->pPxl   = (BYTE*)oPxl;

	SAFE_RELEASE(	pImg	);

	return LC_OK;
}
