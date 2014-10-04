// Interface for the CLcmPack class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcmPack_H_
#define _LcmPack_H_

class CLcmPack : public ILcmPack
{
public:
	void*		m_pLcl;
	lsLcMdl		m_vObj;

public:
	CLcmPack();
	virtual ~CLcmPack();
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();

	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		Query(char* sCmd, void* pVal);

	virtual void	SetLcl(void* pLcl);
	virtual void	AddObj(ILcMdl* pMdB
							, VEC3 vcP		// Position
							, void* p1		// Camera
							, void* p2		// Field
							, void* p3		// Field Local
							, void* p4		// Parent Package
							);

	virtual	void	Push_Back(ILcMdl* pSrc);
	virtual	INT		Size();
	virtual	ILcMdl*	GetObj(int n);
	virtual	INT		Erase(int n);
	virtual	INT		Erase(ILcMdl*);
};


#endif

