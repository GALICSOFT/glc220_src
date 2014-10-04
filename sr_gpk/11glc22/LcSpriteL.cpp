// Implementation of the CLcSpriteL.
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


class CLcSpriteL : public ILC_SpriteL
{
protected:
	INT			m_Type		;									// Sprite Type

	LCXMATRIX	m_mtWld		;									// world Matrix
	LCXMATRIX	m_mtViw		;									// View Matrix
	LCXMATRIX	m_mtPrj		;									// Projection Matrix

	LCXVECTOR3	m_vcRot		;									// Rotation Center
	LCXVECTOR2	m_vcScl		;									// Scaling
	LCXVECTOR2	m_vcTrn		;									// Layer position

	LCXMATRIX3X3 m_mtScl	;
	LCXMATRIX3X3 m_mtRot	;
	LCXMATRIX3X3 m_mtTrn	;

	FLOAT		m_fNr		;									// Near
	FLOAT		m_fFr		;									// Far

	INT			m_BldSrc	;									// source blending option
	INT			m_BldDst	;									// dest blending option
	INT			m_nCntBgn	;									// Save the Count of Call begin() function.

	LCXCOLOR	m_VtxDif[4]	;									// Default diffuse color

public:
	CLcSpriteL();
	virtual ~CLcSpriteL();

	virtual	INT		Query(char* sCmd, LC_HANDLE pData);			// inherit from the ILC_Object
	virtual	INT		Release();									// inherit from the ILC_Object

	virtual INT		GetType() {	return m_Type;	}

	virtual INT		Begin(UINT dMode=LC_OBJ_SPRITE);			// Save 3d environment before the execution of Sprite.
	virtual INT		End(UINT dForce=LC_FALSE);					// Reload saved 3d environment.

	virtual INT		Draw(PLC_TEXTURE pTex, FLOAT* vtxPos		// simple vertex draw
						, FLOAT* vtxTex, FLOAT* vColor= NULL);	//

	virtual	INT		SetRenderState(INT nState, INT v);			// Redering Machine State

	virtual	INT		SetTransform(INT nTM, const LC_HANDLE v);
	virtual	INT		GetTransform(INT nTM, LC_HANDLE v);

	virtual INT		SetRotation (const LC_FLOAT3* v);			// setup the layer Rotation Centre(x,y) and angle(radian:z)
	virtual INT		SetScaling  (const LC_FLOAT2* v);			// setup the layer scaling
	virtual INT		SetPosition (const LC_FLOAT2* v);			// setup the layer position

	virtual const LC_FLOAT3* GetRotation(){return &m_vcRot; };	// get Rotation Centre(x,y) and angle(radian:z)
	virtual const LC_FLOAT2* GetScaling (){return &m_vcScl; };	// get scaling of layer
	virtual const LC_FLOAT2* GetPosition(){return &m_vcTrn; };	// get position of layer

	INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
protected:
	void	Destroy();
};


////////////////////////////////////////////////////////////////////////////////


CLcSpriteL::CLcSpriteL()
{
	m_VtxDif[0] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
	m_VtxDif[1] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
	m_VtxDif[2] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
	m_VtxDif[3] = LCXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);

	m_Type	= LC_OBJ_GUI;

	m_mtWld.Identity()	;
	m_mtViw.Identity()	;
	m_mtPrj.Identity()	;

	m_vcRot = LCXVECTOR3(0.0F, 0.0F, 0.0F);
	m_vcScl = LCXVECTOR2(1.0F, 1.0F      );
	m_vcTrn = LCXVECTOR2(0.0F, 0.0F      );

	m_mtScl.Identity()	;
	m_mtRot.Identity()	;
	m_mtTrn.Identity()	;

	m_fNr	= +4096.0F;
	m_fFr	= -4096.0F;

	m_BldSrc = GL_SRC_ALPHA;
	m_BldDst = GL_ONE_MINUS_SRC_ALPHA;
	m_nCntBgn= 0;
}


CLcSpriteL::~CLcSpriteL()
{
	Destroy();
}


