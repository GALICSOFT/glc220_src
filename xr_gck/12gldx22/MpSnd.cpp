// Implementation of the CMpSnd class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <windows.h>
#include <mmsystem.h>
#include <dxerr9.h>
#include <d3dx9math.h>
#include <dsound.h>

#include "LnxEntity.h"
#include "LnSmd.h"

#include "MpUtil.h"

#include "MpWaveFile.h"
#include "MpSnd.h"


#define SAFE_DELETE(p)			{ if(p){ delete (p);	(p) = NULL; }}
#define SAFE_DELETE_ARRAY(p)	{ if(p){ delete [] (p);	(p) = NULL; }}
#define SAFE_RELEASE(p)			{ if(p){ (p)->Release();	(p) = NULL; }}


CMpSnd::CMpSnd()
{
	m_dwFlags = 0;
	m_dwPrior = 0;
	m_lVolume = 0;
	m_lHz = -1;
	m_lPan = 0;
}



CMpSnd::CMpSnd( LPDSB* apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, void* pWaveFile, DWORD dwCreationFlags )
{
	m_dwFlags = 0;
	m_dwPrior = 0;
	m_lVolume = 0;
	m_lHz = -1;
	m_lPan = 0;

	DWORD i;
	
	m_pBuffer = new LPDSB[dwNumBuffers];
	
	if( NULL != m_pBuffer )
	{
		for( i=0; i<dwNumBuffers; i++ )
			m_pBuffer[i] = apDSBuffer[i];
		
		m_dBufSize	= dwDSBufferSize;
		m_dNbuff	= dwNumBuffers;
		m_pWaveFile	= pWaveFile;
		m_dInitFlag	= dwCreationFlags;
		
		FillBufferWithSound( m_pBuffer[0], FALSE );
	}
}


// Destroys the class
CMpSnd::~CMpSnd()
{
	Destroy();
}



void CMpSnd::Destroy()
{	
	if(IsSoundPlaying())
	{
		Stop();
	}

	for( DWORD i=0; i<m_dNbuff; i++ )
	{
		m_pBuffer[i]->Release();
	}
	
	SAFE_DELETE_ARRAY( m_pBuffer ); 

	CMpWaveFile* pWaveFile = (CMpWaveFile*)m_pWaveFile;
	SAFE_DELETE( pWaveFile );
	m_pWaveFile = NULL;
}



DWORD CMpSnd::GetType()
{
	return LC_SMD_WV_M;
}


// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound

void CMpSnd::Play()
{
	HRESULT hr;
	BOOL    bRestored;

//	if( m_pBuffer == NULL )
//		return CO_E_NOTINITIALIZED;
	
	if( m_pBuffer == NULL )
		return;
	
	LPDSB pDSB = GetFreeBuffer();
	
	if( pDSB == NULL )
		return;
	
	// Restore the buffer if it was lost
	if( FAILED( hr = RestoreBuffer( pDSB, &bRestored ) ) )
	{
		MessageBox( GetActiveWindow(), "Sound RestoreBuffer Failed", "Err", MB_ICONEXCLAMATION);
		return;
	}
	
	if( bRestored )
	{
		// The buffer was restored, so we need to fill it with new data
		if( FAILED( hr = FillBufferWithSound( pDSB, FALSE )))
		{
			MessageBox( GetActiveWindow(), "Sound FillBufferWithSound Failed.", "Err", MB_ICONEXCLAMATION);
			return;
		}
	}
	
	if( m_dInitFlag & DSBCAPS_CTRLVOLUME )
	{
		pDSB->SetVolume( m_lVolume );
	}
	
	if( m_lHz != -1 && (m_dInitFlag & DSBCAPS_CTRLFREQUENCY) )
	{
		pDSB->SetFrequency( m_lHz );
	}
	
	if( m_dInitFlag & DSBCAPS_CTRLPAN )
	{
		pDSB->SetPan( m_lPan );
	}
	
	if(FAILED(pDSB->Play( 0, m_dwPrior, m_dwFlags)))
		MessageBox( GetActiveWindow(), "pDSB->Play() Failed", "Err", MB_ICONEXCLAMATION);

}



