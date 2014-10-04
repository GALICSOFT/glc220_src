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
#include <time.h>
#include <errno.h>

#include <LcType.h>
#include <LcMath.h>

#include "LcBase.h"
#include "LcStDev.h"
#include "LcAux.h"


#if defined(_WIN32)
  #include <windows.h>

#elif defined(_PC_LINUX_)
  #include <GL/glx.h>
  #include <GL/gl.h>
  #include <unistd.h>

  #ifndef XA_ATOM
    #define XA_ATOM ((Atom) 4)
  #endif

#elif defined(__ANDROID__)
  #include <assert.h>
  #include <jni.h>

#elif defined(__BADA__)
  #include <FBaseSys.h>

#endif


#if !defined(__ANDROID__)
  typedef int		jint		;
  typedef void*		jclass		;
  typedef void*		jmethodID	;
  typedef void*		JavaVM		;
  typedef void*		JNIEnv		;

  #if !defined(JNI_VERSION_1_2)
    #define JNI_VERSION_1_2 0x00010002
  #endif
#endif


#include <LcType.h>
#include <LcMath.h>

#include "LcBase.h"
#include "LcAux.h"


////////////////////////////////////////////////////////////////////////////////

namespace LCSYS
{
	// Global or Default Value
	#if defined(__APPLE__) || defined(__IPHONE__)
	INT	g_LConstScnW= 320;
	INT	g_LConstScnH= 480;

	#elif defined(__ANDROID__) || defined(__BADA__)
	INT	g_LConstScnW= 480;
	INT	g_LConstScnH= 800;

	#elif defined(_WIN32) || defined(_PC_LINUX_)
	INT	g_LConstScnW= 640;
	INT	g_LConstScnH= 480;
	#else
	INT	g_LConstScnW= 480;
	INT	g_LConstScnH= 320;
	#endif

	DOUBLE	g_LC_Frame		= 60;
	DOUBLE	g_LC_ElapseTime	= 0;
	DOUBLE	g_LC_StoredTime = 0;
	INT				   g_LC_Active = LC_FALSE;
	INT (LC_CALLBACK*  g_Lc_Run)(LC_HANDLE);
	LONG (LC_CALLBACK* g_Lc_MsgPrc)(LC_HWND, UINT, WPARAM, LPARAM);


	INT	g_LC_Orient = LCSYS_ORIENT_PORTRAIT;

	JavaVM*	g_pJavaVM	= NULL;
	JNIEnv*	g_pEnv		= {0};
	jclass	g_JniClzz	= {0};
	char	g_JniCls	[LC_MAX_FNAME]	={0,};


	extern TLC_INPUT_TS	g_InputMts[LC_MAX_TSB];
}



static LC_HANDLE LC_OS_CreateWindow()
{
    return (LC_HANDLE)malloc( sizeof(LC_HWND) );
}


////////////////////////////////////////////////////////////////////////////////


// Main Default Run Function
INT LC_CALLBACK LcDft_Run(LC_HANDLE){	return LC_OK;}

//Main Default Message Procedure Function
LONG LC_CALLBACK LcDft_MsgPrc(LC_HWND, UINT, WPARAM, LPARAM){ return LC_TRUE;}




