// Interface for the CLcInputD class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcInputD_H_
#define _LcInputD_H_

class CLcInputD : public CLcInput
{
protected:
//	LPDIRECTINPUT8			m_pDiI	;
//	LPDIRECTINPUTDEVICE8	m_pDiK	;	// Keyboard
//	LPDIRECTINPUTDEVICE8	m_pDiM	;	// Mouse	

	void*					m_pDiI	;
	void*					m_pDiK	;	// Keyboard
	void*					m_pDiM	;	// Mouse	



public:
	CLcInputD();
	virtual ~CLcInputD();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		FrameMove();
	virtual INT		Query(char* sCmd, void* pData);

protected:
	void	SetMousePos(FLOAT* vcPos);
	void	OnReset();
};

#endif

