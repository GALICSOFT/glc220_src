// Implementation of the CLcxTdsT class.
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
  #include "ILcxTdm.h"
#else
  #include "../LcxEntity.h"
#endif

#include "LcxTdm.h"
#include "LcxTds.h"


CLcxTds::CLcxTds()
{
	m_pSpl	= NULL;
	m_pInf	= NULL;

	m_pPts	= NULL;
}


CLcxTds::~CLcxTds()
{
	Destroy();
}


INT CLcxTds::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxTds::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcxTds::Destroy()
{
	if(m_pInf)
	{
		if(this == m_pInf->pOrg)
		{
			SAFE_DELETE( m_pInf	);
		}
	}

	SAFE_DELETE_ARRAY(	m_pPts	);
}


INT CLcxTds::CreateFromFile(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT		hr    = 0;

	char*	sFile = (char*)p1;
	char	sBin  [LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sBin, sFile, NULL, NULL, NULL, ".gtds");

	if(p2)
		this->m_pSpl = (PLC_SPRITE_L)p2;

#if defined(__ANDROID__)

	BYTE*		buf = NULL;
	ULONG		len = 0;
	const char*	sApkFile = LcSys_AndroidApkFile(NULL);

	hr = Lzip_UnzipbyName(&buf, &len, sApkFile, sBin, LC_TRUE);
	LOGI("Create Tds Unzip: %x %ld %d %s", (UINT)buf, len, hr, sFile);

	if(LC_FAILED(hr))
		return LC_CANNT_ALLOC;

	hr = CreateFromMemory(sBin, buf, &len);
	SAFE_FREE(	buf	);

	return hr;
#endif


	char	sMedia[LC_MAX_PATH2]={0};
	char	ext   [LC_MAX_EXT  ]={0};
	INT		nType = LC_OBJ_UNKNOWN;

	m_pInf = new TspInf;
	m_pInf->pOrg = this;

	char*	sId	 = m_pInf->sId + sizeof(INT);
	*((INT*)m_pInf->sId) = LcStr_ReplacePath(sId, sFile, NULL, NULL, NULL, "");



	LcStr_SplitPath(sFile, NULL, NULL, NULL, ext);
	if(strstr(ext, ".tds"))
		nType = LC_OBJ_XML;

	else if(strstr(ext, ".tdz"))
		nType = LC_OBJ_NSD;

	else
		goto END;


	if(LC_FAILED(LcSys_ResourcePath(sMedia, sBin)))
	{
#if defined(__APPLE__) || defined(__IPHONE__)
		return LC_CANNT_FIND_RESOURCE;
#endif

		hr = LC_CANNT_ALLOC;

		if     (LC_OBJ_XML == nType)
			hr = LoadFromXML(sFile);				// Load Tda File for xml

		else if(LC_OBJ_NSD == nType)
			hr = LoadFromTxt(sFile);				// Load from text


		if(LC_FAILED(hr))
			goto END;


		m_pInf->Destroy();
		m_pInf->pOrg = this;
		sId	 = m_pInf->sId + sizeof(INT);
		*((INT*)m_pInf->sId) = LcStr_ReplacePath(sId, sFile, NULL, NULL, NULL, "");
	}

	// load from binary
	hr = LoadFromBin(sMedia);
	if(LC_FAILED(hr))
		goto END;


	CLcxTds::Add(this);

	// increase the reference count.
	LcVar_IncRefCountObject();
	return LC_OK;

END:
	SAFE_DELETE( m_pInf );
	return hr;
}


// PLC_ARGS option
//	p0 : sName	Model File Name
//	p1 : pMem	starting memory pointer
//	p2 : pSize	size(unsigned int) pointer
//	p3 : pLyr	Layer pointer
//	p4 : pOpt	Texture Load Option. It needs TLC_TEXTURE pointer

INT CLcxTds::CreateFromMemory(PLC_ARGS pArg)
{
	if(L_NIL == pArg->p0 || L_NIL == pArg->p1)
		return LC_CANNT_ALLOC;

	char*	sFile = (char*)pArg->p0;
	BYTE*	buf   = (BYTE*)pArg->p1;
	ULONG	len   = (ULONG)pArg->p2;

	if(L_NIL != pArg->p3)
		*((LC_HANDLE*)&this->m_pSpl) = pArg->p3;

	INT		hr    = 0;

	m_pInf = new TspInf;
	m_pInf->pOrg = this;

	char*	sId	  = m_pInf->sId + sizeof(INT);
	*((INT*)m_pInf->sId) = LcStr_ReplacePath(sId, sFile
										, NULL, NULL, NULL, "");

	hr = LoadFromMemory(buf, &len);
	LOGI("Create Tds From Memory hr: %d", hr);
	if(LC_FAILED(hr))
	{
		SAFE_DELETE( m_pInf );
		return hr;
	}

	CLcxTds::Add(this);

	// increase the reference count.
	LcVar_IncRefCountObject();
	return LC_OK;
}


