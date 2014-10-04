// Implementation of the CLcSprite.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <vector>

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

#include "LcBase.h"
#include "LcAux.h"
#include "LcSprite.h"


CLcSprite::CLcSprite()
{
	m_Type	= LC_OBJ_GUI;
	m_pTex	= NULL;

	m_nQuad	= 0;
	memset(m_VtxP, 0, LC_MAXRC * sizeof(LCXVECTOR2));
	memset(m_VtxC, 0, LC_MAXRC * sizeof(LCXCOLORB ));
	memset(m_VtxT, 0, LC_MAXRC * sizeof(LCXVECTOR2));

	m_mtWld.Identity()	;
	m_mtViw.Identity()	;
	m_mtPrj.Identity()	;

	m_ExtWld = NULL;
	m_ExtViw = NULL;
	m_ExtPrj = NULL;

	m_mtPrjO.Identity();
	m_mtViwO.Identity();

	m_BndR	 = 0;
	m_BndC	 = LCXVECTOR2(0,0);

	m_BldSrc = GL_SRC_ALPHA;
	m_BldDst = GL_ONE_MINUS_SRC_ALPHA;
	m_bCull	 = LC_TRUE;
	m_ScnW	 = 0;
	m_ScnH	 = 0;
	m_posZ	 = 0.0F;

	m_nCntBgn=0;
}


CLcSprite::~CLcSprite()
{
	Destroy();
}


INT CLcSprite::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcSprite::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcSprite::Destroy()
{
	m_pTex = NULL;
	m_nQuad	= 0;
}


INT CLcSprite::Create(LC_HANDLE p1, LC_HANDLE, LC_HANDLE, LC_HANDLE)
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

	else if(LC_OBJ_MODEL2D== m_Type)
	{
		// Ortho Center
		m_mtPrj.SetupOrthoGl( l, r
							, b, t
							, n, f);
	}


	m_BndR	= FLOAT( lc_max(m_ScnW, m_ScnH) );
	m_BndR *= 1.25F;
	m_BndC	= LCXVECTOR2(m_ScnW, m_ScnH) * 0.5f;

	return LC_OK;
}


////////////////////////////////////////////////////////////////////////////////

INT CLcSprite::Begin(UINT dMode)
{
	++m_nCntBgn;

	if(1 != m_nCntBgn)
		return LC_OK;

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


	if(NULL == m_ExtPrj)
	{
		if(LC_OBJ_GUI == m_Type)
		{
			// Ortho Left Top
			m_mtPrj.SetupOrthoGl( 0.F, 2*r
								, 2*t, 0.F
								, n, f);
		}

		else if(LC_OBJ_MODEL2D== m_Type)
		{
			// Ortho Center
			m_mtPrj.SetupOrthoGl( l, r
								, b, t
								, n, f);
		}
	}


	m_BndR	= FLOAT( lc_max(m_ScnW, m_ScnH) );
	m_BndR *= 1.25F;
	m_BndC	= LCXVECTOR2(m_ScnW, m_ScnH) * 0.5f;


	// load to the projection transform
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if(NULL == m_ExtPrj)
		glLoadMatrixf((GLfloat*)&m_mtPrj);
	else
		glLoadMatrixf((GLfloat*)m_ExtPrj);




	////////////////////////////////////////////////////////////////////////////
	//

	LCXMATRIX  mtOWV;
	LCXMATRIX  mtOrt;
	LcSys_OrientMatrix(&mtOrt, m_Type);	// Orient matrix;

	// setup the model-view projection
	if(NULL == m_ExtViw)
	{
		// Setup the View Matrix
		m_mtViw.Identity();
		mtOWV = mtOrt;	//mtOWV = mtOrt * m_mtViw;
	}
	else
	{
		mtOWV = mtOrt * *m_ExtViw;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat*)&mtOWV);


	glEnable(GL_SCISSOR_TEST);
	glClearDepthf(1.0F);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 4.0F/255.0F);


	glDisable(GL_CULL_FACE);
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
	glDisableClientState(GL_COLOR_ARRAY);

	// enable vertex and texture coordinate buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	return LC_OK;
}


INT CLcSprite::End(UINT dForce)
{
	if(dForce)
		m_nCntBgn = 1;


	//deccrease the count
	--m_nCntBgn;

	if(0 != m_nCntBgn)
		return LC_OK;

	DrawFlush();

	//glDepthMask(GL_TRUE);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// restore old values
	glMatrixMode(GL_PROJECTION);	glLoadMatrixf((GLfloat*)&m_mtPrjO);
	glMatrixMode(GL_MODELVIEW);		glLoadMatrixf((GLfloat*)&m_mtViwO);

	m_BldSrc= GL_SRC_ALPHA;
	m_BldDst= GL_ONE_MINUS_SRC_ALPHA;
	m_bCull = LC_TRUE;

	return LC_OK;
}


