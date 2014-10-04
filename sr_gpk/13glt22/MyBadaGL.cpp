

#include <math.h>

#include <FBase.h>
#include <FBaseSys.h>
#include <FGraphicsOpengl.h>
  
using namespace Osp::Graphics::Opengl;


int LcDev_SetupEGL2(void** pEgDsp, void** pEgSrf, void** pEgCtx, void**  pEgCfg, int ConfigSize, void* pForm)
{

#if defined(__BADA__)

	EGLint match = 1;
	EGLint config_attrib[] =
	{
		EGL_RED_SIZE,		 8					,
		EGL_GREEN_SIZE,		 8					,
		EGL_BLUE_SIZE,		 8					,
		EGL_ALPHA_SIZE,		 8					,
		EGL_DEPTH_SIZE,		 16					,
		EGL_SURFACE_TYPE,	 EGL_WINDOW_BIT		,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT	,
		EGL_NONE		,	 EGL_NONE			,
	};

	EGLint config_context[] =
	{
		EGL_CONTEXT_CLIENT_VERSION,	1,
		EGL_NONE,					EGL_NONE,
	};

	EGLint		ver_maj;
	EGLint		ver_min;
	int			ScnW=0, ScnH=0;
	NativeWindowType hWnd= (NativeWindowType)pForm;


	eglBindAPI(EGL_OPENGL_ES_API);


	*pEgDsp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(NULL == *pEgDsp)
	{
		AppLog("[GlesCube11] eglGetDisplay() is failed. [0x%x]\n", eglGetError());
		return -1;
	}

	if(!eglInitialize(*pEgDsp, &ver_maj, &ver_min))
	{
		AppLog("[GlesCube11] eglInitialize() is failed. [0x%x]\n", eglGetError());
		return -1;
	}
	AppLog("Egl Version: 0x%X, 0x%X\n", ver_maj, ver_min);

	if(!eglChooseConfig(*pEgDsp, config_attrib, &pEgCfg[0], ConfigSize, &match))
	{
		AppLog("[GlesCube11] eglChooseConfig() is failed. [0x%x]\n", eglGetError());
		return -1;
	}

	if(1>match)
	{
		AppLog("[GlesCube11] eglChooseConfig() has been failed. because of matching config doesn't exist \n");
		return -1;
	}

	*pEgSrf = eglCreateWindowSurface(*pEgDsp, pEgCfg[0], hWnd, NULL);
	if(NULL == *pEgSrf)
	{
		AppLog("[GlesCube11] eglCreateWindowSurface() has been failed. NULL [0x%x]\n", eglGetError());
		return -1;
	}

	*pEgCtx = eglCreateContext(*pEgDsp, pEgCfg[0], NULL, (const EGLint*)config_context);
	if(NULL == *pEgCtx)
	{
		AppLog("[GlesCube11] eglCreateContext() has been failed. [0x%x]\n", eglGetError());
		return -1;
	}

	if(!eglMakeCurrent(*pEgDsp, *pEgSrf, *pEgSrf, *pEgCtx))
	{
		AppLog("[GlesCube11] eglMakeCurrent() has been failed. [0x%x]\n", eglGetError());
		return -1;
	}


	eglQuerySurface(*pEgDsp, *pEgSrf, EGL_WIDTH,  &ScnW);		// get the screen width
	eglQuerySurface(*pEgDsp, *pEgSrf, EGL_HEIGHT, &ScnH);		// get the screen height
	glViewport(0, 0, ScnW, ScnH);								// Setup the viewport


	glShadeModel(GL_SMOOTH);									// select the smooth shading model.
	glDisable(GL_DEPTH_TEST);									// disable the depth test
	glDisable(GL_CULL_FACE);									// disable the culling face


	// for Test...
	glClearColor(0, 0.4F, 0.8F, 1.0F);
	glShadeModel(GL_SMOOTH);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	eglSwapBuffers(*pEgDsp, *pEgSrf);
	glFlush();

	AppLog("Complete EGL Create\n");

#endif

	return 0;
}


