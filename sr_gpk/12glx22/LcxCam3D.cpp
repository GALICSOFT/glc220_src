// Implementation of the LcxCam3D.
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
#include "LcxCam3D.h"



CLcxCam3D::CLcxCam3D()
{
	m_vcEye		= LCXVECTOR3(10, -390, 40);
	m_vcLook	= LCXVECTOR3(0,0,0);
	m_vcUp		= LCXVECTOR3(0,0,1);

	m_fFv		= FLOAT(LCX_PI/4.0F);
	m_fAs		= 320.F/240.F;
	m_fNr		= 1.0F;
	m_fFr		= 5000.0F;
}


CLcxCam3D::~CLcxCam3D()
{
	Destroy();
}


INT CLcxCam3D::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	if(LC_FAILED(CLcxCam::Create(p1, p2, p3, p4)))
		return LC_EFAIL;


	//it needs re-setup
	m_vcEye		= LCXVECTOR3(10, -390, 40);
	m_vcLook	= LCXVECTOR3(0,0,0);
	m_vcUp		= LCXVECTOR3(0,0,1);
	m_fFv		= FLOAT(LCX_PI/4.0F);
	m_fNr		= 1.0F;
	m_fFr		= 5000.0F;

	return LC_OK;
}


INT CLcxCam3D::SetTransformViw()
{
	// X-Y 평면에 Z 축이 위로 향하는 공간에서 2, 3의 교환과 14, 24, 34의 반전이 필요
	LCXMATRIX	mtViw;
	mtViw._11 = m_mtViw._11;	mtViw._12 = m_mtViw._13;	mtViw._13 = -m_mtViw._12;	mtViw._14 = -m_mtViw._14;
	mtViw._21 = m_mtViw._21;	mtViw._22 = m_mtViw._23;	mtViw._23 = -m_mtViw._22;	mtViw._24 = -m_mtViw._24;
	mtViw._31 = m_mtViw._31;	mtViw._32 = m_mtViw._33;	mtViw._33 = -m_mtViw._32;	mtViw._34 = -m_mtViw._34;
	mtViw._41 = m_mtViw._41;	mtViw._42 = m_mtViw._43;	mtViw._43 = -m_mtViw._42;	mtViw._44 = +m_mtViw._44;


	PLC_DEVICE	pDev = (PLC_DEVICE)LcVar_ReadP(LC_VAR_DEVICE);

	pDev->SetTransform(LC_TS_VIEW, &mtViw);

	return LC_OK;
}

INT CLcxCam3D::SetTransformPrj()
{
	PLC_DEVICE	pDev = (PLC_DEVICE)LcVar_ReadP(LC_VAR_DEVICE);

	pDev->SetTransform(LC_TS_PROJ, &m_mtPrj);

	return LC_OK;
}




INT CLcxCam3D::Update()
{
	INT		i=0;
	LCXVECTOR3	vcH[8];

	m_vcAxis[0].x = m_mtViw._11;
	m_vcAxis[0].y = m_mtViw._21;
	m_vcAxis[0].z = m_mtViw._31;

	m_vcAxis[1].x = m_mtViw._12;
	m_vcAxis[1].y = m_mtViw._22;
	m_vcAxis[1].z = m_mtViw._32;

	m_vcAxis[2].x = m_mtViw._13;
	m_vcAxis[2].y = m_mtViw._23;
	m_vcAxis[2].z = m_mtViw._33;



	LCXMATRIX	mtViw;
	mtViw._11 = m_mtViw._11;	mtViw._12 = m_mtViw._13;	mtViw._13 = -m_mtViw._12;	mtViw._14 = -m_mtViw._14;
	mtViw._21 = m_mtViw._21;	mtViw._22 = m_mtViw._23;	mtViw._23 = -m_mtViw._22;	mtViw._24 = -m_mtViw._24;
	mtViw._31 = m_mtViw._31;	mtViw._32 = m_mtViw._33;	mtViw._33 = -m_mtViw._32;	mtViw._34 = -m_mtViw._34;
	mtViw._41 = m_mtViw._41;	mtViw._42 = m_mtViw._43;	mtViw._43 = -m_mtViw._42;	mtViw._44 = +m_mtViw._44;


	m_mtVP	= mtViw * m_mtPrj;
	m_mtVPI.Inverse(NULL, &m_mtVP, LC_FALSE);
	m_mtViwI.Inverse(NULL, &m_mtViw, LC_FALSE);
	m_mtBill = m_mtViwI;
	m_mtBill._41 = m_mtBill._42 = m_mtBill._43 = 0;	// Bill board Matrix



	vcH[0] = LCXVECTOR3(-1,-1, 0);
	vcH[1] = LCXVECTOR3( 1,-1, 0);
	vcH[2] = LCXVECTOR3( 1, 1, 0);
	vcH[3] = LCXVECTOR3(-1, 1, 0);

	vcH[4] = LCXVECTOR3(-1,-1, 1);
	vcH[5] = LCXVECTOR3( 1,-1, 1);
	vcH[6] = LCXVECTOR3( 1, 1, 1);
	vcH[7] = LCXVECTOR3(-1, 1, 1);

	for(i=0; i<8; ++i)
		m_mtVPI.TransformCoord(&m_vcFrsm[i], &vcH[i]);



	// Frustum Process
	LCXVECTOR3 vcN = -m_vcAxis[1];			// Near
	LCXVECTOR3 vcF = +m_vcAxis[1];			// Far

	LCXVECTOR3 vcP_n= m_vcEye + vcF * m_fNr;	// Camera Position + Near Value * Z direction
	LCXVECTOR3 vcP_f= m_vcEye + vcF * m_fFr;	// Camera Position + Far Value * Z direction


	m_plFrsm[0].SetupFromPointNormal(&vcP_n, &vcN);			// Near Plane
	m_plFrsm[1].SetupFromPointNormal(&vcP_f, &vcF);			// Far Plane

	m_plFrsm[2].SetupFromPoints( &m_vcEye, &m_vcFrsm[7], &m_vcFrsm[4]);	// Left
	m_plFrsm[3].SetupFromPoints( &m_vcEye, &m_vcFrsm[5], &m_vcFrsm[6]);	// Right
	m_plFrsm[4].SetupFromPoints( &m_vcEye, &m_vcFrsm[6], &m_vcFrsm[7]);	// Top
	m_plFrsm[5].SetupFromPoints( &m_vcEye, &m_vcFrsm[4], &m_vcFrsm[5]);	// Bottom

	return LC_OK;
}


