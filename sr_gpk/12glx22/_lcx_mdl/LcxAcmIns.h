// Interface for the CLcxAcmIns class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxAcmIns_H_
#define _LcxAcmIns_H_

#include <vector>


class CLcxAcmIns : public ILCX_Mdl
{
public:
	struct TlinkTm
	{
		VEC3*		pVdP	;		// Vertex Dest Position
		LCXMATRIX	mtW		;		// World Matrix

		TlinkTm();
		~TlinkTm();
	};

protected:
	CLcxAcm*		m_pOrg		;

	INT				m_nFrmF		;		// First Frame
	INT				m_nFrmL		;		// Last Frame
	INT				m_nFrmS		;		// Frames per one scecond
	INT				m_nFrmT		;		// Tick per Frame

	INT				m_nMtl		;
	CLcxAcm::Tmtl*	m_pMtl		;
	INT				m_nGeo		;
	CLcxAcm::Tgeo*	m_pGeo		;


	CLcxAcmIns::TlinkTm* m_pTM	;		// World and Animation Matrix

	INT				m_BlndNum	;		// Blending Number


	DOUBLE			m_TimeC		;		// Current Time
	FLOAT			m_FrameC	;		// Current Frame
	INT				m_FrameB	;		// Animation Begin Frame
	INT				m_FrameE	;		// Animation End Frame
	LCXMATRIX		m_mtWld		;		// World Matrix


public:
	CLcxAcmIns();
	virtual ~CLcxAcmIns();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual INT		Create(LC_HANDLE p1=NULL, LC_HANDLE p2=NULL, LC_HANDLE p3=NULL, LC_HANDLE p4=NULL);
	virtual void	Destroy();

	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);

	virtual INT		GetRscType()	{	return m_pOrg->GetRscType();	}
};

#endif

