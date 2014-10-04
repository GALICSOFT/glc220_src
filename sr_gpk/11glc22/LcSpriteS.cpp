// Implementation of the CLcSpriteS.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_PC_LINUX_)
  #include <GL/gl.h>

#elif defined(__APPLE__) || defined(__IPHONE__)
  #import <OpenGLES/ES1/gl.h>

#elif defined(__BADA__)
  #include <FGraphicsOpengl.h>
  using namespace Osp::Graphics::Opengl;

#else
  #include <gles/gl.h>

#endif

#include <LcType.h>
#include <LcMath.h>

#include "LcAux.h"
#include "LcBase.h"
#include "LcTexture.h"


class CLcSpriteS : public ILC_SpriteS
{
protected:
	INT			m_Type		;				// Sprite Type
	LCXVECTOR3	m_VtxPos[4]	;				// Position
	LCXCOLOR	m_VtxDif[4]	;				// Diffuse color
	LCXVECTOR2	m_VtxTex[4]	;				// Texture Coordinate

	LCXMATRIX	m_mtWld		;				// world Matrix
	LCXMATRIX	m_mtViw		;				// View Matrix
	LCXMATRIX	m_mtPrj		;				// Projection Matrix

	LCXMATRIX*	m_ExtWld	;				// external World matrix
	LCXMATRIX*	m_ExtViw	;				// external viewing matrix
	LCXMATRIX*	m_ExtPrj	;				// external projection matrix

//	LCXMATRIX	m_mtPrjO	;				// saved projection matrix
//	LCXMATRIX	m_mtViwO	;				// saved viewing  matrix

	FLOAT		m_BndR		;				// bounding Circle Radius
	LCXVECTOR2	m_BndC		;				// bounding circle center

	INT			m_BldSrc	;				// source blending option
	INT			m_BldDst	;				// dest blending option
	INT			m_bCull		;				// Culling Enable
	FLOAT		m_ScnW		;				// Screen Width
	FLOAT		m_ScnH		;				// Screen Height
	INT			m_nCntBgn	;				// Save the Count of Call begin() function.

public:
	CLcSpriteS();
	virtual ~CLcSpriteS();

	virtual	INT	Query(char* sCmd, LC_HANDLE pData);	// inherit from the ILC_Object
	virtual	INT	Release();							// inherit from the ILC_Object

	virtual INT GetType() {	return m_Type;	}

	virtual INT	Begin(UINT dMode=LC_OBJ_SPRITE);	// Save 3d environment before the execution of Sprite.
	virtual INT	End(UINT dForce=LC_FALSE);			// Reload saved 3d environment.

	virtual	INT	SetTransform(INT nTM, const LC_HANDLE v);
	virtual	INT	GetTransform(INT nTM, LC_HANDLE v);
	virtual	INT	SetRenderState(INT nState, INT v);	// Redering Machine State


	virtual INT	Draw( PLC_TEXTURE		pTexture	// LC_Texture Pointer
					, const LC_RECTF*	pImgRc		// Image rect
					, const LC_FLOAT2*	vcTrn		// Position
					, const LC_COLORF*	color=NULL	// color
					);

	virtual INT	DrawScaling(PLC_TEXTURE pTexture	// LC_Texture Pointer
					, const LC_RECTF*	pImgRc		// Image rect
					, const LC_FLOAT2*	vcTrn		// Position
					, const LC_FLOAT2*	vcScl		// Scaling
					, const LC_COLORF*	color=NULL	// color
					);

	virtual INT	DrawEx(PLC_TEXTURE		pTexture	// LC_Texture Pointer
					, const LC_RECTF*	pImgRc		// Image rect
					, const LC_FLOAT2*	vcTrn		// Position
					, const LC_FLOAT2*	vcScl		// Scaling
					, const LC_FLOAT2*	vcRot		// Rotation Center
					, FLOAT				fRot		// Angle(Radian)
					, const LC_COLORF*	color=NULL	// color
					);

	INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);

protected:
	void	Destroy();
};


////////////////////////////////////////////////////////////////////////////////


