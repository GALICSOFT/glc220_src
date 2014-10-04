// Implementation of the OpenSL.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#if defined(__ANDROID__)
  #include <assert.h>
  #include <jni.h>

  #include <SLES/OpenSLES.h>
  #include <SLES/OpenSLES_Android.h>

  typedef SLAndroidSimpleBufferQueueItf	SLAndQueItf;			// buffer queue player interfaces
  typedef SLEnvironmentalReverbItf		SLEnvRevItf;
  typedef SLEnvironmentalReverbSettings SLEnvRevSets;

#else
  typedef int		jint		;
  typedef void*		jclass		;
  typedef void*		jmethodID	;
  typedef void*		JavaVM		;
  typedef void*		JNIEnv		;

  typedef void*		SLAndQueItf	;
  typedef void*		SLObjectItf	;
  typedef void*		SLEngineItf	;
  typedef void*		SLPlayItf	;
  typedef void*		SLVolumeItf	;
  typedef short int SLmillibel	;

  typedef void*		SLEffectSendItf;
  typedef void*		SLEnvRevItf	;
  typedef int		SLEnvRevSets;

  #if !defined(SL_DATAFORMAT_PCM)
    #define SL_DATAFORMAT_PCM	0x00000002
  #endif

  #if !defined(SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR)
    #define SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR	0x00000000
  #endif

#endif

#include "../rc_cmm/08glb22/SndGps.h"

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>

#include "LcxMedia.h"



namespace LCSYS
{
	extern JavaVM*	g_pJavaVM	;
	extern jclass	g_JniClzz	;
}

////////////////////////////////////////////////////////////////////////////////
// for Android Media Player

static jmethodID Jni_SndCreate	={0};
static jmethodID Jni_SndDestroy	={0};
static jmethodID Jni_SndSetData	={0};
static jmethodID Jni_SndGetData	={0};
static jmethodID Jni_SndPlay	={0};
static jmethodID Jni_SndStop	={0};
static jmethodID Jni_SndPause	={0};
static jmethodID Jni_SndReset	={0};



class CLcxSndJni : public ILCX_Sound
{
protected:
	char		m_name[LC_DEFAULT_NAME];	// file name
	INT			m_fd;						// file descriptor

	INT			m_state;					// state
	INT			m_state_s;					// saved satus
	INT			m_rpt;						// repeat Looping = 1, once = 0
	FLOAT		m_vol;						// volume

public:
	CLcxSndJni();
	virtual ~CLcxSndJni();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();

	virtual INT		Play();
	virtual INT		Stop();
	virtual INT		Reset();
	virtual INT		Pause();
	virtual void	SetVolume(FLOAT Vol);
	virtual FLOAT	GetVolume();
	virtual INT		SetRepeat(INT Repeat= LC_INFINITE);
	virtual INT		GetRepeat();
	virtual INT		GetStatus();

	virtual UINT	GetType()	{	return LC_OBJ_OGG; }

public:
	INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	void	Destroy();
};


////////////////////////////////////////////////////////////////////////////////

CLcxSndJni::CLcxSndJni()
{
	memset(m_name, 0, sizeof m_name);
	m_fd		= -1;
	m_state		= LCX_AV_STOP;
	m_state_s	= LCX_AV_STOP;
	m_rpt		= LCX_AV_INFINITE;
	m_vol		= 1.F;
}


CLcxSndJni::~CLcxSndJni()
{
	Destroy();
}


INT CLcxSndJni::Query(char* sCmd, LC_HANDLE pData)
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


INT CLcxSndJni::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcxSndJni::Destroy()
{
	if(0>m_fd)
		return;

#if defined(__ANDROID__)
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("SndJni::Play::Err::AttachCurrentThread::\n");
		return;
	}

	pEnv->CallStaticIntMethod(clzz, Jni_SndDestroy, (jint)m_fd);

#endif

	m_fd = -1;
}


INT CLcxSndJni::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	char*	name = (char*)p1;
	int		fd = -1;

#if defined(__ANDROID__)
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("SndJni::Create::Err::AttachCurrentThread::Create\n");
		return LC_EFAIL;
	}

	fd = pEnv->CallStaticIntMethod(clzz, Jni_SndCreate, pEnv->NewStringUTF(name));

#endif

	if(0>fd)
		return LC_EFAIL;

	strcpy(m_name, name);
	m_fd = fd;

	return LC_OK;
}


