// Interface for the CLcCamA class.
// First or Third Person Camera
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcCamA_H_
#define _LcCamA_H_

class CLcCamA : public CLcCam
{
public:
	CLcCamA();
	virtual ~CLcCamA();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual INT		FrameMove();

public:
	virtual void	MoveSideward(FLOAT	fSpeed);
	virtual void	MoveForward	(FLOAT	fSpeed, FLOAT fY=1);
	virtual void	RotateYaw	(FLOAT fDelta,FLOAT fSpeed);
	virtual void	RotatePitch	(FLOAT fDelta,FLOAT fSpeed);
	virtual	void	Zoom(FLOAT);

public:	
	
};

#endif

