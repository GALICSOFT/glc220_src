// Implementation of the CLcxEftTexAni class.
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
#include "LcxEftTexAni.h"



CLcxEftTexAni::CLcxEftTexAni()
{
}

CLcxEftTexAni::~CLcxEftTexAni()
{
	Destroy();
}


void CLcxEftTexAni::Destroy()
{
}



INT	CLcxEftTexAni::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	return LC_OK;
}


INT CLcxEftTexAni::FrameMove()
{
	return LC_OK;
}


void CLcxEftTexAni::Render()
{
}

