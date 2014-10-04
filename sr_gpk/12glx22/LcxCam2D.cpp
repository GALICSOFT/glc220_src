// Implementation of the LcxCam2D.
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
#include <LcBase.h>
#include <LcAux.h>

#include "LcxEntity.h"

#include "LcxCam.h"
#include "LcxCam2D.h"


CLcxCam2D::CLcxCam2D()
{
	m_mtViw.Identity();
	m_mtPrj.Identity();

	m_fYaw		= 0.f;
	m_fPitch	= 0.f;

	m_vcEye		= LCXVECTOR3(0,  0,  0);
	m_vcLook	= LCXVECTOR3(0,  0,  1);
	m_vcUp		= LCXVECTOR3(0,  1,  0);

	//m_fNr	 = +4096.0F;	// Near
	//m_fFr	 = -4096.0F;	// Far

	m_Scl		= 1.F;
}


CLcxCam2D::~CLcxCam2D()
{
	Destroy();
}



INT CLcxCam2D::Query(char* sCmd, LC_HANDLE pData)
{
	if(0 == LcStr_Stricmp(sCmd, (char*)"Get Scaling"))
		*((FLOAT*)pData) = m_Scl;

	else if(0 == LcStr_Stricmp(sCmd, (char*)"Set Scaling"))
		m_Scl = *((FLOAT*)pData);

	else if(0 == LcStr_Stricmp(sCmd, (char*)"Rotation Absolute"))
	{
		FLOAT fDegree = 0;

		if(pData)
			fDegree = *((FLOAT*)pData);

		RotationAbs(fDegree);
	}
	else
		return LC_EFAIL;

	return LC_OK;
}


INT CLcxCam2D::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT	ScnW=0, ScnH=0;
	LcVar_WindowSize(&ScnW, &ScnH);

	if(LC_FAILED(CLcxCam::Create(p1, p2, p3, p4)))
		return LC_EFAIL;


	FLOAT	l = -ScnW/2.F;
	FLOAT	r = +ScnW/2.F;
	FLOAT	b = -ScnH/2.F;
	FLOAT	t = +ScnH/2.F;
	FLOAT	n = +4096.00F;		// near
	FLOAT	f = -4096.00F;		// far

	m_mtPrj.SetupOrthoGl(l, r, b, t, n, f);

	return LC_OK;
}


INT CLcxCam2D::SetTransformViw()
{
	LCXMATRIX	mtViw;

	mtViw = m_mtViw;


	mtViw._11 *= m_Scl;
	mtViw._12 *= m_Scl;
	mtViw._13 *= m_Scl;
	mtViw._21 *= m_Scl;
	mtViw._22 *= m_Scl;
	mtViw._23 *= m_Scl;
	mtViw._31 *= m_Scl;
	mtViw._32 *= m_Scl;
	mtViw._33 *= m_Scl;
	mtViw._41 *= m_Scl;
	mtViw._42 *= m_Scl;
	mtViw._43 *= m_Scl;

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((FLOAT*)&mtViw);

	return LC_OK;
}

INT CLcxCam2D::SetTransformPrj()
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((FLOAT*)&m_mtPrj);

	return LC_OK;
}


INT CLcxCam2D::Update()
{
	INT	nOrient = 0;
	LcSys_GetAttrib(LCSYS_ORIENT, &nOrient);

	m_mtViw.SetupViewD3dLH(&m_vcEye, &m_vcLook, &m_vcUp);


	if     (LCSYS_ORIENT_LLANDSCAPE == nOrient)
	{
		m_mtViw *= LCXMATRIX(  0.0F, -1.0F, 0.0F, 0.0F
							, +1.0F,  0.0F, 0.0F, 0.0F
							,  0.0F,  0.0F, 1.0F, 0.0F
							,  0.0F,  0.0F, 0.0F, 1.0F);
	}
	else if(LCSYS_ORIENT_RLANDSCAPE == nOrient)
	{
		m_mtViw *= LCXMATRIX(  0.0F, +1.0F, 0.0F, 0.0F
							, -1.0F,  0.0F, 0.0F, 0.0F
							,  0.0F,  0.0F, 1.0F, 0.0F
							,  0.0F,  0.0F, 0.0F, 1.0F);
	}

	return LC_OK;
}


