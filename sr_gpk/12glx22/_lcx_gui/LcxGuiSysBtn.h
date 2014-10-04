// Interface for the CLcxGuiSysBtn class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxGuiSysBtn_H_
#define _LcxGuiSysBtn_H_


class CLcxGuiSysBtn : public CLcxGui
{
protected:
	INT			m_rCbA		;		// Active button rect

public:
	CLcxGuiSysBtn();
	virtual ~CLcxGuiSysBtn();

	INT		Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4);
	void	Destroy();

	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		GetRscType()	{	return LCXGUI_SYSBUTTON;	}

	virtual	INT		SetupWithExtern(TLCX_GUI_EXTERN* ext);
};


#endif

