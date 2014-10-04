// Implementation of the OpenAL.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#if defined(_MSC_VER)
  #include <AL_win/al.h>
  #include <AL_win/alc.h>
  #include <AL_win/alut.h>

#elif defined(__APPLE__) || defined(__IPHONE__)
  #include <OpenAL/al.h>
  #include <OpenAL/alc.h>

  #include <AL/alut.h>

#else
  #include <AL/al.h>
  #include <AL/alc.h>
  #include <AL/alut.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if defined(__BADA__)
  #include <FBaseSys.h>
#endif

#include "../rc_cmm/08glb22/SndGps.h"


#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>

#include "LcxMedia.h"


class CLcxSndAL : public ILCX_Sound
{
protected:
	ALuint		m_uiSource;
	ALuint		m_uiBuffer;

	ALsizei		m_uiSize;
	ALsizei		m_uiFreq;
	ALenum		m_iFormat;
	ALboolean	m_bLoop;

	INT			m_state;
	INT			m_state_s;	// saved satus
	INT			m_rpt;
	FLOAT		m_vol;

public:
	CLcxSndAL();
	virtual ~CLcxSndAL();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual INT		Play();
	virtual INT		Stop();
	virtual INT		Reset();
	virtual INT		Pause();
	virtual void	SetVolume(FLOAT Vol);
	virtual FLOAT	GetVolume();
	virtual INT		SetRepeat(INT Repeat= -1/*INFINITE*/);
	virtual INT		GetRepeat();
	virtual INT		GetStatus();

