// Interface for the CLpMdlX class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcMdlX_H_
#define _LcMdlX_H_

class CLcMdlX : public CLcMdl
{
public:
	struct LcMdlX
	{
		UINT		nFce;
		UINT		nVtx;
		char		sTex[LCM_TEX_NAME];

		VtxIdx*		pIdx;
		VtxUV1*		pVtx;
		PLC_TEXTURE	pTex;

		LcMdlX();
		~LcMdlX();
	};

protected:
	char		m_sName[LCM_FILE_NAME];
	
	DWORD		m_fmFc;															// Format of Face
	DWORD		m_fmVx;															// Format of vertex

	DWORD		m_stFc;															// Stride of Face
	DWORD		m_stVx;															// Stride of Vertex

	DWORD		m_nFce;
	DWORD		m_nVtx;

	INT			m_nGeo;
	LcMdlX*		m_pGeo;
	
public:
	CLcMdlX();
	virtual ~CLcMdlX();

	virtual	INT		Create(void* p1=0, void* p2=0, void* p3=0, void* p4=0);
	virtual	void	Destroy();
	virtual	INT		FrameMove();
	virtual	void	Render();

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		GetType()				{	return LCM_XFILE;	}
	virtual const char*	const GetName()const{	return m_sName;		}
	virtual ILcMdl* GetOriginal()			{	return this;		}

	virtual INT		SetAttrib(char* sCmd, void* pVal);
	virtual INT		GetAttrib(char* sCmd, void* pVal);

protected:
	void*	GetVertexBuffer();

	INT		LoadTexture(char* sPath);
	INT		LoadFromFile(char* sPath);

public:
	DWORD	GetFmtFce()			{	return m_fmFc;	}
	DWORD	GetFmtVtx() const	{	return m_fmVx;	}

	DWORD	GetStrideFce()		{	return m_stFc;	}
	DWORD	GetStrideVtx() const{	return m_stVx;	}

	INT		GetNumGeo()			{	return m_nGeo;	}
	LcMdlX*	GetGeometry() const	{	return m_pGeo;	}
};


#endif


