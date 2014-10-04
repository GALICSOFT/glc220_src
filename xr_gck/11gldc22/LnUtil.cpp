// Implementation of Lc Utility functions.
//
////////////////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT			0x0400
#define _WIN32_WINDOWS			0x0400

#pragma warning(disable : 4786)
#include <vector>
using namespace std;


#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <time.h>


#include "LnUtil.h"



MEMORYSTATUS	g_MmSt	;												// for Memory state



void LcUtil_ControlInit()
{
	LcUtil_MemoryCheckSet();
	LcUtil_TimeGetTime();
	LcUtil_RandSet();	
	LcUtil_SetCurrentDirectory();
}




void LcUtil_ErrMsgBox(char *format,...)
{
	va_list ap;
	char s[2048];
	
	if (format == NULL)
		return;
	
	va_start(ap, format);
	vsprintf((char *)s, format, ap);
	va_end(ap);
	
	if (s == NULL)
		return;
	
	MessageBox(0, s, "Err", MB_OK | MB_ICONERROR);
}



void LcUtil_GetLastError(HWND hWnd)
{
	LPVOID pBuf;
	DWORD	hr = GetLastError();

	FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM
				,	NULL
				,	hr
				,	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
				,	(LPTSTR) &pBuf
				,	0
				,	NULL);

	MessageBox(hWnd, (LPTSTR)pBuf, "GetLastError", MB_OK|MB_ICONERROR);

	LocalFree(pBuf);
}



void LcUtil_SetWindowTitle(HWND hWnd, const char *format, ...)
{
	va_list ap;
	char s[2048];
	
	if (format == NULL) return;
	
	va_start(ap, format);
	vsprintf((char *)s, format, ap);
	va_end(ap);
	
	if (s == NULL)	return;
	
	SetWindowText(hWnd, s);
}






DOUBLE LcUtil_TimeGetTime()														// 하드웨어에서 지원되는 PerformanceCounter를 이용한 FPS구하기
{
//	typedef union _LARGE_INTEGER
//	{
//		struct
//		{
//			DWORD LowPart;
//			LONG HighPart;
//		};
//
//		struct
//		{
//			DWORD LowPart;
//			LONG HighPart;
//		} u;
//
//	} LARGE_INTEGER;


	static LARGE_INTEGER	lFrequency={0};
	static BOOL				bUseQueryPerformance=FALSE;
	static	BOOL			bUseTimeGetTime=FALSE;

	static LARGE_INTEGER	lBgn={0};
	
	LARGE_INTEGER			lEnd={0};
	DOUBLE					fElapsedTime = 0.;

	// QueryPerformanceFrequency를 쓸 수 없다면 timeGetTime을 사용한다.

	if(FALSE == bUseQueryPerformance && FALSE ==bUseTimeGetTime)
	{
		bUseQueryPerformance = QueryPerformanceFrequency( &lFrequency);

		if(FALSE == bUseQueryPerformance)
		{
			bUseTimeGetTime = TRUE;
			lBgn.LowPart	= timeGetTime();
		}

		else
			QueryPerformanceCounter(&lBgn);
	}

	if(bUseQueryPerformance)
	{
		QueryPerformanceCounter( &lEnd);
		fElapsedTime = DOUBLE( (lEnd.QuadPart-lBgn.QuadPart) * 1000.)/ lFrequency.QuadPart;
	}
	else
	{
		lEnd.LowPart = timeGetTime();
		fElapsedTime = lEnd.LowPart-lBgn.LowPart;
	}


	return fElapsedTime;
}







char*	LcUtil_GetSmallTime()
{
	static char s[64];
	SYSTEMTIME st;

	GetLocalTime(&st);
	sprintf(s,	"%04d/%02d/%02d	%02d:%02d:%02d.%03d"
			,	(INT)st.wYear, (INT)st.wMonth, (INT)st.wDay
			,	(INT)st.wHour, (INT)st.wMinute, (INT)st.wSecond, (INT)st.wMilliseconds);

	return s;
}



char* LcUtil_SimpleStrDateTime(char* sYear, char* sMon, char* sDay, char* sTime, time_t lTime)
{
	static char sTimeA[256];

	char C[64];
	char Y[16];
	char M[16];
	char D[16];
	char T[32];

	strcpy(C, ctime( &lTime ) );
	sscanf(C, "%*s %*s %*s %s", T);

	strftime( C, sizeof C, "%Y %m %d", gmtime(&lTime) );
	sscanf(C, "%s %s %s", Y, M, D);

	
	if(sYear)
		strcpy(sYear, Y);
	
	if(sMon)
		strcpy(sMon, M);

	if(sDay)
		strcpy(sDay, D);

	if(sTime)
		strcpy(sTime, T);


	memset(sTimeA, 0, sizeof sTimeA);
	sprintf(sTimeA, "%s %s %s %s", Y, M, D, T);

	return sTimeA;
}




