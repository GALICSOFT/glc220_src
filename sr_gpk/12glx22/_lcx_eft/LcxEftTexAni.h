// Interface for the CLcxEftTexAni class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxEftTexAni_H_
#define _LcxEftTexAni_H_


class CLcxEftTexAni : public CLcxEffect
{
protected:

public:
	CLcxEftTexAni();
	virtual ~CLcxEftTexAni();

	virtual	INT		FrameMove();
	virtual void	Render();

	virtual INT		GetRscType()	{	return LCXEFT_TEXTURE_ANI;	}

public:
	virtual INT		Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4);
	virtual void	Destroy();
};






#endif

