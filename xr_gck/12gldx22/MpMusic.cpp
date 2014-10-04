// Implementation of the CMpMusic class.
//
////////////////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT	0x400

#include <dmusicc.h>			// DirectMusic includes
#include <dmusici.h>
#include <cguid.h>                 // for GUID_NULL

#include <windows.h>
#include <d3dx9math.h>

#include "LnxEntity.h"
#include "LnSmd.h"

#include "MpUtil.h"

#include "MpMusic.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)	{	if(p){(p)->Release();p = NULL;	}}
#endif


CMpMusic::CMpMusic()
: m_pDLoader(0)
, m_pOdioPth(0)
, m_pPerform(0)
, m_pSegment(0)
, m_mtPause(0)
, m_dVolume(MEDIA_MAX_VOLUME)
{
}


CMpMusic::CMpMusic(	LPDIRECTMUSICLOADER			_pDLoader
				,	LPDIRECTMUSICAUDIOPATH		_pOdioPth
				,	LPDIRECTMUSICPERFORMANCE	_pPerform
				,	LPDIRECTMUSICSEGMENT		_pSegment)
: m_pDLoader(_pDLoader)
, m_pOdioPth(_pOdioPth)
, m_pPerform(_pPerform)
, m_pSegment(_pSegment)
, m_mtPause(0)
, m_dVolume(MEDIA_MAX_VOLUME)
{
}


CMpMusic::~CMpMusic()
{
	Destroy();
}


void CMpMusic::Destroy()
{
	Stop();
	
	SAFE_RELEASE(	m_pSegment	);
	SAFE_RELEASE(	m_pOdioPth	);

	if(m_pPerform)
		m_pPerform->Stop(NULL, NULL, 0, 0);

	SAFE_RELEASE(	m_pPerform	);
}


DWORD CMpMusic::GetType()
{
	return LC_SMD_MC_M;
}



void CMpMusic::Play()
{
	m_pSegment->SetStartPoint(m_mtPause);
	m_pPerform->PlaySegmentEx(m_pSegment, NULL, NULL, m_pOdioPth? DMUS_SEGF_DEFAULT : 0, 0, NULL, NULL, m_pOdioPth);
}

void CMpMusic::Stop()
{
	if(S_OK == m_pPerform->IsPlaying(m_pSegment, NULL))
		m_pPerform->StopEx(m_pSegment, 0, 0);
	
	m_mtPause	= 0;
}

void CMpMusic::Reset()
{
	m_mtPause	= 0;
}


void CMpMusic::Pause()
{
	if(S_OK == m_pPerform->IsPlaying(m_pSegment, NULL))
	{
		m_mtPause = LnDms_GetTimeOffset(m_pPerform, m_pSegment);
		m_pPerform->StopEx(m_pSegment, 0, 0);
	}
}

void CMpMusic::SetVolume(LONG dVol)
{
	if(dVol<0)
		dVol = 0;

	if(dVol>MEDIA_MAX_VOLUME)
		dVol = MEDIA_MAX_VOLUME;

	LONG lVolume = dVol - MEDIA_MAX_VOLUME;

	lVolume = LONG(lVolume*FLOAT(MEDIA_MUSIC_MAX_VOLUME)/FLOAT(MEDIA_MAX_VOLUME));

	if(SUCCEEDED(m_pOdioPth->SetVolume(lVolume, 0)))
	{
		m_dVolume = dVol;
	}
}

LONG CMpMusic::GetVolume()
{
	return m_dVolume;
}

void CMpMusic::SetRepeat(DWORD dRepeat)
{
	m_pSegment->SetRepeats(dRepeat);
}



DWORD CMpMusic::GetStatus()
{
	if(NULL == m_pSegment || NULL == m_pPerform)
		return 0;
	
	return (S_OK == m_pPerform->IsPlaying( m_pSegment, NULL))? 1: 0;
}




INT CMpMusic::Query(char*, void*)
{
	return -1;
}



LPDMA CMpMusic::GetAudioPath() const
{
	return m_pOdioPth;
}



INT CMpMusic::Create(void*, void*, void*, void*)
{
	return 0;
}