INT CLcxCam3D::FrameMove()
{
	Update();

	SetTransformViw();
	SetTransformPrj();

	return LC_OK;
}




void CLcxCam3D::SetupParamView(const LC_FLOAT3* vcEye, const LC_FLOAT3* vcLook, const LC_FLOAT3* vcUp)
{
	m_vcEye		= *vcEye;
	m_vcLook	= *vcLook;
	m_vcUp		= *vcUp;

	m_mtViw.SetupView3DMax(&m_vcEye, &m_vcLook, &m_vcUp);
}

void CLcxCam3D::SetupParamProj(FLOAT fFov, FLOAT fScnW, FLOAT fScnH, FLOAT fNear, FLOAT fFar)
{
	m_fFv	= fFov;
	m_fAs	= fScnW/fScnH;
	m_fNr	= fNear;
	m_fFr	= fFar;

	m_mtPrj.SetupPerspectiveD3dRH(m_fFv, m_fAs, m_fNr, m_fFr);
}

void CLcxCam3D::MoveSideward(FLOAT speed)
{
	LCXVECTOR3 Z = m_vcLook - m_vcEye;


	LCXVECTOR3    X;
	X.Cross(&Z, &m_vcUp);
	X.Normalize();

	X *=speed;
	m_vcEye  += X;
	m_vcLook += X;

	m_mtViw.SetupView3DMax(&m_vcEye, &m_vcLook, &m_vcUp);
}



void CLcxCam3D::MoveForward(FLOAT speed, FLOAT)
{
	LCXVECTOR3 T = m_vcLook - m_vcEye;
	T.Normalize();

	T *=speed;

	m_vcLook +=T;
	m_vcEye  +=T;
	m_mtViw.SetupView3DMax(&m_vcEye, &m_vcLook, &m_vcUp);
}



void CLcxCam3D::MoveUpward(FLOAT speed)
{
	LCXVECTOR3 Z = m_vcLook - m_vcEye;


	LCXVECTOR3    X;
	LCXVECTOR3    U;
	X.Cross(&Z, &m_vcUp);
	U.Cross(&Z, &X);
	U.Normalize();

	U *=speed;
	m_vcEye  += U;
	m_vcLook += U;

	m_mtViw.SetupView3DMax(&m_vcEye, &m_vcLook, &m_vcUp);
}

void CLcxCam3D::RotateYaw(FLOAT fDelta,FLOAT fSpeed)
{
	m_fYaw		= LCXToRadian(fDelta*fSpeed);

	LCXMATRIX	rot;
	LCXVECTOR3	vcZ = m_vcLook - m_vcEye;


	{
		rot.SetupRotationZ(m_fYaw);
		rot.TransformCoord(&vcZ, &vcZ);
		rot.TransformCoord(&m_vcUp, &m_vcUp);

		m_vcLook	= vcZ + m_vcEye;

		m_mtViw.SetupView3DMax(&m_vcEye, &m_vcLook, &m_vcUp);
	}

}

void CLcxCam3D::RotatePitch(FLOAT fDelta,FLOAT fSpeed)
{
	m_fPitch	= LCXToRadian(fDelta*fSpeed);

	LCXMATRIX	rot;
	LCXVECTOR3 vcZ = m_vcLook - m_vcEye;
	LCXVECTOR3 vcX = LCXVECTOR3(m_mtViw._11, m_mtViw._21, m_mtViw._31);


	{
		rot.SetupRotationAxis(&vcX, m_fPitch);
		rot.TransformCoord(&vcZ, &vcZ);
		rot.TransformCoord(&m_vcUp, &m_vcUp);

		m_vcLook	= vcZ + m_vcEye;

		m_mtViw.SetupView3DMax(&m_vcEye, &m_vcLook, &m_vcUp);
	}

}


