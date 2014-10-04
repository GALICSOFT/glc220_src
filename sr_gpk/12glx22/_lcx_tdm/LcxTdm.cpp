// Implementation of the CLcxTdmT class.
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


CLcxTdm::CLcxTdm()
{
	m_pSpl	= NULL;
	m_pInf	= NULL;
	m_pTsp	= NULL;

	m_at.dTimeB	= LcSys_TimeGetTime();
	m_at.dTimeC	= m_at.dTimeB;

	m_nRpt	= LC_INFINITE;
	m_nSts	= 0xFFFFFFFF;
}

CLcxTdm::~CLcxTdm()
{
	Destroy();
}


INT CLcxTdm::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxTdm::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcxTdm::Destroy()
{
	if(m_pInf)
	{
		if(this == m_pInf->pOrg)
		{
			SAFE_DELETE( m_pInf	);
		}
	}

	m_pTsp	= NULL;
}


INT CLcxTdm::CreateFromFile(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT		hr    = 0;

	char*	sFile = (char*)p1;
	char	sBin  [LC_MAX_PATH2]={0};
	LcStr_ReplacePath(sBin, sFile, NULL, NULL, NULL, ".gtdm");

	if(p2)
		this->m_pSpl = (PLC_SPRITE_L)p2;

#if defined(__ANDROID__)

	BYTE*		buf = NULL;
	ULONG		len = 0;
	const char*	sApkFile = LcSys_AndroidApkFile(NULL);

	hr = Lzip_UnzipbyName(&buf, &len, sApkFile, sBin, LC_TRUE);
	LOGI("Create Tdm Unzip: %x %ld %d %s", (UINT)buf, len, hr, sFile);

	if(LC_FAILED(hr))
		return LC_CANNT_ALLOC;

	hr = CreateFromMemory(sBin, buf, &len);
	SAFE_FREE(	buf	);

	return hr;
#endif


	char	sMedia[LC_MAX_PATH2]={0};
	char	ext   [LC_MAX_EXT  ]={0};
	INT		nType = LC_OBJ_UNKNOWN;

	m_pInf = new TsptInf;
	m_pInf->pOrg = this;

	char*	sId	 = m_pInf->sId + sizeof(INT);
	*((INT*)m_pInf->sId) = LcStr_ReplacePath(sId, sFile, NULL, NULL, NULL, "");



	LcStr_SplitPath(sFile, NULL, NULL, NULL, ext);
	if(strstr(ext, ".tdm"))
		nType = LC_OBJ_XML;

	else if(strstr(ext, ".tdt"))
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


	CLcxTdm::Add(this);

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

INT CLcxTdm::CreateFromMemory(PLC_ARGS pArg)
{
	if(L_NIL == pArg->p0 || L_NIL == pArg->p1)
		return LC_CANNT_ALLOC;

	char*	sFile = (char*)pArg->p0;
	BYTE*	buf   = (BYTE*)pArg->p1;
	ULONG	len   = (ULONG)pArg->p2;

	if(L_NIL != pArg->p3)
		*((LC_HANDLE*)&this->m_pSpl) = pArg->p3;

	INT		hr    = 0;

	m_pInf = new TsptInf;
	m_pInf->pOrg = this;

	char*	sId	  = m_pInf->sId + sizeof(INT);
	*((INT*)m_pInf->sId) = LcStr_ReplacePath(sId, sFile
										, NULL, NULL, NULL, "");

	hr = LoadFromMemory(buf, &len);
	LOGI("Create Tdm From Memory hr: %d", hr);
	if(LC_FAILED(hr))
	{
		SAFE_DELETE( m_pInf );
		return hr;
	}

	CLcxTdm::Add(this);

	// increase the reference count.
	LcVar_IncRefCountObject();
	return LC_OK;
}


INT CLcxTdm::SetAttrib(UINT dCmd, LC_HANDLE pData)
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

		case LCXM_REPEAT_COUNT:
		{
			INT nRpt = LC_INFINITE;
			if(NULL != pData)
				nRpt = *((INT*)pData);

			m_nRpt = nRpt;
			return LC_OK;
		}

		case LCXM_ANIMATION:
		case LCXM_ANIMATION_ABS:
		{
			char* sAniName = (char*)pData;

			INT nAniIdx = FindAniIdx(sAniName);
			if(LC_FAILED(nAniIdx))
				return LC_EFAIL;

			m_at.nAniC = nAniIdx;
			m_at.nAniS = 0;
			m_at.dTimeB	= LcSys_TimeGetTime();
			return LC_OK;
		}

		case LCXM_SPRITE_LAYER:
		{
			PLC_SPRITE_L m_pSpl = (PLC_SPRITE_L)pData;
			return LC_OK;
		}
		case LCXM_EXEC_TEXTURE_LOAD:
		{
			INT			n=0, j=0;
			TsptAni*	pAni = NULL;
			TsptRec*	pRec = NULL;

			for(n=0; n<m_pInf->nAni; ++n)
			{
				pAni = &m_pInf->vAni[n];

				for(j=0; j<pAni->nRec; ++j)
				{
					pRec = &pAni->vRec[j];
					pRec->pTda->SetAttrib(LCXM_EXEC_TEXTURE_LOAD, pData);
				}
			}
			return LC_OK;
		}
	}

	return LC_EFAIL;
}


