

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
#include <LcxEntity.h>
#include <LcStdev.h>
#include <LcxMedia.h>


#define MEDIA_DIR	"../../media/"


LC_HWND		m_hWnd	= NULL;		// Window Handle
PLC_DEVICE	m_pDev	= NULL;		// Device
PLC_SPRITE	m_pSpt	= NULL;		// 2D Sprite

PLCX_INPUT	m_pInput= NULL;		// Input
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


	FLOAT fFrame = GetAppFrame();


	static INT n=0;
	++n;

	if(n>5)
	{
//		printf("%3.f\n", fFrame);
		n = 0;
	}







	m_pInput->Update();


	LCXVECTOR3	vcTsbPos0 = m_pInput->GetMtsPos();
	LCXVECTOR3	vcTsbPos1 = m_pInput->GetMtsPos(1);

	LC_STATE	nTsbSt0 = m_pInput->MtsState();
	LC_STATE	nTsbSt1 = m_pInput->MtsState(1);
	const BYTE*	pKey = m_pInput->GetKeyMap();
	const BYTE*	pTsb = m_pInput->GetMtsMap();



	if(pKey[LCIK_ESCAPE])
		return LC_EFAIL;



	char sbuf[128]={0};

	sprintf(sbuf, "Try to presss buttons.");

	for(int i=0; i<128; ++i)
	{
		if(pKey[i])
		{
			sprintf(sbuf, "You Pressed %d: state: %d", i, pKey[i]);
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
		sprintf(sbuf, "Touch pos(%.f %.f) state(%d, %d)", vcTsbPos0.x, vcTsbPos0.y, nTsbSt0, nTsbSt1);

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



	// Create Input
	if(LC_FAILED(LcxEnt_CreateInput(NULL, &m_pInput, m_hWnd)))
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

	// Release Font
	m_pFont->Release();

	// Release Input
	m_pInput->Release();

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
