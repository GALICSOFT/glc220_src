// Interface for the CLcXmsh class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcXmsh_H_
#define _LcXmsh_H_

class CLcXmsh : public ILcMsh
{
public:
	struct VtxIdx16
	{
		WORD	a;
		WORD	b;
		WORD	c;

		VtxIdx16() : a(0), b(0), c(0){}
		VtxIdx16(WORD A, WORD B, WORD C): a(A), b(B), c(C){}
		VtxIdx16(const VtxIdx16* pV)		{	a = pV->a; b = pV->b; c = pV->c;	}
		VtxIdx16(const WORD*    pV)			{	a = pV[0]; b = pV[1]; c = pV[2];	}
		VtxIdx16(const VtxIdx16& pV)		{	a = pV.a;	b = pV.b;	c = pV.c;	}

		operator WORD*()					{	return (WORD *) &a;					}
		operator const WORD*() const		{	return (const WORD*)&a;				}

		enum	{	FVF = D3DFMT_INDEX16,	};
	};

	struct VtxIdx32
	{
		DWORD	a;
		DWORD	b;
		DWORD	c;

		VtxIdx32() : a(0), b(0), c(0){}
		VtxIdx32(DWORD A, DWORD B, DWORD C): a(A), b(B), c(C){}
		VtxIdx32(const VtxIdx32* pV)		{	a = pV->a; b = pV->b; c = pV->c;	}
		VtxIdx32(const DWORD*    pV)		{	a = pV[0]; b = pV[1]; c = pV[2];	}
		VtxIdx32(const VtxIdx32& pV)		{	a = pV.a;	b = pV.b;	c = pV.c;	}

		operator DWORD*()					{	return (DWORD *) &a;				}
		operator const DWORD*() const		{	return (const DWORD*)&a;			}

		enum	{	FVF = D3DFMT_INDEX32,	};
	};
	
	struct LcXmsh
	{
		void*		pIB;
		void*		pVB;
		void*		pTx;

		D3DMATERIAL9 Mtrl;
		char		sTxF[260];

		DWORD		fmFc;														// Format of Face
		DWORD		fmVx;														// Format of vertex

		UINT		stFc;														// Stride of Face
		UINT		stVx;														// Stride of Vertex

		UINT		nFce;
		UINT		nVtx;

		LcXmsh();
		virtual ~LcXmsh();
	};

protected:
	char				m_sFile[MAX_PATH];

	DWORD				m_fmFc;													// Format of Face
	DWORD				m_fmVx;													// Format of vertex

	UINT				m_stFc;													// Stride of Face
	UINT				m_stVx;													// Stride of Vertex

	UINT				m_nFce;
	UINT				m_nVtx;

	INT					m_nGeo;
	LcXmsh*				m_pGeo;

	BOOL				m_bMtl;													// Use Material
	BOOL				m_bTxLoad;												// Texture Load in self

	
public:
	CLcXmsh();
	virtual ~CLcXmsh();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);				// p1:Device, p2: X File Name
	virtual	void	Destroy();

	virtual	INT		FrameMove();
	virtual	void	Render();

	virtual INT		Query(char* sCmd, void* pData);

	virtual INT		Lock(UINT nGeo, UINT OffsetToLock, UINT SizeToLock, void** ppbData);
	virtual INT		Unlock(UINT nGeo);

	// Vertex SetUp( if Lock ppbdata is Null.
	virtual	void	SetPos		(INT nGeo, INT nVtx, FLOAT X, FLOAT Y, FLOAT Z, FLOAT W=1.F);
	virtual	void	SetNormal	(INT nGeo, INT nVtx, FLOAT X, FLOAT Y, FLOAT Z);

	// rgba is 0~ 1.f
	virtual	void	SetDiffuse	(INT nGeo, INT nVtx, FLOAT R, FLOAT G, FLOAT B, FLOAT A);
	virtual	void	SetUV		(INT nGeo, INT nVtx, INT nUV, FLOAT U, FLOAT V);

	virtual	void	GetPos		(INT nGeo, INT nVtx, FLOAT* X, FLOAT* Y, FLOAT* Z, FLOAT* W);
	virtual	void	GetNormal	(INT nGeo, INT nVtx, FLOAT* X, FLOAT* Y, FLOAT* Z);
	virtual	void	GetDiffuse	(INT nGeo, INT nVtx, FLOAT* R, FLOAT* G, FLOAT* B, FLOAT* A);
	virtual	void	GetUV		(INT nGeo, INT nVtx, INT  nUV, FLOAT* U, FLOAT* V);

protected:
	INT		LoadTexture(char* sTexPath);
	INT		AnsiToUnicode(WCHAR* sDst, const char* sSrc, INT iLen );
	DWORD	RedefineFVF(DWORD dFVF);
	DWORD	RedefineIdxFmt(DWORD dFmt);	

	INT		SaveLcXmsh(char* sFile);
	INT		OpenLcXmsh(void* pDev, char* sFile);

	char*	GetVertexBuffer();
};


#endif