INT CLcxCam2D::FrameMove()
{
	Update();

	SetTransformViw();
	SetTransformPrj();

	return LC_OK;
}




void CLcxCam2D::SetupParamView(const LC_FLOAT3* vcEye, const LC_FLOAT3* vcLook, const LC_FLOAT3* vcUp)
{
	m_vcEye		= *vcEye;
	m_vcEye.z	= 0;
	m_vcLook.x = m_vcEye.x;
	m_vcLook.y = m_vcEye.y;
	//m_vcUp	= (FLOAT*)vcUp;

	m_mtViw.SetupViewD3dLH(&m_vcEye, &m_vcLook, &m_vcUp);
}

void CLcxCam2D::SetupParamProj(FLOAT fFov, FLOAT fScnW, FLOAT fScnH, FLOAT fNear, FLOAT fFar)
{
	m_fAs = fScnW/fScnH;

	//m_fNr = fNear;	// Near
	//m_fFr = fFar;		// Far
}


void CLcxCam2D::MoveSideward(FLOAT speed)
{
	LCXVECTOR3 T(m_mtViw._11, m_mtViw._21, m_mtViw._31);	// X-axis

	T *=speed;
	m_vcEye  += T;
	m_vcLook += T;

	m_mtViw.SetupViewD3dLH(&m_vcEye, &m_vcLook, &m_vcUp);
}

void CLcxCam2D::MoveForward(FLOAT speed, FLOAT)
{
	FLOAT	t = 0.01F * speed;

	m_Scl += t;

	if( 0.001F> m_Scl)
		m_Scl = 0.001F;

	if(100.F < m_Scl)
		m_Scl = 100.F;


}


void CLcxCam2D::MoveUpward(FLOAT speed)
{
	LCXVECTOR3 T(m_mtViw._12, m_mtViw._22, m_mtViw._32);	// Y-axis

	T *=speed;
	m_vcEye  += T;
	m_vcLook += T;

	m_mtViw.SetupViewD3dLH(&m_vcEye, &m_vcLook, &m_vcUp);
}


void CLcxCam2D::RotateYaw(FLOAT fDelta,FLOAT fSpeed)
{
	m_fYaw		= LCXToRadian(fDelta*fSpeed);

	LCXMATRIX	rot;
	LCXVECTOR3	vcZ = m_vcLook - m_vcEye;


	rot.SetupRotationZ(m_fYaw);
	rot.TransformCoord(&vcZ, &vcZ);
	rot.TransformCoord(&m_vcUp, &m_vcUp);

	m_vcLook	= vcZ + m_vcEye;

	m_mtViw.SetupViewD3dLH(&m_vcEye, &m_vcLook, &m_vcUp);
}


void CLcxCam2D::RotatePitch(FLOAT fDelta,FLOAT fSpeed)
{
}



void CLcxCam2D::RotationAbs(FLOAT fDegree)
{
	m_vcUp		= LCXVECTOR3(0, 1,  0);
	m_fYaw		= LCXToRadian(fDegree);

	LCXMATRIX	rot;
	LCXVECTOR3	vcZ = m_vcLook - m_vcEye;


	rot.SetupRotationZ(m_fYaw);
	rot.TransformCoord(&vcZ, &vcZ);
	rot.TransformCoord(&m_vcUp, &m_vcUp);

	m_vcLook	= vcZ + m_vcEye;

	m_mtViw.SetupViewD3dLH(&m_vcEye, &m_vcLook, &m_vcUp);
}