INT CLcSpriteL::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcSpriteL::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcSpriteL::Destroy()
{
}


INT CLcSpriteL::Create(LC_HANDLE p1, LC_HANDLE, LC_HANDLE, LC_HANDLE)
{
	INT	ScnW=0, ScnH=0;
	LcVar_WindowSize(&ScnW, &ScnH);

	if(p1)
		m_Type = *(INT*)p1;	// Setup the type


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

	return LC_OK;
}


////////////////////////////////////////////////////////////////////////////////

INT CLcSpriteL::Begin(UINT dMode)
{
	++m_nCntBgn;

	if(1 != m_nCntBgn)
		return LC_OK;


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf((GLfloat*)&m_mtPrj);


	////////////////////////////////////////////////////////////////////////////
	//

	LCXMATRIX  mtOWV;
	LCXMATRIX  mtOrt;
	LcSys_OrientMatrix(&mtOrt, m_Type);	// Orient matrix;

	// setup the model-view projection
	// Setup the View Matrix
	mtOWV = mtOrt * m_mtViw;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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


	// normal vector disable
	glDisableClientState(GL_NORMAL_ARRAY);


	// enable vertex and texture coordinate buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	return LC_OK;
}


INT CLcSpriteL::End(UINT dForce)
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

	return LC_OK;
}


