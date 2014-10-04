// Implementation of the ILcpObj class.
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
#include <LnxEntity.h>

#include "LntEntity.h"
#include "LnpObj.h"



INT LcpObj_Create(char* sCmd
				 , ILcpObj** pData
				 , void* p1			// Original Source: Original Source가 NULL이면 새로 만듦
				 , void* p2			// Model File Name
				 , void* p3			// No Use
				 , void* p4			// No Use
				 )
{
	ILcpObj* pObj	= NULL;
	
	(*pData) = NULL;

	pObj = new CLcpObj;

	// Initialize Object
	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;
	return 0;
}

