// Implementation of the LcxCam.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcMath.h>
#include <LcBase.h>

#include "LcxEntity.h"
#include "LcxCam.h"



CLcxCam::CLcxCam()
{
	m_mtViw.Identity();
	m_mtPrj.Identity();

	m_fYaw		= 0.f;
	m_fPitch	= 0.f;

	m_vcEye		= LCXVECTOR3(0,0,0);
	m_vcLook	= LCXVECTOR3(0,1,0);
	m_vcUp		= LCXVECTOR3(0,0,1);

	m_fFv		= FLOAT(LCX_PI/4.0F);
	m_fAs		= 320.F/240.F;
	m_fNr		= 1.0F;
	m_fFr		= 5000.0F;
}


CLcxCam::~CLcxCam()
{
	Destroy();
}


INT CLcxCam::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxCam::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}



void CLcxCam::Destroy()
{
}


INT CLcxCam::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT scnW, scnH;
	LcVar_WindowSize(&scnW, &scnH);

	m_fAs = FLOAT(scnW)/FLOAT(scnH);

	return LC_OK;
}


void CLcxCam::GetParamView(LC_FLOAT3* vcEye, LC_FLOAT3* vcLook, LC_FLOAT3* vcUp)
{
	if(vcEye)
		memcpy(vcEye , &m_vcEye	, sizeof(FLOAT));

	if(vcLook)
		memcpy(vcLook, &m_vcLook, sizeof(FLOAT));

	if(vcUp)
		memcpy(vcUp  , &m_vcUp	, sizeof(FLOAT));
}


void CLcxCam::GetParamProj(FLOAT* fFov, FLOAT* fScnW, FLOAT* fScnH, FLOAT* fNear, FLOAT* fFar)
{
	INT	ScnW=0, ScnH=0;
	LcVar_WindowSize(&ScnW, &ScnH);

	if(fFov) *fFov	= m_fFv	;
	if(fScnW)*fScnW	= FLOAT(ScnW);
	if(fScnH)*fScnH	= FLOAT(ScnH);
	if(fNear)*fNear	= m_fNr	;
	if(fFar) *fFar	= m_fFr	;
}


