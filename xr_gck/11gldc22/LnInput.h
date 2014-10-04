// Interface for the CLcInput class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcInput_H_
#define _LcInput_H_

class CLcInput : public ILcInput
{
public:
	enum ELcInputType
	{
		LN_INPUT_0,
		LN_INPUT_A,										// win api
		LN_INPUT_D,										// direct input
	};


	enum EInputState
	{
		INPUTST_NONE	=0,
		INPUTST_DOWN	=1,
		INPUTST_UP		=2,
		INPUTST_PRESS	=3,
	};
	
protected:
	INT				m_eType		;						// Input Type
	HINSTANCE		m_hInst		;						// HINSTANCE
	HWND			m_hWnd		;						// Window Handle

	BYTE			m_KeyCur[256];						// Current Keyboard
	BYTE			m_KeyOld[256];						// Old Keyboard
	BYTE			m_KeyMap[256];						// Key Map down: INPUTST_DOWN, up: INPUTST_UP, Press INPUTST_PRESS

	BYTE			m_BtnCur[  8];						// Current Button
	BYTE			m_BtnOld[  8];						// Old Bttton
	BYTE			m_BtnMap[  8];						// Button Map down: INPUTST_DOWN, up: INPUTST_UP, Press INPUTST_PRESS 

	FLOAT			m_vMsCur[  3];						// Mouse Current Position
	FLOAT			m_vMsOld[  3];						// Mouse Old Position
	FLOAT			m_vDelta[  3];						// Delta Mouse Position


public:
	CLcInput();
	virtual ~CLcInput();
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy();
	virtual INT		FrameMove();
	virtual INT		Query(char* sCmd, void* pData);
	virtual INT		MsgProc(HWND,UINT,WPARAM,LPARAM);

	virtual BYTE*	GetKeyMap()	const;

	virtual BOOL	KeyDown(INT nKey);
	virtual BOOL	KeyUp(INT nKey);
	virtual BOOL	KeyPress(INT nKey);
	virtual BOOL	KeyState(int nKey);

	virtual BYTE*	GetButtonMap()	const;

	virtual BOOL	ButtonDown(INT nBtn);
	virtual BOOL	ButtonUp(INT nBtn);
	virtual BOOL	ButtonPress(INT nBtn);
	virtual BOOL	ButtonState(int nBtn);

	virtual FLOAT*	GetMousePos();
	virtual FLOAT*	GetMouseDelta();
	virtual BOOL	GetMouseMove();

	virtual BOOL	IsInRect(INT left, INT top, INT right, INT bottom);

protected:
	void	ClientRect(HWND hWnd, RECT* rc);
};

#endif
