// Implementation of the CLcxMdlSolid class.
//
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "../11gldc22/LnD3Type.h"

#include <LnUtil.h>
#include <LnBase.h>

#include "LnxEntity.h"
#include "LnMdl.h"

#include "LnmPack.h"															// Model Pack



CLcmPack::CLcmPack()
{
}


CLcmPack::~CLcmPack()
{
	Destroy();
}


INT CLcmPack::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_pLcl = p1;
	
	return 0;
}


void CLcmPack::Destroy()
{
	SAFE_DEL_LST(	m_vObj	);
}


INT CLcmPack::FrameMove()
{
	INT		i;
	INT		iSize;

	iSize = (INT)m_vObj.size();

	for(i=0; i<iSize; ++i)
	{
		ILcMdl*	pMdB = m_vObj[i];
		pMdB->FrameMove();
	}

	return 0;
}


void CLcmPack::Render()
{
	INT		i;
	INT		iSize;
	
	iSize = (INT)m_vObj.size();
	
	for(i=0; i<iSize; ++i)
	{
		ILcMdl*	pMdB  = m_vObj[i];
		CLcMdl*	pObj  = (CLcMdl*)pMdB;
		
		pObj->Query("Bound box render", NULL);
		pObj->Render();
	}
}





INT CLcmPack::Query(char* sCmd, void* pVal)
{
	if(0==_stricmp("local", sCmd))
	{
		INT nAdd = ((INT)m_pLcl);

		*((INT*)pVal) = nAdd;
		
		return 0;
	}

	return -1;
}





void CLcmPack::AddObj(ILcMdl* pSrc
							, VEC3 vcP		// Position
							, void* p1		// Camera
							, void* p2		// Field
							, void* p3		// Field Local
							, void* p4		// Parent Package
							)
{
	if(!m_pLcl)
		return;

	ILcMdl*	pObj = NULL;

	if(FAILED(LcMdl_Create( NULL, &pObj, pSrc)))
		return;

	pObj->SetPosition((FLOAT*)&vcP);
	pObj->Query("Set Camera", p1);
	pObj->Query("Set Package", p4);

	m_vObj.push_back(pObj);
}





void CLcmPack::SetLcl(void*	pLcl)
{
	m_pLcl = pLcl;
}



void CLcmPack::Push_Back(ILcMdl* pMdB)
{
	m_vObj.push_back(pMdB);
}

INT CLcmPack::Size()
{
	return (INT)m_vObj.size();
}

ILcMdl* CLcmPack::GetObj(int n)
{
	INT iSize = (INT)m_vObj.size();

	if(n<0 || n>=iSize)
		return NULL;

	return m_vObj[n];
}


INT CLcmPack::Erase(int n)
{
	INT iSize = (INT)m_vObj.size();

	if(n<0 || n>=iSize)
		return -1;

	itLcMdl it = m_vObj.begin() + n;
	ILcMdl* p = (*it);
	delete (p);
	m_vObj.erase(it);

	return 0;
}


INT CLcmPack::Erase(ILcMdl* pObj)
{
	INT iSize = (INT)m_vObj.size();

	ILcMdl*	pObj2	= NULL;

	for(int k = 0; k<iSize; ++k)
	{
		pObj2 = this->GetObj(k);

		if(pObj2 == pObj)
			return this->Erase(k);
	}

	return -1;
}




INT LcMdl_CreatePack(char* sCmd, ILcmPack** pData, void* p1, void* p2, void* p3, void* p4)
{
	CLcmPack* pMdPck = NULL;

	pMdPck = new CLcmPack;

	if(FAILED(pMdPck->Create(p1, p2, p3, p4)))
	{
		// Return Error Notification
		delete pMdPck;
		return -1;
	}

	(*pData) = pMdPck;
	return 0;
}







