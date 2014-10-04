// Implementation of the LCX_Entity class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcAux.h>
#include <LcMath.h>
#include <LcBase.h>

#include "LcxEntity.h"

#include "LcxGeoObj.h"




// Create Entity Instance
INT LcxEnt_CreateEntity(char* sCmd
				   , PLCX_ENTITY* pOut	// Output instance
				   , LC_HANDLE p1		// User Data(Option)
				   , LC_HANDLE p2		// No Use
				   , LC_HANDLE p3		// No Use
				   , LC_HANDLE p4		// No Use
					 )
{
	INT			hr = 0;
	PLCX_ENTITY	pObj = NULL;

	*pOut = NULL;

	if(0==LcStr_Stricmp(sCmd, LCXCMD_ENTITY_GRID))
	{
		pObj = new CLcxGeoGrid;
		hr = ((CLcxGeoGrid*)pObj)->Create(p1, p2, p3, p4);
	}
	else if(0==LcStr_Stricmp(sCmd, LCXCMD_ENTITY_AXIS))
	{
		pObj = new CLcxGeoAxis;
		hr = ((CLcxGeoAxis*)pObj)->Create(p1, p2, p3, p4);
	}
	else
		return LC_CANNT_ALLOC;


	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}





////////////////////////////////////////////////////////////////////////////////
// make 2d vertex list from the drawing image


void LcxEnt_VtxFromDrawingImage( LC3D::Vx2UV* OutVtx			// Output Vertex List(Position + Texture Coord)
							   , const FLOAT ImgW				// Input Image width
							   , const FLOAT ImgH				// Input Image height
							   , const LCXRECT* pImgRc			// Input Image Rect
							   , const LCXVECTOR2* OffsetPos	// StartPosition
							   , INT	bCCW
							   )
{
	FLOAT	DrwL= 0;	// Draw Region Left
	FLOAT	DrwT= 0;	// Draw Region Top
	FLOAT	DrwW= ImgW;	// Draw Region Width
	FLOAT	DrwH= ImgH;	// Draw Region Height

	LCXVECTOR2	st0(0,0);
	LCXVECTOR2	st1(1,1);


	// Draw Region Width, Height
	DrwL = pImgRc->x;
	DrwT = pImgRc->y;
	DrwW = pImgRc->w;
	DrwH = pImgRc->h;

	st0.x = (DrwL +    0)/ImgW;
	st1.x = (DrwL + DrwW)/ImgW;
	st0.y = (ImgH - DrwT -    0)/ImgH;
	st1.y = (ImgH - DrwT - DrwH)/ImgH;


	if(OffsetPos)
		OutVtx[0].p = *OffsetPos;
	else
		OutVtx[0].p = LCXVECTOR2(   0,    0);

	//OutVtx[1].p = OutVtx[0].p + LCXVECTOR2(DrwW,    0);
	//OutVtx[2].p = OutVtx[0].p + LCXVECTOR2(DrwW, DrwH);
	//OutVtx[3].p = OutVtx[0].p + LCXVECTOR2(   0, DrwH);

	OutVtx[1].p = OutVtx[0].p;
	OutVtx[2].p = OutVtx[0].p;
	OutVtx[3].p = OutVtx[0].p;
	OutVtx[1].p+= LCXVECTOR2(DrwW,    0);
	OutVtx[2].p+= LCXVECTOR2(DrwW, DrwH);
	OutVtx[3].p+= LCXVECTOR2(   0, DrwH);


	if(bCCW)
	{
		OutVtx[3].t = LCXVECTOR2(st0.x, st0.y);
		OutVtx[2].t = LCXVECTOR2(st1.x, st0.y);
		OutVtx[1].t = LCXVECTOR2(st1.x, st1.y);
		OutVtx[0].t = LCXVECTOR2(st0.x, st1.y);		
	}
	else
	{
		OutVtx[0].t = LCXVECTOR2(st0.x, st0.y);
		OutVtx[1].t = LCXVECTOR2(st1.x, st0.y);
		OutVtx[2].t = LCXVECTOR2(st1.x, st1.y);
		OutVtx[3].t = LCXVECTOR2(st0.x, st1.y);
	}
}


