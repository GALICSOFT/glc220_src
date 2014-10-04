//	Copyright(C) GaLic Soft. All rights reserved.
//
//	for 2D Sprite Mx
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcSprite_H_
#define _LcSprite_H_

#include <LcType.h>

class CLcSprite : public ILC_Sprite
{
public:
	struct TLAYER_QUAD
	{
		LCXVECTOR2 	p;						// Position
		LCXVECTOR2 	t;						// Texture
	};

	enum
	{
		LC_MAXRC =	128 * 128,
	};

protected:
	INT			m_Type		;				// Sprite Type
	PLC_TEXTURE	m_pTex		;				// Current texture
	INT			m_nQuad		;				// count of quad
	LCXVECTOR3	m_VtxP[LC_MAXRC*4];			// Position
	LCXCOLORB	m_VtxC[LC_MAXRC*4];			// Color
	LCXVECTOR2	m_VtxT[LC_MAXRC*4];			// Texture

	LCXMATRIX	m_mtWld		;				// world Matrix
	LCXMATRIX	m_mtViw		;				// View Matrix
	LCXMATRIX	m_mtPrj		;				// Projection Matrix

	LCXMATRIX*	m_ExtWld	;				// external World matrix
	LCXMATRIX*	m_ExtViw	;				// external viewing matrix
	LCXMATRIX*	m_ExtPrj	;				// external projection matrix

	LCXMATRIX	m_mtPrjO	;				// saved projection matrix
	LCXMATRIX	m_mtViwO	;				// saved viewing  matrix

	FLOAT		m_BndR		;				// bounding Circle Radius
	LCXVECTOR2	m_BndC		;				// bounding circle center

	INT			m_BldSrc	;				// source blending option
	INT			m_BldDst	;				// dest blending option
	INT			m_bCull		;				// Culling Enable
	FLOAT		m_ScnW		;				// Screen Width
	FLOAT		m_ScnH		;				// Screen Height
	FLOAT		m_posZ		;				// z-bias
	INT			m_nCntBgn	;				// Save the Count of Call begin() function.

public:
	CLcSprite();
	virtual ~CLcSprite();

	virtual INT	Query(char* sCmd, LC_HANDLE pData);
	virtual INT	Release();

	virtual INT GetType() {	return m_Type;	}

	virtual INT	Begin(UINT dMode=LC_OBJ_SPRITE);		// Save 3d environment before the execution of Sprite.
	virtual INT	End(UINT dForce=LC_FALSE);				// Reload saved 3d environment.

	virtual	INT	SetTransform(INT nTM, const LC_HANDLE v);
	virtual	INT	GetTransform(INT nTM, LC_HANDLE v);
	virtual	INT	SetRenderState(INT nState, INT v);		// Redering Machine State

	virtual INT		Draw( PLC_TEXTURE		pTexture	// LC_Texture Pointer
						, const LC_RECTF*	pImgRc		// Image rect
						, const LC_FLOAT2*	vcTrn		// Position
						, const LC_COLORF*	color=NULL	// color
						);

	virtual INT		DrawScaling( PLC_TEXTURE pTexture	// LC_Texture Pointer
						, const LC_RECTF*	pImgRc		// Image rect
						, const LC_FLOAT2*	vcTrn		// Position
						, const LC_FLOAT2*	vcScl		// Scaling
						, const LC_COLORF*	color=NULL	// color
						);


	virtual INT		DrawEx(PLC_TEXTURE		pTexture	// LC_Texture Pointer
						, const LC_RECTF*	pImgRc		// Image rect
						, const LC_FLOAT2*	vcTrn		// Position
						, const LC_FLOAT2*	vcScl		// Scaling
						, const LC_FLOAT2*	vcRot		// Rotation Center
						, FLOAT				fRot		// Angle(Radian)
						, const LC_COLORF*	color=NULL	// color
						);

	// Draw 2D Layer
	virtual INT	DrawFlush();							// it is going to render the assembled vertex list. if the current teuxture and new texture is not equal then absolutely draw assembled vertex list and assemble vertex for new texture.

public:
	INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);

protected:
	void	Destroy();
	void	AddQuad(LCXVECTOR2* p, LCXVECTOR2* t, LCXCOLORB c);
};


#endif

