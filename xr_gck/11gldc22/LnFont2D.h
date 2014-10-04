// Interface for the CLcFont2D class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcFont2D_H_
#define _LcFont2D_H_

class CLcFont2D : public ILC_Font
{
protected:
	PDTX		m_pTxD;

	INT			m_iW;					// String Width
	INT			m_iH;					// Font Height

	INT			m_iTxX;					// Texture Width
	INT			m_iTxY;					// Texture Height
	
	INT			m_iSx;					// Surface Width
	INT			m_iSy;					// Surface Height
										
	INT			m_iLen;					// String Length
										
	TCHAR*		m_sSrc;					// String
	RECT		m_rc;					// Texture Rect
										
	HFONT		m_hFnt;					
	
	DWORD		m_dDff;					// Diffuse Color
	DWORD		m_dFrn;					// front color
	DWORD		m_dBck;					// back color
	DWORD		m_dAln;					// Alignment
										
	INT			m_iTcX;					// thick X
	INT			m_iTcY;					// thick Y
										
	VEC3		m_vcP;					// String Pos
	VEC3		m_vcA;					// Align pos

public:
	CLcFont2D();
	virtual ~CLcFont2D();

	virtual INT		Create(	  void* p1=0	// LPDIRECT3DDEVICE9
							, void* p2=0	// TlnFnt
							, void* p3=0	// No Use
							, void* p4=0	// No Use
							);

	virtual void	Destroy();
	virtual void	DrawTxt();

	virtual INT		Query(char* sCmd, void* pData);

	virtual	void	SetString(TCHAR* sStr=NULL);
	virtual	void	SetPosition(FLOAT* pos=NULL/*FLOAT3 or Float2*/);
	
	virtual	INT		SetColorDiffuse	(FLOAT* pColor=NULL /*rgba <== D3DXCOLOR*/);
	virtual	INT		SetColorFore	(FLOAT* pColor=NULL /*rgba <== D3DXCOLOR*/);
	virtual	INT		SetColorBack	(FLOAT* pColor=NULL /*rgba <== D3DXCOLOR*/);
	//INT		SetColorBack(DWORD dBack = 0xFF333333);

	virtual void	SetPosAlign(DWORD dwAlign=0);

	virtual	INT		OnLostDevice();
	virtual	INT		OnReset();

	virtual	INT		GetType();


protected:
	void	SetThickX(INT iThickX=1);
	void	SetThickY(INT iThickY=1);
	void	SetThick(INT iThick=1);
	void	SetHeight(INT Height);

	
	
	INT		SetTexture();
	INT		SetDiffuse();
	BYTE*	GetColorFromARGB(DWORD& argb);
	INT		Quntum2Power(INT val);



	struct VtxDRHW
	{
		D3DXVECTOR4		p;
		DWORD			d;
		FLOAT			u,v;

		VtxDRHW(): d(0xFFFFFFFF){};

		enum	{	FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE| D3DFVF_TEX1 ),	};
	};

	VtxDRHW					m_pVtx[4];
};


#endif



