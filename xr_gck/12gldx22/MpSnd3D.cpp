// Implementation of the CMpSnd3D class.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <d3dx9.h>
#include <dsound.h>

#include "LnxEntity.h"
#include "LnSmd.h"

#include "MpUtil.h"

#include "MpSnd.h"
#include "MpSnd3D.h"



CMpSnd3D::CMpSnd3D()
{
	m_pDsrc	= 0;
	m_pDlsn	= 0;
	m_pSnd	= 0;
}

CMpSnd3D::~CMpSnd3D()
{
	Destroy();
}



void CMpSnd3D::Destroy()
{
	CMpSnd*	pSnd = (CMpSnd*)m_pSnd;

	if(pSnd && pSnd->IsSoundPlaying())
		pSnd->Stop();
	
	if( m_pSnd )
	{
		delete m_pSnd;
		m_pSnd = NULL;
	}
	
	if( m_pDlsn )
	{
		m_pDlsn->Release();
		m_pDlsn = NULL;
	}
	
	if( m_pDsrc )
	{
		m_pDsrc->Release();
		m_pDsrc = NULL;
	}
}


INT CMpSnd3D::Query(char*, void*)
{
	return -1;
}


DWORD CMpSnd3D::GetType()
{
	return LC_SMD_WV_S;
}



void CMpSnd3D::Play()
{
	if(m_pSnd->GetStatus())
		m_pSnd->Stop();
	
	m_pSnd->Play();
}


void CMpSnd3D::Stop()
{
	m_pSnd->Stop();
}


void CMpSnd3D::Reset()
{
	m_pSnd->Reset();
}

void CMpSnd3D::Pause()
{
	m_pSnd->Pause();
}

void CMpSnd3D::SetVolume(LONG dVol)
{
	m_pSnd->SetVolume(dVol);
}

LONG CMpSnd3D::GetVolume()
{
	return m_pSnd->GetVolume();
}


void CMpSnd3D::SetRepeat(DWORD dRepeat)
{
	m_pSnd->SetRepeat(dRepeat);
}


DWORD CMpSnd3D::GetStatus()
{
	return m_pSnd->GetStatus();
}




void CMpSnd3D::SetPosSrc(const D3DXVECTOR3* vcP)
{
	m_pDsrc->SetPosition( vcP->x, vcP->y, vcP->z, DS3D_IMMEDIATE);
}

void CMpSnd3D::SetPosLsn(const D3DXVECTOR3* vcP)
{
	m_pDlsn->SetPosition( vcP->x, vcP->y, vcP->z, DS3D_IMMEDIATE);
}

D3DXVECTOR3 CMpSnd3D::GetPosSrc()
{
	D3DXVECTOR3	vcP;
	m_pDsrc->GetPosition( &vcP);
	return vcP;
}


D3DXVECTOR3 CMpSnd3D::GetPosLsn()
{
	D3DXVECTOR3	vcP;
	m_pDlsn->GetPosition( &vcP);

	return vcP;
}




//INT CMpSnd3D::Create(LPDIRECTSOUND8 pDS, TCHAR* sFile)
INT CMpSnd3D::Create(void* p1, void* p2, void*, void*)
{
	HRESULT hr=-1;

	LPDIRECTSOUND8	pDS		=(LPDIRECTSOUND8)p1;
	TCHAR*			sFile	=(TCHAR*)p2;

	
	//1. Create 3D Sound
//	if(FAILED(hr = SNDMN->CreateSound( &m_pSnd, sFile, DSBCAPS_CTRL3D, DS3DALG_HRTF_LIGHT )))
	
	if(FAILED(hr = LnDms_CreateSoundFromFile(&m_pSnd, pDS, sFile, DSBCAPS_CTRL3D, DS3DALG_NO_VIRTUALIZATION, 1 )))
	{
		MessageBox(NULL, "Create 3D Sound BufferInterface Failed.", "Err", MB_ICONERROR);
		return hr;
	}
	
	//2. Create 3D Buffer
	if( FAILED(hr = ((CMpSnd*)m_pSnd)->Get3DBufferInterface( 0, &m_pDsrc )))
	{
		MessageBox(NULL, "Get3DBufferInterface Failed", "Err", MB_ICONERROR);
		return hr;
	}

	
	//3. Create 3D Listener
	if( FAILED( hr = LnDms_Get3DListener(pDS, &m_pDlsn)))
	{
		MessageBox(NULL, "Initializing DirectSound Faild.", "Err", MB_ICONERROR);
		return hr;
	}
	
	
	memset(&m_DsSrc, 0, sizeof m_DsSrc);
	memset(&m_DsLsn, 0, sizeof m_DsLsn);

	m_DsSrc.dwSize			= sizeof m_DsSrc;
	m_DsSrc.dwMode			= DS3DMODE_NORMAL;
	m_DsSrc.flMinDistance	= 0.f;
	m_DsSrc.flMaxDistance	= 0.1f;

	m_pDsrc->SetAllParameters( &m_DsSrc, DS3D_IMMEDIATE );

	
	m_DsLsn.dwSize			= sizeof m_DsLsn;
	m_pDlsn->GetAllParameters(&m_DsLsn);

	m_DsLsn.vPosition.x		= 0.f;
	m_DsLsn.vPosition.y		= 0.f;
	m_DsLsn.vPosition.z		= 0.f;
	m_DsLsn.flDopplerFactor	= 0.F;
	m_DsLsn.flRolloffFactor	= 0.1F;
	m_DsLsn.flDistanceFactor= 0.F;						// 이것도 중요
	
	m_pDlsn->SetAllParameters(&m_DsLsn, DS3D_IMMEDIATE);
	
	return 0;
}



INT	LnDms_Create3DSoundFromFile(ILcSmd** ppSound, void* pL, char* sFile)
{
	LPDIRECTSOUND8 pDS = (LPDIRECTSOUND8) pL;
	(*ppSound) = new CMpSnd3D;
		
	if(FAILED( (*ppSound)->Create(pDS, sFile) ))
	{
		delete (*ppSound);
		(*ppSound) = NULL;
		return -1;
	}
	
	return 0;
}