////////////////////////////////////////////////////////////////////////////////
//
// Implementation of the CMpMusic3D class.
//
////////////////////////////////////////////////////////////////////////////////


CMpMusic3D::CMpMusic3D()
{
	m_pDsrc		= NULL;
	m_pDlsn		= NULL;
	m_pMusic	= NULL;
}


CMpMusic3D::~CMpMusic3D()
{
	Destroy();
}


void CMpMusic3D::Destroy()
{
	if(m_pMusic)
	{
		delete m_pMusic;
		m_pMusic = NULL;
	}

	m_pDsrc		= NULL;
	m_pDlsn		= NULL;
	m_pMusic	= NULL;
}


DWORD CMpMusic3D::GetType()
{
	return LC_SMD_MC_S;
}


void CMpMusic3D::Play()
{
	m_pMusic->Play();
}

void CMpMusic3D::Stop()
{
	m_pMusic->Stop();
}

void CMpMusic3D::Reset()
{
	m_pMusic->Reset();
}

void CMpMusic3D::Pause()
{
	m_pMusic->Pause();
}



void CMpMusic3D::SetVolume(LONG dVol)
{
	m_pMusic->SetVolume(dVol);
}

LONG CMpMusic3D::GetVolume()
{
	return m_pMusic->GetVolume();
}

void CMpMusic3D::SetRepeat(DWORD dRepeat)
{
	m_pMusic->SetRepeat(dRepeat);
}


DWORD CMpMusic3D::GetStatus()
{
	return m_pMusic->GetStatus();
}


INT CMpMusic3D::Query(char*, void*)
{
	return -1;
}



//INT CMpMusic3D::Create(LPDIRECTMUSICLOADER pLoader, HWND hWnd, char* sFile)
INT CMpMusic3D::Create(void* p1, void* p2, void* p3, void* p4)
{
	HRESULT hr;

	LPDIRECTMUSICLOADER pLoader	= (LPDIRECTMUSICLOADER)p1;
	HWND				hWnd	= (HWND)p2;
	char*				sFile	= (char*)p3;

	ILcSmd*	pMusic = NULL;

	if(FAILED(LnDms_CreateMusicFromFile(&pMusic, pLoader, hWnd, sFile, DMUS_APATH_DYNAMIC_3D)))
	{
		MessageBox(hWnd, "Create Music From File Failed.", "Err", MB_ICONEXCLAMATION);
		return -1;
	}

	this->m_pMusic = pMusic;


	// 3D buffer
	LPDMA pOdioPth = ((CMpMusic*)m_pMusic)->GetAudioPath();

	if(FAILED(hr = pOdioPth->GetObjectInPath(0, DMUS_PATH_BUFFER, 0, GUID_NULL, 0, IID_IDirectSound3DBuffer, (void**)&m_pDsrc)))
		return -1;
	
	// listener
	if(FAILED(hr = pOdioPth->GetObjectInPath(0, DMUS_PATH_PRIMARY_BUFFER, 0, GUID_NULL, 0, IID_IDirectSound3DListener, (void**)&m_pDlsn)))
		return -1;

	
	// 3D buffer parameters
	memset(&m_DsSrc, 0, sizeof m_DsSrc);
	memset(&m_DsLsn, 0, sizeof m_DsLsn);

	m_DsSrc.dwSize				= sizeof m_DsSrc;
	m_DsSrc.dwMode				= DS3DMODE_NORMAL;		// 절대적인 리쓰너. 이게 필요하다.
	m_DsSrc.flMinDistance		= 0.f;
	m_DsSrc.flMaxDistance		= 50.f;
	m_pDsrc->SetAllParameters(&m_DsSrc, DS3D_IMMEDIATE);


	m_DsLsn.dwSize = sizeof m_DsLsn;
	m_pDlsn->GetAllParameters(&m_DsLsn);

	m_DsLsn.vPosition.x			= 0.f;
	m_DsLsn.vPosition.y			= 0.f;
	m_DsLsn.vPosition.z			= 0.f;
	m_DsLsn.flDopplerFactor		= 0.F;
	m_DsLsn.flRolloffFactor		= 0.1F;
	m_DsLsn.flDistanceFactor	= 0.F;						// 이것도 중요
	m_pDlsn->SetAllParameters(&m_DsLsn, DS3D_IMMEDIATE);

	return 0;
}



