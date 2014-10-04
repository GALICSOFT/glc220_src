// Implementation of the CLcxAcm class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_PC_LINUX_)
  #include <GL/gl.h>

#elif defined(__APPLE__) || defined(__IPHONE__)
  #import <OpenGLES/ES1/gl.h>

#elif defined(__BADA__)
  #include <FGraphicsOpengl.h>
  using namespace Osp::Graphics::Opengl;

#else
  #include <gles/gl.h>

#endif

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
#else
  #include "../LcxEntity.h"
#endif

#include "LcxAcm.h"



CLcxAcm::Tmtl::Tmtl()
{
	memset(sTex, 0, LCM_TEX_NAME);
	pTex = NULL;
}

CLcxAcm::Tmtl::Tmtl(char* s, LC_HANDLE p)
{
	strcpy(sTex, s);
	pTex = (PLC_TEXTURE)p;
}


CLcxAcm::Tmtl::~Tmtl()
{
	SAFE_RELEASE(	pTex	);
}


CLcxAcm::Tgeo::Tgeo()
{
	memset(sName, 0, sizeof sName);
	nType	= 0;

	nPrn	= -1;			// No parent
	pPrn	= NULL;

	nFce	= 0;
	nVtx	= 0;
	dVtx	= 0;

	pFce	= NULL;


	pVsP	= NULL;
	pVsW	= NULL;
	pVsB	= NULL;
	pVsN	= NULL;
	pVsT	= NULL;

	pVdP	= NULL;

	nBnd	= 0;
	pBnd	= NULL;

	nAni	= 0;
	vAni	= NULL;

	nMtl	= -1;
}


CLcxAcm::Tgeo::~Tgeo()
{
	SAFE_DELETE_ARRAY(	pFce	);

	SAFE_DELETE_ARRAY(	pVsP	);
	SAFE_DELETE_ARRAY(	pVsW	);
	SAFE_DELETE_ARRAY(	pVsB	);
	SAFE_DELETE_ARRAY(	pVsN	);
	SAFE_DELETE_ARRAY(	pVsT	);

	SAFE_DELETE_ARRAY(	pVdP	);

	SAFE_DELETE_ARRAY(	vAni	);
	SAFE_DELETE_ARRAY(	pBnd	);
}


CLcxAcm::CLcxAcm()
{
	m_pMtl	= NULL;
	m_pGeo	= NULL;

	m_TimeC		= 0;
	m_FrameC	= 0;
	m_FrameB	= 0;
	m_FrameE	= 0;

	m_mtWld.Identity();

	m_BlndNum = 1;
}


CLcxAcm::~CLcxAcm()
{
	Destroy();
}


INT CLcxAcm::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxAcm::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}



void CLcxAcm::Destroy()
{
	SAFE_DELETE_ARRAY(	m_pGeo	);
	SAFE_DELETE_ARRAY(	m_pMtl	);
}


INT CLcxAcm::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	strcpy(m_sFile, (char*)p1);

	if(LC_FAILED(Load()))
		return LC_EFAIL;

	m_FrameB = m_Info.nFrmF;		// Begin Frame of Animation Loop
	m_FrameE = m_Info.nFrmL;		// End Frame of Animation Loop

	return LC_OK;
}


INT CLcxAcm::FrameMove()
{
	INT		n=0, j=0;
	INT		nFrame=0;

	if(NULL == m_pGeo)
		return LC_OK;


	// Modify Frame and Time in  Repeat Region
	// Frame = FrameSpeed * Time;
	if(m_FrameE == m_FrameB)
	{
		m_TimeC = DOUBLE(m_FrameB)/DOUBLE(m_Info.nFrmS);
	}
	else
	{
		nFrame	= INT(m_Info.nFrmS * m_TimeC);
		if( nFrame>= (m_FrameE+1)  ||nFrame< (m_FrameB))
			m_TimeC = DOUBLE(m_FrameB+0.00001)/DOUBLE(m_Info.nFrmS+0.0000001);
	}


	// Current Frame
	m_FrameC = FLOAT(m_Info.nFrmS * m_TimeC);


	for(n=0; n<m_Info.nGeo; ++n)
	{
		Tgeo*		pGeo = &m_pGeo[n];

		this->GetAniMatrix(&pGeo->mtW, m_FrameC, pGeo);
	}



	// Update Rendering Vertex Position

	MATA	mtW;
	MATA*	pmtW;
	FLOAT*	Wgx = NULL;		// BLEND WEIGHT
	INT  *	Idx = NULL;		// MATRIX Index

	for(n=0; n<m_Info.nGeo; ++n)
	{
		Tgeo*	pGeo = &m_pGeo[n];

		VEC3*	pVsP = NULL;
		VEC4*	pVsW = NULL;
		INT4*	pVsB = NULL;

		VEC3*	pVdP = NULL;


		if(pGeo->nType != LCX_MESH || 0>=pGeo->nVtx)
			continue;


		pVsP = pGeo->pVsP;
		pVsW = pGeo->pVsW;
		pVsB = pGeo->pVsB;

		pVdP = pGeo->pVdP;


		for(j=0; j<pGeo->nVtx; ++j)
		{
			if(1 == m_BlndNum)
			{
				Idx  = (INT  *)&pVsB[j];
				pmtW = &m_pGeo[ Idx[0] ].mtW;
				pmtW->TransformCoord(&pVdP[j], &pVsP[j]);
			}

			else
			{
				Wgx  = (FLOAT*)&pVsW[j];
				Idx  = (INT  *)&pVsB[j];

				// Transform Matrix = ¥ÒMi * Wi
				memset(&mtW, 0, sizeof mtW);

				for(int i=0; i<m_BlndNum; ++i)
					mtW += Wgx[i] * m_pGeo[ Idx[i] ].mtW;

				mtW.TransformCoord(&pVdP[j], &pVsP[j]);
			}


		}
	}


	return LC_OK;
}


