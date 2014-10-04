// Interface for the CLcxAst class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxAst_H_
#define _LcxAst_H_


class CLcxAst : public ILCX_Mdl
{
protected:
	char			m_sFile[LC_MAX_PATH];

	LCX_Ase::TInfo	m_Info		;
	LCX_Ase::Tmtl*	m_pMtl		;
	LCX_Ase::Tgeo*	m_pGeo		;

	LCX_Ase::_AseMtl* m_pMtlS	;
	LCX_Ase::_AseGeo* m_pGeoS	;


	DOUBLE			m_TimeC		;		// Current Time
	FLOAT			m_FrameC	;		// Current Frame
	INT				m_FrameB	;		// Animation Begin Frame
	INT				m_FrameE	;		// Animation End Frame
	LCXMATRIX		m_mtWld		;		// World Matrix


public:
	CLcxAst();
	virtual ~CLcxAst();


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
	const LCX_Ase::TInfo* GetInfo() const	{	return &m_Info;	}
	LCX_Ase::Tmtl*		GetMtrl()	 const	{	return m_pMtl;	}
	LCX_Ase::Tgeo*		GetGeometry() const	{	return m_pGeo;	}

protected:
	INT		Load();
	INT		ParseScene(FILE* fp);
	INT		ParseMaterial(FILE* fp);
	INT		ParseGeometry(FILE* fp);
	INT		ParseAnimation(FILE* fp);

	INT		SetupRenderData();
	INT		SetupAniMatrix();

	INT		LoadTexture(LC_HANDLE pOption);

protected:
	void	SaveToAsmBin();
	void	SaveToAsmTxt();
};


#endif

