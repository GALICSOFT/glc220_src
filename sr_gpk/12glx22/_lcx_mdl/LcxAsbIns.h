// Interface for the CLcxAsbIns class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxAsbIns_H_
#define _LcxAsbIns_H_


class CLcxAsbIns : public ILCX_Mdl
{
protected:
	CLcxAsb*		m_pOrg		;

	INT				m_nFrmF		;		// First Frame
	INT				m_nFrmL		;		// Last Frame
	INT				m_nFrmS		;		// Frames per one scecond
	INT				m_nFrmT		;		// Tick per Frame

	INT				m_nMtl		;
	LCX_Ase::Tmtl*	m_pMtl		;
	INT				m_nGeo		;
	LCX_Ase::Tgeo*	m_pGeo		;

	LCX_Ase::TlinkTm* m_pTM		;		// World and Animation Matrix

	DOUBLE			m_TimeC		;		// Current Time
	FLOAT			m_FrameC	;		// Current Frame
	INT				m_FrameB	;		// Animation Begin Frame
	INT				m_FrameE	;		// Animation End Frame
	LCXMATRIX		m_mtWld		;		// World Matrix


public:
	CLcxAsbIns();
	virtual ~CLcxAsbIns();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	virtual void	Destroy();

	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);

	virtual INT		GetRscType()	{	return m_pOrg->GetRscType();	}
};

#endif

