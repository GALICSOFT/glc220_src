// Implementation of the ILcxMdl class.
//
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "LntEntity.h"

#include "TblTex.h"
#include "TblMdl.h"


INT LcTbl_Create(char* sCmd
				 , ILcTbl** pData
				 , void* p1			// File Name
				 , void* p2			// Etc
				 , void* p3			// No Use
				 , void* p4			// No Use
				 )
{
	ILcTbl*	pObj = NULL;

	(*pData) = NULL;

	if(0==_stricmp("Texture", sCmd))
	{
		pObj = new CTblTex;
	}

	else if(0==_stricmp("Model", sCmd))
	{
		pObj = new CTblMdl;
	}
	else
	{
		return -1;
	}


	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;
	return 0;
}