	virtual UINT	GetType()	{	return LC_OBJ_WAV; }

public:
	INT		CreateFromFile(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	INT		CreateFromBuffer(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	INT		CreateFromMemory(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	void	Destroy();

	static INT	m_refSnd;

protected:
	INT		CreateALSound(void* Buff);
};




static ALCcontext*	g_pAlCtx	= NULL;
static ALCdevice*	g_pAlDev	= NULL;

INT CLcxSndAL::m_refSnd = 0;


INT LcxSmd_ALCDeviceOpen()
{
	if(g_pAlDev && g_pAlCtx)
		return LC_OK;



#if defined(_MSC_VER)
	g_pAlDev = alcOpenDevice((ALCubyte*)"DirectSound3D");
#else
	g_pAlDev = alcOpenDevice(NULL);
#endif

	if(NULL == g_pAlDev)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Sound () Failed\n");
		return LC_CANNT_ACQUIRE_DEVICE;
	}

	g_pAlCtx=alcCreateContext(g_pAlDev, NULL);	// Create context(s)

	if(!g_pAlCtx)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("LcxSmd_ALCDeviceOpen() Failed\n");
		return LC_CANNT_ACQUIRE_DEVICE;
	}

	alcMakeContextCurrent(g_pAlCtx);			// Set active context
	alGetError();								// Clear Error Code

	return LC_OK;
}


INT	LcxSmd_ALCDeviceClose()
{
	if(NULL == g_pAlDev && NULL == g_pAlCtx)
		return LC_OK;

	g_pAlCtx=alcGetCurrentContext();			// Get active context
	g_pAlDev=alcGetContextsDevice(g_pAlCtx);	// Get device for active context

	alcDestroyContext(g_pAlCtx);				// Release context(s)
	alcCloseDevice(g_pAlDev);					// Close device

	g_pAlCtx = NULL;
	g_pAlDev = NULL;

	return  LC_OK;
}



////////////////////////////////////////////////////////////////////////////////

CLcxSndAL::CLcxSndAL()
{
	m_uiSource	= 0;
	m_uiBuffer	= 0;

	m_uiSize	= 0;
	m_uiFreq	= 0;
	m_iFormat	= 0;
	m_bLoop		= 0;

	m_state		= LCX_AV_STOP;
	m_state_s	= LCX_AV_STOP;
	m_rpt		= LCX_AV_INFINITE;

	m_vol		= 1.F;
}


CLcxSndAL::~CLcxSndAL()
{
	Destroy();
}


INT CLcxSndAL::Query(char* sCmd, LC_HANDLE pData)
{
	if     (0 == LcStr_Stricmp(sCmd, LCQUERY_INVALID))
	{
		UINT st = this->GetStatus();
		if(LCX_AV_PLAY == st)
		{
			m_state_s = LCX_AV_PLAY;
			this->Pause();
		}

		return LC_OK;
	}
	else if(0 == LcStr_Stricmp(sCmd, LCQUERY_RESTORE))
	{
		if(LCX_AV_PLAY == m_state_s)
		{
			m_state_s = LCX_AV_STOP;
			this->Play();
		}

		return LC_OK;
	}

	return LC_EFAIL;
}


INT CLcxSndAL::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcxSndAL::Destroy()
{
	// Release the Sound Instance
	alSourceStop(m_uiSource);
	alDeleteSources(1, &m_uiSource);
	alDeleteBuffers(1, &m_uiBuffer);

	--CLcxSndAL::m_refSnd;
	if(0 == CLcxSndAL::m_refSnd)
		LcxSmd_ALCDeviceClose();
}



INT CLcxSndAL::CreateFromFile(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT		hr		= LC_EFAIL;

	char*	orgFile	= (char*)p1;

	UINT	SndF = 0;		// Audio Format
	UINT	SndB = 0;		// Audio Bit Rate
	UINT	SndC = 0;		// Channels
	UINT	SndHz= 0;		// Sampling Rate
	BYTE*	Buff = NULL;	// Audio Buffer
	UINT	Size = 0;		// Buffer Size


	char	sDest[LC_MAX_PATH2]={0};
	char	drive[LC_MAX_DRIVE]={0};
	char	dir  [LC_MAX_DIR  ]={0};
	char	fname[LC_MAX_FNAME]={0};
	char	ext  [LC_MAX_EXT  ]={0};

	char	sMedia[LC_MAX_PATH2]={0};


	// check reference count
	if(0 > (CLcxSndAL::m_refSnd+1))
	{
		LOGE("The reference count of sound instance is out of bound.\n");
		return LC_EFAIL;
	}


	// get the file name and extension
	LcStr_SplitPath((char*)orgFile, drive, dir, fname, ext);
	sprintf(sDest, "%s%s%s%s", drive, dir, fname, ".gpsf");


	// Load gpsf file in embedded systems
	// for iphone, android
#if !defined(_MSC_VER) && !defined(_PC_LINUX_)

	if(LC_FAILED(LcSys_ResourcePath(sMedia, sDest)))
		return LC_CANNT_FIND_RESOURCE;


	hr = Gps_FileRead(&Buff, &SndF, &SndB, &SndC, &SndHz, &Size, sMedia);
	if(LC_FAILED(hr))
	{
		SAFE_FREE(	Buff	);
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Read Sound File Failed: %s\n", (char*)orgFile);
		hr = LC_CANNT_FIND_RESOURCE;
	}
	else
	{
		if     ( 8 == SndB && 1 == SndC)	m_iFormat = AL_FORMAT_MONO8;
		else if(16 == SndB && 1 == SndC)	m_iFormat = AL_FORMAT_MONO16;
		else if( 8 == SndB && 2 == SndC)	m_iFormat = AL_FORMAT_STEREO8;
		else if(16 == SndB && 2 == SndC)	m_iFormat = AL_FORMAT_STEREO16;

		if(0 == m_iFormat)
		{
			SAFE_FREE(	Buff	);
			LOGE("Err: %d, %s \t", __LINE__, __FILE__);
			LOGE("Sound File Format Error: %s\n", (char*)orgFile);
			hr = LC_INVALID_RESOURCE;
		}

		m_uiSize = Size;
		m_uiFreq = SndHz;
	}

	if(LC_FAILED(hr))
		return hr;

#endif

	if(LC_FAILED(hr))
	{
		if(LC_FAILED(LcSys_ResourcePath(sMedia, orgFile)))
			return LC_CANNT_FIND_RESOURCE;


			// Load Wave File
		alutLoadWAVFile((ALbyte*)sMedia, (ALenum*)&m_iFormat, (void**)&Buff
						, (ALsizei*)&m_uiSize, (ALsizei*)&m_uiFreq, (ALboolean*)&m_bLoop);
		if(NULL == Buff || 0 == m_iFormat || 0 == m_uiSize || 0 == m_uiFreq)
		{
			SAFE_FREE(	Buff	);
			LOGE("Err: %d, %s \t", __LINE__, __FILE__);
			LOGE("Load WAV File Failed\n");
			hr = LC_INVALID_RESOURCE;
			return hr;
		}


		Size  = m_uiSize;
		SndHz = m_uiFreq;


		if(AL_FORMAT_MONO8 == m_iFormat)		{	SndB =  8;	SndC = 1;	}
		else if(AL_FORMAT_MONO16 == m_iFormat)	{	SndB = 16;	SndC = 1;	}
		else if(AL_FORMAT_STEREO8 == m_iFormat)	{	SndB =  8;	SndC = 2;	}
		else if(AL_FORMAT_STEREO16 == m_iFormat){	SndB = 16;	SndC = 2;	}

		hr = Gps_FileWrite(Buff, LCX_AV_PCM, SndB, SndC, SndHz, Size, sDest);
		SAFE_FREE(	Buff	);
		if(LC_FAILED(hr))
		{
			LOGE("Err: %d, %s \t", __LINE__, __FILE__);
			LOGE("Write Sound File Failed: %s\n", (char*)orgFile);
			hr = LC_INVALID_RESOURCE;
			return hr;
		}

		// reload
		hr = Gps_FileRead(&Buff, &SndF, &SndB, &SndC, &SndHz, &Size, sDest);
		if(LC_FAILED(hr))
		{
			SAFE_FREE(	Buff	);
			LOGE("Err: %d, %s \t", __LINE__, __FILE__);
			LOGE("Read Sound File Failed: %s\n", (char*)sDest);
			hr = LC_INVALID_RESOURCE;
			return hr;
		}

		if     ( 8 == SndB && 1 == SndC) m_iFormat = AL_FORMAT_MONO8;
		else if(16 == SndB && 1 == SndC) m_iFormat = AL_FORMAT_MONO16;
		else if( 8 == SndB && 2 == SndC) m_iFormat = AL_FORMAT_STEREO8;
		else if(16 == SndB && 2 == SndC) m_iFormat = AL_FORMAT_STEREO16;

		if(0 == m_iFormat)
		{
			SAFE_FREE(	Buff	);
			LOGE("Err: %d, %s \t", __LINE__, __FILE__);
			LOGE("Sound File Format Error: %s\n", (char*)sMedia);
			hr = LC_INVALID_RESOURCE;
			return hr;
		}
	}


	if(0 == CLcxSndAL::m_refSnd)
	{
		hr = LcxSmd_ALCDeviceOpen();
		if(LC_FAILED(hr))
		{
			SAFE_FREE(	Buff	);
			return hr;
		}
	}


	hr = CreateALSound(Buff);
	SAFE_FREE(	Buff	);

	if(LC_FAILED(hr))
	{
		if(0 == CLcxSndAL::m_refSnd)
		LcxSmd_ALCDeviceClose();
	}

	++CLcxSndAL::m_refSnd;
	return LC_OK;
}


INT CLcxSndAL::CreateFromMemory(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT		hr		= LC_EFAIL;
	void*	Memory	= p1;
	UINT 	MemorySize=0;
	UINT	nType	= LC_OBJ_WAV;

	UINT 	SndF	= 0;
	UINT 	SndB	= 0;
	UINT 	SndC	= 0;
	BYTE* 	Buff	= NULL;


	if(!p1 || !p2)
		return hr;

	MemorySize = *((UINT*)p2);

	if(p3)
		nType = *((UINT*)p3);

	// check reference count
	if(0 > (CLcxSndAL::m_refSnd+1))
	{
		LOGE("The reference count of sound instance is out of bound.\n");
		return LC_EFAIL;
	}


	if(LC_OBJ_OGG == nType)
	{
		hr = Ogg_CreateFromBuffer((oggbuf*)&Buff
								, (oggint*)&m_uiSize
								, (oggint*)&SndB
								, (oggint*)&SndC
								, (oggint*)&m_uiFreq
								, (oggbuf )Memory
								, (oggint )MemorySize);
	}
	else
	{
		hr = Gps_MemRead((unsigned char**)&Buff
						, &SndF
						, &SndB
						, &SndC
						, (unsigned int*)&m_uiFreq
						, (unsigned int*)&m_uiSize
						, Memory, MemorySize);
	}


	if     ( 8 == SndB && 1 == SndC) m_iFormat = AL_FORMAT_MONO8;
	else if(16 == SndB && 1 == SndC) m_iFormat = AL_FORMAT_MONO16;
	else if( 8 == SndB && 2 == SndC) m_iFormat = AL_FORMAT_STEREO8;
	else if(16 == SndB && 2 == SndC) m_iFormat = AL_FORMAT_STEREO16;


	if( LC_FAILED(hr) || 
		0 == m_iFormat||
		0 == m_uiFreq ||
		0 == m_uiSize ||
		NULL== Buff)
	{
		SAFE_FREE(	Buff	);
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Read Memory Failed\n");
		hr = LC_CANNT_FIND_RESOURCE;
		return hr;
	}


	if(0 == CLcxSndAL::m_refSnd)
	{
		hr = LcxSmd_ALCDeviceOpen();
		if(LC_FAILED(hr))
		{
			SAFE_FREE(	Buff	);
			return hr;
		}
	}


	hr = CreateALSound(Buff);
	SAFE_FREE(	Buff	);

	if(LC_FAILED(hr))
	{
		if(0 == CLcxSndAL::m_refSnd)
		LcxSmd_ALCDeviceClose();
	}

	++CLcxSndAL::m_refSnd;
	return LC_OK;
}



INT CLcxSndAL::CreateFromBuffer(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT				hr = 0;
	TLCX_SOUND_INF*	pInf = (TLCX_SOUND_INF*)p1;
	void*			Buff = NULL;

	if(	NULL == pInf ||
		NULL == pInf->Buff ||
		0xCC == pInf->Size ||
		0xCC == pInf->SndHz )
	{
		LOGE("AL Sound Buffer Error.\n");
		return LC_EFAIL;
	}

	if(0 > (CLcxSndAL::m_refSnd+1))
	{
		LOGE("The reference count of sound instance is out of bound.\n");
		return LC_EFAIL;
	}


	if     ( 8 == pInf->SndB && 1 == pInf->SndC) m_iFormat = AL_FORMAT_MONO8;
	else if(16 == pInf->SndB && 1 == pInf->SndC) m_iFormat = AL_FORMAT_MONO16;
	else if( 8 == pInf->SndB && 2 == pInf->SndC) m_iFormat = AL_FORMAT_STEREO8;
	else if(16 == pInf->SndB && 2 == pInf->SndC) m_iFormat = AL_FORMAT_STEREO16;

	m_uiSize	= pInf->Size;
	m_uiFreq	= pInf->SndHz;
	Buff		= pInf->Buff;

	if(0 == m_iFormat)
	{
		LOGE("Wave Format for AL Error\n");
		return LC_EFAIL;
	}


	if(0 == CLcxSndAL::m_refSnd)
	{
		hr = LcxSmd_ALCDeviceOpen();
		if(LC_FAILED(hr))
		{
			SAFE_FREE(	Buff	);
			return hr;
		}
	}


	hr = CreateALSound(Buff);
	SAFE_FREE(	Buff	);

	if(LC_FAILED(hr))
	{
		if(0 == CLcxSndAL::m_refSnd)
		LcxSmd_ALCDeviceClose();
	}

	++CLcxSndAL::m_refSnd;
	return LC_OK;
}




INT CLcxSndAL::CreateALSound(void* Buff)
{
	INT hr = 0;

	// Generate Buffers
	alGenBuffers( 1, &m_uiBuffer );
	hr = alGetError();
	if(AL_NO_ERROR != hr)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Generate Buffers: %s\n", alGetString(hr));

		hr = LC_CANNT_ACQUIRE_DEVICE;
		return hr;
	}


