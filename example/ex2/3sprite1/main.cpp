

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

PLC_TEXTURE	m_pTex = NULL;		// Texture List


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


	m_pTex->Query("Invalid", NULL);
	m_pTex->Query("Restore", NULL);



	LCXCOLOR r(0,0,1,1);
	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, &r);

	// Prepare Rendering...
	m_pDev->BeginScene();

	// Clear Backbuffer
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);

	LCXMATRIX mtScl;
	mtScl.Identity();
	mtScl.SetupScaling(2,2,1);

	mtScl._41 = 100;
	//mtScl._42 = 0;

	// Sprite Begin
	m_pSpt->Begin();

	m_pSpt->SetTransform(LC_TS_WORLD, &mtScl);


	LCXMATRIX mtWld;
	LCXMATRIX mtViw;
	LCXMATRIX mtPrj;
	LCXMATRIX mtWVP;

	m_pSpt->GetTransform(LC_TS_WORLD, &mtWld);
	m_pSpt->GetTransform(LC_TS_VIEW, &mtViw);
	m_pSpt->GetTransform(LC_TS_PROJ, &mtPrj);

	mtWVP = mtWld * mtViw * mtPrj;

	LCXVECTOR3 vcPos0(  0,   0, 0);
	LCXVECTOR3 vcPos1(480,   0, 0);
	LCXVECTOR3 vcPos2(480, 320, 0);
	LCXVECTOR3 vcPos3(  0, 320, 0);

	mtWVP.TransformCoord(&vcPos0, &vcPos0);
	mtWVP.TransformCoord(&vcPos1, &vcPos1);
	mtWVP.TransformCoord(&vcPos2, &vcPos2);
	mtWVP.TransformCoord(&vcPos3, &vcPos3);


	LCXVECTOR2	vcTrn(20, 25);
	LCXCOLOR	color(1,1, 0, 1.0F);
	LCXRECT		rc(50,50,300,300);

	m_pSpt->Draw(m_pTex, &rc, &vcTrn, &color);


	// Sprite End
	m_pSpt->End();


	// Post Rendering...
	m_pDev->EndScene();


	return LC_OK;
}




// Main Program
int main(int argc, char* argv[])
{
	INT	hr = 0;


	char sFile [] = MEDIA_DIR"tex/bg256x256.bmp";

	UINT dColorKey = 0xFFFF0000;
	UINT Use16 = LC_FMT_INDEX16;


	UINT		len = 0;
	LC_HANDLE	Mem = NULL;

	FILE*	fp = fopen(sFile, "rb");

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);

	Mem = LcMem_Allock(len, 1);

	fseek(fp, 0, SEEK_SET);
	fread(Mem, 1, len, fp);
	fclose(fp);



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


	// Create 2D Rendering Sprite
	if(LC_FAILED(LcDev_CreateSprite(NULL, &m_pSpt)))
		goto End;

	LcVar_WriteP(LC_VAR_SPRITE0, m_pSpt);


	if(LC_FAILED(LcDev_CreateTextureFromMemory(NULL, &m_pTex, Mem, &len, sFile, &dColorKey, NULL, &Use16)))
		goto End;


	SAFE_FREE(	Mem	);


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
	//LC_DestroyTexture(&m_pTex);
	//LcSys_DestroyObject((PLC_OBJECT*)&m_pTex);
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
	static DOUBLE	dBgn = ::LcSys_GetTickCount();
	DOUBLE	dCur = ::LcSys_GetTickCount();  // GetTickCount()

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
