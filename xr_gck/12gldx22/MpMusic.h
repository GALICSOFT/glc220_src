// Interface for the CMpMusic class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MpMusic_H_
#define _MpMusic_H_

typedef struct IDirectMusicLoader8			*LPDIRECTMUSICLOADER		;
typedef struct IDirectMusicAudioPath		*LPDIRECTMUSICAUDIOPATH		;
typedef struct IDirectMusicPerformance8		*LPDIRECTMUSICPERFORMANCE	;
typedef struct IDirectMusicSegment8			*LPDIRECTMUSICSEGMENT		;


typedef LPDIRECTMUSICLOADER				LPDML;
typedef LPDIRECTMUSICAUDIOPATH			LPDMA;
typedef LPDIRECTMUSICPERFORMANCE		LPDMP;
typedef LPDIRECTMUSICSEGMENT			LPDMS;

//	It's Defined dmusici.h
//
//	#define DMUS_APATH_SHARED_STEREOPLUSREVERB   1
//	#define DMUS_APATH_DYNAMIC_3D                6
//	#define DMUS_APATH_DYNAMIC_MONO              7
//	#define DMUS_APATH_DYNAMIC_STEREO            8
//	#define DMUS_SEG_REPEAT_INFINITE    0xFFFFFFFF
//	#define DMUS_SEG_ALLTRACKS          0x80000000
//	#define DMUS_SEG_ANYTRACK           0x80000000




class CMpMusic : public CLcSmd
{
protected:
	LPDML		m_pDLoader;		// Music Loader
	LPDMA		m_pOdioPth;		// Music Audiopath
	LPDMP		m_pPerform;		// Music Performance
	LPDMS		m_pSegment;		// Music Segment

	LONG		m_mtPause;		// Segment Position for Pause
	LONG		m_dVolume;		// Volume

public:
	CMpMusic();
	CMpMusic(	LPDML	_pDLoader
			,	LPDMA	_pOdioPth
			,	LPDMP	_pPerform
			,	LPDMS	_pSegment);

	virtual ~CMpMusic();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		Query(char* sCmd, void* pData);

	virtual DWORD	GetType();
	virtual void	Play();
	virtual void	Stop();
	virtual void	Reset();
	virtual void	Pause();
	virtual void	SetVolume(LONG dVol);
	virtual LONG	GetVolume();
	virtual void	SetRepeat(DWORD dRepeat= 0xFFFFFFFF/*DMUS_SEG_REPEAT_INFINITE */);
	virtual DWORD	GetStatus();

public:
	LPDMA	GetAudioPath() const;
};






struct CMpMusic3D : public CLcSmd
{
	LPDIRECTSOUND3DBUFFER	m_pDsrc;				// 3D sound buffer
	LPDIRECTSOUND3DLISTENER	m_pDlsn;				// 3D listener object

	DS3DBUFFER				m_DsSrc;				// 3D buffer properties (Source)
	DS3DLISTENER			m_DsLsn;				// Listener properties (Dest)

	ILcSmd*					m_pMusic;				// CMpMusic* pointer

	CMpMusic3D();
	virtual ~CMpMusic3D();

	//INT			Create(LPDML pLoader, HWND hWnd, char* sFile);
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		Query(char* sCmd, void* pData);

	virtual DWORD	GetType();
	virtual void	Play();
	virtual void	Stop();
	virtual void	Reset();
	virtual void	Pause();
	virtual void	SetVolume(LONG dVol);
	virtual LONG	GetVolume();
	virtual void	SetRepeat(DWORD dRepeat= 0xFFFFFFFF/*INFINITE*/);
	virtual DWORD	GetStatus();

public:
	D3DXVECTOR3	GetPosSrc();
	D3DXVECTOR3	GetPosLsn();
	void		SetPosSrc(const D3DXVECTOR3* vcP);								// Sound Source Position
	void		SetPosLsn(const D3DXVECTOR3* vcP);								// Sound Listener Position
};


INT		LnDms_Create3DSoundFromMusic(											// Create 3D sound form Music
							ILcSmd** pSnd3D
						,	void* pL
						,	HWND hWnd
						,	char* sFile);

INT		LnDms_CreateMusicFromFile(												// Create Music from File
							ILcSmd** pMusic
						,	void* pL
						,	HWND hWnd
						,	char* sFile
						,	DWORD dwDefaultPathType=1 // <= DMUS_APATH_SHARED_STEREOPLUSREVERB
						);

INT		LnDms_CreateMusicFromRsc(												// Create Music from Resource
							ILcSmd** pMusic
						,	void* pL
						,	HWND hWnd
						,	INT nResourceID
						,	char* sResourceType
						,	DWORD dwDefaultPathType=1 //=DMUS_APATH_SHARED_STEREOPLUSREVERB
						);

INT		LnDms_DMusicLoaderCreate(LPDML& pMusicLoader, HWND hWnd);
void	LnDms_DMusicLoaderDestroy(LPDML& pMusicLoader);

LONG	LnDms_GetTimeOffset(LPDMP pPerform, LPDMS pSegment);

#endif