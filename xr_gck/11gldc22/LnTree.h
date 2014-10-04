// Interface for the LcTree class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcTree_H_
#define _LcTree_H_

class CLcTree								// Lc tree class
{
public:
	CLcTree*	pP;							// 부모노드
	CLcTree*	pC;							// 자식노드
	CLcTree*	pB;							// 자매노드 이전
	CLcTree*	pN;							// 자매노드 다음
	
public:
	CLcTree();
	CLcTree(CLcTree *pNod);

	virtual ~CLcTree();

	BOOL		HasParent();				// 부모가 있냐?
	BOOL		HasNotParent();									
	BOOL		HasChild();					// 자식이 있냐?
	BOOL		HasNotChild();									
	BOOL		IsSiblingF();				// 내가 첫번째 아들이냐

	BOOL		IsSiblingL();				// 내가 마직막 아들이냐
	BOOL		IsRoot();					// 내가 루트인가?
	BOOL		IsChild();					// 내가 자식인가?

	CLcTree*	GetSiblingF();				// 자매의 처음 포인터를 찾는다.
	CLcTree*	GetSiblingL();				// 자매의 마지막 포인터를 찾는다
	CLcTree*	FindRoot();					// 최상위 루트

	void		AttachTo(CLcTree *pPrn);	// 부모에  입양되기
	void		Attach(CLcTree *pCh);		// 입양하기
	void		Detach();
	
	int			CountNodes();	
};


#endif