// LC System Init : Global and Environment Init
INT  LcSys_Init( LC_HANDLE _apk, LC_HANDLE _cls, LC_HANDLE _env, LC_HANDLE _etc)
{
	INT hr = LC_OK;
	char* apk = (char*)_apk;

	// print Version
	UINT	dVer=0;
	UINT	dDate=0;

	LcVar_Version(&dVer, &dDate);

#if defined(_MSC_VER)
	printf("GLC Vesion: 0x%X Date: %X\n", dVer, dDate);
#else
	LOGI("GLC Vesion: 0x%X Date: %X\n", dVer, dDate);
#endif


	// setup the package directory
#if defined(__ANDROID__)

	char* clss = (char*)_cls;
	LCSYS::g_pEnv = (JNIEnv*)(_env);
	(LCSYS::g_pEnv)->GetJavaVM(&LCSYS::g_pJavaVM);
	strcpy(LCSYS::g_JniCls, clss);

	
	if(NULL == clss || 0 == (int)LCSYS::g_pEnv)
	{
		LOGE("LcSys_Init::Err::clss, env is null -------\n");
		return LC_EFAIL;
	}

	//LOGI("LcSys_Init::Class Name: %s\n", clss);

	LCSYS::g_JniClzz = (jclass)LCSYS::g_pEnv->FindClass(LCSYS::g_JniCls);

	if(0 == LCSYS::g_JniClzz)
	{
		LOGE("LcSys_Init::Err::FindClass::%s ----------\n", LCSYS::g_JniCls);
		return LC_EFAIL;
	}


	//LCSYS::g_JniClzz = (jclass)LCSYS::g_pEnv->NewGlobalRef(clazz);
	//if(0 == LCSYS::g_JniClzz)
	//{
	//	LOGE("LcSys_Init::Err::NewGlobalRef \n");
	//	return LC_EFAIL;
	//}
	////LOGW("LcSys_Init::NewGlobalRef:: 0x%X \n", LCSYS::g_JniClzz);



	hr = (int)LcSys_AndroidApkFile(apk);
	hr = (int)LcSys_AppFolder(apk);

	if(0 == hr)
		return LC_EFAIL;

	hr = LcStv_HapticOpen();
	if(0>hr)
	{
		LOGE("Haptic Open Failed\n");
		return LC_EFAIL;
	}

	hr = LcStv_SensorOpen();
	if(0>hr)
	{
		LOGE("Sensor Open Failed\n");
		return LC_EFAIL;
	}

#endif

	LCSYS::g_LC_Active = LC_TRUE;
	LCSYS::g_Lc_Run    = LcDft_Run;
	LCSYS::g_Lc_MsgPrc = LcDft_MsgPrc;


	hr = LcSys_tzset();								// Timer Init
	hr = LcSys_InitTime();							// Time Init
	hr = LcSys_RandSeedSet(0);						// Random SeedSet

#if defined(_MSC_VER)
	if(apk)
		hr = LcSys_SetCurrentDir(apk);
	else
		hr = LcSys_SetCurrentDirWithCommandLine();	// Set current directory
#endif

	LcLog_SetType(LC_LOG_MIX);					// log file open

	return hr;
}


void LcSys_Close()
{
#if defined(__ANDROID__)
	//if(LCSYS::g_JniClzz)
	//{
	//	LCSYS::g_pEnv->DeleteGlobalRef(LCSYS::g_JniClzz);
	//	LCSYS::g_JniClzz = 0;
	//}

	LcStv_HapticClose();
	LcStv_SensorClose();

#endif
}



// Main Run Function
INT LcSys_Run()
{
	if(!LCSYS::g_LC_Active)
		return LC_EFAIL;

	// Run Process
#if defined(_WIN32)
	static MSG msg ={0};

	if(msg.message==WM_QUIT)
		return LC_EFAIL;

	if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
	{
		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}
#endif

	return LCSYS::g_Lc_Run(NULL);
}



// Main Message Function
LONG LC_CALLBACK LcSys_MsgPrc(LC_HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT hr = LC_OK;

#if defined(_MSC_VER)

#if !defined(WM_MOUSEWHEEL)
  #define WM_MOUSEWHEEL		0x020A
#endif

	if(WM_MOUSEWHEEL == uMsg)
	{
		INT c= HIWORD(wParam);
		INT d= LOWORD(wParam);
		c = short( c );
		FLOAT f = (FLOAT)(c);
		LCSYS::g_InputMts[2].x += (INT)(f); 

		//LOGI("Wheel Event: %d, %d \n", c, LCSYS::g_InputMts[2].x);
	}

	hr = LCSYS::g_Lc_MsgPrc((LC_HWND)hWnd, uMsg, wParam, lParam);

	if(LC_OK == hr)
		return hr;


	if(WM_CLOSE == uMsg || WM_DESTROY == uMsg)
	{
		PostQuitMessage(0);
		return LC_OK;
	}


	hr = DefWindowProc((HWND)hWnd, uMsg, wParam, lParam);
#endif

	return hr;
}



