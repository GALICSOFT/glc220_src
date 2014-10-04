// Interface for the LcxCam3D class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxCam3D_H_
#define _LcxCam3D_H_


// Single Camera
class CLcxCam3D : public CLcxCam
{
protected:


public:
	CLcxCam3D();
	virtual ~CLcxCam3D();


	virtual INT		GetType()	{	return LCX_CAM_FIRST;	}


	virtual INT		Update();				// Setup View and Projection Matrix of Camera
	virtual INT		FrameMove();			// Update() + SetTransformViw() and SetTransformPrj()
	virtual INT		SetTransformViw();		// Setup View Matrix of Device and 3d Pipeline.
	virtual INT		SetTransformPrj();		// Setup Projection Matrix of Device and 3d Pipeline.


	// Set Parameter
	virtual void	SetupParamView(const LC_FLOAT3* Eye, const LC_FLOAT3* LookAt, const LC_FLOAT3* Up);
	virtual void	SetupParamProj(FLOAT fFov, FLOAT fScnW, FLOAT fScnH, FLOAT fNear, FLOAT fFar);

	// Individual Camera
	virtual void	SetupMatrixViw(const LC_MATRIX* mtViw){	m_mtViw = *mtViw;	}
	virtual void	SetupMatrixPrj(const LC_MATRIX* mtPrj){	m_mtPrj = *mtPrj;	}

	virtual void	SetEye (const LC_FLOAT3* v)	{	m_vcEye  = *v;	}
	virtual void	SetLook(const LC_FLOAT3* v)	{	m_vcLook = *v;	}
	virtual void	SetUp  (const LC_FLOAT3* v)	{	m_vcUp   = *v;	}

	// Action
	virtual void	MoveSideward(FLOAT fSpeed);
	virtual void	MoveForward	(FLOAT fSpeed, FLOAT fY=1);
	virtual void	MoveUpward  (FLOAT fSpeed);
	virtual void	RotateYaw	(FLOAT fDelta, FLOAT fSpeed);
	virtual void	RotatePitch	(FLOAT fDelta, FLOAT fSpeed);

public:
	INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
};


#endif


