// Implementation of the ILC_Device.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_PC_LINUX_)
  #include <X11/keysym.h>
  #include <GL/gl.h>
  #include <GL/glext.h>
  #include <GL/glx.h>
  #include <unistd.h>

#elif defined(__APPLE__) || defined(__IPHONE__)
  #import <OpenGLES/ES1/gl.h>

#elif defined(__ANDROID__)
//  #include <EGL/egl.h>
  #include <GLES/gl.h>
  #include <android/log.h>

#elif defined(__BADA__)
  #include <FBase.h>
  #include <FBaseSys.h>
  #include <FGraphicsOpengl.h>

  using namespace Osp::Graphics::Opengl;

#else
  #include <gles/egl.h>
  #include <gles/gl.h>

#endif

#include <LcType.h>
#include <LcMath.h>

#include "LcAux.h"
#include "LcBase.h"
#include "LcDevice.h"


LC_HANDLE	CLcDevice::m_pEgDsp		= NULL	;		// Egl Display
LC_HANDLE	CLcDevice::m_pEgSrf		= NULL	;		// Egl Surface
LC_HANDLE	CLcDevice::m_pEgCtx		= NULL	;		// Egl Context
LC_HANDLE	CLcDevice::m_pEgCfg[LC_MAX_EGL_CONFIG]={0};
UINT		CLcDevice::m_fbFrame	= 0	;			// Default Frame Buffer for iPhone
UINT		CLcDevice::m_fbColor	= 0	;			// Color Render Buffer for iPhone

LCXCOLOR	CLcDevice::m_ClearColor(UINT(0xFF001122));
INT			CLcDevice::m_dFVF = LC_FVF_XYZ			;
INT			CLcDevice::m_iFVF = LC_FMT_INDEX16		;
INT			CLcDevice::m_BlndS= GL_SRC_ALPHA			;
INT			CLcDevice::m_BlndD= GL_ONE_MINUS_SRC_ALPHA	;
LCXMATRIX	CLcDevice::m_mtWld	;
LCXMATRIX	CLcDevice::m_mtViw	;
LCXMATRIX	CLcDevice::m_mtPrj	;

PLC_TEXTURE CLcDevice::m_pTex  [4] = {NULL};		// Current Texture[4]
INT			CLcDevice::m_TssAdS[4] = {LC_TS_REPEAT, LC_TS_REPEAT, LC_TS_REPEAT, LC_TS_REPEAT};
INT			CLcDevice::m_TssAdT[4] = {LC_TS_REPEAT, LC_TS_REPEAT, LC_TS_REPEAT, LC_TS_REPEAT};
INT			CLcDevice::m_TssMin[4] = {LC_TS_LINEAR, LC_TS_LINEAR, LC_TS_LINEAR, LC_TS_LINEAR};
INT			CLcDevice::m_TssMag[4] = {LC_TS_LINEAR, LC_TS_LINEAR, LC_TS_LINEAR, LC_TS_LINEAR};
INT			CLcDevice::m_TssOp [4] = {LC_TS_MODULATE,LC_TS_MODULATE,LC_TS_MODULATE,LC_TS_MODULATE};



#ifndef EGL_CONTEXT_CLIENT_VERSION
#define EGL_CONTEXT_CLIENT_VERSION	0x3098
#endif



CLcDevice::CLcDevice()
{
	LcVar_WriteP(LC_VAR_DEVICE, this);

	CLcDevice::m_mtWld.Identity();
	CLcDevice::m_mtViw.Identity();
	CLcDevice::m_mtPrj.Identity();
}


CLcDevice::~CLcDevice()
{
	Destroy();
}


INT CLcDevice::Query(char* sCmd, LC_HANDLE pData)
{
	if( 0 == LcStr_Stricmp(sCmd, LCQUERY_GET_SCREEN_WIDTH_HEIGHT))
	{
		if(pData)
		{
			INT	ScnW=0, ScnH=0;
			INT* pBuf = (INT*)pData;

			LcVar_WindowSize(&ScnW, &ScnH);

			pBuf[0] = ScnW	;
			pBuf[1] = ScnH	;
		}
	}
	else
		return LC_EFAIL;


	return LC_OK;
}


INT CLcDevice::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcDevice::Destroy()
{
	if(NULL == m_pEgCtx)
		return;

	LC_HWND hWnd = (LC_HWND)LcVar_ReadP(LC_VAR_WINDOW);

	glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
	this->Present();


#if defined(_PC_LINUX_)
	//LOGW("Destroy Context\n");
	glXMakeCurrent((Display*)m_pEgDsp, None, NULL);
	glXDestroyContext((Display*)m_pEgDsp, (GLXContext)m_pEgCtx);

	//LOGW("Destroy Window\n");
	XDestroyWindow((Display*)m_pEgDsp, (Window)hWnd->w);

	//LOGW("Close Display\n");
	XCloseDisplay((Display*)m_pEgDsp);


#elif defined(__APPLE__) || defined(__IPHONE__)
	XcodeEglDestroy(&m_pEgCtx, &m_fbFrame, &m_fbColor);

#elif !defined(__ANDROID__)

	eglMakeCurrent(m_pEgDsp, 0, 0, 0);
	eglDestroyContext(m_pEgDsp, m_pEgCtx);
	eglDestroySurface(m_pEgDsp, m_pEgSrf);

	#if defined(_WIN32)
		eglTerminateWin32((NativeDisplayType)hWnd, (EGLDisplay)m_pEgDsp);
	#else
		eglTerminate(m_pEgDsp);
	#endif

#endif

	m_fbColor	= 0;
	m_fbFrame	= 0;

	m_pEgCtx	= NULL;
	m_pEgSrf	= NULL;
	m_pEgDsp	= NULL;
	hWnd		= NULL;

	LcVar_WriteP(LC_VAR_DEVICE, hWnd);
}



