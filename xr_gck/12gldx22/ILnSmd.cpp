// Implementation of the ILcSmd class.
//
////////////////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT	0x400

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p){ if(p){ (p)->Release(); p=NULL;	}}
#endif

#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <dsound.h>

#include <d3dx9.h>

#include <LnUtil.h>

#include "LnxEntity.h"

#include "LnSmd.h"

#include "MpUtil.h"
#include "MpWaveFile.h"
#include "MpSnd.h"
#include "MpSnd3D.h"
#include "MpMusic.h"
#include "MpMp3.h"



static LPDIRECTSOUND8		m_pDs8_0f1b8a9b_6b1976c205bf	= NULL;				// DirectSound
static LPDIRECTMUSICLOADER	m_pDm8_0f1b8a9b_6b1976c205bf	= NULL;				// DieretMusic Loader

static HWND					m_hWnd_0f1b8a9b_6b1976c205bf	= NULL;				// Window Handle



void*	LcSmd_GetDirectSound()
{
	return m_pDs8_0f1b8a9b_6b1976c205bf;
}

void*	LcSmd_GetDirectMusicLoader()
{
	return m_pDs8_0f1b8a9b_6b1976c205bf;
}

HWND	LcSmd_GetHwnd()
{
	return m_hWnd_0f1b8a9b_6b1976c205bf;
}


INT LcSmd_DeviceInit(HWND hWnd)
{
	m_hWnd_0f1b8a9b_6b1976c205bf = hWnd;

	#if (_WIN32_WINNT >= 0x0400 )

	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
		return -1;

	#else

	if (FAILED(CoInitialize(NULL)))
		return -1;

	#endif



	if(FAILED(LnDms_DSoundCreate(m_pDs8_0f1b8a9b_6b1976c205bf, hWnd)))			// DirectSound8 积己
		return -2;

	if(FAILED(LnDms_DMusicLoaderCreate(m_pDm8_0f1b8a9b_6b1976c205bf, hWnd)))	// Direct Music Loader 积己
		return -3;

	
	if(FAILED(LnDms_DSound3DTest(hWnd, m_pDs8_0f1b8a9b_6b1976c205bf, NULL)))	// 3D 荤款靛 矫氰
		return -4;

	return 0;
}


void	LcSmd_DeviceClose()
{
	SAFE_RELEASE(	m_pDs8_0f1b8a9b_6b1976c205bf	);

	if(m_pDm8_0f1b8a9b_6b1976c205bf)
	{
		LnDms_DMusicLoaderDestroy(	m_pDm8_0f1b8a9b_6b1976c205bf	);
		m_pDm8_0f1b8a9b_6b1976c205bf = NULL;
	}

	
	CoUninitialize();
}



