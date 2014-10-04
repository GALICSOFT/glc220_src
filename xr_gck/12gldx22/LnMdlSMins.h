// Interface for the CLcMdlSMins class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcMdlSMins_H_
#define _LcMdlSMins_H_

class CLcMdlSMins : public CLcMdl
{
protected:
	CLcMdlSM*	m_pOrg;					// Original Pointer

public:
	CLcMdlSMins();
	virtual ~CLcMdlSMins();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual	void	Destroy();
	virtual	INT		FrameMove();
	virtual	void	Render();

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		GetType()				{	return m_pOrg->GetType();	}
	virtual const char*	const GetName()const{	return m_pOrg->GetName();	}
	virtual	ILcMdl* GetOriginal()			{	return m_pOrg;				}

protected:
	void*		m_pPack;					// miscellany	Packate
	VEC3		m_vcEps;					// Epsilon
};


#endif

