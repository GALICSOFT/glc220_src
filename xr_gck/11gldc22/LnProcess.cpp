// Implementation of Processes...
//
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string.h>

#include "LnUtil.h"


typedef unsigned (__stdcall* _PBEGIN_THREAD_EX)(void*);							// Thread용 함수포인터
typedef _PBEGIN_THREAD_EX LPBEGIN_THREAD_EX;									//



HANDLE LcProc_ThreadCreate(LPTHREAD_START_ROUTINE pFunc,PVOID pParam, ULONG dFlag, DWORD* dId)
{
	return (HANDLE)_beginthreadex(NULL, 0, (LPBEGIN_THREAD_EX)pFunc, pParam, dFlag, (unsigned*)dId);
}

void LcProc_ThreadEnd()
{
	_endthreadex(0);
}


void LcProc_ThreadClose(HANDLE* hThread)
{
	INT		dExit= 0;
	INT		hr	 = 0;
	
	if(0==hThread || 0 == *hThread)
		return;

	GetExitCodeThread(*hThread, (DWORD*)&dExit);

	if(dExit<0)
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0,
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);

		printf("Thread Close: %s\n", (LPCTSTR)lpMsgBuf);
		LocalFree( lpMsgBuf );
	}
	
	if(dExit>0)
	{
		SuspendThread(*hThread);
		Sleep(10);

		if(0==TerminateThread(*hThread, dExit))
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				0,
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);

			printf("Thread Close: %s\n", (LPCTSTR)lpMsgBuf);
			LocalFree( lpMsgBuf );
		}
	}

	CloseHandle(*hThread);
	Sleep(10);

	*hThread = NULL;
}


void LcProc_ThreadRun(HANDLE* hThread, BOOL bRun)
{
	if(bRun)
		ResumeThread(hThread);
	else
		SuspendThread(hThread);
}


DWORD LcProc_ThreadResume(HANDLE* hThread)
{
	if(0==hThread || 0 == *hThread)
		return 0;

	return ::ResumeThread(*hThread);
}



DWORD LcProc_ThreadSuspend(HANDLE* hThread)
{
	if(0==hThread || 0 == *hThread)
		return 0;

	return ::SuspendThread(*hThread);
}


DWORD LcProc_ThreadWait(HANDLE* hThread, DWORD dMilliseconds)
{
	if(0==hThread || 0 == *hThread)
		return 0;

	return ::WaitForSingleObject( *hThread, dMilliseconds);
}







HANDLE LcProc_MutexCreate(char* pName, BOOL bSignal)
{
	return CreateMutex(NULL, bSignal, pName);
}


void LcProc_MutexDestroy(HANDLE* hMutex)
{
	if(NULL == hMutex || NULL == *hMutex)
		return;

	CloseHandle(*hMutex);

	*hMutex = NULL;
}


HANDLE LcProc_MutexOpen(char *pName, DWORD dwDesiredAccess, BOOL bInheritHandle)
{
	return OpenMutex(dwDesiredAccess, bInheritHandle, pName);
}


DWORD LcProc_MutexLock(HANDLE hMutex, DWORD dTimeOut)
{
	return WaitForSingleObject(hMutex, dTimeOut);
}

BOOL LcProc_MutexUnlock(HANDLE hMutex)
{
	return ReleaseMutex(hMutex);
}



static CRITICAL_SECTION* g_LcCsReference_f52501eb_09371e73c11b = NULL;			// Critical Section Reference

HANDLE LcProc_CsCreate()
{
	if( !g_LcCsReference_f52501eb_09371e73c11b)
	{
		g_LcCsReference_f52501eb_09371e73c11b = new CRITICAL_SECTION;
		memset(g_LcCsReference_f52501eb_09371e73c11b, 0, sizeof(CRITICAL_SECTION));

		InitializeCriticalSection(g_LcCsReference_f52501eb_09371e73c11b);
	}

	return g_LcCsReference_f52501eb_09371e73c11b;
}

void LcProc_CsDestroy()
{
	if(g_LcCsReference_f52501eb_09371e73c11b)
	{
		DeleteCriticalSection(g_LcCsReference_f52501eb_09371e73c11b);
		delete g_LcCsReference_f52501eb_09371e73c11b;
		g_LcCsReference_f52501eb_09371e73c11b = NULL;
	}
}

void LcProc_CsLock()															// Enter Critical Section
{
	EnterCriticalSection(g_LcCsReference_f52501eb_09371e73c11b);
}

void LcProc_CsUnlock()															// Enter Critical Section
{
	LeaveCriticalSection(g_LcCsReference_f52501eb_09371e73c11b);
}







