// Implementation of the CLcMdl class.
//
////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"

#include "LnMdl.h"


CLcMdl::CLcMdl()
{
	m_fStlSrtR	= 0;

	m_pCam	= NULL;
	
	m_vcRot	= VEC4(0,1,0, 0);
	m_vcScl	= VEC3(1,1,1);
	m_vcTrn	= VEC3(0,0,0);
	D3DXMatrixRotationAxis(&m_mtRot, (VEC3*)&m_vcRot, m_vcRot.w);
	m_mtWld	= m_mtRot;

	m_mtRot	= MATA(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
	m_mtWld	= MATA(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);


	m_BndInf.SetColor(0xFF666666);
	m_BndInf.SetOwner(this);
	m_nRnd	= LCM_RND_ALL;
}


CLcMdl::~CLcMdl()
{
	Destroy();
}


INT	CLcMdl::Create(void*, void*, void*, void*)
{
	return 0;
}

void CLcMdl::Destroy()
{
}

INT  CLcMdl::FrameMove()
{
	return 0;
}

void CLcMdl::Render()
{
}

INT  CLcMdl::Query(char* sCmd, void* pVal)
{
	if(0==_stricmp("Set Camera", sCmd))
	{
		m_pCam	= pVal;
		return 0;
	}

	else if(0==_stricmp("Get Camera", sCmd))
	{
		*((ILcCam**)pVal)= (ILcCam*)m_pCam;
		return 0;
	}
	
	else if(0==_stricmp("Set ids", sCmd))
	{
		WORD*	wIds = (WORD*)pVal;

		this->SetId(wIds[0], wIds[1], wIds[2], wIds[3]);
		
		return 0;
	}

	else if(0==_stricmp("Get ids", sCmd))
	{
		WORD nI0 = this->GetId1();
		WORD nI1 = this->GetId2();
		WORD nI2 = this->GetId3();
		WORD nI3 = this->GetId4();

		*((WORD*)(pVal)+0) = nI0;
		*((WORD*)(pVal)+1) = nI1;
		*((WORD*)(pVal)+2) = nI2;
		*((WORD*)(pVal)+3) = nI3;
		
		return 0;
	}

	return -1;
}

INT CLcMdl::SetAttrib(char* sCmd, void* pVal)
{
	if( 0 ==_stricmp("World Matrix", sCmd))
	{
		D3DXMATRIX* pTM = (D3DXMATRIX*)pVal;

		m_mtWld	= *pTM;
		return 0;
	}

	else if( 0 ==_stricmp("Rendering Level", sCmd))
	{
		SetupReneringLevel(pVal);
		return 0;
	}

	return -1;
}

INT CLcMdl::GetAttrib(char* sCmd, void* pVal)
{
	if( 0 ==_stricmp("World Matrix", sCmd))
	{
		*((D3DXMATRIX*)pVal) = m_mtWld;
		return 0;
	}

	else if( 0 ==_stricmp("Rendering Level", sCmd))
	{
		*((INT*)pVal)	= m_nRnd;
		return 0;
	}

	return -1;
}


INT CLcMdl::SetPosition(FLOAT* float3)
{
	m_vcTrn = *((VEC3*)float3);

	m_mtWld._41 = m_vcTrn.x;
	m_mtWld._42 = m_vcTrn.y;
	m_mtWld._43 = m_vcTrn.z;

	return 0;
}


INT CLcMdl::SetRotation(FLOAT* float4, INT nOpt)
{
	if(LCM_ROT_XYZ == nOpt)
	{
		m_vcRot.x = float4[0];
		m_vcRot.y = float4[1];
		m_vcRot.z = float4[2];

		MATA	mtY;
		MATA	mtZ;
		MATA	mtX;

		D3DXMatrixRotationY(&mtY, D3DXToRadian(m_vcRot.y));
		D3DXMatrixRotationZ(&mtZ, D3DXToRadian(m_vcRot.z));
		D3DXMatrixRotationX(&mtX, D3DXToRadian(m_vcRot.x));

		m_mtRot = mtY * mtZ * mtX;
	}
	else if(LCM_ROT_AXIS == nOpt)
	{
		m_vcRot = *((VEC4*)float4);
		D3DXMatrixRotationAxis(&m_mtRot, (VEC3*)&m_vcRot, m_vcRot.w);
	}

	WorldMatrixSetup();

	return 0;
}


INT CLcMdl::SetScaling(FLOAT* float3)
{
	m_vcScl = *((VEC3*)float3);

	WorldMatrixSetup();

	return 0;
}



INT CLcMdl::GetPosition(FLOAT* float3)
{
	*((VEC3*)float3)= m_vcTrn;

	return 0;
}

INT CLcMdl::GetRotation(FLOAT* float3)
{
	*((VEC4*)float3)= m_vcRot;
	return 0;
}


INT CLcMdl::GetScaling(FLOAT* float3)
{
	*((VEC3*)float3)= m_vcScl;
	return 0;
}


void CLcMdl::WorldMatrixSetup()
{
	m_mtWld = m_mtRot;

	m_mtWld._11 *= m_vcScl.x;
	m_mtWld._22 *= m_vcScl.y;
	m_mtWld._33 *= m_vcScl.z;

	m_mtWld._41 = m_vcTrn.x;
	m_mtWld._42 = m_vcTrn.y;
	m_mtWld._43 = m_vcTrn.z;
}


FLOAT CLcMdl::GetSortValue()
{
	return m_fStlSrtR;
}

void CLcMdl::SetupSortValue()
{
	ILcCam* pCam	= (ILcCam*)m_pCam;

	if(pCam)
	{
		MATA	mtViwI	= *pCam->GetMatrixViwI();
		
		VEC3 vcCam	= VEC3(mtViwI._41, mtViwI._42, mtViwI._43);
		VEC3 vcZ	= VEC3(mtViwI._31, mtViwI._32, mtViwI._33);
		VEC3 vcTmp	= m_vcTrn - vcCam;

		m_fStlSrtR = D3DXVec3Dot(&vcZ, &vcTmp);
	}
}

void CLcMdl::SetupReneringLevel(void* p)
{
	m_nRnd = ILcMdl::LCM_RND_ALL;

	struct T
	{
		VEC3*	vcCam;
		FLOAT	fRadiusCam;
		DPLN*	plan;
	} *t =(T*)p;


	// 1. Sphere and Sphere Collision Test
	VEC3	vcObj		= VEC3(m_mtWld._41, m_mtWld._42, m_mtWld._43);

	FLOAT	fRadiusObj	= m_BndInf.fR;
	FLOAT	fRadiusTst	= D3DXVec3Length( &(vcObj - *t->vcCam));

	if( fRadiusTst> (fRadiusObj + t->fRadiusCam) )
	{
		m_nRnd = ILcMdl::LCM_RND_NONE;
		return;
	}

	//2. Frustum and Sphere Collision Test
	for(int i=0; i<6; ++i)
	{
		FLOAT h = D3DXPlaneDotCoord(&t->plan[i], &vcObj) - fRadiusObj*1.3f;

		if(0 <= h)
		{
			m_nRnd = ILcMdl::LCM_RND_NONE;
			break;
		}
	}
}



void Lcm_BoundBoxTransform(TbndAB* pDst, const TbndAB* pSrc, const D3DXMATRIX* mtWld, const D3DXVECTOR3* vcEps)
{
	// Transform Bound Box

//	TbndAB*	pSrc = (TbndAB*)m_pOrg->GetBndInf();
//	TbndAB* pDst = &m_BndInf;


	for(int i=0; i<8; ++i)
	{
		D3DXVECTOR3* vcSrc = (D3DXVECTOR3*)&(pSrc->vcE[i].p);
		D3DXVECTOR3* vcDst = &(pDst->vcE[i].p);
		
		D3DXVec3TransformCoord(vcDst, vcSrc, mtWld);
		*vcDst += *vcEps;
	}

	pDst->vcC = pDst->vcE[0].p + pDst->vcE[7].p;
	pDst->vcC *=0.5f;

	pDst->fR = D3DXVec3Length( &(pDst->vcC - pDst->vcE[0].p));
}



void Lcm_BoundBoxRender(PDEV pDev, TbndAB* pBnd)
{
	DWORD dMnLgt;
	pDev->GetRenderState( D3DRS_LIGHTING, &dMnLgt);
	
	// Render BoundBox
	pDev->SetRenderState( D3DRS_LIGHTING, FALSE);
	pDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);

	pBnd->RenderBox(pDev);

	pDev->SetRenderState( D3DRS_LIGHTING, dMnLgt);
}

