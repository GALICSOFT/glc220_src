// Interface for the CLcmSptB class.
// Block Class
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcmSptB_H_
#define _LcmSptB_H_

class CLcmSptB
{
public:
	enum
	{
		LCXM_MAP_NAME		= 64,
		LCXM_TILE_MAX_W		= 16,
		LCXM_TEX_NAME		= 128,
		LCXM_MDL_NAME		= 128,
		LCXM_MAP_LIGHT		= 16,

		LCXM_FILE_OFFSET	= 256,
	};


	struct VtxIdx																	// Index structure
	{
		union	{	struct	{	WORD a;	WORD b;	WORD c;	};	WORD m[3];	};

		VtxIdx()					: a(0),b(1),c(2){}
		VtxIdx(WORD A,WORD B,WORD C): a(A),b(B),c(C){}
		VtxIdx(WORD* R)				: a(R[0]),b(R[1]),c(R[2]){}

		operator WORD* ()			{	return (WORD *) &a;			}
		operator const WORD* () const{	return (const WORD *) &a;	}
	};

	typedef FLOAT	VEC6[6];
	typedef	int		SplIdx;

	struct VtxSpl
	{
		VEC3	p;
		VEC3	n;
		DWORD	d;
		FLOAT	u, v;
		VEC6	ts;

		VtxSpl()	: p(0,0,0),n(0,0,0),u(0),v(0),d(0xFFFFFFFF)
		{
			ts[0] = 0;	ts[1] = 0;	ts[2] = 0;	ts[3] = 0;	ts[4] = 0;	ts[5] = 0;
		}
		VtxSpl(FLOAT X, FLOAT Y, FLOAT Z
				, FLOAT _X, FLOAT _Y, FLOAT _Z
				, FLOAT U, FLOAT V
				, DWORD D=0xFFFFFFFF)	: p(X,Y,Z),n(_X,_Y,_Z),u(U),v(V),d(D)
		{
			ts[0] = 0;	ts[1] = 0;	ts[2] = 0;	ts[3] = 0;	ts[4] = 0;	ts[5] = 0;
		}

		enum {	FVF= (D3DFVF_XYZ|D3DFVF_NORMAL|\
					D3DFVF_DIFFUSE|D3DFVF_TEX3|\
					D3DFVF_TEXCOORDSIZE2(0)|\
					D3DFVF_TEXCOORDSIZE4(1)|\
					D3DFVF_TEXCOORDSIZE2(2)
					)
				};
	};


	struct MpTex
	{
		struct _Tex
		{
			LPDIRECT3DTEXTURE9	pTex;					// D3DTexture Pointer
			D3DXIMAGE_INFO		Image;					// Image Info
			char				sName[LCXM_TEX_NAME];	// File Name

			_Tex();
		};

		_Tex	TexB;

		MpTex();
	};

	typedef vector<MpTex >		lsMpTex;
	typedef lsMpTex::iterator	itMpTex;


public:
	D3DXMATRIX		m_mtW;					// World Matrix
	D3DXVECTOR3		m_vcP;					// Position

	INT				m_iNx;					// Mesh: Number of tile for Width
	FLOAT			m_fWx;					// Mesh: Width of tile for x;
	FLOAT			m_fUV;					// Mesh: UV Width

	INT				m_iVs;					// Vertex Size
	DWORD			m_FVF;
	INT				m_iNi;					// Index Number
	INT				m_iNv;					// Vertex Number

	VtxIdx*			m_pFce;
	VtxSpl*			m_pVtx;
	INT*			m_pVtt;

	lsMpTex			m_vTex;					// Tiling Texture Name and Pointer List

	ILcmPack*		m_pMdb2D;				// 2D Model Pack
	ILcmPack*		m_pMdb3D;				// 3D Model Pack

	BOOL			m_bRend	;				// Rendering Enable

protected:
	void*			m_pDst	;				// Parent Manager Local Field

public:
	CLcmSptB();
	virtual ~CLcmSptB();

	INT		Create(void* p1, void* p2, void* p3, void* p4);
	void	Destroy();

	INT		FrameMove();

	void	RenderBlc();
	void	RenderObj();

	void	RenderTline();														// Show Block

	FLOAT	GetHeight(D3DXVECTOR3& pos);

	void	CreateMesh(INT iNx, FLOAT fWx, FLOAT fUV);
	void	LayerSetup(lsMpTex* pTxLst);
	void	LayerAdd(MpTex* pTex);

	void	SetNormal();
	void	SetIndex();
	D3DXVECTOR3	NormalVec(int z, int x);

	void	SetDiffuse(int nIdx=-1);
	void	SetPos(D3DXVECTOR3	pos)	{	m_vcP = pos	;	}
	D3DXVECTOR3	GetPos()				{	return m_vcP;	}
	INT		GetTileN()					{	return m_iNx;	}					// Get Tile Number of X or Z
	FLOAT	GetTileW()					{	return m_fWx;	}					// Get Tile Width



	void	CalculateMap();
	void	CalculateMapTile(int nTx);
};


#endif


