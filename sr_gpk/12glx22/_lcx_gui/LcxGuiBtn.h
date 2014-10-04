// Interface for the CLcxGuiBtn class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxGuiBtn_H_
#define _LcxGuiBtn_H_


class CLcxGuiBtn : public CLcxGui
{
protected:
	LCXRECT*		m_rCbA;		// Active button rect

public:
	CLcxGuiBtn();
	virtual ~CLcxGuiBtn();

	INT		Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4);
	void	Destroy();

	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		GetRscType()	{	return LCXGUI_BUTTON;	}

	virtual	INT		SetupWithExtern(TLCX_GUI_EXTERN* ext);
};


#endif