////////////////////////////////////////////////////////////////////////////////
//
INT CLcDevice::CreateForLinux(LC_HANDLE p1, LC_HANDLE p2)
{
	INT hr = LC_EFAIL;
	if(NULL == p1)
		return LC_INVALID_CALL;


#if defined(_PC_LINUX_)
	LC_HWND			pWnd = (LC_HWND)p1;

	Window			hWnd = (Window)pWnd->w;
	XVisualInfo*	pVi	 = (XVisualInfo*)pWnd->n;

	// Caution Order !!!
	m_pEgDsp= pWnd->d;

	LOGW("Recv Tmp: %p Wnd: %p, Dsp: %p, Visual Info: %p\n", pTmp, hWnd, m_pEgDsp, pVi);


	// Create Rendering Context
	if(NULL == (m_pEgCtx = glXCreateContext((Display*)m_pEgDsp, pVi, 0, GL_TRUE)))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("glXCreateContext() Failed\n");
		return LC_CANNT_ALLOC;
	}


	// Set Make current of Rendering Context
	glXMakeCurrent((Display*)m_pEgDsp, (Window)hWnd, (GLXContext)m_pEgCtx);


	// save the window handle
	LcVar_WriteP(LC_VAR_WINDOW, hWnd);


	glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);	// Confirm Device Operation with Clear Color
	glShadeModel(GL_SMOOTH);
//	glTextureDither(0);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glXSwapBuffers((Display*)m_pEgDsp, (Window)hWnd);
	glFlush();

	LOGW("GLX Complete Creating.\n");

	hr = LC_OK;
#endif

	return hr;
}



////////////////////////////////////////////////////////////////////////////////
//
INT CLcDevice::CreateForIphone(LC_HANDLE p1, LC_HANDLE p2)
{
	INT hr = LC_EFAIL;


#if defined(__APPLE__) || defined(__IPHONE__)

	hr = XcodeEglCreate(&m_pEgCtx, &m_fbFrame, &m_fbColor);
	if(LC_FAILED(hr))
		return hr;

	glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);	// Confirm Device Operation with Clear Color
	glShadeModel(GL_SMOOTH);

//	woops!! bugs.... in iPhone
//	if you want to rendering, it needs before callinng the "[pCtx renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];"
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	LcEAGL_SwapBuffers(m_pEgCtx);
//	glFlush();

	hr = LC_OK;
#endif

	return hr;
}



////////////////////////////////////////////////////////////////////////////////
//
INT CLcDevice::CreateForAndroid(LC_HANDLE p1, LC_HANDLE p2)
{
	INT hr = LC_OK;

	LC_HWND hWnd  = (LC_HWND)p1;

/*
	INT		match  = 0;
	EGLint	format = 0;
	EGLint	ver_maj= 0;
	EGLint	ver_min= 0;

	struct _Tconfig_att { EGLint a; EGLint b;	};
	const _Tconfig_att config_attrib[] =
	{
		{	EGL_RED_SIZE,		 8				},
		{	EGL_GREEN_SIZE,		 8				},
		{	EGL_BLUE_SIZE,		 8				},
		{	EGL_ALPHA_SIZE,		 8				},
		{	EGL_DEPTH_SIZE  ,   16				},
		{	EGL_SURFACE_TYPE,	EGL_WINDOW_BIT	},
		{	EGL_NONE,			EGL_NONE,		},
	};
*/

	//LOGI("Android Option: %d-------------------\n", bUseAndroidGLSurface);

	INT scn[2]={0};
	FLOAT f[4]={0};
	glGetFloatv(GL_VIEWPORT, f);

	scn[0] = (INT)lc_min( f[2], f[3]);
	scn[1] = (INT)lc_max( f[2], f[3]);

	LcSys_SetAttrib(LCSYS_CONST_WINDOW_SIZE, scn);
	LOGW("CLcDevice::System window: %d %d\n", scn[0], scn[1]);

	LcVar_WriteP(LC_VAR_WINDOW, hWnd);


	if(p2)
	{
		#if defined(__ANDROID__)

		glDisable(GL_DITHER);
		
		#endif

		LOGI("CLcDevice::EGL Created in JAVA Frame Work\n");

		return LC_OK;
	}



	hr = LC_OK;
	return hr;
}


//#if !defined(EGL_RENDERABLE_TYPE)
//  #define EGL_RENDERABLE_TYPE	0x3040
//#endif
//
//#if !defined(EGL_OPENGL_ES_BIT)
//  #define EGL_OPENGL_ES_BIT		0x0001
//#endif
//
//#if !defined(EGL_OPENGL_ES_API)
//  #define EGL_OPENGL_ES_API		0x30A0
//#endif

