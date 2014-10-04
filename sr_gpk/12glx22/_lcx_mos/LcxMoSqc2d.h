// Interface for the LCX_MoSqc2d class.
// 2D Motion Sequence
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxMoSqc2d_H_
#define _LcxMoSqc2d_H_

#include <vector>

typedef std::vector<TLCX_MOSQIC2D >	lsMoSqc2d;
typedef lsMoSqc2d::iterator			itMoSqc2d;


class CLcxMoSqc2d : public CLcxMoSqc
{
protected:
	lsMoSqc2d	m_vSqc	;	// Sequence list
	TLCX_MOSQ2D	m_Sqc	;	// saved result

public:
	CLcxMoSqc2d();
	virtual ~CLcxMoSqc2d();

	virtual INT		FrameMove();

	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);	// Write Attribute Value
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);	// Read Attribute Value

	virtual INT		GetRscType()	{ return LCXMO_SQC2D; }		// Resource type: 2D Motion Sequence


public:
	virtual INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	virtual void	Destroy();

};

#endif

