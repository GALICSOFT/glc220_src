// Interface for the CLcMdl2D class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcMdl2D_H_
#define _LcMdl2D_H_

class CLcMdl2D : public CLcMdl
{
public:
	char			m_sName[LCM_FILE_NAME];
	VtxDUV1			m_pVtx[4];

	char			m_sTex[LCM_TEX_NAME];
	PLC_TEXTURE		m_pTex;

public:
	CLcMdl2D();
	virtual ~CLcMdl2D();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual	void	Destroy();
	virtual	INT		FrameMove();
	virtual	void	Render();

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		GetType()				{	return LCM_BILL;}
	virtual const char*	const GetName()const{	return m_sName;	}
	virtual ILcMdl* GetOriginal()			{	return this;	}

protected:
	INT		LoadTexture(char* sPath);
	INT		LoadFromFile(char* sPath);
};


#endif