/*
int LcDev_SetupEGL(EGLDisplay* pEgDsp, EGLSurface* pEgSrf, EGLContext* pEgCtx, EGLConfig*  pEgCfg, int ConfigSize, void* pForm)
{
	EGLint match = 1;
	EGLint config_attrib[] =
	{
		EGL_RED_SIZE,		 8					,
		EGL_GREEN_SIZE,		 8					,
		EGL_BLUE_SIZE,		 8					,
		EGL_ALPHA_SIZE,		 8					,
		EGL_DEPTH_SIZE,		 16					,
		EGL_SURFACE_TYPE,	 EGL_WINDOW_BIT		,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT	,
		EGL_NONE		,	 EGL_NONE			,
	};

	EGLint config_context[] =
	{
		EGL_CONTEXT_CLIENT_VERSION,	1,
		EGL_NONE,					EGL_NONE,
	};

	EGLint		ver_maj;
	EGLint		ver_min;
	int			ScnW=0, ScnH=0;
	NativeWindowType hWnd= (NativeWindowType)pForm;


	eglBindAPI(EGL_OPENGL_ES_API);


	*pEgDsp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(NULL == *pEgDsp)
	{
		AppLog("[GlesCube11] eglGetDisplay() is failed. [0x%x]\n", eglGetError());
		return -1;
	}

	if(!eglInitialize(*pEgDsp, &ver_maj, &ver_min))
	{
		AppLog("[GlesCube11] eglInitialize() is failed. [0x%x]\n", eglGetError());
		return -1;
	}
	AppLog("Egl Version: 0x%X, 0x%X\n", ver_maj, ver_min);

	if(!eglChooseConfig(*pEgDsp, config_attrib, &pEgCfg[0], ConfigSize, &match))
	{
		AppLog("[GlesCube11] eglChooseConfig() is failed. [0x%x]\n", eglGetError());
		return -1;
	}

	if(1>match)
	{
		AppLog("[GlesCube11] eglChooseConfig() has been failed. because of matching config doesn't exist \n");
		return -1;
	}

	*pEgSrf = eglCreateWindowSurface(*pEgDsp, pEgCfg[0], hWnd, NULL);
	if(NULL == *pEgSrf)
	{
		AppLog("[GlesCube11] eglCreateWindowSurface() has been failed. NULL [0x%x]\n", eglGetError());
		return -1;
	}

	*pEgCtx = eglCreateContext(*pEgDsp, pEgCfg[0], NULL, (const EGLint*)config_context);
	if(NULL == *pEgCtx)
	{
		AppLog("[GlesCube11] eglCreateContext() has been failed. [0x%x]\n", eglGetError());
		return -1;
	}

	if(!eglMakeCurrent(*pEgDsp, *pEgSrf, *pEgSrf, *pEgCtx))
	{
		AppLog("[GlesCube11] eglMakeCurrent() has been failed. [0x%x]\n", eglGetError());
		return -1;
	}


	eglQuerySurface(*pEgDsp, *pEgSrf, EGL_WIDTH,  &ScnW);		// get the screen width
	eglQuerySurface(*pEgDsp, *pEgSrf, EGL_HEIGHT, &ScnH);		// get the screen height
	glViewport(0, 0, ScnW, ScnH);								// Setup the viewport


	glShadeModel(GL_SMOOTH);									// select the smooth shading model.
	glDisable(GL_DEPTH_TEST);									// disable the depth test
	glDisable(GL_CULL_FACE);									// disable the culling face


	// for Test...
	glClearColor(0, 0.4F, 0.8F, 1.0F);
	glShadeModel(GL_SMOOTH);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	eglSwapBuffers(*pEgDsp, *pEgSrf);
	glFlush();

	AppLog("Complete EGL Create\n");

	return 0;
}
*/

