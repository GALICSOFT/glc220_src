// Interface for the CLcxAsb class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxAsb_H_
#define _LcxAsb_H_


class CLcxAsb : public ILCX_Mdl
{
protected:
	char			m_sFile[LC_MAX_PATH];

	LCX_Ase::TInfo	m_Info		;
	LCX_Ase::Tmtl*	m_pMtl		;
	LCX_Ase::Tgeo*	m_pGeo		;

	DOUBLE			m_TimeC		;		// Current Time
	FLOAT			m_FrameC	;		// Current Frame
	INT				m_FrameB	;		// Animation Begin Frame
	INT				m_FrameE	;		// Animation End Frame
	LCXMATRIX		m_mtWld		;		// World Matrix


public:
	CLcxAsb();
	virtual ~CLcxAsb();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	virtual void	Destroy();

	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);
	virtual INT		GetRscType()	{	return LCXM_SRC_ASE;	}

public:
	const LCX_Ase::TInfo* GetInfo() const	{	return &m_Info; }
	LCX_Ase::Tmtl*		GetMtrl()	 const	{	return m_pMtl;	}
	LCX_Ase::Tgeo*		GetGeometry() const	{	return m_pGeo;	}

protected:
	INT		Load();
	INT		LoadTexture(LC_HANDLE pOption);
};



#endif


