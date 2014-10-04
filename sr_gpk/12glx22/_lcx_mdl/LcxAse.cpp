// Implementation of the LCX_Ase class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#if defined(_MSC_VER)
  #pragma warning(disable: 4786)
#endif


#include <set>

#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
#else
  #include "../LcxEntity.h"
#endif

#include "LcxAse.h"



LCX_Ase::_AseMtl::_AseMtl()
{
	memset(sTex, 0, sizeof sTex);
}

LCX_Ase::_AseGeo::_AseGeo()
{
	nMtlRef = -1;
	iNumVtx = 0;
	iNumFce = 0;
	iNumTvtx= 0;
	iNumTfce = 0;

	pLstVtx = NULL;
	pLstFce = NULL;

	pLstTvtx= NULL;
	pLstTfce= NULL;

	memset(sNodeCur, 0, sizeof sNodeCur);
	memset(sNodePrn, 0, sizeof sNodePrn);

	nNodePrn= -1;
	pGeoPrn	= NULL;
}



LCX_Ase::_AseGeo::~_AseGeo()
{
	nNodePrn= -1;
	pGeoPrn	= NULL;
}






LCX_Ase::Tmtl::Tmtl()
{
	memset(sTex, 0, LCXM_TEXNAME);
	pTex = NULL;
}

LCX_Ase::Tmtl::~Tmtl()
{
	SAFE_DELETE(	pTex	);
}


LCX_Ase::Tgeo::Tgeo()
{
	nMtl	= -1;

	memset(sName, 0, sizeof sName);

	nPrn	= -1;
	iFce	= 0;
	iNvx	= 0;
	pIdx	= NULL;
	pVtx	= NULL;
	dFVF	= 0;
	dVtx	= 0;

	mtTmL.Identity();
	mtW.Identity();
	mtL.Identity();

	nAni	= 0;
	vAni	= NULL;
}

LCX_Ase::Tgeo::~Tgeo()
{
	SAFE_DELETE_ARRAY(	pIdx	);

	if(pVtx)
	{
		if(dFVF == (UINT)Tvtx::FVF)
		{
			Tvtx* p = (Tvtx*)pVtx;
			delete [] p;
		}

		else if(dFVF == (UINT)TvtxUV::FVF)
		{
			TvtxUV* p = (TvtxUV*)pVtx;
			delete [] p;
		}

		pVtx = NULL;
	}

	nPrn	= -1;
	iFce	= 0;
	iNvx	= 0;
	dFVF	= 0;
	dVtx	= 0;

	nAni = 0;
	SAFE_DELETE_ARRAY(	vAni	);
}




LCX_Ase::TlinkTm::TlinkTm()
{
	nPrn	= -1;

	mtW.Identity();
	mtL.Identity();
}






INT LCX_Ase::GetAniFrame(INT* First, INT* Last, INT* oAni, INT** oPivot, LCX_Ase::_AseGeo* pSrc)
{
	typedef std::set<INT >		IntLst;
	typedef IntLst::iterator	IntIt;



	IntLst vPivot;


	INT		R = pSrc->vRot.size();
	INT		T = pSrc->vTrs.size();

	INT		B_rot=0, B_trs=0, E_rot=0, E_trs=0;

	INT		i;



	//Gather Pivot
	for(i=0; i<R; ++i)
	{
		INT Frame = pSrc->vRot[i].nF;
		vPivot.insert(Frame);
	}

	for(i=0; i<T; ++i)
	{
		INT Frame = pSrc->vTrs[i].nF;
		vPivot.insert(Frame);
	}

	if(oAni && oPivot)
	{
		*oAni = vPivot.size();

		if(0 < *oAni)
		{
			INT* pPivot = new INT[ *oAni ];

			i = 0;

			for(IntIt t= vPivot.begin(); t !=vPivot.end();)
			{
				INT k = (*t);

				pPivot[i] = k;

				++i;
				++t;
			}

			*oPivot = pPivot;
		}
	}


	// Clear Pivot
	vPivot.clear();



	if(0 < R)
	{
		B_rot = pSrc->vRot[0  ].nF;
		E_rot = pSrc->vRot[R-1].nF;
	}

	if(0 < T)
	{
		B_trs = pSrc->vTrs[0  ].nF;
		E_trs = pSrc->vTrs[T-1].nF;
	}

	if(First)
		*First = lc_min(B_rot, B_trs);

	if(Last)
		*Last  = lc_max(E_rot, E_trs);


	if(0 == R && 0 == T)
		return -1;


	return 0;
}




