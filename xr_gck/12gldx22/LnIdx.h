// Interface for the CLcIdx class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcIdx_H_
#define _LcIdx_H_

class CLcIdx : public ILcIdx
{
protected:
	DWORD				m_dFMT;
	INT					m_nNum;			// Index Number

public:
	CLcIdx();
	virtual ~CLcIdx();

public:
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual DWORD	GetFMT()	{	return m_dFMT;	}
};



class CLcIdx16 : public CLcIdx
{
protected:
	LcM::VtxIdx*	m_pIdx;

public:
	CLcIdx16();
	virtual ~CLcIdx16();

public:
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual void*	GetIndex()	{	return m_pIdx;	}
};


class CLcIdx32 : public CLcIdx
{
protected:
	LcM::VtxIdx32*	m_pIdx;

public:
	CLcIdx32();
	virtual ~CLcIdx32();

public:
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual void*	GetIndex()	{	return m_pIdx;	}
};


#endif

