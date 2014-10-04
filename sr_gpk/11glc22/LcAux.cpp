// Implementation of the LC System.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include<errno.h>


#if defined(_WIN32)
  #define _WIN32_WINNT 0x0500
  #include <Windows.h>

#if defined(_MSC_VER)
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <io.h>
  #include <direct.h>
#endif

#elif defined(__GNUC__)

#if defined(__BADA__)
  #include <FBaseSys.h>
  #include <sys/time.h>
#else
  #include <dirent.h>
#endif

  #include <fcntl.h>
  #include <signal.h>
  #include <unistd.h>
  #include <sys/stat.h>

#endif


#if defined(__APPLE__) || defined(__IPHONE__)
  #include <sys/time.h>
  #include <sys/types.h>
  #include <unistd.h>
#endif


#include <LcType.h>

#include "LcAux.h"


/* Subdirectory */
#ifndef _A_SUBDIR
#define _A_SUBDIR       0x10
#endif

/* Archive file */
#ifndef _A_ARCH
#define _A_ARCH         0x20
#endif



////////////////////////////////////////////////////////////////////////////////
// Time


INT	LcSys_tzset()
{
#if defined(_MSC_VER)
	_tzset();
#else
	tzset();
#endif

	return LC_OK;
}


namespace LCSYS
{
#if defined(_WIN32)
  DOUBLE g_BeginTime = ::GetTickCount();

#else

  #if HAVE_CLOCK_GETTIME
    struct timespec g_BeginTime={0};
  #else
    struct timeval g_BeginTime={0};
  #endif
#endif

	char g_DirApp[LC_MAX_PATH2]={0};		// application for reading and writing folder
	char g_DirApk[LC_MAX_PATH2]={0};		// Apk folder
	char g_DirRsc[LC_MAX_PATH2]={0};		// Resource Directory for iphone
}



INT LcSys_InitTime()
{
#if defined(_WIN32)
	LCSYS::g_BeginTime = ::GetTickCount();
#else

#if HAVE_CLOCK_GETTIME
	clock_gettime(CLOCK_MONOTONIC,&LCSYS::g_BeginTime);

#else
	gettimeofday( &LCSYS::g_BeginTime, NULL);

#endif	// !HAVE_CLOCK_GETTIME

#endif	// ! WIN32

	return LC_OK;
}




DOUBLE LcSys_GetTickCount()
{
	DOUBLE ticks=0;

#if defined(_WIN32)
	ticks  = ::GetTickCount();
	ticks -= LCSYS::g_BeginTime;

#else

	#if HAVE_CLOCK_GETTIME
		struct timespec now={0};

		clock_gettime(CLOCK_MONOTONIC,&now);

		ticks  = DOUBLE(now.tv_sec - LCSYS::g_BeginTime.tv_sec ) * 1000.;
		ticks += DOUBLE(now.tv_nsec- LCSYS::g_BeginTime.tv_nsec) / 1000000.;

	#else
		struct timeval now={0};

		gettimeofday( &now, NULL);

		ticks  = DOUBLE(now.tv_sec - LCSYS::g_BeginTime.tv_sec ) * 1000.;
		ticks += DOUBLE(now.tv_usec- LCSYS::g_BeginTime.tv_usec) / 1000.;

	#endif	// !HAVE_CLOCK_GETTIME

#endif	// ! WIN32

	return ticks;
}


DOUBLE LcSys_TimeGetTime()
{
	DOUBLE ticks=0;

#if defined(_WIN32)
	//ticks  = ::timeGetTime();
	ticks  = ::GetTickCount();
	ticks -= LCSYS::g_BeginTime;

#else

	#if HAVE_CLOCK_GETTIME
		struct timespec now={0};

		clock_gettime(CLOCK_MONOTONIC,&now);

		ticks  = DOUBLE(now.tv_sec - LCSYS::g_BeginTime.tv_sec ) * 1000.;
		ticks += DOUBLE(now.tv_nsec- LCSYS::g_BeginTime.tv_nsec) / 1000000.;

	#else
		struct timeval now={0};

		gettimeofday( &now, NULL);

		ticks  = DOUBLE(now.tv_sec - LCSYS::g_BeginTime.tv_sec ) * 1000.;
		ticks += DOUBLE(now.tv_usec- LCSYS::g_BeginTime.tv_usec) / 1000.;

	#endif	// !HAVE_CLOCK_GETTIME

#endif	// ! WIN32

	return ticks;
}


// Hi PerformanceCounter
DOUBLE LcSys_GetTime()
{
	DOUBLE					ticks = 0.;

#if defined(_WIN32)

	static LARGE_INTEGER	lFreq={0};
	static BOOL				bQuery=LC_FALSE;
	static	BOOL			bGetTime=LC_FALSE;

	LARGE_INTEGER			lCur={0};

	// Enable Hi resolution Timer?
	if(LC_FALSE == bQuery && LC_FALSE ==bGetTime)
	{
		bQuery = ::QueryPerformanceFrequency( &lFreq);

		if(!bQuery)
			bGetTime = LC_TRUE;
	}

	if(bQuery)
	{
		::QueryPerformanceCounter(&lCur);
		ticks = DOUBLE(lCur.QuadPart) * 1000./ DOUBLE(lFreq.QuadPart);
	}
	else
	{
		//ticks = ::timeGetTime();
		ticks = ::GetTickCount();
	}

	ticks -= LCSYS::g_BeginTime;


#else

	#if HAVE_CLOCK_GETTIME
		struct timespec now={0};

		clock_gettime(CLOCK_MONOTONIC,&now);

		ticks  = DOUBLE(now.tv_sec - LCSYS::g_BeginTime.tv_sec ) * 1000.;
		ticks += DOUBLE(now.tv_nsec- LCSYS::g_BeginTime.tv_nsec) / 1000000.;

	#else
		struct timeval now={0};

		gettimeofday( &now, NULL);

		ticks  = DOUBLE(now.tv_sec - LCSYS::g_BeginTime.tv_sec ) * 1000.;
		ticks += DOUBLE(now.tv_usec- LCSYS::g_BeginTime.tv_usec) / 1000.;

	#endif	// !HAVE_CLOCK_GETTIME

#endif	// ! WIN32

	return ticks;
}




DINT LcSys_DateandTime(char *tmpbuf/*need 128byte*/)
{
	time_t ltime;

	time( &ltime );
	sprintf(tmpbuf, "%s", ctime( &ltime ) );

	return ltime;
}




////////////////////////////////////////////////////////////////////////////////

extern INT LcVar_IncRefCountObject();
extern INT LcVar_DecRefCountObject();

class LcTimer : public ILC_Timer
{
protected:
	LC_HANDLE	m_ID;		// handle for windows system and id for linux
	UINT		m_bgn;		// start time
	UINT		m_ntv;		// time interval
	UINT		m_tot;		// timer event total count

public:
	UINT		m_cnt;		// count
	PLC_MSGPRC	m_pPrc;		// Message Proc
	LC_HANDLE	m_pPtr;		// User data

public:
	LcTimer();
	virtual ~LcTimer();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();

	virtual INT		Set(UINT bgn, UINT interval, UINT count);
	virtual INT		Start();
	virtual INT		Stop();
	virtual INT		Reset();
	virtual UINT		GetCount()		{	return m_cnt;		}
	virtual UINT		GetRemainCount(){	return m_tot-m_cnt;	}
	virtual LC_HANDLE	GetUserData()	{	return m_pPtr;		}