////////////////////////////////////////////////////////////////////////////////


INT CLcSprite::SetTransform(INT nTM, const LC_HANDLE v)
{
	LCXMATRIX* pmtWld = &m_mtWld;
	LCXMATRIX* pmtViw = &m_mtViw;
	LCXMATRIX* pmtPrj = &m_mtPrj;


	DrawFlush();


	if(LC_TS_WORLD_IDENTITY<= nTM && nTM <=LC_TS_PROJ_IDENTITY)
	{
		if      (LC_TS_WORLD_IDENTITY== nTM)	m_mtWld.Identity();
		else if	(LC_TS_VIEW_IDENTITY == nTM)	m_mtViw.Identity();
		else if	(LC_TS_PROJ_IDENTITY == nTM)	m_mtPrj.Identity();
	}
	else
	{
		if		(LC_TS_WORLD== nTM)
		{
			if(v)	memcpy(&m_mtWld, v, sizeof(LCXMATRIX));
			else	m_mtWld.Identity();
		}
		else if	(LC_TS_VIEW == nTM)
		{
			if(v)	memcpy(&m_mtViw, v, sizeof(LCXMATRIX));
			else	m_mtViw.Identity();
		}
		else if	(LC_TS_PROJ == nTM)
		{
			if(v)	memcpy(&m_mtPrj, v, sizeof(LCXMATRIX));
			else	m_mtPrj.Identity();
		}
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

		// is Rendering?
		if(m_nCntBgn)
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf((GLfloat*)&mtWV);
		}
	}
	else if(LC_TS_PROJ== nTM || LC_TS_PROJ_EXT== nTM)
	{
		// is Rendering?
		if(m_nCntBgn)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf((FLOAT*)pmtPrj);
			glMatrixMode(GL_MODELVIEW);
		}
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


INT CLcSprite::GetTransform(INT nTM, LC_HANDLE v)
{
	if		(LC_TS_WORLD== nTM)	memcpy(v, &m_mtWld, sizeof(LCXMATRIX));
	else if	(LC_TS_PROJ == nTM)	memcpy(v, &m_mtPrj, sizeof(LCXMATRIX));
	else if	(LC_TS_VIEW == nTM)	memcpy(v, &m_mtViw, sizeof(LCXMATRIX));
	else	return LC_EFAIL;

	return LC_OK;
}


INT CLcSprite::SetRenderState(INT ns, INT v)
{
	INT hr = LC_OK;

	// if the render state changed then draw and flush for new data
	if(LC_RS_BLEND_SRC == ns)
	{
		if(v == m_BldSrc)
			return LC_OK;

		DrawFlush();
		m_BldSrc= v;
		glBlendFunc(m_BldSrc, m_BldDst);
	}
	else if(LC_RS_BLEND_DEST == ns)
	{
		if(v == m_BldDst)
			return LC_OK;

		DrawFlush();
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
		if(z == m_posZ)
			return LC_OK;

		DrawFlush();
		m_posZ = z;
		//LOGI("LcSprite::SetRenderState::ZBIAS:: changed the z value: %f\n", m_posZ);
	}

	else
		return LC_EFAIL;

	return hr;
}



////////////////////////////////////////////////////////////////////////////////

