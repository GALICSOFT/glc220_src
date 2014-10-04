

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
#include <LcMath.h>
#include <LcAux.h>
#include <LcStdev.h>


LC_HWND			m_hWnd	= NULL;		// Window Handle
PLC_DEVICE		m_pDev	= NULL;		// Device


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

	if(n>20)
	{
//		printf("%f\n", fFrame);
		n = 0;
	}


	// Volume Setting
	INT	volLeft	= 10 + rand()%100;
	INT	volRight= 10 + rand()%100;

	printf("Set Volume(Left, Right): %3d %3d   ", volLeft, volRight);

	LcStv_MixerWriteVolume(LC_MIXER_VOLUME, volLeft, volRight);

	volLeft	= 0;	volRight= 0;
	LcStv_MixerReadVolume(LC_MIXER_VOLUME, &volLeft, &volRight);

	printf("Get: %d %d\n", volLeft, volRight);



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
	UINT		dClearColor = 0xFF006699;


	INT	vol[2]={0};
	INT	volMax=0;
	INT	volMin=0;

	INT	volLeft=-200;
	INT	volRight=127;

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



	// Mixer Device 초기화
	if(LC_FAILED(LcStv_MixerOpen()))
		goto End;


	// Volume Range 얻기
	LcStv_MixerReadVolumeRange(vol);

	volMax=vol[0];
	volMin=vol[1];



	// Volume Setting
	LcStv_MixerWriteVolume(LC_MIXER_VOLUME, volLeft, volRight);



	// Run Program
	while(1)
	{
		if(LC_FAILED(LcSys_Run()))
			break;

		// Flipping
		m_pDev->Present(0,0,0,0);
	}


End:
	// Release Mixer Device
	LcStv_MixerClose();


	if(0 != m_pDev->Release())	// Release Device
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