INT CLcxTds::SetAttrib(UINT dCmd, LC_HANDLE pData)
{
	switch(dCmd)
	{
		case LCXM_ADVANCE_TIME:
		{
			DOUBLE fElapsed = *((DOUBLE*)pData);
			return LC_OK;
		}

		case LCXM_CURRENT_TIME:
		{
			DOUBLE fTime = *((DOUBLE*)pData);
			return LC_OK;
		}

		case LCXM_SPRITE_LAYER:
		{
			PLC_SPRITE_L m_pSpl = (PLC_SPRITE_L)pData;
			return LC_OK;
		}
	}

	return LC_EFAIL;
}


INT CLcxTds::GetAttrib(UINT dCmd, LC_HANDLE pData)
{
	switch(dCmd)
	{
		case LCXM_CURRENT_TIME:
		{
			return LC_OK;
		}
		case LC_TS_WORLD:
		{
			memcpy(pData, this->m_tm.GetTM(), sizeof(LCXMATRIX) );
			return LC_OK;
		}

		case LC_TS_WORLD_PT:
		{
			*((LCXMATRIX**)pData) = (LCXMATRIX*)this->m_tm.GetTM();
			return LC_OK;
		}

		case LCXM_CREAE_COLONE:
		{
			CLcxTds* pObj = new CLcxTds;

			pObj->m_pInf = this->m_pInf;

			TdsPts* pPts = new TdsPts[m_pInf->nPts];


			TdsPts*		vPtsO = NULL;
			PLCX_TDM	pTdmO = NULL;

			TdsPts*		vPtsD = NULL;
			PLCX_TDM	pTdmL = NULL;
			PLCX_TDM	pTdmS = NULL;
			INT			n=0, j=0;

			for(n=0; n<m_pInf->nPts; ++n)
			{
				vPtsO = &m_pInf->vPts[n];
				vPtsD = &pPts[n];

				pTdmO = vPtsO->pTdm;
				pTdmO->GetAttrib(LCXM_CREAE_COLONE, &pTdmL);

				pObj->Attach(pTdmL);
				vPtsD->pTdm = pTdmL;
				vPtsD->nRec = vPtsO->nRec;

				if( 0>= vPtsD->nRec)
					continue;


				vPtsD->vRec = new PLCX_TDM[vPtsD->nRec];

				for(j=0; j<vPtsD->nRec; ++j)
				{
					pTdmO = vPtsO->vRec[j];
					pTdmO->GetAttrib(LCXM_CREAE_COLONE, &pTdmS);

					pObj->Attach(pTdmS);
					((CLcxTdm*)pTdmS)->SetTsp(pTdmL);
					vPtsD->vRec[j] = pTdmS;
				}
			}

			pObj->m_pSpl = this->m_pSpl;
			pObj->m_pPts = pPts;


			LcVar_IncRefCountObject();
			*((CLcxTds**)pData) = pObj;
			return LC_OK;
		}
	}


	return LC_EFAIL;
}


INT CLcxTds::FrameMove()
{
	TdsPts*		vPts = NULL;
	PLCX_TDM	pTdm = NULL;
	INT			n=0;


	for(n=0; n<m_pInf->nPts; ++n)
	{
		vPts = &m_pPts[n];

		pTdm = vPts->pTdm;
		pTdm->FrameMove();
	}

	return LC_OK;
}



void CLcxTds::Render()
{
	TdsPts*		vPts = NULL;
	PLCX_TDM	pTdm = NULL;
	INT			n=0, j=0;

	if(NULL == m_pPts)
		return;


	for(n=0; n<m_pInf->nPts; ++n)
	{
		vPts = &m_pPts[n];

		pTdm = vPts->pTdm;

		pTdm->Render();

		for(j=0; j<vPts->nRec; ++j)
		{
			pTdm = vPts->vRec[j];
			pTdm->Render();
		}
	}

}


////////////////////////////////////////////////////////////////////////////////

INT CLcxTds::FindPtsIdx(char* sPtsName)
{
	//for(INT n=0; n<m_pInf->nPts; ++n)
	//{
	//	char * sName = m_pInf->vPts[n].sName;
	//	if(0 == LcStr_Stricmp(sName, sPtsName))
	//		return n;
	//}

	return -1;
}


