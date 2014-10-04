// Implementation of the CLcxEffect class.
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
#include <LcBase.h>
#include <LcMath.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
  #include "../ILCX_Effect.h"
#else
  #include "../LcxEntity.h"
#endif

#include "LcxEffect.h"
#include "LcxEftPtc2d.h"
#include "LcxEftPtc3d.h"
#include "LcxEftTexAni.h"



TLCX_PRT2d::TLCX_PRT2d()
{
	idx  = 0;
	pos.x = 0.0F;	pos.y = 0.0F;
	col.r = 1.0F;	col.g = 1.0F;	col.b = 1.0F;	col.a = 1.0F;
	mag.x = 1.0F;	mag.y = 1.0F;

	Life  = 1.0F;
}

TLCX_PRT2dINF::TLCX_PRT2dINF()
: nPrtNum(0)
, bPrtRen(LCXEFT_REN_TIANGLE)
, nPrtSize(0)
, pPrtList(NULL)
, nPntL(30.F)
, pTex(NULL)
{
	IntlRect= LCXRECT	(0,0,0,0);	// boundary
	IntlPos	= LCXVECTOR2(0,0);		// Position
	IntlVel	= LCXVECTOR2(0,0);		// Velocity
	IntlAcc	= LCXVECTOR2(0,0);		// Acceleration

	IntlRot	= FLOAT		(0);		// rotation Angle
	IntlRov	= FLOAT		(0);		// rotation Angle Velocity

	IntlScl	= LCXVECTOR2(1,1);		// scale
	IntlScv	= LCXVECTOR2(0,0);		// Scale Velocity

	IntlCol	= LCXCOLOR	(1,1,1,1);	// Color
	IntlCov	= LCXCOLOR	(0,0,0,0);	// Color Velocity

	Life	= FLOAT		(1);		// Life
	Lifv	= FLOAT		(0);		// Life speed. Fade Speed

	IntlMgW	= FLOAT		(1);		// Width
	IntlMgH	= FLOAT		(1);		// Height
}

INT TLCX_PRT2d::Init(TLCX_PRT2dINF* pInf)	{	return 0;	}
INT TLCX_PRT2d::Reset(TLCX_PRT2dINF* pInf)	{	return 0;	}
INT TLCX_PRT2d::Update(TLCX_PRT2dINF* pInf)	{	return 0;	}




CLcxEffect::CLcxEffect()
{
}

CLcxEffect::~CLcxEffect()
{
	Destroy();
}


INT CLcxEffect::Query(char* sCmd, LC_HANDLE pData)	{	return LC_EFAIL;	}

INT CLcxEffect::Release()
{
	INT hr = 0;

	LC_HANDLE pOwn = NULL;
	hr = this->Query(LCQUERY_GET_OWNER, &pOwn);
	if(LC_SUCCEEDED(hr))	// if there is owner then this cannot be released
		return LC_EFAIL;


	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


INT	CLcxEffect::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	return LC_OK;
}


// Create graphic user interface.
INT LcxEnt_CreateEffect(char*     sCmd		// Command(sCmd): "2d", "3d"
				, PLCX_EFFECT* pOut		// Output data
				, LC_HANDLE     p1		// Effect  Info TLCX_Effect* Pointer
				, LC_HANDLE     p2		// No Use
				, LC_HANDLE     p3		// No Use
				, LC_HANDLE     p4		// No Use
				)
{
	INT			hr = 0;
	PLCX_EFFECT	pObj = NULL;


	*pOut = NULL;


	if(0==LcStr_Stricmp(sCmd, LCXCMD_EFT_PARTICLE2D))
	{
		pObj = new CLcxEftPtc2d;
		hr = ((CLcxEftPtc2d*)pObj)->Create(p1, p2, p3, p4);
	}
	else if(0==LcStr_Stricmp(sCmd, LCXCMD_EFT_PARTICLE3D))
	{
		pObj = new CLcxEftPtc3d;
		hr = ((CLcxEftPtc3d*)pObj)->Create(p1, p2, p3, p4);
	}
	else if(0==LcStr_Stricmp(sCmd, LCXCMD_EFT_TEXTURE_ANI))
	{
		pObj = new CLcxEftTexAni;
		hr = ((CLcxEftTexAni*)pObj)->Create(p1, p2, p3, p4);
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
