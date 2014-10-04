// Implementation of the CLcpObj class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <stdio.h>
#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <LnEuclid.h>
#include <LnxEntity.h>

#include "LntEntity.h"

#include "LnpObj.h"


#define SAFE_DELETE(p)	{	if(p){delete (p);p = NULL;	}}


CLcpObj::CLcpObj()
{
	m_DataB	= NULL;
	m_DataE	= NULL;
	m_DataX	= NULL;

	m_pMdl	= NULL;
	
	m_vMdlE.clear();
	m_vMdlX.clear();

	m_pFld	= NULL;

	m_nState= LCPOBJ_IDLE;

	m_FrmLoop	= OBJ_ANI_LOOP_INFINITE;
	m_FrmCnt	= 0;
	m_FrmIdx	= -1;
}

CLcpObj::~CLcpObj()
{
	Destroy();
}
	
INT CLcpObj::Create(void* p1, void* p2, void* p3, void* p4)
{
	if(p1)
	{
		m_DataB	= (ILcpData*)p1;
	}
	else
	{
		// Primary데이터가 없으면 기본 데이터를 만든다.
		ILcpData*	pData = NULL;
		if(FAILED(LcpData_CreateBasic(NULL, &pData) ))
			return -1;

		m_DataB = pData;
	}

	if(p2)
		m_DataE	= (ILcpData*)p2;

	if(p3)
	{
		// 모델은 반드시 외부에서 생성해야 한다.
		ILcMdl* pMdl	= (ILcMdl*)p3;
		m_pMdl = pMdl;

		m_pMdl->GetAttrib("Frame List", &m_FrmLst);
		m_FrmLoop	= 0;
		m_FrmCnt	= 0;
		m_FrmIdx	= -1;

		if(!m_FrmLst.empty())
			m_FrmIdx	= 0;
	}

	if(p4)
	{
		lsLcMdl* vMdl	= (lsLcMdl*)p4;

		for(int i=0; i<vMdl->size(); ++i)
		{
			m_vMdlE.push_back(*(vMdl->begin() + i));
		}
	}

	return 0;
}

void CLcpObj::Destroy()
{
	// Release Data.

//	if(m_DataB)
//	{
//		if(m_DataB->GetType() == ILcpData::LCP_TYPE_INTER)
//			delete m_DataB;
//
//		m_DataB	= NULL;
//	}

	SAFE_DELETE(	m_DataB	);
	SAFE_DELETE(	m_DataE	);

	m_DataX = NULL;



	// Release Model.

	SAFE_DELETE(	m_pMdl	);

	if(!m_vMdlE.empty())
	{
		for(int i=0; i<(INT)(m_vMdlE.size()); ++i)
		{
			SAFE_DELETE(	m_vMdlE[i]	);
		}
		m_vMdlE.clear();
	}

	if(!m_vMdlX.empty())
		m_vMdlX.clear();
}

INT CLcpObj::FrameMove()
{
	int i = 0;
	INT	Loop = 0;

	// Main Model Update
	if(m_pMdl)
	{
		if( (LCPOBJ_BIT_MOVE & m_nState) && m_DataB)
		{
			D3DXVECTOR3	vcPos(0,0,0);

			if( SUCCEEDED(m_DataB->GetAttrib("Position", &vcPos)) && m_pFld)
			{
				if(SUCCEEDED(m_pFld->GetHeight((FLOAT*)&vcPos, (FLOAT*)&vcPos)))
				{
					m_DataB->SetAttrib("Position", &vcPos);
					m_pMdl->SetPosition((FLOAT*)&vcPos);
				}
			}
		}

		// 모델은 한 번 애니메이션을 수행하면 반환 값을 1을 준다.
		Loop = m_pMdl->FrameMove();

		INT	nFrmC = 0;
		m_pMdl->GetAttrib("Current Frame", &nFrmC);

//		char	sBuf[32] ={0};
//		sprintf(sBuf, "%d", nFrmC);
//		SetWindowText(GetActiveWindow(), sBuf);

		if(OBJ_ANI_LOOP_ONCE == m_FrmLoop && !m_FrmLst.empty())
		{
			INT nB = m_FrmLst[m_FrmIdx].nB;
			INT nE = m_FrmLst[m_FrmIdx].nE;
			
			if(1 <= Loop || (nFrmC+1)>=nE)
				m_FrmCnt++;
		}
	}


	// FrameMove Equip Model
	for(i=0; i<(INT)(m_vMdlE.size()); ++i)
	{
		if(m_vMdlE[i])
			m_vMdlE[i]->FrameMove();
	}

	return 0;
}