void LcUtil_LogFile(char* sFile, char *format,...)
{
	va_list		vl;
	char		sDebug[2048];
	
	va_start(vl, format);
	wvsprintf(sDebug, format, vl);
	va_end(vl);
	
	FILE * fp;
	
	fp = fopen(sFile, "a+");
	fprintf(fp, "%s\t%s\t%05d\n", LcUtil_GetSmallTime(), __FILE__ , __LINE__, sDebug);
	fclose(fp);
}



void LcUtil_TextOut(HDC hDC, float x, float y, DWORD color, const char *format, ...)
{
	va_list ap;
	char s[2048];
	
	if (format == NULL) return;
	
	va_start(ap, format);
	vsprintf((char *)s, format, ap);
	va_end(ap);
	
	if (s == NULL)	return;

	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, color);
	TextOut(hDC, INT(x), INT(y), s, strlen(s));
}




void LcUtil_OutputDebug(const char * format, ...)
{
	va_list ap;
	char s[2048];
	
	if (format == NULL) return;
	
	va_start(ap, format);
	vsprintf((char *)s, format, ap);
	va_end(ap);
	
	if (s == NULL)	return;
	
	::OutputDebugString(s);
}



char*	LcUtil_GetFolder(char*	sPath, HWND hWnd, char* sTitle)
{
	BROWSEINFO		bi;
	static char		sPathName[512];
	static char		sPathFull[512];
	LPITEMIDLIST	pidl = NULL;
	LPITEMIDLIST	pidlFolder= NULL;
	LPMALLOC		pMalloc = NULL;

	memset(sPathName, 0, sizeof(sPathName));
	memset(sPathFull, 0, sizeof(sPathFull));
	memset(&bi, 0, sizeof(BROWSEINFO));

	bi.hwndOwner = hWnd;
	bi.lpszTitle = sTitle;
	bi.ulFlags = BIF_STATUSTEXT |BIF_EDITBOX   ;
	bi.ulFlags = BIF_EDITBOX   ;

	GetCurrentDirectory(512, sPathFull);

	LPSHELLFOLDER pShellFolder = NULL;
	OLECHAR wszPath[MAX_PATH] = {0};
	ULONG nCharsParsed = 0;
	
	// Get an IShellFolder interface pointer
	::SHGetDesktopFolder(&pShellFolder);
	
	// Convert the path name to Unicode
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, sPathFull, -1, wszPath, MAX_PATH);
	
	// Call ParseDisplayName() to do the job
	pShellFolder->ParseDisplayName(NULL, NULL, wszPath, &nCharsParsed, &pidl, NULL);

	bi.pidlRoot = pidl;


	pidlFolder = SHBrowseForFolder(&bi);

	if(pidlFolder == NULL)
	{
		sPath[0] =0;
		sPathFull[0];
		return sPathFull;														//취소한 경우
	}

	SHGetPathFromIDList(pidlFolder, sPathName);

	strcpy(sPath, sPathName);
	strcpy(sPathFull, sPathName);

	SHGetMalloc(&pMalloc);
	pMalloc->Free(pidl);
	pMalloc->Free(pidlFolder);
	pMalloc->Release();

	return sPathFull;
}



DWORD	LcUtil_GetFileVersion()
{
	//			
	return 0x00000101;
}


void SetDlgItemFlt(HWND hWnd, UINT id, float z, INT decimal)
{
	char s[128]={0};
	char f[64]={0};
	
	sprintf(f,"%%.%df", decimal);
	sprintf(s, f, z);
	SetDlgItemText(hWnd,id,s);
}

FLOAT GetDlgItemFlt(HWND hWnd, UINT id)
{
	char s[256]={0};

	GetDlgItemText(hWnd, id, s, sizeof(s));
	return (FLOAT)atof(s);
}

void SetDlgItemHex(HWND hWnd, UINT id, INT val)
{
	char	buf[64] = "0x";
	char	s[32]={0};
	char*	pDst;

	sprintf(s, "0000000000%X", val);
	pDst = s + strlen(s) - 8;

	strcat(buf, pDst);
	SetDlgItemText(hWnd,id,buf);
}



DWORD LcUtil_VectorToRGB(FLOAT* NormalVector)
{
	DWORD dwR = (DWORD)(100.f * NormalVector[0] + 155.f);
	DWORD dwG = (DWORD)(100.f * NormalVector[1] + 155.f);
	DWORD dwB = (DWORD)(100.f * NormalVector[2] + 155.f);
	
	return (DWORD)(0xff000000 + (dwR << 16) + (dwG << 8) + dwB);
}






void LcUtil_SetCurrentDirectory()
{
	char	sExe1[512]={0};
	char	sExe2[512]={0};

	strcpy(sExe1, GetCommandLine());

	INT	iLen = strlen(sExe1);

	int k=0;

	for(INT i=0; i<iLen; ++i)
	{
		if('"' == sExe1[i])
		{
			++k;
			continue;
		}

		if(2==k)
			break;

		sExe2[i-k] = sExe1[i];
		if('\\' == sExe2[i-k])
			sExe2[i-k] ='/';
	}

	char*pdest;
	
	pdest = strrchr(sExe2, '/');

	int result;
	result = pdest - sExe2;

	sExe2[result] =0;

	BOOL hr = ::SetCurrentDirectory(sExe2);
}