INT LCX_Ase::GetAniTrack(LC_HANDLE OutTm, FLOAT dFrame, LCX_Ase::_AseGeo* pSrc)
{
	INT			i=0;

	LCXMATRIX*	mtA;

	INT		iSizeR = pSrc->vRot.size();
	INT		iSizeP = pSrc->vTrs.size();
	INT		nIdx;
	INT		E_rot=0, E_trs=0;


	mtA = (LCXMATRIX*)OutTm;

	mtA->Identity();


	if(iSizeR && pSrc->vRot[0].nF <=dFrame)
	{
		E_rot = pSrc->vRot[iSizeR-1].nF;

		if(1 == iSizeR)
		{
			LCXQUATERNION q;

			nIdx =0;

			q.x = pSrc->vRot[nIdx].x;
			q.y = pSrc->vRot[nIdx].y;
			q.z = pSrc->vRot[nIdx].z;
			q.w = pSrc->vRot[nIdx].w;

			q.RotationMatrix(mtA, LC_FALSE);
		}

		else if(E_rot <=dFrame)
		{
			LCXQUATERNION q;

			nIdx = iSizeR-1;

			q.x = pSrc->vRot[nIdx].x;
			q.y = pSrc->vRot[nIdx].y;
			q.z = pSrc->vRot[nIdx].z;
			q.w = pSrc->vRot[nIdx].w;

			q.RotationMatrix(mtA, LC_FALSE);
		}

		else
		{
			for(i=0; i<iSizeR-1; ++i)
			{
				if(pSrc->vRot[i].nF <=dFrame && dFrame <pSrc->vRot[i+1].nF)
				{
					LCXQUATERNION q;
					LCXQUATERNION q1;
					LCXQUATERNION q2;

					nIdx = i;

					q1.x = pSrc->vRot[nIdx].x;
					q1.y = pSrc->vRot[nIdx].y;
					q1.z = pSrc->vRot[nIdx].z;
					q1.w = pSrc->vRot[nIdx].w;

					q2.x = pSrc->vRot[nIdx+1].x;
					q2.y = pSrc->vRot[nIdx+1].y;
					q2.z = pSrc->vRot[nIdx+1].z;
					q2.w = pSrc->vRot[nIdx+1].w;

					FLOAT	w = (dFrame - pSrc->vRot[i].nF)/(pSrc->vRot[i+1].nF- pSrc->vRot[i].nF);

					q.SLerp(&q1, &q2, w);
					q.RotationMatrix(mtA, LC_FALSE);

					break;
				}
			}
		}
	}

	else
	{
		*mtA = pSrc->TmInf.mtL;
	}


	if(iSizeP && pSrc->vTrs[0].nF <=dFrame)
	{
		E_trs = pSrc->vTrs[iSizeP-1].nF;

		if(1 == iSizeP)
		{
			LCXVECTOR3 p;

			nIdx =0;
			p.x = pSrc->vTrs[nIdx].x;
			p.y = pSrc->vTrs[nIdx].y;
			p.z = pSrc->vTrs[nIdx].z;

			mtA->_41 = p.x;
			mtA->_42 = p.y;
			mtA->_43 = p.z;
		}

		else if(E_trs <=dFrame)
		{
			LCXVECTOR3 p;

			nIdx = iSizeP-1;
			p.x = pSrc->vTrs[nIdx].x;
			p.y = pSrc->vTrs[nIdx].y;
			p.z = pSrc->vTrs[nIdx].z;

			mtA->_41 = p.x;
			mtA->_42 = p.y;
			mtA->_43 = p.z;
		}

		else
		{
			for(i=0; i<iSizeP-1; ++i)
			{
				if(pSrc->vTrs[i].nF <=dFrame && dFrame <pSrc->vTrs[i+1].nF)
				{
					LCXVECTOR3 p;
					LCXVECTOR3 p1;
					LCXVECTOR3 p2;

					nIdx = i;

					p1.x = pSrc->vTrs[nIdx].x;
					p1.y = pSrc->vTrs[nIdx].y;
					p1.z = pSrc->vTrs[nIdx].z;

					p2.x = pSrc->vTrs[nIdx+1].x;
					p2.y = pSrc->vTrs[nIdx+1].y;
					p2.z = pSrc->vTrs[nIdx+1].z;


					FLOAT	w = (dFrame- pSrc->vTrs[i].nF)/(pSrc->vTrs[i+1].nF- pSrc->vTrs[i].nF);

					p = p1  + w * (p2-p1);
					mtA->_41 = p.x;
					mtA->_42 = p.y;
					mtA->_43 = p.z;
					break;
				}
			}
		}
	}
	else
	{
		mtA->_41 = pSrc->TmInf.mtL._41;
		mtA->_42 = pSrc->TmInf.mtL._42;
		mtA->_43 = pSrc->TmInf.mtL._43;
	}


	return LC_OK;
}




INT LCX_Ase::GetAniMatrix(LC_HANDLE OutTm, FLOAT dFrame, LCX_Ase::Tgeo* pGeo)
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