	alBufferData(m_uiBuffer,m_iFormat,Buff,m_uiSize,m_uiFreq);
	hr = alGetError();
	if(AL_NO_ERROR != hr)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Buffer Data: %s\n", alGetString(hr));

		hr = LC_CANNT_ACQUIRE_DEVICE;
		return hr;
	}


	// Generate a Source to playback the Buffer
	alGenSources( 1, &m_uiSource );
	hr = alGetError();

	alSourcei( m_uiSource, AL_BUFFER, m_uiBuffer );
	hr = alGetError();

	alSourcef(m_uiSource,AL_GAIN, m_vol);			// Set volume
	hr = alGetError();

	alSourcei(m_uiSource,AL_LOOPING, AL_FALSE);		// Set looping
	hr = alGetError();

	return LC_OK;
}





INT CLcxSndAL::Play()
{
	m_state = LCX_AV_PLAY;
	alSourcePlay( m_uiSource);

	return LC_OK;
}

INT CLcxSndAL::Stop()
{
	m_state = LCX_AV_STOP;
	alSourceStop(m_uiSource);

	return LC_OK;
}

INT CLcxSndAL::Reset()
{
	m_state = LCX_AV_STOP;
	alSourceRewind(m_uiSource);

	return LC_OK;
}

INT CLcxSndAL::Pause()
{
	if(LCX_AV_STOP == m_state)
		return LC_OK;

	m_state = LCX_AV_PAUSE;
	alSourcePause(m_uiSource);

	return LC_OK;
}

