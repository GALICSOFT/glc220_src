// Implementation of the ILcCam class.
//
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <stdio.h>
#include <d3dx9.h>

#include "LnxEntity.h"
#include "LnCam.h"
#include "LnCamA.h"


INT LcCam_Create(char* sCmd
				, ILcCam** pData
				, void* p1		// Camera name
				, void* p2		// Camera Initial Value
				, void* p3		// LPDIRECT3DDEVICE9
				, void* p4		// No Use
				, void* p5		// No Use
				, void* p6		// No Use
				)
{
	ILcCam* pObj = NULL;
	
	(*pData) = NULL;

	pObj = new CLcCamA;

	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;

	return 0;
}