INT CLcxTdm::GetAttrib(UINT dCmd, LC_HANDLE pData)
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

		case LCXM_ANIMATION:
		case LCXM_ANIMATION_ABS:
		{
			PLC_ARGS pArg = (PLC_ARGS)pData;

			pArg->p0 = m_pInf->vAni[m_at.nAniC].sName;
			pArg->p1 = m_at.nAniC;
			return LC_OK;
		}

		case LCXM_SRC_ID:
		{
			char* sId = m_pInf->sId + sizeof(INT);
			char* str = (char*)pData;
			strcpy(str, sId);
			return LC_OK;
		}

		case LCXM_CREAE_COLONE:
		{
			CLcxTdm* pObj = new CLcxTdm;

			pObj->m_pInf = this->m_pInf;

			if(m_pSpl)
				pObj->m_pSpl = this->m_pSpl;

			LcVar_IncRefCountObject();
			*((CLcxTdm**)pData) = pObj;
			return LC_OK;
		}
	}


	return LC_EFAIL;
}


INT CLcxTdm::FrameMove()
{
	INT st = (m_nSts & LCXM_ST_UPDATE)<<4;
	if(!st)
		return 0;


	if(m_pTsp)
		return 0;


	TsptAni* pAni = &m_pInf->vAni[m_at.nAniC];
	TsptRec* pRec = &pAni->vRec[pAni->nRec-1];
	INT		 TimeLast = pRec->Time;


	if(0 == m_nRpt)
	{
		if(-1 == pAni->nNxt)
		{
			m_at.nAniS = pAni->nRec-1;
			return LC_OK;
		}
		else
		{
			m_at.nAniC = pAni->nNxt;
			m_at.nAniS = 0;
			m_at.dTimeB	= LcSys_TimeGetTime();

			pAni = &m_pInf->vAni[m_at.nAniC];
			pRec = &pAni->vRec[pAni->nRec-1];
			TimeLast = pRec->Time;

			m_nRpt = pAni->nRpt;
		}
	}


	m_at.dTimeC = LcSys_TimeGetTime();
	INT dTime = INT(m_at.dTimeC - m_at.dTimeB);
	INT dRepeat = 0;

	dRepeat = dTime/TimeLast;
	dTime   = dTime%TimeLast;

	if(LC_INFINITE != m_nRpt && 0 < dRepeat)
	{
		m_nRpt -= dRepeat;
		m_at.dTimeB	= m_at.dTimeC - dTime;

		if(0>=m_nRpt)
			m_nRpt = 0;

		//printf("-------------- %d  ,  %d\n", m_nRpt, dRepeat);
	}


	// find frame

	if(0 != m_nRpt)
	{
		INT nFrame = FindAniSeq(m_at.nAniC, dTime);
		m_at.nAniS = nFrame;
	}

	return LC_OK;
}



void CLcxTdm::Render()
{
	INT st = (m_nSts & LCXM_ST_RENDER)<<4;
	if(!st)
		return;

	LCXMATRIX* pTM = NULL;
	INT	nAniC	= this->m_at.nAniC;
	INT	nAniS	= this->m_at.nAniS;


	ILC_Object* pNodePrn =  this->GetParent();								// parent

	if(NULL == pNodePrn)
		pTM = (LCXMATRIX*)m_tm.GetTM();

	else
		pNodePrn->GetAttrib(LC_TS_WORLD_PT, &pTM);


	if(pTM)
		m_pSpl->SetTransform(LC_TS_WORLD, pTM);

	if(m_pTsp)
	{
		nAniC	= ((CLcxTdm*)m_pTsp)->m_at.nAniC;
		nAniS	= ((CLcxTdm*)m_pTsp)->m_at.nAniS;
	}



	TsptAni*	pAni = &m_pInf->vAni[nAniC];
	TsptRec*	pRec = &pAni->vRec[nAniS];
	PLCX_TDA	pTda = pRec->pTda;
	TLC_TDAREC* pTsc = pRec->pTsc;
	PLC_TEXTURE	pTex = pTda->GetTexture();

	FLOAT* pVtxPos = (FLOAT*)pTsc->pos;
	FLOAT* pVtxTex = (FLOAT*)pTsc->tex;
	FLOAT* pVtxCol = NULL;


	m_pSpl->Draw(pTex, pVtxPos, pVtxTex, pVtxCol);

	if(pTM)
		m_pSpl->SetTransform(LC_TS_WORLD_IDENTITY, NULL);
}


