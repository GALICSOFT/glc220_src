// Implementation of the Tree class.
//
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>

#include "LnTree.h"



CLcTree::CLcTree() : pP(0), pC(0), pB(0), pN(0){}
CLcTree::CLcTree(CLcTree *pNod) : pP(0), pC(0), pB(0), pN(0)
{
	AttachTo(pNod);
}


CLcTree::~CLcTree()
{	
	Detach();																	// 부모의 인연을 끊음

	if (0 == pC)
		return;
	
	while(pC)																	// 내 밑에 자식이 있냐?
	{
		pC->Detach();
	}
}

BOOL CLcTree::HasParent()		{	return (0 != pP);	}						// 부모가 있냐?
BOOL CLcTree::HasNotParent()	{	return (0 == pP);	}
BOOL CLcTree::HasChild()		{	return (0 != pC);	}						// 자식이 있냐?
BOOL CLcTree::HasNotChild()		{	return (0 == pC);	}
BOOL CLcTree::IsSiblingF()		{	return (pP && pP->pC == this);			}	// 내가 첫번째 아들이냐



BOOL CLcTree::IsSiblingL()		{	return (pP && 0 == pN)	;				}	// 내가 마직막 아들이냐
BOOL CLcTree::IsRoot()			{	return (0 == pP);						}	// 내가 루트인가?
BOOL CLcTree::IsChild()			{	return (0 != pP);						}	// 내가 자식인가?



CLcTree* CLcTree::GetSiblingF()	{	return (pB)? pB->GetSiblingF(): this;	}	// 자매의 처음 포인터를 찾는다.
CLcTree* CLcTree::GetSiblingL()	{	return (pN)? pN->GetSiblingL(): this;	}	// 자매의 마지막 포인터를 찾는다
CLcTree* CLcTree::FindRoot()	{	return (pP)? pP->FindRoot():	this;	}	// 최상위 루트



void CLcTree::AttachTo(CLcTree *pPrn)				// 부모에  입양되기^^
{
	if (pP)											// 만약에 내가 다른 곳에 Attach되어 있다면
		Detach();									// 끊어 버린다.
	
	pP = pPrn;										// 나의 부모를 세팅
	
	CLcTree *pParentChild =	pP->pC; 
	
	if (!pP->pC)									// 부모의 자식이 없다면
	{
		pP->pC = this;								// 나를 부모의 자식으로 만든다.
	}
	
	else											// 부모 밑에 자식이 있다면
	{
		CLcTree * pSiblingLast;
		pSiblingLast =
			pParentChild->GetSiblingL();

		pB = pSiblingLast;							// 내 앞에는 부모자식을 붙인다.
		pSiblingLast->pN = this;					// 나를 부모 자식의 수평으로 연결되어 있는 끝에 붙인다.
	}
}

void CLcTree::Attach(CLcTree *pCh)					// 입양하기
{
	if (pCh->HasParent())							// 입양되는 자식이 다른 부모 밑에 있을 경우
		pCh->Detach();
	
	pCh->pP = this;									// 일단 부모를 나로 세팅해준다.
	
	if (pC)											// 내 밑에 자식이 있을 경우
	{
		CLcTree * pSiblingLast;
		pSiblingLast = pC->GetSiblingL();			// 자매들중 맨 마지막을 선택
		
		pSiblingLast->pN = pCh;
		pCh->pB = pSiblingLast;						// 자식의 자매들중에 맨 끝에 붙인다.
	}
	
	else											// 자식이 없는 경우 바로 붙인다.
	{
		pC = pCh;
	}
}

void CLcTree::Detach()
{
	if (pP && pP->pC == this)						// 부모가 있고 내가 부모의 자식이라면
	{
		if (pN)										// 나의 자매가 있다면
		{
			pP->pC = pN;							// 부모의 자식을 내 자매로 한다.
			pP->pC->pB = 0;
		}
		
		else										// 자식이 바로 나라면
			pP->pC = 0;								// 바로 끊어 버린다.
	}
	
	if(pB)
		pB->pN = pN;								// get rid of links
	
	if(pN)
		pN->pB = pB;
	
	pP = 0;											// 부모
	pB = 0;											// 
	pN = 0;
}

int CLcTree::CountNodes()
{
	if (pC)
		return pC->CountNodes() + 1;
	else
		return 1;
}



