// Interface for the CLcxEffect class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxEffect_H_
#define _LcxEffect_H_


class CLcxEffect : public ILCX_Effect
{
protected:

public:
	CLcxEffect();
	virtual ~CLcxEffect();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual	INT		FrameMove()								{	return LC_EFAIL;	}
	virtual void	Render()								{						}

	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData)	{	return LC_EFAIL;	}
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData)	{	return LC_EFAIL;	}

	virtual INT		GetRscType()							{	return LC_DEFAULT;	}

public:
	virtual INT		Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4);
	virtual void	Destroy()								{						}
};






#endif

