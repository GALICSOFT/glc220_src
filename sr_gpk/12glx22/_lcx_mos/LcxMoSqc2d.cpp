// Implementation of the LCX_MoSqc2d.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcMath.h>
#include <LcBase.h>
#include <LcAux.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
  #include "../ILCX_MoSqc.h"

#else
  #include "../LcxEntity.h"

#endif


#include "LcxMoSqc.h"
#include "LcxMoSqc2d.h"



CLcxMoSqc2d::CLcxMoSqc2d()
: m_vSqc(0)
{
}


CLcxMoSqc2d::~CLcxMoSqc2d()
{
	Destroy();
}



void CLcxMoSqc2d::Destroy()
{
	m_vSqc.clear();
}


INT CLcxMoSqc2d::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT				n;

	TLCX_MOSQIC2D*	pSqc = NULL;
	INT				nSqc = 0;


	if(!p1 || !p2)
		return LC_EFAIL;

	pSqc = (TLCX_MOSQIC2D*)p1;
	nSqc = *((INT*)p2);

	if(nSqc<2)
		return LC_EFAIL;

	for(n=0; n<nSqc; ++n)
	{
		m_vSqc.push_back( pSqc[n] );
		m_vSqc[n].nIdx = n;
	}


	// make the begin and end pointer
	m_pSqcBgn = &m_vSqc[0];
	m_pSqcEnd = &m_vSqc[nSqc-1];

	return LC_OK;
}


INT CLcxMoSqc2d::FrameMove()
{
	INT				n	= 0;
	TLCX_MOSQIC2D*	pSqc0 = NULL;
	TLCX_MOSQIC2D*	pSqc1 = NULL;
	INT				nSqc = 0;

	FLOAT			w0 = 0, w1=0;

	if((INT)LCXMO_DEFAULT == m_nSqcStt || LCXMO_FINISH == m_nSqcStt)
		return m_nSqcStt;

	if(LCXMO_BEGIN == m_nSqcStt)
	{
		memcpy(&m_Sqc, m_pSqcBgn, sizeof(TLCX_MOSQ2D));
		m_nSqcStt = LCXMO_PLAY;
		return m_nSqcStt;
	}

	if( ((TLCX_MOSQIC2D*)m_pSqcEnd)->time <= m_dTimeC)
	{
		m_nSqcStt = LCXMO_FINISH;

		if(m_pSqcEnd)
			memcpy(&m_Sqc, m_pSqcEnd, sizeof(TLCX_MOSQ2D));

		return m_nSqcStt;
	}


	// find sequence
	if( ((TLCX_MOSQ2D*)m_pSqcEnd)->time <= m_dTimeC)
	{
		m_nSqcStt = LCXMO_FINISH;

		if(m_pSqcEnd)
			memcpy(&m_Sqc, m_pSqcEnd, sizeof(TLCX_MOSQ2D));

		return m_nSqcStt;
	}


	nSqc = m_vSqc.size();
	for(n=0; n<nSqc; ++n)
	{
		pSqc0 = &m_vSqc[n  ];
		pSqc1 = &m_vSqc[n+1];

		if( pSqc0->time <= m_dTimeC && m_dTimeC < pSqc1->time)
			break;
	}

	w0 = (pSqc1->time - m_dTimeC)/(pSqc1->time - pSqc0->time);
	w1 = 1.0F - w0;


	m_Sqc.time	= m_dTimeC;
	m_Sqc.pos	= pSqc0->pos * w0 + pSqc1->pos * w1;
	m_Sqc.scl	= pSqc0->scl * w0 + pSqc1->scl * w1;
	m_Sqc.roc	= pSqc0->roc * w0 + pSqc1->roc * w1;
	m_Sqc.ror	= pSqc0->ror * w0 + pSqc1->ror * w1;
	m_Sqc.clr	= pSqc0->clr * w0 + pSqc1->clr * w1;

	return LC_OK;
}


INT CLcxMoSqc2d::SetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if(LC_SUCCEEDED(CLcxMoSqc::SetAttrib(dCmd, pData)))
		return LC_OK;


	return LC_EFAIL;
}


INT CLcxMoSqc2d::GetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if(LC_SUCCEEDED(CLcxMoSqc::GetAttrib(dCmd, pData)))
		return LC_OK;

	if( dCmd == LCXMO_RESULT_SQC)
	{
		memcpy(pData, &m_Sqc, sizeof(TLCX_MOSQ2D));
		return m_nSqcStt;
	}
	else
		return LC_EFAIL;


	return LC_EFAIL;
}







