// Interface for the LCX_MoSqc3d class.
// 2D Motion Sequence
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxMoSqc3d_H_
#define _LcxMoSqc3d_H_

#include <vector>

typedef std::vector<TLCX_MOSQIC2D >	lsMoSqc3d;
typedef lsMoSqc3d::iterator			itMoSqc3d;


class CLcxMoSqc3d : public CLcxMoSqc
{
protected:
	lsMoSqc3d		m_vSqc		;		// Sequence list

public:
	CLcxMoSqc3d();
	virtual ~CLcxMoSqc3d();

	virtual INT		FrameMove();

	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);		// Write Attribute Value
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);		// Read Attribute Value

	virtual INT		GetRscType()	{ return LCXMO_SQC3D; }		// Resource type: 3D Motion Sequence


public:
	virtual INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	virtual void	Destroy();

};

#endif

