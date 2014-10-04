

#if defined(_MSC_VER) && _MSC_VER > 1000
  #pragma warning( disable : 4786)
  #pragma warning( disable : 4996)
#endif


#if defined(_MSC_VER)
  #pragma comment(linker, "/subsystem:console")

  #pragma comment(lib, "lgpwn_b22_.lib")
  #pragma comment(lib, "lgpwn_c22_.lib")
  #pragma comment(lib, "lgpwn_x22_.lib")
  #pragma comment(lib, "lgpwn_t22_.lib")
  #pragma comment(lib, "lgpwn_ogg_.lib")
  #pragma comment(lib, "lgpwn_vorbis_.lib")

  #pragma comment(lib, "ftf243m_.lib")
  #pragma comment(lib, "ImgJpg_.lib")
  #pragma comment(lib, "png144_.lib")
  #pragma comment(lib, "zlib125_.lib")
  #pragma comment(lib, "al11s_.lib")

  #pragma comment(lib, "opengl32.lib")
  #pragma comment(lib, "winmm.lib")
  #pragma comment(lib, "dxguid.lib")
#endif


#include <stdio.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>

#include <LcStdev.h>
#include <LcxEntity.h>


#define MEDIA_DIR	"../../media/"


LC_HWND		m_hWnd	= NULL;		// Window Handle
PLC_DEVICE	m_pDev	= NULL;		// Device
PLC_SPRITE	m_pSpt	= NULL;		// 2D Sprite

PLC_FONT	m_pFont	= NULL;		// Font


FLOAT GetAppFrame();


INT LC_CALLBACK Run(LC_HANDLE)
{
#if defined(_WIN32)
	LcSys_Sleep(10);
#endif


	static INT c=0;
	++c;

	if(c>18*60*20)
	{
		printf("Game Process Terminated\n");
		return LC_EFAIL;
	}


//	FLOAT fFrame = GetAppFrame();
//	static INT n=0;
//	++n;

//	if(n>5)
//	{
//		printf("%3.f\n", fFrame);
//		n = 0;
//	}




	BYTE		Keypad[LC_MAX_KEY]={0};
	LC_INPUT_TS	mts[4]={0};


	LcStv_InputReadKey(Keypad);				// Read Keypad data
	LcStv_InputReadMts(0, &mts[0]);			// Read Touch Screen or left
	LcStv_InputReadMts(1, &mts[1]);			// Read Touch Screen or right
	LcStv_InputReadMts(2, &mts[2]);			// Read Touch Screen or middle


	if(Keypad[LCIK_ESCAPE])
		return LC_EFAIL;



	char sbuf[128]={0};

	sprintf(sbuf, "Try to presss buttons.");

	for(int i=0; i<LC_MAX_KEY; ++i)
	{
		if(Keypad[i])
		{
			sprintf(sbuf, "You Pressed %d: state: %d", i, Keypad[i]);
		}
	}


	LCXVECTOR2	vcPos(5, 5);
	LCXCOLOR	color(1, 1, 0, 1);

	m_pFont->SetPosition(&vcPos);
	m_pFont->SetColor(&color);
	m_pFont->SetString(sbuf);


	// Prepare Rendering...
	m_pDev->BeginScene();

	// Clear Backbuffer
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);


	// Sprite Begin
	m_pSpt->Begin();


	// Draw buffer
	m_pFont->DrawTxt();


	{
		sprintf(sbuf, "Touch pos:(%3d %3d) st(%3d, %3d)", mts[0].x, mts[0].y, mts[0].t, mts[1].t);

		vcPos = LCXVECTOR2(5, 35);
		color = LCXCOLOR(0, 1, 1, 1);

		m_pFont->SetPosition(&vcPos);
		m_pFont->SetColor(&color);
		m_pFont->SetString(sbuf);

		m_pFont->DrawTxt();
	}


	// Sprite End
	m_pSpt->End();


	// Post Rendering...
	m_pDev->EndScene();

	return LC_OK;
}




// Main Program
int main(int argc, char* argv[])
{
	INT		hr = 0;


	UINT	dClearColor = 0xFF006699;

	char		sFont1[] = MEDIA_DIR"font/mwg_b.otf";
	TLC_FONT	tFont(sFont1, 12);

	// Init GLC Library
	if(LC_FAILED(LcSys_Init()))
		return LC_OK;

	// Set Main procedure Function Pointer
	if(LC_FAILED(LcSys_SetMainRunFunc(Run)))
		goto End;


	// Create Window
	m_hWnd = LcSys_CreateWindow(NULL, (char*)"LC_Tutorial", 480, 320);
	if(NULL == m_hWnd)
		goto End;


	// Create GLC Rendering Device
	if(LC_FAILED(LcDev_CreateDevice(NULL, &m_pDev, m_hWnd, NULL)))
		goto End;

	// Set Clear Color
	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, dClearColor);


	// Create 2D Rendering Sprite
	if(LC_FAILED(LcDev_CreateSprite(NULL, &m_pSpt)))
		goto End;

	LcVar_WriteP(LC_VAR_SPRITE0, m_pSpt);



	// Create Input20
	if(LC_FAILED(LcStv_InputOpen(m_hWnd)))
		goto End;

	printf("LcxEnt_CreateInput Success\n");



	// Create Font Instance
	if(LC_FAILED(LcDev_CreateFontFromFile(NULL, &m_pFont, m_pSpt, &tFont)))
		goto End;





	// Run Program
	while(1)
	{
		if(LC_FAILED(LcSys_Run()))
			break;

		// Flipping
		m_pDev->Present(0,0,0,0);
	}


End:


	// Release font instance
	m_pFont->Release();

	
	// Release Input
	LcStv_InputClose();


	// Release Sprite
	m_pSpt->Release();


	// Release Device
	hr = m_pDev->Release();
	if(0 != hr)
		printf(
				"The non-zero reference count means that\n"
				"there is something object not cleaned up.\n"
				);


	// Release Window
	LcSys_DestroyWindow(&m_hWnd);

	// Release GLC
	LcSys_Close();

	return LC_OK;

}







FLOAT GetAppFrame()
{
#if defined(_WIN32)
	static INT		MaxCount = 30;
#else
	static INT		MaxCount = 10;
#endif

	static DOUBLE  fFps = 0.;
	static INT     iCnt = 0;
	static DOUBLE   dBgn = ::LcSys_GetTickCount();
	DOUBLE   dCur = ::LcSys_GetTickCount();  // GetTickCount()

	++iCnt;

	if(iCnt>=MaxCount)
	{
		fFps = DOUBLE(dCur-dBgn);
		fFps = iCnt*1000./fFps;
		iCnt = 0;
		dBgn = dCur;
	}

	return (FLOAT)fFps;
}