void LcUtil_MemoryCheckSet()
{
	::GlobalMemoryStatus(&g_MmSt);
}


DWORD LcUtil_MemoryCheckGet()
{
	MEMORYSTATUS	MmSt;
	::GlobalMemoryStatus(&MmSt);
	return ((g_MmSt.dwAvailPhys - MmSt.dwAvailPhys)/1024);								// KByte
}


void LcUtil_RandSet(DWORD dSeed)
{
	if(0 == dSeed)
	{
//		dSeed = ::time(NULL);
		dSeed = timeGetTime();
	}
	
	::srand( dSeed );
}


FLOAT	LcUtil_GetLinearWg(FLOAT f1,FLOAT f2,FLOAT f)
{
		return ( (f-f1) / (f2-f1) );
}



void LcUtil_GetDefaultWebBrowser(char* sHomePage)
{
	// HKEY_CLASSES_ROOT\http\shell\open\command
	DWORD	dwType, cbData;
	HKEY	hKey; 
	long	lRet;
	char*	sStr;
	
	
	if ((lRet = RegOpenKeyEx(HKEY_CLASSES_ROOT, "http\\shell\\open\\command", 0, KEY_READ | KEY_QUERY_VALUE , &hKey)) == ERROR_SUCCESS)
	{
		cbData = 512;
		sStr = (char*)malloc(cbData * sizeof(char));
		memset(sStr, 0, sizeof(char)*cbData);

		if ((lRet = RegQueryValueEx(hKey, "",  0, &dwType, (BYTE*)sStr, &cbData)) == ERROR_SUCCESS)
		{
			
			int iLen = strlen(sStr);
			
			for(int i=0; i<iLen; ++i)
			{
				if('"' == *(sStr +i))
				{
					for(int j=i; j<iLen; ++j)
						*(sStr +j) = *(sStr +j+1);
					
					iLen = strlen(sStr);
				}
			}
			
			char *pdest = strstr(sStr, ".exe");
			
			int r;
			
			r= pdest - sStr+4;
			
			sStr[r+0] = ' ';
			sStr[r+1] = 0;

			strcat(sStr, sHomePage);
			
			WinExec((LPCSTR)sStr, SW_SHOWNORMAL);            
		}

		free(sStr);
		RegCloseKey(hKey);
	}
}


void LcUtil_SHGetWindowsFolder(char *pBuf)
{
	SHGetSpecialFolderPath( NULL, pBuf, CSIDL_WINDOWS, 0);
}

void LcUtil_SHGetProgramFilesFolder(char *pBuf)
{
	SHGetSpecialFolderPath( NULL, pBuf, CSIDL_PROGRAM_FILES, 0);
}


void LcUtil_SHGetSpecialFolder(char* pBuf/*out*/, INT nFolder)
{
	SHGetSpecialFolderPath( NULL, pBuf, nFolder, 0);
}


INT LcUtil_GetWindowsVersion(OSVERSIONINFO* pVer)
{
	pVer->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if( 0== GetVersionEx(pVer))
		return -1;


//	printf ("Major Version:      %d\n", pVer->dwMajorVersion);
//	printf ("Minor Version:      %d\n", pVer->dwMinorVersion);
//	printf ("Build Number:       %d\n", pVer->dwBuildNumber);
//	printf ("Platform ID:        %d\n", pVer->dwPlatformId);
//	printf ("Platorm is NT?(0/1) %d\n", (pVer->dwPlatformId == VER_PLATFORM_WIN32_NT));
//	printf ("Service Pack:       %s\n", pVer->szCSDVersion);

	return 1;
}


void LcUtil_GetWindowsInfo(SYSTEM_INFO* pSys)
{
	GetSystemInfo(pSys);

//	printf ("OEM Id:             %d\n", pSys->dwOemId);
//	printf ("Processor Arch:     %d\n", pSys->wProcessorArchitecture);
//	printf ("Page Size:          %x\n", pSys->dwPageSize);
//	printf ("Min appl addr:      %p\n", pSys->lpMinimumApplicationAddress);
//	printf ("Max appl addr:      %p\n", pSys->lpMaximumApplicationAddress);
//	printf ("ActiveProcMask:     %x\n", pSys->dwActiveProcessorMask);
//	printf ("Number processors:  %d\n", pSys->dwNumberOfProcessors);
//	printf ("Processor type:     %d\n", pSys->dwProcessorType);
//	printf ("Alloc grnrty:       %x\n", pSys->dwAllocationGranularity);
//	printf ("Processor level:    %d\n", pSys->wProcessorLevel);
//	printf ("Processor rev:      %d\n", pSys->wProcessorRevision);
//	printf ("OEM Id:             %d\n", pSys->dwOemId);
}