void CLcxSndAL::SetVolume(FLOAT fVol)
{
	if(fVol<0)
		return;

	m_vol = fVol;
	alSourcef(m_uiSource,AL_GAIN, m_vol);
}


FLOAT CLcxSndAL::GetVolume()
{
	return m_vol;
}

INT CLcxSndAL::SetRepeat(INT Repeat)
{
	m_rpt = Repeat;

	if(LCX_AV_INFINITE == *(  (UINT*)(&m_rpt)  ) )
		alSourcei(m_uiSource, AL_LOOPING, AL_TRUE);		// Set looping
	else
		alSourcei(m_uiSource, AL_LOOPING, AL_FALSE);

	return LC_OK;
}

INT CLcxSndAL::GetRepeat()
{
	return m_rpt;
}

INT CLcxSndAL::GetStatus()
{
	if(LCX_AV_PLAY == m_state)
	{
		ALint st=0;
		alGetSourcei(m_uiSource, AL_SOURCE_STATE, &st);

		if(AL_STOPPED == st)
			m_state = LCX_AV_STOP;
		else if(AL_PAUSED == st)
			m_state = LCX_AV_PAUSE;
	}

	return m_state;
}





INT LcxSmd_CreateALSndFromFile(char* sCmd
							, PLCX_SOUND* pOut	// Output instance
							, LC_HANDLE p1		// Sound File Name
							, LC_HANDLE p2		// User Data(Option)
							, LC_HANDLE p3		// No Use
							, LC_HANDLE p4		// No Use
							)
{
	INT			hr = 0;
	UINT		nType = 0;

	CLcxSndAL*	pObj = NULL;

	*pOut		= NULL;
	
	char*	orgFile	= (char*)p1;
	char	fname[LC_MAX_FNAME]={0};
	char	ext  [LC_MAX_EXT  ]={0};
	LcStr_SplitPath((char*)orgFile, NULL, NULL, fname, ext);


	if     (0 == LcStr_Stricmp(".ogg", ext))
		nType = LC_OBJ_OGG;

	else if(0 == LcStr_Stricmp(".wav", ext))
		nType = LC_OBJ_WAV;

	else
		return LC_INVALID_RESOURCE;



	if(LC_OBJ_OGG == nType)
	{
		FILE*	fp      = NULL;
		BYTE*	bufSrc  = NULL;
		UINT	bufSize = 0;

		fp = fopen(orgFile, "rb");
		if(!fp)
			return -1;

		fseek(fp, 0, SEEK_END);
		bufSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		bufSrc = (BYTE*)malloc(bufSize);

		fread(bufSrc, 1, bufSize, fp);
		fseek(fp, 0, SEEK_SET);
		fclose(fp);

		hr = LcxSmd_CreateALSndFromMemory(NULL, pOut, bufSrc, &bufSize, &nType);

		SAFE_FREE( bufSrc );
		return hr;


//		oggbuf sndData = NULL;
//		int sndSize   = 0;
//		int sndChannel= 0;
//		int sndBits   = 0;
//		int sndHz     = 0;
//
//		hr = Ogg_CreateFromBuffer(&sndData, &sndSize, &sndBits, &sndChannel, &sndHz, bufSrc, bufSize);
//		free(bufSrc);
//
//		if(0>hr)
//			return hr;
//
//		INT				hr = 0;
//		TLCX_SOUND_INF	oggInf= { LCX_AV_PCM, sndBits, sndChannel, sndHz, (BYTE*)sndData, sndSize};
//	
//		hr = LcxSmd_CreateALSndFromBuffer(NULL, pOut, &oggInf, p2, p3, p4);
//		//Ogg_ReleaseBuf(&sndData);
//
//		return hr;
	}


#if defined(__ANDROID__)
	char		sMedia[LC_MAX_PATH2]={0};

	BYTE*		buf = NULL;
	ULONG		len = 0;
	const char*	sApkFile = LcSys_AndroidApkFile(NULL);


	LcStr_SplitPath((char*)orgFile, NULL, NULL, sMedia, NULL);
	strcat(sMedia, ".gpsf");

	hr = Lzip_UnzipbyName(&buf, &len, sApkFile, sMedia, LC_TRUE);
	//LOGI("Create AL Sound Unzip: %x %ld %d %s\n", (UINT)buf, len, hr, sMedia);

	if(LC_FAILED(hr))
		return LC_CANNT_ALLOC;


	hr = LcxSmd_CreateALSndFromMemory(NULL, (PLCX_SOUND*)&pObj, buf, &len, p2, p3);
	LOGI("Create AL Sound From Memory hr: %d\n", hr);
	SAFE_FREE(	buf	);

	if(LC_FAILED(hr))
		return LC_CANNT_ALLOC;


	*pOut = (PLCX_SOUND)pObj;
	return LC_OK;
#endif


	pObj = new CLcxSndAL;

	hr = pObj->CreateFromFile(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}



INT LcxSmd_CreateALSndFromMemory(char* sCmd
								, PLCX_SOUND* pOut
								, LC_HANDLE p1
								, LC_HANDLE p2
								, LC_HANDLE p3
								, LC_HANDLE p4
								)
{
	INT			hr = 0;
	CLcxSndAL*	pObj = NULL;

	*pOut = NULL;
	 pObj = new CLcxSndAL;

	 hr = pObj->CreateFromMemory(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}


INT LcxSmd_CreateALSndFromBuffer(char* sCmd
								, PLCX_SOUND* pOut
								, LC_HANDLE p1
								, LC_HANDLE p2
								, LC_HANDLE p3
								, LC_HANDLE p4
								)
{
	INT			hr = 0;
	CLcxSndAL*	pObj = NULL;

	*pOut = NULL;
	pObj = new CLcxSndAL;

	hr = pObj->CreateFromBuffer(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}