void CLcpObj::Render()
{
	int i = 0;

	if(m_pMdl)
		m_pMdl->Render();


	for(i=0; i<(INT)(m_vMdlE.size()); ++i)
	{
		if(m_vMdlE[i])
			m_vMdlE[i]->Render();
	}

	for(i=0; i<(INT)(m_vMdlX.size()); ++i)
	{
		if(m_vMdlX[i])
			m_vMdlX[i]->Render();
	}



	if(OBJ_ANI_LOOP_ONCE == m_FrmLoop && !m_FrmLst.empty())
	{
		if(1 <= m_FrmCnt)
		{
			INT nFrmLoop = OBJ_ANI_LOOP_INFINITE;
			this->SetAnimation( "Idle", &nFrmLoop);
		}
	}
}

INT CLcpObj::Query(char* sCmd, void* pVal)
{
	if( 0 ==_stricmp(sCmd, "Setup Height") )
	{
		INT	hr=0;
		D3DXVECTOR3	vcPos(0,0,0);
		
		if( !(m_pMdl && m_DataB && m_pFld) )
			return -1;

		hr = m_DataB->GetAttrib("Position", &vcPos);

		if(FAILED(hr))
			return -1;

		hr = m_pFld->GetHeight((FLOAT*)&vcPos, (FLOAT*)&vcPos);

		if(FAILED(hr))
			return -1;

		m_DataB->SetAttrib("Position", &vcPos);
		m_pMdl->SetPosition((FLOAT*)&vcPos);
	}
	
	else if( 0 ==_stricmp(sCmd, "Set Basic Data") )
	{
		m_DataB	= (ILcpData*)pVal;
	}
	
	else if( 0 ==_stricmp(sCmd, "Set Extended Data") )
	{
		m_DataE	= (ILcpData*)pVal;
	}

	else if( 0 ==_stricmp(sCmd, "Set Auxiliary Data") )
	{
		m_DataX	= (ILcpData*)pVal;
	}

	else if( 0 ==_stricmp(sCmd, "Set Basic Model") )
	{
		ILcMdl* vMdl	= (ILcMdl*)pVal;

		SAFE_DELETE(	m_pMdl	);
		m_pMdl = vMdl;

		m_FrmLst.clear();
		m_pMdl->GetAttrib("Frame List", &m_FrmLst);

		m_FrmLoop	= 0;
		m_FrmCnt	= 0;
		m_FrmIdx	= -1;

		if(!m_FrmLst.empty())
			m_FrmIdx	= 0;
	}

	else if( 0 ==_stricmp(sCmd, "Set Extended Model") )
	{
		int i = 0;
		lsLcMdl* vMdl	= (lsLcMdl*)pVal;

		for(i=0; i<(INT)(m_vMdlE.size()); ++i)
		{
			SAFE_DELETE(	m_vMdlE[i]	);
		}

		m_vMdlE.clear();

		for(i=0; i<vMdl->size(); ++i)
		{
			m_vMdlE.push_back(*(vMdl->begin() + i));
		}
	}

	else if( 0 ==_stricmp(sCmd, "Set Auxiliary Model") )
	{
		int i = 0;
		lsLcMdl* vMdl	= (lsLcMdl*)pVal;

		m_vMdlX.clear();

		for(i=0; i<vMdl->size(); ++i)
		{
			m_vMdlX.push_back(*(vMdl->begin() + i));
		}
	}

	else if( 0 ==_stricmp(sCmd, "Set Field") )
	{
		m_pFld = (ILcmFld*)pVal;
	}

	else if( 0 ==_stricmp(sCmd, "Set Camera") )
	{
		if(m_pMdl)
			m_pMdl->Query(sCmd, pVal);
		else
			return -1;
	}

	else if( 0 ==_stricmp(sCmd, "Get Basic Data") )
	{
		*((ILcpData**)pVal) = m_DataB;
	}
	
	else if( 0 ==_stricmp(sCmd, "Get Extended Data") )
	{
		*((ILcpData**)pVal) = m_DataE;
	}

	else if( 0 ==_stricmp(sCmd, "Get Auxiliary Data") )
	{
		*((ILcpData**)pVal) = m_DataX;
	}

	else if( 0 ==_stricmp(sCmd, "Get Basic Model") )
	{
		*((ILcMdl**)pVal)	= m_pMdl;
	}

	else if( 0 ==_stricmp(sCmd, "Get Extended Model") )
	{
		lsLcMdl* vMdl	= (lsLcMdl*)pVal;

		vMdl->clear();

		for(int i=0; i<(INT)(m_vMdlE.size()); ++i)
		{
			vMdl->push_back(*(m_vMdlE.begin() + i));
		}
	}

	else if( 0 ==_stricmp(sCmd, "Get Auxiliary Model") )
	{
		lsLcMdl* vMdl	= (lsLcMdl*)pVal;

		vMdl->clear();

		for(int i=0; i<(INT)(m_vMdlX.size()); ++i)
		{
			vMdl->push_back(*(m_vMdlX.begin() + i));
		}
	}

	else if( 0 ==_stricmp(sCmd, "Get Field") )
	{
		*((ILcmFld**)pVal)	= m_pFld;
	}

	else
	{
		return -1;
	}

	return 0;
}