void LcUtil_GetClientRect(HWND hWnd, RECT* rc)
{
	RECT rc1;
	RECT rc2;
	INT FrmW;
	INT FrmH;

	GetWindowRect(hWnd, &rc1);
	GetClientRect(hWnd, &rc2);

	FrmW = (rc1.right - rc1.left - rc2.right)/2;
	FrmH = rc1.bottom - rc1.top - rc2.bottom - FrmW;

	// Window Client Rect 시작 위치
	rc->left = rc1.left + FrmW;
	rc->top  = rc1.top  + FrmH;

	// Window Client Rect Width and Height
	rc->right	= rc2.right+ rc2.left;
	rc->bottom	= rc2.bottom+ rc2.top;
}










void	LcUtil_Rectangle(HDC hDC, INT x0, INT y0, INT x1, INT y1, DWORD color)
{
	HPEN	hPen = CreatePen(PS_SOLID, 1, color);
	SelectObject(hDC, hPen);
	
	MoveToEx(hDC, x0, y0, 0);	LineTo(hDC, x1, y0);
	MoveToEx(hDC, x1, y0, 0);	LineTo(hDC, x1, y1);
	MoveToEx(hDC, x1, y1, 0);	LineTo(hDC, x0, y1);
	MoveToEx(hDC, x0, y1, 0);	LineTo(hDC, x0, y0);

	DeleteObject(hPen);
}




// ARGB <--> BGR
BYTE* LcUtil_GetColorFromARGB(DWORD& _argb)
{
	static BYTE argb[4];
	BYTE* pClr = (BYTE*)&_argb;

	argb[0] = pClr[3];
	argb[1] = pClr[2];
	argb[2] = pClr[1];
	argb[3] = pClr[0];

	return argb;
}

// ARGB <--> BGR
BYTE* LcUtil_GetColorFromBGR(DWORD& bgr)
{
	static BYTE argb[4];

	BYTE* pClr = (BYTE*)&bgr;

	argb[0] = 0x0;
	argb[1] = pClr[0];
	argb[2] = pClr[1];
	argb[3] = pClr[2];
	return argb;
}


INT	LcUtil_MonitorMaxHz(INT nScnW, INT nScnH, INT nBitsPerPel)
{
	int nMax = -1;

	DEVMODE dmModCur={0};
	DEVMODE dmModNew={0};

//	dmModCur.dmSize = sizeof(DEVMODE);
	
	if(0 ==EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmModCur))
		return -1;


	memcpy(&dmModNew, &dmModCur, sizeof dmModNew);
	dmModNew.dmPelsWidth	= nScnW;
	dmModNew.dmPelsHeight	= nScnH;
	dmModNew.dmBitsPerPel	= nBitsPerPel;

	for(int i=60; i<240; ++i)
	{
		DWORD hr = -1;
		
		dmModNew.dmDisplayFrequency = i;
		hr = ChangeDisplaySettings(&dmModNew, CDS_TEST);

		if(DISP_CHANGE_SUCCESSFUL == hr)
			nMax = i;
	}


	DEVMODE dmModOld;
	memcpy(&dmModOld, &dmModCur, sizeof dmModOld);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmModOld);

	
	if( 0!=memcmp(&dmModOld, &dmModCur, sizeof(DEVMODE)) )
		ChangeDisplaySettings(&dmModCur, 0);;


	return nMax;
}





#if(WINVER >= 0x0500)

void LcUtil_MonitorInfo(HWND hWnd, MONITORINFOEX* pMiOut)
{
	if(!hWnd)
		return;

	HMONITOR		hMonitor = MonitorFromWindow(hWnd,MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX	MntInf={0};

	MntInf.cbSize=sizeof(MONITORINFOEX);
	
	GetMonitorInfo(hMonitor,&MntInf);

	memcpy(pMiOut, &MntInf, sizeof(MONITORINFOEX));
}




INT LcUtil_MonitorInfo(INT* nBitsPixel, RECT* rcVirtualScreen, INT* bAllSameFormat, INT* nMonitorNum, MONITORINFOEX* pMiOut)
{
	INT	hr	= -1;
	//1. 화면 픽셀 비트 가져오기

	if(nBitsPixel)
	{
		HDC hDC = GetDC(NULL);
		*nBitsPixel = GetDeviceCaps (hDC, BITSPIXEL) ;
		hr = ReleaseDC(NULL, hDC);

		if(1 != hr)
			return -1;
	}


	//2. 화면 영역과 칼라 포멧
	// SM_CMONITORS: Windows NT 5.0 and later ,Windows 98 and later.
	// Number of display monitors on the desktop. 
	// SM_SAMEDISPLAYFORMAT: Windows NT 5.0 and later, Windows 98 and later.
	// TRUE if all the display monitors have the same color format, FALSE otherwise 

	if(bAllSameFormat)
		*bAllSameFormat = GetSystemMetrics(SM_SAMEDISPLAYFORMAT) ? 1:0;

	if(rcVirtualScreen)
	{
		rcVirtualScreen->left		= GetSystemMetrics(SM_XVIRTUALSCREEN);
		rcVirtualScreen->top		= GetSystemMetrics(SM_YVIRTUALSCREEN);

		rcVirtualScreen->right		= GetSystemMetrics(SM_CXVIRTUALSCREEN);
		rcVirtualScreen->bottom		= GetSystemMetrics(SM_CYVIRTUALSCREEN);

		rcVirtualScreen->right		+= rcVirtualScreen->left ;
		rcVirtualScreen->bottom		+= rcVirtualScreen->top  ;
	}

	//3. 모니터 갯수

	if(nMonitorNum)
	{
		hr = GetSystemMetrics(SM_CMONITORS);
		*nMonitorNum = hr ? hr: 1;
	}


	//4. 모니터의 세세한 정보
	BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,HDC hdcMonitor,LPRECT prcMonitor,LPARAM lParam);
	
	if(pMiOut)
	{
		if(0 == EnumDisplayMonitors(NULL,NULL,MonitorEnumProc, (LPARAM)pMiOut))
			return -1;
	}

	return 1;
}


BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,HDC hdcMonitor,LPRECT prcMonitor,LPARAM lParam)
{
	MONITORINFOEX* pMiOut = (MONITORINFOEX*)(lParam);

	MONITORINFOEX MntInf={0};

	MntInf.cbSize=sizeof(MONITORINFOEX);

	GetMonitorInfo(hMonitor,&MntInf);

	memcpy(pMiOut, &MntInf, sizeof(MONITORINFOEX));

	return TRUE;
}

#endif //#if(WINVER >= 0x0500)





INT		LcUtil_CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit)
{
	#if (_WIN32_WINNT >= 0x0400 )

	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
		return -1;

	#else

	if (FAILED(CoInitialize(NULL)))
		return -1;

	#endif


	return 1;
}



void LcUtil_CoUninitialize()
{
	CoUninitialize();
}



INT	LcUtil_MemoryLoadFromResource(	// return value Success:1 Fail:-1
									void** pOut	// You must be free pOut
								  , INT* iSize	// pOut Size
								  , INT nResourceId
								  , const char* sType)
{
	HINSTANCE	hInst	= NULL;
	HRSRC		hRsc	= NULL;
	DWORD		dwSize	= 0;
	HGLOBAL		hMem	= NULL;
	LPVOID		pMem	= NULL;
	
	*iSize	= 0;
	*pOut	= NULL;

	hInst	= GetModuleHandle(NULL);
	hRsc	= FindResource( hInst, MAKEINTRESOURCE(nResourceId), sType);
	
	if(NULL == hRsc)
		return -1;
	
	dwSize = SizeofResource(hInst,hRsc);

	if(0==dwSize)
		return -1;
	
	hMem = LoadResource(hInst, hRsc);

	if(NULL == hMem)
		return -1;

	pMem = LockResource(hMem);

	*pOut = malloc(dwSize);
	*iSize = dwSize;

	memcpy(*pOut, pMem, dwSize);
	
	UnlockResource(hMem);
	FreeResource(hMem);

	return 1;
}



















void LcUtil_TreeExpand(lsHItem& ls, HWND& hwTree, INT iM, INT iS)
{
	INT iSize = (INT)ls.size();

	for(INT i=0;i<iSize; ++i)
	{
		if(ls[i].nM == iM && ls[i].nS == iS)
		{
			HTREEITEM	hT = ls[i].hT;
			SendMessage(hwTree, TVM_EXPAND,TVE_EXPAND, (LPARAM)hT);

			break;
		}
	}
}










////////////////////////////////////////////////////////////////////////////////
//
// Implementation of the CMcInputBox class.
//
////////////////////////////////////////////////////////////////////////////////

static CMcInputBox*	g_pInputBox;

CMcInputBox::CMcInputBox()
{
	g_pInputBox = this;

	m_hInst			= NULL;
	m_hWnd			= NULL;

	m_hFont			= NULL;
	
	m_hWndPrnt		= NULL;
	m_hWndEdit		= NULL;
	m_hWndOK		= NULL;
	m_hWndCancel	= NULL;
	m_hWndPrompt	= NULL;
	
	m_pTxt			= NULL;

	m_iBoxW			= 400;
	m_iBoxH			= 125;
}


INT CMcInputBox::Create(HWND hPrnt)
{
	m_hWndPrnt= hPrnt;
	m_hInst = (HINSTANCE)GetModuleHandle(NULL);
	
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	if (!GetClassInfoEx(m_hInst, "InputBox", &wcex))
	{
		wcex.cbSize = sizeof(WNDCLASSEX); 
		
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= m_hInst;
		wcex.hIcon			= NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= "InputBox";
		wcex.hIconSm		= NULL;
		
		if (RegisterClassEx(&wcex) == 0)
		{
			MessageBox(NULL, "Can't create CMcInputBox!", "Error", MB_OK|MB_ICONEXCLAMATION);
			return -1;
		}
	}
	
	
	return 1;
}