INT CLcxSndJni::Play()
{
	m_state = LCX_AV_PLAY;

#if defined(__ANDROID__)
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("SndJni::Play::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	pEnv->CallStaticIntMethod(clzz, Jni_SndPlay, (jint)m_fd);

#endif

	return LC_OK;
}

INT CLcxSndJni::Stop()
{
	m_state = LCX_AV_STOP;

#if defined(__ANDROID__)
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("SndJni::Stop::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	pEnv->CallStaticIntMethod(clzz, Jni_SndStop, (jint)m_fd);

#endif
	return LC_OK;
}

INT CLcxSndJni::Reset()
{
	m_state = LCX_AV_STOP;

#if defined(__ANDROID__)
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("SndJni::Reset::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	pEnv->CallStaticIntMethod(clzz, Jni_SndReset, (jint)m_fd);

#endif
	return LC_OK;
}

INT CLcxSndJni::Pause()
{
	GetStatus();

	if(LCX_AV_PLAY != m_state)
		return LC_OK;

	m_state = LCX_AV_PAUSE;

#if defined(__ANDROID__)
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("SndJni::Pause::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	pEnv->CallStaticIntMethod(clzz, Jni_SndPause, (jint)m_fd);

#endif

	return LC_OK;
}


void CLcxSndJni::SetVolume(FLOAT fVol)
{
	if(fVol<0)
		return;

	m_vol = fVol;

#if defined(__ANDROID__)
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("SndJni::SetVolume::Err::AttachCurrentThread::\n");
		return;
	}

	jint vol = m_vol * 10000;
	pEnv->CallStaticIntMethod(clzz, Jni_SndSetData, (jint)m_fd, 0x02, vol);

#endif
}


FLOAT CLcxSndJni::GetVolume()
{
	return m_vol;
}

INT CLcxSndJni::SetRepeat(INT Repeat)
{
	m_rpt = Repeat;

#if defined(__ANDROID__)
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("SndJni::SetRepeat::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	jint rpt = ((INT)LCX_AV_INFINITE == m_rpt)? 1: 0;
	pEnv->CallStaticIntMethod(clzz, Jni_SndSetData, (jint)m_fd, 0x01, rpt);

#endif

	return LC_OK;
}

INT CLcxSndJni::GetRepeat()
{
	return m_rpt;
}

INT CLcxSndJni::GetStatus()
{
#if defined(__ANDROID__)
	jint	st		=	0;

	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("SndJni::GetStatus::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	st = pEnv->CallStaticIntMethod(clzz, Jni_SndGetData, (jint)m_fd, 0x03);
	if(0 == st && LCX_AV_PLAY == m_state)
		m_state = LCX_AV_STOP;

#endif

	return m_state;
}




////////////////////////////////////////////////////////////////////////////////
// OpenSL

class CLcxSndSL : public ILCX_Sound
{
public:
	// engine interfaces
	static SLObjectItf	g_slEngObj;
	static SLEngineItf	g_slEngDev;

	// output mix interfaces
	static SLObjectItf	g_slMixObj;
	static SLEnvRevItf	g_slMixEnv;

	// aux effect on the output mix, used by the buffer queue player
	static SLEnvRevSets g_slMixRev;

protected:
	SLObjectItf		m_slBufObct;
	SLPlayItf		m_slBufPlay;
	SLAndQueItf		m_slBufQue;
	SLEffectSendItf	m_slBufEft;

	SLmillibel		m_slMaxVol;		// MaxVolume
	SLVolumeItf		m_slBufVol;

protected:
	// pointer and size of the next player buffer to enqueue, and number of remaining buffers
	INT				m_RscType;		// buff = 0, file = 1, Memory = 2;
	UINT			m_SndF;			// Audio Format
	UINT			m_SndB;			// Audio Bit Rate
	UINT			m_SndC;			// Channels
	UINT			m_SndHz;		// Sampling Rate
	BYTE*			m_SndBuf;		// Audio Buffer
	UINT			m_SndSize;		// Buffer Size

	INT				m_state;		// State
	INT				m_state_s;		// saved satus
	INT				m_rpt;			// Repeat Count

public:
	CLcxSndSL();
	virtual ~CLcxSndSL();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual INT		Play();
	virtual INT		Stop();
	virtual INT		Reset();
	virtual INT		Pause();
	virtual void	SetVolume(FLOAT Vol);
	virtual FLOAT	GetVolume();
	virtual INT		SetRepeat(INT Repeat= LCX_AV_INFINITE){ m_rpt=Repeat; return LC_OK; }
	virtual INT		GetRepeat()	{	return m_rpt;	}
	virtual INT		GetStatus()	{	return m_state;		}
	virtual UINT	GetType()	{	return LC_OBJ_WAV; }

public:
	INT			CreateFromFile  (LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	INT			CreateFromMemory(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	INT			CreateFromBuffer(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE = NULL);
	void		Destroy();

	void		SetState(INT st){	m_state = st;		}
	SLAndQueItf	GetBufQue()		{	return m_slBufQue;	}

	static void PlayerCallback(SLAndQueItf que, void* ctx);
	static INT	m_refSnd;

protected:
	INT			CreateSLSound();
};


////////////////////////////////////////////////////////////////////////////////


INT				CLcxSndSL::m_refSnd = 0;
SLObjectItf		CLcxSndSL::g_slEngObj = NULL;
SLEngineItf		CLcxSndSL::g_slEngDev = NULL;
SLObjectItf		CLcxSndSL::g_slMixObj = NULL;
SLEnvRevItf		CLcxSndSL::g_slMixEnv = NULL;
SLEnvRevSets	CLcxSndSL::g_slMixRev = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;


////////////////////////////////////////////////////////////////////////////////

INT LcxSmd_AndDeviceOpen()
{
#if defined(__ANDROID__)
	INT hr = LC_OK;

	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("SndDeviceOpen::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	Jni_SndCreate	= pEnv->GetStaticMethodID(clzz, "JniCB_MpCreate" , "(Ljava/lang/String;)I")	;	if(0 == Jni_SndCreate	){	LOGE("GetStaticMethodID::Err::JniCB_MpCreate  ----------\n");	return LC_EFAIL;	}
	Jni_SndDestroy	= pEnv->GetStaticMethodID(clzz, "JniCB_MpDestroy", "(I)I")					;	if(0 == Jni_SndDestroy	){	LOGE("GetStaticMethodID::Err::JniCB_MpDestroy ----------\n");	return LC_EFAIL;	}
	Jni_SndSetData	= pEnv->GetStaticMethodID(clzz, "JniCB_MpSetData", "(III)I")				;	if(0 == Jni_SndSetData	){	LOGE("GetStaticMethodID::Err::JniCB_MpSetData ----------\n");	return LC_EFAIL;	}
	Jni_SndGetData	= pEnv->GetStaticMethodID(clzz, "JniCB_MpGetData", "(II)I")					;	if(0 == Jni_SndGetData	){	LOGE("GetStaticMethodID::Err::JniCB_MpGetData ----------\n");	return LC_EFAIL;	}
	Jni_SndPlay		= pEnv->GetStaticMethodID(clzz, "JniCB_MpPlay"   , "(I)I")					;	if(0 == Jni_SndPlay		){	LOGE("GetStaticMethodID::Err::JniCB_MpPlay    ----------\n");	return LC_EFAIL;	}
	Jni_SndStop		= pEnv->GetStaticMethodID(clzz, "JniCB_MpStop"   , "(I)I")					;	if(0 == Jni_SndStop		){	LOGE("GetStaticMethodID::Err::JniCB_MpStop    ----------\n");	return LC_EFAIL;	}
	Jni_SndPause	= pEnv->GetStaticMethodID(clzz, "JniCB_MpPause"  , "(I)I")					;	if(0 == Jni_SndPause	){	LOGE("GetStaticMethodID::Err::JniCB_MpPause   ----------\n");	return LC_EFAIL;	}
	Jni_SndReset	= pEnv->GetStaticMethodID(clzz, "JniCB_MpReset"  , "(I)I")					;	if(0 == Jni_SndReset	){	LOGE("GetStaticMethodID::Err::JniCB_MpReset   ----------\n");	return LC_EFAIL;	}

	//LOGI("Play Sound Environemnt:: "
	//	"Play-0x%X, Stop-0x%X, Pause-0x%X \n"
	//	, (int)Jni_SndPlay, (int)Jni_SndStop, (int)Jni_SndPause);


	if( CLcxSndSL::g_slEngObj && CLcxSndSL::g_slEngDev &&
		CLcxSndSL::g_slMixObj && CLcxSndSL::g_slMixEnv)
		return LC_OK;

	// create engine
	hr = slCreateEngine(&CLcxSndSL::g_slEngObj, 0, NULL, 0, NULL, NULL);
	assert(SL_RESULT_SUCCESS == hr);

	// realize the engine
	hr = (*CLcxSndSL::g_slEngObj)->Realize(CLcxSndSL::g_slEngObj, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == hr);

	// get the engine interface, which is needed in order to create other objects
	hr = (*CLcxSndSL::g_slEngObj)->GetInterface(CLcxSndSL::g_slEngObj, SL_IID_ENGINE, &CLcxSndSL::g_slEngDev);
	assert(SL_RESULT_SUCCESS == hr);

	// create output mix, with environmental reverb specified as a non-required interface
	const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
	const SLboolean req[1] = {SL_BOOLEAN_FALSE};
	hr = (*CLcxSndSL::g_slEngDev)->CreateOutputMix(CLcxSndSL::g_slEngDev, &CLcxSndSL::g_slMixObj, 1, ids, req);
	assert(SL_RESULT_SUCCESS == hr);

	// realize the output mix
	hr = (*CLcxSndSL::g_slMixObj)->Realize(CLcxSndSL::g_slMixObj, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == hr);

	// get the environmental reverb interface
	// this could fail if the environmental reverb effect is not available,
	// either because the feature is not present, excessive CPU load, or
	// the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
	hr = (*CLcxSndSL::g_slMixObj)->GetInterface(CLcxSndSL::g_slMixObj, SL_IID_ENVIRONMENTALREVERB,
			&CLcxSndSL::g_slMixEnv);

	if (SL_RESULT_SUCCESS == hr)
	{
		hr = (*CLcxSndSL::g_slMixEnv)->SetEnvironmentalReverbProperties( CLcxSndSL::g_slMixEnv, &CLcxSndSL::g_slMixRev);
	}
	// ignore unsuccessful hr codes for environmental reverb, as it is optional for this example

	LOGI("OpenSL Success\n");
	return LC_OK;
#endif

	return LC_EFAIL;
}


INT	LcxSmd_AndDeviceClose()
{
	if( NULL == CLcxSndSL::g_slEngObj && NULL == CLcxSndSL::g_slEngDev &&
		NULL == CLcxSndSL::g_slMixObj && NULL == CLcxSndSL::g_slMixEnv)
		return LC_OK;

	LOGI("OpenSL Close ------------------\n");

#if defined(__ANDROID__)
	// destroy output mix object, and invalidate all associated interfaces
	if(CLcxSndSL::g_slMixObj)
		(*CLcxSndSL::g_slMixObj)->Destroy(CLcxSndSL::g_slMixObj);


	// destroy engine object, and invalidate all associated interfaces
	if(CLcxSndSL::g_slEngObj)
		(*CLcxSndSL::g_slEngObj)->Destroy(CLcxSndSL::g_slEngObj);
#endif

	CLcxSndSL::g_slEngObj = NULL;
	CLcxSndSL::g_slEngDev = NULL;
	CLcxSndSL::g_slMixObj = NULL;
	CLcxSndSL::g_slMixEnv = NULL;

	return LC_OK;
}


////////////////////////////////////////////////////////////////////////////////


CLcxSndSL::CLcxSndSL()
{
	m_RscType	= 0;
	m_SndF		= 0;
	m_SndB		= 0;
	m_SndC		= 0;
	m_SndHz		= 0;
	m_SndBuf	= NULL;
	m_SndSize	= 0;

	m_state		= LCX_AV_STOP;
	m_state_s	= LCX_AV_STOP;
	m_rpt		= 1;

	m_slBufObct = NULL;
	m_slBufPlay = NULL;
	m_slBufQue	= NULL;
	m_slBufEft	= NULL;

	m_slMaxVol	= 0;
	m_slBufVol	= NULL;
}


CLcxSndSL::~CLcxSndSL()
{
	Destroy();
}


INT CLcxSndSL::Query(char* sCmd, void* pData)
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


INT CLcxSndSL::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcxSndSL::Destroy()
{
	if(!m_slBufObct)
		return;

	LOGI("Destroy the Sound Object -------------------\n");

#if defined(__ANDROID__)
	(*m_slBufObct)->Destroy(m_slBufObct);
#endif

	m_slBufObct = NULL;
	m_slBufPlay = NULL;
	m_slBufQue  = NULL;
	m_slBufEft  = NULL;

	m_slMaxVol	= 0;
	m_slBufVol	= NULL;


	if(m_RscType)
	{
		SAFE_FREE(	m_SndBuf	);
	}


	--CLcxSndSL::m_refSnd;
	if(0 == CLcxSndSL::m_refSnd)
		LcxSmd_AndDeviceClose();

}


INT CLcxSndSL::CreateFromFile(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT		hr		= LC_EFAIL;

	char*	orgFile	= (char*)p1;
	char	sMedia[LC_MAX_PATH2]={0};
	char	drive[LC_MAX_DRIVE]={0};
	char	dir  [LC_MAX_DIR  ]={0};
	char	fname[LC_MAX_FNAME]={0};
	char	ext  [LC_MAX_EXT  ]={0};


	// check reference count
	if(0 > (CLcxSndSL::m_refSnd+1))
	{
		LOGE("The reference count of sound instance is out of bound.\n");
		return LC_EFAIL;
	}


	// get the file name and extension
	LcStr_SplitPath((char*)orgFile, drive, dir, fname, ext);
	sprintf(sMedia, "%s%s%s%s", drive, dir, fname, ".gpsf");


	// Load gpsf file in embedded systems
	// for iphone, android

	m_RscType = 1;		// File type
	hr = Gps_FileRead((unsigned char**)&m_SndBuf, &m_SndF, &m_SndB, &m_SndC, &m_SndHz, &m_SndSize, sMedia);
	if( LC_FAILED(hr)   ||
		0 == m_SndB     ||
		0 == m_SndC     ||
		0 == m_SndHz    ||
		0 == m_SndSize  ||
		NULL== m_SndBuf)
	{
		SAFE_FREE(	m_SndBuf	);
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Read Sound File Failed: %s\n", (char*)orgFile);
		hr = LC_CANNT_FIND_RESOURCE;
		return hr;
	}


	if(0 == CLcxSndSL::m_refSnd)
	{
		hr = LcxSmd_AndDeviceOpen();
		if(LC_FAILED(hr))
		{
			SAFE_FREE(	m_SndBuf	);
			return hr;
		}
	}


	LOGI("Sound Info: %d - %d - %d KHz size: %d\n", m_SndB, m_SndC, m_SndHz, m_SndSize);

	hr = CreateSLSound();
	if(LC_FAILED(hr))
	{
		hr = LC_CANNT_ACQUIRE_DEVICE;
		goto ERR;
	}

	++CLcxSndSL::m_refSnd;
	return LC_OK;

ERR:
	SAFE_FREE(	m_SndBuf	);

	if(0 == CLcxSndSL::m_refSnd)
		LcxSmd_ALCDeviceClose();

	return hr;
}


INT CLcxSndSL::CreateFromMemory(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT		hr		= LC_EFAIL;
	void*	Memory	= p1;
	UINT 	MemorySize=0;
	UINT	nType	= LC_OBJ_WAV;

	if(!p1 || !p2)
		return hr;

	MemorySize = *((UINT*)p2);

	if(p3)
		nType = *((UINT*)p3);

	// check reference count
	if(0 > (CLcxSndSL::m_refSnd+1))
	{
		LOGE("The reference count of sound instance is out of bound.\n");
		return LC_EFAIL;
	}


	m_RscType = 2;		// Memory type


	if(LC_OBJ_OGG == nType)
	{
		hr = Ogg_CreateFromBuffer((oggbuf*)&m_SndBuf
								, (oggint*)&m_SndSize
								, (oggint*)&m_SndB
								, (oggint*)&m_SndC
								, (oggint*)&m_SndHz
								, (oggbuf )Memory
								, (oggint )MemorySize);
	}
	else
	{
		hr = Gps_MemRead((unsigned char**)&m_SndBuf
						, &m_SndF
						, &m_SndB
						, &m_SndC
						, &m_SndHz
						, &m_SndSize
						, Memory, MemorySize);
	}


	if( LC_FAILED(hr)   ||
		0 == m_SndB     ||
		0 == m_SndC     ||
		0 == m_SndHz    ||
		0 == m_SndSize  ||
		NULL== m_SndBuf)
	{
		SAFE_FREE(	m_SndBuf	);
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Read Memory Failed\n");
		hr = LC_CANNT_FIND_RESOURCE;
		return hr;
	}


	if(0 == CLcxSndSL::m_refSnd)
	{
		hr = LcxSmd_AndDeviceOpen();
		if(LC_FAILED(hr))
		{
			SAFE_FREE(	m_SndBuf	);
			return hr;
		}
	}


	LOGI("Sound Info: %d - %d - %d KHz size: %d\n", m_SndB, m_SndC, m_SndHz, m_SndSize);

	hr = CreateSLSound();
	if(LC_FAILED(hr))
	{
		hr = LC_CANNT_ACQUIRE_DEVICE;
		goto ERR;
	}

	++CLcxSndSL::m_refSnd;
	return LC_OK;

ERR:
	SAFE_FREE(	m_SndBuf	);

	if(0 == CLcxSndSL::m_refSnd)
		LcxSmd_ALCDeviceClose();

	return hr;
}


INT CLcxSndSL::CreateFromBuffer(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT				hr = LC_EFAIL;
	TLCX_SOUND_INF*	pInf = (TLCX_SOUND_INF*)p1;

	if(	NULL == pInf ||
		NULL == pInf->Buff ||
		0xCC == pInf->Size ||
		0xCC == pInf->SndHz )
	{
		LOGE("SL Sound Buffer Error.\n");
		return LC_EFAIL;
	}


	if(0 > (CLcxSndSL::m_refSnd+1))
	{
		LOGE("The reference count of sound instance is out of bound.\n");
		return LC_EFAIL;
	}

	if(0 == CLcxSndSL::m_refSnd)
	{
		hr = LcxSmd_AndDeviceOpen();
		if(LC_FAILED(hr))
			return hr;
	}

	m_RscType	= 0;		// Memory type
	m_SndF		= pInf->Format;
	m_SndB		= pInf->SndB;
	m_SndC		= pInf->SndC;
	m_SndHz		= pInf->SndHz;
	m_SndBuf	= pInf->Buff;
	m_SndSize	= pInf->Size;

	LOGI("Sound Info: %d - %d - %d KHz size: %d\n", m_SndB, m_SndC, m_SndHz, m_SndSize);

	hr = CreateSLSound();
	if(LC_FAILED(hr))
	{
		hr = LC_CANNT_ACQUIRE_DEVICE;
		return hr;
	}

	++CLcxSndSL::m_refSnd;
	return LC_OK;
}



INT CLcxSndSL::CreateSLSound()
{
	INT				hr = LC_EFAIL;

#if defined(__ANDROID__)
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};

// struct SLDataFormat_PCM {
//	SLuint32 		formatType;
//	SLuint32 		numChannels;
//	SLuint32 		samplesPerSec;
//	SLuint32 		bitsPerSample;
//	SLuint32 		containerSize;
//	SLuint32 		channelMask;
//	SLuint32		endianness;
//} format_pcm = {
//		SL_DATAFORMAT_PCM					// pcm
//		, 1									// mono
//		, 22050 * 1000						// SL_SAMPLINGRATE_22_05					// 22050 kHz
//		, SL_PCMSAMPLEFORMAT_FIXED_16		// 16bit
//		, SL_PCMSAMPLEFORMAT_FIXED_16
//		, SL_SPEAKER_FRONT_CENTER
//		, SL_BYTEORDER_LITTLEENDIAN
//	};


	SLDataFormat_PCM format_pcm =
	{
		SL_DATAFORMAT_PCM					// pcm
		, m_SndC
		, m_SndHz * 1000
		, SL_PCMSAMPLEFORMAT_FIXED_16		// 16bit
		, SL_PCMSAMPLEFORMAT_FIXED_16
		, SL_SPEAKER_FRONT_CENTER
		, SL_BYTEORDER_LITTLEENDIAN
	};
	SLDataSource audioSrc = {&loc_bufq, &format_pcm};


	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, g_slMixObj};
	SLDataSink audioSnk = {&loc_outmix, NULL};

	//(V)::AFEW
	// create audio player
	const SLInterfaceID ids[3] ={
		SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME
	};

	const SLboolean		req[3] ={
		SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE
	};


	hr = (*g_slEngDev)->CreateAudioPlayer(g_slEngDev, &m_slBufObct, &audioSrc
										, &audioSnk, 3, ids, req);
	if(SL_RESULT_SUCCESS != hr)
	{
		LOGE("CreateAudioPlayer() Failed: %d, %s \t", __LINE__, __FILE__);
		LOGE("hr: 0x%x 0x%x\n", hr, (UINT)g_slEngDev);
		hr = LC_CANNT_ACQUIRE_DEVICE;
		return hr;
	}



	// realize the player
	hr = (*m_slBufObct)->Realize(m_slBufObct, SL_BOOLEAN_FALSE);
	if(SL_RESULT_SUCCESS != hr)
	{
		LOGE("Audio Realize() Failed: %d, %s \t", __LINE__, __FILE__);
		hr = LC_CANNT_ACQUIRE_DEVICE;
		return hr;
	}

	// get the play interface
	hr = (*m_slBufObct)->GetInterface(m_slBufObct, SL_IID_PLAY, &m_slBufPlay);
	if(SL_RESULT_SUCCESS != hr)
	{
		LOGE("Audio GetInterface() for Play Failed: %d, %s \t", __LINE__, __FILE__);
		hr = LC_CANNT_ACQUIRE_DEVICE;
		return hr;
	}

	// get the buffer queue interface
	hr = (*m_slBufObct)->GetInterface(m_slBufObct, SL_IID_BUFFERQUEUE, &m_slBufQue);
	if(SL_RESULT_SUCCESS != hr)
	{
		LOGE("Audio GetInterface() for Buffer Failed: %d, %s \t", __LINE__, __FILE__);
		hr = LC_CANNT_ACQUIRE_DEVICE;
		return hr;
	}

	// register callback on the buffer queue
	hr = (*m_slBufQue)->RegisterCallback(m_slBufQue, PlayerCallback, this);
	if(SL_RESULT_SUCCESS != hr)
	{
		LOGE("RegisterCallback() Failed: %d, %s \t", __LINE__, __FILE__);
		hr = LC_CANNT_ACQUIRE_DEVICE;
		return hr;
	}

	// get the effect send interface
	hr = (*m_slBufObct)->GetInterface(m_slBufObct, SL_IID_EFFECTSEND, &m_slBufEft);
	if(SL_RESULT_SUCCESS != hr)
	{
		LOGE("Audio GetInterface() for EffectSend Failed: %d, %s \t", __LINE__, __FILE__);
		hr = LC_CANNT_ACQUIRE_DEVICE;
		return hr;
	}


	// get the volume interface
	hr = (*m_slBufObct)->GetInterface(m_slBufObct, SL_IID_VOLUME, &m_slBufVol);
	if(SL_RESULT_SUCCESS != hr)
	{
		LOGE("Audio GetInterface() for Volume Failed: %d, %s \t", __LINE__, __FILE__);
		hr = LC_CANNT_ACQUIRE_DEVICE;
		return hr;
	}

	// get the max volume level
	(*m_slBufVol)->GetMaxVolumeLevel(m_slBufVol, &m_slMaxVol);


	// set the player's state to playing
	hr = (*m_slBufPlay)->SetPlayState(m_slBufPlay, SL_PLAYSTATE_PLAYING);
	if(SL_RESULT_SUCCESS != hr)
	{
		LOGE("Audio SetPlayState() Failed: %d, %s \t", __LINE__, __FILE__);
		hr = LC_CANNT_ACQUIRE_DEVICE;
		return hr;
	}

#endif

	return LC_OK;
}


// this callback handler is called every time a buffer finishes playing
// the ctx is RegisterCallback 3rd argument
void CLcxSndSL::PlayerCallback(SLAndQueItf que, void* ctx)
{
	CLcxSndSL*	pSnd = NULL;
	INT			Rpt = 0;


	if(NULL== ctx)
		return;

	pSnd = (CLcxSndSL*)ctx;

	if(!que || que != pSnd->GetBufQue())
		return;


	pSnd->SetState(LCX_AV_STOP);		// setup the state
	Rpt = pSnd->GetRepeat();			// get the repeat count

	if(LCX_AV_INFINITE == (UINT)Rpt)	// Infinite then play again
	{
		pSnd->Play();
		return;
	}

	LOGI("PlayerCallback: Remain repeat: %d  Object: 0x%X  Que: 0x%X\n", Rpt, (UINT)ctx, (UINT)que);

	if(0 < Rpt)
		--Rpt;

	pSnd->SetRepeat(Rpt);

	if(0 == Rpt)
		return;


	// repeat...
	pSnd->Play();
}



INT  CLcxSndSL::Play()
{
	INT hr = 0;
	if(0 == m_rpt)
		return LC_EFAIL;


	if(LCX_AV_PLAY == m_state)
		return LC_OK;


	LOGI("Play: Repeat: %d  Que:0x%X  Src: 0x%X  Size: %d\n", m_rpt, (UINT)m_slBufQue, (UINT)m_SndBuf, m_SndSize);

#if defined(__ANDROID__)
	if(LCX_AV_STOP == m_state)
	{
		hr = (*m_slBufQue)->Enqueue(m_slBufQue, m_SndBuf, m_SndSize);
		if(SL_RESULT_SUCCESS != hr)
		{
			LOGW("Enqueue Faild-------------------------------------\n");
			return LC_EFAIL;
		}
	}
	else if(LCX_AV_PAUSE == m_state)
	{
		hr = (*m_slBufPlay)->SetPlayState(m_slBufPlay, SL_PLAYSTATE_PLAYING);
	}
#endif

	m_state = LCX_AV_PLAY;

	return LC_OK;
}


INT  CLcxSndSL::Stop()
{
	INT hr =0;

	if(LCX_AV_PLAY != m_state)
		return LC_OK;

#if defined(__ANDROID__)
	hr = (*m_slBufQue)->Clear(m_slBufQue);
	assert(SL_RESULT_SUCCESS == hr);
#endif

	m_state = LCX_AV_STOP;

	LOGI("Open SL Stop ----\n");

	return LC_OK;
}



// Reset is Rewind and pre-play
// Sequence
// Stop -> Play and Pause

INT  CLcxSndSL::Reset()
{
	if(LCX_AV_PLAY != m_state)
		return LC_OK;

	this->Stop();
	this->Play();
	this->Pause();

	return LC_OK;
}


INT  CLcxSndSL::Pause()
{
	INT hr =0;

	if(LCX_AV_PLAY != m_state)
		return LC_OK;


#if defined(__ANDROID__)
	hr = (*m_slBufPlay)->SetPlayState(m_slBufPlay, SL_PLAYSTATE_PAUSED);
	assert(SL_RESULT_SUCCESS == hr);
#endif

	m_state = LCX_AV_PAUSE;

	LOGI("Open SL Pause ----\n");

	return LC_OK;
}


void  CLcxSndSL::SetVolume(FLOAT fVol)
{
#if defined(__ANDROID__)
	int hr = 0;
	int vol = m_slMaxVol;
	
	vol += 0x8000;
	vol *= fVol;

	if(vol>0x7FFF)
		vol = 0x7FFF;

	vol -= 0x8000;

	SLmillibel millibel = (SLmillibel)vol;

	hr = (*m_slBufVol)->SetVolumeLevel(m_slBufVol, millibel);
	assert(SL_RESULT_SUCCESS == hr);
#endif
}


FLOAT CLcxSndSL::GetVolume()
{
	FLOAT fVol = 1.0F;

#if defined(__ANDROID__)
	int hr = 0;

	SLmillibel millibel = 0;
	hr = (*m_slBufVol)->GetVolumeLevel(m_slBufVol, &millibel);
	assert(SL_RESULT_SUCCESS == hr);

	int vol      = (int)(millibel  ) + 0x8000;
	int volRange = (int)(m_slMaxVol) + 0x8000;
	
	fVol = (FLOAT)(vol)/(FLOAT)(volRange);	
#endif

	return fVol;
}



////////////////////////////////////////////////////////////////////////////////


INT LcxSmd_CreateSLSndFromFile(char* sCmd
							, PLCX_SOUND* pOut	// Output instance
							, LC_HANDLE p1		// Sound File Name
							, LC_HANDLE p2		// User Data(Option)
							, LC_HANDLE p3		// No Use
							, LC_HANDLE p4		// No Use
							)
{
	INT			hr = LC_EFAIL;
	UINT		nType =0;

	char*		orgFile	= (char*)p1;
	char		sMedia[LC_MAX_PATH2]={0};
	char		Ext[16]={0};
	LcStr_SplitPath(orgFile, NULL, NULL, sMedia, Ext );


	*pOut = NULL;


	BYTE*		buf = NULL;
	ULONG		len = 0;
	const char*	sApkFile = LcSys_AndroidApkFile(NULL);

	if     (0== LcStr_Stricmp(".ogg", Ext))
	{
		nType = LC_OBJ_OGG;
		strcat(sMedia, ".ogg");
	}
	else if(0== LcStr_Stricmp(".wav", Ext))
	{
		nType = LC_OBJ_WAV;
		strcat(sMedia, ".gpsf");
	}
	else
		return LC_INVALID_RESOURCE;


#if defined(__ANDROID__)
	if(LC_OBJ_OGG == nType)
	{
		CLcxSndJni*	pObj = new CLcxSndJni;

		hr = pObj->Create(p1, p2, p3, p4);
		if(LC_FAILED(hr))
		{
			delete pObj;
			return LC_CANNT_ALLOC;
		}

		LcVar_IncRefCountObject();
		*pOut = pObj;
		return LC_OK;
	}


	hr = Lzip_UnzipbyName(&buf, &len, sApkFile, sMedia, LC_TRUE);
	LOGI("Create SL Sound Unzip: %x %ld %d %s\n", (UINT)buf, len, hr, sMedia);
	if(LC_FAILED(hr))
		return LC_CANNT_ALLOC;


	hr = LcxSmd_CreateSLSndFromMemory(NULL, pOut, buf, &len, &nType, p2);
	LOGI("Create SL Sound From Memory hr: %d\n", hr);
	SAFE_FREE(	buf	);

#endif

	return hr;
}


INT LcxSmd_CreateSLSndFromMemory(char* sCmd
								, PLCX_SOUND* pOut
								, LC_HANDLE p1
								, LC_HANDLE p2
								, LC_HANDLE p3
								, LC_HANDLE p4
								)
{
	INT			hr = 0;
	CLcxSndSL*	pObj = NULL;

	*pOut = NULL;
	pObj = new CLcxSndSL;

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


INT LcxSmd_CreateSLSndFromBuffer(char* sCmd
								, PLCX_SOUND* pOut
								, LC_HANDLE p1
								, LC_HANDLE p2
								, LC_HANDLE p3
								, LC_HANDLE p4
								)
{
	INT			hr = 0;
	CLcxSndSL*	pObj = NULL;

	*pOut = NULL;
	pObj = new CLcxSndSL;

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