INT CLcSpriteL::Draw(PLC_TEXTURE pTex, FLOAT* vtxPos, FLOAT* vtxTex, FLOAT* vtxColor)
{
	UINT	nTex = 0;

	LCXCOLOR*	vtxDif = (vtxColor)? (LCXCOLOR*)vtxColor : &m_VtxDif[0];

	if(pTex)
		nTex = pTex->GetId();


	if(nTex)
	{
		glBindTexture(GL_TEXTURE_2D, nTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexCoordPointer(2, GL_FLOAT, 0, vtxTex);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}


	glVertexPointer  (2, GL_FLOAT, 0, vtxPos);
	glColorPointer	 (4, GL_FLOAT, 0, vtxDif);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	return LC_OK;
}



////////////////////////////////////////////////////////////////////////////////

INT CLcSpriteL::SetRenderState(INT ns, INT v)
{
	INT hr = LC_OK;

	if(LC_RS_BLEND_SRC == ns)
	{
		if(v == m_BldSrc)
			return LC_OK;

		m_BldSrc= v;
		glBlendFunc(m_BldSrc, m_BldDst);
	}
	else if(LC_RS_BLEND_DEST == ns)
	{
		if(v == m_BldDst)
			return LC_OK;

		m_BldDst= v;
		glBlendFunc(m_BldSrc, m_BldDst);
	}
	else
		return LC_EFAIL;

	return hr;
}


////////////////////////////////////////////////////////////////////////////////


INT CLcSpriteL::SetTransform(INT nTM, const LC_HANDLE v)
{
	LCXMATRIX* pmtWld = &m_mtWld;
	LCXMATRIX* pmtViw = &m_mtViw;
	LCXMATRIX* pmtPrj = &m_mtPrj;

	switch(nTM)
	{
		case LC_TS_WORLD_IDENTITY:
			m_mtWld.Identity();
			break;

		case LC_TS_VIEW_IDENTITY:
			m_mtViw.Identity();
			break;

		case LC_TS_PROJ_IDENTITY:
			m_mtPrj.Identity();
			break;

		case LC_TS_WORLD:
			if(v)
				memcpy(&m_mtWld, v, sizeof(LCXMATRIX));
			else
				m_mtWld.Identity();
			break;

		case LC_TS_VIEW:
			if(v)
				memcpy(&m_mtViw, v, sizeof(LCXMATRIX));
			else
				m_mtViw.Identity();
			break;

		case LC_TS_PROJ:
			if(v)
				memcpy(&m_mtPrj, v, sizeof(LCXMATRIX));
			else
				m_mtPrj.Identity();
			break;
	}


	LCXMATRIX  mtOWV;
	LCXMATRIX  mtOrt;
	LCXMATRIX  mtWV = *pmtWld * *pmtViw;

	LcSys_OrientMatrix(&mtOrt, m_Type);	// Orient matrix;

	switch(nTM)
	{
		case LC_TS_WORLD:
		case LC_TS_WORLD_IDENTITY:
		case LC_TS_VIEW:
		case LC_TS_VIEW_IDENTITY:
			mtOWV = mtOrt * mtWV;

			// is Rendering?
			if(m_nCntBgn)
			{
				glMatrixMode(GL_MODELVIEW);
				glLoadMatrixf((GLfloat*)&mtWV);
			}
			break;

		case LC_TS_PROJ:
		case LC_TS_PROJ_EXT:

			// is Rendering?
			if(m_nCntBgn)
			{
				glMatrixMode(GL_PROJECTION);
				glLoadMatrixf((FLOAT*)pmtPrj);
				glMatrixMode(GL_MODELVIEW);
			}
			break;
	}

	return LC_OK;
}


INT CLcSpriteL::GetTransform(INT nTM, LC_HANDLE v)
{
	if		(LC_TS_WORLD== nTM)	memcpy(v, &m_mtWld, sizeof(LCXMATRIX));
	else if	(LC_TS_PROJ == nTM)	memcpy(v, &m_mtPrj, sizeof(LCXMATRIX));
	else if	(LC_TS_VIEW == nTM)	memcpy(v, &m_mtViw, sizeof(LCXMATRIX));
	else	return LC_EFAIL;

	return LC_OK;
}


INT CLcSpriteL::SetRotation(const LC_FLOAT3* v)
{
	if(m_vcRot == *v )
		return LC_OK;

	m_vcRot = *v;

	FLOAT COS = (FLOAT)LCXMathCos(m_vcRot.z);
	FLOAT SIN = (FLOAT)LCXMathSin(m_vcRot.z);

	LCXMATRIX3X3 mtRot(COS, -SIN,  0,  SIN, COS, 0,           0,          0, 1);
	LCXMATRIX3X3 mtTrI(  1,    0,  0,    0,   1, 0,  -m_vcRot.x, -m_vcRot.y, 1);
	LCXMATRIX3X3 mtTrO(  1,    0,  0,    0,   1, 0,   m_vcRot.x,  m_vcRot.y, 1);

	m_mtRot = (mtTrI * mtRot * mtTrO);

	LCXMATRIX3X3 mtWld = m_mtScl * m_mtRot * m_mtTrn;

	LCXMatrix4X4From3X3Tm(&m_mtWld, &mtWld);

	return LC_OK;
}


INT CLcSpriteL::SetScaling(const LC_FLOAT2* v)
{
	if(m_vcScl == *v )
		return LC_OK;

	m_vcScl = *v;

	m_mtScl._11 =  m_vcScl.x;
	m_mtScl._22 =  m_vcScl.y;

	LCXMATRIX3X3 mtWld = m_mtScl * m_mtRot * m_mtTrn;

	LCXMatrix4X4From3X3Tm(&m_mtWld, &mtWld);

	return LC_OK;
}


INT CLcSpriteL::SetPosition(const LC_FLOAT2* v)
{
	if(m_vcTrn == *v )
		return LC_OK;

	m_vcTrn	= *v;

	m_mtTrn._31 = m_vcTrn.x;
	m_mtTrn._32 = m_vcTrn.y;

	LCXMATRIX3X3 mtWld = m_mtScl * m_mtRot * m_mtTrn;

	LCXMatrix4X4From3X3Tm(&m_mtWld, &mtWld);

	return LC_OK;
}



////////////////////////////////////////////////////////////////////////////////

// Create Sprite Layer
INT LcDev_CreateSpriteL(char* sCmd
					, PLC_SPRITE_L* pOut	// Output instance
					, LC_HANDLE p1			// No Use
					, LC_HANDLE p2			// No Use
					, LC_HANDLE p3			// No Use
					, LC_HANDLE p4			// No Use
					 )
{
	INT				hr = 0;
	CLcSpriteL*	pObj = NULL;

	*pOut		= NULL;
	pObj = new CLcSpriteL;

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