void CLcxAcm::Render()
{
	PLC_DEVICE	pDev = (PLC_DEVICE)LcVar_ReadP(LC_VAR_DEVICE);

	INT		n =0;

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);


	for(n=0; n<m_Info.nGeo; ++n)
	{
		Tgeo*		 pGeo = &m_pGeo[n];

		PLC_TEXTURE pTex = NULL;
		UINT		 nTex = 0;

		if(pGeo->nFce <1 || pGeo->nVtx <1 || pGeo->nType != LCX_MESH)
			continue;


		if(0<=pGeo->nMtl)
			pTex = m_pMtl[pGeo->nMtl].pTex;

		if(NULL == pTex)
			continue;


		nTex = pTex->GetId();
		if(!nTex)
			continue;


		pDev->SetTransform(LC_TS_WORLD, &m_mtWld);

		glVertexPointer  (3, GL_FLOAT,	0, pGeo->pVdP);


		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glClientActiveTexture(GL_TEXTURE0);

		glBindTexture (GL_TEXTURE_2D, nTex);

		glTexCoordPointer(2, GL_FLOAT,	0, pGeo->pVsT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, LC_TS_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, LC_TS_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, LC_TS_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, LC_TS_LINEAR);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, LC_TS_MODULATE);

		glDrawElements(GL_TRIANGLES, pGeo->nFce*3, GL_UNSIGNED_SHORT, pGeo->pFce);
	}


	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	pDev->SetTransform(LC_TS_WORLD_IDENTITY, NULL);
}


INT CLcxAcm::SetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if( dCmd == LCXM_ADVANCE_TIME)
	{
		DOUBLE fElapsed = *((DOUBLE*)pData);
		m_TimeC += fElapsed * 0.001;		// Millisecond -> second

		return LC_OK;
	}

	else if( dCmd == LCXM_CURRENT_TIME)
	{
		DOUBLE fTime = *((DOUBLE*)pData);
		m_TimeC = fTime;
		return LC_OK;
	}

	else if( dCmd == LC_TS_WORLD)
	{
		LCXMATRIX* pTM = (LCXMATRIX*)pData;

		m_mtWld	= *pTM;
		return LC_OK;
	}

	else if( dCmd == LCXM_LOOP_SECTION)
	{
		struct _TT { INT B; INT E; } *t =(_TT*)pData;

		m_FrameB = t->B;
		m_FrameE = t->E;

		if(m_FrameB < m_Info.nFrmF)	m_FrameB = m_Info.nFrmF;
		if(m_FrameB > m_Info.nFrmL)	m_FrameB = m_Info.nFrmL;

		if(m_FrameE < m_FrameB)		m_FrameE = m_FrameB;
		if(m_FrameE > m_Info.nFrmL)	m_FrameE = m_Info.nFrmL;

		return LC_OK;
	}

	else if( dCmd == LCXM_EXEC_TEXTURE_LOAD)
	{
		return LoadTexture(pData);
	}
	else if( dCmd == LCXM_MAX_ANI_BLENDING)
	{
		m_BlndNum = *((INT*)pData);

		if(1>m_BlndNum)
			m_BlndNum = 1;

		else if(4<m_BlndNum)
			m_BlndNum = 4;


		return LC_OK;
	}

	return LC_EFAIL;
}



