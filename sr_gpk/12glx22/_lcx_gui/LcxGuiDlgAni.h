// Interface for the CLcxGuiDlgAni class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxGuiDlgAni_H_
#define _LcxGuiDlgAni_H_


class CLcxGuiDlgAni : public CLcxGuiDialog
{
protected:
	INT				m_BgVtx		;
	LCXVECTOR2*		m_BgPos		;
	LCXVECTOR2*		m_BgTex		;

	CLcxGui*		m_SysBtn	;		// System button
	LCXVECTOR2		m_SysPos	;		// system close button position

public:
	CLcxGuiDlgAni();
	virtual ~CLcxGuiDlgAni();

	INT		Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4);
	void	Destroy();

	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		GetRscType()	{	return LCXGUI_DIALOG;	}
};


#endif

