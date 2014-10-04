// Implementation of the ILC_Device.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#if defined(_MSC_VER)


#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gles/gl.h>
#include <gles/egl.h>

#include <LcType.h>
#include <LcMath.h>

#include "LcAux.h"


//GL_APICALL void      glClearDepthf (GLclampf depth);
//GL_APICALL void      glClipPlanef (GLenum plane, const GLfloat* equation);
//GL_APICALL void      glDepthRangef (GLclampf nearValue, GLclampf farValue);
//GL_APICALL void      glFrustumf (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near_val, GLfloat far_val);
//GL_APICALL void      glGetClipPlanef (GLenum plane, GLfloat* equation);

//GL_APICALL void      glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
//GL_APICALL void      glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near_val, GLfloat far_val);
//GL_APICALL void      glPointParameterf (GLenum pname, GLfloat params);
//GL_APICALL void      glPointParameterfv (GLenum pname, const GLfloat* params);
//GL_APICALL void      glActiveTexture (GLenum texture);

//GL_APICALL void      glBindBuffer (GLenum target, GLuint buffer);
//GL_APICALL void      glBufferData (GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
//GL_APICALL void      glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
//GL_APICALL void      glClientActiveTexture(GLenum texture);
//GL_APICALL void      glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data);

//GL_APICALL void      glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data);
//GL_APICALL void      glDeleteBuffers (GLsizei n, const GLuint* buffers);
//GL_APICALL void      glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params);
//GL_APICALL void      glGenBuffers (GLsizei n, GLuint* buffers);
//GL_APICALL GLboolean glIsBuffer (GLuint buffer);

//GL_APICALL void      glSampleCoverage (GLclampf value, GLboolean invert);
//GL_APICALL void      glCurrentPaletteMatrixOES(GLuint matrix);
//GL_APICALL void      glMatrixIndexPointerOES(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
//GL_APICALL void      glWeightPointerOES   (GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
//GL_APICALL void      glPointSizePointerOES(GLenum type, GLsizei stride, const GLvoid* pointer);
//GL_APICALL GLbitfield glQueryMatrixxOES       (GLfixed mantissa[16], GLint exponent[16]);								// ES Àü¿ë


#ifndef PENTRY
#define PENTRY APIENTRY *
#endif

