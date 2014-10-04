

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

vector<PLC_TEXTURE > m_vTex;	// Texture List

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


	// Prepare Rendering...
	m_pDev->BeginScene();

	// Clear Backbuffer
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);

	// Sprite Begin
	m_pSpt->Begin();

//	m_pSpt->Draw(NULL, NULL, NULL);


	FLOAT depth = 0;
	depth = -.5F;
	//m_pSpt->SetRenderState(LC_RS_ZBIAS, *(int*)&depth);
	m_pSpt->Draw(m_vTex[0], NULL, NULL);

	depth = +.5F;
	//m_pSpt->SetRenderState(LC_RS_ZBIAS, *(int*)&depth);

	LC_RECTF	rc;
	LCXVECTOR2	s;
	LCXVECTOR2	r;
	LCXVECTOR2	t;
	LCXCOLOR	d;


	rc = LC_RECTF(  0,   0, 128, 128);	LCXRect2Region(&rc); t = LCXVECTOR2(  10,  10);							m_pSpt->Draw(m_vTex[1], &rc, &t);
	rc = LC_RECTF(128,   0, 256, 128);	LCXRect2Region(&rc); t = LCXVECTOR2( 138,  10);	d = LCXCOLOR(1,0,0,1);	m_pSpt->Draw(m_vTex[2], &rc, &t, &d);
	rc = LC_RECTF(  0, 128, 128, 256);	LCXRect2Region(&rc); t = LCXVECTOR2(  10, 138);	d = LCXCOLOR(0,1,0,1);	m_pSpt->Draw(m_vTex[3], &rc, &t, &d);
	rc = LC_RECTF(128, 128, 256, 256);	LCXRect2Region(&rc); t = LCXVECTOR2( 138, 138);	d = LCXCOLOR(0,0,1,1);	m_pSpt->Draw(m_vTex[4], &rc, &t, &d);


	for(INT i=5; i<10; ++i)
	{
		FLOAT X = FLOAT(rand()%320)-60;
		FLOAT Y = FLOAT(rand()%240)-60;

		t = LCXVECTOR2(X, Y);
		m_pSpt->Draw(m_vTex[i], NULL, &t, NULL);
	}


	float fRadian = LCXToRadian(c*2.f);

	fRadian = LCXToRadian(c);

	rc  = LC_RECTF(0,0, m_vTex[9]->GetImgW(), m_vTex[9]->GetImgH());
	s	= LCXVECTOR2( -.5f, .5f);
	r	= LCXVECTOR2(100, 100);
	t	= LCXVECTOR2(100, 100);
	d	= LCXCOLOR(0,1,1,0.5f);

	m_pSpt->DrawEx(m_vTex[9], &rc, &t, &s, &r, fRadian, &d);


	// Sprite End
	m_pSpt->DrawFlush();
	m_pSpt->End();


	// Post Rendering...
	m_pDev->EndScene();


	return LC_OK;
}




// Main Program
int main(int argc, char* argv[])
{
	INT hr = 0;

	INT		i;
	UINT	dClearColor = 0xFF006699;

	PLC_TEXTURE	pTex = NULL;
	char	sFile[128]={0};
	UINT	colorKey = 0xFF00FFFF;


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


	sprintf(sFile, (char*)MEDIA_DIR"tex/bg512x256.png");
	hr = LcDev_CreateTextureFromFile(NULL, &pTex, (LC_HANDLE)sFile, &colorKey);
	if(LC_FAILED(hr))
		goto End;


	m_vTex.push_back(pTex);

	for(i=1;i<10; ++i)
	{
		sprintf(sFile, (char*)MEDIA_DIR"tex/t0%d.tga", i);
		hr = LcDev_CreateTextureFromFile(NULL, &pTex, (LC_HANDLE)sFile);
		if(LC_FAILED(hr))
			goto End;

		m_vTex.push_back(pTex);
	}

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
	// Release Texture
	for(vector<PLC_TEXTURE>::iterator t= m_vTex.begin(); t != m_vTex.end(); ++t)
		(*t)->Release();

	m_vTex.clear();

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