CLcSpriteS::CLcSpriteS()
{
	m_Type		= LC_OBJ_GUI;

	m_VtxPos[0] = LCXVECTOR3(-0.9F, -0.9F, 0.0F);
	m_VtxPos[1] = LCXVECTOR3( 0.9F, -0.9F, 0.0F);
	m_VtxPos[2] = LCXVECTOR3( 0.9F,  0.9F, 0.0F);
	m_VtxPos[3] = LCXVECTOR3(-0.9F,  0.9F, 0.0F);

	m_VtxTex[0] = LCXVECTOR2( 0.0F,  0.0F);
	m_VtxTex[1] = LCXVECTOR2( 1.0F,  0.0F);
	m_VtxTex[2] = LCXVECTOR2( 1.0F,  1.0F);
	m_VtxTex[3] = LCXVECTOR2( 0.0F,  1.0F);

	m_mtWld.Identity()	;
	m_mtViw.Identity()	;
	m_mtPrj.Identity()	;

	m_ExtWld = NULL;
	m_ExtViw = NULL;
	m_ExtPrj = NULL;

//	m_mtPrjO.Identity();
//	m_mtViwO.Identity();

	m_BndR	 = 0;
	m_BndC	 = LCXVECTOR2(0,0);

	m_BldSrc = GL_SRC_ALPHA;
	m_BldDst = GL_ONE_MINUS_SRC_ALPHA;
	m_bCull	 = LC_TRUE;
	m_ScnW	 = 0;
	m_ScnH	 = 0;

	m_nCntBgn= 0;
}


CLcSpriteS::~CLcSpriteS()
{
	Destroy();
}


INT CLcSpriteS::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcSpriteS::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}

void CLcSpriteS::Destroy()
{
}


INT CLcSpriteS::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE, LC_HANDLE)
{
	INT	ScnW=0, ScnH=0;
	LcVar_WindowSize(&ScnW, &ScnH);


	if(p1)
		m_Type = *(INT*)p1;	// Setup the type


	//m_ScnW = 320;
	//m_ScnH = 480;
	m_ScnW = (FLOAT)ScnW;
	m_ScnH = (FLOAT)ScnH;


	// Setup the Projection Matrix
	FLOAT	l = -ScnW/2.F;		// left
	FLOAT	r = +ScnW/2.F;		// right

	FLOAT	b = -ScnH/2.F;		// bottom
	FLOAT	t = +ScnH/2.F;		// top

	FLOAT	n = +4096.00F;		// near
	FLOAT	f = -4096.00F;		// far


	if(LC_OBJ_GUI == m_Type)
	{
		// Ortho Left Top
		m_mtPrj.SetupOrthoGl( 0.F, 2*r
							, 2*t, 0.F
							, n, f);
	}

	else if(LC_OBJ_MODEL2D == m_Type)
	{
		// Ortho Center
		m_mtPrj.SetupOrthoGl( l, r
							, b, t
							, n, f);
	}


	m_BndR	= FLOAT( lc_max(m_ScnW, m_ScnH) );
	m_BndR *= 1.25F;
	m_BndC	= LCXVECTOR2(m_ScnW, m_ScnH) * 0.5f;


	m_VtxPos[0] = LCXVECTOR3(0.000F, m_ScnH, 0.0F);
	m_VtxPos[1] = LCXVECTOR3(m_ScnW, m_ScnH, 0.0F);
	m_VtxPos[2] = LCXVECTOR3(m_ScnW, 0.000F, 0.0F);
	m_VtxPos[3] = LCXVECTOR3(0.000F, 0.000F, 0.0F);

	return LC_OK;
}


////////////////////////////////////////////////////////////////////////////////

INT CLcSpriteS::Begin(UINT dMode)
{
	//increase the count
	++m_nCntBgn;

	if(1 != m_nCntBgn)
		return LC_OK;


	// save old state
	//glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat*)&m_mtPrjO);
	//glGetFloatv(GL_MODELVIEW_MATRIX,  (GLfloat*)&m_mtViwO);


	////////////////////////////////////////////////////////////////////////////
	// setup the projection transform

	INT	ScnW=0, ScnH=0;
	LcVar_WindowSize(&ScnW, &ScnH);
	m_ScnW = (FLOAT)ScnW;
	m_ScnH = (FLOAT)ScnH;


	// Setup the Projection Matrix
	FLOAT	l = -ScnW/2.F;		// left
	FLOAT	r = +ScnW/2.F;		// right

	FLOAT	b = -ScnH/2.F;		// bottom
	FLOAT	t = +ScnH/2.F;		// top

	FLOAT	n = +4096.00F;		// near
	FLOAT	f = -4096.00F;		// far


	if(LC_OBJ_GUI == m_Type)
	{
		// Ortho Left Top
		m_mtPrj.SetupOrthoGl( 0.F, 2*r
							, 2*t, 0.F
							, n, f);
	}

	else if(LC_OBJ_MODEL2D == m_Type)
	{
		// Ortho Center
		m_mtPrj.SetupOrthoGl( l, r
							, b, t
							, n, f);
	}


	m_BndR	= FLOAT( lc_max(m_ScnW, m_ScnH) );
	m_BndR *= 1.25F;
	m_BndC	= LCXVECTOR2(m_ScnW, m_ScnH) * 0.5f;


	// load to the projection transform
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf((GLfloat*)&m_mtPrj);



	////////////////////////////////////////////////////////////////////////////
	// setup the model-view projection

	// Setup the View Matrix
	m_mtViw.Identity();

	LCXMATRIX  mtWV;
	LCXMATRIX  mtOrt;

	LcSys_OrientMatrix(&mtOrt, m_Type);


	mtWV = mtOrt * m_mtViw;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat*)&mtWV);



	////////////////////////////////////////////////////////////////////////////
	// setup the rendering state

	glEnable(GL_SCISSOR_TEST);
	glClearDepthf(1.0F);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 4.0F/255.0F);


	glDisable(GL_CULL_FACE );
	glDisable(GL_LIGHTING);

	glEnable(GL_BLEND);
	glBlendFunc(m_BldSrc, m_BldDst);


	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glClientActiveTexture(GL_TEXTURE0);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// normal vector and color buffer disable
	glDisableClientState(GL_NORMAL_ARRAY);

	// enable vertex and texture coordinate buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	return LC_OK;
}



