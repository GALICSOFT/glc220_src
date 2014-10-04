// Interface for the LcxCam2D class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxCam2D_H_
#define _LcxCam2D_H_


// Single Camera
class CLcxCam2D : public CLcxCam
{
protected:
	FLOAT	m_Scl;

public:
	CLcxCam2D();
	virtual ~CLcxCam2D();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);

	virtual INT		GetType()		{	return LCX_CAM_2D;	}

	virtual INT		Update();				// Setup View and Projection Matrix of Camera
	virtual INT		FrameMove();			// Update() + SetTransformViw() and SetTransformPrj()
	virtual INT		SetTransformViw();		// Setup View Matrix of Device and 3d Pipeline.
	virtual INT		SetTransformPrj();		// Setup Projection Matrix of Device and 3d Pipeline.


	// Set Parameter
	virtual void	SetupParamView(const LC_FLOAT3* Eye, const LC_FLOAT3* LookAt, const LC_FLOAT3* Up);
	virtual void	SetupParamProj(FLOAT fFov, FLOAT fScnW, FLOAT fScnH, FLOAT fNear, FLOAT fFar);


	virtual void	SetEye (const LC_FLOAT3* v)
	{
		m_vcEye.x  = v->x;
		m_vcEye.y  = v->y;
		m_vcLook.x = m_vcEye.x;
		m_vcLook.y = m_vcEye.y;
	}

	virtual void	SetLook(const LC_FLOAT3* v)
	{
		return;
	}

	virtual void	SetUp  (const LC_FLOAT3* v)
	{
		return;
	}

	// Action
	virtual void	MoveSideward(FLOAT fSpeed);
	virtual void	MoveForward	(FLOAT fSpeed, FLOAT fY=1);
	virtual	void	MoveUpward  (FLOAT fSpeed);
	virtual void	RotateYaw	(FLOAT fDelta, FLOAT fSpeed);
	virtual void	RotatePitch	(FLOAT fDelta, FLOAT fSpeed);

public:
	INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);

protected:
	void	RotationAbs(FLOAT fDegree);		// rotation Absolute
};


#endif