INT CLcSprite::Draw(PLC_TEXTURE       pTex		// LC_Texture Pointer
					, const LC_RECTF*  pImgRc		// Image rect
					, const LC_FLOAT2* pvcTrn		// Position
					, const LC_COLORF* pColor		// color
					)
{
	FLOAT	ImgW = 0;
	FLOAT	ImgH = 0;
	UINT	nTex = 0;

	if(NULL == pTex)
		return LC_CANNT_FIND_RESOURCE;


	ImgW = (FLOAT)pTex->GetImgW();
	ImgH = (FLOAT)pTex->GetImgH();
	nTex = pTex->GetId();

	if(0==nTex)
		return LC_CANNT_FIND_RESOURCE;



	FLOAT	DrwL=  0;	// Draw Region Left
	FLOAT	DrwT=  0;	// Draw Region Top
	FLOAT	DrwW=ImgW;	// Draw Region Width
	FLOAT	DrwH=ImgH;	// Draw Region Height

	LCXVECTOR2	pos[4];
	LCXVECTOR2	st0(0,0);
	LCXVECTOR2	st1(1,1);


	// Image area setting.
	if(pImgRc)
	{
		DrwL = pImgRc->x;
		DrwT = pImgRc->y;
		DrwW = pImgRc->w;
		DrwH = pImgRc->h;
	}


	// to set ST of Texture corresponding to the image
	if(DrwW > ImgW)		DrwW = ImgW;
	if(DrwH > ImgH)		DrwH = ImgH;

	st0.x = (DrwL +    0)/ImgW;
	st1.x = (DrwL + DrwW)/ImgW;
	st0.y = (ImgH - DrwT -    0)/ImgH;
	st1.y = (ImgH - DrwT - DrwH)/ImgH;



	// Set a location on the screen
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




	//Finally, generate the 4 vertices and set ST of color texture
	LCXVECTOR2	tVtxPos[4];	// Position
	LCXVECTOR2	tVtxTex[4];	// Texture Coordinate
	LCXCOLORB	tVtxCol((UINT)0XFFFFFFFF);

	tVtxPos[0] = LCXVECTOR2(pos[0]);
	tVtxPos[1] = LCXVECTOR2(pos[1]);
	tVtxPos[2] = LCXVECTOR2(pos[2]);
	tVtxPos[3] = LCXVECTOR2(pos[3]);


	// Is in Screen?
	FLOAT r = lc_max(DrwW, DrwH);
	LCXVECTOR2 centre = pos[0] + LCXVECTOR2(DrwW, DrwH ) * 0.5F;
	if(m_bCull && LC_FAILED( LCXCollisionCircle2Circle(m_BndC, centre, m_BndR, r )))
		return 0;


	// Final...
//	tVtxPos[0] = LCXVECTOR2(tVtxPos[0].y, m_ScnH- tVtxPos[0].x);
//	tVtxPos[1] = LCXVECTOR2(tVtxPos[1].y, m_ScnH- tVtxPos[1].x);
//	tVtxPos[2] = LCXVECTOR2(tVtxPos[2].y, m_ScnH- tVtxPos[2].x);
//	tVtxPos[3] = LCXVECTOR2(tVtxPos[3].y, m_ScnH- tVtxPos[3].x);

	if(LC_OBJ_GUI == m_Type)
	{
		tVtxTex[0] = LCXVECTOR2(st0.x, st0.y);
		tVtxTex[1] = LCXVECTOR2(st1.x, st0.y);
		tVtxTex[2] = LCXVECTOR2(st1.x, st1.y);
		tVtxTex[3] = LCXVECTOR2(st0.x, st1.y);
	}
	else if(LC_OBJ_MODEL2D == m_Type)
	{
		tVtxTex[3] = LCXVECTOR2(st0.x, st0.y);
		tVtxTex[2] = LCXVECTOR2(st1.x, st0.y);
		tVtxTex[1] = LCXVECTOR2(st1.x, st1.y);
		tVtxTex[0] = LCXVECTOR2(st0.x, st1.y);
	}

	// color
	if(pColor)
	{
		UINT d = LCXCOLOR(*pColor);
		tVtxCol = d;
	}


	// it the input texture is not equal to current texture then draw and flush.
	if( (m_pTex && m_pTex != pTex) ||
		m_nQuad >= LC_MAXRC)
		DrawFlush();


	// add to Vertex List
	m_pTex = pTex;
	AddQuad(tVtxPos, tVtxTex, tVtxCol);


	return LC_OK;
}


INT CLcSprite::DrawScaling( PLC_TEXTURE	pTex	// LC_Texture Pointer
						, const LC_RECTF*	pImgRc	// Image rect
						, const LC_FLOAT2*	vcTrn	// Position
						, const LC_FLOAT2*	vcScl	// Scaling
						, const LC_COLORF*	color	// color
						)
{
	return DrawEx(pTex, pImgRc, vcTrn, vcScl, NULL, 0, color);
}