////////////////////////////////////////////////////////////////////////////////
//
INT CLcDevice::CreateForBada(LC_HANDLE p1, LC_HANDLE p2)
{
	if(NULL == p1)
	{
		LOGE("Err: Form Handle doesn't exist: CreateForBada() Failed\n");
		return LC_INVALID_CALL;
	}

#if defined(__BADA__)

	struct _Tconfig_att { EGLint a; EGLint b;	};
	const _Tconfig_att config_attrib[] =
	{
		{	EGL_RED_SIZE,		 8				},
		{	EGL_GREEN_SIZE,		 8				},
		{	EGL_BLUE_SIZE,		 8				},
		{	EGL_ALPHA_SIZE,		 8				},
		{	EGL_DEPTH_SIZE  ,   16				},
		{	EGL_SURFACE_TYPE,	EGL_WINDOW_BIT	},
		{	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT},
		{	EGL_NONE,			EGL_NONE,		},
	};

	const _Tconfig_att config_context[] =
	{
		{	EGL_CONTEXT_CLIENT_VERSION, 1			},
		{	EGL_NONE,					EGL_NONE	},
	};

	INT				match = 0;
	EGLint			ver_maj=0;
	EGLint			ver_min=0;
	NativeWindowType hWnd= (NativeWindowType)p1;


	LOGI("CreateForBada(): hWnd- 0x%X\n", hWnd);


	eglBindAPI(EGL_OPENGL_ES_API);
	LOGI("eglBindAPI():----------\n");



	m_pEgDsp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(NULL == m_pEgDsp)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("eglGetDisplay() Failed\n");
		return LC_CANNT_ALLOC;
	}

	LOGI("eglGetDisplay() Success- Dsp: 0x%X\n", m_pEgDsp);


	if(!eglInitialize(m_pEgDsp, &ver_maj, &ver_min))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("eglInitialize() Failed\n");
		return LC_CANNT_ALLOC;
	}

	LOGI("Egl Version: 0x%X, 0x%X\n", ver_maj, ver_min);

	if(!eglChooseConfig(m_pEgDsp, (const EGLint*)config_attrib, &m_pEgCfg[0], LC_MAX_EGL_CONFIG, &match))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("eglChooseConfig() Failed\n");
		return LC_CANNT_ALLOC;
	}

	LOGI("eglChooseConfig() Success\n");

	if(1>match)	// No Match
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("eglChooseConfig() Failed\n");
		return LC_CANNT_ALLOC;
	}

	m_pEgSrf = eglCreateWindowSurface(m_pEgDsp, m_pEgCfg[0], hWnd, NULL);
	if(NULL == m_pEgSrf)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("eglCreateWindowSurface() Failed\n");
		return LC_CANNT_ALLOC;
	}

	LOGI("eglCreateWindowSurface() Success\n");

	m_pEgCtx = eglCreateContext(m_pEgDsp, m_pEgCfg[0], NULL, (const EGLint*)config_context);
	if(NULL == m_pEgCtx)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("eglCreateContext() Failed\n");
		return LC_CANNT_ALLOC;
	}

	LOGI("eglCreateContext() Success\n");

	if(!eglMakeCurrent(m_pEgDsp, m_pEgSrf, m_pEgSrf, m_pEgCtx))
	{
		LOGE("eglMakeCurrent() Err");
		return LC_EFAIL;
	}

	LOGW("eglMakeCurrent() Success\n");


	// save the window handle
	LcVar_WriteP(LC_VAR_WINDOW, hWnd);


	int ScnW=0, ScnH=0;
	eglQuerySurface(m_pEgDsp, m_pEgSrf, EGL_WIDTH,  &ScnW);		// get the screen width
	eglQuerySurface(m_pEgDsp, m_pEgSrf, EGL_HEIGHT, &ScnH);		// get the screen height
	glViewport(0, 0, ScnW, ScnH);								// Setup the viewport


	glShadeModel(GL_SMOOTH);									// select the smooth shading model.
	glDisable(GL_DEPTH_TEST);									// disable the depth test
	glDisable(GL_CULL_FACE);									// disable the culling face


	// for Test...
	glClearColor(m_ClearColor.r, m_ClearColor.g,m_ClearColor.b,m_ClearColor.a);	// Confirm Device Operation with Clear Color
	glShadeModel(GL_SMOOTH);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	eglSwapBuffers(m_pEgDsp, m_pEgSrf);
	glFlush();

	LOGW("Complete EGL Create\n");

#endif

	return LC_OK;
}







// Setup EGL for Windows
////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
extern INT LcDev_GLInit();
#endif

