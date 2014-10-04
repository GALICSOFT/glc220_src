// Interface for the ILcAcmIns class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcAcmIns_H_
#define _LcAcmIns_H_

#pragma warning(disable : 4530)
#pragma warning(disable : 4786)
#pragma warning(disable : 4996)

#include <vector>
#include <set>
#include <map>
#include <string>
using namespace	std;


class CLcAcmIns : public CLcMdl
{
protected:
	CLcAcm*				m_pOrg		;

	CLcAcm::LcmHeader*	m_pLcHead	;
	CLcAcm::LcMtl*		m_pLcMtl	;
	CLcAcm::LcGeo*		m_pLcGeo	;
	vector<_Tframe>*	m_pFrame	;
	map<string,PDTX>*	m_mpTxOrg	;

	MATA*				m_mtPrn		;	// Parent World Matrix
	MATA*				m_pmtWld	;

	DOUBLE				m_dFrmCur	;			// Current Frame
	DOUBLE				m_dTimeCur	;			// Current Time
	INT					m_nActM		;			// Action Master Index Current

public:
	CLcAcmIns();
	virtual ~CLcAcmIns();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		GetType()				{	return m_pOrg->GetType();	}
	virtual const char*	const GetName()const{	return m_pOrg->GetName();	}
	virtual	ILcMdl* GetOriginal()			{	return m_pOrg;				}

	virtual INT		SetAttrib(char* sCmd, void* pVal);
	virtual INT		GetAttrib(char* sCmd, void* pVal);

protected:
	INT		m_bBone;					// for Viewer: Show Bone
	INT		m_bModel;					// for Viewer: Show Model
	INT		m_bAnima;					// for Viewer: Animation
};


#endif

