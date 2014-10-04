// Interface for the CLcCamM class.
// Multi Camera
//////////////////////////////////////////////////////////////////////

#ifndef _LcCamM_H_
#define _LcCamM_H_

class CLcCamM : public ILcCamM
{
protected:
	ILcCam*	m_pCam[LC_MUTI_CAMERA_MAX];	// Camera First
	

public:
	CLcCamM();
	virtual ~CLcCamM();


	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		FrameMove();
	virtual INT		Query(char* sCmd, void* pData);

public:
	virtual void	SetName(INT nIdx, char* sN);
	virtual char*	GetName(INT nIdx);
	
	virtual void	SetType(INT nIdx, INT	eType);
	virtual INT		GetType(INT nIdx);
	
	virtual const	D3DXMATRIX*  const GetMatrixViw		(INT nIdx) const;
	virtual const	D3DXMATRIX*  const GetMatrixPrj		(INT nIdx) const;
	virtual const	D3DXMATRIX*  const GetMatrixViwI	(INT nIdx) const;
	virtual const	D3DXMATRIX*  const GetMatrixBill	(INT nIdx) const;
	virtual const	D3DXMATRIX*  const GetMatrixViwPrj	(INT nIdx) const;
	virtual const	D3DXMATRIX*  const GetMatrixViwPrjI	(INT nIdx) const;
	
	virtual const	D3DXVECTOR3* const GetEye (INT nIdx) const;
	virtual const	D3DXVECTOR3* const GetLook(INT nIdx) const;
	virtual const	D3DXVECTOR3* const GetUp  (INT nIdx) const;

	virtual void	SetEye	(INT nIdx, D3DXVECTOR3*);
	virtual void	SetLook	(INT nIdx, D3DXVECTOR3*);
	virtual void	SetUp	(INT nIdx, D3DXVECTOR3*);
	
	virtual FLOAT	GetYaw	(INT nIdx);
	virtual FLOAT	GetPitch(INT nIdx);
	
	virtual void	SetFov	(INT nIdx, FLOAT);
	virtual void	SetScnW	(INT nIdx, FLOAT);
	virtual void	SetScnH	(INT nIdx, FLOAT);
	virtual void	SetNear	(INT nIdx, FLOAT);
	virtual void	SetFar	(INT nIdx, FLOAT);

	virtual FLOAT	GetFov	 (INT nIdx);
	virtual FLOAT	GetScnW	 (INT nIdx);
	virtual FLOAT	GetScnH	 (INT nIdx);
	virtual FLOAT	GetAspect(INT nIdx);
	virtual FLOAT	GetNear	 (INT nIdx);
	virtual FLOAT	GetFar	 (INT nIdx);

	virtual const	D3DXVECTOR3* const GetAxisX(INT nIdx);
	virtual const	D3DXVECTOR3* const GetAxisY(INT nIdx);
	virtual const	D3DXVECTOR3* const GetAxisZ(INT nIdx);

	virtual const	D3DXPLANE*   const GetFrustumPlanes(INT nIdx);
	virtual const	D3DXVECTOR3* const GetFrustumPoints(INT nIdx);
		
	virtual void	SetParamView(INT nIdx, D3DXVECTOR3* Eye, D3DXVECTOR3* Look, D3DXVECTOR3* Up);
	virtual void	SetParamProj(INT nIdx, FLOAT fFov, FLOAT fScnW, FLOAT fScnH, FLOAT fNear, FLOAT fFar);
	
	virtual void	SetTransformViw(INT nIdx);
	virtual void	SetTransformPrj(INT nIdx);

	virtual D3DXVECTOR3	GetDirectionOfMouse(INT nIdx, FLOAT fMouseX, FLOAT fMouseY);

	virtual void	SetMatrixViw(INT nIdx, D3DXMATRIX*);
	virtual void	SetMatrixPrj(INT nIdx, D3DXMATRIX*);

	virtual void	MoveSideward(INT nIdx, FLOAT fSpeed);
	virtual void	MoveForward	(INT nIdx, FLOAT fSpeed, FLOAT fY=1);
	virtual void	RotateYaw	(INT nIdx, FLOAT fDelta, FLOAT fSpeed);
	virtual void	RotatePitch	(INT nIdx, FLOAT fDelta, FLOAT fSpeed);
	virtual void	Zoom		(INT nIdx, FLOAT fDelta);


	// For Third Person Camera
	virtual void	SetBasis(INT nIdx, D3DXVECTOR3*);
	virtual const	D3DXVECTOR3* const GetBasis(INT nIdx) const;

	virtual void	SetEpsilonY(INT nIdx, FLOAT);
	virtual FLOAT	GetEpsilonY(INT nIdx)		;

	virtual void	SetGap(INT nIdx, FLOAT)	;
	virtual FLOAT	GetGap(INT nIdx)		;

	// Camera Pointer
	virtual	ILcCam*	GetCameraPointer(INT nIdx);
};

#endif



