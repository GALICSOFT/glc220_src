// Implementation of the Sprite Collector
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <vector>
using namespace std;

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


// sprite collector
class CLcSpriteC : public ILC_SpriteC
{
protected:
	INT					m_Type;
	PLC_TEXTURE			m_pTex;


	vector<LCXVECTOR2>	m_VtxP;		// Position
	vector<LCXCOLORB >	m_VtxC;		// Color
	vector<LCXVECTOR2>	m_VtxT;		// Texture


public:
	CLcSpriteC();
	virtual ~CLcSpriteC();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();

	virtual INT		AddRect(const LC_RECTF*	pImgRc			// Image rect
						, const LC_FLOAT2*	vcTrn			// Position
						, const LC_FLOAT2*	vcScl			// Scaling
						, const LC_FLOAT2*	vcRot			// Rotation Center
						, FLOAT				fRot			// Angle(Radian)
						, const LC_COLORF*	color=NULL		// color
						);

	virtual INT		Draw();									// Draw the vertex list
	virtual void	SetTexture(PLC_TEXTURE v){ m_pTex= v;}	// Set Texture

	INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	void	Destroy();
};


/////////////////////////////////////////////////////////////////////////

CLcSpriteC::CLcSpriteC()
{
	m_Type = LC_OBJ_GUI;
	m_pTex = NULL;
}

CLcSpriteC::~CLcSpriteC()
{
	Destroy();
}


INT CLcSpriteC::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcSpriteC::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcSpriteC::Destroy()
{
	m_pTex = NULL;
	m_VtxP.clear();
	m_VtxC.clear();
	m_VtxT.clear();
}


INT CLcSpriteC::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	if(p1 == NULL)
		return LC_EFAIL;


	m_pTex = (PLC_TEXTURE)p1;
	if(0 >= m_pTex->GetId())
		return LC_EFAIL;

	if(p2)
		m_Type = *(INT*)p2;

	if(LC_OBJ_GUI != m_Type && LC_OBJ_MODEL2D != m_Type)
		m_Type = LC_OBJ_GUI;


	return LC_OK;
}


INT CLcSpriteC::AddRect(  const LC_RECTF*	pImgRc		// Image rect
						, const LC_FLOAT2*	pvcTrn		// Position
						, const LC_FLOAT2*	pvcScl		// Scaling
						, const LC_FLOAT2*	pvcRot		// Rotation Center
						, FLOAT				fRot		// Angle(Radian)
						, const LC_COLORF*	pColor		// color
						)
{
	if(NULL == m_pTex || 0 == m_pTex->GetId())
		return LC_CANNT_FIND_RESOURCE;


	FLOAT	ImgW = (FLOAT)m_pTex->GetImgW();
	FLOAT	ImgH = (FLOAT)m_pTex->GetImgH();

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


	
	if(LC_OBJ_GUI == m_Type)
	{
		tVtxTex[0] = LCXVECTOR2(st0.x, st0.y);
		tVtxTex[1] = LCXVECTOR2(st1.x, st0.y);
		tVtxTex[2] = LCXVECTOR2(st1.x, st1.y);
		tVtxTex[3] = LCXVECTOR2(st0.x, st1.y);
	}
	else if(LC_OBJ_MODEL2D== m_Type)
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


	for(int i=0; i<4; ++i)
	{
		m_VtxP.push_back(tVtxPos[i]);
		m_VtxC.push_back(tVtxCol   );
		m_VtxT.push_back(tVtxTex[i]);
	}

	return LC_OK;
}


INT CLcSpriteC::Draw()
{
	INT			nTex = m_pTex->GetId();
	INT			nTri = (INT)m_VtxP.size();
	WORD*		pIdx = LcSys_QuadIndex();

	nTri = nTri * 6/4;

	glBindTexture(GL_TEXTURE_2D, nTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer  (2, GL_FLOAT         , 0, &m_VtxP[0]);
	glColorPointer	 (4, GL_UNSIGNED_BYTE , 0, &m_VtxC[0]);
	glTexCoordPointer(2, GL_FLOAT         , 0, &m_VtxT[0]);

	glDrawElements(GL_TRIANGLES, nTri, GL_UNSIGNED_SHORT, pIdx);

	return LC_OK;
}


INT LcDev_CreateSpriteC(char* sCmd
					   , PLC_SPRITE_C* pOut								// Output instance
					   , LC_HANDLE p1									// Texture point
					   , LC_HANDLE p2
					   , LC_HANDLE p3
					   , LC_HANDLE p4
					 )
{
	INT			hr = 0;
	CLcSpriteC*	pObj = NULL;

	*pOut		= NULL;
	pObj = new CLcSpriteC;

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