INT CLcSprite::DrawEx(PLC_TEXTURE     pTex		// LC_Texture Pointer
					, const LC_RECTF*  pImgRc		// Image rect
					, const LC_FLOAT2* pvcTrn		// Position
					, const LC_FLOAT2* pvcScl		// Scaling
					, const LC_FLOAT2* pvcRot		// Rotation Center
					, FLOAT            fRot			// Angle(Radian)
					, const LC_COLORF* pColor		// color
					)
{
	FLOAT	ImgW = 0;
	FLOAT	ImgH = 0;
	UINT	nTex = 0;

	if(NULL == pTex)
		return LC_CANNT_FIND_RESOURCE;


	ImgW = (FLOAT)pTex->GetImgW();
	ImgH = (FLOAT)pTex->GetImgH();
	nTex = pTex->GetId();

	if(0==nTex)
		return LC_CANNT_FIND_RESOURCE;



	FLOAT	DrwL=  0;	// Draw Region Left
	FLOAT	DrwT=  0;	// Draw Region Top
	FLOAT	DrwW=ImgW;	// Draw Region Width
	FLOAT	DrwH=ImgH;	// Draw Region Height

	LCXVECTOR2	pos[4];
	LCXVECTOR2	st0(0,0);
	LCXVECTOR2	st1(1,1);


	// Final Data
	LCXVECTOR2	tVtxPos[4];	// Position
	LCXVECTOR2	tVtxTex[4];	// Texture Coordinate
	LCXCOLORB	tVtxCol((UINT)0XFFFFFFFF);


	// Draw Region Width, Height
	if(pImgRc)
	{
		DrwL = pImgRc->x;
		DrwT = pImgRc->y;
		DrwW = pImgRc->w;
		DrwH = pImgRc->h;
	}


	// Setup UV
	if(DrwW > ImgW)		DrwW = ImgW;
	if(DrwH > ImgH)		DrwH = ImgH;

	st0.x = (DrwL +    0)/ImgW;
	st1.x = (DrwL + DrwW)/ImgW;
	st0.y = (ImgH - DrwT -    0)/ImgH;
	st1.y = (ImgH - DrwT - DrwH)/ImgH;



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
	if(NULL == pvcScl)
	{
		pos[1] = pos[0] + LCXVECTOR2(DrwW,     0);
		pos[2] = pos[0] + LCXVECTOR2(DrwW, DrwH);
		pos[3] = pos[0] + LCXVECTOR2(    0, DrwH);
	}

	else
	{
		FLOAT	fSclX = pvcScl->x;
		FLOAT	fSclY = pvcScl->y;

		if(fSclX>=0)
		{
			pos[1].x = pos[0].x + DrwW * fSclX;
			pos[2].x = pos[1].x;
			pos[3].x = pos[0].x;
		}
		else
		{
			pos[1].x = pos[0].x;
			pos[2].x = pos[1].x;
			pos[0].x = pos[1].x - DrwW * fSclX;
			pos[3].x = pos[0].x;
		}


		if(fSclY>=0)
		{
			pos[1].y = pos[0].y;
			pos[3].y = pos[0].y + DrwH * fSclY;
			pos[2].y = pos[3].y;
		}
		else
		{
			pos[3].y = pos[0].y;
			pos[2].y = pos[3].y;
			pos[0].y = pos[3].y - DrwH * fSclY;
			pos[1].y = pos[0].y;
		}
	}


	// app rotation
	if(pvcRot)
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

			tVtxPos[i].x = (vcR.x + d.x);
			tVtxPos[i].y = (vcR.y + d.y);
		}
	}
	else
	{
		tVtxPos[0] = LCXVECTOR2(pos[0]);
		tVtxPos[1] = LCXVECTOR2(pos[1]);
		tVtxPos[2] = LCXVECTOR2(pos[2]);
		tVtxPos[3] = LCXVECTOR2(pos[3]);
	}


	// Is in Screen?
	LCXVECTOR2 vcMin;
	LCXVECTOR2 vcMax;
	LCXVec2MakeBoundingBox(&vcMin, &vcMax, tVtxPos, 4);

	//if(NULL == pvcScl || (0 <= pvcScl->x &&  0 <= pvcScl->y))
	//{
	//	vcMin = tVtxPos[0];
	//	vcMax = tVtxPos[2];
	//}
	//else
	//{
	//	LCXVec2MakeBoundingBox(&vcMin, &vcMax, tVtxPos, 4);
	//}


	// Is in Screen?
	FLOAT r = lc_max( (vcMax.x - vcMin.x) , (vcMax.y - vcMin.y) );
	LCXVECTOR2 centre = (vcMin+ vcMax) * 0.5F;
	if(m_bCull && LC_FAILED( LCXCollisionCircle2Circle(m_BndC, centre, m_BndR, r )))
		return 0;


	// Final...