INT CLcDevice::CreateForWin(LC_HANDLE p1, LC_HANDLE p2)
{
	INT hr = LC_EFAIL;
	if(NULL == p1)
		return LC_INVALID_CALL;



#if defined(_WIN32)

	LC_HWND hWnd = (LC_HWND)p1;

	// Setup Display
	// receive HDC
	m_pEgDsp = eglGetDisplay((NativeDisplayType)hWnd);
	if(NULL == m_pEgDsp)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("eglGetDisplay() Failed\n");
		return LC_CANNT_ALLOC;
	}

	// m_pEgSrf is same to HDC
	if(NULL == eglCreateWindowSurface(m_pEgDsp, NULL, hWnd, 0) )
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("eglCreateWindowSurface() Failed\n");
		return LC_CANNT_ALLOC;
	}

	//LOGW("eglCreateWindowSurface() Success\n");

	m_pEgCtx = eglCreateContext(m_pEgDsp, NULL, NULL, NULL);
	if(NULL == m_pEgCtx)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("eglCreateContext() Failed\n");
		return LC_CANNT_ALLOC;
	}

	//LOGW("eglCreateContext() Success\n");

	// Make Current
	if(!eglMakeCurrent(m_pEgDsp, m_pEgSrf, m_pEgSrf, m_pEgCtx))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("eglMakeCurrent() Failed\n");
		return LC_CANNT_ALLOC;
	}


	//LOGW("eglMakeCurrent() Success\n");

	if(0>LcDev_GLInit())
	{
		LOGE("Initialize Device Failed.\n");
		return LC_CANNT_ALLOC;
	}

	if(!eglSwapInterval(m_pEgDsp, 0))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("eglSwapInterval() Failed\n");
		return LC_CANNT_ALLOC;
	}


	// save the window handle
	LcVar_WriteP(LC_VAR_WINDOW, hWnd);


	glColor4f(1,1,1,1);

	glClearColor(CLcDevice::m_ClearColor.r, CLcDevice::m_ClearColor.g
				, CLcDevice::m_ClearColor.b, CLcDevice::m_ClearColor.a);	// Confirm Device Operation with Clear Color

	glShadeModel(GL_SMOOTH);
//	glTextureDither(0);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	eglSwapBuffers(m_pEgDsp, m_pEgSrf);
	glFlush();

	LOGW("Complete EGL Create\n");

	hr = LC_OK;
#endif

	return hr;
}

////////////////////////////////////////////////////////////////////////////////




INT CLcDevice::SetTransform(INT nTM, const LC_HANDLE v)
{
	if(NULL == v || (LC_TS_WORLD_IDENTITY<= nTM && nTM <=LC_TS_PROJ_IDENTITY))
	{
		if      (LC_TS_WORLD_IDENTITY== nTM)	m_mtWld.Identity();
		else if	(LC_TS_VIEW_IDENTITY == nTM)	m_mtViw.Identity();
		else if	(LC_TS_PROJ_IDENTITY == nTM)	m_mtPrj.Identity();
	}
	else
	{
		if		(LC_TS_WORLD== nTM)	memcpy(&m_mtWld, v, sizeof(LCXMATRIX));
		else if	(LC_TS_VIEW == nTM)	memcpy(&m_mtViw, v, sizeof(LCXMATRIX));
		else if	(LC_TS_PROJ == nTM)	memcpy(&m_mtPrj, v, sizeof(LCXMATRIX));
		else	return LC_EFAIL;
	}


	if(LC_TS_WORLD == nTM || LC_TS_VIEW == nTM)
	{
		LCXMATRIX	mtWldViw = CLcDevice::m_mtWld*CLcDevice::m_mtViw;		// OpenGL is a column-based matrix. but it's processing is same  to DirectX. Matrix multiplication have to be multiply left to right as the world * view * projection transformation process.

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((FLOAT*)&mtWldViw);
	}

	if(LC_TS_PROJ == nTM)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((FLOAT*)&CLcDevice::m_mtPrj);
	}

	return LC_OK;
}

INT CLcDevice::GetTransform(INT nTM, LC_HANDLE v)
{
	if		(LC_TS_WORLD== nTM)	memcpy(v, &CLcDevice::m_mtWld, sizeof(LCXMATRIX));
	else if	(LC_TS_PROJ == nTM)	memcpy(v, &CLcDevice::m_mtPrj, sizeof(LCXMATRIX));
	else if	(LC_TS_VIEW == nTM)	memcpy(v, &CLcDevice::m_mtViw, sizeof(LCXMATRIX));
	else	return LC_EFAIL;


	return LC_OK;
}


INT CLcDevice::SetTextureEnv(INT v1, INT v2)
{
	glTexEnvi(GL_TEXTURE_ENV, v1, v2);

	return LC_OK;
}


INT CLcDevice::SetTextureAddressS (INT nState, INT v) { if(nState & 0xFFFFFFFC) return LC_EFAIL; CLcDevice::m_TssAdS[nState] = v; return LC_OK;	}	// nState & 0xFFFFFFFC: 0~3¸¸ »ç¿ë
INT CLcDevice::SetTextureAddressT (INT nState, INT v) { if(nState & 0xFFFFFFFC) return LC_EFAIL; CLcDevice::m_TssAdT[nState] = v; return LC_OK;	}
INT CLcDevice::SetTextureFilterMin(INT nState, INT v) { if(nState & 0xFFFFFFFC) return LC_EFAIL; CLcDevice::m_TssMin[nState] = v; return LC_OK;	}
INT CLcDevice::SetTextureFilterMag(INT nState, INT v) { if(nState & 0xFFFFFFFC) return LC_EFAIL; CLcDevice::m_TssMag[nState] = v; return LC_OK;	}
INT CLcDevice::SetTextureStageOp  (INT nState, INT v) { if(nState & 0xFFFFFFFC) return LC_EFAIL; CLcDevice::m_TssOp [nState] = v; return LC_OK;	}