CMcInputBox::~CMcInputBox()
{
	if (m_pTxt)
	{
		delete[] m_pTxt;
		m_pTxt = NULL;
	}
}


LRESULT CALLBACK CMcInputBox::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pInputBox->MsgProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CMcInputBox::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) 
	{
		case WM_CREATE:
		{
			LOGFONT lfont={0};
			// font
			strcpy(lfont.lfFaceName, "Arial");
			lfont.lfHeight = 16;
			lfont.lfWeight = FW_NORMAL;//FW_BOLD;
			lfont.lfItalic = FALSE;
			lfont.lfCharSet = DEFAULT_CHARSET;
			lfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
			lfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			lfont.lfQuality = DEFAULT_QUALITY;
			lfont.lfPitchAndFamily = DEFAULT_PITCH;
			
			m_hFont = CreateFontIndirect(&lfont);
			m_hInst = GetModuleHandle(NULL);
			
			// creating Edit
			m_hWndEdit = CreateWindowEx(WS_EX_STATICEDGE, "edit","",
				WS_VISIBLE | WS_CHILD  | WS_TABSTOP | ES_AUTOHSCROLL, 
				5, m_iBoxH - 50, m_iBoxW - 16, 20, 
				hWnd, 
				NULL, 
				m_hInst, 
				NULL); 
			
			// setting font
			SendMessage(m_hWndEdit, WM_SETFONT, (WPARAM)m_hFont, 0);
			
			// button OK
			m_hWndOK = CreateWindowEx(WS_EX_STATICEDGE, "button","OK",
				WS_VISIBLE | WS_CHILD | WS_TABSTOP, 
				m_iBoxW - 100, 10, 90, 25, 
				hWnd, 
				NULL, 
				m_hInst, 
				NULL); 
			
			// setting font
			SendMessage(m_hWndOK, WM_SETFONT, (WPARAM)m_hFont, 0);
			
			// button Cancel
			m_hWndCancel = CreateWindowEx(WS_EX_STATICEDGE, "button","Cancel",
				WS_VISIBLE | WS_CHILD | WS_TABSTOP, 
				m_iBoxW - 100, 40, 90, 25, 
				hWnd, 
				NULL, 
				m_hInst, 
				NULL); 
			
			// setting font
			SendMessage(m_hWndCancel, WM_SETFONT, (WPARAM)m_hFont, 0);
			
			// static Propmpt
			m_hWndPrompt = CreateWindowEx(WS_EX_STATICEDGE, "static","",
				WS_VISIBLE | WS_CHILD, 
				5, 10, m_iBoxW - 110, m_iBoxH - 70, 
				hWnd, 
				NULL, 
				m_hInst, 
				NULL); 
			
			// setting font
			SendMessage(m_hWndPrompt, WM_SETFONT, (WPARAM)m_hFont, 0);
			
			SetFocus(m_hWndEdit);
			break;
		}
			
		case WM_DESTROY:
		{
			if(m_hFont)
			{
				DeleteObject(m_hFont);
				m_hFont = NULL;
			}

			if(m_hWndEdit)
			{
				DestroyWindow(m_hWndEdit);
				m_hWndEdit = NULL;
			}

			if(m_hWndOK)
			{
				DestroyWindow(m_hWndOK);
				m_hWndOK = NULL;
			}

			if(m_hWndCancel)
			{
				DestroyWindow(m_hWndCancel);
				m_hWndCancel = NULL;
			}

			if(m_hWndPrompt)
			{
				DestroyWindow(m_hWndPrompt);
				m_hWndPrompt = NULL;
			}

			if(m_hWnd)
			{
				m_hWnd= NULL;
				EnableWindow(m_hWndPrnt, TRUE);
				SetForegroundWindow(m_hWndPrnt);
				DestroyWindow(hWnd);
				PostQuitMessage(0);
			}
			break;
		}
		
		case WM_COMMAND:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if ((HWND)lParam == m_hWndOK)
					PostMessage(m_hWnd, WM_KEYDOWN, VK_RETURN, 0);
				if ((HWND)lParam == m_hWndCancel)
					PostMessage(m_hWnd, WM_KEYDOWN, VK_ESCAPE, 0);
				break;
			}
			break;
		}
		
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}


