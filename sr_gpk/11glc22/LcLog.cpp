// Implementation of the GLC Log.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#if defined(_MSC_VER)
  #include <io.h>
  #include <sys/types.h>
  #include <sys/timeb.h>

#endif

#include <LcType.h>

#include "LcBase.h"
#include "LcAux.h"


#if defined(__APPLE__) || defined(__IPHONE__)

static INT   g_LogEnable = LC_LOG_DISABLE;
static INT   g_LogWay    = LC_LOG_NONE;

#elif defined(__ANDROID__) || defined(__BADA__)
static INT   g_LogEnable = LC_LOG_ENABLE;
static INT   g_LogWay    = LC_LOG_SERIAL;
#else
static INT   g_LogEnable = LC_LOG_ENABLE;
static INT   g_LogWay    = LC_LOG_FILE;
#endif

static FILE* g_LogFile   = NULL;
static INT	 g_LogErrCnt = 0;


// Function Name :: OpenLogFile
void _LcLog_OpenFile()
{
	char sFile[LC_MAX_PATH2]={0};
	char CC[64]={0};
	char YY[16]={0};
	char MM[16]={0};
	char DD[16]={0};
	char TT[32]={0};

	char hh[4]={0};
	//char mm[4]={0};
	//char ss[4]={0};

	time_t	ltime;

#if defined(__APPLE__) || defined(__IPHONE__)
	return;
#endif

	if(LC_LOG_FILE > g_LogWay ||  g_LogFile)
		return;

	if(LC_FAILED(LcFile_mkdir("_glc_log")))
	{
		LcLog_SetType(LC_LOG_SERIAL);
		return;
	}

	time( &ltime );
	strcpy(CC, ctime( (const time_t *)&ltime ) );
	sscanf(CC, "%*s %*s %*s %s", TT);

	strncpy(hh, TT, 2);
	//strncpy(mm, TT+3, 2);
	//strncpy(ss, TT+6, 2);

	strftime(CC, sizeof(CC), "%Y %m %d", gmtime((const time_t *)&ltime) );
	sscanf(CC, "%s %s %s", YY, MM, DD);

	//sprintf(sFile, "glc_log%s%s_%s%s%s.txt", MM, DD, hh, mm, ss);
	sprintf(sFile, "_glc_log/glc_log%s%s_%s.txt", MM, DD, hh);


	// create log file
	g_LogFile = fopen(sFile, "wb");
}


// close log file.
void _LcLog_CloseFile()
{
	if(NULL == g_LogFile)
		return;

	::fclose(g_LogFile);
	g_LogFile = NULL;
}


INT LcLog_ErrCount()
{
	return g_LogErrCnt;
}

// print string through serial port.
void LcLog_Print(char* slog)
{
	if(LC_LOG_SERIAL & g_LogWay)
		printf("%s", slog);

	if(LC_LOG_FILE & g_LogWay)
	{
		if(!g_LogFile)
			_LcLog_OpenFile();

		if(g_LogFile)
		{
			fwrite(slog, 1, strlen(slog), g_LogFile);
		}

		//else
		//{
		//	printf("LcLog_ :: can't write log to files.\n");
		//}
	}
}


// logout
void LcLog_Out(INT LogType, const char *fmt, ...)
{
	char	slog[1024]={0};
	va_list	arg;

#if defined(__APPLE__) || defined(__IPHONE__)
	return;
#endif

	if(LC_LOG_ERROR ==  LogType)
		++g_LogErrCnt;

	if(LC_LOG_NONE == g_LogWay || LC_LOG_DISABLE == g_LogEnable)
		return;

	// make text
	va_start(arg, fmt);
	vsprintf(slog, fmt, arg);
	va_end(arg);

	LcLog_Print(slog);
}



void LcLog_OutI(const char* fmt, ...)
{
	char	slog[1024]={0};
	va_list	arg;

#if defined(__APPLE__) || defined(__IPHONE__)
	return;
#endif

	if(LC_LOG_NONE == g_LogWay || LC_LOG_DISABLE == g_LogEnable)
		return;

	// make text
	va_start(arg, fmt);
	vsprintf(slog, fmt, arg);
	va_end(arg);

	LcLog_Print(slog);
}

void LcLog_OutW(const char* fmt, ...)
{
	char	slog[1024]={0};
	va_list	arg;

#if defined(__APPLE__) || defined(__IPHONE__)
	return;
#endif

	if(LC_LOG_NONE == g_LogWay || LC_LOG_DISABLE == g_LogEnable)
		return;

	// make text
	va_start(arg, fmt);
	vsprintf(slog, fmt, arg);
	va_end(arg);

	LcLog_Print(slog);
}

void LcLog_OutE(const char* fmt, ...)
{
	char	slog[1024]={0};
	va_list	arg;

#if defined(__APPLE__) || defined(__IPHONE__)
	return;
#endif

	++g_LogErrCnt;

	if(LC_LOG_NONE == g_LogWay || LC_LOG_DISABLE == g_LogEnable)
		return;

	// make text
	va_start(arg, fmt);
	vsprintf(slog, fmt, arg);
	va_end(arg);

	LcLog_Print(slog);
}


// change to way of log.
void LcLog_SetType(INT nType)
{
#if defined(__APPLE__) || defined(__IPHONE__)
	return;
#endif

#if defined(__BADA__)
	if(LC_LOG_SERIAL <nType)
		return;
#endif

	if( LC_LOG_NONE > nType || nType > LC_LOG_MIX)
		return;

	g_LogWay = nType;

	if(LC_LOG_FILE > nType)
	{
		_LcLog_CloseFile();
		return;
	}

	_LcLog_OpenFile();
}


void LcLog_SetSate(INT v)
{
#if defined(__APPLE__) || defined(__IPHONE__)
	return;
#endif

	g_LogEnable = v;
}


INT  LcLog_GetState()
{
	return g_LogEnable;
}