	void	Destroy();
	void	MsgPrc(LC_HANDLE p1=0, LC_HANDLE p2=0, LC_HANDLE p3=0, LC_HANDLE p4=0)
	{
		this->m_pPrc->MsgPrc(this, p1, p2, p3);
	}

#if defined(_WIN32) || defined(_MSC_VER)
	static void CALLBACK TimerPrc(LC_HANDLE handle, BYTE cTimerOrWaitFired)
	{
		LcTimer* pTimer = (LcTimer*)handle;

		INT val = cTimerOrWaitFired;

		if(0xFFFFFFFF != pTimer->m_cnt)
		{
			++pTimer->m_cnt;

			// timer stop
			if(pTimer->m_cnt >= pTimer->m_tot)
				pTimer->Stop();
		}

		pTimer->MsgPrc( (LC_HANDLE)val, NULL, NULL, NULL);
	}
#else

	static INT	m_timerInit;
	static void TimerPrc(int sig, siginfo_t *si, void *context)
	{
		LcTimer* pTimer = (LcTimer*)si->si_value.sival_int;

		if(0xFFFFFFFF != pTimer->m_cnt)
		{
			++pTimer->m_cnt;

			// timer stop
			if(pTimer->m_cnt >= pTimer->m_tot)
				pTimer->Stop();
		}

		pTimer->MsgPrc( (LC_HANDLE)sig, (LC_HANDLE)context, NULL, NULL);
	}
#endif
};


#if !defined(_WIN32) && !defined(_MSC_VER)
INT	LcTimer::m_timerInit = 0;
#endif


LcTimer::LcTimer()
{
	m_ID	= NULL;
	m_pPrc	= NULL;
	m_bgn	= 0;
	m_tot	= 0;
	m_cnt	= 0;
	m_ntv	= 0;

	m_pPtr	= NULL;
}


LcTimer::~LcTimer()
{
	Destroy();
}


void LcTimer::Destroy()
{
	Stop();

	m_ID  = NULL;
	m_bgn = 0;
	m_tot = 0;
	m_cnt = 0;
	m_ntv = 0;

	if(m_pPrc)
	{
		delete m_pPrc;
		m_pPrc = NULL;
	}

	m_pPtr	= NULL;
}


INT LcTimer::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT LcTimer::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();
	return hr;
}


INT LcTimer::Set(UINT bgn, UINT interval, UINT count)
{
	m_bgn = bgn;
	m_ntv = interval;
	m_tot = count;
	m_cnt = 0;

	return LC_OK;
}


INT LcTimer::Start()
{
	INT hr =0;

#if defined(_WIN32) || defined(_MSC_VER)

	hr = ::CreateTimerQueueTimer(&m_ID
								, NULL
								, LcTimer::TimerPrc
								, this
								, (DWORD)m_bgn
								, (DWORD)m_ntv
								, WT_EXECUTEDEFAULT);

#else
	timer_t		tmId={0};
	sigevent_t	sigEvt={{0},};
	itimerspec	its={{0},};

	long scnd = 0;
	long nano = 0;

	if(0 == LcTimer::m_timerInit)
	{
		LcTimer::m_timerInit = 1;

		struct sigaction sa;

		sa.sa_flags 	= SA_SIGINFO;
		sa.sa_sigaction = LcTimer::TimerPrc;

		sigemptyset(&sa.sa_mask);

		hr = sigaction(SIGRTMIN, &sa, NULL);
		if(0>hr)
		{
			LOGE("LcTimer::sigaction failed\n");
			return LC_EFAIL;
		}
	}

	sigEvt.sigev_notify			 = SIGEV_SIGNAL;
	sigEvt.sigev_signo			 = SIGRTMIN;
	sigEvt.sigev_value.sival_int = (int)this;

	hr = timer_create(CLOCK_REALTIME, &sigEvt, &tmId);

	if(0>hr)
	{
		LOGE("LcTimer::timer_create failed\n");
		return LC_EFAIL;
	}

	if(0<m_bgn)
	{
		scnd = m_bgn/1000;
		nano  = (m_bgn - scnd * 1000) * 1000000;

		// initial expiration
		its.it_value.tv_sec 	= scnd;
		its.it_value.tv_nsec 	= nano;

		scnd = m_ntv/1000;
		nano  = (m_ntv - scnd * 1000) * 1000000;

		// timer interval
		its.it_interval.tv_sec	= scnd;
		its.it_interval.tv_nsec	= nano;
	}
	else
	{
		scnd = m_ntv/1000;
		nano  = (m_ntv - scnd * 1000) * 1000000;

		its.it_value.tv_sec 	= scnd;
		its.it_value.tv_nsec 	= nano;
		its.it_interval.tv_sec	= scnd;
		its.it_interval.tv_nsec	= nano;
	}

	hr = timer_settime(tmId, 0, &its, NULL);
	if(0>hr)
	{
		LOGE("LcTimer::timer_settime failed\n");
		return LC_EFAIL;
	}

	// Save Timer Inforamtion
	m_ID 	= (LC_HANDLE)tmId;
#endif

	return hr;
}


INT LcTimer::Stop()
{
	INT hr =0;

	if(NULL == m_ID)
		return hr;

#if defined(_WIN32) || defined(_MSC_VER)
	hr = ::DeleteTimerQueueTimer(NULL, m_ID, NULL);
	hr = ::CloseHandle (m_ID);
#else
	hr = timer_delete((timer_t)m_ID);
	if(0>hr)
		LOGE("Timer Setting Failed!\n");
#endif

	m_ID = NULL;

	return hr;
}


INT LcTimer::Reset()
{
	m_cnt = 0;

	return LC_OK;
}


INT LcSys_TimerCreate(char*			sCmd
					, PLC_TIMER*	pOut
					, UINT			begin
					, UINT			interval
					, UINT			count
					, PLC_MSGPRC	pPrc
					, LC_HANDLE		pUsrData
					)
{
	INT hr = 0;
	LcTimer* pObj = NULL;

	if(0>=interval || NULL == pPrc)
	{
		LOGE("Timer interval or event listener is not exist\n");
		return LC_EFAIL;
	}


	pObj = new LcTimer;
	pObj->m_pPrc = pPrc;
	pObj->m_pPtr = pUsrData;

	hr = pObj->Set(begin, interval, count);

	if(0>hr)
	{
		LOGE("LcSys_TimerCreate Failed\n");
		delete pObj;
		return LC_EFAIL;
	}

	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}




////////////////////////////////////////////////////////////////////////////////


INT LcSys_Sleep(UINT Milliseconds)
{
#if defined(_WIN32)
	Sleep(Milliseconds);
#else
	Milliseconds *= 1000;
    if (Milliseconds == 0)
        Milliseconds = 1;
    usleep( Milliseconds );
#endif

	return LC_OK;
}



INT LcSys_RandSeedSet(ULONG dSeed)
{
	if(0 == dSeed)
	{
		DOUBLE ticks=0;

	#if defined(_WIN32)
		//ticks  = ::timeGetTime();
		ticks  = ::GetTickCount();

	#else

		#if HAVE_CLOCK_GETTIME
			struct timespec now={0};

			clock_gettime(CLOCK_MONOTONIC,&now);

			ticks  = DOUBLE(now.tv_sec ) * 1000.;
			ticks += DOUBLE(now.tv_nsec) / 1000000.;

		#else
			struct timeval now={0};

			gettimeofday( &now, NULL);

			ticks  = DOUBLE(now.tv_sec ) * 1000.;
			ticks += DOUBLE(now.tv_usec) / 1000.;

		#endif	// !HAVE_CLOCK_GETTIME

	#endif	// ! WIN32



		dSeed = (ULONG)(ticks);
	}

	srand( dSeed );

	return LC_OK;
}

