// Interface for the CLcAseBins class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcAseBins_H_
#define _LcAseBins_H_

class CLcAseBins : public CLcMdl
{
public:
	struct TlinkTm
	{
		D3DXMATRIX	mtW		;		// World Matrix
		D3DXMATRIX	mtL		;		// Local Matrix

		INT			nPrn	;		// Parent Node Index

		TlinkTm();
	};

protected:
	CLcAseB*		m_pOrg	;

	CLcAseB::Thead*	m_pHead	;
	CLcAseB::Tmtl*	m_pMtl	;
	CLcAseB::Tgeo*	m_pGeo	;
	vector<_Tframe>* m_pFrame;

	TlinkTm*		m_pTM	;		// World and Animation Matrix

	DOUBLE		m_dFrmCur	;		// Current Frame
	DOUBLE		m_dTimeCur	;		// Current Time
	INT			m_nActM		;		// Action Master Index Current

public:
	CLcAseBins();
	virtual ~CLcAseBins();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
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