void CMpSnd::Stop()
{
	Pause();
	Reset();
}


void CMpSnd::Reset()
{
	HRESULT hr = 0;
	
	for( DWORD i=0; i<m_dNbuff; i++ )
		hr |= m_pBuffer[i]->SetCurrentPosition( 0 );
}


void CMpSnd::Pause()
{
	HRESULT hr = 0;
	
	for( DWORD i=0; i<m_dNbuff; ++i)
		hr |= m_pBuffer[i]->Stop();
}


void CMpSnd::SetVolume(LONG dVol)
{
	if(dVol<0 || dVol>MEDIA_MAX_VOLUME)
		return ;

	LONG lVolume = dVol - MEDIA_MAX_VOLUME;

	LPDSB pDSB = GetFreeBuffer();
	
	if( NULL == pDSB)
		return;
	
	
	if( m_dInitFlag & DSBCAPS_CTRLVOLUME )
	{
		if(SUCCEEDED(pDSB->SetVolume( lVolume )))
		{
			m_lVolume = lVolume;
		}
	}
}


LONG CMpSnd::GetVolume()
{
	LONG lVolume=0xFFFFFFFF;

	LPDSB pDSB = GetFreeBuffer();
	
	if( NULL == pDSB)
		return lVolume;

	if( m_dInitFlag & DSBCAPS_CTRLVOLUME )
	{
		if(SUCCEEDED(pDSB->GetVolume(&lVolume )))
		{
			m_lVolume = lVolume;
		}
	}

	return lVolume + MEDIA_MAX_VOLUME;
}


void CMpSnd::SetRepeat(DWORD dRepeat)
{
	if(0xFFFFFFFF == dRepeat)
	{
		m_dwFlags = DSBPLAY_LOOPING;
	}

	else
	{
		m_dwFlags = 0;
	}
}



DWORD CMpSnd::GetStatus()
{
	DWORD	dStatus= 0;
	
	if( m_pBuffer == NULL )
		return dStatus; 
	
	for( DWORD i=0; i<m_dNbuff; i++ )
	{
		if( m_pBuffer[i] )
		{  
			DWORD dwStatus = 0;
			m_pBuffer[i]->GetStatus( &dwStatus );
			dStatus |= dwStatus;
		}
	}
	
	return dStatus;
}



INT CMpSnd::Query(char*, void*)
{
	return -1;
}


// Desc: Checks to see if a buffer is playing and returns TRUE if it is.
BOOL CMpSnd::IsSoundPlaying()
{
	BOOL bIsPlaying = FALSE;
	
	if( m_pBuffer == NULL )
		return FALSE; 
	
	for( DWORD i=0; i<m_dNbuff; i++ )
	{
		if( m_pBuffer[i] )
		{  
			DWORD dwStatus = 0;
			m_pBuffer[i]->GetStatus( &dwStatus );
			bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
		}
	}
	
	return bIsPlaying;
}





INT CMpSnd::Create(void*, void*, void*, void*)
{
	return 0;
}





// Desc: Fills a DirectSound buffer with a sound file 