INT CLcSpriteS::End(UINT dForce)
{
	if(dForce)
		m_nCntBgn = 1;


	//deccrease the count
	--m_nCntBgn;

	if(0 != m_nCntBgn)
		return LC_OK;


	//glDepthMask(GL_TRUE);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_BldSrc= GL_SRC_ALPHA;
	m_BldDst= GL_ONE_MINUS_SRC_ALPHA;
	m_bCull = LC_TRUE;


	// restore old values
	//glMatrixMode(GL_PROJECTION);	glLoadMatrixf((GLfloat*)&m_mtPrjO);
	//glMatrixMode(GL_MODELVIEW );	glLoadMatrixf((GLfloat*)&m_mtViwO);

	return LC_OK;
}


////////////////////////////////////////////////////////////////////////////////


INT CLcSpriteS::SetTransform(INT nTM, const LC_HANDLE v)
{
	LCXMATRIX* pmtWld = &m_mtWld;
	LCXMATRIX* pmtViw = &m_mtViw;
	LCXMATRIX* pmtPrj = &m_mtPrj;

	if(LC_TS_WORLD_IDENTITY<= nTM && nTM <=LC_TS_PROJ_IDENTITY)
	{
		if      (LC_TS_WORLD_IDENTITY== nTM)	m_mtWld.Identity();
		else if	(LC_TS_VIEW_IDENTITY == nTM)	m_mtViw.Identity();
		else if	(LC_TS_PROJ_IDENTITY == nTM)	m_mtPrj.Identity();
	}
	else
	{
		if		(LC_TS_WORLD== nTM)		memcpy(&m_mtWld, v, sizeof(LCXMATRIX));
		else if	(LC_TS_VIEW == nTM)		memcpy(&m_mtViw, v, sizeof(LCXMATRIX));
		else if	(LC_TS_PROJ == nTM)		memcpy(&m_mtPrj, v, sizeof(LCXMATRIX));
		else if (LC_TS_WORLD_EXT== nTM)	m_ExtWld = (LCXMATRIX*)v;
		else if (LC_TS_VIEW_EXT == nTM)	m_ExtViw = (LCXMATRIX*)v;
		else if (LC_TS_PROJ_EXT == nTM)	m_ExtPrj = (LCXMATRIX*)v;
		else
			return LC_EFAIL;
	}


	if(m_ExtWld) pmtWld = m_ExtWld;
	if(m_ExtViw) pmtViw = m_ExtViw;
	if(m_ExtPrj) pmtPrj = m_ExtPrj;

	LCXMATRIX  mtOWV;
	LCXMATRIX  mtOrt;
	LCXMATRIX  mtWV = *pmtWld * *pmtViw;

	LcSys_OrientMatrix(&mtOrt, m_Type);	// Orient matrix;


	if( LC_TS_WORLD    == nTM || LC_TS_VIEW     == nTM || LC_TS_WORLD_IDENTITY == nTM ||
		LC_TS_WORLD_EXT== nTM || LC_TS_VIEW_EXT == nTM || LC_TS_VIEW_IDENTITY  == nTM   )
	{
		mtOWV = mtOrt * mtWV;
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((GLfloat*)&mtWV);
	}
	else if(LC_TS_PROJ== nTM || LC_TS_PROJ_EXT== nTM)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((FLOAT*)pmtPrj);
		glMatrixMode(GL_MODELVIEW);
	}

	// only to glOrtho...
	FLOAT r = ( 1 - pmtPrj->_41)/pmtPrj->_11;
	FLOAT l = (-1 - pmtPrj->_41)/pmtPrj->_11;
	FLOAT t = ( 1 - pmtPrj->_42)/pmtPrj->_22;
	FLOAT b = (-1 - pmtPrj->_42)/pmtPrj->_22;

	r = fabsf(r - l);
	b = fabsf(b - t);

	m_BndR	= lc_max(r, b);
	m_BndR *= 1.25F;
	m_BndC	= LCXVECTOR2(m_ScnW, m_ScnH) * 0.5f;

	LCXVECTOR2 vOff(0,0);
	mtWV.Inverse(NULL, LC_TRUE);
	vOff.x   = mtWV._41;		// camera position x
	vOff.y   = mtWV._42;		// camera position y
	m_BndC  += vOff;

	return LC_OK;
}


