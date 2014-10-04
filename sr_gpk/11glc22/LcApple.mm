// Implementation of the Apple System.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__APPLE__) || defined(__IPHONE__)

  #include <dirent.h>
  #include <fcntl.h>
  #include <signal.h>
  #include <unistd.h>
  #include <sys/stat.h>

  #include <sys/time.h>
  #include <sys/types.h>

  #import <QuartzCore/QuartzCore.h>
  #import <OpenGLES/EAGL.h>
  #import <OpenGLES/ES1/gl.h>
  #import <OpenGLES/ES1/glext.h>
#endif

#include <LcType.h>
#include <LcMath.h>

#include "LcAux.h"
#include "LcBase.h"
#include "LcDevice.h"


namespace LCSYS
{
	extern char g_DirApp[LC_MAX_PATH2];
	extern char g_DirRsc[LC_MAX_PATH2];
}


#if defined(__APPLE__) || defined(__IPHONE__)


const char* LcSys_AppFolder(char* apkName)
{
	if(0 == LCSYS::g_DirApp[0] && 0 == LCSYS::g_DirApp[1] &&
	   0 == LCSYS::g_DirApp[2] && 0 == LCSYS::g_DirApp[3])
	{
		NSArray* pPath= NSSearchPathForDirectoriesInDomains(NSDocumentDirectory
							, NSUserDomainMask, YES);	// creates pPath so that you can pull the app's path from it

		NSString*   t = [pPath objectAtIndex:0];		// gets the applications directory on the users iPhone
		const char* s = [t UTF8String ];				// appends scoreFileName to the end of documentsDirectory.

		strcpy(LCSYS::g_DirApp, s);
	}

	return LCSYS::g_DirApp;
}
#endif


const char* LcSys_IphoneRscPath()
{
#if defined(__APPLE__) || defined(__IPHONE__)
	if(0 == LCSYS::g_DirRsc[0] && 0 == LCSYS::g_DirRsc[1] &&
	   0 == LCSYS::g_DirRsc[2] && 0 == LCSYS::g_DirRsc[3])
	{
		NSString  * t = [[NSBundle mainBundle] resourcePath];
		const char* s = [t UTF8String ];
		strcpy(LCSYS::g_DirRsc, s);
	}
#endif

	return LCSYS::g_DirRsc;
}



#if defined(__APPLE__) || defined(__IPHONE__)

INT LcSys_FindResource(char* sOut, const char* sPath)
{
	char File[LC_MAX_PATH2]={0};
	char tExt[16]={0};
	char Ext[16]={0};

	LcStr_SplitPath(sPath, NULL, NULL, File, tExt);

	char* p  = ::strchr(tExt, '.' );
	if(p)
		strcpy(Ext, p+1);


	NSString* e = [NSString stringWithUTF8String: Ext];
	NSString* f = [NSString stringWithUTF8String: File];
	NSString* t = nil;

	if(nil == e || nil == f)
		return LC_EFAIL;


	t = [[NSBundle mainBundle] pathForResource:f ofType: e];
	if(nil == t)
		return LC_EFAIL;

	const char* s = [t UTF8String ];
	strcpy(sOut, s);

	return LC_OK;
}

#endif




// EGL context, framebuffer, color buffer for iphone.
//
////////////////////////////////////////////////////////////////////////////////

void XcodeEglDestroy(LC_HANDLE* pEgCtx, UINT* pFrameBuffer, UINT* pColorBuffer)
{
#if defined(__APPLE__) || defined(__IPHONE__)
	EAGLContext* pCtx = (EAGLContext*)(*pEgCtx);

	if(*pFrameBuffer)
		glDeleteFramebuffersOES(1, pFrameBuffer);


	if(*pColorBuffer)
		glDeleteRenderbuffersOES(1, pColorBuffer);

	if(pCtx)
	{
		if ([EAGLContext currentContext] == pCtx)
			[EAGLContext setCurrentContext:nil];

		[pCtx release];
	}

	*pColorBuffer = 0;
	*pFrameBuffer = 0;
	*pEgCtx = NULL;

#endif

}



INT XcodeEglCreate(LC_HANDLE* pEgCtx, UINT* pFrameBuffer, UINT* pColorBuffer)
{
#if defined(__APPLE__) || defined(__IPHONE__)

	EAGLContext* pCtx = NULL;
	pCtx = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];

	if (!pCtx || ![EAGLContext setCurrentContext:pCtx])
		return -1;

	// Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
	glGenFramebuffersOES(1,  pFrameBuffer);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, *pFrameBuffer);

	// Create color render buffer and allocate backing store
	glGenRenderbuffersOES(1, pColorBuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, *pColorBuffer);

	*pEgCtx = pCtx;

#endif

	return 0;
}


INT XcodeEglResize(INT* ScnW, INT* ScnH, LC_HANDLE pLayer, LC_HANDLE pEgCtx, UINT FrameBuffer, UINT ColorBuffer)
{
#if defined(__APPLE__) || defined(__IPHONE__)
	INT w, h;
	EAGLContext* pCtx  = (EAGLContext*)pEgCtx;
	CAEAGLLayer* layer = (CAEAGLLayer*)pLayer;

	[pCtx renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES , &w);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &h);

	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, ColorBuffer);

	if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
		return -1;

	if(ScnW) *ScnW= w;
	if(ScnH) *ScnH= h;

#endif

	return 0;
}


void XcodeEglSetFrameBuffer(LC_HANDLE pEgCtx, UINT FrameBuffer)
{
#if defined(__APPLE__) || defined(__IPHONE__)
	EAGLContext* pCtx  = (EAGLContext*)pEgCtx;
	[EAGLContext setCurrentContext:pCtx];
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, FrameBuffer);
#endif
}



void XcodeEglPresent(LC_HANDLE pEgCtx)
{
#if defined(__APPLE__) || defined(__IPHONE__)
	EAGLContext* pCtx  = (EAGLContext*)pEgCtx;
	if(pCtx)
		[pCtx presentRenderbuffer:GL_RENDERBUFFER_OES];

#endif

}



void* glc_fopen(const char *filename, const char *mode )
{
	return fopen(filename, mode);
}