typedef void (PENTRY PF_ACTIVETEXTURE			) (GLenum texture);
typedef void (PENTRY PF_CLIENTACTIVETEXTURE		) (GLenum texture);
typedef void (PENTRY PF_COMPRESSEDTEXIMAGE2D	) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (PENTRY PF_COMPTEXSUBIMG2D			) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (PENTRY PF_MULTITEXCOORD4F			) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (PENTRY PF_SAMPLECOVERAGE			) (GLclampf value, GLboolean invert);
typedef void (PENTRY PF_BINDBUFFER				) (GLenum target, GLuint buffer);
typedef void (PENTRY PF_BUFFERDATA				) (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
typedef void (PENTRY PF_CURRENTPALETTEMATRIX	) (GLint index);
typedef void (PENTRY PF_DELETEBUFFERS			) (GLsizei n, const GLuint *buffers);

typedef void (PENTRY PF_BUFFERSUBDATA			) (GLenum target, GLsizeiptr offset, GLsizeiptr size, const GLvoid *data);
typedef void (PENTRY PF_GENBUFFERS				) (GLsizei n, GLuint *buffers);
typedef void (PENTRY PF_GETBUFFERPARAMETERIV	) (GLenum target, GLenum pname, GLint *params);
typedef void (PENTRY PF_MATRIXINDEXPOINTER		) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (PENTRY PF_WEIGHTPOINTER			) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (PENTRY PF_POINTPARAMETERFV		) (GLenum pname, const GLfloat *params);
typedef void (PENTRY PF_POINTPARAMETERF			) (GLenum pname, GLfloat param);
typedef GLboolean (PENTRY PF_ISBUFFER			) (GLuint buffer);


typedef int (WINAPI * PF_WGLSWAPINTERVAL		) (int interval);


static PF_ACTIVETEXTURE			pArbActiveTexture			= NULL;
static PF_CLIENTACTIVETEXTURE	pArbClientActiveTexture		= NULL;
static PF_COMPRESSEDTEXIMAGE2D	pArbCompressedTexImage2D	= NULL;
static PF_COMPTEXSUBIMG2D		pArbCompTexSubImg2D			= NULL;
static PF_MULTITEXCOORD4F		pArbMultiTexCoord4f			= NULL;
static PF_SAMPLECOVERAGE		pArbSampleCoverage			= NULL;
static PF_BINDBUFFER			pArbBindBuffer				= NULL;
static PF_BUFFERDATA			pArbBufferData				= NULL;
static PF_CURRENTPALETTEMATRIX	pArbCurrentPaletteMatrix	= NULL;
static PF_DELETEBUFFERS			pArbDeleteBuffers			= NULL;

static PF_BUFFERSUBDATA			pArbBufferSubData			= NULL;
static PF_GENBUFFERS			pArbGenBuffers				= NULL;
static PF_GETBUFFERPARAMETERIV	pArbGetBufferParameteriv	= NULL;
static PF_MATRIXINDEXPOINTER	pArbMatrixIndexPointer		= NULL;
static PF_WEIGHTPOINTER			pArbWeightPointer			= NULL;
static PF_POINTPARAMETERFV		pArbPointParameterfv		= NULL;
static PF_POINTPARAMETERF		pArbPointParameterf			= NULL;
static PF_ISBUFFER				pArbIsBuffer				= NULL;

static PF_WGLSWAPINTERVAL		pWglSwapInterval			= NULL;

static int g_GLverMajor=0;
static int g_GLverMinor=0;


INT LcDev_GLInit()
{
	L_PTR r = 1;

	char* s= (char*)glGetString(GL_VERSION);
	char* m = NULL;
	char* n = NULL;

	int major=0, minor=0;

	m = strchr(s, '.');
	n = strchr(m+1, '.');

	g_GLverMajor = s[m-s-1] -'0';
	g_GLverMinor = s[n-s-1] -'0';

	LOGI("OpenGL version: %d.%d\n", g_GLverMajor, g_GLverMinor);

	;	if(!r) LOGE("Err: " "glActiveTextureARB"            " Failed.\n");   
	;	if(!r) LOGE("Err: " "glAttachShader"                " Failed.\n");   
	;	if(!r) LOGE("Err: " "glBindAttribLocation"          " Failed.\n");   
	;	if(!r) LOGE("Err: " "glBindBuffer"                  " Failed.\n");   
	;	if(!r) LOGE("Err: " "glBindFramebuffer"             " Failed.\n");   
	;	if(!r) LOGE("Err: " "glBindRenderbuffer"            " Failed.\n");   
	;	if(!r) LOGE("Err: " "glBlendColor"                  " Failed.\n");   
	;	if(!r) LOGE("Err: " "glBlendEquation"               " Failed.\n");   
	;	if(!r) LOGE("Err: " "glBlendEquationSeparate"       " Failed.\n");   
	;	if(!r) LOGE("Err: " "glBlendFuncSeparate"           " Failed.\n");   
	;	if(!r) LOGE("Err: " "glBufferData"                  " Failed.\n");   
	;	if(!r) LOGE("Err: " "glBufferSubData"               " Failed.\n");   
	;	if(!r) LOGE("Err: " "glCheckFramebufferStatus"      " Failed.\n");   
	;	if(!r) LOGE("Err: " "glCompileShader"               " Failed.\n");   
	;	if(!r) LOGE("Err: " "glCompressedTexImage2D"        " Failed.\n");   
	;	if(!r) LOGE("Err: " "glCompressedTexSubImage2D"     " Failed.\n");   
	;	if(!r) LOGE("Err: " "glCreateProgram"               " Failed.\n");   
	;	if(!r) LOGE("Err: " "glCreateShader"                " Failed.\n");   
	;	if(!r) LOGE("Err: " "glDeleteBuffers"               " Failed.\n");   
	;	if(!r) LOGE("Err: " "glDeleteFramebuffers"          " Failed.\n");   
	;	if(!r) LOGE("Err: " "glDeleteProgram"               " Failed.\n");   
	;	if(!r) LOGE("Err: " "glDeleteRenderbuffers"         " Failed.\n");   
	;	if(!r) LOGE("Err: " "glDeleteShader"                " Failed.\n");   
	;	if(!r) LOGE("Err: " "glDetachShader"                " Failed.\n");   
	;	if(!r) LOGE("Err: " "glDisableVertexAttribArray"    " Failed.\n");   
	;	if(!r) LOGE("Err: " "glEnableVertexAttribArray"     " Failed.\n");   
	;	if(!r) LOGE("Err: " "glFramebufferRenderbuffer"     " Failed.\n");   
	;	if(!r) LOGE("Err: " "glFramebufferTexture2D"        " Failed.\n");   
	;	if(!r) LOGE("Err: " "glGenBuffers"                  " Failed.\n");   
	;	if(!r) LOGE("Err: " "glGenerateMipmap"              " Failed.\n");   
	;	if(!r) LOGE("Err: " "glGenFramebuffers"             " Failed.\n");   
	;	if(!r) LOGE("Err: " "glGenRenderbuffers"            " Failed.\n");   
	;	if(!r) LOGE("Err: " "glGetActiveAttrib"             " Failed.\n");   
	;	if(!r) LOGE("Err: " "glGetActiveUniform"            " Failed.\n");   

	r = r && (L_PTR)(pArbActiveTexture       = (PF_ACTIVETEXTURE        )wglGetProcAddress("glActiveTextureARB"        ) );	if(!pArbActiveTexture       ) LOGE("Err: " "glActiveTextureARB"            " Failed.\n");
	r = r && (L_PTR)(pArbClientActiveTexture = (PF_CLIENTACTIVETEXTURE  )wglGetProcAddress("glClientActiveTextureARB"  ) );	if(!pArbClientActiveTexture ) LOGE("Err: " "glClientActiveTextureARB"      " Failed.\n");
	r = r && (L_PTR)(pArbCompressedTexImage2D= (PF_COMPRESSEDTEXIMAGE2D )wglGetProcAddress("glCompressedTexImage2DARB" ) );	if(!pArbCompressedTexImage2D) LOGE("Err: " "glCompressedTexImage2DARB"     " Failed.\n");
	                (pArbCompTexSubImg2D     = (PF_COMPTEXSUBIMG2D      )wglGetProcAddress("glCompressedTexSubImage2D" ) );	if(!pArbCompTexSubImg2D     ) LOGE("Err: " "glCompressedTexSubImage2D"     " Failed.\n");
	                (pArbMultiTexCoord4f     = (PF_MULTITEXCOORD4F      )wglGetProcAddress("glMultiTexCoord4fARB"      ) );	if(!pArbMultiTexCoord4f     ) LOGE("Err: " "glMultiTexCoord4fARB"          " Failed.\n");
	                (pArbSampleCoverage      = (PF_SAMPLECOVERAGE       )wglGetProcAddress("glSampleCoverageARB"       ) );	if(!pArbSampleCoverage      ) LOGE("Err: " "glSampleCoverageARB"           " Failed.\n");
	r = r && (L_PTR)(pArbBindBuffer          = (PF_BINDBUFFER           )wglGetProcAddress("glBindBufferARB"           ) );	if(!pArbBindBuffer          ) LOGE("Err: " "glBindBufferARB"               " Failed.\n");
	                (pArbBufferData          = (PF_BUFFERDATA           )wglGetProcAddress("glBufferDataARB"           ) );	if(!pArbBufferData          ) LOGE("Err: " "glBufferDataARB"               " Failed.\n");
	                (pArbCurrentPaletteMatrix= (PF_CURRENTPALETTEMATRIX )wglGetProcAddress("glCurrentPaletteMatrixARB" ) );	if(!pArbCurrentPaletteMatrix) LOGE("Err: " "glCurrentPaletteMatrixARB"     " Failed.\n");
	                (pArbDeleteBuffers       = (PF_DELETEBUFFERS        )wglGetProcAddress("glDeleteBuffersARB"        ) );	if(!pArbDeleteBuffers       ) LOGE("Err: " "glDeleteBuffersARB"            " Failed.\n");
	                (pArbBufferSubData       = (PF_BUFFERSUBDATA        )wglGetProcAddress("glBufferSubDataARB"        ) );	if(!pArbBufferSubData       ) LOGE("Err: " "glBufferSubDataARB"            " Failed.\n");
	r = r && (L_PTR)(pArbGenBuffers          = (PF_GENBUFFERS           )wglGetProcAddress("glGenBuffersARB"           ) );	if(!pArbGenBuffers          ) LOGE("Err: " "glGenBuffersARB"               " Failed.\n");
	                (pArbGetBufferParameteriv= (PF_GETBUFFERPARAMETERIV )wglGetProcAddress("glGetBufferParameterivARB" ) );	if(!pArbGetBufferParameteriv) LOGE("Err: " "glGetBufferParameterivARB"     " Failed.\n");
	                (pArbMatrixIndexPointer  = (PF_MATRIXINDEXPOINTER   )wglGetProcAddress("glMatrixIndexPointerARB"   ) );	if(!pArbMatrixIndexPointer  ) LOGE("Err: " "glMatrixIndexPointerARB"       " Failed.\n");
	                (pArbWeightPointer       = (PF_WEIGHTPOINTER        )wglGetProcAddress("glWeightPointerARB"        ) );	if(!pArbWeightPointer       ) LOGE("Err: " "glWeightPointerARB"            " Failed.\n");
	                (pArbPointParameterfv    = (PF_POINTPARAMETERFV     )wglGetProcAddress("glPointParameterfvARB"     ) );	if(!pArbPointParameterfv    ) LOGE("Err: " "glPointParameterfvARB"         " Failed.\n");
	                (pArbPointParameterf     = (PF_POINTPARAMETERF      )wglGetProcAddress("glPointParameterfARB"      ) );	if(!pArbPointParameterf     ) LOGE("Err: " "glPointParameterfARB"          " Failed.\n");
	                (pArbIsBuffer            = (PF_ISBUFFER             )wglGetProcAddress("glIsBufferARB"             ) );	if(!pArbIsBuffer            ) LOGE("Err: " "glIsBufferARB"                 " Failed.\n");

	if(!r)
	{
		LOGE("Err: OpenGL Init Failed. This device cannot be emulating.\n");
		return -1;
	}


	pWglSwapInterval = (PF_WGLSWAPINTERVAL	)wglGetProcAddress("wglSwapIntervalEXT");

	return 0;
}




void glAlphaFuncx(GLenum func, GLclampx ref)
{
	glAlphaFunc(func, LCXFloatFromFixed(ref));
}

void glActiveTexture(GLenum texture)
{
	if(NULL == pArbActiveTexture)
	{
		LOGW("Warning: glActiveTexture. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbActiveTexture(texture);
}

void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
	glClearColor( LCXFloatFromFixed(red), LCXFloatFromFixed(green), LCXFloatFromFixed(blue), LCXFloatFromFixed(alpha) );
}

void glClearDepthf(GLclampf depth)
{
	glClearDepth((GLclampd)depth);
}

void glClearDepthx(GLclampx depth)
{
	glClearDepth((GLclampd)LCXFloatFromFixed(depth));
}

void glClientActiveTexture(GLenum texture)
{
	if(NULL == pArbClientActiveTexture)
	{
		LOGW("Warning: glClientActiveTexture. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbClientActiveTexture(texture);
}

void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
	glColor4f((GLfloat)LCXFloatFromFixed(red)
			, (GLfloat)LCXFloatFromFixed(green)
			, (GLfloat)LCXFloatFromFixed(blue)
			, (GLfloat)LCXFloatFromFixed(alpha)
			);	// Redefine glColor4x
}


void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data)
{
	if(NULL == pArbCompressedTexImage2D)
	{
		LOGW("Warning: glCompressedTexImage2D. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data)
{
	if(NULL == pArbCompTexSubImg2D)
	{
		LOGW("Warning: glCompTexSubImg2D. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbCompTexSubImg2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

void glDepthRangef(GLclampf nearValue, GLclampf farValue)
{
	glDepthRange((GLclampd)nearValue, (GLclampd)farValue);
}

void glDepthRangex(GLclampx nearValue, GLclampx farValue)
{
	glDepthRange((GLclampd)LCXFloatFromFixed(nearValue), (GLclampd)LCXFloatFromFixed(farValue) );
}

void glFogx(GLenum pname, GLfixed param)
{
	glFogf(pname, LCXFloatFromFixed(param) );
}

void glFogxv(GLenum pname, const GLfixed* params)
{
	GLfloat	param[8]={0};

	param[0] = LCXFloatFromFixed(params[0]);
	param[1] = LCXFloatFromFixed(params[1]);
	param[2] = LCXFloatFromFixed(params[2]);
	param[3] = LCXFloatFromFixed(params[3]);

	glFogfv(pname, param );
}

void glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near_val, GLfloat far_val)
{
	glFrustum((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top, (GLdouble)near_val, (GLdouble)far_val);
}

void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed near_val, GLfixed far_val)
{
	glFrustum((GLdouble)LCXFloatFromFixed(left)
			, (GLdouble)LCXFloatFromFixed(right)
			, (GLdouble)LCXFloatFromFixed(bottom)
			, (GLdouble)LCXFloatFromFixed(top)
			, (GLdouble)LCXFloatFromFixed(near_val)
			, (GLdouble)LCXFloatFromFixed(far_val));
}


void glLightx(GLenum light, GLenum pname, GLfixed param)
{
	glLightf(light, pname, LCXFloatFromFixed(param) );
}


void glLightxv(GLenum light, GLenum pname, const GLfixed* params)
{
	GLfloat	param[8]={0};

	param[0] = LCXFloatFromFixed(params[0]);
	param[1] = LCXFloatFromFixed(params[1]);
	param[2] = LCXFloatFromFixed(params[2]);
	param[3] = LCXFloatFromFixed(params[3]);

	glLightfv(light, pname, param);
}


void glLightModelx(GLenum pname, GLfixed param)
{
	glLightModelf(pname, LCXFloatFromFixed(param) );
}


void glLightModelxv(GLenum pname, const GLfixed* params)
{
	GLfloat	param[8]={0};

	param[0] = LCXFloatFromFixed(params[0]);
	param[1] = LCXFloatFromFixed(params[1]);
	param[2] = LCXFloatFromFixed(params[2]);
	param[3] = LCXFloatFromFixed(params[3]);

	glLightModelfv(pname, param);
}


void glLineWidthx(GLfixed width)
{
	glLineWidth(LCXFloatFromFixed(width) );
}


void glLoadMatrixx(const GLfixed* m)
{
	GLfloat	param[16]={0};

	for(int i=0; i<16; ++i)
		param[i] = LCXFloatFromFixed(m[i]);

	glLoadMatrixf(param);
}


void glMaterialx(GLenum face, GLenum pname, GLfixed param)
{
	glMaterialf(face, pname, LCXFloatFromFixed(param) );
}


void glMaterialxv(GLenum face, GLenum pname, const GLfixed* params)
{
	GLfloat	param[8]={0};

	param[0] = LCXFloatFromFixed(params[0]);
	param[1] = LCXFloatFromFixed(params[1]);
	param[2] = LCXFloatFromFixed(params[2]);
	param[3] = LCXFloatFromFixed(params[3]);

	glMaterialfv(face, pname, param);
}


void glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	if(NULL == pArbMultiTexCoord4f)
	{
		LOGW("Warning: glMultiTexCoord4f. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbMultiTexCoord4f(target, s, t, r, q);
}


void glMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
	if(NULL == pArbMultiTexCoord4f)
	{
		LOGW("Warning: glMultiTexCoord4f. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbMultiTexCoord4f(target, LCXFloatFromFixed(s), LCXFloatFromFixed(t), LCXFloatFromFixed(r), LCXFloatFromFixed(q) );
}


void glMultMatrixx(const GLfixed* m)
{
	GLfloat	param[16]={0};

	for(int i=0; i<16; ++i)
		param[i] = LCXFloatFromFixed(m[i]);

	glMultMatrixf(param);
}


void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
	glNormal3f(LCXFloatFromFixed(nx), LCXFloatFromFixed(ny), LCXFloatFromFixed(nz) );
}


void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near_val, GLfloat far_val)
{
	glOrtho((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top, (GLdouble)near_val, (GLdouble)far_val);
}


void glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed near_val, GLfixed far_val)
{
	glOrtho((GLdouble)LCXFloatFromFixed(left), (GLdouble)LCXFloatFromFixed(right), (GLdouble)LCXFloatFromFixed(bottom), (GLdouble)LCXFloatFromFixed(top), (GLdouble)LCXFloatFromFixed(near_val), (GLdouble)LCXFloatFromFixed(far_val) );
}


void glPointSizex(GLfixed size)
{
	glPointSize(LCXFloatFromFixed(size));
}

void glPolygonOffsetx(GLfixed factor, GLfixed units)
{
	glPolygonOffset(LCXFloatFromFixed(factor), LCXFloatFromFixed(units));
}

void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
	glRotatef(LCXFloatFromFixed(angle), LCXFloatFromFixed(x), LCXFloatFromFixed(y), LCXFloatFromFixed(z) );
}


void glSampleCoverage(GLclampf value, GLboolean invert)
{
	if(NULL == pArbSampleCoverage)
	{
		LOGW("Warning: glSampleCoverage. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbSampleCoverage(value, invert);
}

void glSampleCoveragex(GLclampx value, GLboolean invert)
{

	if(NULL == pArbSampleCoverage)
	{
		LOGW("Warning: glSampleCoverage. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbSampleCoverage(LCXFloatFromFixed(value), invert);
}


void glScalex(GLfixed x, GLfixed y, GLfixed z)
{
	glScalef(LCXFloatFromFixed(x), LCXFloatFromFixed(y), LCXFloatFromFixed(z));
}


void glTexEnvx(GLenum target, GLenum pname, GLfixed param)
{
	glTexEnvf(target, pname, LCXFloatFromFixed(param) );
}


void glTexEnvxv(GLenum target, GLenum pname, const GLfixed* params)
{
	GLfloat	param[8]={0};

	param[0] = LCXFloatFromFixed(params[0]);
	param[1] = LCXFloatFromFixed(params[1]);
	param[2] = LCXFloatFromFixed(params[2]);
	param[3] = LCXFloatFromFixed(params[3]);

	glTexEnvfv(target, pname, param);
}

void glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
	glTexParameterf(target, pname, LCXFloatFromFixed(param) );
}


void glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
	glTranslatef(LCXFloatFromFixed(x), LCXFloatFromFixed(y), LCXFloatFromFixed(z) );
}


void glBindBuffer(GLenum target, GLuint buffer)
{
	if(NULL == pArbBindBuffer)
	{
		LOGW("Warning: glBindBuffer. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbBindBuffer(target, buffer);
}


void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
	if(NULL == pArbBufferData)
	{
		LOGW("Warning: glBufferData. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbBufferData(target, size, data, usage);
}


void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
	if(NULL == pArbBufferSubData)
	{
		LOGW("Warning: glBufferSubData. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbBufferSubData(target, offset, size, data);
}


void glCurrentPaletteMatrixOES (GLuint matrix)
{
	if(NULL == pArbCurrentPaletteMatrix)
	{
		LOGW("Warning: glCurrentPaletteMatrix. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbCurrentPaletteMatrix(matrix);
}


void glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
	if(NULL == pArbDeleteBuffers)
	{
		LOGW("Warning: glDeleteBuffers. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbDeleteBuffers(n, buffers);
}


void glGenBuffers(GLsizei n, GLuint* buffers)
{
	if(NULL == pArbGenBuffers)
	{
		LOGW("Warning: glGenBuffers. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbGenBuffers(n, buffers);
}


void glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	if(NULL == pArbGetBufferParameteriv)
	{
		LOGW("Warning: glGetBufferParameteriv. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbGetBufferParameteriv(target, pname, params);
}


void glGetClipPlanef(GLenum plane, GLfloat* equation)
{
	GLdouble	param[8]={0};

	glGetClipPlane(plane, param);

	equation[0] = (GLfloat)param[0];
	equation[1] = (GLfloat)param[1];
	equation[2] = (GLfloat)param[2];
	equation[3] = (GLfloat)param[3];
}


void glGetClipPlanex(GLenum plane, GLfixed* equation)
{
	GLdouble	param[8]={0};

	glGetClipPlane(plane, param);

	equation[0] = LCXFixed(param[0]);
	equation[1] = LCXFixed(param[1]);
	equation[2] = LCXFixed(param[2]);
	equation[3] = LCXFixed(param[3]);
}


void glGetFixedv(GLenum pname, GLfixed* params)
{
	GLfloat	param[8]={0};

	glGetFloatv(pname, param);

	params[0] = LCXFixed(param[0]);
	params[1] = LCXFixed(param[1]);
	params[2] = LCXFixed(param[2]);
	params[3] = LCXFixed(param[3]);
}


void glGetLightxv(GLenum light, GLenum pname, GLfixed* params)
{
	GLfloat	param[8]={0};

	glGetLightfv(light, pname, param);

	params[0] = LCXFixed(param[0]);
	params[1] = LCXFixed(param[1]);
	params[2] = LCXFixed(param[2]);
	params[3] = LCXFixed(param[3]);
}


void glGetMaterialxv(GLenum face, GLenum pname, GLfixed* params)
{
	GLfloat	param[8]={0};

	glGetMaterialfv(face, pname, param);

	params[0] = LCXFixed(param[0]);
	params[1] = LCXFixed(param[1]);
	params[2] = LCXFixed(param[2]);
	params[3] = LCXFixed(param[3]);
}

void glGetTexEnvxv(GLenum target, GLenum pname, GLfixed* params)
{
	GLfloat	param[8]={0};

	glGetTexEnvfv(target, pname, param);

	params[0] = LCXFixed(param[0]);
	params[1] = LCXFixed(param[1]);
	params[2] = LCXFixed(param[2]);
	params[3] = LCXFixed(param[3]);
}


void glGetTexParameterxv(GLenum target, GLenum pname, GLfixed* params)
{
	GLfloat	param[8]={0};

	glGetTexParameterfv(target, pname, param);

	params[0] = LCXFixed(param[0]);
	params[1] = LCXFixed(param[1]);
	params[2] = LCXFixed(param[2]);
	params[3] = LCXFixed(param[3]);
}


GLboolean glIsBuffer(GLuint buffer)
{
	if(NULL == pArbIsBuffer)
	{
		LOGW("Warning: glIsBuffer. It is only supported on the OpenGL ES.\n");
		return 0;
	}

	return pArbIsBuffer(buffer);
}




void glMatrixIndexPointerOES(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	if(NULL == pArbMatrixIndexPointer)
	{
		LOGW("Warning: glMatrixIndexPointer. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbMatrixIndexPointer(size, type, stride, pointer);
}


void glWeightPointerOES(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	if(NULL == pArbWeightPointer)
	{
		LOGW("Warning: glWeightPointer. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbWeightPointer(size, type, stride, pointer);
}


void glClipPlanef(GLenum plane, const GLfloat* equation)
{
	GLdouble	param[8]={0};

	param[0] = (GLdouble)equation[0];
	param[1] = (GLdouble)equation[1];
	param[2] = (GLdouble)equation[2];
	param[3] = (GLdouble)equation[3];

	glClipPlane(plane, param);
}

void glClipPlanex(GLenum plane, const GLfixed* equation)
{
	GLdouble	param[8]={0};

	param[0] = (GLdouble)LCXFloatFromFixed(equation[0]);
	param[1] = (GLdouble)LCXFloatFromFixed(equation[1]);
	param[2] = (GLdouble)LCXFloatFromFixed(equation[2]);
	param[3] = (GLdouble)LCXFloatFromFixed(equation[3]);

	glClipPlane(plane, param);
}


void glPointSizePointerOES(GLenum type, GLsizei stride, const GLvoid* pointer)
{
	LOGW("Warning: glPointSizePointerOES. It is only supported on the OpenGL ES.\n");
}


void glPointParameterfv(GLenum pname, const GLfloat* params)
{
	if(NULL == pArbPointParameterfv)
	{
		LOGW("Warning: glPointParameterfv. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbPointParameterfv (pname, params);
}


void glPointParameterxv(GLenum pname, const GLfixed* params)
{
	GLfloat	param[8]={0};

	if(NULL == pArbPointParameterfv)
	{
		LOGW("Warning: glMultiTexCoord4f. It is only supported on the OpenGL ES.\n");
		return;
	}

	param[0] = LCXFloatFromFixed(params[0]);
	param[1] = LCXFloatFromFixed(params[1]);
	param[2] = LCXFloatFromFixed(params[2]);
	param[3] = LCXFloatFromFixed(params[3]);

	pArbPointParameterfv (pname, param);
}


void glPointParameterf(GLenum pname, GLfloat params)
{
	if(NULL == pArbPointParameterf)
	{
		LOGW("Warning: glMultiTexCoord4f. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbPointParameterf(pname, params);
}


void glPointParameterx(GLenum pname, GLfixed params)
{
	if(NULL == pArbPointParameterf)
	{
		LOGW("Warning: glMultiTexCoord4f. It is only supported on the OpenGL ES.\n");
		return;
	}

	pArbPointParameterf(pname, LCXFloatFromFixed(params));
}


void glTexParameterxv(GLenum target, GLenum pname, const GLfixed* params)
{
	GLfloat	param[8]={0};

	param[0] = LCXFloatFromFixed(params[0]);
	param[1] = LCXFloatFromFixed(params[1]);
	param[2] = LCXFloatFromFixed(params[2]);
	param[3] = LCXFloatFromFixed(params[3]);

	glTexParameterfv(target, pname, param);
}

GLbitfield glQueryMatrixxOES(GLfixed mantissa[16], GLint exponent[16])
{
	LOGW("Warning: glQueryMatrixxOES. It is only supported on the OpenGL ES.\n");
	return 0;
}






INT EnumExtension(char* sCheck, char* sGetStrings)
{

	char*	sSrc= sGetStrings;
	char	seps[]   = " ";
	char*	token;

	token = strtok( sSrc, seps );
	while( token != NULL )
	{
		if(0 == LcStr_Stricmp(sCheck, token))
			return 0;

		token = strtok( NULL, seps );
	}

	return -1;

//	sExtension
//	GL_ARB_multitexture
//	GL_EXT_texture_env_add
//	GL_EXT_compiled_vertex_array
//	GL_S3_s3tc
//	GL_ARB_depth_texture
//	GL_ARB_fragment_program
//	GL_ARB_fragment_program_shadow
//	GL_ARB_fragment_shader
//	GL_ARB_multisample
//	GL_ARB_occlusion_query
//	GL_ARB_point_param
}




EGLint eglGetError(void)
{
	return glGetError();
}


EGLDisplay  eglGetDisplay(NativeDisplayType displayID)
{
	HDC	hDC	= GetDC((HWND)displayID);
	return hDC;
}

EGLBoolean  eglTerminateWin32(NativeDisplayType displayID, EGLDisplay dpy)
{
	HWND	hWnd = (HWND)displayID;
	HDC		hDC	= (HDC)dpy;

	ReleaseDC(hWnd, hDC);
	return true;
}


EGLBoolean  eglTerminate(EGLDisplay dpy)
{
	HWND	hWnd = GetActiveWindow();
	HDC		hDC	= (HDC)dpy;

	if(hWnd)
		ReleaseDC(hWnd, hDC);

	return true;
}


EGLBoolean  eglInitialize(EGLDisplay dpy, EGLint* major, EGLint* minor)
{
	// do nothing
	*major = g_GLverMajor;
	*minor = g_GLverMinor;
	return true;
}





const char* eglQueryString(EGLDisplay dpy, EGLint name)
{
	LOGW("Warning: eglQueryString. It is only supported on the OpenGL ES.\n");
	return NULL;
}


EGLBoolean  eglGetConfigs(EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config)
{
	*num_config = 1;
	return true;
}


EGLBoolean  eglChooseConfig(EGLDisplay dpy, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config)
{
	LOGW("Warning: eglChooseConfig. It is only supported on the OpenGL ES.\n");
	return true;
}


EGLBoolean  eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint* value)
{
	LOGW("Warning: eglChooseConfig. It is only supported on the OpenGL ES.\n");
	return true;
}


EGLSurface  eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, NativeWindowType win, const EGLint* attrib_list)
{
	HWND	hWnd = (HWND)win;
	HDC		hDC	= (HDC)dpy;


	PIXELFORMATDESCRIPTOR pfd = {0};
	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |PFD_DOUBLEBUFFER;	//|PFD_GENERIC_ACCELERATED;
	pfd.iPixelType	= PFD_TYPE_RGBA;
	pfd.cColorBits	= 32;
	pfd.cDepthBits	= 16;
	pfd.cStencilBits=  0;
	pfd.iLayerType	= PFD_MAIN_PLANE;


	UINT PixelFormat = ChoosePixelFormat(hDC, &pfd);

	if(0 == PixelFormat)
		return NULL;

	if(0 == SetPixelFormat(hDC, PixelFormat,&pfd))
		return NULL;

	return hDC;
}


EGLBoolean  eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
	// do nothing
	LOGW("Warning: eglDestroySurface. It is only supported on the OpenGL ES.\n");
	return true;
}


EGLContext  eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint* attrib_list)
{
	HDC		hDC	= (HDC)dpy;
	HGLRC	hRC = wglCreateContext(hDC);

	if(NULL == hRC)
		return NULL;


	return hRC;
}


EGLBoolean  eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
	HGLRC	hRC	= (HGLRC)ctx;
	wglDeleteContext(hRC);
	return true;
}



EGLBoolean  eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
	HDC		hDC	= (HDC)dpy;
	HGLRC	hRC	= (HGLRC)ctx;

	if(0 == wglMakeCurrent(hDC, hRC))
		return false;

	return true;
}

EGLBoolean  eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
	HDC		hDC	= (HDC)dpy;
	SwapBuffers(hDC);
	return true;
}

EGLSurface  eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint* attrib_list)
{
	LOGW("Warning: eglCreatePbufferSurface. It is only supported on the OpenGL ES.\n");
	return NULL;
}

EGLSurface  eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint* attrib_list)
{
	LOGW("Warning: eglCreatePixmapSurface. It is only supported on the OpenGL ES.\n");
	return NULL;
}


EGLBoolean  eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint* value)
{
	LOGW("Warning: eglQuerySurface. It is only supported on the OpenGL ES.\n");
	return false;
}

EGLBoolean eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
	LOGW("Warning: eglSurfaceAttrib. It is only supported on the OpenGL ES.\n");
	return false;
}


EGLBoolean eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
	LOGW("Warning: eglBindTexImage. It is only supported on the OpenGL ES.\n");
	return false;
}


EGLBoolean eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
	LOGW("Warning: eglReleaseTexImage. It is only supported on the OpenGL ES.\n");
	return false;
}


EGLBoolean  eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
	EGLBoolean hr;
	if(NULL == pWglSwapInterval)
	{
		LOGW("Warning: eglSwapInterval. It is only supported on the OpenGL ES.\n");
		return true;
	}

	hr = pWglSwapInterval(interval);
	return true;
}




EGLContext  eglGetCurrentContext(void)
{
	LOGW("Warning: eglGetCurrentContext. It is only supported on the OpenGL ES.\n");
	return NULL;
}


EGLSurface  eglGetCurrentSurface(EGLint readdraw)
{
	LOGW("Warning: eglGetCurrentSurface. It is only supported on the OpenGL ES.\n");
	return NULL;
}

EGLDisplay  eglGetCurrentDisplay(void)
{
	LOGW("Warning: eglGetCurrentDisplay. It is only supported on the OpenGL ES.\n");
	return NULL;
}


EGLBoolean  eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value)
{
	LOGW("Warning: eglQueryContext. It is only supported on the OpenGL ES.\n");
	return false;
}

EGLBoolean  eglWaitGL(void)
{
	LOGW("Warning: eglWaitGL. It is only supported on the OpenGL ES.\n");
	return true;
}


EGLBoolean  eglWaitNative(EGLint engine)
{
	LOGW("Warning: eglWaitNative. It is only supported on the OpenGL ES.\n");
	return true;
}


EGLBoolean  eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, NativePixmapType target)
{
	LOGW("Warning: eglCopyBuffers. It is only supported on the OpenGL ES.\n");
	return true;
}


//extern void (*eglGetProcAddress(const char *procname)) (void);


#endif