//	tVtxPos[0] = LCXVECTOR2(tVtxPos[0].y, m_ScnH- tVtxPos[0].x);
//	tVtxPos[1] = LCXVECTOR2(tVtxPos[1].y, m_ScnH- tVtxPos[1].x);
//	tVtxPos[2] = LCXVECTOR2(tVtxPos[2].y, m_ScnH- tVtxPos[2].x);
//	tVtxPos[3] = LCXVECTOR2(tVtxPos[3].y, m_ScnH- tVtxPos[3].x);
	
	if(LC_OBJ_GUI == m_Type)
	{
		tVtxTex[0] = LCXVECTOR2(st0.x, st0.y);
		tVtxTex[1] = LCXVECTOR2(st1.x, st0.y);
		tVtxTex[2] = LCXVECTOR2(st1.x, st1.y);
		tVtxTex[3] = LCXVECTOR2(st0.x, st1.y);
	}
	else if(LC_OBJ_MODEL2D == m_Type)
	{
		tVtxTex[3] = LCXVECTOR2(st0.x, st0.y);
		tVtxTex[2] = LCXVECTOR2(st1.x, st0.y);
		tVtxTex[1] = LCXVECTOR2(st1.x, st1.y);
		tVtxTex[0] = LCXVECTOR2(st0.x, st1.y);
	}

	// color
	if(pColor)
	{
		UINT d = LCXCOLOR(*pColor);
		tVtxCol = d;
	}



	// it the input texture is not equal to current texture then draw and flush.
	if( (m_pTex && m_pTex != pTex) ||
		m_nQuad >= LC_MAXRC)
		DrawFlush();


	// add to Vertex List
	m_pTex = pTex;
	AddQuad(tVtxPos, tVtxTex, tVtxCol);

	return LC_OK;
}



INT CLcSprite::DrawFlush()
{
	if(NULL == m_pTex || 0 >= m_nQuad || 0 >= m_nCntBgn)
		return LC_EFAIL;


	INT			nTex = m_pTex->GetId();
	INT			nTri = m_nQuad * 6;
	WORD*		pIdx = LcSys_QuadIndex();

//	LCXMATRIX	mtWldViw = m_mtWld*m_mtViw;		// OpenGL is a column-based matrix. but it's processing is same  to DirectX. Matrix multiplication have to be multiply left to right as the world * view * projection transformation process.

	glBindTexture(GL_TEXTURE_2D, nTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


//	glMatrixMode(GL_PROJECTION);
//	glPushMatrix();
//		glLoadMatrixf((FLOAT*)&m_mtPrj);
//
//		glMatrixMode(GL_MODELVIEW);
//		glPushMatrix();
//			glLoadMatrixf((FLOAT*)&mtWldViw);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer  (3, GL_FLOAT         , 0, &m_VtxP[0]);
			glColorPointer	 (4, GL_UNSIGNED_BYTE , 0, &m_VtxC[0]);
			glTexCoordPointer(2, GL_FLOAT         , 0, &m_VtxT[0]);

			glDrawElements(GL_TRIANGLES, nTri, GL_UNSIGNED_SHORT, pIdx);

//		glPopMatrix();
//
//	glPopMatrix();


	m_nQuad = 0;
	m_pTex = NULL;

	return LC_OK;
}


void CLcSprite::AddQuad(LCXVECTOR2* p, LCXVECTOR2* t, LCXCOLORB c)		// get an image area of a rectangle and generate 6 vertices..
{
	//TLAYER_QUAD	vtx[6];

	//vtx[0].p = p[0];	vtx[0].t = t[0];
	//vtx[1].p = p[1];	vtx[1].t = t[1];
	//vtx[2].p = p[2];	vtx[2].t = t[2];
	//vtx[3].p = p[0];	vtx[3].t = t[0];
	//vtx[4].p = p[2];	vtx[4].t = t[2];
	//vtx[5].p = p[3];	vtx[5].t = t[3];

	//for(int i=0; i<6; ++i)
	//{
	//	m_VtxP.push_back(vtx[i].p);
	//	m_VtxC.push_back(c);
	//	m_VtxT.push_back(vtx[i].t);
	//}

	int nIdx = m_nQuad * 4;

	// replace the list data
	for(int i=0; i<4; ++i)
	{
		m_VtxP[nIdx+i].x = p[i].x;
		m_VtxP[nIdx+i].y = p[i].y;
		m_VtxP[nIdx+i].z = m_posZ;
		m_VtxC[nIdx+i] = c;
		m_VtxT[nIdx+i] = t[i];
	}

	++m_nQuad;
}




////////////////////////////////////////////////////////////////////////////////

// Create Sprite
INT LcDev_CreateSprite(char* sCmd
					, PLC_SPRITE* pOut	// Output instance
					, LC_HANDLE p1			// No Use
					, LC_HANDLE p2			// No Use
					, LC_HANDLE p3			// No Use
					, LC_HANDLE p4			// No Use
					 )
{
	INT				hr = 0;
	CLcSprite*	pObj = NULL;

	*pOut		= NULL;
	pObj = new CLcSprite;

	hr = pObj->Create(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}


