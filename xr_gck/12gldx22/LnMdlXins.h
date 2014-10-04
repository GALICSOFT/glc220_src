// Interface for the CLcMdlXins class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcMdlXins_H_
#define _LcMdlXins_H_

class CLcMdlXins : public CLcMdl
{
protected:
	CLcMdlX*		m_pOrg	;
	
public:
	CLcMdlXins();
	virtual ~CLcMdlXins();

	virtual	INT		Create(void* p1=0, void* p2=0, void* p3=0, void* p4=0);
	virtual	void	Destroy();
	virtual	INT		FrameMove();
	virtual	void	Render();

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		GetType()				{	return m_pOrg->GetType();	}
	virtual const char*	const GetName()const{	return m_pOrg->GetName();	}
	virtual	ILcMdl* GetOriginal()			{	return m_pOrg;				}

	virtual INT		SetAttrib(char* sCmd, void* pVal);
	virtual INT		GetAttrib(char* sCmd, void* pVal);
};


#endif


