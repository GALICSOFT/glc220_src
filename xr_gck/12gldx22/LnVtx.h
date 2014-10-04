// Interface for the CLcVtx class.
// PC Game Vertex
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcVtx_H_
#define _LcVtx_H_

class CLcVtx : public ILcVtx
{
protected:
	DWORD				m_dFVF;
	UINT				m_dStrd;		// sizeof(Vtx...)
	INT					m_nNum;			// Vertex Number

	void*				m_pVB;			// 시스템 메모리 사용. malloc 으로 생성
	DWORD				m_dPtType;
	UINT				m_dPtNum;

	LPDIRECT3DTEXTURE9	m_pTx[8];

public:
	CLcVtx();
	virtual ~CLcVtx();

public:
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual void	Render();

	virtual void*	GetVertex();
	virtual DWORD	GetFVF();
	virtual UINT	GetStrid();
	virtual	INT		GetVtxNum();

	virtual INT		Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
	virtual INT		Unlock();

	virtual	void	SetTexture(INT nStage, void* pTx);
	virtual	void	DrawPrimitive(DWORD PrimitiveType, UINT StartVertex, UINT PrimitiveCount);

	virtual	void	SetPtType(DWORD dPtType);
	virtual	void	SetPtNum(UINT iPtNum);

	// Vertex SetUp( if Lock ppbdata is Null.
	virtual	void	SetPos		(INT nIdx, FLOAT X, FLOAT Y, FLOAT Z, FLOAT W=1.F);
	virtual	void	SetNormal	(INT nIdx, FLOAT X, FLOAT Y, FLOAT Z);

	/* rgba is 0~ 1.f */
	virtual	void	SetDiffuse	(INT nIdx, FLOAT R, FLOAT G, FLOAT B, FLOAT A);
	virtual	void	SetUV		(INT nIdx, INT nUV, FLOAT U, FLOAT V);

public:
	void	SetFVF(DWORD dFVF);
	void	SetStrdSize(UINT iStrd);
};


INT	LcM3d_CreateVtx_PC(
					ILcVtx** pData	//
					, void* p1		// Vertex Structure
					, void* p2		// Need D3D Device
					, void* p3		// Primitive Type
					, void* p4		// Primitive Count
					, void* p5		// Total Vertices
					, void* p6=0	// Vertex Memory Copy Source
					, void* p7=0	// Source Total Size
					, void* p8=0	// No Use
					, void* p9=0	// No Use
					);



#endif



