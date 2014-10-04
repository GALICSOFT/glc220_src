// Interface for the CLcSpriteX class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcSpriteX_H_
#define _LcSpriteX_H_

typedef D3DXVECTOR2							VEC2;
typedef	D3DXVECTOR3							VEC3;
typedef D3DXVECTOR4							VEC4;
typedef D3DXMATRIX							MATA;
typedef D3DXCOLOR							DCOL;

typedef LPDIRECT3DDEVICE9					PDEV;

typedef LPDIRECT3DVERTEXSHADER9				PDVS;
typedef LPDIRECT3DPIXELSHADER9				PDPS;
typedef LPDIRECT3DVERTEXDECLARATION9		PDVD;

typedef LPDIRECT3DTEXTURE9					PDTX;
typedef	ID3DXEffect*						PDEF;


struct LcTexture
{
	INT		ImgW;		// Original Image Width
	INT		ImgH;		// Original Image Height
	INT		ImgD;		// Original Image Depth

	INT		TexW;		// Texture Width
	INT		TexH;		// Texture Height
	PDTX	pTex;
};

class CLcSpriteX : ILC_Sprite
{
public:
	struct VtxRHWUV1
	{
		VEC2	p;
		FLOAT	z;
		FLOAT	w;
		VEC2	t0;
		VEC2	t1;
		
		VtxRHWUV1()	: p(0,0), z(0), w(1), t0(0,0), t1(0,0){}
		VtxRHWUV1(FLOAT X,FLOAT Y,FLOAT Z
					,FLOAT U0,FLOAT V0
					,FLOAT U1,FLOAT V1
					):p(X,Y), z(Z), w(1), t0(U0,V0), t1(U1,V1){}

		enum {FVF = (D3DFVF_XYZRHW|D3DFVF_TEX2),};
	};

public:
	PDVD			m_pFVF;
	PDEF			m_pEft;			// ID3DXEffect
	VtxRHWUV1		m_pVtx[4];		// Vertex Buffer

	INT				m_nScnW;
	INT				m_nScnH;

	
public:
	CLcSpriteX();
	virtual ~CLcSpriteX();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy()=0;

	virtual	INT		Begin(DWORD=0);
	virtual	INT		End(DWORD=0);

	virtual	INT		OnResetDevice();
	virtual	INT		OnLostDevice();

	virtual	INT		Draw( void* pTx0				// 0 Stage Texture(<==ILcTex)
						, void* pTx1				// 1 Stage Texture(<==ILcTex)
						, const RECT* pRC0			// Draw Region 0 State Texture
						, const RECT* pRC1			// Draw Region 1 State Texture
						, void* pScl=NULL			// Scaling
						, void* pPos=NULL			// Position
						, DWORD dColor =0xFFFFFFFF	// color
						, DWORD dFilter=0x0			// Filter(<==D3DMTEXF_POINT)
						);

	virtual	INT		DrawEx( void* pTx0				// 0 Stage Texture(<==ILcTex)
						, void* pTx1				// 1 Stage Texture(<==ILcTex)
						, const RECT* pRC0			// Draw Region 0 State Texture
						, const RECT* pRC1			// Draw Region 1 State Texture
						, void* pScl				// Scaling
						, void* pPos				// Position
						, void* pRot				// Rotation Center
						, FLOAT	fAngle				// Rotation Angle(Radian)
						, DWORD dColor				// color
						, DWORD dFilter=0x0			// Filter(<==D3DMTEXF_POINT)
						);
protected:	
	INT		DrawExt(  void* pLcTex0
						, void* pLcTex1
						, const RECT* pRc1
						, const RECT* pRc2
						, VEC2* pScl
						, VEC2* pRot
						, FLOAT fRot
						, VEC2* pTrn
						, DWORD _dCol
						, INT _bMono);
};

#endif
