// Implementation of the CLcxEftPtc3d class.
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
#include "LcxEftPtc3d.h"



CLcxEftPtc3d::CLcxEftPtc3d()
{
}

CLcxEftPtc3d::~CLcxEftPtc3d()
{
	Destroy();
}


void CLcxEftPtc3d::Destroy()
{
}



INT	CLcxEftPtc3d::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	return LC_OK;
}


INT CLcxEftPtc3d::FrameMove()
{
	return LC_OK;
}


void CLcxEftPtc3d::Render()
{
}

