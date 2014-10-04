// Interface for the CMpSnd class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MpSnd_H_
#define _MpSnd_H_

#pragma warning(disable : 4786)
#include <vector>
using namespace std;


typedef LPDIRECTSOUNDBUFFER				LPDSB;
typedef LPDIRECTSOUND3DBUFFER			LPDS3D;


class CMpSnd : public CLcSmd
{
protected:
	LPDSB*	m_pBuffer	;
	DWORD	m_dBufSize	;
	void*	m_pWaveFile	;
	DWORD	m_dNbuff	;
	DWORD	m_dInitFlag	;

	DWORD	m_dwFlags	;
	DWORD	m_dwPrior	;
	LONG	m_lVolume	;
	LONG	m_lHz		;
	LONG	m_lPan		;

public:
	CMpSnd();
	CMpSnd(LPDSB* apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, void* pWaveFile, DWORD dwCreationFlags);
	virtual ~CMpSnd();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		Query(char* sCmd, void* pData);

public:
	virtual DWORD	GetType();
	virtual void	Play();
	virtual void	Stop();
	virtual void	Reset();
	virtual void	Pause();
	virtual void	SetVolume(LONG dVol);
	virtual LONG	GetVolume();
	virtual void	SetRepeat(DWORD dRepeat= 0xFFFFFFFF/*INFINITE. DSBPLAY_LOOPING */);
	virtual DWORD	GetStatus();

public:
	HRESULT		Get3DBufferInterface( DWORD dwIndex, LPDS3D* ppDS3DBuffer );
	HRESULT		FillBufferWithSound( LPDSB pDSB, BOOL bRepeatWavIfBufferLarger );
	LPDSB		GetFreeBuffer();
	LPDSB		GetBuffer( DWORD dwIndex );
	BOOL		IsSoundPlaying();

protected:
	HRESULT		RestoreBuffer( LPDSB pDSB, BOOL* pbWasRestored );
};



INT	LnDms_DSoundCreate(															// Direct Sound Create
					LPDIRECTSOUND8&	pDS											// DirectSound8
					,	HWND hWnd
					,	DWORD dwCoopLevel=DSSCL_PRIORITY						// Sets the priority level
					,	DWORD dwPrimaryChannels	=2								// Stereo. Mono:1
					,	DWORD dwPrimaryFreq		= 22050							// 22KHz
					,	DWORD dwPrimaryBitRate	= 16							// 16bit
					);

INT		LnDms_DSoundPrimaryBufferFMT(LPDIRECTSOUND8 pDS, DWORD dChannels, DWORD dHz, DWORD dBitRate );
DWORD	LnDms_DSoundSpeakerConfig(LPDIRECTSOUND8 pDS);
INT		LnDms_DSound3DTest(HWND hWnd, LPDIRECTSOUND8 pDS, char* sFile);






HRESULT LnDms_CreateSoundFromFile(
							ILcSmd** ppSound
						,	void* pL
						,	LPTSTR sWaveFile
						,	DWORD dwCreationFlags = 0
						,	GUID guid3DAlgorithm = GUID_NULL
						,	DWORD dwNumBuffers = 1
					   );



HRESULT LnDms_CreateSoundFromMemory(
							ILcSmd** ppSound
						,	void* pL
						,	BYTE* pbData
						,	ULONG ulDataSize
						,	LPWAVEFORMATEX pwfx
						,	DWORD dwCreationFlags = 0
						,	GUID guid3DAlgorithm = GUID_NULL
						,	DWORD dwNumBuffers = 1
						);


HRESULT LnDms_Create3DSoundFromMemoryForTst(
							ILcSmd** ppSound
						,	void* pL
						,	DWORD dwCreationFlags = 0
						,	GUID guid3DAlgorithm = GUID_NULL
						,	DWORD dwNumBuffers = 1
						);


HRESULT LnDms_Get3DListener(void* pL, LPDIRECTSOUND3DLISTENER* ppDSListener);




#endif