////////////////////////////////////////////////////////////////////////////////

INT CLcxTdm::FindAniIdx(char* sAniName)
{
	for(INT n=0; n<m_pInf->nAni; ++n)
	{
		char * sName = m_pInf->vAni[n].sName;
		if(0 == LcStr_Stricmp(sName, sAniName))
			return n;
	}

	return -1;
}


INT CLcxTdm::FindAniSeq(  INT nAniC	// Current ani index
						, INT dTime
						)
{
	TsptAni*	pAni = &m_pInf->vAni[nAniC];
	TsptRec*	pRec = &pAni->vRec[pAni->nRec-1];

	INT TimeLast = pRec->Time;

	// find frame
	for(INT n=0; n<pAni->nRec-1; ++n)
	{
		if(pAni->vRec[n].Time <= dTime && dTime <= pAni->vRec[n+1].Time)
		{
			return n;
		}
	}

	return -1;
}



INT CLcxTdm::SetPosition(const LC_FLOAT2* v)
{
	this->m_tm.SetPos((LCXVECTOR2*)v);
	return LC_OK;
}


INT CLcxTdm::SetRotation(const LC_FLOAT3* v)
{
	this->m_tm.SetRot((LCXVECTOR3*)v);
	return LC_OK;
}


INT CLcxTdm::SetScaling(const LC_FLOAT2* v)
{
	this->m_tm.SetScl((LCXVECTOR2*)v);
	return LC_OK;
}


TsptInf* CLcxTdm::GetSptInf()
{
	return this->m_pInf;
}




////////////////////////////////////////////////////////////////////////////////

INT LcxEnt_CreateTdm(char* sCmd
				 , PLCX_TDM* pOut						// Output instance
				 , LC_HANDLE sName						// Model File Name
				 , LC_HANDLE pLyr						// Layer pointer. Layer pointer can be setup to SetAttrib(LCXM_SPRITE_LAYER)
				 , LC_HANDLE pOpt						// Other Option
				 , LC_HANDLE p4							// No Use or Layer
				 )
{
	INT			hr = 0;
	INT			nIdx = -1;
	CLcxTdm*	pObjO = NULL;
	CLcxTdm*	pObjD = NULL;

	char* s_name = (char*)sName;

	*pOut		= NULL;
	if(NULL == s_name || 4 > strlen(s_name) )
		return LC_CANNT_FIND_RESOURCE;

	// find original
	nIdx = ILCX_Tdm::FindIdx(s_name);
	if(LC_SUCCEEDED(nIdx))
	{
		pObjO = (CLcxTdm*)ILCX_Tdm::Find(nIdx);
		pObjO->GetAttrib(LCXM_CREAE_COLONE, &pObjD);

		*pOut = pObjD;
		return LC_OK;
	}

	// create original
	pObjO = new CLcxTdm;
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


// Create TDM Instance from memory with name, buffer pointer, and size
// PLC_ARGS option
//	p0 : sName	Model File Name
//	p1 : pMem	starting memory pointer
//	p2 : pSize	size(unsigned int) pointer
//	p3 : pLyr	Layer pointer
//	p4 : pOpt	Texture Load Option. It needs TLC_TEXTURE pointer

INT LcxEnt_CreateTdmFromMemory(char* sCmd, PLCX_TDM* pOut, PLC_ARGS	pArg)
{
	INT			hr = 0;
	INT			nIdx = -1;
	CLcxTdm*	pObjO = NULL;
	CLcxTdm*	pObjD = NULL;

	char* s_name = (char*)pArg->p0;

	*pOut		= NULL;
	if( L_NIL == s_name || 4 > strlen(s_name) ||
		L_NIL == pArg->p1 || L_NIL == pArg->p2)
		return LC_CANNT_FIND_RESOURCE;


	// find original
	nIdx = ILCX_Tdm::FindIdx(s_name);
	if(LC_SUCCEEDED(nIdx))
	{
		pObjO = (CLcxTdm*)ILCX_Tdm::Find(nIdx);
		pObjO->GetAttrib(LCXM_CREAE_COLONE, &pObjD);

		*pOut = pObjD;
		return LC_OK;
	}

	// create original
	pObjO = new CLcxTdm;
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


