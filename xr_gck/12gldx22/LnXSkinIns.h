// Interface for the CLcXSkinIns class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcXSkinIns_H_
#define _LcXSkinIns_H_

class CLcXSkinIns : public CLcXSkin
{
protected:
	CLcXSkinSrc*				m_pOrg	;				// Original CXmshAniM pointer
	LPD3DXFRAME					m_pFrameOrg;			// Original Frame Root Pointer
	LPD3DXANIMATIONCONTROLLER	m_pAC;

public:
	CLcXSkinIns();
	virtual ~CLcXSkinIns();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void    Destroy();
	virtual INT		FrameMove();
	virtual void	Render();
	virtual INT		Query(char* sCmd, void* pData);

	virtual const char*	const GetName()const{	return m_pOrg->GetName();	}
	virtual ILcMdl* GetOriginal()			{	return m_pOrg;				}
};



#endif


