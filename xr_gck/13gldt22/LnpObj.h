// Interface for the CLcpObj class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcpObj_H_
#define _LcpObj_H_

#pragma warning(disable : 4786)
#include <vector>
using namespace std;


class CLcpObj : public ILcpObj
{
public:
	enum ElcpObj
	{
		OBJ_ANI_LOOP_INFINITE	= 0,
		OBJ_ANI_LOOP_ONCE		= 1,
	};


protected:
	// Data
	ILcpData*	m_DataB;		// Basic Game Data		==> internal delete.
	ILcpData*	m_DataE;		// Extended Game Data	==> internal delete.
	ILcpData*	m_DataX;		// Auxiliary Gaem Data	==> Shred data and external delete.

	// Model
	ILcMdl*		m_pMdl;			// Rendering Primary Model.
	lsLcMdl		m_vMdlE;		// Equip and Extended Model.
	lsLcMdl		m_vMdlX;		// Auxiliary Model.

	// Map
	ILcmFld*	m_pFld;			// Field Map

	// State
	INT			m_nState;		// State

	INT			m_FrmLoop;		// Animation Frame Loop
	INT			m_FrmCnt;		// Animation Frame Count
	INT			m_FrmIdx;		// Animation Frame Index
	
	vector<ILcMdl::_Tframe	> m_FrmLst;	// Animation Frame List

public:
	CLcpObj();
	virtual ~CLcpObj();
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		SetAttrib(char* sCmd, void* pVal);
	virtual INT		GetAttrib(char* sCmd, void* pVal);

	virtual INT		SetAnimation(char* sCmd, void* pVal);
	virtual INT		GetAnimation(char* sCmd, void* pVal);

protected:
	INT	FindAnimationSet(char*);
};


#endif


