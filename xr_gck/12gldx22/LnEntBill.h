// Implementation of the CLcEntBill classes.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcEntBill_H_
#define _LcEntBill_H_

class CLcEntBill : public ILcEnt
{
public:
	struct VtxDUV1
	{
		D3DXVECTOR3	p;		// Vertex position
		DWORD		d;		// Vertex color
		FLOAT	 u, v;		// Vertex texture coordinates
		
		enum {FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1) };

		VtxDUV1(){}
		VtxDUV1(FLOAT X, FLOAT Y, FLOAT Z, FLOAT U, FLOAT V, DWORD D=0XFFFFFFFF)
		{
			p.x = X;	p.y = Y;	p.z = Z;
			d = D	;	u = U;		v = V;
		}
	};

	struct Billboard
	{
		VtxDUV1				Vtx[4];			// Four corners of billboard quad
		D3DXVECTOR3			vcP;			// Origin of tree
		LPDIRECT3DTEXTURE9	pTx;			// Which texture map to use
		FLOAT				fStlSrtR;

		Billboard()
		{
			//             Max
			//	1----------3
			//	|¡¬        |
			//	|  ¡¬      |
			//	|    ¡¬    |
			//	|      ¡¬  |
			//	|        ¡¬|
			//	0----------2
			//	Min

			Vtx[0]	= VtxDUV1(0, 0, 0, 0, 1);
			Vtx[1]	= VtxDUV1(0, 0, 0, 0, 0);
			Vtx[2]	= VtxDUV1(0, 0, 0, 1, 1);
			Vtx[3]	= VtxDUV1(0, 0, 0, 1, 0);
			
			vcP		= D3DXVECTOR3(0,0,0);
			pTx		= NULL;

			fStlSrtR= FLT_MAX;
		}

		void SetDiffuse(DWORD d)
		{
			Vtx[0].d = d;
			Vtx[1].d = d;
			Vtx[2].d = d;
			Vtx[3].d = d;
		}
		
		void SetUV(INT nUV, FLOAT U, FLOAT V)
		{
			Vtx[nUV].u = U;
			Vtx[nUV].v = V;
		}

		void GetPos(D3DXVECTOR3* pOut)
		{
			*pOut = vcP;
		}

	};


protected:
	UINT					m_dPtNum;
	INT						m_nBill;
	Billboard*				m_pBill;

	D3DXVECTOR3				m_vcCamP;
	D3DXVECTOR3				m_vcCamX;
	D3DXVECTOR3				m_vcCamY;
	D3DXVECTOR3				m_vcCamZ;

	D3DXMATRIX				m_mtBill;

public:
	CLcEntBill();
	virtual ~CLcEntBill();

public:
	//						Device	,	Billboard Number,		NULL,		NULL
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		Query(char* sCmd, void* pData);

protected:
	void	SetTexture(INT nIdx, void* pTx);

	//						Billboard Index,  Min(x0,y0),   Max(x1,y1)
	void	SetDimension(INT nIdx, D3DXVECTOR4*);
	void	SetPos		(INT nIdx, D3DXVECTOR3*);

	// rgba is 0~ 1.f
	void	SetDiffuse	(INT nIdx, DWORD);
	void	SetUV		(INT nIdx, INT nUV, D3DXVECTOR2*);

	void	GetPos		(INT nIdx, D3DXVECTOR3*);
	void	GetDiffuse	(INT nIdx, D3DXCOLOR*);
	void	GetUV		(INT nIdx, INT  nUV, D3DXVECTOR2*);

	void	SetBillMatrix(D3DXMATRIX* mtBill);

public:
	static INT Sort( const void* arg1, const void* arg2 );
};


#endif

