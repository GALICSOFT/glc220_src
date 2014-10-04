// Interface for the CLcMdlSM class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcMdlSM_H_
#define _LcMdlSM_H_

class CLcMdlSM : public CLcMdl
{
public:
	char		m_sName[LCM_FILE_NAME];

	INT			m_nIdx	;														// Count Vertex Index
	INT			m_nVtx	;														// Count Vertex

	VtxIdx*		m_pIdx	;														// Vertex Face Index
	VtxNDUV1*	m_pVtx	;														//	VtxD, VtxUV, VtxNUV, VtxNDUV

	char		m_sTex[LCM_TEX_NAME];
	PLC_TEXTURE	m_pTex	;


public:
	CLcMdlSM();
	virtual ~CLcMdlSM();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual	void	Destroy();
	virtual	INT		FrameMove();
	virtual	void	Render();

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		GetType()				{	return LCM_SOLID;	}
	virtual const char*	const GetName()const{	return m_sName;		}
	virtual ILcMdl* GetOriginal()			{	return this;		}

protected:
	INT		LoadTexture(char* sPath);
	INT		LoadFromFile(char* sPath);

public:
	void	RenderMesh();
};


#endif