INT CLcDevice::SetFVF(INT nFVF)
{
	if(!(LC_FVF_XYZ & nFVF))
		return LC_EFAIL;

	CLcDevice::m_dFVF = nFVF;

	if(0 == CLcDevice::m_dFVF)
	{
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	return LC_OK;
}


INT CLcDevice::SetTexture(INT nState, PLC_TEXTURE pTex)
{
	if(nState & 0xFFFFFFFC)
		return LC_EFAIL;

	CLcDevice::m_pTex[nState] = pTex;

	if(NULL == pTex)
	{
		glActiveTexture(GL_TEXTURE0 + nState);
		glClientActiveTexture(GL_TEXTURE0 + nState);
		glBindTexture (GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);

//		return LC_OK;
	}

	return LC_OK;

//	return m_pTex->BindTexture(m_TexOp, m_TexFilterMin, m_TexFilterMag, m_TexFilterAddS, m_TexFilterAddT);
}


INT CLcDevice::SetColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
{
	::glColor4f(r, g, b, a);
	return LC_OK;
}


INT CLcDevice::SetColor(FLOAT* v)
{
	::glColor4f(v[0], v[1], v[2], v[3]);
	return LC_OK;
}



INT CLcDevice::BeginScene()
{
	INT scn[2]={0};
	LcSys_GetAttrib(LCSYS_CONST_WINDOW_SIZE, scn);

//	LOGW("Sreen Size: %d, %d--------------\n", scn[0], scn[1]);


#if defined(__APPLE__) || defined(__IPHONE__)
	XcodeEglSetFrameBuffer(m_pEgCtx, m_fbFrame);
#endif

	// setup the viewport:: sume device needs setup the viewport like iphone
	glViewport(0,0, scn[0], scn[1]);

	return LC_OK;
}

INT	CLcDevice::EndScene()
{
	return LC_OK;
}



INT CLcDevice::SetRenderState(INT ns, INT v)
{
	if(LC_RS_SHADE_MODEL == ns)
	{
		glShadeModel(v);
	}

	else if(LC_RS_DEPTH_TEST == ns)
	{
		if(v)
			glEnable(ns);

		else
			glDisable(ns);
	}

	else if(LC_RS_CULL_MODE == ns)
	{
		glFrontFace(v);
	}

	else if(LC_RS_CULL_FACE == ns)
	{
		if(LC_ST_BACK == v || LC_ST_FRONT == v)
			glCullFace(v);

		else if(LC_ST_ENABLE == v)
			glEnable(ns);

		else if(LC_ST_DISABLE== v)
			glDisable(ns);
	}

	else if(LC_RS_CLEAR_COLOR == ns)
	{
		UINT dColor = v;
		CLcDevice::m_ClearColor = dColor;
		glClearColor(CLcDevice::m_ClearColor.r, CLcDevice::m_ClearColor.g
					, CLcDevice::m_ClearColor.b, CLcDevice::m_ClearColor.a);
	}

	else if(LC_RS_BLEND_SRC == ns)
	{
		CLcDevice::m_BlndS = v;
		glBlendFunc( CLcDevice::m_BlndS, CLcDevice::m_BlndD);
	}

	else if(LC_RS_BLEND_DEST == ns)
	{
		CLcDevice::m_BlndD = v;
		glBlendFunc( CLcDevice::m_BlndS, CLcDevice::m_BlndD);
	}


	else if(LC_RS_BLEND == ns)
	{
		if(v)
			glEnable(ns);

		else
			glDisable(ns);
	}

	else if(LC_RS_FOG == ns)
	{
		if(v)
			glEnable(ns);
		else
			glDisable(ns);
	}

	else if(LC_RS_LIGHTING == ns)
	{
		if(v)
			glEnable(ns);
		else
			glDisable(ns);
	}

	else if(LC_RS_LIGHT0 <= ns && ns <= LC_RS_LIGHT7)
	{
		if(v)
			glEnable(ns);
		else
			glDisable(ns);
	}


	return LC_OK;
}


INT CLcDevice::SetRenderState(INT ns, LC_HANDLE v)
{
	INT hr = LC_OK;

	if(LC_RS_CLEAR_COLOR == ns)
	{
		LCXCOLOR* dColor = (LCXCOLOR*)v;
		CLcDevice::m_ClearColor = *dColor;
		glClearColor(CLcDevice::m_ClearColor.r, CLcDevice::m_ClearColor.g
					, CLcDevice::m_ClearColor.b, CLcDevice::m_ClearColor.a);
	}

	else if(LC_RS_FOG_DATA == ns)
	{
		TLC_FOG*  p = (TLC_FOG*)v;

		glFogfv(GL_FOG_COLOR, (const GLfloat *)&p->FogColor);
		glFogf(GL_FOG_MODE,	 p->FogMode);
		glFogf(GL_FOG_START, p->FogBegin);
		glFogf(GL_FOG_END,	 p->FogEnd);
	}

	else if(LC_RS_LIGHT0_DATA <= ns && ns <= LC_RS_LIGHT7_DATA )
	{
		TLC_LIGHT*  p = (TLC_LIGHT*)v;
		INT nLgt = ns - LC_RS_LIGHT0_DATA + GL_LIGHT0;

		if(0.0F < p->Amb.a)	glLightfv(nLgt, GL_AMBIENT				, (const GLfloat *)&p->Amb);
		if(0.0F < p->Dif.a)	glLightfv(nLgt, GL_DIFFUSE				, (const GLfloat *)&p->Dif);
		if(0.0F < p->Spc.a)	glLightfv(nLgt, GL_SPECULAR				, (const GLfloat *)&p->Spc);
		if(0.0F !=p->Pos.w)	glLightfv(nLgt, GL_POSITION				, (const GLfloat *)&p->Pos);
		if(0.0F !=p->Dir.w)	glLightfv(nLgt, GL_SPOT_DIRECTION		, (const GLfloat *)&p->Dir);
		if(0.0F !=p->Spe)	glLightf (nLgt, GL_SPOT_EXPONENT		,                   p->Spe);
		if(0.0F !=p->Cut)	glLightf (nLgt, GL_SPOT_CUTOFF			,                   p->Cut);
		if(0.0F !=p->Att0)	glLightf (nLgt, GL_CONSTANT_ATTENUATION	,                   p->Att0);
		if(0.0F !=p->Att1)	glLightf (nLgt, GL_LINEAR_ATTENUATION	,                   p->Att1);
		if(0.0F !=p->Att2)	glLightf (nLgt, GL_QUADRATIC_ATTENUATION,                   p->Att2);
	}

	else if(LC_RS_MATERIAL_DATA == ns)
	{
		TLC_MATERIAL*  p = (TLC_MATERIAL*)v;
		if(0.0F < p->Amb.a)	glMaterialfv(p->Fce, GL_AMBIENT , (const GLfloat *)&p->Amb);
		if(0.0F < p->Dif.a)	glMaterialfv(p->Fce, GL_DIFFUSE , (const GLfloat *)&p->Dif);
		if(0.0F < p->Spc.a)	glMaterialfv(p->Fce, GL_SPECULAR, (const GLfloat *)&p->Spc);
		if(0.0F < p->Spc.a)	glMaterialfv(p->Fce, GL_EMISSION, (const GLfloat *)&p->Ems);
		if(0.0F !=p->Shp)	glMaterialf (p->Fce, GL_SHININESS,                  p->Shp);
	}

	else if(LC_RS_EAGL_RESIZE ==ns)
	{
		if(!m_pEgCtx)
			return LC_EFAIL;

#if defined(__APPLE__) || defined(__IPHONE__)
		// create context, frame buffer, color buffer and get the screen width and height
		INT ScnW=0, ScnH=0;
		hr = XcodeEglResize(&ScnW, &ScnH, v, m_pEgCtx, m_fbFrame, m_fbColor);

		//LcVar_WindowSize(&ScnW, &ScnH, LC_TRUE);	// setting Screen Width and Height

		glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
		this->Present();
#endif
	}

	else if(LC_RS_LINE_WIDTH ==ns)
	{
		FLOAT	width = *((FLOAT*)v);
		glLineWidth(width);
	}

	else if(LC_RS_VIEWPORT ==ns)
	{
		LC_RECT* rc =(LC_RECT*)v;
		glViewport(rc->x, rc->y, rc->w, rc->h);
	}

	else
		return LC_EFAIL;

	return hr;
}


INT CLcDevice::SetStatusEnable(INT nStatus)
{
	glEnable(nStatus);
	return LC_OK;
}


INT CLcDevice::SetStatusDisable(INT nStatus)
{
	glDisable(nStatus);
	return LC_OK;
}




// Rendering Primitive for Vertex Buffer Object
INT CLcDevice::SetStreamSource(PLC_VB pVB)
{
#if defined(_PC_LINUX_)
	PFNGLBINDBUFFERARBPROC glBindBuffer =
	(PFNGLBINDBUFFERARBPROC) glXGetProcAddressARB((const GLubyte *) "glBindBufferARB");

	if(NULL == glBindBuffer)
	{
		LOGE("not supported\n");
		return -1;
	}
#endif

	if(NULL == pVB)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return LC_OK;
	}

	return pVB->Bind(LC_TRUE);
}