HRESULT CMpSnd::FillBufferWithSound( LPDSB pDSB, BOOL bRepeatWavIfBufferLarger )
{
	HRESULT hr; 
	void*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
	DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
	DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 
	
	if( pDSB == NULL )
		return CO_E_NOTINITIALIZED;
	
	// Make sure we have focus, and we didn't just switch in from
	// an app which had a DirectSound device
	if( FAILED( hr = RestoreBuffer( pDSB, NULL ) ) ) 
		return DXTRACE_ERR( TEXT("RestoreBuffer"), hr );
	
	// Lock the buffer down
	if( FAILED( hr = pDSB->Lock( 0, m_dBufSize, 
		&pDSLockedBuffer, &dwDSLockedBufferSize, 
		NULL, NULL, 0L ) ) )
		return DXTRACE_ERR( TEXT("Lock"), hr );
	
	// Reset the wave file to the beginning 
	((CMpWaveFile*)m_pWaveFile)->ResetFile();
	
	if( FAILED( hr = ((CMpWaveFile*)m_pWaveFile)->Read( (BYTE*) pDSLockedBuffer,
		dwDSLockedBufferSize, 
		&dwWavDataRead ) ) )           
		return DXTRACE_ERR( TEXT("Read"), hr );
	
	if( dwWavDataRead == 0 )
	{
		// Wav is blank, so just fill with silence
		FillMemory( (BYTE*) pDSLockedBuffer, 
			dwDSLockedBufferSize, 
			(BYTE)(  ((CMpWaveFile*)m_pWaveFile)->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
	}
	else if( dwWavDataRead < dwDSLockedBufferSize )
	{
		// If the wav file was smaller than the DirectSound buffer, 
		// we need to fill the remainder of the buffer with data 
		if( bRepeatWavIfBufferLarger )
		{       
			// Reset the file and fill the buffer with wav data
			DWORD dwReadSoFar = dwWavDataRead;    // From previous call above.
			while( dwReadSoFar < dwDSLockedBufferSize )
			{  
				// This will keep reading in until the buffer is full 
				// for very short files
				if( FAILED( hr = ((CMpWaveFile*)m_pWaveFile)->ResetFile() ) )
					return DXTRACE_ERR( TEXT("ResetFile"), hr );
				
				hr = ((CMpWaveFile*)m_pWaveFile)->Read( (BYTE*)pDSLockedBuffer + dwReadSoFar,
					dwDSLockedBufferSize - dwReadSoFar,
					&dwWavDataRead );
				if( FAILED(hr) )
					return DXTRACE_ERR( TEXT("Read"), hr );
				
				dwReadSoFar += dwWavDataRead;
			} 
		}
		else
		{
			// Don't repeat the wav file, just fill in silence 
			FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, 
				dwDSLockedBufferSize - dwWavDataRead, 
				(BYTE)(  ((CMpWaveFile*)m_pWaveFile)->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
		}
	}
	
	// Unlock the buffer, we don't need it anymore.
	pDSB->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );
	
	return S_OK;
}



// Desc: Restores the lost buffer. *pbWasRestored returns TRUE if the buffer was 
//       restored.  It can also NULL if the information is not needed.

HRESULT CMpSnd::RestoreBuffer( LPDSB pDSB, BOOL* pbWasRestored )
{
	HRESULT hr;
	
	if( pDSB == NULL )
		return CO_E_NOTINITIALIZED;
	if( pbWasRestored )
		*pbWasRestored = FALSE;
	
	DWORD dwStatus;
	if( FAILED( hr = pDSB->GetStatus( &dwStatus ) ) )
		return DXTRACE_ERR( TEXT("GetStatus"), hr );
	
	if( dwStatus & DSBSTATUS_BUFFERLOST )
	{
		// Since the app could have just been activated, then
		// DirectSound may not be giving us control yet, so 
		// the restoring the buffer may fail.  
		// If it does, sleep until DirectSound gives us control.
		do 
		{
			hr = pDSB->Restore();
			if( hr == DSERR_BUFFERLOST )
				Sleep( 100 );
		}
		while( ( hr = pDSB->Restore() ) == DSERR_BUFFERLOST );
		
		if( pbWasRestored != NULL )
			*pbWasRestored = TRUE;
		
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}



// Desc: Finding the first buffer that is not playing and return a pointer to 
//       it, or if all are playing return a pointer to a randomly selected buffer.

LPDSB CMpSnd::GetFreeBuffer()
{
	DWORD	i=0;

	if( m_pBuffer == NULL )
		return FALSE; 
	
	for(i=0; i<m_dNbuff; i++ )
	{
		if( m_pBuffer[i] )
		{  
			DWORD dwStatus = 0;
			m_pBuffer[i]->GetStatus( &dwStatus );
			if ( ( dwStatus & DSBSTATUS_PLAYING ) == 0 )
				break;
		}
	}
	
	if( i != m_dNbuff )
		return m_pBuffer[ i ];
	else
		return m_pBuffer[ rand() % m_dNbuff ];
}



LPDSB CMpSnd::GetBuffer( DWORD dwIndex )
{
	if( m_pBuffer == NULL )
		return NULL;
	if( dwIndex >= m_dNbuff )
		return NULL;
	
	return m_pBuffer[dwIndex];
}



HRESULT CMpSnd::Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer )
{
	if( m_pBuffer == NULL )
		return CO_E_NOTINITIALIZED;

	if( dwIndex >= m_dNbuff )
		return E_INVALIDARG;
	
	*ppDS3DBuffer = NULL;
	
	return m_pBuffer[dwIndex]->QueryInterface( IID_IDirectSound3DBuffer, (void**)ppDS3DBuffer );
}








HRESULT LnDms_CreateSoundFromFile(
						  	ILcSmd** ppSnd
						  ,	void* pL
						  ,	LPTSTR sWaveFile
						  ,	DWORD dwCreationFlags
						  ,	GUID guid3DAlgorithm
						  ,	DWORD dwNumBuffers )
{
	HRESULT hr;
	HRESULT hrRet = S_OK;
	DWORD   i;
	LPDIRECTSOUNDBUFFER*	apDSBuffer     = NULL;
	DWORD					dwDSBufferSize = NULL;
	CMpWaveFile*			pWaveFile      = NULL;

	LPDIRECTSOUND8 pDS	= (LPDIRECTSOUND8)pL;
	
	if( pDS == NULL || sWaveFile == NULL || ppSnd == NULL || dwNumBuffers < 1 )
		return E_INVALIDARG;
	
	apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
	if( apDSBuffer == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto LFail;
	}

	dwCreationFlags |= DSBCAPS_CTRLVOLUME;


	
	pWaveFile = new CMpWaveFile();
	if( pWaveFile == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto LFail;
	}
	
	pWaveFile->Open( sWaveFile, NULL, WAVEFILE_READ );
	
	if( pWaveFile->GetSize() == 0 )
	{
		// Wave is blank, so don't create it.
		hr = E_FAIL;
		goto LFail;
	}
	
	// Make the DirectSound buffer the same size as the wav file
	dwDSBufferSize = pWaveFile->GetSize();
	
	// Create the direct sound buffer, and only request the flags needed
	// since each requires some overhead and limits if the buffer can 
	// be hardware accelerated
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize          = sizeof(DSBUFFERDESC);
	dsbd.dwFlags         = dwCreationFlags;
	dsbd.dwBufferBytes   = dwDSBufferSize;
	dsbd.guid3DAlgorithm = guid3DAlgorithm;
	dsbd.lpwfxFormat     = pWaveFile->m_pwfx;
	
	// DirectSound is only guarenteed to play PCM data.  Other
	// formats may or may not work depending the sound card driver.
	hr = pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL );
	
	// Be sure to return this error code if it occurs so the
	// callers knows this happened.
	if( hr == DS_NO_VIRTUALIZATION )
		hrRet = DS_NO_VIRTUALIZATION;
	
	if( FAILED(hr) )
	{
		// DSERR_BUFFERTOOSMALL will be returned if the buffer is
		// less than DSBSIZE_FX_MIN and the buffer is created
		// with DSBCAPS_CTRLFX.
		
		// It might also fail if hardware buffer mixing was requested
		// on a device that doesn't support it.
		DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
		
		goto LFail;
	}
	
	// Default to use DuplicateSoundBuffer() when created extra buffers since always 
	// create a buffer that uses the same memory however DuplicateSoundBuffer() will fail if 
	// DSBCAPS_CTRLFX is used, so use CreateSoundBuffer() instead in this case.
	if( (dwCreationFlags & DSBCAPS_CTRLFX) == 0 )
	{
		for( i=1; i<dwNumBuffers; i++ )
		{
			if( FAILED( hr = pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
			{
				DXTRACE_ERR( TEXT("DuplicateSoundBuffer"), hr );
				goto LFail;
			}
		}
	}
	else
	{
		for( i=1; i<dwNumBuffers; i++ )
		{
			hr = pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[i], NULL );
			if( FAILED(hr) )
			{
				DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
				goto LFail;
			}
		}
	}
	
	// Create the sound
	*ppSnd = new CMpSnd( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile, dwCreationFlags );
	
	SAFE_DELETE( apDSBuffer );
	return hrRet;
	
LFail:
	// Cleanup
	SAFE_DELETE( pWaveFile );
	SAFE_DELETE( apDSBuffer );
	return hr;
}








HRESULT LnDms_CreateSoundFromMemory(
									ILcSmd** ppSnd
								,	void* pL
								,	BYTE* pbData
								,	ULONG  ulDataSize
								,	LPWAVEFORMATEX pwfx
								,	DWORD dwCreationFlags
								,	GUID guid3DAlgorithm
								,	DWORD dwNumBuffers )
{
	HRESULT hr;
	DWORD   i;
	LPDIRECTSOUNDBUFFER*	apDSBuffer     = NULL;
	DWORD					dwDSBufferSize = NULL;
	CMpWaveFile*			pWaveFile      = NULL;

	LPDIRECTSOUND8 pDS	= (LPDIRECTSOUND8)pL;
	
	if( pDS == NULL || pbData == NULL || ppSnd == NULL || dwNumBuffers < 1 )
		return E_INVALIDARG;
	
	apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
	if( apDSBuffer == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto LFail;
	}

	
	dwCreationFlags |= DSBCAPS_CTRLVOLUME;

	
	pWaveFile = new CMpWaveFile();
	if( pWaveFile == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto LFail;
	}
	
	pWaveFile->OpenFromMemory( pbData,ulDataSize, pwfx, WAVEFILE_READ );
	
	
	// Make the DirectSound buffer the same size as the wav file
	dwDSBufferSize = ulDataSize;
	
	// Create the direct sound buffer, and only request the flags needed
	// since each requires some overhead and limits if the buffer can 
	// be hardware accelerated
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize          = sizeof(DSBUFFERDESC);
	dsbd.dwFlags         = dwCreationFlags;
	dsbd.dwBufferBytes   = dwDSBufferSize;
	dsbd.guid3DAlgorithm = guid3DAlgorithm;
	dsbd.lpwfxFormat     = pwfx;
	
	if( FAILED( hr = pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL ) ) )
	{
		DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
		goto LFail;
	}
	
	// Default to use DuplicateSoundBuffer() when created extra buffers since always 
	// create a buffer that uses the same memory however DuplicateSoundBuffer() will fail if 
	// DSBCAPS_CTRLFX is used, so use CreateSoundBuffer() instead in this case.
	if( (dwCreationFlags & DSBCAPS_CTRLFX) == 0 )
	{
		for( i=1; i<dwNumBuffers; i++ )
		{
			if( FAILED( hr = pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
			{
				DXTRACE_ERR( TEXT("DuplicateSoundBuffer"), hr );
				goto LFail;
			}
		}
	}
	else
	{
		for( i=1; i<dwNumBuffers; i++ )
		{
			hr = pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[i], NULL );
			if( FAILED(hr) )
			{
				DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
				goto LFail;
			}
		}
	}
	
	// Create the sound
	*ppSnd = new CMpSnd( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile, dwCreationFlags );
	
	SAFE_DELETE( apDSBuffer );
	return S_OK;
	
	LFail:
	// Cleanup
	
	SAFE_DELETE( apDSBuffer );
	return hr;
}



HRESULT LnDms_Get3DListener(void* pL, LPDIRECTSOUND3DLISTENER* ppDSListener)
{
	HRESULT             hr;
	DSBUFFERDESC        dsbdesc;
	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

	LPDIRECTSOUND8 pDS	= (LPDIRECTSOUND8)pL;
	
	if( NULL == pDS || NULL == ppDSListener)
		return E_INVALIDARG;

	*ppDSListener = NULL;
	
	// Obtain primary buffer, asking it for 3D control
	ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

	if( FAILED( hr = pDS->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
		return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
	
	if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, (void**)ppDSListener ) ) )
	{
		SAFE_RELEASE( pDSBPrimary );
		return DXTRACE_ERR( TEXT("QueryInterface"), hr );
	}
	
	// Release the primary buffer, since it is not need anymore
	SAFE_RELEASE( pDSBPrimary );
	
	return S_OK;
}
									
									




INT	LnDms_DSoundCreate(LPDIRECTSOUND8&	pDS, HWND hWnd, DWORD dwCoopLevel, DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate)
{
	HRESULT hr;
	
	// Create IDirectSound using the primary sound device
	if( FAILED( hr = DirectSoundCreate8( NULL, &pDS, NULL ) ) )
	{
		MessageBox(hWnd, "DirectSoundCreate8 Failed.", "Err", MB_ICONEXCLAMATION);
		return hr;
	}
	
	// Set DirectSound coop level 
	if( FAILED( hr = pDS->SetCooperativeLevel( hWnd, dwCoopLevel ) ) )
	{
		MessageBox(hWnd, "SetCooperativeLevel Failed.", "Err", MB_ICONEXCLAMATION);
		return hr;
	}


	if( FAILED( hr = LnDms_DSoundPrimaryBufferFMT(pDS, dwPrimaryChannels, dwPrimaryFreq, dwPrimaryBitRate ) ) )
	{
		MessageBox(hWnd, "m_pSndMng->SetPrimaryBufferFormat.", "Err", MB_ICONEXCLAMATION);
		return hr;
	}

	
	return S_OK;
}





// Desc: Set primary buffer to a specified format 
//       !WARNING! - Setting the primary buffer format and then using this
//                   same dsound object for DirectMusic messes up DirectMusic! 
//       For example, to set the primary buffer format to 22kHz stereo, 16-bit
//       then:   dwPrimaryChannels = 2
//               dwPrimaryFreq     = 22050, 
//               dwPrimaryBitRate  = 16

INT LnDms_DSoundPrimaryBufferFMT(
									LPDIRECTSOUND8 pDS
								,	DWORD dChannels
								,	DWORD dHz
								,	DWORD dBitRate )
{
	HRESULT             hr;
	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

	HWND				hWnd = GetActiveWindow();
	
	// Get the primary buffer 
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat   = NULL;
	
	if( FAILED( hr = pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
	{
		MessageBox(hWnd, "CreateSoundBuffer Failed.", "Err", MB_ICONEXCLAMATION);
		return hr;
	}
	
	WAVEFORMATEX wfx;
	ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
	wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
	wfx.nChannels       = (WORD) dChannels; 
	wfx.nSamplesPerSec  = (DWORD) dHz; 
	wfx.wBitsPerSample  = (WORD) dBitRate; 
	wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);
	
	if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
	{
		MessageBox(hWnd, "pDSBPrimary->SetFormat() Failed.", "Err", MB_ICONEXCLAMATION);
		return hr;
	}
	
	SAFE_RELEASE( pDSBPrimary );
	
	return S_OK;
}




DWORD LnDms_DSoundSpeakerConfig(LPDIRECTSOUND8 pDS)
{
	DWORD	dSpCfg ;
	WORD	dSpT;
	TCHAR	sTmp[1024] ="\0";
		
	pDS->GetSpeakerConfig(&dSpCfg);
	dSpT = LOWORD(dSpCfg);

	switch(dSpT)
	{
	case DSSPEAKER_HEADPHONE :
		sprintf( sTmp, "The audio is played through headphones.");
		break;
		
	case DSSPEAKER_MONO :
		sprintf( sTmp, "The audio is played through a single speaker.");
		break;
		
	case DSSPEAKER_QUAD :
		sprintf( sTmp, "The audio is played through quadraphonic speakers.");
		break;
		
	case DSSPEAKER_STEREO:
		sprintf( sTmp, "The audio is played through stereo speakers.");
		break;
		
	case DSSPEAKER_SURROUND :
		sprintf( sTmp, "The audio is played through surround speakers.");
		break;
	}
	
	dSpT = HIWORD(dSpCfg);
	
	switch(dSpT)
	{
	case DSSPEAKER_GEOMETRY_WIDE :
		strcat( sTmp, "The speakers are directed over an arc of 20 degrees.");
		break;
		
	case DSSPEAKER_GEOMETRY_NARROW :
		strcat( sTmp, "The speakers are directed over an arc of 10 degrees.");
		break;
		
	case DSSPEAKER_GEOMETRY_MIN :
		strcat( sTmp, "The speakers are directed over an arc of 5 degrees.");
		break;
		
	case DSSPEAKER_GEOMETRY_MAX :
		strcat( sTmp, "The speakers are directed over an arc of 180 degrees.");
		break;
	}
	
	return dSpCfg;
}




INT LnDms_DSound3DTest(HWND hWnd, LPDIRECTSOUND8 pDS, char* sFile)
{

	ILcSmd*					pSnd	= NULL;										// Test 3D Sound
	LPDIRECTSOUND3DBUFFER	pDSBuf;												// 3D Sound buffer
	LPDIRECTSOUND3DLISTENER	pDSLsn;												// 3D Listener object


	//1. Get the 3D listener, so we can control its params
	
	if( FAILED( LnDms_Get3DListener(pDS, &pDSLsn) ))
	{
		MessageBox(hWnd, "Listener Failed.", "Err", MB_ICONEXCLAMATION);
		return -1;
	}

	//2. Get the 3D buffer from the secondary buffer.
	// It's Test. Where is 3D sound or not.

	if(sFile && strlen(sFile))
	{
		if(FAILED(LnDms_CreateSoundFromFile(&pSnd, pDS, sFile, DSBCAPS_CTRL3D, DS3DALG_HRTF_LIGHT)))
		{
			MessageBox(hWnd, "Listener Failed.", "Err", MB_ICONEXCLAMATION);
			return -1;
		}
	}

	else
	{
		if(FAILED(LnDms_Create3DSoundFromMemoryForTst(&pSnd, pDS, DSBCAPS_CTRL3D, DS3DALG_HRTF_LIGHT)))
		{
			MessageBox(hWnd, "Listener Failed.", "Err", MB_ICONEXCLAMATION);
			return -1;
		}
	}
	
	if( FAILED( ((CMpSnd*)pSnd)->Get3DBufferInterface( 0, &pDSBuf )))
	{
		MessageBox(hWnd, "3D Sound Buffer Failed.", "Err", MB_ICONEXCLAMATION);
		return -1;
	}


	SAFE_RELEASE(	pDSLsn	);
	SAFE_RELEASE(	pDSBuf	);
	SAFE_DELETE(	pSnd	);

	return 0;
}


















HRESULT LnDms_Create3DSoundFromMemoryForTst(
										ILcSmd** ppSnd
									,	void* pL
									,	DWORD dwCreationFlags
									,	GUID guid3DAlgorithm
									,	DWORD dwNumBuffers
									)
{

#pragma pack (push,1)
	struct WAVEHEADER
	{
		char			szRIFF[4];
		long			lRIFFSize;
		char			szWave[4];
		char			szFmt[4];
		long			lFmtSize;
		WAVEFORMATEX	wfex;
		char			szData[4];
		long			lDataSize;
	};
#pragma pack (pop)

	INT	nTstBuffer = 64;

	WAVEHEADER	sWaveHeader;
	int			iDataSize = nTstBuffer;
	int			iSize= sizeof(WAVEHEADER);

	iSize= sizeof(WAVEFORMATEX);
	iSize= 0;

	BYTE*	Buffer = (BYTE*) malloc( nTstBuffer);
	BYTE*	pbData = (BYTE*) malloc( nTstBuffer + sizeof(WAVEHEADER));

	memset(Buffer, 0, nTstBuffer);

	// Prepare a WAVE file header for the captured data
	sprintf(sWaveHeader.szRIFF, "RIFF");
	sWaveHeader.lRIFFSize = 0;
	sprintf(sWaveHeader.szWave, "WAVE");
	sprintf(sWaveHeader.szFmt, "fmt ");
	sWaveHeader.lFmtSize = sizeof(WAVEFORMATEX);		
	sWaveHeader.wfex.nChannels = 1;
	sWaveHeader.wfex.wBitsPerSample = 16;
	sWaveHeader.wfex.wFormatTag = WAVE_FORMAT_PCM;
	sWaveHeader.wfex.nSamplesPerSec = 22050;
	sWaveHeader.wfex.nBlockAlign = sWaveHeader.wfex.nChannels * sWaveHeader.wfex.wBitsPerSample / 8;
	sWaveHeader.wfex.nAvgBytesPerSec = sWaveHeader.wfex.nSamplesPerSec * sWaveHeader.wfex.nBlockAlign;
	sWaveHeader.wfex.cbSize = 0;
	sprintf(sWaveHeader.szData, "data");
	sWaveHeader.lDataSize = 0;

	memcpy( (void*)(pbData + 0), &sWaveHeader, sizeof(WAVEHEADER));
	memcpy( (void*)(pbData + sizeof(WAVEHEADER) * 1), Buffer, nTstBuffer);
	
	// Fill in Size information in Wave Header
	iSize = iDataSize + sizeof(WAVEHEADER) - 8;

	memcpy( (void*)(pbData + 4), &iSize, sizeof(int));
	memcpy( (void*)(pbData + 42), &iDataSize, sizeof(int));


	free(	Buffer	);

	HRESULT hr;
	DWORD   i;
	LPDIRECTSOUNDBUFFER*	apDSBuffer     = NULL;
	DWORD					dwDSBufferSize = NULL;
	CMpWaveFile*			pWaveFile      = NULL;



	WAVEFORMATEX* pwfx = &sWaveHeader.wfex;
	ULONG ulDataSize = nTstBuffer + sizeof(WAVEHEADER);
	

	LPDIRECTSOUND8 pDS	= (LPDIRECTSOUND8)pL;
	
	if( pDS == NULL || ppSnd == NULL)
		return E_INVALIDARG;
	

	apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
	if( apDSBuffer == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto LFail;
	}

	
	dwCreationFlags |= DSBCAPS_CTRLVOLUME;

	
	pWaveFile = new CMpWaveFile();
	if( pWaveFile == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto LFail;
	}
	
	

	pWaveFile->OpenFromMemory( pbData,ulDataSize, pwfx, WAVEFILE_READ );
	
	
	// Make the DirectSound buffer the same size as the wav file
	dwDSBufferSize = ulDataSize;
	
	// Create the direct sound buffer, and only request the flags needed
	// since each requires some overhead and limits if the buffer can 
	// be hardware accelerated
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize          = sizeof(DSBUFFERDESC);
	dsbd.dwFlags         = dwCreationFlags;
	dsbd.dwBufferBytes   = dwDSBufferSize;
	dsbd.guid3DAlgorithm = guid3DAlgorithm;
	dsbd.lpwfxFormat     = pwfx;
	
	if( FAILED( hr = pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL ) ) )
	{
		DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
		goto LFail;
	}
	
	// Default to use DuplicateSoundBuffer() when created extra buffers since always 
	// create a buffer that uses the same memory however DuplicateSoundBuffer() will fail if 
	// DSBCAPS_CTRLFX is used, so use CreateSoundBuffer() instead in this case.
	if( (dwCreationFlags & DSBCAPS_CTRLFX) == 0 )
	{
		for( i=1; i<dwNumBuffers; i++ )
		{
			if( FAILED( hr = pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
			{
				DXTRACE_ERR( TEXT("DuplicateSoundBuffer"), hr );
				goto LFail;
			}
		}
	}
	else
	{
		for( i=1; i<dwNumBuffers; i++ )
		{
			hr = pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[i], NULL );
			if( FAILED(hr) )
			{
				DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
				goto LFail;
			}
		}
	}
	
	// Create the sound
	*ppSnd = new CMpSnd( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile, dwCreationFlags );
	
	SAFE_DELETE( apDSBuffer );

	free(pbData);

	return S_OK;
	
	LFail:
	// Cleanup
	
	SAFE_DELETE( apDSBuffer );
	free(pbData);

	return hr;
}