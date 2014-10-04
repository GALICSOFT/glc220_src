//	Copyright(C) GaLic Soft. All rights reserved.
//
//	for 3D Device
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcDevice_H_
#define _LcDevice_H_


class CLcDevice : public ILC_Device
{
// System
protected:
	static	LC_HANDLE	m_pEgDsp	;		// Egl Display
	static	LC_HANDLE	m_pEgSrf	;		// Egl Surface
	static	LC_HANDLE	m_pEgCtx	;		// Egl Context
	static	LC_HANDLE	m_pEgCfg[LC_MAX_EGL_CONFIG];
	static	UINT		m_fbFrame	;		// Default Frame Buffer for iPhone
	static	UINT		m_fbColor	;		// Color Render Buffer for iPhone

// 3d pipe line
public:
	static LCXCOLOR		m_ClearColor;		// Clear Color
	static INT			m_dFVF		;		// Flexible Vertex Format
	static INT			m_iFVF		;		// Flexible Index Format
	static INT			m_BlndS		;		// Source Blend Function
	static INT			m_BlndD		;		// Dest Blend Function
	static LCXMATRIX	m_mtWld		;		// world Matrix
	static LCXMATRIX	m_mtViw		;		// View Matrix
	static LCXMATRIX	m_mtPrj		;		// Projection Matrix

	static	PLC_TEXTURE m_pTex  [4]	;		// Current Texture[4]
	static	INT			m_TssAdS[4]	;		// Texture Address mode f
	static	INT			m_TssAdT[4]	;		// Texture Address mode for the T coordinate. default: GL_REPEAT
	static	INT			m_TssMin[4]	;		// Minification Filter. default: GL_LINEAR
	static	INT			m_TssMag[4]	;		// Magnification Filter. default: GL_LINEAR
	static	INT			m_TssOp [4]	;		// MultiTexture Blending Mode

public:
	CLcDevice();
	virtual ~CLcDevice();

	virtual INT Query(char* sCmd, LC_HANDLE pData);// inherit from the ILC_Object
	virtual INT	Release();							// inherit from the ILC_Object

	// 3D PipeLine Interface
	virtual	INT	SetTransform(INT nTransform, const LC_HANDLE Val);
	virtual	INT	GetTransform(INT nTransform, LC_HANDLE Val);

	virtual	INT	BeginScene();								// Prepare Rendering
	virtual	INT	EndScene();									// Post Rendering

	virtual	INT	SetRenderState(INT nState, INT v);			// Redering Machine State
	virtual	INT	SetRenderState(INT nState, LC_HANDLE v);	// Redering Machine State for Pointer
	virtual	INT	SetStatusEnable(INT nStatus);				// Wrapping glEnable()
	virtual	INT	SetStatusDisable(INT nStatus);				// Wrapping glDisable()

	// Sampler and Multi-texture State
	virtual INT	SetTextureEnv(INT v1, INT v2);				// glTexEnvi(GL_TEXTURE_ENV, ..., ...)

	virtual INT	SetTextureAddressS (INT nState, INT v);		// nState & 0xFFFFFFFC: 0~3¸¸ »ç¿ë
	virtual INT	SetTextureAddressT (INT nState, INT v);
	virtual INT	SetTextureFilterMin(INT nState, INT v);
	virtual INT	SetTextureFilterMag(INT nState, INT v);
	virtual INT	SetTextureStageOp  (INT nState, INT v);

	// Vertex Format
	virtual	INT	SetFVF(INT nFVF);

	// Set Texture
	virtual INT	SetTexture(INT nState, PLC_TEXTURE pTex);

	// Color Setting...
	virtual	INT	SetColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a=1.f);	// glColor4f()
	virtual	INT	SetColor(FLOAT* v);									// glColor4v()

	// Rendering Primitive for Vertex Buffer Object
	virtual INT	SetStreamSource(PLC_VB);
	virtual INT	SetIndices(PLC_IB);
	virtual INT	DrawPrimitive();
	virtual INT	DrawIndexedPrimitive(INT nMode, INT nCount);

	// Rendering Primitive for User Memory Pointer
	virtual INT	DrawPrimitiveUP(INT nMode, INT First, INT nVtx, const void* pVtx, INT nStride);
	virtual INT	DrawIndexedPrimitiveUP(INT nMode, INT nIdx, const void *pIdx, const void* pVtx, INT nStride);

	virtual INT	Clear(UINT nMode);
	virtual INT	Present(LC_HANDLE =0, LC_HANDLE  =0, LC_HANDLE =0, LC_HANDLE =0);

public:
	INT		CreateForLinux  (LC_HANDLE p1=NULL, LC_HANDLE p2=NULL);
	INT		CreateForIphone (LC_HANDLE p1=NULL, LC_HANDLE p2=NULL);
	INT		CreateForAndroid(LC_HANDLE p1=NULL, LC_HANDLE p2=NULL);
	INT		CreateForBada   (LC_HANDLE p1=NULL, LC_HANDLE p2=NULL);
	INT		CreateForWin    (LC_HANDLE p1=NULL, LC_HANDLE p2=NULL);

	void	Destroy();
};

// for iphone
INT  XcodeEglCreate (LC_HANDLE* EgCtx, UINT* FrameBuffer, UINT* ColorBuffer);
void XcodeEglDestroy(LC_HANDLE* EgCtx, UINT* FrameBuffer, UINT* ColorBuffer);
INT  XcodeEglResize (INT* ScnW, INT* ScnH, LC_HANDLE Layer, LC_HANDLE EgCtx, UINT FrameBuffer, UINT ColorBuffer);
void XcodeEglSetFrameBuffer(LC_HANDLE EgCtx, UINT FrameBuffer);
void XcodeEglPresent(LC_HANDLE pEgCtx);


#endif