INT CLcDevice::SetIndices(PLC_IB pIB)
{
#if defined(_PC_LINUX_)
	PFNGLBINDBUFFERARBPROC glBindBuffer =
	(PFNGLBINDBUFFERARBPROC) glXGetProcAddressARB((const GLubyte *) "glBindBufferARB");

	if(NULL == glBindBuffer)
	{
		LOGE("not supported\n");
		return -1;
	}
#endif


	if(NULL == pIB)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		CLcDevice::m_iFVF	= LC_FMT_INDEX16;
		return LC_OK;
	}

	CLcDevice::m_iFVF	= pIB->GetFmt();

	return pIB->Bind(LC_TRUE);
}


INT CLcDevice::DrawPrimitive()
{
	return LC_OK;
}


INT CLcDevice::DrawIndexedPrimitive(INT nMode, INT nCount)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, LC_FMT_FLOAT, 0, 0);

	glDrawElements(nMode, nCount, CLcDevice::m_iFVF, 0);

	return LC_OK;
}





INT CLcDevice::DrawPrimitiveUP(INT nMode, INT First, INT nVtx, const void* pVtx, INT nStride)
{
	char*	p = (char*)pVtx;
	INT		dFVF = CLcDevice::m_dFVF;

	UINT	nTex0 = 0;
	UINT	nTex1 = 0;

	if(!pVtx)
		return LC_EFAIL;

	if(!(LC_FVF_XYZ & dFVF))	// No position is failed. must have positions.
		return LC_EFAIL;


	if(CLcDevice::m_pTex[0])
		nTex0 = CLcDevice::m_pTex[0]->GetId();

	if(CLcDevice::m_pTex[1])
		nTex1 = CLcDevice::m_pTex[1]->GetId();


	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer  (3, LC_FMT_FLOAT,	nStride, p);
		p += sizeof(LC_FLOAT3);
	}


	if( LC_FVF_NORMAL & CLcDevice::m_dFVF)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer  (   LC_FMT_FLOAT,	nStride, p);
		p += sizeof(LC_FLOAT3);
	}

	if( LC_FVF_DIFFUSE & dFVF)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer   (4, LC_FMT_UBYTE,	nStride, p);
		p += sizeof(LC_COLORB);
	}


	if( LC_FVF_TEX1 & dFVF)
	{
		if(nTex0)
		{
			glActiveTexture(GL_TEXTURE0);
			glClientActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture (GL_TEXTURE_2D, nTex0);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, LC_FMT_FLOAT,	nStride, p);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, CLcDevice::m_TssAdS[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, CLcDevice::m_TssAdT[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, CLcDevice::m_TssMin[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, CLcDevice::m_TssMag[0]);

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, CLcDevice::m_TssOp[0]);

		}

		if(nTex0 && nTex1)
		{
			glActiveTexture(GL_TEXTURE1);
			glClientActiveTexture(GL_TEXTURE1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture (GL_TEXTURE_2D, nTex1);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, LC_FMT_FLOAT,	nStride, p);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, CLcDevice::m_TssAdS[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, CLcDevice::m_TssAdT[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, CLcDevice::m_TssMin[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, CLcDevice::m_TssMag[1]);

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, CLcDevice::m_TssOp[1]);
		}
	}



	glDrawArrays(nMode, First, nVtx);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return LC_OK;
}




