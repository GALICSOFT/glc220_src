// Implementation of the CLcxAst class.
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
#include <LcBase.h>
#include <LcMath.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
#else
  #include "../LcxEntity.h"
#endif

#include "LcxAse.h"
#include "LcxAst.h"
#include "LcxAsb.h"
#include "LcxAsbIns.h"

#include "LcxAcm.h"
#include "LcxAcmIns.h"



INT LcxEnt_CreateMdl(char* sCmd
				 , PLCX_MDL*  pOut		// Output instance
				 , LC_HANDLE sName		// Model File Name
				 , LC_HANDLE pOrg		// Original LCX_Mdl Pointer for Clone Creating
				 , LC_HANDLE p3			// Not Use
				 , LC_HANDLE p4			// Not Use
				 )
{
	PLCX_MDL pObj = NULL;

	*pOut	= NULL;



	// Original
	if(sName)
	{
		char*	sSrc =(char*)sName;
		char	sExt[LC_MAX_EXT ]={0};

		LcStr_SplitPath(sSrc, NULL, NULL, NULL, sExt );


		if(0 == LcStr_Stricmp(sExt, ".acm"))
		{
			pObj = new CLcxAcm;
			if(LC_FAILED( ((CLcxAcm*)pObj)->Create(sName)))
			{
				delete pObj;
				return LC_CANNT_ALLOC;
			}
		}

		else if(0 == LcStr_Stricmp(sExt, ".ase"))
		{
			// 1. load ase
			// 2. make asb
			// 4. load asb

#if defined(_WIN32) || defined(_PC_LINUX_)
			// ASEfile이면 Binary로 만든 다음 다시 로드
			pObj = new CLcxAst;
			if(LC_FAILED( ((CLcxAst*)pObj)->Create(sName)))
			{
				delete pObj;
				return LC_CANNT_ALLOC;
			}

			delete pObj;
			pObj = NULL;
#endif


			char drive[LC_MAX_DRIVE]={0};
			char dir  [LC_MAX_DIR  ]={0};
			char fname[LC_MAX_FNAME]={0};
			char ext  [LC_MAX_EXT  ]=".asb";
			char sFile[LC_MAX_PATH ]={0};

			LcStr_SplitPath(sSrc, drive, dir, fname, NULL );

			sprintf(sFile, "%s%s%s%s", drive, dir, fname, ext);



			pObj = new CLcxAsb;
			if(LC_FAILED( ((CLcxAsb*)pObj)->Create(sFile)))
			{
				delete pObj;
				return LC_CANNT_ALLOC;
			}

		}

		else if(0 == LcStr_Stricmp(sExt, ".asb"))
		{
			pObj = new CLcxAsb;
			if(LC_FAILED(  ((CLcxAsb*)pObj)->Create(sName)))
			{
				delete pObj;
				return LC_CANNT_ALLOC;
			}
		}
	}

	else if( pOrg)
	{
		PLCX_MDL pSrc = (PLCX_MDL)pOrg;
		INT		 Type = pSrc->GetRscType();

		if(LCXM_SRC_ACM == Type)
		{
			pObj = new CLcxAcmIns;
			if(LC_FAILED( ((CLcxAcmIns*)pObj)->Create(pOrg, NULL)))
			{
				delete pObj;
				return LC_CANNT_ALLOC;
			}
		}
		else if(LCXM_SRC_ASE == Type)
		{
			pObj= new CLcxAsbIns;
			if(LC_FAILED( ((CLcxAsbIns*)pObj)->Create(pOrg, NULL)))
			{
				delete pObj;
				return LC_CANNT_ALLOC;
			}
		}
		else
			return LC_CANNT_ALLOC;
	}

	else
		return LC_EFAIL;


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}