// Create Window
LC_HWND LcSys_CreateWindow(pLc_MsgPrc WndProc
						   , char* ClassName
						   , INT scnW
						   , INT scnH
						   , INT scnX
						   , INT scnY
						   , LC_HANDLE Etc
						   )
{
	if(0 == scnW || 0 == scnH)
	{
		LcVar_WindowSize(&scnW, &scnH);
	}

	INT w = scnW;
	INT h = scnH;

#if !defined(_MSC_VER) && !defined(_PC_LINUX_)
	scnX = 0;
	scnY = 0;
#endif


#if defined(__APPLE__) || defined(__IPHONE__) || defined(__ANDROID__) || defined(__BADA__)
	if(h<w)
		LCSYS::g_LC_Orient = LCSYS_ORIENT_LLANDSCAPE;

	w = lc_min(scnW, scnH);
	h = lc_max(scnW, scnH);
#else

	if(0<scnW) LCSYS::g_LConstScnW = w;
	if(0<scnH) LCSYS::g_LConstScnH = h;
#endif


#if defined(__BADA__)
	return (LC_HWND)Etc;
#endif


	if(WndProc)
		LCSYS::g_Lc_MsgPrc = WndProc;



#if defined(_MSC_VER)
	HWND	hWnd = NULL;
	HINSTANCE	hInst = (HINSTANCE)GetModuleHandle(NULL);
	WNDCLASS wc=
	{
		CS_HREDRAW | CS_VREDRAW
		,	(WNDPROC)LcSys_MsgPrc
		,	0L
		,	0L
		,	hInst
		,	LoadIcon(NULL, IDI_APPLICATION)
		,	LoadCursor(NULL, IDC_ARROW)
		,	(HBRUSH)GetStockObject(WHITE_BRUSH)
		,	NULL
		,	ClassName
	};

	if( 0==	RegisterClass(&wc))
		return NULL;


	DWORD dStyle=WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
	RECT rc = {0,0, scnW, scnH};
	AdjustWindowRect(&rc, dStyle, 0);


	if(-1 == scnX)
	{
		scnX = ::GetSystemMetrics(SM_CXSCREEN)/2;
		scnX -= (rc.right  - rc.left)/2;
	}

	if(-1 == scnY)
	{
		scnY = ::GetSystemMetrics(SM_CYSCREEN)/3;
		scnY -=(rc.bottom - rc.top)/2;
	}


	// write the screen info
	TLC_VAR vScn;
	vScn._ix = scnX;
	vScn._iy = scnY;
	vScn._iw = scnW;
	vScn._ih = scnH;
	LcVar_Write(LC_VAR_SCREEN, &vScn);

	hWnd = CreateWindow(ClassName, ClassName, dStyle
						  , scnX, scnY
						  , rc.right  - rc.left
						  , rc.bottom - rc.top
						  , NULL, NULL, hInst, NULL);
	if (NULL == hWnd)
	{
		LcVar_Write(LC_VAR_SCREEN, NULL);
		return NULL;
	}

	EnableWindow( hWnd, 1);
	SetForegroundWindow(hWnd);
	ShowWindow(hWnd, SW_SHOWNORMAL);

	return (LC_HWND)hWnd;
#endif



#if defined(_PC_LINUX_)

	LC_HWND hWnd;
	// Open Display --> Create Window

	// Double buffering attribute
	INT AttDBL[] =
	{
		GLX_RGBA,					// Color Order Red, Green, Blue, Alpha
		GLX_DOUBLEBUFFER,			// Double Buffering Enable
		GLX_RED_SIZE,   8,			// Red Pixel Size 8-bit
		GLX_GREEN_SIZE, 8,			// Green Pixel Size 8-bit
		GLX_BLUE_SIZE,  8,			// Blue Pixel Size 8-bit
		GLX_ALPHA_SIZE, 8,			// Alpha Pixel Size 8-bit
		GLX_DEPTH_SIZE, 24,			// Depth resolution 24-bit
		GLX_STENCIL_SIZE, 8,		// Stencil Buffer size 8-bit
		None
	};


	// single buffering attribute
	INT AttSBL[] =
	{
		GLX_RGBA,
		GLX_RED_SIZE,   8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE,  8,
		None
	};


	Display*			pDspX = 0;		// Main Display
	Window				pWndX = 0;		// Display Window

	char*				sTitle= ClassName;
	XVisualInfo*		pVi;
	INT					root, screen;
	Atom				atmWM[16]={0};
	XSetWindowAttributes swa;

	Atom type_dst	= None;
	Atom type_stt	= None;
	Atom type_val1	= None;
	Atom type_val2	= None;

	long	event_msk = 0;


	if (!XInitThreads ())
	{
		LOGE ("XInitThreads Failed\n");
	    return NULL;
	}



	// Open Display
	if(NULL == (pDspX = XOpenDisplay ("localhost:0.0")))
	{
		pDspX = XOpenDisplay(0);
		if(NULL == pDspX)
		{
			LOGE("Open Display Failed\n");
			return NULL;
		}
	}


	// Get Default Screen
	screen = DefaultScreen(pDspX);

	// Choose Double buffer Rendering of Frame Buffer
	pVi = glXChooseVisual(pDspX, screen, AttDBL);
	if(NULL == pVi)
	{
		// Choose Sigle Frame buffer rendering of Frame Buffer
		pVi = glXChooseVisual(pDspX, screen, AttSBL);
		if(NULL == pVi)
		{
			LOGE("Choose Visual Failed\n");
			goto End;
		}
	}


	// Root Window
	root = RootWindow(pDspX, pVi->screen);

	// Set Property
	type_dst	= XInternAtom(pDspX, "_NET_WM_WINDOW_TYPE", False);
	type_val1	= XInternAtom(pDspX, "_NET_WM_WINDOW_TYPE_DIALOG", False);

	type_stt	= XInternAtom(pDspX, "_NET_WM_STATE", False);
	type_val2	= XInternAtom(pDspX, "_NET_WM_STAYS_ON_TOP", False);


	// Create Colormap and Setup Window Attribute
	swa.colormap = XCreateColormap(pDspX, root, pVi->visual, AllocNone);
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask;


	// Create Window for Rendering Display
	pWndX = XCreateWindow(pDspX
					, root
					, 0, 0, LCSYS::g_LC_ScnW, LCSYS::g_LC_ScnH
					, 0
					, pVi->depth, InputOutput
					, pVi->visual
					, CWBorderPixel|CWColormap|CWEventMask, &swa);

	if(!pWndX)
	{
		LOGE("Create XWindow Failed: %ld\n", pWndX);
		goto End;

	}


	// Set Property and value
	//	_NET_WM_WINDOW_TYPE
	//		_NET_WM_WINDOW_TYPE_DESKTOP
	//		_NET_WM_WINDOW_TYPE_DOCK
	//		_NET_WM_WINDOW_TYPE_TOOLBAR
	//		_NET_WM_WINDOW_TYPE_MENU
	//		_NET_WM_WINDOW_TYPE_DIALOG
	//		_NET_WM_WINDOW_TYPE_NORMAL
	//		_NET_WM_WINDOW_TYPE_SPLASH
	//		_NET_WM_WINDOW_TYPE_UTILITY
	//		_KDE_NET_WM_WINDOW_TYPE_OVERRIDE
	//
	//	_NET_WM_STATE
	//		_NET_WM_STATE_MODAL
	//		_NET_WM_STATE_STICKY
	//		_NET_WM_STATE_MAXIMIZED_VERT
	//		_NET_WM_STATE_MAXIMIZED_HORIZ
	//		_NET_WM_STATE_SHADED
	//		_NET_WM_STATE_SKIP_TASKBAR
	//		_NET_WM_STATE_SKIP_PAGER
	//		_NET_WM_STATE_HIDDEN
	//		_NET_WM_STATE_STAYS_ON_TOP
	//		_NET_WM_STATE_FULLSCREEN

	type_dst	= XInternAtom(pDspX, "_NET_WM_WINDOW_TYPE", False);
	type_val1	= XInternAtom(pDspX, "_NET_WM_WINDOW_TYPE_DIALOG", False);

	type_stt	= XInternAtom(pDspX, "_NET_WM_STATE", False);
	type_val2	= XInternAtom(pDspX, "_NET_WM_STAYS_ON_TOP", False);

	XChangeProperty(pDspX, pWndX, type_dst, XA_ATOM, 32, PropModeReplace, (unsigned char*)&type_val1, 1);
	XChangeProperty(pDspX, pWndX, type_stt, XA_ATOM, 32, PropModeReplace, (unsigned char*)&type_val2, 1);


	// Set Window Message
	atmWM[0] = XInternAtom(pDspX, "WM_PROTOCOLS", True);
	atmWM[1] = XInternAtom(pDspX, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(pDspX, pWndX, atmWM, 2);


	XSetStandardProperties(pDspX, pWndX, sTitle, sTitle, None, NULL, 0, NULL);
	XMapRaised(pDspX, pWndX);


	// Mapping to pDspX
	XMapWindow(pDspX, pWndX);
	//XFlush( pDspX );


	// Event Notify
	// event_msk  = ( ExposureMask | EnterWindowMask  | LeaveWindowMask );
	// event_msk |= ( KeyPressMask| KeyReleaseMask );
	// event_msk |= ( ButtonPressMask | ButtonReleaseMask );
	event_msk = KeyPressMask| KeyReleaseMask | ButtonPressMask | ButtonReleaseMask;

	XSelectInput(pDspX, pWndX, event_msk);


	// Compose Output value.
	hWnd = 	LC_OS_CreateWindow();

	hWnd->w = (LC_HANDLE)pWndX;
	hWnd->d = (LC_HANDLE)pDspX;
	hWnd->n = (LC_HANDLE)pVi;
	hWnd->t = NULL;


//	printf("Send Tmp: 0x%x,  Wnd: 0x%x, Dsp: x%x, Visual Info: x%x\n"
//			, pOut, pWndX, pDspX, pVi);

	LOGI("Create X-Window Success.\n");

	return hWnd;

End:
	if(pWndX)
	{
		//LOGI("Destroy Window\n");
		XDestroyWindow(pDspX, pWndX);
		pWndX = 0;
	}

	if(pDspX)
	{
		//LOGI("Close Display\n");
		XCloseDisplay(pDspX);
		pDspX = NULL;
	}

	LOGE("Create X-Window Failed.\n");
	return NULL;
#endif



#if !defined(_MSC_VER) && !defined(_PC_LINUX_)
	LC_HANDLE hRet = LC_OS_CreateWindow();

	if(NULL ==hRet)
	{
		LOGE("Create Window Failed.\n");
		return hRet;
	}

	LOGI("Create OS Window Success.\n");
	return hRet;
#endif


	return NULL;
}


// Release Window
INT LcSys_DestroyWindow(LC_HWND* hWnd)
{
	INT hr = LC_OK;

	if(NULL == *hWnd)
		return LC_OK;

#if !defined(_MSC_VER)
	free(*hWnd);
#endif

	*hWnd = NULL;


	LCSYS::g_LC_Active = LC_FALSE;			// Non Active

	LcLog_SetType(LC_LOG_NONE);			// -1: can't write log.

#if defined(_MSC_VER)
	hr = _fcloseall();						// Close all open files.

#elif defined(_PC_LINUX_)
	hr = fcloseall();						// Close all open files.

#endif

	return hr;
}



// Setup Main Run Function Pointer.  Callback Application Program
INT LcSys_SetMainRunFunc(INT (LC_CALLBACK*pFunc)(LC_HANDLE) )
{
	LCSYS::g_Lc_Run = pFunc;
	return LC_OK;
}


INT LcSys_ElapseTime(DOUBLE* Elapsed, INT bWrite)
{
	if(bWrite)
	{
		if(Elapsed)
			LCSYS::g_LC_ElapseTime = *Elapsed ;
		else
			LCSYS::g_LC_ElapseTime = 0;
	}
	else
	{
		if(Elapsed)
			*Elapsed = LCSYS::g_LC_ElapseTime;
	}

	return LC_OK;
}


INT LcSys_FramePerSecond(DOUBLE* Frames, INT bWrite)
{
	if(bWrite)
	{
		if(Frames)
			LCSYS::g_LC_Frame = *Frames ;
		else
			LCSYS::g_LC_Frame = 0;
	}
	else
	{
		if(Frames)
			*Frames = LCSYS::g_LC_Frame;
	}

	return LC_OK;
}


INT LcSys_StoredTime(DOUBLE* Stored, INT bWrite)
{
	if(bWrite)
	{
		if(Stored)
			LCSYS::g_LC_StoredTime = *Stored ;
		else
			LCSYS::g_LC_StoredTime = 0;
	}
	else
	{
		if(Stored)
			*Stored = LCSYS::g_LC_StoredTime;
	}
	return LC_OK;
}



INT LcSys_DestroyObject(PLC_OBJECT* pData)
{
	INT hr =0;

	// Return Reference Count
	if(NULL == *pData)
	{
		hr = LcVar_RefCountObject();
		return hr;
	}



	// if there is an owner then this object cannot be released
	//if( 0 <= (*pData)->Query(LCQUERY_GET_OWNER, NULL) )
	//	return LC_EFAIL;

	hr = (*pData)->Release();
	if(0 >hr)
		return hr;


	(*pData) = NULL;
	return hr;
}



////////////////////////////////////////////////////////////////////////////////
// JNI
struct StreamList
{
	int		rsId;						// Resource Id
	int		streamId;					// Stream Id
	char	name[LC_DEFAULT_NAME];		// Name(Key)
	void*	pData;						// Aux data
};


namespace LCSYS
{
	JavaVM*					g_Jvm = NULL;	// Java virtual machine system
	std::vector<StreamList > g_Strm(0);		// Stream Id list
}


#ifdef __cplusplus
extern "C" {
#endif

JavaVM* JNI_OnGetJavaVM()
{
	return LCSYS::g_Jvm;
}


void JNI_OnUnload(JavaVM* vm, void* reserved)
{
	LCSYS::g_Jvm = NULL;

	if(LCSYS::g_Strm.empty() )
		return;


//	int iSize = (int)LCSYS::g_Strm.size();
//	StreamList* pLst;
//
//	for(i=0; i<iSize; ++i)
//	{
//		pLst = &LCSYS::g_Strm[i];
//
//		if(pLst->pData)
//			free(pLst->pData);
//	}

	LCSYS::g_Strm.clear();
}



// Global Interface
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LCSYS::g_Jvm = vm;
	JNIEnv* pEnv = NULL;
	jint result = -1;

	LOGW("JNI_OnLoad----------------------\n");


#if defined(__ANDROID__)
	if (vm->GetEnv((void**) &pEnv, JNI_VERSION_1_2) != JNI_OK)
	{
		LOGE("GetEnv failed!---------------\n");
		return result;
	}

	if(!pEnv)
		LOGE("Could not retrieve the pEnv!---------------\n");
#endif

	return JNI_VERSION_1_2;
}


int JNI_OnFindStreamName(char* name)
{
	int i;
	StreamList* pLst;
	int iSize = (int)LCSYS::g_Strm.size();


	for(i=0; i<iSize; ++i)
	{
		pLst = &LCSYS::g_Strm[i];

		if(0 >= pLst->streamId)
			continue;

		if(0 == LcStr_Stricmp(pLst->name, name) )
			return pLst->streamId;
	}

	return -1;
}


int JNI_OnFindStreamRscId(int rsId)
{
	int i;
	StreamList* pLst;
	int iSize = (int)LCSYS::g_Strm.size();


	for(i=0; i<iSize; ++i)
	{
		pLst = &LCSYS::g_Strm[i];

		if(0 >= pLst->streamId)
			continue;

		if(pLst->rsId == rsId)
			return pLst->streamId;
	}

	return -1;
}


int JNI_OnAddStream(char* name, int rsId, int streamid, void* pAux)
{
	StreamList dst;
	int idx = 0;

	strcpy(dst.name, name);
	dst.rsId     = rsId;
	dst.streamId = streamid;
	dst.pData    = pAux;

	LCSYS::g_Strm.push_back(dst);

	idx = (int)LCSYS::g_Strm.size();

	idx -= 1;

	return idx;
}



#ifdef __cplusplus
}
#endif