INT CLcSpriteS::GetTransform(INT nTM, LC_HANDLE v)
{
	if		(LC_TS_WORLD== nTM)	memcpy(v, &m_mtWld, sizeof(LCXMATRIX));
	else if	(LC_TS_PROJ == nTM)	memcpy(v, &m_mtPrj, sizeof(LCXMATRIX));
	else if	(LC_TS_VIEW == nTM)	memcpy(v, &m_mtViw, sizeof(LCXMATRIX));
	else	return LC_EFAIL;

	return LC_OK;
}


INT CLcSpriteS::SetRenderState(INT ns, INT v)
{
	INT hr = LC_OK;

	if(LC_RS_BLEND_SRC == ns)
	{
		m_BldSrc= v;
		glBlendFunc(m_BldSrc, m_BldDst);
	}
	else if(LC_RS_BLEND_DEST == ns)
	{
		m_BldDst= v;
		glBlendFunc(m_BldSrc, m_BldDst);
	}
	else if(LC_RS_CULL_MODE == ns)
	{
		m_bCull = v;
	}
	else if(LC_RS_ZBIAS == ns)
	{
		FLOAT z = *((FLOAT*)&v);
		if(z == m_VtxPos[0].z)
			return LC_OK;

		m_VtxPos[0].z = z;
		m_VtxPos[1].z = z;
		m_VtxPos[2].z = z;
		m_VtxPos[3].z = z;
	}

	else
		return LC_EFAIL;

	return hr;
}


////////////////////////////////////////////////////////////////////////////////


