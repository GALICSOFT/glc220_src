

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

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>
#include <LcxEntity.h>
#include <LctEntity.h>


#define MEDIA_DIR	"../../media/"
#define GAME_MAX_FONT	10


LC_HWND		m_hWnd	= NULL;		// Window Handle
PLC_DEVICE	m_pDev	= NULL;		// Device
PLC_SPRITE	m_pSpt	= NULL;		// 2D Sprite

PLC_FONT	m_pFont	= NULL;		// font

std::vector<PLC_FONT > m_vFont;



FLOAT GetAppFrame();



// Main Loop
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


	int i;
	char str[128]={0};


	LCXVECTOR2	vcPos(15, 5);
	LCXCOLOR	color(1, 0, 1, 1);

	sprintf(str, "갈릭 Default Font ABCDEF abcdef @!#$ 12345678");
	m_pFont->SetPosition(&vcPos);
	m_pFont->SetColor(&color);
	m_pFont->SetString(str);

	

	// Set string to Font
	color= LCXCOLOR(1, 1, 0, 1);
	for(i=0; i<(int)m_vFont.size(); ++i)
	{
		PLC_FONT  pFont = m_vFont[i];
		sprintf(str, "GLC 폰트: %d, Count: %d, Frame: %.f", i+9, c, fFrame);
		pFont->SetString(str);
	}


	// Setting position and color
	int iSize = m_vFont.size(); 
	for(i=0; i<iSize; ++i)
	{
		PLC_FONT  pFont = m_vFont[i];
		LCXVECTOR2 vcPos(5.0F,55.0F+ 25*i);

		pFont->SetPosition(&vcPos);
		pFont->SetColor(&LCXCOLOR(1, 1, 0, 1));
	}



	// Prepare Rendering...
	m_pDev->BeginScene();

	// Clear Backbuffer
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);


	// Sprite Begin
	m_pSpt->Begin();


	m_pFont->DrawTxt();


	// Draw buffer
	for(i=0; i<(int)m_vFont.size(); ++i)
	{
		PLC_FONT  pFont = m_vFont[i];
		pFont->DrawTxt();
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
	int		hr = 0;
	int		i;


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
	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, (UINT)0xFF000000);



	// Create 2D Rendering Sprite
	if(LC_FAILED(LcDev_CreateSprite(NULL, &m_pSpt)))
		goto End;

	printf("LcDev_CreateSprite Success\n");

	LcVar_WriteP(LC_VAR_SPRITE0, m_pSpt);


	// Create Default Font
	{
		TLC_FONT	tFont(LCCMD_FONT_DEFAULT, NULL, NULL, 14);
		if(LC_FAILED(LcDev_CreateFontFromMemory(NULL, &m_pFont, m_pSpt, &tFont)))
			goto End;

	}

	// Create font from file
	for(i=0; i<GAME_MAX_FONT; ++i)
	{
		PLC_FONT	pFont;
		TLC_FONT	tFont((char*)MEDIA_DIR"font/mwg_b.otf", 9+i);

		if(LC_FAILED(LcDev_CreateFontFromFile(NULL, &pFont, m_pSpt, &tFont)))
			goto End;

		m_vFont.push_back(pFont);
	}




	// Run Program
	while(1)
	{
		if(LC_FAILED(LcSys_Run()))
			break;

		// Flipping
		m_pDev->Present(0,0,0,0);
	}


//	free(buf);

End:

	SAFE_RELEASE(	m_pFont	);

	// Release font Instance
	for(vector<PLC_FONT>::iterator t= m_vFont.begin(); t != m_vFont.end(); ++t)
		(*t)->Release();

	m_vFont.clear();



	// Release Sprite
	SAFE_RELEASE(	m_pSpt	);


	// Release Device
	hr  = m_pDev->Release();

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
	static INT		MaxCount = 10;
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