INT CLcDevice::DrawIndexedPrimitiveUP(INT nMode, INT nIdx, const void *pIdx, const void* pVtx, INT nStride)
{
	char*	p = (char*)pVtx;
	INT		dFVF = CLcDevice::m_dFVF;

	UINT	nTex0 = 0;
	UINT	nTex1 = 0;

	if(!pVtx)
		return LC_EFAIL;

	if(!(LC_FVF_XYZ & dFVF))	// No position is failed. must have positions.
		return LC_EFAIL;


	if(CLcDevice::m_pTex[0])
		nTex0 = CLcDevice::m_pTex[0]->GetId();

	if(CLcDevice::m_pTex[1])
		nTex1 = CLcDevice::m_pTex[1]->GetId();


	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer  (3, LC_FMT_FLOAT,	nStride, p);
		p += sizeof(LC_FLOAT3);
	}

	if( LC_FVF_NORMAL & dFVF)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer  (   LC_FMT_FLOAT,	nStride, p);
		p += sizeof(LC_FLOAT3);
	}

	if( LC_FVF_DIFFUSE & dFVF)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer   (4, LC_FMT_UBYTE, nStride, p);
		p += sizeof(LC_COLORB);
	}


	if( LC_FVF_TEX1 & dFVF)
	{
		if(nTex0)
		{
			glActiveTexture(GL_TEXTURE0);
			glClientActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture (GL_TEXTURE_2D, nTex0);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, LC_FMT_FLOAT,	nStride, p);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, CLcDevice::m_TssAdS[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, CLcDevice::m_TssAdT[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, CLcDevice::m_TssMin[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, CLcDevice::m_TssMag[0]);

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, CLcDevice::m_TssOp[0]);

		}

		if(nTex0 && nTex1)
		{
			glActiveTexture(GL_TEXTURE1);
			glClientActiveTexture(GL_TEXTURE1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture (GL_TEXTURE_2D, nTex1);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, LC_FMT_FLOAT,	nStride, p);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, CLcDevice::m_TssAdS[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, CLcDevice::m_TssAdT[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, CLcDevice::m_TssMin[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, CLcDevice::m_TssMag[1]);

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, CLcDevice::m_TssOp[1]);
		}
	}


	glDrawElements(nMode, nIdx, LC_FMT_INDEX16, pIdx);


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	return LC_OK;
}


INT CLcDevice::Clear(UINT dClearMode)
{
	glClear(dClearMode);
	return LC_OK;
}