BOOL CMcInputBox::DoModal(LPCTSTR szCaption, LPCTSTR szPrompt)
{
	RECT r;
	GetWindowRect(GetDesktopWindow(), &r);
	
	m_hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, 
		"InputBox",
		szCaption,
		WS_POPUPWINDOW | WS_CAPTION | WS_TABSTOP, 
		(r.right - m_iBoxW) / 2, (r.bottom - m_iBoxH) / 2,
		m_iBoxW, m_iBoxH,
		m_hWndPrnt,
		NULL,
		m_hInst,
		NULL);
	if(m_hWnd == NULL)
		return FALSE;
	
	
	SetWindowText(m_hWndPrompt, szPrompt);
	
	SetForegroundWindow(m_hWnd);
	
	EnableWindow(m_hWndPrnt, FALSE);
	
	ShowWindow(m_hWnd, SW_SHOW); 
	UpdateWindow(m_hWnd);
	
	BOOL ret = 0;
	
	MSG msg;
	
	HWND hWndFocused;
	
	while (GetMessage(&msg, NULL, 0, 0)) 
	{       
		if (msg.message == WM_KEYDOWN) 
		{
			if (msg.wParam == VK_ESCAPE)
			{
				SendMessage(m_hWnd, WM_DESTROY, 0, 0);
				ret = 0;
			}
			if (msg.wParam == VK_RETURN)
			{
				int nCount = GetWindowTextLength(m_hWndEdit);
				nCount++;

				if (m_pTxt) 
				{
					delete[] m_pTxt;
					m_pTxt = NULL;
				}

				m_pTxt = new char[nCount];

				GetWindowText(m_hWndEdit, m_pTxt, nCount);
				SendMessage(m_hWnd, WM_DESTROY, 0, 0);
				ret = 1;
			}
			if (msg.wParam == VK_TAB)
			{
				hWndFocused = GetFocus();
				if (hWndFocused == m_hWndEdit) SetFocus(m_hWndOK);
				if (hWndFocused == m_hWndOK) SetFocus(m_hWndCancel);
				if (hWndFocused == m_hWndCancel) SetFocus(m_hWndEdit);
			}
			
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return ret;
}


LPTSTR CMcInputBox::GetText()
{
	return m_pTxt;
}





////////////////////////////////////////////////////////////////////////////////
//
// Implementation of the Tskip class.
//
////////////////////////////////////////////////////////////////////////////////


Tskip::Tskip()
 : iB(1)
 , iC(2)
 , iS(2)
{
}

void Tskip::SetStep(INT _iS)
{	iS = _iS;
}

INT Tskip::GetStep()
{
	return iS;
}

INT Tskip::FrameMove()
{
	if(iB)
		++iC;

	if(iC<iS)
		return 0;

	if(iC>iS)
		iC = iS;

	return 1;
}

INT Tskip::IsEnd()
{
	if(iC>=iS)
		return 1;

	return 0;
}

INT	Tskip::SetStart()
{
	iB = 1;
	iC =0;

	return 1;
}




////////////////////////////////////////////////////////////////////////////////
//
// Implementation of the TexcInf class.
//
////////////////////////////////////////////////////////////////////////////////

TexcInf::TexcInf()
{
	memset(sExcPath,	0, sizeof(sExcPath	));
	memset(sVersion,	0, sizeof(sVersion	));
	memset(SvrIP,		0, sizeof(SvrIP		));
	memset(SvrPt,		0, sizeof(SvrPt		));
	memset(SvrPt,		0, sizeof(FileFont	));
}


FLOAT* TexcInf::GetLod()
{
	return &m_fLod[0];
}

void TexcInf::SetLod(FLOAT* pLod)
{
}



////////////////////////////////////////////////////////////////////////////////
//
// Implementation of the TkeyHooking class.
//
////////////////////////////////////////////////////////////////////////////////

HHOOK TkeyHooking::m_hHook		= NULL;

TkeyHooking::TkeyHooking()
{
}


void TkeyHooking::Hooking()
{
	if(NULL == m_hHook)
	{
		OSVERSIONINFO os;
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&os);

		DWORD		dId   = GetCurrentThreadId();
		HINSTANCE	hInst = GetModuleHandle(NULL);

		if(os.dwPlatformId == VER_PLATFORM_WIN32_NT && os.dwMajorVersion > 4)
			m_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, TkeyHooking::LowLevelKeyboardProc, hInst, 0);
	}
}


void TkeyHooking::UnHooking()
{
	UnhookWindowsHookEx(m_hHook);
}





LRESULT CALLBACK TkeyHooking::LowLevelKeyboardProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT* pkbhs = (KBDLLHOOKSTRUCT*)lParam;
	BOOL KeyDownCtrl = 0;
	
	switch(nCode)
	{
	case HC_ACTION:
		KeyDownCtrl = ::GetAsyncKeyState(VK_CONTROL) & 0X8000;

		if(pkbhs->vkCode == VK_ESCAPE && KeyDownCtrl)							// Ctrl + Esc
			return 1;

		
		if(pkbhs->flags & LLKHF_ALTDOWN)
		{
			if(pkbhs->vkCode == VK_TAB ||										// Alt+Tab
				pkbhs->vkCode == VK_F4 ||										// Alt+F4
				pkbhs->vkCode == VK_ESCAPE										// Alt+ESC
				)
				return 1;
		}
		
		if(pkbhs->vkCode == VK_LWIN || pkbhs->vkCode == VK_RWIN)				// Window Logo Key
			return 1;
		
		break;
		
	}
	
	return ::CallNextHookEx(m_hHook, nCode, wParam, lParam);
}