INT CLcSpriteS::Draw(  PLC_TEXTURE		pTexture	// LC_Texture Pointer
					, const LC_RECTF*	pImgRc		// Image rect
					, const LC_FLOAT2*	pvcTrn		// Position
					, const LC_COLORF*	pColor		// color
					)
{
	FLOAT	ImgW = 0;
	FLOAT	ImgH = 0;
	FLOAT	TexW = 0;
	FLOAT	TexH = 0;
	UINT	nTex = 0;

	if(NULL == pTexture && NULL == pImgRc)	// There are non exist Texture and Image Rect
		return LC_CANNT_FIND_RESOURCE;

	if(pTexture)
	{
		ImgW = (FLOAT)pTexture->GetImgW();
		ImgH = (FLOAT)pTexture->GetImgH();
		TexW = (FLOAT)pTexture->GetTexW();
		TexH = (FLOAT)pTexture->GetTexH();
		nTex = pTexture->GetId();

		if(0==nTex)
			return LC_CANNT_FIND_RESOURCE;
	}


	FLOAT	DrwL=  0;			// Draw Region Left
	FLOAT	DrwT=  0;			// Draw Region Top
	FLOAT	DrwW=ImgW;			// Draw Region Width
	FLOAT	DrwH=ImgH;			// Draw Region Height

	LCXVECTOR2	pos[4];
	LCXVECTOR2	st0(0,0);
	LCXVECTOR2	st1(1,1);

	if(pImgRc)					// Draw Region Width, Height
	{
		DrwL = pImgRc->x;
		DrwT = pImgRc->y;
		DrwW = pImgRc->w;
		DrwH = pImgRc->h;
	}


	if(pTexture)
	{
		// Invalidate draw region
		if( DrwL >= ImgW || DrwT >= ImgH)
			return 0;

		if(DrwL + DrwW > ImgW)	DrwW = ImgW - DrwL;
		if(DrwT + DrwH > ImgH)	DrwH = ImgH - DrwT;

		st0.x = (DrwL +    0)/TexW;
		st1.x = (DrwL + DrwW)/TexW;
		st0.y = (ImgH - DrwT -    0)/TexH;
		st1.y = (ImgH - DrwT - DrwH)/TexH;
	}


	// Setup Draw Position
	if(pvcTrn)
	{
		pos[0].x = pvcTrn->x;
		pos[0].y = pvcTrn->y;
	}
	else
	{
		pos[0].x = 0;
		pos[0].y = 0;
	}

	// Setup Position
	pos[1] = pos[0] + LCXVECTOR2(DrwW,    0);
	pos[2] = pos[0] + LCXVECTOR2(DrwW, DrwH);
	pos[3] = pos[0] + LCXVECTOR2(   0, DrwH);


	// Is in Screen?
	FLOAT r = lc_max(DrwW, DrwH);
	LCXVECTOR2 centre = pos[0] + LCXVECTOR2(DrwW, DrwH ) * 0.5F;
	if(m_bCull && LC_FAILED( LCXCollisionCircle2Circle(m_BndC, centre, m_BndR, r )))
		return 0;


	// Final...
//	pos[0] = LCXVECTOR2(pos[0].y, m_ScnW- pos[0].x);
//	pos[1] = LCXVECTOR2(pos[1].y, m_ScnW- pos[1].x);
//	pos[2] = LCXVECTOR2(pos[2].y, m_ScnW- pos[2].x);
//	pos[3] = LCXVECTOR2(pos[3].y, m_ScnW- pos[3].x);

	m_VtxPos[0].x = pos[0].x;	m_VtxPos[0].y = pos[0].y;
	m_VtxPos[1].x = pos[1].x;	m_VtxPos[1].y = pos[1].y;
	m_VtxPos[2].x = pos[2].x;	m_VtxPos[2].y = pos[2].y;
	m_VtxPos[3].x = pos[3].x;	m_VtxPos[3].y = pos[3].y;

	if(LC_OBJ_GUI == m_Type)
	{
		m_VtxTex[0] = LCXVECTOR2(st0.x, st0.y);
		m_VtxTex[1] = LCXVECTOR2(st1.x, st0.y);
		m_VtxTex[2] = LCXVECTOR2(st1.x, st1.y);
		m_VtxTex[3] = LCXVECTOR2(st0.x, st1.y);
	}
	else if(LC_OBJ_MODEL2D == m_Type)
	{
		m_VtxTex[3] = LCXVECTOR2(st0.x, st0.y);
		m_VtxTex[2] = LCXVECTOR2(st1.x, st0.y);
		m_VtxTex[1] = LCXVECTOR2(st1.x, st1.y);
		m_VtxTex[0] = LCXVECTOR2(st0.x, st1.y);
	}

	// Color...
	if(pColor)
	{
		m_VtxDif[0] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
		m_VtxDif[1] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
		m_VtxDif[2] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
		m_VtxDif[3] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
	}
	else
	{
		m_VtxDif[0] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
		m_VtxDif[1] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
		m_VtxDif[2] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
		m_VtxDif[3] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
	}


	glBindTexture(GL_TEXTURE_2D, nTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glVertexPointer  (3, GL_FLOAT, 0, m_VtxPos);
	glColorPointer	 (4, GL_FLOAT, 0, m_VtxDif);
	glTexCoordPointer(2, GL_FLOAT, 0, m_VtxTex);

	if(!pTexture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}


	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	return LC_OK;
}


INT CLcSpriteS::DrawScaling(   PLC_TEXTURE		pTexture	// LC_Texture Pointer
							, const LC_RECTF*	pImgRc		// Image rect
							, const LC_FLOAT2*	pvcTrn		// Position
							, const LC_FLOAT2*	pvcScl		// Scaling
							, const LC_COLORF*	pColor		// color
							)
{
	FLOAT	ImgW = 0;
	FLOAT	ImgH = 0;
	FLOAT	TexW = 0;
	FLOAT	TexH = 0;
	UINT	nTex = 0;

	if(NULL == pTexture && NULL == pImgRc)
		return LC_CANNT_FIND_RESOURCE;


	if(pTexture)
	{
		ImgW = (FLOAT)pTexture->GetImgW();
		ImgH = (FLOAT)pTexture->GetImgH();
		TexW = (FLOAT)pTexture->GetTexW();
		TexH = (FLOAT)pTexture->GetTexH();
		nTex = pTexture->GetId();

		if(0==nTex)
			return LC_CANNT_FIND_RESOURCE;
	}


	FLOAT	DrwL=  0;	// Draw Region Left
	FLOAT	DrwT=  0;	// Draw Region Top
	FLOAT	DrwW=ImgW;	// Draw Region Width
	FLOAT	DrwH=ImgH;	// Draw Region Height

	LCXVECTOR2	pos[4];
	LCXVECTOR2	st0(0,0);
	LCXVECTOR2	st1(1,1);


	// Draw Region Width, Height
	if(pImgRc)
	{
		DrwL = pImgRc->x;
		DrwT = pImgRc->y;
		DrwW = pImgRc->w;
		DrwH = pImgRc->h;
	}


	if(pTexture)
	{
		// Invalidate draw region
		if( DrwL >= ImgW || DrwT >= ImgH)
			return 0;


		if(DrwL + DrwW > ImgW)	DrwW = ImgW - DrwL;
		if(DrwT + DrwH > ImgH)	DrwH = ImgH - DrwT;

		st0.x = (DrwL +    0)/TexW;
		st1.x = (DrwL + DrwW)/TexW;
		st0.y = (ImgH - DrwT -    0)/TexH;
		st1.y = (ImgH - DrwT - DrwH)/TexH;
	}


	// Setup Draw Position
	if(pvcTrn)
	{
		pos[0].x = pvcTrn->x;
		pos[0].y = pvcTrn->y;
	}
	else
	{
		pos[0].x = 0;
		pos[0].y = 0;
	}



	// Setup Position
	if(NULL == pvcScl || (pvcScl && 1.0F == pvcScl->x && 1.0F == pvcScl->y))
	{
		pos[1] = pos[0] + LCXVECTOR2(DrwW,    0);
		pos[2] = pos[0] + LCXVECTOR2(DrwW, DrwH);
		pos[3] = pos[0] + LCXVECTOR2(   0, DrwH);
	}
	else
	{
		FLOAT	scX = pvcScl->x;
		FLOAT	scY = pvcScl->y;

		if(scX>=0)
		{
			pos[1].x = pos[0].x + DrwW * scX;
			pos[2].x = pos[1].x;
			pos[3].x = pos[0].x;
		}
		else
		{
			pos[1].x = pos[0].x;
			pos[2].x = pos[1].x;
			pos[0].x = pos[1].x - DrwW * scX;
			pos[3].x = pos[0].x;
		}


		if(scY>=0)
		{
			pos[1].y = pos[0].y;
			pos[3].y = pos[0].y + DrwH * scY;
			pos[2].y = pos[3].y;
		}

		else
		{
			pos[3].y = pos[0].y;
			pos[2].y = pos[3].y;
			pos[0].y = pos[3].y - DrwH * scY;
			pos[1].y = pos[0].y;
		}
	}


	// Is in Screen?
	LCXVECTOR2 vcMin;
	LCXVECTOR2 vcMax;

	if(NULL == pvcScl || (0 <= pvcScl->x &&  0 <= pvcScl->y))
	{
		vcMin = pos[0];
		vcMax = pos[2];
	}
	else
	{
		LCXVec2MakeBoundingBox(&vcMin, &vcMax, pos, 4);
	}


	// Is in Screen?
	FLOAT r = lc_max( (vcMax.x - vcMin.x) , (vcMax.y - vcMin.y) );
	LCXVECTOR2 centre = (vcMin+ vcMax) * 0.5F;
	if(m_bCull && LC_FAILED( LCXCollisionCircle2Circle(m_BndC, centre, m_BndR, r )))
		return 0;


	// Final...
//	pos[0] = LCXVECTOR2(pos[0].y, m_ScnW- pos[0].x);
//	pos[1] = LCXVECTOR2(pos[1].y, m_ScnW- pos[1].x);
//	pos[2] = LCXVECTOR2(pos[2].y, m_ScnW- pos[2].x);
//	pos[3] = LCXVECTOR2(pos[3].y, m_ScnW- pos[3].x);

	m_VtxPos[0].x = pos[0].x;	m_VtxPos[0].y = pos[0].y;
	m_VtxPos[1].x = pos[1].x;	m_VtxPos[1].y = pos[1].y;
	m_VtxPos[2].x = pos[2].x;	m_VtxPos[2].y = pos[2].y;
	m_VtxPos[3].x = pos[3].x;	m_VtxPos[3].y = pos[3].y;

	if(LC_OBJ_GUI == m_Type)
	{
		m_VtxTex[0] = LCXVECTOR2(st0.x, st0.y);
		m_VtxTex[1] = LCXVECTOR2(st1.x, st0.y);
		m_VtxTex[2] = LCXVECTOR2(st1.x, st1.y);
		m_VtxTex[3] = LCXVECTOR2(st0.x, st1.y);
	}
	else if(LC_OBJ_MODEL2D == m_Type)
	{
		m_VtxTex[3] = LCXVECTOR2(st0.x, st0.y);
		m_VtxTex[2] = LCXVECTOR2(st1.x, st0.y);
		m_VtxTex[1] = LCXVECTOR2(st1.x, st1.y);
		m_VtxTex[0] = LCXVECTOR2(st0.x, st1.y);
	}

	// Color...
	if(pColor)
	{
		m_VtxDif[0] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
		m_VtxDif[1] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
		m_VtxDif[2] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
		m_VtxDif[3] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
	}
	else
	{
		m_VtxDif[0] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
		m_VtxDif[1] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
		m_VtxDif[2] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
		m_VtxDif[3] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
	}


	glBindTexture(GL_TEXTURE_2D, nTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glVertexPointer  (3, GL_FLOAT, 0, m_VtxPos);
	glColorPointer	 (4, GL_FLOAT, 0, m_VtxDif);
	glTexCoordPointer(2, GL_FLOAT, 0, m_VtxTex);

	if(!pTexture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}


	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	return LC_OK;
}


INT CLcSpriteS::DrawEx(PLC_TEXTURE		pTexture	// LC_Texture Pointer
					, const LC_RECTF*	pImgRc		// Image rect
					, const LC_FLOAT2*	pvcTrn		// Position
					, const LC_FLOAT2*	pvcScl		// Scaling
					, const LC_FLOAT2*	pvcRot		// Rotation Center
					, FLOAT				fRot		// Angle(Radian)
					, const LC_COLORF*	pColor		// color
					)
{
	FLOAT	ImgW = 0;
	FLOAT	ImgH = 0;
	FLOAT	TexW = 0;
	FLOAT	TexH = 0;

	UINT	nTex = 0;

	if(NULL == pTexture && NULL == pImgRc)
		return LC_CANNT_FIND_RESOURCE;


	if(pTexture)
	{
		ImgW = (FLOAT)pTexture->GetImgW();
		ImgH = (FLOAT)pTexture->GetImgH();
		TexW = (FLOAT)pTexture->GetTexW();
		TexH = (FLOAT)pTexture->GetTexH();

		nTex = pTexture->GetId();

		if(0==nTex)
			return LC_CANNT_FIND_RESOURCE;
	}


	FLOAT	DrwL=  0;	// Draw Region Left
	FLOAT	DrwT=  0;	// Draw Region Top
	FLOAT	DrwW=ImgW;	// Draw Region Width
	FLOAT	DrwH=ImgH;	// Draw Region Height

	LCXVECTOR2	pos[4];
	LCXVECTOR2	st0(0,0);
	LCXVECTOR2	st1(1,1);


	// Draw Region Width, Height
	if(pImgRc)
	{
		DrwL = pImgRc->x;
		DrwT = pImgRc->y;
		DrwW = pImgRc->w;
		DrwH = pImgRc->h;
	}


	if(pTexture)
	{
		// Invalidate draw region
		if( DrwL >= ImgW || DrwT >= ImgH)
			return 0;


		if(DrwL + DrwW > ImgW)	DrwW = ImgW - DrwL;
		if(DrwT + DrwH > ImgH)	DrwH = ImgH - DrwT;


		st0.x = (DrwL +    0)/TexW;
		st1.x = (DrwL + DrwW)/TexW;
		st0.y = (ImgH - DrwT -    0)/TexH;
		st1.y = (ImgH - DrwT - DrwH)/TexH;
	}


	// Setup Draw Position
	if(pvcTrn)
	{
		pos[0].x = pvcTrn->x;
		pos[0].y = pvcTrn->y;
	}
	else
	{
		pos[0].x = 0;
		pos[0].y = 0;
	}



	// Setup Position
	if(NULL == pvcScl || (pvcScl && 1.0F == pvcScl->x && 1.0F == pvcScl->y))
	{
		pos[1] = pos[0] + LCXVECTOR2(DrwW,    0);
		pos[2] = pos[0] + LCXVECTOR2(DrwW, DrwH);
		pos[3] = pos[0] + LCXVECTOR2(   0, DrwH);
	}
	else
	{
		FLOAT	scX = pvcScl->x;
		FLOAT	scY = pvcScl->y;

		if(scX>=0)
		{
			pos[1].x = pos[0].x + DrwW * scX;
			pos[2].x = pos[1].x;
			pos[3].x = pos[0].x;
		}
		else
		{
			pos[1].x = pos[0].x;
			pos[2].x = pos[1].x;
			pos[0].x = pos[1].x - DrwW * scX;
			pos[3].x = pos[0].x;
		}


		if(scY>=0)
		{
			pos[1].y = pos[0].y;
			pos[3].y = pos[0].y + DrwH * scY;
			pos[2].y = pos[3].y;
		}

		else
		{
			pos[3].y = pos[0].y;
			pos[2].y = pos[3].y;
			pos[0].y = pos[3].y - DrwH * scY;
			pos[1].y = pos[0].y;
		}
	}


	// Rotation...
	if(pvcRot && 0.0F != fRot)
	{
		LCXVECTOR2 vcR = LCXVECTOR2(pvcRot->x, pvcRot->y);
		LCXVECTOR2 d(0,0);

		FLOAT	fCos = (FLOAT)LCXMathCos(-fRot);
		FLOAT	fSin = (FLOAT)LCXMathSin(-fRot);

		for(INT i=0; i<4; ++i)
		{
			LCXVECTOR2 vcT = pos[i] - vcR;

			d.x = vcT.x * fCos + vcT.y * fSin;
			d.y =-vcT.x * fSin + vcT.y * fCos;

			pos[i].x = (vcR.x + d.x);
			pos[i].y = (vcR.y + d.y);
		}
	}

	// Is in Screen?
	LCXVECTOR2 vcMin;
	LCXVECTOR2 vcMax;
	LCXVec2MakeBoundingBox(&vcMin, &vcMax, pos, 4);

	//if(0.0F == fRot && (NULL == pvcScl || (0 <= pvcScl->x &&  0 <= pvcScl->y))  )
	//{
	//	vcMin = pos[0];
	//	vcMax = pos[2];
	//}
	//else
	//{
	//	LCXVec2MakeBoundingBox(&vcMin, &vcMax, pos, 4);
	//}


	// Is in Screen?
	FLOAT r = lc_max( (vcMax.x - vcMin.x) , (vcMax.y - vcMin.y) );
	LCXVECTOR2 centre = (vcMin+ vcMax) * 0.5F;
	if(m_bCull && LC_FAILED( LCXCollisionCircle2Circle(m_BndC, centre, m_BndR, r )))
		return 0;


	// Final...
//	pos[0] = LCXVECTOR2(pos[0].y, m_ScnW- pos[0].x);
//	pos[1] = LCXVECTOR2(pos[1].y, m_ScnW- pos[1].x);
//	pos[2] = LCXVECTOR2(pos[2].y, m_ScnW- pos[2].x);
//	pos[3] = LCXVECTOR2(pos[3].y, m_ScnW- pos[3].x);

	m_VtxPos[0].x = pos[0].x;	m_VtxPos[0].y = pos[0].y;
	m_VtxPos[1].x = pos[1].x;	m_VtxPos[1].y = pos[1].y;
	m_VtxPos[2].x = pos[2].x;	m_VtxPos[2].y = pos[2].y;
	m_VtxPos[3].x = pos[3].x;	m_VtxPos[3].y = pos[3].y;

	if(LC_OBJ_GUI == m_Type)
	{
		m_VtxTex[0] = LCXVECTOR2(st0.x, st0.y);
		m_VtxTex[1] = LCXVECTOR2(st1.x, st0.y);
		m_VtxTex[2] = LCXVECTOR2(st1.x, st1.y);
		m_VtxTex[3] = LCXVECTOR2(st0.x, st1.y);
	}
	else if(LC_OBJ_MODEL2D == m_Type)
	{
		m_VtxTex[3] = LCXVECTOR2(st0.x, st0.y);
		m_VtxTex[2] = LCXVECTOR2(st1.x, st0.y);
		m_VtxTex[1] = LCXVECTOR2(st1.x, st1.y);
		m_VtxTex[0] = LCXVECTOR2(st0.x, st1.y);
	}

	// Color...
	if(pColor)
	{
		m_VtxDif[0] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
		m_VtxDif[1] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
		m_VtxDif[2] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
		m_VtxDif[3] = LCXCOLOR(pColor->r, pColor->g, pColor->b, pColor->a);
	}
	else
	{
		m_VtxDif[0] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
		m_VtxDif[1] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
		m_VtxDif[2] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
		m_VtxDif[3] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
	}


	glBindTexture(GL_TEXTURE_2D, nTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glVertexPointer  (3, GL_FLOAT, 0, m_VtxPos);
	glColorPointer	 (4, GL_FLOAT, 0, m_VtxDif);
	glTexCoordPointer(2, GL_FLOAT, 0, m_VtxTex);

	if(!pTexture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}


	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	return LC_OK;
}




////////////////////////////////////////////////////////////////////////////////

// Create Simple Sprite
INT LcDev_CreateSpriteS(char* sCmd
					, PLC_SPRITE_S* pOut		// Output instance
					, LC_HANDLE p1			// type
					, LC_HANDLE p2			// No Use
					, LC_HANDLE p3			// No Use
					, LC_HANDLE p4			// No Use
					 )
{
	INT			hr = 0;
	CLcSpriteS*	pObj = NULL;

	*pOut		= NULL;

	pObj = new CLcSpriteS;

	hr = pObj->Create(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}

