// Implementation of the ILcVtx class.
//
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "LnxEntity.h"

#include "LnEntBill.h"
#include "LnGeoGrid.h"
#include "LnGeoAxis.h"


////////////////////////////////////////////////////////////////////////////////

INT	LcEnt_Create(char* sCmd					// Command
					, ILcEnt** pData		//
					, void* p1				// Billboard Count or Etc...
					, void* p2				//
					, void* p3				//
					, void* p4				//
					)

{
	ILcEnt*	pObj = NULL;

	(*pData) = NULL;


	if(0==_stricmp("Billboard", sCmd))
		pObj	= new CLcEntBill;

	else if(0==_stricmp("Grid", sCmd))
		pObj = new CLcGeoGrid;

	else if(0==_stricmp("Axis", sCmd))
		pObj = new CLcGeoAxis;

	else
		return -1;


	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;
	return 0;
}