void Lcm_BoundBoxSetup(TbndAB* pDst, const TbndAB* pSrc, void* Owner)
{
	memcpy(pDst, pSrc, sizeof(TbndAB));
	pDst->SetColor(0xFF666666);
	pDst->SetOwner(Owner);
}







INT LoadAtc(void** pOut, char* sFile)
{
	vector<ILcMdl::_Tframe >*	pLcFrame = NULL;

	char	sPath[_MAX_PATH]={0};
	FILE*	fp = NULL;
	char	sLine[128]={0};

	INT		iN=0;
	INT		i=0;


	strcpy(sPath, sFile);
	char* p = strrchr(sPath, '.');
	*(p+0) = 0;
	*(p+1) = 0;

	strcat(sPath, ".atc");


	// Text로 읽는다.
	fp= fopen(sPath, "rt");

	// File is not exist.
	if(!fp)
		return -1;

	while(!feof(fp))
	{
		fgets(sLine, 128, fp);

		if(0==_strnicmp(sLine, "NumFrame*", strlen("NumFrame*")))
		{
			sscanf(sLine, "%*s %d", &iN);

			// Animation Set Number Error
			if(iN<1)
			{
				fclose(fp);
				return 0xFFFFFFFD;
			}

			pLcFrame = new vector<ILcMdl::_Tframe >;
		}

		else if(0==_strnicmp(sLine, "FrameLst", strlen("FrameLst")))
		{
			for(i=0;i<iN; ++i)
			{
				fgets(sLine, 128, fp);

				if(strlen(sLine)<5)
					continue;

				INT		nIdx;
				char	sPhs[64]={0};
				INT		nBgn;
				INT		nEnd;

				sscanf(sLine, "%d %s %d %d", &nIdx, sPhs, &nBgn, &nEnd);
				pLcFrame->push_back( ILcMdl::_Tframe(sPhs, nBgn, nEnd));
			}
		}
	}

	fclose(fp);


	*((vector<ILcMdl::_Tframe >**)pOut) = pLcFrame;

	return 0;
}