void CMpMusic3D::SetPosSrc(const D3DXVECTOR3* vcP)
{
	m_pDsrc->SetPosition( vcP->x, vcP->y, vcP->z, DS3D_IMMEDIATE);
}

void CMpMusic3D::SetPosLsn(const D3DXVECTOR3* vcP)
{
	m_pDlsn->SetPosition( vcP->x, vcP->y, vcP->z, DS3D_IMMEDIATE);
}

D3DXVECTOR3 CMpMusic3D::GetPosSrc()
{
	D3DXVECTOR3	vcP;
	m_pDsrc->GetPosition( &vcP);
	return vcP;
}


D3DXVECTOR3 CMpMusic3D::GetPosLsn()
{
	D3DXVECTOR3	vcP;
	m_pDlsn->GetPosition( &vcP);

	return vcP;
}


//void CMpMusic3D::Set3DSoundPosListener(float x, float y, float z)
//{
//	if(m_pDlsn)
//	{
//		// get the listener parameters
//		m_DsLsn.dwSize = sizeof(DS3DLISTENER);
//		m_pDlsn->GetAllParameters(&m_DsLsn);
//		
//		// set position of listener
//		m_DsLsn.vPosition.x = x;
//		m_DsLsn.vPosition.y = y;
//		m_DsLsn.vPosition.z = z;
//		m_pDlsn->SetAllParameters(&m_DsLsn, DS3D_IMMEDIATE);
////		m_pDlsn->SetPosition(x,y,z, DS3D_IMMEDIATE);
//	}
//}







void LnDms_DMusicLoaderDestroy(LPDML& pMusicLoader)
{
	if(pMusicLoader)
	{
		pMusicLoader->Release();
		pMusicLoader = NULL;
	}
}


