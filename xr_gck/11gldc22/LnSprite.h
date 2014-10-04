// Interface for the CLcSprite class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcSprite_H_
#define _LcSprite_H_

typedef D3DXVECTOR2					VEC2;
typedef D3DXVECTOR3					VEC3;
typedef D3DXVECTOR4					VEC4;

typedef LPDIRECT3DDEVICE9			PDEV;
typedef LPDIRECT3DTEXTURE9			PDTX;

class CLcSprite : public ILC_Sprite
{
protected:
	struct VDRHW
	{
		VEC4	p;
		DWORD	d;
		FLOAT	u0,v0;
		FLOAT	u1,v1;

		VDRHW(): d(0xFFFFFFFF){};

		enum	{	FVF = (D3DFVF_XYZRHW| D3DFVF_DIFFUSE| D3DFVF_TEX2),	};
	};

protected:
	VDRHW	m_pVtx[4];

public:
	CLcSprite();
	virtual ~CLcSprite();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();

	virtual	INT		Begin(DWORD=0);
	virtual	INT		End(DWORD=0);

	virtual	INT		OnResetDevice();
	virtual	INT		OnLostDevice();

	virtual	INT		Draw( void* pTx0				// 0 Stage Texture(<==ILcTex)
						, const RECT* pRC0			// Draw Region 0 State Texture
						, void* pPos=NULL			// Position
						, DWORD dColor =0xFFFFFFFF	// color
						, DWORD dFilter=0x0			// Filter(<==D3DMTEXF_POINT)
						);

	virtual	INT		Draw( void* pTx0				// 0 Stage Texture(<==ILcTex)
						, void* pTx1				// 1 Stage Texture(<==ILcTex)
						, const RECT* pRC0			// Draw Region 0 State Texture
						, const RECT* pRC1			// Draw Region 1 State Texture
						, void* pScl=NULL			// Scaling
						, void* pPos=NULL			// Position
						, DWORD dColor =0xFFFFFFFF	// color
						, DWORD dFilter=0x0			// Filter
						);

	virtual	INT		DrawEx( void* pTx0				// 0 Stage Texture(<==ILcTex)
						, void* pTx1				// 1 Stage Texture(<==ILcTex)
						, const RECT* pRC0			// Draw Region 0 State Texture
						, const RECT* pRC1			// Draw Region 1 State Texture
						, void* pScl				// Scaling
						, void* pPos				// Position
						, void* pRot				// Rotation Center
						, FLOAT	fAngle				// Rotation Angle(Radian)
						, DWORD dColor =0xFFFFFFFF	// color
						, DWORD dFilter=0x0			// Filter(<==D3DMTEXF_POINT)
						);

protected:
	inline	INT LcMath_MakePower2(INT a);
};

#endif


