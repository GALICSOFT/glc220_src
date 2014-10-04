// Implementation of the ILcpData class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <stdio.h>
#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <LnEuclid.h>

#include "LntEntity.h"

#include "LnpDataB.h"



INT LcpData_CreateBasic(char* sCmd
				 , ILcpData** pData
				 , void* p1			// No Use
				 , void* p2			// No Use
				 , void* p3			// No Use
				 , void* p4			// No Use
				 )
{
	ILcpData* pObj	= NULL;
	
	(*pData) = NULL;

	pObj = new CLcpDataB;

	// 객체를 초기화 한다.
//	if(FAILED(pObj->Create(p1, p2, p3, p4)))
//	{
//		delete pObj;
//		return -1;
//	}

	(*pData) = pObj;
	return 0;
}

