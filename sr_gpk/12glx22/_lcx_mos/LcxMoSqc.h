// Interface for the LCX_MoSqc class.
// 2D Motion Sequence
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxMoSqc_H_
#define _LcxMoSqc_H_


class CLcxMoSqc : public ILCX_MoSqc
{
protected:
	INT				m_Type		;		// LCXMO_CARTESIAN, ...
	DOUBLE			m_dTimeC	;		// Current Time(millisecond)

	INT				m_nSqcStt	;		// Sequence state
	INT				m_nSqcCur	;		// Current Sequence Index
	LC_HANDLE		m_pSqcBgn	;		// Begin Pointer
	LC_HANDLE		m_pSqcEnd	;		// End Pointer

public:
	CLcxMoSqc();
	virtual ~CLcxMoSqc();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();

	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);		// Write Attribute Value
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);		// Read Attribute Value
};

#endif

