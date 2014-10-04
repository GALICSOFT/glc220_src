// Implementation of the CLcxTda class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
  #include "ILcxTda.h"
#else
  #include "../LcxEntity.h"
#endif

#include "LcxTda.h"


vector<PLCX_TDA>	CLcxTda::g_lstTda;


PLCX_TDA ILCX_Tda::Find(INT nIdx)
{
	if( 0 > nIdx || nIdx >=(INT)CLcxTda::g_lstTda.size())
		return NULL;

	return CLcxTda::g_lstTda[nIdx];
}


INT ILCX_Tda::FindIdx(char* sFile)
{
	CLcxTda* pLst = NULL;
	INT iSize = (INT)CLcxTda::g_lstTda.size();

	if(NULL==sFile)
		return iSize;


	for(INT n=0; n<iSize; ++n)
	{
		pLst = (CLcxTda*)CLcxTda::g_lstTda[n];
		INT   len  = *((INT*)pLst->m_sId);
		char* name = pLst->m_sId + sizeof(INT);

		if(0 == LcStr_Strncmp(sFile, name, len))
			return n;
	}

	return -1;
}


INT ILCX_Tda::ListRelease()
{
	INT iSize = (INT)CLcxTda::g_lstTda.size();

	for(INT n=0; n<iSize; ++n)
	{
		if(NULL == CLcxTda::g_lstTda[n])
			continue;
		
		CLcxTda::g_lstTda[n]->Release();
	}

	CLcxTda::g_lstTda.clear();

	return LC_OK;
}


////////////////////////////////////////////////////////////////////////////////
INT CLcxTda::PopFromList(PLCX_TDA pTda)
{
	INT iSize = (INT)CLcxTda::g_lstTda.size();

	for(INT n=0; n<iSize; ++n)
	{
		if(pTda != CLcxTda::g_lstTda[n] || NULL == CLcxTda::g_lstTda[n])
			continue;

		CLcxTda::g_lstTda[n] = NULL;
		break;
	}

	return LC_OK;
}


INT CLcxTda::Add(CLcxTda* pTda)
{
	INT n = 0;
	INT iSize = (INT)CLcxTda::g_lstTda.size();


	char* sId = pTda->m_sId + sizeof(INT);
	if(LC_SUCCEEDED(ILCX_Tda::FindIdx(sId)))
		return LC_EFAIL;


	for(n=0; n<iSize; ++n)
	{
		if(NULL == CLcxTda::g_lstTda[n])
			break;
	}


	if(n == iSize)
		CLcxTda::g_lstTda.push_back(pTda);
	else
		CLcxTda::g_lstTda[n] = pTda;

	return LC_OK;
}