INT CLcpObj::SetAttrib(char* sCmd, void* pVal)
{
	if( 0 ==_stricmp(sCmd, "Position") )
	{
		if(m_DataB)
			m_DataB->SetAttrib(sCmd, pVal);

		if(m_pMdl)
			m_pMdl->SetPosition((FLOAT*)pVal);
	}

	else if( 0 ==_stricmp(sCmd, "Rotation") )
	{
		if(m_DataB)
			m_DataB->SetAttrib(sCmd, pVal);

		if(m_pMdl)
			m_pMdl->SetRotation((FLOAT*)pVal, ILcMdl::LCM_ROT_AXIS);
	}

	else if( 0 ==_stricmp(sCmd, "Scaling") )
	{
		if(m_DataB)
			m_DataB->SetAttrib(sCmd, pVal);

		if(m_pMdl)
			m_pMdl->SetScaling((FLOAT*)pVal);
	}

	else if( 0 ==_stricmp(sCmd, "Advance Time") )
	{
		if(m_pMdl)
			m_pMdl->SetAttrib(sCmd, pVal);
	}

	else
	{
		return -1;
	}

	return 0;
}


INT CLcpObj::GetAttrib(char* sCmd, void* pVal)
{
	if( 0 ==_stricmp(sCmd, "Position") )
	{
		if(m_DataB)
			m_DataB->GetAttrib(sCmd, pVal);
	}

	else if( 0 ==_stricmp(sCmd, "Rotation") )
	{
		if(m_DataB)
			m_DataB->SetAttrib(sCmd, pVal);
	}

	else if( 0 ==_stricmp(sCmd, "Scaling") )
	{
		if(m_DataB)
			m_DataB->SetAttrib(sCmd, pVal);
	}

	else
	{
		return -1;
	}

	return 0;
}





INT CLcpObj::SetAnimation(char* sCmd, void* pVal)
{
	INT		nFrmIdx = -1;

	if(!m_pMdl)
		return -1;

	// 1. Find Animation From the Model in act File.
	for(int i=0; i<(INT)(m_FrmLst.size()); ++i)
	{
		char*	sAni = m_FrmLst[i].sN;

		if( 0 ==_stricmp(sCmd, sAni) )
		{
			nFrmIdx = i;
			break;
		}
	}

	if(FAILED(nFrmIdx))
		return -1;


	struct T{ int l; int s; } *t = (T*)pVal;

	m_FrmIdx	= nFrmIdx;
	m_FrmCnt	= 0;
	m_FrmLoop	= t->l;

	if(1 == t->s)
	{
		if(FAILED(m_pMdl->SetAttrib("Frame Index", &nFrmIdx)))
			return -1;
	}
	else
	{
		if(FAILED(m_pMdl->SetAttrib("Frame Index Absolute", &nFrmIdx)))
			return -1;
	}

	return 0;
}


INT CLcpObj::GetAnimation(char* sCmd, void* pVal)
{
	return -1;
}


INT CLcpObj::FindAnimationSet(char*)
{
	return -1;
}