INT LnDms_DMusicLoaderCreate(LPDML& pMusicLoader, HWND hWnd)
{
	char sPath[MAX_PATH];       // path for audio file
	WCHAR wsFile[MAX_PATH];
	

	// create the loader object
	if(FAILED(CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, IID_IDirectMusicLoader8, (void**)&pMusicLoader)))
	{
		pMusicLoader = NULL;
		MessageBox(hWnd, "CoCreate Instance Failed.\nWe need CoInitializeEx", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	

	// retrieve the current directory
	GetCurrentDirectory(MAX_PATH, sPath);
	MultiByteToWideChar(CP_ACP, 0, sPath, -1, wsFile, MAX_PATH);

	pMusicLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wsFile, FALSE);



	LPDMP pPerformTst= NULL;			// For Test
		

	// Create IDirectMusicPerformance8 for Test
	if(FAILED(CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, (void**)&pPerformTst)))
	{
		MessageBox(hWnd, "CoCreate Instance Failed.", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	

	if(FAILED(pPerformTst->InitAudio(NULL, NULL, hWnd, DMUS_APATH_DYNAMIC_STEREO, 128, DMUS_AUDIOF_ALL, NULL)))
	{
		MessageBox(hWnd, "Init Audio Failed.", "Err", MB_ICONEXCLAMATION);
		return -1;
	}

	if(pPerformTst)
	{
		pPerformTst->Stop(NULL, NULL, 0, 0);
		pPerformTst->CloseDown();
		pPerformTst->Release();
		pPerformTst = NULL;
	}


	return 0;
}





INT LnDms_Create3DSoundFromMusic(ILcSmd** pSnd3D, void* pL, HWND hWnd, char* sFile)
{
	LPDIRECTMUSICLOADER pLoader = (LPDIRECTMUSICLOADER)pL;
	(*pSnd3D) = new CMpMusic3D;
		
	if(FAILED( (*pSnd3D)->Create(pLoader, hWnd, sFile) ))
	{
		delete (*pSnd3D);
		(*pSnd3D) = NULL;
		return -1;
	}
	
	return 0;
}


// Load the Segment From File
INT LnDms_CreateMusicFromFile(ILcSmd** pMusic, void* pL, HWND hWnd, char* sFile, DWORD dwDefaultPathType)
{
	HRESULT	hr;
	
	LPDIRECTMUSICLOADER			pLoader = (LPDIRECTMUSICLOADER)pL;
	LPDIRECTMUSICSEGMENT		pSegment	= NULL;								// the Segment
	LPDIRECTMUSICPERFORMANCE	pPerform	= NULL;								// the Performance
	LPDIRECTMUSICAUDIOPATH		pOdioPth	= NULL;								// Audio path

	if(FAILED(CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, (void**)&pPerform)))
	{
		MessageBox(hWnd, "CoCreate Instance Failed.\nWe need CoInitializeEx", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	
	// General	Midi type: DMUS_APATH_SHARED_STEREOPLUSREVERB
	// 3D		Midi type: DMUS_APATH_DYNAMIC_3D
	if(FAILED(pPerform->InitAudio(NULL, NULL, hWnd, dwDefaultPathType, 128, DMUS_AUDIOF_ALL, NULL)))
	{
		MessageBox(hWnd, "Init Audio Failed.", "Err", MB_ICONEXCLAMATION);
		return -1;
	}

	// Audio Path는 3D 사운드와 볼륨 조절에서 반드시 필요하다.
	if(FAILED(hr = pPerform->CreateStandardAudioPath(dwDefaultPathType, 128, TRUE, &pOdioPth)))
	{
		MessageBox(hWnd, "Create Standard Audio Path Failed.", "Err", MB_ICONEXCLAMATION);
		return -1;
	}



	// Load Music from File
	{
		WCHAR	wsFile[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, sFile, -1, wsFile, MAX_PATH);			// convert sFile to unicode string
		
		// Load from File
		if(FAILED(hr = pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,	IID_IDirectMusicSegment8,	wsFile,	(void**)&pSegment)))
		{
			MessageBox(hWnd, "Load Object From File Failed.", "Err", MB_ICONEXCLAMATION);
			return -1;
		}
	}
	
	pSegment->Download(pPerform);												// download the segment's instruments to the synthesizer
	
	(*pMusic) = new CMpMusic( pLoader, pOdioPth, pPerform, pSegment);
	
	return 0;
}




// Load the Segment From Resource
INT LnDms_CreateMusicFromRsc(ILcSmd** pMusic, void* pL, HWND hWnd, INT nResourceID, char* sResourceType, DWORD dwDefaultPathType)
{
	HRESULT	hr;

	LPDIRECTMUSICLOADER			pLoader = (LPDIRECTMUSICLOADER)pL;
	LPDIRECTMUSICSEGMENT		pSegment	= NULL;								// the Segment
	LPDIRECTMUSICPERFORMANCE	pPerform	= NULL;								// the Performance
	LPDIRECTMUSICAUDIOPATH		pOdioPth	= NULL;								// Audio path

	if(FAILED(CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, (void**)&pPerform)))
	{
		MessageBox(hWnd, "CoCreate Instance Failed.\nWe need CoInitializeEx", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	
	if(FAILED(pPerform->InitAudio(NULL, NULL, hWnd, dwDefaultPathType, 128, DMUS_AUDIOF_ALL, NULL)))
	{
		MessageBox(hWnd, "Init Audio Failed.", "Err", MB_ICONEXCLAMATION);
		return -1;
	}

	// Audio Path는 3D 사운드와 볼륨 조절에서 반드시 필요하다.
	if(FAILED(hr = pPerform->CreateStandardAudioPath(dwDefaultPathType, 128, TRUE, &pOdioPth)))
	{
		MessageBox(hWnd, "Create Standard Audio Path Failed.", "Err", MB_ICONEXCLAMATION);
		return -1;
	}




	// Load Music from Resource
	{
		HINSTANCE				hInst		= GetModuleHandle(NULL);
		HRSRC					hres		= NULL;
		void*					pMem		= NULL;
		DWORD					dwSize		= 0;
		DMUS_OBJECTDESC			objdesc;
		
		
		// Find the resource 
		if( !(hres = FindResource( hInst,MAKEINTRESOURCE(nResourceID), sResourceType )))
			return -1;
		
		
		// Load the resource
		if( !(pMem = (void*)LoadResource(hInst, hres)) ) 
			return -1;
		
		
		// Memory Size of the resource
		dwSize = SizeofResource(hInst,hres);
		
		// Set up object description 
		ZeroMemory(&objdesc,sizeof(DMUS_OBJECTDESC));
		objdesc.dwSize = sizeof(DMUS_OBJECTDESC);
		objdesc.dwValidData = DMUS_OBJ_MEMORY | DMUS_OBJ_CLASS;
		objdesc.guidClass = CLSID_DirectMusicSegment;
		objdesc.llMemLength =(LONGLONG)dwSize;
		objdesc.pbMemData = (BYTE*)pMem;
		
		if(FAILED( hr = pLoader->GetObject( &objdesc, IID_IDirectMusicSegment8, (void**)&pSegment )))
		{
			MessageBox(hWnd, "Loader GetObject Failed.", "Err", MB_ICONEXCLAMATION);
			return -1;
		}
	}
	

	pSegment->Download(pPerform);
	
	(*pMusic)	= new CMpMusic( pLoader, pOdioPth, pPerform, pSegment);
	
	return 0;
}






// From http://www.GameDev.net

LONG LnDms_GetTimeOffset(LPDIRECTMUSICPERFORMANCE pPerform, LPDIRECTMUSICSEGMENT pSegment)
 {

	IDirectMusicSegmentState*	pSegState = NULL;
	MUSIC_TIME mtNow;           // From GetTime
	MUSIC_TIME mtStartTime;     // From GetStartTime
	MUSIC_TIME mtStartPoint;    // From GetStartPoint
	MUSIC_TIME mtLoopStart;     // From GetLoopPoints
	MUSIC_TIME mtLoopEnd;       // From GetLoopPoints
	MUSIC_TIME mtLength;        // From GetLength
	DWORD dwLoopRepeats;        // From GetRepeats
	
//	pSegment->GetTime		(&mtNow);
//	pSegment->GetStartTime	(&mtStartTime);
//	pSegment->GetStartPoint	(&mtStartPoint);
//	pSegment->GetLoopPoints	(&mtLoopStart, &mtLoopEnd);
//	pSegment->GetLength		(&mtLength);
//	pSegment->GetRepeats	(&dwLoopRepeats);

	pPerform ->GetTime(NULL, &mtNow);
	pPerform ->GetSegmentState(&pSegState, mtNow);
	pSegState->GetStartPoint(&mtStartPoint);
	pSegState->GetStartTime(&mtStartTime);
	pSegment ->GetLoopPoints(&mtLoopStart, &mtLoopEnd);
	pSegment ->GetRepeats(&dwLoopRepeats);
	
//	if(thisAudio->audioType == WAV)
//		pSegment->SetLength(mtNow);

	pSegment->GetLength(&mtLength);
	
	// Convert mtNow from absolute time to an offset
	// from when the segment started playing
	LONGLONG llOffset = mtNow - (mtStartTime - mtStartPoint);
	
	// If loopEnd is non zero, set lLoopEnd to loopEnd,
	// otherwise use the segment length
	LONGLONG llLoopEnd = mtLoopEnd ? mtLoopEnd : mtLength;
	LONGLONG llLoopStart = mtLoopStart;
	
	if((dwLoopRepeats != 0) && (llLoopStart < llLoopEnd) && (llLoopEnd > mtStartPoint))
	{
		if((dwLoopRepeats != DMUS_SEG_REPEAT_INFINITE) && (llOffset > (llLoopStart + (llLoopEnd - llLoopStart) * (signed)dwLoopRepeats)))
		{
			llOffset -= (llLoopEnd - llLoopStart) * dwLoopRepeats;
		}
		else if( llOffset > llLoopStart )
		{
			llOffset = llLoopStart + (llOffset - llLoopStart) % (llLoopEnd - llLoopStart);
		}
	}
	
	llOffset = min( llOffset, LONG_MAX );	// LONG_MAX is defined in Limits.h.
	
	return LONG(llOffset);
}