INT LcSys_Rand(INT _min, INT _max)
{
	if( _min == _max)
		return 0;

	INT n = lc_min(_min, _max);
	INT x = lc_max(_min, _max);
	INT w = x-n +1;
	INT v = rand() % w + n;

	return v;
}


#if !defined(__APPLE__) && !defined(__IPHONE__)
INT LcSys_ResourcePath(char* sOut, const char* sPath)
{
	FILE* fp = NULL;
	LONG lSize = 64;

#if defined(__BADA__)

	sprintf(sOut, "/Res/%s", sPath);

	if(NULL ==(fp = fopen(sOut, "r")))
		return LC_EFAIL;

#elif defined(__ANDROID__)

	strcpy(sOut, sPath);

//	sprintf(sOut, "%s%s", LcSys_AppFolder(), sPath);
//	if(NULL ==(fp = fopen(sOut, "r")))
//		return LC_EFAIL;

#else
	strcpy(sOut, sPath);

	if(NULL ==(fp = fopen(sPath, "r")))
		return LC_EFAIL;

#endif

	if(fp)
	{
		fseek(fp, 0, SEEK_END);
		lSize = ftell(fp);
		fclose(fp);
	}

	if(4>lSize)
		return LC_EFAIL;

	return LC_OK;
}


const char* LcSys_AppFolder(char* apkName)
{
#if defined(__ANDROID__)
	//char apkName[] ="com.galic.totemIST";
	FILE* fp = NULL;
	char Apk[LC_MAX_PATH2]= {0};


	if(NULL == apkName)
	{
		if( 0 == LCSYS::g_DirApp[0] && 0 == LCSYS::g_DirApp[1] &&
			0 == LCSYS::g_DirApp[2] && 0 == LCSYS::g_DirApp[3])
			return NULL;


		return LCSYS::g_DirApp;
	}


	if( LCSYS::g_DirApp[0] && LCSYS::g_DirApp[1] &&
		LCSYS::g_DirApp[2] && LCSYS::g_DirApp[3])
	{
		//LOGW("Apkdir: %s\n", LCSYS::g_DirApp);
		return LCSYS::g_DirApp;
	}

	// copy to apk name and set the full name of apk
	sprintf(Apk, "/data/data/%s", apkName);		//"/data/app/com.galic.gpack.native_activity-%d.apk"
	fp = fopen(Apk, "r");
	if(fp)
	{
		strcpy(LCSYS::g_DirApp, Apk);
		fclose(fp);

		int len = strlen(LCSYS::g_DirApp);

		if(1 == len && '.' == LCSYS::g_DirApp[len-1])
				LCSYS::g_DirApp[len] ='/';

		if(1 < len && '/' != LCSYS::g_DirApp[len-1] && '\\' != LCSYS::g_DirApp[len-1])
				LCSYS::g_DirApp[len] ='/';


		LOGI("Apkdir: %s\n", LCSYS::g_DirApp);
		return LCSYS::g_DirApp;
	}

	LOGE("Apkdir Failed: %s\n", apkName);
	return NULL;

#else

	if(0 == LCSYS::g_DirApp[0] && 0 == LCSYS::g_DirApp[1] &&
	   0 == LCSYS::g_DirApp[2] && 0 == LCSYS::g_DirApp[3])
	{
		getcwd(LCSYS::g_DirApp, LC_MAX_PATH2);

		int len = strlen(LCSYS::g_DirApp);

		if(1 == len && '.' == LCSYS::g_DirApp[len-1])
				LCSYS::g_DirApp[len] ='/';

		if(1 < len && '/' != LCSYS::g_DirApp[len-1] && '\\' != LCSYS::g_DirApp[len-1])
				LCSYS::g_DirApp[len] ='/';
	}

	return LCSYS::g_DirApp;
#endif
}

#endif




const char* LcSys_TempFolder()
{
	return NULL;
}


const char* LcSys_AndroidApkFile(char* apkName)
{
	//char apkName[] ="com.galic.totemIST";

	FILE* fp = NULL;
	char Apk[LC_MAX_PATH2]= {0};


	if(NULL == apkName)
		return LCSYS::g_DirApk;


	if( LCSYS::g_DirApk[0] && LCSYS::g_DirApk[1] &&
		LCSYS::g_DirApk[2] && LCSYS::g_DirApk[3])
	{
		//LOGW("Apkdir: %s\n", LCSYS::g_DirApk);
		return LCSYS::g_DirApk;
	}


	// copy to apk name and set the full name of apk
	sprintf(Apk, "/data/app/%s.apk", apkName);		//"/data/app/com.galic.gpack.native_activity-%d.apk"
	fp = fopen(Apk, "r");
	if(fp)
	{
		strcpy(LCSYS::g_DirApk, Apk);
		fclose(fp);
		//LOGW("Apkdir: %s\n", LCSYS::g_DirApk);
		return LCSYS::g_DirApk;
	}


	for(int i=0; i<10; ++i)
	{
		memset(Apk, 0, LC_MAX_PATH2);
		sprintf(Apk, "/data/app/%s-%d.apk", apkName, i);

		fp = fopen(Apk, "r");

		if(fp)
		{
			strcpy(LCSYS::g_DirApk, Apk);
			fclose(fp);
			//LOGW("Apkdir: %s ---------------------------------------\n", LCSYS::g_DirApk);
			return LCSYS::g_DirApk;
		}
	}

	//LOGW("Apkdir Failed: %s\n", apkName);
	return NULL;
}



INT LcSys_GetCurrentDir(char* dir, INT MaxLength)
{
#if defined(_MSC_VER)
	if(NULL == _getcwd( dir, MaxLength) )
		return LC_EFAIL;
#else
	if(NULL ==  getcwd( dir, MaxLength) )
		return LC_EFAIL;
#endif

	return LC_OK;
}


INT LcSys_SetCurrentDir(char* dir)
{
	INT hr = LC_EFAIL;

#if defined(_MSC_VER)
	hr = _chdir(dir);
#else
	hr =  chdir(dir);
#endif

	if(0>hr)
	{
		LOGE("LcSys_SetCurrentDir Failed\n");
	}

	return hr;
}
	
INT  LcSys_SetCurrentDirWithCommandLine()
{
	INT hr = LC_OK;


#if defined(_MSC_VER)
	char	exe[LC_MAX_PATH2]={0};
	char*	dst	= NULL;
	char*	end	= NULL;

	char*	src	= NULL;
	char*	it	= NULL;

	dst		= exe;
	end		= dst;

	src = (char*)::GetCommandLine();
	it  = src;


	for( ; *it; ++it)
	{
		if('"' == *it)
			continue;


		if('\\' == *it || '/' == *it)
		{
			*dst  ='/';
			end = dst;
		}
		else
		{
			*dst  = *it;
		}

		++dst;
	}

	if(end)
	{
		*end = 0;

		if(end-exe < 258)
			*(end+1) = 0;
	}


	printf("EXE::%s\n", exe);
	if(0 == ::SetCurrentDirectory(exe))
	{
		printf("::SetCurrentDirectory Failed\n");
		hr = LC_EFAIL;
	}
#endif

	return hr;
}



////////////////////////////////////////////////////////////////////////////////
// Memory

LC_HANDLE LcMem_Allock(INT size, INT initSetVal)
{
	LC_HANDLE pOut = NULL;

	pOut = malloc(size);
	memset(pOut, initSetVal, size);
	return pOut;
}

