// Interface for the CLcmTileB class.
// Block Class
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcmTileB_H_
#define _LcmTileB_H_

class CLcmTileB
{
public:
	enum
	{
		LCXM_TILE_MAX_W		= 16,
	};


	struct _Tile
	{
		SHORT	nAtt;	// Attribute
		SHORT	nIdP;	// Primary _Tile Index
		SHORT	nIdS;	// Secondary _Tile Index

		_Tile() : nAtt(-1), nIdP(-1), nIdS(-1){}

		void GetTexcoordPrim(FLOAT* x0, FLOAT* y0, FLOAT* x1, FLOAT* y1)
		{
			*x0 = FLOAT(  int( nIdP%LCXM_TILE_MAX_W) + 0.01F) /LCXM_TILE_MAX_W;
			*y0 = FLOAT(  int( nIdP/LCXM_TILE_MAX_W) + 0.01F) /LCXM_TILE_MAX_W;
			*x1 = FLOAT(  int( nIdP%LCXM_TILE_MAX_W) + 0.99F) /LCXM_TILE_MAX_W;
			*y1 = FLOAT(  int( nIdP/LCXM_TILE_MAX_W) + 0.99F) /LCXM_TILE_MAX_W;

//			*x0 = FLOAT(  int( nIdP%LCXM_TILE_MAX_W) + 0) /LCXM_TILE_MAX_W;
//			*y0 = FLOAT(  int( nIdP/LCXM_TILE_MAX_W) + 0) /LCXM_TILE_MAX_W;
//			*x1 = FLOAT(  int( nIdP%LCXM_TILE_MAX_W) + 1) /LCXM_TILE_MAX_W;
//			*y1 = FLOAT(  int( nIdP/LCXM_TILE_MAX_W) + 1) /LCXM_TILE_MAX_W;

			*x0 += 0.5f/1024;
			*y0 += 0.5f/1024;
			*x1 -= 0.5f/1024;
			*y1 -= 0.5f/1024;
		}

		void GetTexcoordSecond(FLOAT* x0, FLOAT* y0, FLOAT* x1, FLOAT* y1, FLOAT* z)
		{
			*x0 = FLOAT(  int( nIdS%LCXM_TILE_MAX_W) + 0.01F) /LCXM_TILE_MAX_W;
			*y0 = FLOAT(  int( nIdS/LCXM_TILE_MAX_W) + 0.01F) /LCXM_TILE_MAX_W;
			*x1 = FLOAT(  int( nIdS%LCXM_TILE_MAX_W) + 0.99F) /LCXM_TILE_MAX_W;
			*y1 = FLOAT(  int( nIdS/LCXM_TILE_MAX_W) + 0.99F) /LCXM_TILE_MAX_W;

//			*x0 = FLOAT(  int( nIdS%LCXM_TILE_MAX_W) + 0) /LCXM_TILE_MAX_W;
//			*y0 = FLOAT(  int( nIdS/LCXM_TILE_MAX_W) + 0) /LCXM_TILE_MAX_W;
//			*x1 = FLOAT(  int( nIdS%LCXM_TILE_MAX_W) + 1) /LCXM_TILE_MAX_W;
//			*y1 = FLOAT(  int( nIdS/LCXM_TILE_MAX_W) + 1) /LCXM_TILE_MAX_W;

			*x0 += 0.5f/1024;
			*y0 += 0.5f/1024;
			*x1 -= 0.5f/1024;
			*y1 -= 0.5f/1024;

			*z= (-1 == nIdS)?  -1.0f: 0.0f;
		}
	};


	struct _VtxDX
	{
		VEC3	p;
		DWORD	d;
		VEC2	t0;
		VEC3	t1;

		_VtxDX()							: p(0,0,0),t0(0,0),t1(0,0,-1), d(0xFFFFFFFF){}
		_VtxDX(FLOAT X, FLOAT Y, FLOAT Z
				, FLOAT U0=0, FLOAT V0=0
				, FLOAT U1=0, FLOAT V1=0, FLOAT W1=-1
				, DWORD D=0xFFFFFFFF)	: p(X,Y,Z),t0(U0,V0), t1(U1,V1,W1), d(D){}

		enum {	FVF= (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2| D3DFVF_TEXCOORDSIZE3(1))	};
	};


public:
	MATA			m_mtW;					// World Matrix
	VEC3			m_vcP;					// Position

	// Tile
	INT				m_nCell;
	_Tile*			m_pTile;


	// Mesh
	FLOAT			m_fWx;					// Mesh: Width of tile for x;
	INT				m_iVs;					// Vertex Size
	DWORD			m_FVF;
	INT				m_nVtx;					// Number of Vertices
	_VtxDX*			m_pVtx;

	BOOL			m_bRend	;				// Rendering Enable
	ILcmPack*		m_pMdLst;				// Model Pack

protected:
	void*			m_pDst	;				// Parent Manager Local Field

public:
	CLcmTileB();
	virtual ~CLcmTileB();

	INT		Create(void* =0, void* =0, void* =0, void* =0);
	void	Destroy();

	INT		FrameMove();
	void	RenderBlc();
	void	RenderTline();														// Show Block


public:
	void	CreateFieldMesh(INT nTile, FLOAT fWx);
	void	SetUpMesh();
	void	SetUpTexcoord();

	INT		GetBlcHeight(D3DXVECTOR3* pOut, const D3DXVECTOR3* pIn);

	void	SetDiffuse(int nIdx=-1);
	void	SetPos(VEC3	pos)	{	m_vcP = pos	;	}
	VEC3	GetPos()				{	return m_vcP;	}
	INT		GetTileN()					{	return m_nCell;	}					// Get Tile Number of X or Z
	FLOAT	GetTileW()					{	return m_fWx;	}					// Get Tile Width
};


#endif