INT CLcDevice::Present(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
#if defined(_PC_LINUX_)
	// read the window handle
	LC_HWND hWnd = (LC_HWND)LcVar_ReadP(LC_VAR_WINDOW);

	glXSwapBuffers((Display*)m_pEgDsp, (Window)hWnd);
	glFlush();

#elif defined(__APPLE__) || defined(__IPHONE__)
	XcodeEglPresent(m_pEgCtx);

#elif !defined(__ANDROID__)
	eglSwapBuffers(m_pEgDsp, m_pEgSrf);
	glFlush();

#endif

	return LC_OK;
}



// Draw Primitive.
// Position, Normal, and Texture Coordiante are Float. Color is unsigned byte.
//
INT LcDev_DrawPrimitiveUP(INT dFVF				// LC_FVF
					 , INT nMode				// Primitive Type
					 , INT First				// Start Vertex Index
					 , INT nVertexCount			// Vertex Number
					 , const void* pVtx			// Vertex Stream
					 , INT dStride				// sizeof One Vertex
					 );

// Draw Indexed Primitive.
// Position, Normal, and Texture Coordiante are Float. Color is unsigned byte.
// Index buffer type is unsigned short.
//
INT LcDev_DrawIndexedPrimitiveUP(INT dFVF		// LC_FVF
							, INT nMode			// Primitive Type
							, INT nIndexCount	// Number of Index
							, const void *pIdx	// Index data Stream
							, const void* pVtx	// Vertex data Stream
							, INT dStride		// sizeof One Vertex
							);




INT LcDev_DrawPrimitiveUP(INT dFVF, INT nMode, INT First, INT nVtx, const void* pVtx, INT nStride)
{
	char* p = (char*)pVtx;

	if(!pVtx)
		return LC_EFAIL;

	if(!(LC_FVF_XYZ & dFVF))	// No position is failed. must have positions.
		return LC_EFAIL;


	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


							  { glEnableClientState(GL_VERTEX_ARRAY);		glVertexPointer  (3, LC_FMT_FLOAT,	nStride, p); p += sizeof(LC_FLOAT3);	}
	if( LC_FVF_NORMAL & dFVF) {	glEnableClientState(GL_NORMAL_ARRAY);		glNormalPointer  (   LC_FMT_FLOAT,	nStride, p); p += sizeof(LC_FLOAT3);	}
	if( LC_FVF_DIFFUSE & dFVF){	glEnableClientState(GL_COLOR_ARRAY);		glColorPointer   (4, LC_FMT_UBYTE,	nStride, p); p += sizeof(LC_COLORB);	}
	if( LC_FVF_TEX1 & dFVF)	  {	glEnableClientState(GL_TEXTURE_COORD_ARRAY);glTexCoordPointer(2, LC_FMT_FLOAT,	nStride, p); p += sizeof(LC_FLOAT2);	}

	glDrawArrays(nMode, First, nVtx);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return LC_OK;
}



INT LcDev_DrawIndexedPrimitiveUP(INT dFVF, INT nMode, INT nIdx, const void *pIdx, const void* pVtx, INT nStride)
{
	char* p = (char*)pVtx;

	if(!pVtx)
		return LC_EFAIL;

	if(!(LC_FVF_XYZ & dFVF))	// No position is failed. must have positions.
		return LC_EFAIL;


	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


							  { glEnableClientState(GL_VERTEX_ARRAY);		glVertexPointer  (3, LC_FMT_FLOAT, nStride, p); p += sizeof(LC_FLOAT3);	}
	if( LC_FVF_NORMAL & dFVF) {	glEnableClientState(GL_NORMAL_ARRAY);		glNormalPointer  (   LC_FMT_FLOAT, nStride, p); p += sizeof(LC_FLOAT3);	}
	if( LC_FVF_DIFFUSE & dFVF){	glEnableClientState(GL_COLOR_ARRAY);		glColorPointer   (4, LC_FMT_UBYTE, nStride, p); p += sizeof(LC_COLORB);	}
	if( LC_FVF_TEX1 & dFVF)	  {	glEnableClientState(GL_TEXTURE_COORD_ARRAY);glTexCoordPointer(2, LC_FMT_FLOAT, nStride, p); p += sizeof(LC_FLOAT2);	}


	glDrawElements(nMode, nIdx, LC_FMT_INDEX16, pIdx);


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	return LC_OK;
}



// Create Device
INT LcDev_CreateDevice(char* sCmd
				   , PLC_DEVICE* pOut		// Output instance
				   , LC_HANDLE p1			// Window Handle
				   , LC_HANDLE p2			// Option
				   , LC_HANDLE p3			// No Use
				   , LC_HANDLE p4			// No Use
					 )
{
	INT			hr = 0;
	CLcDevice*	pObj = NULL;

	*pOut		= NULL;

	pObj = new CLcDevice;


#if defined(_PC_LINUX_)
	hr = pObj->CreateForLinux  (p1, p2);

#elif defined(__ANDROID__)
	hr = pObj->CreateForAndroid(p1, p2);

#elif defined(__APPLE__) || defined(__IPHONE__)
	hr = pObj->CreateForIphone (p1, p2);

#elif defined(__BADA__)
	hr = pObj->CreateForBada   (p1, p2);

#else
	hr = pObj->CreateForWin    (p1, p2);

#endif

	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}

	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}


