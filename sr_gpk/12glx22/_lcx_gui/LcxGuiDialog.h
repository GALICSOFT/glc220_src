// Interface for the CLcxGuiDialog class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxGuiDialog_H_
#define _LcxGuiDialog_H_


class CLcxGuiDialog : public CLcxGui
{
protected:
	INT				m_BgVtx		;
	LCXVECTOR2*		m_BgPos		;
	LCXVECTOR2*		m_BgTex		;

	CLcxGui*		m_SysBtn	;		// System button
	LCXVECTOR2		m_SysPos	;		// system close button position

public:
	CLcxGuiDialog();
	virtual ~CLcxGuiDialog();

	INT		Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4);
	void	Destroy();

	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		GetRscType()	{	return LCXGUI_DIALOG;	}

	virtual	INT		SetupWithExtern(TLCX_GUI_EXTERN* ext);
	virtual	void	SetExternTexture(PLC_TEXTURE pTex);
};


#endif

