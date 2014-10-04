

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



LC_HWND					m_hWnd	= NULL;		// Window Handle
PLC_DEVICE				m_pDev	= NULL;		// Device
PLC_SPRITE				m_pSpt	= NULL;		// 2D Sprite

vector<PLC_TEXTURE  >	m_vTex;				// Texture list
vector<PLC_SPRITE_C >	m_vTdm;				// Sprite collector list


FLOAT GetAppFrame();


INT LC_CALLBACK Run(LC_HANDLE)
{
#if defined(_WIN32)
//	LcSys_Sleep(10);
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

	for(INT i=0; i<(int)m_vTdm.size(); ++i)
	{
		LCXMATRIX mtWld;
		mtWld.Identity();
		mtWld._41 = i* 100.0F;	// x
		mtWld._42 = i* 50.0F;	// y

		m_pSpt->SetTransform(LC_TS_WORLD, &mtWld);

		m_vTdm[i]->Draw();
	}


	m_pSpt->SetTransform(LC_TS_WORLD, NULL);
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

	PLC_TEXTURE	pTex = NULL;
	char		sFile[128]={0};
	UINT		colorKey = 0xFF00FFFF;
	INT		i=0;


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


	for(i=0;i<=5; ++i)
	{
		sprintf(sFile, (char*)MEDIA_DIR"tex/t0%d.tga", i);
		hr = LcDev_CreateTextureFromFile(NULL, &pTex, (LC_HANDLE)sFile);
		if(LC_FAILED(hr))
			goto End;

		m_vTex.push_back(pTex);

		PLC_SPRITE_C pSpc = NULL;
		hr = LcDev_CreateSpriteC(NULL, &pSpc, pTex);

		m_vTdm.push_back(pSpc);
	}

	printf("LcDev_CreateTextureFromFile Success\n");



	// collector test
	for(i=0; i< (int)m_vTex.size(); ++i)
	{
		PLC_SPRITE_C pSpc = NULL;
		hr = LcDev_CreateSpriteC(NULL, &pSpc, pTex);

		m_vTdm.push_back(pSpc);
	}

	printf("LcDev_CreateSpriteC Success\n");


	// test code for collector
	for(i=0;i< (int)m_vTdm.size(); ++i)
	{
		int maxRc = 50 + rand()%200;

		for(int j=0;j<maxRc; ++j)
		{
			float X = FLOAT(rand()%320)-80;
			float Y = FLOAT(rand()%240)-80;
			LCXVECTOR2 pos = LCXVECTOR2(X, Y);

			m_vTdm[i]->AddList(NULL, &pos, NULL, NULL, 0, NULL);
		}
	}



	// Run Program
	while(1)
	{
		if(LC_FAILED(LcSys_Run()))
			break;

		// Flipping
		m_pDev->Present(0,0,0,0);
	}


End:
	for(vector<PLC_SPRITE_C>::iterator t= m_vTdm.begin(); t != m_vTdm.end(); ++t)
		(*t)->Release();

	m_vTdm.clear();


	for(vector<PLC_TEXTURE>::iterator t = m_vTex.begin(); t != m_vTex.end(); ++t)
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
