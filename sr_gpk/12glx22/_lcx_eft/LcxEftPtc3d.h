// Interface for the CLcxEftPtc3d class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxEftPtc3d_H_
#define _LcxEftPtc3d_H_


class CLcxEftPtc3d : public CLcxEffect
{
protected:

public:
	CLcxEftPtc3d();
	virtual ~CLcxEftPtc3d();

	virtual	INT		FrameMove();
	virtual void	Render();

	virtual INT		GetRscType()	{	return LCXEFT_3D_PARTICLE;	}

public:
	virtual INT		Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4);
	virtual void	Destroy();
};






#endif