INT CLcxAcm::GetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if( dCmd == LCXM_CURRENT_TIME)
	{
		*((DOUBLE*)pData) = m_TimeC;
		return LC_OK;
	}


	else if( dCmd == LC_TS_WORLD)
	{
		*((LCXMATRIX*)pData) = m_mtWld;
		return LC_OK;
	}
	else if( dCmd == LC_TS_WORLD_PT)
	{
		*((LCXMATRIX**)pData) = &m_mtWld;
		return LC_OK;
	}


	else if( dCmd == LCXM_GEOMETRY_MATRIX)
	{
		struct _TT { INT n;	LCXMATRIX mtT; } *t = (_TT*)pData;
		INT		n = t->n;

		if(n<0 || n>=this->m_Info.nGeo)
			return LC_CANNT_FIND_RESOURCE;

		t->mtT = m_pGeo[n].mtW;
		return LC_OK;
	}
	else if( dCmd == LCXM_GEOMETRY_MATRIX_PT)
	{
		struct _TT { INT n;	LCXMATRIX* mtT; } *t = (_TT*)pData;
		INT		n = t->n;

		if(n<0 || n>=this->m_Info.nGeo)
			return LC_CANNT_FIND_RESOURCE;

		t->mtT = &m_pGeo[n].mtW;
		return LC_OK;
	}


	else if( dCmd == LCXM_LOOP_SECTION)
	{
		struct _TT { INT B; INT E; } t = {m_FrameB, m_FrameE};
		*((_TT*)pData) = t;
		return LC_OK;
	}

	return LC_EFAIL;
}





//INT CLcxAcm::GetAniMatrix(LC_HANDLE OutTm, INT nGeo, FLOAT dFrame)
//{
//	Tgeo*		pGeo	= &m_pGeo[nGeo];
//	INT			iSize	= pGeo->nAni;
//
//	INT			nIdx	= 0;
//	FLOAT		W;
//
//	MATA*		mtA		= (MATA*)OutTm;
//
//
//	if(1>iSize)
//		return LC_OK;
//
//
//	if(1 == iSize)
//	{
//		LCXMatrix4X4From4X3(mtA, &pGeo->pAni[0]);
//		return LC_OK;
//	}
//
//
//
//	nIdx = INT( dFrame);
//	W = dFrame - nIdx;
//	nIdx %= iSize;
//
//
//	if(nIdx >= iSize-1)
//		LCXMatrix4X4From4X3(mtA, &pGeo->pAni[iSize-1]);
//	else
//		LCXMatrix4X4From4X3(mtA, &( (1-W) * pGeo->pAni[nIdx] + W * pGeo->pAni[nIdx+1]) );
//
//
//	return LC_OK;
//}



INT CLcxAcm::GetAniMatrix(LC_HANDLE OutTm, FLOAT dFrame, CLcxAcm::Tgeo* pGeo)
{
	INT			iSize = 0;
	LCXMATRIX*	mtA = NULL;
	INT			nFrame = (INT)(dFrame);
	FLOAT		w = 0;
	INT			i = 0;

	LCXMATRIX4X3* m1 = NULL;
	LCXMATRIX4X3* m2 = NULL;

	if(NULL == pGeo)
		return LC_EFAIL;


	iSize = pGeo->nAni;

	mtA	  = (LCXMATRIX*)OutTm;


	// Boundary
	if(0 == iSize || 0 >= dFrame)
	{
		mtA->Identity();
		return LC_OK;
	}

	if(1 == iSize)
	{
		LCXMatrix4X4From4X3(mtA, &pGeo->vAni[0].tm);
		return LC_OK;
	}


	//if( (iSize-1)<=nFrame)
	//{
	//	LCXMatrix4X4From4X3(mtA, &pGeo->vAni[iSize-1].tm);
	//	return LC_OK;
	//}

	if(1 < iSize)
	{
		if(pGeo->vAni[iSize-1].nF<=nFrame)
		{
			LCXMatrix4X4From4X3(mtA, &pGeo->vAni[iSize-1].tm);
			return LC_OK;
		}
	}


	// Find Time Index in the Frame Track


	for(i=0; i<iSize-1; ++i)
	{
		if(pGeo->vAni[i].nF <=dFrame && dFrame <pGeo->vAni[i+1].nF)
		{
			w = (dFrame - pGeo->vAni[i].nF)/(pGeo->vAni[i+1].nF- pGeo->vAni[i].nF);

			if(0.0F == w)
			{
				LCXMatrix4X4From4X3(mtA, &pGeo->vAni[i].tm);
			}
			else
			{
				m1 = &pGeo->vAni[i  ].tm;
				m2 = &pGeo->vAni[i+1].tm;

				//LCXMATRIX4X3  tm = (1.F - w) * *m1 + w * *m2;
				//LCXMatrix4X4From4X3(mtA, &tm );

				mtA->Lerp(m1, m2, w);
			}



			break;
		}
	}


	return LC_OK;
}



