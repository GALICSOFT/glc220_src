

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

  #pragma comment(lib, "lgpwn_ogg_.lib")
  #pragma comment(lib, "lgpwn_vorbis_.lib")
#endif


#include <vector>
using namespace std;


#if defined(_WIN32)
	#include <AL_win/al.h>
	#include <AL_win/alc.h>
	#include <AL_win/alut.h>
#else
	#include <AL/al.h>
	#include <AL/alc.h>
	#include <AL/alut.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>
#include <LcStdev.h>
#include <LcxEntity.h>
#include <LcxMedia.h>


#define MEDIA_DIR	"../../media/"
#define MAX_SOUND	4


LC_HWND		m_hWnd	= NULL;		// Window Handle
PLC_DEVICE	m_pDev	= NULL;		// Device
PLC_SPRITE	m_pSpt	= NULL;		// 2D Sprite

PLCX_INPUT	m_pInput= NULL;		// Input
PLC_FONT	m_pFont	= NULL;		// Font

vector<PLCX_SOUND > m_vSnd(0);	// Sound Instance



FLOAT GetAppFrame();


INT LC_CALLBACK Run(LC_HANDLE)
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

	FLOAT fFrame = GetAppFrame();


	static INT n=0;
	++n;

	if(n>5)
	{
		printf("%f\n", fFrame);
		n = 0;
	}

	char sbuf[128]={0};
	LCXVECTOR2	vcPos(5, 5);
	LCXCOLOR	color(1, 1, 0, 1);

	sprintf(sbuf, "Try to presss F_A(left), F_B(right), or F_Y(up) button.");
	m_pFont->SetPosition(&vcPos);
	m_pFont->SetColor(&color);
	m_pFont->SetString(sbuf);




	// Input Update
	m_pInput->Update();


	if(m_pInput->KeyDown(LCIK_ESCAPE))
		return LC_EFAIL;


	// Play Sound
	if(m_pInput->KeyDown(LCIK_LEFT))
	{
		for(int i=2; i< m_vSnd.size(); ++i)
			m_vSnd[i]->Play();
	}

	// Play ding.wav
	if(m_pInput->KeyDown(LCIK_RIGHT))
		m_vSnd[0]->Play();


	// Play ding.wav
	if(m_pInput->KeyDown(LCIK_UP))
		m_vSnd[1]->Play();



	// Prepare Rendering...
	m_pDev->BeginScene();

	// Clear Backbuffer
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);


	// Sprite Begin
	m_pSpt->Begin();


	// Draw buffer
	m_pFont->DrawTxt();


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
	INT		i;

	UINT		dClearColor = 0xFF006699;
	char*		sFont1 = LCCMD_FONT_DEFAULT;
	TLC_FONT	tFont(sFont1, 10);


	char* soundfile[] =
	{
		(char*)MEDIA_DIR"sound/ding.wav",
		(char*)MEDIA_DIR"sound/snd2.wav",
		(char*)MEDIA_DIR"sound/snd3.wav",
		(char*)MEDIA_DIR"sound/theme.ogg",
			NULL,
	};


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


	// Create Font Instance
	if(LC_FAILED(LcDev_CreateFontFromFile(NULL, &m_pFont, m_pSpt, &tFont)))
		goto End;

	// Create Sound Object
	
	for(i=0; NULL != soundfile[i]; ++i)
	{
		PLCX_SOUND	pSnd;
		hr = LcxSmd_CreateSoundFromFile(NULL, &pSnd, soundfile[i]);
		if(LC_FAILED(hr))
			goto End;

		m_vSnd.push_back(pSnd);
		m_vSnd[i]->SetVolume(0.8F);		// set the sound volume
		m_vSnd[i]->SetRepeat(1);		// set the repeat count
	}

	m_vSnd[i-1]->SetRepeat(LC_INFINITE);		// set the repeat count
	m_vSnd[i-1]->Play();					// play

	printf("Sound Load Complete\n");





	// Run Program
	while(1)
	{
		if(LC_FAILED(LcSys_Run()))
			break;

		// Flipping
		m_pDev->Present(0,0,0,0);
	}


End:

	for(i=0; i<m_vSnd.size(); ++i)	// Release sound object
		m_vSnd[i]->Release();

	m_vSnd.clear();			// Clear sound container


	m_pFont->Release();		// Release Font Instance

	m_pInput->Release();	// Release Input
	m_pSpt->Release();	// Release Device

	hr = m_pDev->Release();	// Release Device
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



