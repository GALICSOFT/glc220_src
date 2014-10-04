

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
#include <LcMath.h>
#include <LcAux.h>


LC_HWND			m_hWnd = NULL;		// Window Handle
PLC_DEVICE		m_pDev = NULL;		// Device



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

	static LCXCOLOR ClearColor(1,1,1,1);

	static int n=0;
	++n;

	if(n>60)
	{
		printf("%f\n", fFrame);
		n = 0;

		ClearColor.r = (LcSys_Rand()%156 + 100)/255.0f;
		ClearColor.g = (LcSys_Rand()%156 + 100)/255.0f;
		ClearColor.b = (LcSys_Rand()%156 + 100)/255.0f;

		// Set Clear Color
		UINT dColor = ClearColor;
		m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, dColor);
		m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);
	}


	// Prepare Rendering...
	m_pDev->BeginScene();

	// Clear Backbuffer
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);


	// Post Rendering...
	m_pDev->EndScene();

	return LC_OK;
}


// Main Program
int main(int argc, char* argv[])
{
	int hr;
	// Initialize GLC Library.
	LcSys_Init();

	printf("LcSys_Init\n");

	// Set Main procedure Function Pointer
	LcSys_SetMainRunFunc(Run);

	// Create Window
	m_hWnd = LcSys_CreateWindow(NULL, (char*)"LC_Tutorial", 480, 320);
	if(NULL == m_hWnd)
	{
		printf("LcSys_CreateWindow Failed\n");
		return LC_EFAIL;
	}

	// Create GLC Rendering Device
	if(LC_FAILED(LcDev_CreateDevice(NULL, &m_pDev, m_hWnd, NULL)))
	{
		printf("LcDev_CreateDevice Failed\n");
		return LC_EFAIL;
	}




	// Run Program
	while(1)
	{
		if(LC_FAILED(LcSys_Run()))
			break;

		// Flipping
		m_pDev->Present(0,0,0,0);
	}

	// Release Device
	hr = m_pDev->Release();
	m_pDev = NULL;

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



