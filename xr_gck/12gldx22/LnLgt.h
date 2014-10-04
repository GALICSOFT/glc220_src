// Interface for the CLcLgt class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcLgt_H_
#define _LcLgt_H_

class CLcLgt : public ILcLgt
{
protected:
	D3DLIGHT9			m_D3Lgt;
	INT					m_nLgt;

public:
	CLcLgt();
	virtual ~CLcLgt();

public:
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();

	virtual void	SetLight(INT nIdx);											// pDev->SetLight( nIdx,....)
	virtual void	SetLightEnable(BOOL	nEnable);								// pDev->LightEnable( nIdx,....)

	virtual void	SetLgtType(DWORD dType);
	
	virtual void	SetLgtAmbient(FLOAT* pVal/*D3DXCOLOR*/ );
	virtual void	SetLgtDiffuse(FLOAT* pVal/*D3DXCOLOR*/ );
    virtual void	SetLgtSpecular(FLOAT* pVal/*D3DXCOLOR*/ );

    virtual void	SetLgtPosition(FLOAT* pVal/*D3DXVECTOR3*/ );
	virtual void	SetLgtDirection(FLOAT* pVal/*D3DXVECTOR3*/ );

	virtual void	SetLgtRange(FLOAT pVal);									// Cutoff range
    virtual void	SetLgtFalloff(FLOAT pVal);									// Falloff

	virtual void	SetLgtAttenuation0(FLOAT pVal);								// Constant attenuation
	virtual void	SetLgtAttenuation1(FLOAT pVal);								// Linear attenuation
	virtual void	SetLgtAttenuation2(FLOAT pVal);								// Quadratic attenuation
	virtual void	SetLgtTheta(FLOAT pVal);									// Inner angle of spotlight cone
	virtual void	SetLgtPhi(FLOAT pVal);										// Outer angle of spotlight cone

	virtual DWORD	GetLgtType();
	
	virtual FLOAT*	GetLgtAmbient();
	virtual FLOAT*	GetLgtDiffuse();
    virtual FLOAT*	GetLgtSpecular();

    virtual FLOAT*	GetLgtPosition();
	virtual FLOAT*	GetLgtDirection();

	virtual FLOAT	GetLgtRange();												// Cutoff range
    virtual FLOAT	GetLgtFalloff();											// Falloff

	virtual FLOAT	GetLgtAttenuation0();										// Constant attenuation
	virtual FLOAT	GetLgtAttenuation1();										// Linear attenuation
	virtual FLOAT	GetLgtAttenuation2();										// Quadratic attenuation
	virtual FLOAT	GetLgtTheta();												// Inner angle of spotlight cone
	virtual FLOAT	GetLgtPhi();												// Outer angle of spotlight cone
};


#endif

