

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


#include <vector>
using namespace std;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>


#define MEDIA_DIR	"../../media/"



LC_HWND		m_hWnd	= NULL;		// Window Handle
PLC_DEVICE	m_pDev	= NULL;		// Device
PLC_SPRITE	m_pSpt	= NULL;		// 2D Sprite

PLC_TEXTURE	m_pTex	= NULL;		// Texture List


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


	struct AniSeq
	{
		float	x, y, w, h, oX, oY;		// x, y, width,height, offset x, offset y
	};

	AniSeq sq[]=
	{
		{	269,   1,  90, 153,	   3,	63.5},
		{	360,   1, 110, 152,	  13,	63  },
		{	  0, 201, 103, 152,	 9.5,	63  },
		{	197, 201,  91, 151,	 3.5,	62.5},
		{	394, 201, 115, 147,	15.5,	60.5},

		{	289, 201, 104, 148,	10	,	61  },
	};

	// Prepare Rendering...
	m_pDev->BeginScene();

	// Clear Backbuffer
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);

	// Sprite Begin
	m_pSpt->Begin();

	static int Idx = 0;
	static DOUBLE dBgn = LcSys_GetTickCount();

	DOUBLE dCur = LcSys_GetTickCount();


	if(dCur-dBgn>67)
	{
		++Idx;
		Idx %= 6;

		dBgn = dCur;
	}

	LCXRECT		rc(sq[Idx].x, sq[Idx].y, sq[Idx].w, sq[Idx].h);

	LCXVECTOR2	vcDelta= LCXVECTOR2(-sq[Idx].w*.5F, -sq[Idx].h*.5F)
						+ LCXVECTOR2(sq[Idx].oX, -sq[Idx].oY );


	LCXVECTOR2	vcpos = LCXVECTOR2(160, 200);


	vcpos += vcDelta;

	

	

	m_pSpt->Draw(m_pTex, &rc, &vcpos);

	// Sprite End
	m_pSpt->End();


	// Post Rendering...
	m_pDev->EndScene();


	return LC_OK;
}




// Main Program
int main(int argc, char* argv[])
{
	INT hr = 0;

	UINT	dClearColor = 0xFF006699;

	// Init GLC Library
	if(LC_FAILED(LcSys_Init()))
		return LC_OK;

	// Set Main procedure Function Pointer
	if(LC_FAILED(LcSys_SetMainRunFunc(Run)))
		goto End;


	// Create Window
	m_hWnd = LcSys_CreateWindow(NULL, (char*)"LC_Tutorial", 800, 480);
	if(NULL == m_hWnd)
		goto End;

	// Create GLC Rendering Device
	if(LC_FAILED(LcDev_CreateDevice(NULL, &m_pDev, m_hWnd, NULL)))
		goto End;


	printf("LcDev_CreateDevice Success\n");


	// Set Clear Color
	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, dClearColor);


	// Create 2D Rendering Sprite
	if(LC_FAILED(LcDev_CreateSprite(NULL, &m_pSpt)))
		goto End;

	printf("LcDev_CreateSprite Success\n");

	LcVar_WriteP(LC_VAR_SPRITE0, m_pSpt);


	if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &m_pTex, (char*)MEDIA_DIR"model2d/character02.png")))
		goto End;


	printf("LcDev_CreateTextureFromFile Success\n");


	// Run Program
	while(1)
	{
		if(LC_FAILED(LcSys_Run()))
			break;

		// Flipping
		m_pDev->Present(0,0,0,0);
	}


End:
	m_pTex->Release();
	

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
