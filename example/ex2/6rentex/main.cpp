

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
//#include <gles/gl.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>


#define MEDIA_DIR	"../../media/tex/"


LC_HWND		m_hWnd	= NULL;		// Window Handle
PLC_DEVICE	m_pDev	= NULL;		// Device
PLC_SPRITE	m_pSpt	= NULL;		// 2D Sprite

PLC_TEXTURE	m_pTex1 = NULL;		// Texture 1
PLC_TEXTURE	m_pTex2 = NULL;		// Texture 2
PLC_TEXTURE	m_pTexR = NULL;		// Texture 3

INT				g_ImgW = 480;
INT				g_ImgH = 320;



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

	LCXCOLOR r(0,0,1,1);
	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, &r);



	////////////////////////////////////////////////////////////////////////////
	// Rendering to Frame Buffer Process



	m_pDev->BeginScene();			// Prepare Rendering...
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);	// Clear Backbuffer


	m_pSpt->Begin();

	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);	// Clear Backbuffer

	LC_RECT rc(0,0, g_ImgW, g_ImgH);
	m_pDev->SetRenderState(LC_RS_VIEWPORT, &rc);
	m_pSpt->Draw(m_pTex1, NULL, NULL);


	static int angle = 0;
	LCXVECTOR2 pos(48, 48);
	LCXVECTOR2 rot(320/2.F, 240/2.F);
	LCXVECTOR2 scl(1.5F, 1.5F);
	float fRad = 0;

	++angle;
	fRad = LCXToRadian(angle);

	LCXCOLOR color(1,1,1, (float)(cos(fRad) * cos(fRad)));

	m_pSpt->DrawEx(m_pTex2, NULL, &pos, &scl, &rot, fRad, &color);

	rc = LC_RECT(0,0, 480, 320);
	m_pDev->SetRenderState(LC_RS_VIEWPORT, &rc);
	
	m_pSpt->End();						// Sprite End
	m_pDev->EndScene();						// Post Rendering...


	////////////////////////////////////////////////////////////////////////////
	// Copy From Frame Buffer

	m_pTexR->CopyTexImage2D();



	////////////////////////////////////////////////////////////////////////////
	// Rendering to Frame Buffer with saved pixel from framebuffer


	m_pDev->BeginScene();			// Prepare Rendering...
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);	// Clear Backbuffer



	m_pSpt->Begin();


	for(int i=10; i>=0; --i)
	{
		LCXVECTOR2	scl( i/10.F, i/10.F);
		pos = LCXVECTOR2( (10-i)*16, (10-i)* 12);
		m_pSpt->DrawEx(m_pTexR, NULL, &pos, &scl, NULL, 0, &LCXCOLOR(1,1,1, i/10.F));
	}

	m_pSpt->End();						// Sprite End
	m_pDev->EndScene();						// Post Rendering...


	return LC_OK;
}




// Main Program
int main(int argc, char* argv[])
{
	int	hr = 0;

	char sFile1 [] = MEDIA_DIR"bg256x256x256.bmp";
	char sFile2 [] = MEDIA_DIR"fire128.tga";

	TLC_IMAGE	t=
	{
		  LC_FMT_RGB
		, LC_FMT_UBYTE
		, g_ImgW
		, g_ImgH
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


	// Create 2D Rendering Sprite
	if(LC_FAILED(LcDev_CreateSprite(NULL, &m_pSpt)))
		goto End;

	LcVar_WriteP(LC_VAR_SPRITE0, m_pSpt);




	if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &m_pTex1, (LC_HANDLE)sFile1)))
		goto End;

	if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &m_pTex2, (LC_HANDLE)sFile2)))
		goto End;


	// for rendering copy texture
	if(LC_FAILED(LcDev_CreateTextureTarget(NULL, &m_pTexR, &g_ImgW, &g_ImgH)))
		goto End;



	printf("LcDev_CreateTexture Success\n");


	// Run Program
	while(1)
	{
		if(LC_FAILED(LcSys_Run()))
			break;

		// Flipping
		m_pDev->Present(0,0,0,0);
	}


End:
	// Release Texture
	m_pTex1->Release();
	m_pTex2->Release();
	m_pTexR->Release();

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
