// Interface for the CLcxGuiStatic class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxGuiStatic_H_
#define _LcxGuiStatic_H_


class CLcxGuiStatic : public CLcxGui
{
protected:

public:

	CLcxGuiStatic();
	virtual ~CLcxGuiStatic();

	INT		Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4);
	void	Destroy();

	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		GetRscType()	{	return LCXGUI_STATIC;	}

	virtual	INT		SetupWithExtern(TLCX_GUI_EXTERN* ext);
};


#endif

