// Implementation of the ILcmFld class.
//
////////////////////////////////////////////////////////////////////////////////


#include <algorithm>

#include <windows.h>
#include <d3d9.h>
#include <d3dX9.h>

#include "../11gldc22/LnD3Type.h"

#include <LnUtil.h>
#include <LnxEntity.h>

#include "LntEntity.h"

#include "LnmSptB.h"
#include "LnmSptD.h"
#include "LnmTileB.h"
#include "LnmTileD.h"



INT LcMap_CreateOutdoor(char* sCmd
				, ILcmFld** pData
				, void* p1			// Map File Name
				, void* p2			// Texture Table
				, void* p3			// Model Table
				, void* p4			// D3DXEffect
				, void* p5			// No Use
				, void* p6			// No Use
				)
{
	ILcmFld* pObj	= NULL;

	(*pData) = NULL;

	char*	sFile = (char*)p2;
	char	Ext[_MAX_EXT]={0};
	char*	p  = NULL;

	p = strrchr(sFile, '.');
	strcpy(Ext, p+1);


	if(0==_stricmp(Ext, "bos"))
	{
		pObj = new CLcmSptD;
	}

	else if(0==_stricmp(Ext, "bot"))
	{
		pObj = new CLcmTileD;
	}
	else
	{
		return -1;
	}


	pObj->Query("Set HLSL", p5);
	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;
	return 0;
}