INT LcSys_SetAttrib(UINT Cmd,LC_HANDLE pData)
{
	if(LCSYS_ELAPSED_TIME == Cmd)
	{
		if(pData) LCSYS::g_LC_ElapseTime = *((DOUBLE*)pData);
		else      LCSYS::g_LC_ElapseTime = 0;
	}
	else if(LCSYS_STORED_TIME  == Cmd)
	{
		if(pData) LCSYS::g_LC_StoredTime= *((DOUBLE*)pData);
		else      LCSYS::g_LC_StoredTime = 0;
	}

	else if(LCSYS_WINDOW_SIZE  == Cmd)
	{
		INT* v= (INT*)pData;

		LcVar_WindowSize(&v[0], &v[1], LC_TRUE);
	}

	else if(LCSYS_CONST_WINDOW_SIZE== Cmd)
	{
		INT* v= (INT*)pData;
		LCSYS::g_LConstScnW= v[0];
		LCSYS::g_LConstScnH= v[1];
	}
	else if(LCSYS_ORIENT_PORTRAIT   == Cmd) LCSYS::g_LC_Orient = LCSYS_ORIENT_PORTRAIT  ;
	else if(LCSYS_ORIENT_RLANDSCAPE == Cmd) LCSYS::g_LC_Orient = LCSYS_ORIENT_RLANDSCAPE;
	else if(LCSYS_ORIENT_LLANDSCAPE == Cmd) LCSYS::g_LC_Orient = LCSYS_ORIENT_LLANDSCAPE;
	else   return LC_EFAIL;

	return LC_OK;
}