INT LcSmd_Create(char* sCmd
				, ILcSmd** pData
				, void* p1				// Wave File Name
				, void* p2
				, void* p3
				, void* p4
				, void* p5
				, void* p6
				, void* p7
				, void* p8
				)
{
	ILcSmd* pObj	= NULL;

	INT		nFile	= -1;
	char*	sFile	= (char*)p1;
	DWORD	dFMT	= (DWORD)p2;
	DWORD	dFlag	= (DWORD)p3;
	INT		nEvent	= (INT  )p4;
	void*	pDev	= (void*)p5;


	(*pData) = NULL;


	if(0==_stricmp("CE File", sCmd))
	{
		// p1: HWND
		// p2: File Name
		// p3: 
		pObj = new CMpSnd;

		if(FAILED(pObj->Create(p1, p2, p3, p4)))
		{
			delete pObj;
			return -1;
		}

		(*pData) = pObj;
		return 0;
	}




	// enum File
	char	sExt[32];
	LcFile_GetExt(sFile, sExt);

	if( 0 == _stricmp(sExt, ".wav"))
		nFile = 0;

	else if( 0 == _stricmp(sExt, ".mid"))
		nFile = 1;

	else if( 0== _stricmp(sExt, ".mp3"))
		nFile = 2;

	else if( 0== _stricmp(sExt, ".avi"))
		nFile = 3;

	
	if(LC_SMD_UNKNOWN == dFMT)
	{
		if(DSBCAPS_CTRL3D ==dFlag)
		{
			if(0 == nFile)
			{
				if(FAILED(LnDms_Create3DSoundFromFile( &pObj, m_pDs8_0f1b8a9b_6b1976c205bf, sFile)))
					return -1;
			}

			else if(1 == nFile)
			{
				if(FAILED(LnDms_Create3DSoundFromMusic( &pObj, m_pDm8_0f1b8a9b_6b1976c205bf, m_hWnd_0f1b8a9b_6b1976c205bf, sFile)))
					return -1;
			}
		}
		else
		{
			if(0 == nFile)
			{
				if(FAILED(LnDms_CreateSoundFromFile( &pObj, m_pDs8_0f1b8a9b_6b1976c205bf, sFile)))
					return -1;
			}
			
			else if(1 == nFile)
			{
				if(FAILED(LnDms_CreateMusicFromFile( &pObj, m_pDm8_0f1b8a9b_6b1976c205bf,m_hWnd_0f1b8a9b_6b1976c205bf, sFile)))
					return -1;
			}

			else if(2 == nFile)
			{
				if(FAILED(LnDms_CreateMp3FromFile(&pObj, m_hWnd_0f1b8a9b_6b1976c205bf, nEvent, sFile)))
					return -1;
			}

			else if(3 == nFile)
			{
				if(FAILED(LcSmd_CreateDShowFromFile(&pObj, pDev, nEvent, sFile)))
					return -1;
			}
		}
	}

	else
	{
		//	LC_SMD_UNKNOWN	= 0x0000,	// 0000 0000 0000	: Loader	Player
		//	
		//	LC_SMD_WV_D		= 0x0008,	// 0000 0000 1000	: Wave		Default
		//	LC_SMD_WV_M		= 0x0009,	// 0000 0000 1001	: Wave		Mono and Stero
		//	LC_SMD_WV_S		= 0x000C,	// 0000 0000 1100	: 3d		Sound
		//
		//	LC_SMD_MW_D		= 0x0080,	// 0000 1000 0000	: Music		Default
		//	LC_SMD_MW_M		= 0x0081,	// 0000 1000 0001	: Music		Wave
		//	LC_SMD_MW_S		= 0x0084,	// 0000 1000 0100	: Music		Wave 3D
		//
		//	LC_SMD_MC_M		= 0x0090,	// 0000 1001 0000	: Music		Mono and Stero
		//	LC_SMD_MC_S		= 0x00C0,	// 0000 1100 0000	: Music		3D
		//	
		//	LC_SMD_SH_D		= 0x0800,	// 1000 0000 0000	: DShow		Default
		//	LC_SMD_SH_M		= 0x0900,	// 1001 0000 0000	: DShow		Mp3
		//	LC_SMD_SH_A		= 0x0A00,	// 1010 0000 0000	: DShow		AVI

		if( LC_SMD_WV_D & dFMT)
		{
			if( LC_SMD_WV_M & dFMT)
			{
				if(FAILED(LnDms_CreateSoundFromFile( &pObj, m_pDs8_0f1b8a9b_6b1976c205bf, sFile)))
					return -1;
			}
			else if( DSBCAPS_CTRL3D ==dFlag)
			{
				if(FAILED(LnDms_Create3DSoundFromFile( &pObj, m_pDs8_0f1b8a9b_6b1976c205bf, sFile)))
					return -1;
			}
		}

		else if( LC_SMD_MW_D & dFMT)
		{
			if( LC_SMD_MC_M & dFMT || LC_SMD_MW_M & dFMT)
			{
				if(FAILED(LnDms_CreateMusicFromFile( &pObj, m_pDm8_0f1b8a9b_6b1976c205bf,m_hWnd_0f1b8a9b_6b1976c205bf, sFile)))
					return -1;
			}
			else if( LC_SMD_MC_S & dFMT || LC_SMD_MW_S & dFMT)
			{
				if(FAILED(LnDms_Create3DSoundFromMusic( &pObj, m_pDm8_0f1b8a9b_6b1976c205bf, m_hWnd_0f1b8a9b_6b1976c205bf, sFile)))
					return -1;
			}
		}

		else if( LC_SMD_SH_D & dFMT)
		{
			if( LC_SMD_SH_M & dFMT)
			{
				if(FAILED(LnDms_CreateMp3FromFile(&pObj, m_hWnd_0f1b8a9b_6b1976c205bf, nEvent, sFile)))
					return -1;
			}
			else if( LC_SMD_SH_A & dFMT)
			{
				if(FAILED(LcSmd_CreateDShowFromFile(&pObj, pDev, nEvent, sFile)))
					return -1;
			}
		}
	}


	if(!pData)
		return -1;


	*pData = pObj;
	return 0;
}



