// Interface for the CLcMdl2Dins class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcMdl2Dins_H_
#define _LcMdl2Dins_H_

class CLcMdl2Dins : public CLcMdl
{
protected:
	CLcMdl2D*		m_pOrg;		// Original Pointer
	DCLR			m_dC;		// Color
	VtxDUV1			m_pVtx[4];

public:
	CLcMdl2Dins();
	virtual ~CLcMdl2Dins();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual	void	Destroy();
	virtual	INT		FrameMove();
	virtual	void	Render();

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		GetType()				{	return m_pOrg->GetType();	}
	virtual const char*	const GetName()const{	return m_pOrg->GetName();	}
	virtual	ILcMdl* GetOriginal()			{	return m_pOrg;				}

public:
	void			SetColor(DWORD dc)		{	m_dC= dc;			}

protected:
	void*		m_pPack;					// miscellany	Packate
	VEC3		m_vcEps;					// Epsilon
};


#endif

