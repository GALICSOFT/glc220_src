// Implementation of the LCX_MoSqc2d.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1200
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
#include "LcxMoSqc3d.h"



TLCX_MOSQIC2D::TLCX_MOSQIC2D()
{
	Reset();
}


TLCX_MOSQIC2D::TLCX_MOSQIC2D(const TLCX_MOSQIC2D& v)
{
	time	= v.time;
	pos		= v.pos;
	scl		= v.scl;
	roc		= v.roc;
	ror		= v.ror;
	clr		= v.clr;

	nIdx	= v.nIdx;

	memset(name, 0, sizeof name);
	memset(keys, 0, sizeof keys);

	memcpy(name, v.name, sizeof name);
	memcpy(keys, v.keys, sizeof keys);

	psa		= v.psa;
	psv		= v.psv;
	scv		= v.scv;
	rov		= v.rov;
	clv		= v.clv;
}


TLCX_MOSQIC2D::TLCX_MOSQIC2D(char* sName, int TimeOut, int* KeyList)
{
	Reset();
	strcpy(name, sName);
	time	= TimeOut;

	if(KeyList)
	{
		for(int i=0; 0 != KeyList[i]; ++i)
			keys[i] = KeyList[i];
	}
}


TLCX_MOSQIC2D& TLCX_MOSQIC2D::operator =(const TLCX_MOSQIC2D& v)
{
	time	= v.time;
	pos		= v.pos;
	scl		= v.scl;
	roc		= v.roc;
	ror		= v.ror;
	clr		= v.clr;

	nIdx	= v.nIdx;

	memset(name, 0, sizeof name);
	memset(keys, 0, sizeof keys);

	memcpy(name, v.name, sizeof name);
	memcpy(keys, v.keys, sizeof keys);

	psa		= v.psa;
	psv		= v.psv;
	scv		= v.scv;
	rov		= v.rov;
	clv		= v.clv;

	return *this;
}


void TLCX_MOSQIC2D::Reset()
{
	time	= 0;
	pos		= LCXVECTOR2(0,0);
	scl		= LCXVECTOR2(1,1);
	roc		= LCXVECTOR2(0,0);
	ror		= FLOAT     (0  );
	clr		= LCXCOLOR	(1,1,1,1);

	nIdx	= -1;

	memset(name, 0, sizeof name);
	memset(keys, 0, sizeof keys);

	psa = LCXVECTOR2(0,0);
	psv = LCXVECTOR2(0,0);
	scv = LCXVECTOR2(0,0);
	rov	= FLOAT     (0  );
	clv = LCXCOLOR	(0,0,0,0);
}


TLCX_MOSQ2D::TLCX_MOSQ2D()
: time(0)
, pos(0,0)
, scl(1,1)
, roc(0,0)
, ror(0)
, clr(1,1,1,1)
{
}



CLcxMoSqc::CLcxMoSqc()
: m_Type(LCXMO_CART_INTP)
, m_dTimeC(0.0)
, m_nSqcStt(LC_DEFAULT)
, m_nSqcCur(-1)
, m_pSqcBgn(NULL)
, m_pSqcEnd(NULL)
{
}


CLcxMoSqc::~CLcxMoSqc()
{
}


INT CLcxMoSqc::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}

INT CLcxMoSqc::Release()
{
	INT hr = 0;

	LC_HANDLE pOwn = NULL;
	hr = this->Query(LCQUERY_GET_OWNER, &pOwn);
	if(LC_SUCCEEDED(hr))	// if there is owner then this cannot be released
		return LC_EFAIL;

	// delete Object
	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


INT CLcxMoSqc::SetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if( dCmd == LCXMO_ADVANCE_TIME)
	{
		DOUBLE fElapsed = *((DOUBLE*)pData);

		if(LCXMO_DEFAULT == *((UINT*)&m_nSqcStt) || LCXMO_FINISH == m_nSqcStt)
			return LC_OK;

		m_dTimeC += fElapsed;
		return LC_OK;
	}

	else if( dCmd == LCXMO_CURRENT_TIME)
	{
		DOUBLE fTime = *((DOUBLE*)pData);
		m_dTimeC = fTime;
		return LC_OK;
	}
	else if( dCmd == LCXMO_BEGIN)
	{
		m_dTimeC = 0;
		m_nSqcStt  = LCXMO_BEGIN;

		return LC_OK;
	}
	else
		return LC_EFAIL;


	return LC_OK;
}


INT CLcxMoSqc::GetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if( dCmd == LCXMO_CURRENT_TIME)
	{
		*((DOUBLE*)pData) = m_dTimeC;
		return LC_OK;
	}
	else
		return LC_EFAIL;


	return LC_OK;
}





INT LcxEnt_CreateMoSqc( char*     sCmd		// Command.
					, PLCX_MOSQC* pOut		// Output data
					, LC_HANDLE    p1		// Motion Sequence List. => TLCX_MOTSQC pointer
					, LC_HANDLE    p2		// List Counter
					, LC_HANDLE    p3		// No Use
					, LC_HANDLE    p4		// No Use
					)
{
	INT        hr   = LC_OK;
	PLCX_MOSQC pObj = NULL;

	*pOut	= NULL;

	if(NULL == sCmd || strlen(sCmd)<4)
		return LC_EFAIL;


	// Original
	if(0 == LcStr_Stricmp(sCmd, LCXCMD_MOSQC_2D))
	{
		pObj = new CLcxMoSqc2d;
		hr = ((CLcxMoSqc2d*)pObj)->Create(p1, p2, p3, p4);
	}

	else if(0 == LcStr_Stricmp(sCmd, LCXCMD_MOSQC_3D))
	{
		pObj = new CLcxMoSqc3d;
		hr = ((CLcxMoSqc3d*)pObj)->Create(p1, p2, p3, p4);
	}

	else
		return LC_EFAIL;


	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_EFAIL;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}