void LcxEnt_VtxFromDrawingImage( LCXVECTOR2*		OutPos		// Output Position
							   , LCXVECTOR2*		OutTex		// Output Texture Coordinate
							   , const FLOAT		ImgW		// Input Image width
							   , const FLOAT		ImgH		// Input Image height
							   , const LCXRECT*		pImgRc		// Input Image Rect
							   , const LCXVECTOR2*	OffsetPos	// StartPosition
							   , INT	bCCW
							   )
{
	FLOAT	DrwL= 0;	// Draw Region Left
	FLOAT	DrwT= 0;	// Draw Region Top
	FLOAT	DrwW= ImgW;	// Draw Region Width
	FLOAT	DrwH= ImgH;	// Draw Region Height

	LCXVECTOR2	st0(0,0);
	LCXVECTOR2	st1(1,1);


	// Draw Region Width, Height
	DrwL = pImgRc->x;
	DrwT = pImgRc->y;
	DrwW = pImgRc->w;
	DrwH = pImgRc->h;


	if(OutTex)
	{
		st0.x = (DrwL +    0)/ImgW;
		st1.x = (DrwL + DrwW)/ImgW;
		st0.y = (ImgH - DrwT -    0)/ImgH;
		st1.y = (ImgH - DrwT - DrwH)/ImgH;
	}


	if(OffsetPos)
		OutPos[0] = *OffsetPos;
	else
		OutPos[0] = LCXVECTOR2(   0,    0);


	OutPos[1] = OutPos[0] + LCXVECTOR2(DrwW,    0);
	OutPos[2] = OutPos[0] + LCXVECTOR2(DrwW, DrwH);
	OutPos[3] = OutPos[0] + LCXVECTOR2(   0, DrwH);


	if(OutTex)
	{
		if(bCCW)
		{
			OutTex[3] = LCXVECTOR2(st0.x, st0.y);
			OutTex[2] = LCXVECTOR2(st1.x, st0.y);
			OutTex[1] = LCXVECTOR2(st1.x, st1.y);
			OutTex[0] = LCXVECTOR2(st0.x, st1.y);
		}
		else
		{
			OutTex[0] = LCXVECTOR2(st0.x, st0.y);
			OutTex[1] = LCXVECTOR2(st1.x, st0.y);
			OutTex[2] = LCXVECTOR2(st1.x, st1.y);
			OutTex[3] = LCXVECTOR2(st0.x, st1.y);
		}
	}
}


void LcxEnt_VtxFromDrawingImage(LCXVECTOR2*		OutPos		// Output position List. need 4 LCXVECTOR2
							, LCXVECTOR2*		OutTex		// Output Texture List. need 4 LCXVECTOR2
							, const FLOAT		ImgW		// Image Width
							, const FLOAT		ImgH		// Image Height
							, const LCXRECT*	pImgRc		// Image rect
							, const LCXVECTOR2* OffsetPos	// Position
							, const LCXVECTOR2*	pvcScl		// Scaling
							, const LCXVECTOR2*	pvcRot		// Rotation Center
							, FLOAT				fRot		// Angle(Radian)
							, INT	bCCW
							)
{
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


	if(OutTex)
	{
		st0.x = (DrwL +    0)/ImgW;
		st1.x = (DrwL + DrwW)/ImgW;
		st0.y = (ImgH - DrwT -    0)/ImgH;
		st1.y = (ImgH - DrwT - DrwH)/ImgH;
	}


	// Setup Draw Position
	if(OffsetPos)
	{
		pos[0].x = OffsetPos->x;
		pos[0].y = OffsetPos->y;
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


	// Final...
	OutPos[0] = pos[0];
	OutPos[1] = pos[1];
	OutPos[2] = pos[2];
	OutPos[3] = pos[3];


	if(OutTex)
	{

		if(bCCW)
		{
			OutTex[3] = LCXVECTOR2(st0.x, st0.y);
			OutTex[2] = LCXVECTOR2(st1.x, st0.y);
			OutTex[1] = LCXVECTOR2(st1.x, st1.y);
			OutTex[0] = LCXVECTOR2(st0.x, st1.y);
		}
		else
		{
			OutTex[0] = LCXVECTOR2(st0.x, st0.y);
			OutTex[1] = LCXVECTOR2(st1.x, st0.y);
			OutTex[2] = LCXVECTOR2(st1.x, st1.y);
			OutTex[3] = LCXVECTOR2(st0.x, st1.y);
		}
	}
}