HINSTANCE LcUtil_WindowInstance()												// Get Window HINSTANCE
{
	return (HINSTANCE)::GetModuleHandle(NULL);
}

HWND LcUtil_WindowHandle()														// Get Window Handle
{
	return ::GetActiveWindow();
}

HCURSOR LcUtil_WindowCursor(HWND hWnd)											// Get Window Cursor
{
	HCURSOR hCursor = NULL;
#ifdef _WIN64
	hCursor = (HCURSOR)GetClassLongPtr( hWnd, GCLP_HCURSOR );
#else
	hCursor = (HCURSOR)ULongToHandle( GetClassLong( hWnd, GCL_HCURSOR ) );
#endif

	return hCursor;
}


INT LcUtil_WindowExeName(char* sName, int nSize)								// Get Execute File Full Name
{
	INT hr;
	HINSTANCE hInst = (HINSTANCE)::GetModuleHandle(NULL);
	hr = GetModuleFileName(hInst, sName, nSize);

	return (0==hr)? -1: hr;
}


INT LcUtil_WindowClassName(HWND hWnd, char* sName, int nSize)					// Window Class Name
{
	INT hr;
	hr = GetClassName(hWnd, sName, nSize);

	return (0==hr)? -1: hr;
}


INT LcUtil_WindowObject(HGDIOBJ hObj, void* pData, int nSize)					// Get Object Info.. FONT, Pen, Brush....
{
	INT hr;
	hr = GetObject(hObj, nSize, pData);

	return (0==hr)? -1: hr;
}


INT LcUtil_WindowObjectType(HGDIOBJ hObj)										// Get Object Type
{
	INT hr;
	hr = GetObjectType(hObj);

	//	OBJ_PEN             1	Bitmap
	//	OBJ_BRUSH           2	Brush
	//	OBJ_DC              3	Font
	//	OBJ_METADC          4	Palette
	//	OBJ_PAL             5	Pen
	//	OBJ_FONT            6	Extended pen
	//	OBJ_BITMAP          7	Region
	//	OBJ_REGION          8	Device context
	//	OBJ_METAFILE        9	Memory device context
	//	OBJ_MEMDC           10	Metafile
	//	OBJ_EXTPEN          11	Metafile device context
	//	OBJ_ENHMETADC       12	Enhanced metafile
	//	OBJ_ENHMETAFILE     13	Enhanced metafile device context

	return (0==hr)? -1: hr;
}


INT LcUtil_WindowClassInfo(HWND hWnd, WNDCLASS* wc)								// Get Window Class Info
{
	INT			hr;
	char		sName[MAX_PATH];

	hr = GetClassName(hWnd, sName, sizeof sName);

	if(0==hr)
		return -1;

	hr = GetClassInfo( GetModuleHandle(NULL), sName, wc);

	return (0==hr)? -1: hr;
}



INT	LcUtil_INIReadString(	char* sFile
						,	char* sApp
						,	char* sKey
						,	char* sValue
						,	DWORD sValueSize
						,	char* sDefault)
{
	DWORD hr = GetPrivateProfileString(sApp, sKey, sDefault, sValue, sValueSize, sFile);

	if(0==hr)
		return -1;

	return hr;
}


INT LcUtil_INIReadINT(	char*	sFile
					,	char*	sApp
					,	char*	sKey
					,	INT		iDefault)
{
	INT hr = GetPrivateProfileInt(sApp, sKey, iDefault, sFile);
	return hr;
}




FLOAT	LcUtil_INIReadFloat(	char*	sFile
							,	char*	sApp
							,	char*	sKey
							,	FLOAT	fDefault)
{
	char	sTemp[64];
	float	fValue= fDefault;

	if(GetPrivateProfileString(sApp, sKey, NULL, sTemp, sizeof sTemp, sFile)>0)
		sscanf(sTemp, "%f", &fValue);

	return fValue;
}




INT LcUtil_INIReadSscanf(	char* sFile
						,	char* sApp
						,	char* sKey
						,	char *format,...)
{
	char	sBuf[512]={0};
	DWORD hr = GetPrivateProfileString(sApp, sKey, NULL, sBuf, sizeof sBuf, sFile);

	if(0==hr)
		return -1;
	
	va_list ap;
	va_start(ap, format);
	
	return vsscanf(sBuf, format, ap);
}




void LcUtil_GetFullPath(WCHAR* stPath, const char* sFile)
{
	WCHAR apppath[MAX_PATH] = {0};

	if (!apppath[0])
	{
		::GetModuleFileNameW(NULL, apppath, MAX_PATH);

		int appLen = wcslen(apppath);

		while (appLen > 1)
		{
			if (apppath[appLen-1] == L'\\')
				break;
			appLen--;
		}

		apppath[appLen] = L'\0';
	}

	WCHAR stFile[MAX_PATH]={0};


	LcStr_AnsiToUnicode(stFile, sFile, MAX_PATH);

	wcscpy(stPath, apppath);
	wcscat(stPath, stFile);
}