LC_HANDLE LcMem_Reallock(LC_HANDLE org, INT size_dst, INT size_org, INT initSetVal)
{
	char* dst = NULL;
	INT d = size_dst - size_org;

	// re-alloc the dest
	dst = (char*)realloc(org, size_dst);

	if(!dst)
	{
		dst = (char*)malloc(size_dst);
		if(NULL == dst)
			return NULL;


		if(0<size_org)
		{
			if(size_org < size_dst)
				memcpy(dst, org, size_org);
			else
				memcpy(dst, org, size_dst);
		}
	}

	if(0<size_org && size_org < size_dst)
		memset(dst + size_org, initSetVal, d);

	return dst;
}


void LcMem_Free(LC_HANDLE* v)
{
	if(*v)
	{
		free(*v);
		*v = NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////
// String

LC_INLINE INT LcStr_tolower(INT c)
{
	if('A' <= c && c<='Z')
		return c + 'a' - 'A';

	return c;
}

LC_INLINE INT LcStr_toupper(INT c)
{
	if('a' <= c && c<='a')
		return c - 'a' + 'A';

	return c;
}

extern int _LcFnt_Han2UniAll(char* src, unsigned short* dst);
extern int _LcFnt_Uni2HanAll(unsigned short* src, char* dst);


INT LcStr_MultiByteToWideChar(MCHAR* dst, const char* src)
{
	return _LcFnt_Han2UniAll((char*)src, dst);
}

INT LcStr_WideCharToMultiByte(char* dst, const MCHAR* src)
{
	return _LcFnt_Uni2HanAll((unsigned short*)src, dst);
}


INT LcStr_Format(char* dest, const char* format, ...)
{
	va_list ap;

	va_start(ap, format);
	vsprintf(dest, format, ap);
	va_end(ap);

	return strlen(dest);
}



INT LcStr_Stricmp (const char* str1, const char* str2)
{
    INT s1, s2;

    do
	{
		if ( ((s1 = (unsigned char)(*(str1++))) >= 'A') && (s1 <= 'Z') )
			s1 -= 'A' - 'a';

		if ( ((s2 = (unsigned char)(*(str2++))) >= 'A') && (s2 <= 'Z') )
            s2 -= 'A' - 'a';

	} while ( s1 && (s1 == s2) );

    return(s1 - s2);
}


// same to strncasecmp
INT LcStr_Strnicmp(const char* str1, const char* str2, UINT count)
{
    INT s1, s2;

    do
	{
		if ( ((s1 = (unsigned char)(*(str1++))) >= 'A') && (s1 <= 'Z') )
			s1 -= 'A' - 'a';

		if ( ((s2 = (unsigned char)(*(str2++))) >= 'A') && (s2 <= 'Z') )
			s2 -= 'A' - 'a';

	} while ( --count && s1 && (s1 == s2) );

    return ( s1 - s2 );
}


INT  LcStr_Replace(char* dst, const char* src, const char* s_tgt, const char* s_rep)
{
	char* b = (char*)src;

	INT   lenS = strlen(src);
	INT   lenT = strlen(s_tgt);
	INT   lenR = strlen(s_rep);
	char* s = NULL;
	INT   n=0;

	INT   rcnt = 0;	// replace count

	if(0 == lenT)
		return -1;


	while(n<lenS)
	{
		s = b + n;

		if(0 != LcStr_Strncmp(s, s_tgt, lenT) )		// compare to s_tgt string
		{
			*dst++ = *s;
			 ++n;
			continue;
		}

		memcpy(dst, s_rep, lenR);
		dst += lenR;
		n   += lenT;

		++rcnt;
	}

	return rcnt;
}


INT LcStr_Strtok(char** dst,int block, int nList, char* src, const char* seps)
{
	int		cnt = 0;
	char*	token = NULL;
	char*	b = (char*)dst;

	token = strtok(src, seps);
	while(token && cnt < nList)
	{
		//printf( " %s\n", token);
		strcpy(b, token);

		b += block;
		++cnt;
		token = strtok( NULL, seps );
	}

	return cnt;
}


void LcStr_SplitPath(const char *path, char *drive, char *dir, char *fname, char *ext)
{
	register char *p;
	char *last_slash = NULL, *dot = NULL;
	unsigned len;


	if ((strlen(path) >= (LC_MAX_DRIVE - 2)) && (*(path + LC_MAX_DRIVE - 2) == ':'))
	{
		if (drive)
		{
			strncpy(drive, path, LC_MAX_DRIVE - 1);
			*(drive + LC_MAX_DRIVE-1) = '\0';
		}

		path += LC_MAX_DRIVE - 1;
	}

	else if (drive)
	{
		*drive = '\0';
	}

	/* extract path string, if any.  Path now points to the first character
	* of the path, if any, or the filename or extension, if no path was
	* specified.  Scan ahead for the last occurence, if any, of a '/' or
	* '\' path separator character.  If none is found, there is no path.
	* We will also note the last '.' character found, if any, to aid in
	* handling the extension.
	*/

	for (last_slash = NULL, p = (char *)path; *p; p++)
	{

		if (*p == '/' || *p == '\\')
			/* point to one beyond for later copy */
			last_slash = p + 1;
		else if (*p == '.')
			dot = p;
	}

	if (last_slash)
	{

		/* found a path - copy up through last_slash or max. characters
		* allowed, whichever is smaller
		*/

		if (dir)
		{
			len = lc_min((unsigned)(((char *)last_slash - (char *)path) / sizeof(char)),
				(LC_MAX_DIR - 1));
			strncpy(dir, path, len);
			*(dir + len) = '\0';
		}
		path = last_slash;
	}

	else if (dir)
	{

		/* no path found */

		*dir = '\0';
	}

	/* extract file name and extension, if any.  Path now points to the
	* first character of the file name, if any, or the extension if no
	* file name was given.  Dot points to the '.' beginning the extension,
	* if any.
	*/

	if (dot && (dot >= path))
	{
		/* found the marker for an extension - copy the file name up to
		* the '.'.
		*/
		if (fname)
		{
			len = lc_min((unsigned)(((char *)dot - (char *)path) / sizeof(char)),
				(LC_MAX_FNAME - 1));
			strncpy(fname, path, len);
			*(fname + len) = '\0';
		}
		/* now we can get the extension - remember that p still points
		* to the terminating nul character of path.
		*/
		if (ext)
		{
			len = lc_min((unsigned)(((char *)p - (char *)dot) / sizeof(char)),
				(LC_MAX_EXT - 1));
			strncpy(ext, dot, len);
			*(ext + len) = '\0';
		}
	}
	else
	{
		/* found no extension, give empty extension and copy rest of
		* string into fname.
		*/
		if (fname)
		{
			len = lc_min((unsigned)(((char *)p - (char *)path) / sizeof(char)),
				(LC_MAX_FNAME - 1));
			strncpy(fname, path, len);
			*(fname + len) = '\0';
		}
		if (ext)
		{
			*ext = '\0';
		}
	}
}


INT LcStr_MakePath(char* dstPath
				   , char* drive, char* dir, char* fname, char* ext)
{
	char* dst =(char*)dstPath;
	char* buf = NULL;

	buf = drive;
	while(buf && *buf)
		*dst++ = *buf++;

	buf = dir;
	while(buf && *buf)
		*dst++ = *buf++;

	buf = fname;
	while(buf && *buf)
		*dst++ = *buf++;

	buf = ext;
	while(buf && *buf)
		*dst++ = *buf++;

	return dst - dstPath;
}


INT LcStr_ReplacePath(char* dstPath, const char* srcPath
					, char* drive, char* dir, char* fname, char* ext)
{
	char* dst =(char*)dstPath;
	char* buf = NULL;

	char	src_drive[LC_MAX_DRIVE]={0};
	char	src_dir  [LC_MAX_DIR  ]={0};
	char	src_fname[LC_MAX_FNAME]={0};
	char	src_ext  [LC_MAX_EXT  ]={0};

	LcStr_SplitPath(srcPath, src_drive, src_dir, src_fname, src_ext);


	buf = (drive)? drive : src_drive;
	while(*buf)
			*dst++ = *buf++;

	buf = (dir)? dir : src_dir;
	while(*buf)
		*dst++ = *buf++;

	buf = (fname)? fname : src_fname;
	while(*buf)
		*dst++ = *buf++;

	buf = (ext)? ext : src_ext;
	while(*buf)
		*dst++ = *buf++;

	return dst - dstPath;
}


void LcStr_Trim(char* sBuf, const char *sTrm)
{
	char*	D= (char*)sBuf;
	char*	R= (char*)sBuf;
	char*	L= (char*)sBuf;
	char*	T= NULL;

	// find L
	while( *L )
	{
		T= (char*)sTrm;
		for(; *T && *T != *L; ++T);

		if(!*T)
			break;

		++L;
	}


	// Find R
	R = L;

	while( *R )
		++R;


	// NULL string
	if(L >= R)
	{
		*D = '\0';
		return;
	}

	--R;

	while(*R)
	{
		T= (char*)sTrm;
		for(; *T && *T != *R; ++T);

		if(!*T)
			break;

		--R;
	}

	while( L <= R)
		*(D++) = *(L++);

	*D = '\0';
}


void LcStr_TrimLeft(char* sBuf, const char *sTrm)
{
	char*	D= (char*)sBuf;
	char*	L= (char*)sBuf;
	char*	T= NULL;

	while( *L )
	{
		T= (char*)sTrm;
		for(; *T && *T != *L; ++T);

		if(!*T)
			break;

		++L;
	}

	while( *L )
		*(D++) = *(L++);

	*D = '\0';
}


void LcStr_TrimRight(char* sBuf, const char *sTrm)
{
	char*	D= (char*)sBuf;
	char*	R= (char*)sBuf;
	char*	L= (char*)sBuf;
	char*	T= NULL;

	if('\0' == *R)
	{
		*D = '\0';
		return;
	}

	while( *R )
		++R;

	--R;

	while(*R)
	{
		T= (char*)sTrm;
		for(; *T && *T != *R; ++T);

		if(!*T)
			break;

		--R;
	}

	while( L <= R)
		*(D++) = *(L++);

	*D = '\0';
}



INT LcStr_Quot(char* Dst, const char* Src)
{
	INT	nCnt  =0;
	INT bTogle=0;

	char* p = (char*)Src;
	INT	iLen = strlen(Src);

	*Dst = '\0';

	while( *p && p <= Src+iLen)
	{
		if( '\"' == *p)
		{
			bTogle ^= 1;

			if(!bTogle)
			{
				++nCnt;
				*(Dst++) = '\0';
			}
			else
			{
				++p;
				continue;
			}
		}


		if(bTogle)
			*(Dst++) = *p;

		++p;
	}

	return nCnt;
}




////////////////////////////////////////////////////////////////////////////////

TLC_STRING::TLC_STRING()
{
	m_ls = 0;
	m_lb = 0;
	m_sb = NULL;
}


TLC_STRING::TLC_STRING(const char* s)
{
	m_ls = 0;
	m_lb = 0;
	m_sb = NULL;

	char* src = (char*)s;
	CopyFrom(src);
}


TLC_STRING::TLC_STRING(const BYTE* s)
{
	m_ls = 0;
	m_lb = 0;
	m_sb = NULL;

	char* src = (char*)s;
	CopyFrom(src);
}


TLC_STRING::TLC_STRING(const TLC_STRING& rhs)
{
	m_ls = 0;
	m_lb = 0;
	m_sb = NULL;
	CopyFrom(rhs.m_sb);
}


TLC_STRING::~TLC_STRING()
{
	Destroy();
}


void TLC_STRING::Destroy()
{
	if(!m_sb)
		return;

	free(m_sb);

	m_ls = 0;
	m_lb = 0;
	m_sb = NULL;
}


INT TLC_STRING::GetLen()
{
	return m_ls;
}


const char* TLC_STRING::GetStr() const
{
	return (const char*)m_sb;
}


INT TLC_STRING::CopyFrom(const char* src)
{
	Destroy();

	if(NULL == src)
		return LC_EFAIL;

	m_ls = strlen(src);
	if(0 == m_ls)
		return LC_EFAIL;

	if( 13 <= m_ls)
		m_lb = INT( (m_ls+7)/ 4 ) * 4 * 2 + 4;	// 문자열 버퍼의 2배만큼 메모리 생성
	else
		m_lb = 44;

	m_sb = (char*)malloc(m_lb);
	memset(m_sb, 0, m_lb);

	memcpy(m_sb, src, m_ls);

	return m_ls;
}


INT TLC_STRING::CopyTo(char* dst, INT size)
{
	if(NULL == m_sb)
		return LC_EFAIL;

	INT   len = m_ls;

	len = len > size ? size : len;

	memcpy(dst, m_sb, len);
	*(dst + len) = 0;

	return len;
}


INT TLC_STRING::Replace(INT target, INT dst, INT idx)
{
	if(NULL == m_sb || 0 == m_ls)
		return LC_EFAIL;

	if(idx>=m_ls)
		return LC_EFAIL;

	if(0<idx)
	{
		m_sb[idx] = dst;
		return LC_OK;
	}

	for(INT i=0; i<m_ls; ++i)
	{
		if( m_sb[i] == target)
			m_sb[i] = dst;
	}

	return LC_OK;
}


INT TLC_STRING::Replace(const char* target, const char* dst)
{
	INT lenT = strlen(target);
	INT lenD = strlen(dst);

	INT d = lenD - lenT;
	char* s;

	if(NULL == m_sb || 0 == lenD || 0 == lenT)
		return LC_EFAIL;


	for(INT n=0; n<m_ls;)
	{
		s = m_sb + n;

		// compare to target string
		if(0 != LcStr_Strncmp(s, target, lenT) )
		{
			 ++n;
			continue;
		}


		if(d)
		{
			// increase the buffer size
			if(m_ls + d + 4 >= m_lb)
			{
				INT new_size = INT( ( (d+16) * 4 + m_ls + 7) /4 ) * 4 * 2 + 4;
				m_sb = (char*)LcMem_Reallock(m_sb, new_size, m_lb);
				m_lb = new_size;

				s = m_sb + n;
			}

			memmove(m_sb + n + lenD, m_sb + n + lenT, m_ls - (n + lenT));
			m_ls += d;
		}

		memcpy(m_sb + n, dst, lenD);

		n +=lenD;
	}

	return LC_OK;
}


INT TLC_STRING::Mid(char* dst, INT nFirst, INT nCount)
{
	if(NULL == m_sb || nFirst >= m_ls)
		return LC_EFAIL;

	INT   len = nCount;

	if(nFirst + nCount > m_ls)
		len -= (nFirst + nCount-m_ls);

	memcpy(dst, m_sb, len);
	*(dst + len) = 0;

	return len;
}


void TLC_STRING::ToUpper()
{
	if(NULL == m_sb)
		return;

	for(INT i=0; i<m_ls; ++i)
		m_sb[i] = LcStr_toupper( m_sb[i] );
}


void TLC_STRING::ToLower()
{
	if(NULL == m_sb)
		return;

	for(INT i=0; i<m_ls; ++i)
		m_sb[i] = LcStr_tolower( m_sb[i] );
}


INT TLC_STRING::Reverse()
{
	if(NULL == m_sb)
		return LC_EFAIL;

	INT tmp = 0;
	INT len = m_ls;

	for(INT i=0; i< len/2; ++i)
	{
		tmp = m_sb[i];
		m_sb[i      ] = m_sb[len-1-i];
		m_sb[len-1-i] = tmp;
	}

	return LC_OK;
}


INT TLC_STRING::Insert(INT n, char* src)
{
	INT len = strlen(src);

	if(NULL == m_sb || n >= m_ls)
		return LC_EFAIL;


	// increase the buffer size
	if(len + m_ls + 4 >= m_lb)
	{
		INT new_size = INT( (len + m_ls + 7) /4 ) * 4 * 2 + 4;
		m_sb = (char*)LcMem_Reallock(m_sb, new_size, m_lb);
		m_lb = new_size;
	}


	// move to...
	memmove(m_sb + n + len, m_sb + n, m_ls- n);
	memcpy (m_sb + n, src, len);
	m_ls += len;

	return LC_OK;
}


INT TLC_STRING::Format(const char* format, ...)
{
	char dst[4096]={0};
	va_list ap;

	va_start(ap, format);
	vsprintf(dst, format, ap);
	va_end(ap);

	return CopyFrom(dst);
}


INT TLC_STRING::Compare(const char* rhs, INT bCapitcal) const
{
	if(bCapitcal)
		return ::LcStr_Stricmp(m_sb, rhs);

	return ::LcStr_Strcmp(m_sb, rhs);
}


void TLC_STRING::Add(const char* s)
{
	(*this).operator +=(s);
}


TLC_STRING& TLC_STRING::Trim(const char *sTrm)
{
	LcStr_Trim(m_sb, sTrm);
	m_ls = ::strlen(m_sb);

	return *this;
}


TLC_STRING& TLC_STRING::TrimL(const char *sTrm)
{
	LcStr_TrimLeft(m_sb, sTrm);
	m_ls = ::strlen(m_sb);

	return *this;
}


TLC_STRING& TLC_STRING::TrimR(const char *sTrm)
{
	LcStr_TrimRight(m_sb, sTrm);
	m_ls = ::strlen(m_sb);

	return *this;
}


////////////////////////////////////////
// for operator

INT TLC_STRING::operator[](int n)
{
	if( 0> n || n>= m_ls)
		return LC_EFAIL;

	return m_sb[n];
}


TLC_STRING::operator char* ()
{
	return m_sb;
}


TLC_STRING::operator const char* () const
{
	return (const char*)(m_sb);
}


TLC_STRING::operator BYTE* ()
{
	return (BYTE*)(m_sb);
}


TLC_STRING::operator const BYTE* () const
{
	return (const BYTE*)(m_sb);
}


TLC_STRING& TLC_STRING::operator=(BYTE* src)
{
	CopyFrom( (char*)src);

	return *this;
}


TLC_STRING& TLC_STRING::operator=(const BYTE* src)
{
	CopyFrom((char*)src);

	return *this;
}


TLC_STRING& TLC_STRING::operator+=(const char* src)
{
	if(NULL == m_sb || 0 == m_ls)
	{
		CopyFrom(src);
		return *this;
	}

	INT len = strlen(src);

	// increase the buffer size
	if(len + m_ls + 4 >= m_lb)
	{
		INT new_size = INT( (len + m_ls + 7) /4 ) * 4 * 2 + 4;
		m_sb = (char*)LcMem_Reallock(m_sb, new_size, m_lb);
		m_lb = new_size;
	}

	strcat(m_sb, src);
	m_ls += len;

	return *this;
}

bool TLC_STRING::operator==(const TLC_STRING& r){	return 0 == LcStr_Strcmp(  m_sb, r.m_sb);	}
bool TLC_STRING::operator!=(const TLC_STRING& r){	return 0 != LcStr_Strcmp(  m_sb, r.m_sb);	}
bool TLC_STRING::operator< (const TLC_STRING& r){	return 0 <  LcStr_Strcmp(  m_sb, r.m_sb);	}
bool TLC_STRING::operator> (const TLC_STRING& r){	return 0 >  LcStr_Strcmp(  m_sb, r.m_sb);	}
bool TLC_STRING::operator<=(const TLC_STRING& r){	return 0 <= LcStr_Strcmp(  m_sb, r.m_sb);	}
bool TLC_STRING::operator>=(const TLC_STRING& r){	return 0 >= LcStr_Strcmp(  m_sb, r.m_sb);	}

bool TLC_STRING::operator==(const char* s)		{	return 0 == LcStr_Strcmp(  m_sb, s);		}
bool TLC_STRING::operator!=(const char* s)		{	return 0 != LcStr_Strcmp(  m_sb, s);		}
bool TLC_STRING::operator< (const char* s)		{	return 0 <  LcStr_Strcmp(  m_sb, s);		}
bool TLC_STRING::operator> (const char* s)		{	return 0 >  LcStr_Strcmp(  m_sb, s);		}
bool TLC_STRING::operator<=(const char* s)		{	return 0 <= LcStr_Strcmp(  m_sb, s);		}
bool TLC_STRING::operator>=(const char* s)		{	return 0 >= LcStr_Strcmp(  m_sb, s);		}

bool operator==(const char* s,const TLC_STRING& r){	return 0 == LcStr_Strcmp(r.m_sb, s);		}
bool operator!=(const char* s,const TLC_STRING& r){	return 0 != LcStr_Strcmp(r.m_sb, s);		}
bool operator< (const char* s,const TLC_STRING& r){	return 0 <  LcStr_Strcmp(r.m_sb, s);		}
bool operator> (const char* s,const TLC_STRING& r){	return 0 >  LcStr_Strcmp(r.m_sb, s);		}
bool operator<=(const char* s,const TLC_STRING& r){	return 0 <= LcStr_Strcmp(r.m_sb, s);		}
bool operator>=(const char* s,const TLC_STRING& r){	return 0 >= LcStr_Strcmp(r.m_sb, s);		}




////////////////////////////////////////////////////////////////////////////////
// File


INT LcFile_Info(char *sFile, ULONG* size, ULONG* time_m, ULONG* time_c)
{
	INT hr = 0;

#if defined(_MSC_VER)
	struct _stat st;
	hr = _stat(sFile, &st);

#else
	struct stat st ;
	hr = stat( sFile, &st);
#endif

	if(0 != hr)
	{
		//switch(errno)
		//{
		//	case ENOENT:
		//		LOGE("LcFile_Info::Err::File Not Found:%s\n", sFile);
		//		break;
		//	default:
		//		LOGE("LcFile_Info::Err::%s:%s\n", strerror(errno), sFile);
		//}

		if(size   )	*size   = 0;
		if(time_m )	*time_m = 0;
		if(time_c )	*time_c = 0;

		return LC_EFAIL;
	}

	if(size   )	*size   = (ULONG)st.st_size;
	if(time_m )	*time_m = (ULONG)st.st_mtime;
	if(time_c )	*time_c = (ULONG)st.st_ctime;

	return LC_OK;
}



UINT LcFile_Attribute(const char* sPath)
{
#if defined(_MSC_VER)
	return GetFileAttributes(sPath);

#else
	INT hr =0;

	struct stat file_info;
	hr = stat(sPath, &file_info);

	return hr;
#endif
}



L_PTR LcFile_FindFirst(TLC_FINDDATA* fd, const char* sSearchPath)
{
	L_PTR handle = LC_EFAIL;

#if defined(_WIN32)

	handle = (L_PTR)_findfirst(sSearchPath, (struct _finddata_t*)fd);
	return handle;

#elif !defined(__BADA__)

	DIR*	pd	= NULL;			// directory stream object.
	dirent* dt  = NULL;
	struct  stat st={0};


	pd = opendir(sSearchPath);
	if(NULL == pd)
	{
		LOGE( "opendir() failed: %s\n", sSearchPath);
		return handle;
	}

	dt = readdir(pd);

	if(NULL == dt)
	{
		LOGE("readdir() failed: %s\n", sSearchPath);
		return handle;
	}

	lstat( dt->d_name, &st);


	// setting atribute for windows
	if(DT_DIR == dt->d_type)		fd->attrib = LC_TYPE_FOLDER;
	else if(DT_REG == dt->d_type)	fd->attrib = LC_TYPE_FILE;
	else							fd->attrib = LC_TYPE_SYSTEM;

	// setting time
	fd->time_create = st.st_mtime;
	fd->time_access = st.st_atime;
	fd->time_write  = st.st_ctime;

	// Settting size
	fd->size		= st.st_size;

	// copy name
	strcpy(fd->name, dt->d_name);

	// return pd
	handle = (L_PTR)pd;
#endif

	return handle;
}


INT LcFile_FindNext(L_PTR handle, TLC_FINDDATA* fd)
{
	INT hr = LC_EFAIL;

#if defined(_WIN32)

	hr =_findnext((long)handle, (struct _finddata_t*)fd);

#elif !defined(__BADA__)

	DIR*	pd	= (DIR*)handle;
	dirent* dt  = NULL;
	struct  stat st={0};

	dt = readdir(pd);
	if(NULL == dt)
		return hr;

	lstat( dt->d_name, &st);


	// setting atribute for windows
	if(DT_DIR == dt->d_type)		fd->attrib = LC_TYPE_FOLDER;
	else if(DT_REG == dt->d_type)	fd->attrib = LC_TYPE_FILE;
	else							fd->attrib = LC_TYPE_SYSTEM;

	// setting time
	fd->time_create = st.st_mtime;
	fd->time_access = st.st_atime;
	fd->time_write  = st.st_ctime;

	// Settting size
	fd->size		= st.st_size;

	// copy name
	strcpy(fd->name, dt->d_name);

	hr = LC_OK;

#endif

	return hr;
}



INT LcFile_FindClose(L_PTR handle)
{
	INT hr = LC_EFAIL;

#if defined(_WIN32)

	hr = _findclose((long)handle);

#elif !defined(__BADA__)

	DIR* pd	= (DIR*)handle;

	hr = closedir(pd);
	if( 0 > hr)
		LOGE( "LcFile_FindClose() failed.\n");

#endif

	return hr;
}



LC_INLINE INT t_Multiple4(INT v){ return ((v + 7)/4 )*4; }

void t_LcFile_Gather(TLC_FINF** pCur, const char* sDir)
{
	char			sSearchPath[LC_MAX_PATH2]={0};
	TLC_FINDDATA	fd;
	L_PTR			handle;

	INT				result=1;

#if defined(_WIN32)
	sprintf(sSearchPath,"%s/*.*", sDir);
#else
	sprintf(sSearchPath,"%s", sDir);
#endif

	handle= LcFile_FindFirst(&fd, sSearchPath);

	if (handle == -1)
		return;

	while (result != -1)
	{
		// Folder
		if( (fd.attrib & 0x10) && '.' != fd.name[0])
		{
//			time_t	t = fd.time_write;
//
//			char	sTime[LC_MAX_PATH2]={0};
//			strcpy(sTime, ctime(&t) );
//			struct tm *gmt = gmtime(&t);
//			struct tm *lcl = localtime(&t);

			char	tNode[LC_MAX_PATH2]={0};
			INT		iLen = 0;
			sprintf(tNode, "%s/%s", sDir, fd.name);
			iLen	= strlen(tNode);
			iLen	= t_Multiple4(iLen);

			TLC_FINF*	pNodeScrpt = (TLC_FINF*)LcMem_Allock(sizeof(TLC_FINF));
			pNodeScrpt->nType	= LC_TYPE_FOLDER;			// Folder: LC_TYPE_FOLDER
			pNodeScrpt->iname	= iLen;						// Length of File Name
			pNodeScrpt->name		= (char*)LcMem_Allock(1, iLen);
			strcpy(pNodeScrpt->name, tNode);

			pNodeScrpt->lDate	= fd.time_write;
			pNodeScrpt->lBuf		= fd.size;

			if(*pCur)
				(*pCur)->pNext = pNodeScrpt;

			*pCur = pNodeScrpt;
			t_LcFile_Gather(pCur, pNodeScrpt->name);
		}


		// File
		if( (fd.attrib & 0x20) && '.' != fd.name[0])
		{
			char	tNode[LC_MAX_PATH2]={0};
			INT		iLen;
			sprintf(tNode, "%s/%s", sDir, fd.name);
			iLen	= strlen(tNode);
			iLen	= t_Multiple4(iLen);

			TLC_FINF*	pNodeScrpt = (TLC_FINF*)LcMem_Allock(sizeof(TLC_FINF));
			pNodeScrpt->nType	= LC_TYPE_FILE;			// File: 0x20
			pNodeScrpt->iname	= iLen;						// Length of File Name
			pNodeScrpt->name		= (char*)LcMem_Allock(iLen, 0x0);
			strcpy(pNodeScrpt->name, tNode);

			pNodeScrpt->lDate	= fd.time_write;
			pNodeScrpt->lBuf		= fd.size;

			if(*pCur)
				(*pCur)->pNext = pNodeScrpt;

			*pCur = pNodeScrpt;
		}

		result= LcFile_FindNext(handle, &fd);
	}

	LcFile_FindClose(handle);
}




void LcFile_GatherAllFiles(TLC_FINF** pOut, const char* sRoot)
{
	TLC_FINF*	pRoot = (TLC_FINF*)LcMem_Allock(sizeof(TLC_FINF), 0x0);
	TLC_FINF*	pCur = pRoot;
	INT			iLen = strlen(sRoot);
	iLen			= t_Multiple4(iLen);

	pRoot->nType	= LC_TYPE_FOLDER;
	pRoot->iname	= iLen;
	pRoot->name		= (char*)LcMem_Allock(iLen, 0x0);
	strcpy(pRoot->name, sRoot);

	t_LcFile_Gather(&pCur, pRoot->name);

	*pOut = pRoot;
}


void LcFile_GatherFree(TLC_FINF** pIn)
{
	TLC_FINF*	pNodeScrpt = *pIn;
	TLC_FINF*	pNext = NULL;

	while(pNodeScrpt)
	{
		pNext= pNodeScrpt->pNext;

		if(pNodeScrpt->name)
			free(pNodeScrpt->name);

		free(pNodeScrpt);
		pNodeScrpt= pNext;
	}

	*pIn = NULL;
}


INT LcFile_mkdir(const char* sFolder, UINT mod)
{
	INT hr = 0;
#if defined(_MSC_VER)
	hr = _mkdir(sFolder);
#elif !defined(_WIN32)
	hr = mkdir(sFolder, mod);
#endif

	if(0>hr)
	{
		if(EEXIST == errno)
			hr = 0;
	}

	return hr;
}

INT LcFile_rmdir(const char* sFolder)
{
#if defined(_MSC_VER)
	return _rmdir(sFolder);
#elif !defined(_WIN32)
	return rmdir(sFolder);
#endif
}




void LcFile_SimpleStrDateTime(char* pOut, char* sYear, char* sMon, char* sDay, char* sTime, DINT lTime)
{
	char C[64];
	char Y[16];
	char M[16];
	char D[16];
	char T[32];

	time_t	tlTime = (time_t)lTime;

	strcpy(C, ctime( (const time_t *)&tlTime ) );
	sscanf(C, "%*s %*s %*s %s", T);

	strftime( C, sizeof C, "%Y %m %d", gmtime((const time_t *)&tlTime) );
	sscanf(C, "%s %s %s", Y, M, D);

	if(sYear)
		strcpy(sYear, Y);

	if(sMon)
		strcpy(sMon, M);

	if(sDay)
		strcpy(sDay, D);

	if(sTime)
		strcpy(sTime, T);

	if(pOut)
		sprintf(pOut, "%s %s %s %s", Y, M, D, T);
}



INT LcFile_Finds(TLC_FINF** pOut, const char* sPath, const char* sFile)
{
	char			sSearchPath[LC_MAX_PATH2]={0};
	TLC_FINDDATA	fd;
	L_PTR			handle;
	INT				result=1;

	sprintf(sSearchPath,"%s/*.*", sPath);
	handle= LcFile_FindFirst(&fd, sSearchPath);

	if (handle == -1)
		return LC_EFAIL;

	while (result != -1)
	{
		if(fd.name[0] !='.')
		{
			if((fd.attrib & _A_SUBDIR))
			{
				char sSearchFolder[LC_MAX_PATH2];

				sprintf(sSearchFolder, "%s/%s", sPath, fd.name);
				LcFile_Finds(pOut, sSearchFolder, sFile);
			}

			else
			{
				if(0== LcStr_Stricmp(fd.name, sFile))
				{
					char sYear[32]={0};
					char sMon[32]={0};
					char sDay[32]={0};
					char sTime[32]={0};

					LcFile_SimpleStrDateTime(NULL, sYear, sMon, sDay, sTime, fd.time_write);

					char sT[LC_MAX_PATH2]={0};
					sprintf(sT, "%s %s %s %s  %10d %s %s", sYear, sMon, sDay, sTime, (INT)fd.size, sPath, fd.name);


					INT		iLen;
					iLen	= strlen(sT);
					iLen	= t_Multiple4(iLen);

					TLC_FINF*	pNodeScrpt = (TLC_FINF*)LcMem_Allock(sizeof(TLC_FINF), 0x0);
					pNodeScrpt->nType	= LC_TYPE_FILE;	// File: LC_TYPE_FILE
					pNodeScrpt->iname	= iLen;				// Length of File Name
					pNodeScrpt->name		= (char*)LcMem_Allock(iLen, 0x0);
					strcpy(pNodeScrpt->name, sT);

					pNodeScrpt->lDate	= fd.time_write;
					pNodeScrpt->lBuf		= fd.size;

					if(*pOut)
						(*pOut)->pNext	= pNodeScrpt;

					*pOut = pNodeScrpt;
				}
			}
		}

		result= LcFile_FindNext(handle, &fd);
	}

	LcFile_FindClose(handle);

	return LC_OK;
}



INT LcFile_FindSameExt(TLC_FINF** pOut, const char* sPath, const char* sExt)
{
	char			sSearchPath[LC_MAX_PATH2]={0};
	TLC_FINDDATA	fd;
	L_PTR			handle;
	INT				result=1;


	char		tPath[LC_MAX_PATH2]={0};
	if(sPath)
	{
		strcpy(tPath, sPath);
	}

	INT iLen = strlen(tPath);

	if(iLen>0 && ('/' == tPath[iLen-1] || '\\' == tPath[iLen-1]))
	{
		tPath[iLen-1] =0;
	}


	sprintf(sSearchPath,"%s/*.*", tPath);
	handle = LcFile_FindFirst(&fd, sSearchPath);

	if (handle == -1)
		return LC_EFAIL;

	while (result != -1)
	{
		if(fd.name[0] !='.')
		{
			if((fd.attrib & _A_SUBDIR))
			{
				char sSearchFolder[1024]={0};

				sprintf(sSearchFolder, "%s/%s", tPath, fd.name);
				LcFile_FindSameExt(pOut, sSearchFolder, sExt);
			}

			else
			{
				char* p = strrchr(fd.name, '.');

				if(0== LcStr_Stricmp(p, sExt))
				{
					char sT[LC_MAX_PATH2]={0};
					sprintf(sT, "%s/%s", tPath, fd.name);

					iLen	= strlen(sT);
					iLen	= t_Multiple4(iLen);

					TLC_FINF*	pNodeScrpt = (TLC_FINF*)LcMem_Allock(sizeof(TLC_FINF), 0x0);
					pNodeScrpt->nType	= LC_TYPE_FILE;	// File: LC_TYPE_FILE
					pNodeScrpt->iname	= iLen;				// Length of File Name
					pNodeScrpt->name		= (char*)LcMem_Allock(iLen, 0x0);
					strcpy(pNodeScrpt->name, sT);

					pNodeScrpt->lDate	= fd.time_write;
					pNodeScrpt->lBuf		= fd.size;

					if(*pOut)
						(*pOut)->pNext	= pNodeScrpt;

					*pOut = pNodeScrpt;
				}
			}
		}

		result= LcFile_FindNext(handle, &fd);

	}

	LcFile_FindClose(handle);

	return LC_OK;

}



INT   LcFile_XcopyFileToFile(char* dstfile, char* srcfile)
{
	char buf [1024]={0};
	INT  Read = 0;
	INT  ftrs = 0;

	FILE* fs = fopen(srcfile, "rb");
	FILE* fd = fopen(dstfile, "wb");

	if(NULL == fs)
	{
		LOGE("XCopy Source File Open Error: %s\n", srcfile);
	}

	if(NULL == fd)
	{
		LOGE("XCopy Dest File Open Error: %s\n", dstfile);
	}


	if(NULL == fs || NULL == fd)
	{
		if(fs)
			fclose(fs);

		if(fd)
			fclose(fd);

		return LC_EFAIL;
	}


	while( !feof(fs) )
	{
		Read = fread(buf, 1, 1024, fs);
		fwrite(buf, 1, Read, fd);

		ftrs += Read;
	}

	fclose(fs);
	fclose(fd);

	return ftrs;
}


INT LcFile_XcopyBufferToFile(char* dstfile, unsigned char* buf, INT size)
{
	FILE* fd = fopen(dstfile, "w");
	INT  ftrs = 0;

	if(NULL == fd)
	{
		LOGE("XCopy Dest File Open Error: %s\n", dstfile);
		return LC_EFAIL;
	}

	ftrs = fwrite(buf, 1, size, fd);
	fclose(fd);

	return ftrs;
}




TLC_RAND::TLC_RAND()	{	::srand((unsigned)time(NULL));	}	// Sets the random starting point with current time.
TLC_RAND::TLC_RAND(UINT v){	::srand(v);						}	// Sets the random starting point with the argument value.

DOUBLE TLC_RAND::NextDouble(){ return (DOUBLE)rand()/RAND_MAX;}	// return random double number in [0, 1.0 ]


INT TLC_RAND::Next()		{	return ::rand();			}	// return random int number in [0, 32767( RAND_MAX ) ]


INT TLC_RAND::Next(INT _max)						// return random int number in [0, _max]
{
	INT w = _max +1;
	return (0>=_max)? 0 : rand() % w;
}

INT TLC_RAND::Next(INT _min, INT _max)				// return int number in [_min,_max]
{
	INT n = lc_min(_min, _max);
	INT x = lc_max(_min, _max);
	INT w = x - n +1;

	if( _min == _max)
		return 0;

	INT v = rand() % w + n;

	return v;
}