INT CLcxTds::FindPtsSeq(  INT nPtsC	// Current ani index
						, INT dTime
						)
{
	//TdsPts*	pPts = &m_pInf->vPts[nPtsC];
	//PLCX_TDM	pRec = &pPts->vRec[pPts->nRec-1];

	//INT TimeLast = pRec->Time;

	//// find frame
	//for(INT n=0; n<pPts->nRec-1; ++n)
	//{
	//	if(pPts->vRec[n].Time <= dTime && dTime <= pPts->vRec[n+1].Time)
	//	{
	//		return n;
	//	}
	//}

	return -1;
}



INT CLcxTds::SetPosition(const LC_FLOAT2* v)
{
	this->m_tm.SetPos((LCXVECTOR2*)v);
	return LC_OK;
}


INT CLcxTds::SetRotation(const LC_FLOAT3* v)
{
	this->m_tm.SetRot((LCXVECTOR3*)v);
	return LC_OK;
}


INT CLcxTds::SetScaling(const LC_FLOAT2* v)
{
	this->m_tm.SetScl((LCXVECTOR2*)v);
	return LC_OK;
}


TspInf* CLcxTds::GetSptInf()
{
	return this->m_pInf;
}




////////////////////////////////////////////////////////////////////////////////

INT LcxEnt_CreateTds(char* sCmd
				 , PLCX_TDS* pOut						// Output instance
				 , LC_HANDLE sName						// Model File Name
				 , LC_HANDLE pLyr						// Layer pointer. Layer pointer can be setup to SetAttrib(LCXM_SPRITE_LAYER)
				 , LC_HANDLE pOpt						// Other Option
				 , LC_HANDLE p4							// No Use or Layer
				 )
{
	INT			hr = 0;
	INT			nIdx = -1;
	CLcxTds*	pObjO = NULL;
	CLcxTds*	pObjD = NULL;

	char* s_name = (char*)sName;

	*pOut		= NULL;
	if(NULL == s_name || 4 > strlen(s_name) )
		return LC_CANNT_FIND_RESOURCE;

	// find original
	nIdx = ILCX_Tds::FindIdx(s_name);
	if(LC_SUCCEEDED(nIdx))
	{
		pObjO = (CLcxTds*)ILCX_Tds::Find(nIdx);
		pObjO->GetAttrib(LCXM_CREAE_COLONE, &pObjD);

		*pOut = pObjD;
		return LC_OK;
	}

	// create original
	pObjO = new CLcxTds;
	hr = pObjO->CreateFromFile(sName, pLyr, pOpt, p4);
	if(LC_FAILED(hr))
	{
		delete pObjO;
		return LC_CANNT_ALLOC;
	}

	pObjO->GetAttrib(LCXM_CREAE_COLONE, &pObjD);
	*pOut = pObjD;
	return LC_OK;
}


// Create TDS Instance from memory with name, buffer pointer, and size
// PLC_ARGS option
//	p0 : sName	Model File Name
//	p1 : pMem	starting memory pointer
//	p2 : pSize	size(unsigned int) pointer
//	p3 : pLyr	Layer pointer
//	p4 : pOpt	Texture Load Option. It needs TLC_TEXTURE pointer

INT LcxEnt_CreateTdsFromMemory(char* sCmd, PLCX_TDS* pOut, PLC_ARGS	pArg)
{
	INT			hr = 0;
	INT			nIdx = -1;
	CLcxTds*	pObjO = NULL;
	CLcxTds*	pObjD = NULL;

	char* s_name = (char*)pArg->p0;

	*pOut		= NULL;
	if( L_NIL == s_name || 4 > strlen(s_name) ||
		L_NIL == pArg->p1 || L_NIL == pArg->p2)
		return LC_CANNT_FIND_RESOURCE;


	// find original
	nIdx = ILCX_Tds::FindIdx(s_name);
	if(LC_SUCCEEDED(nIdx))
	{
		pObjO = (CLcxTds*)ILCX_Tds::Find(nIdx);
		pObjO->GetAttrib(LCXM_CREAE_COLONE, &pObjD);

		*pOut = pObjD;
		return LC_OK;
	}

	// create original
	pObjO = new CLcxTds;
	hr = pObjO->CreateFromMemory(pArg);
	if(LC_FAILED(hr))
	{
		delete pObjO;
		return LC_CANNT_ALLOC;
	}

	pObjO->GetAttrib(LCXM_CREAE_COLONE, &pObjD);
	*pOut = pObjD;
	return LC_OK;
}