INT LcSys_GetAttrib(UINT Cmd,LC_HANDLE pData)
{
	if     (LCSYS_ELAPSED_TIME == Cmd) *((DOUBLE*)pData) = LCSYS::g_LC_ElapseTime;
	else if(LCSYS_STORED_TIME  == Cmd) *((DOUBLE*)pData) = LCSYS::g_LC_StoredTime;
	else if(LCSYS_ORIENT       == Cmd) *((INT*   )pData) = LCSYS::g_LC_Orient;
	else if(LCSYS_WINDOW_SIZE  == Cmd)
	{
		INT* v= (INT*)pData;
		LcVar_WindowSize(&v[0], &v[1], LC_FALSE);
	}
	else if(LCSYS_CONST_WINDOW_SIZE == Cmd)
	{
		INT* v= (INT*)pData;
		v[0] = LCSYS::g_LConstScnW;
		v[1] = LCSYS::g_LConstScnH;
	}
	
	else   return LC_EFAIL;

	return LC_OK;
}



void LcSys_OrientMatrix(void* pOut, INT nModelType)		// Get the system window direction matrix. needs FLOAT*16. ModelType:: LC_OBJ_SPRITE, LC_OBJ_GUI, LC_RSC_MODEL2D
{
	INT _scnW, _scnH;
	LcVar_WindowSize(&_scnW, &_scnH);
	INT nOrient= LCSYS::g_LC_Orient;
	FLOAT scnW = (FLOAT)_scnW;
	FLOAT scnH = (FLOAT)_scnH;

	((LCXMATRIX*)pOut)->Identity();

	if(LCSYS_ORIENT_LLANDSCAPE == nOrient)
	{
		if(LC_OBJ_SPRITE == nModelType || LC_OBJ_GUI == nModelType)
		{
			*((LCXMATRIX*)pOut)=LCXMATRIX(
							0.0F,  1.0F, 0.0F, 0.0F
						  ,-1.0F,  0.0F, 0.0F, 0.0F
						  , 0.0F,  0.0F, 0.0F, 0.0F
						  , scnW,  0.0F, 0.0F, 1.0F);
		}
		else if(LC_OBJ_MODEL2D == nModelType)
		{
			*((LCXMATRIX*)pOut)=LCXMATRIX(
							0.0F,  1.0F, 0.0F, 0.0F
						  ,-1.0F,  0.0F, 0.0F, 0.0F
						  , 0.0F,  0.0F, 0.0F, 0.0F
						  , 0.0F,  0.0F, 0.0F, 1.0F);
		}
	}
	else if(LCSYS_ORIENT_RLANDSCAPE == nOrient)
	{
		if(LC_OBJ_SPRITE == nModelType || LC_OBJ_GUI == nModelType)
		{
			*((LCXMATRIX*)pOut)=LCXMATRIX(
							0.0F, -1.0F, 0.0F, 0.0F
						  , 1.0F,  0.0F, 0.0F, 0.0F
						  , 0.0F,  0.0F, 0.0F, 0.0F
						  , 0.0F,  scnH, 0.0F, 1.0F);
		}
		else if(LC_OBJ_MODEL2D == nModelType)
		{
			*((LCXMATRIX*)pOut)=LCXMATRIX(
							0.0F, -1.0F, 0.0F, 0.0F
						  , 1.0F,  0.0F, 0.0F, 0.0F
						  , 0.0F,  0.0F, 0.0F, 0.0F
						  , 0.0F,  scnH, 0.0F, 1.0F);
		}
	}
}

