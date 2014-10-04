// Implementation of the LcxCam3D.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcAux.h>
#include <LcMath.h>
#include <LcBase.h>

#include "LcxEntity.h"

#include "LcxCam.h"
#include "LcxCam3D.h"
#include "LcxCam2D.h"



INT LcxEnt_CreateCamera(char* sCmd
				   , PLCX_CAMERA* pOut		// Output instance
				   , LC_HANDLE p1			// User Data(Option)
				   , LC_HANDLE p2			// No Use
				   , LC_HANDLE p3			// No Use
				   , LC_HANDLE p4			// No Use
					 )
{
	PLCX_CAMERA pObj = NULL;

	*pOut = NULL;

	if(NULL == sCmd)
	{
		CLcxCam3D* pObj3D = new CLcxCam3D;
		if(LC_FAILED(pObj3D->Create(p1, p2, p3, p4)))
		{
			delete pObj3D;
			return LC_CANNT_ALLOC;
		}

		pObj = pObj3D;
	}

	else if( !LcStr_Stricmp(sCmd, LCXCMD_CAMERA_2D))
	{
		CLcxCam2D* pObj2D = new CLcxCam2D;
		if(LC_FAILED(pObj2D->Create(p1, p2, p3, p4)))
		{
			delete pObj2D;
			return LC_CANNT_ALLOC;
		}

		pObj = pObj2D;
	}

	else
		return LC_EFAIL;


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}

