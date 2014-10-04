// Implementation of the LCX_MoSqc3d.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1200
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcMath.h>
#include <LcBase.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
  #include "../ILCX_MoSqc.h"

#else
  #include "../LcxEntity.h"

#endif


#include "LcxMoSqc.h"
#include "LcxMoSqc3d.h"



CLcxMoSqc3d::CLcxMoSqc3d()
{
	m_dTimeC = 0.F;
}


CLcxMoSqc3d::~CLcxMoSqc3d()
{
	Destroy();
}



void CLcxMoSqc3d::Destroy()
{
	m_vSqc.clear();
}


INT CLcxMoSqc3d::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	return LC_EFAIL;
}


INT CLcxMoSqc3d::FrameMove()
{
	return LC_OK;
}


INT CLcxMoSqc3d::SetAttrib(UINT dCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxMoSqc3d::GetAttrib(UINT dCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}







