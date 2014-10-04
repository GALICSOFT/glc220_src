// Interface for the CLcXSkin class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcXSkin_H_
#define _LcXSkin_H_

typedef D3DXVECTOR2						VEC2;
typedef D3DXVECTOR3						VEC3;
typedef D3DXVECTOR4						VEC4;

typedef D3DXMATRIX						MATA;
typedef D3DXQUATERNION					QUAT;
typedef D3DXCOLOR						DCLR;
typedef D3DXPLANE						DPLN;


struct TexLayer
{
	enum	{	MAX_TEXTURE_LAYER	=	8,	};

	union
	{
		struct
		{
			LPDIRECT3DTEXTURE9	p0;
			LPDIRECT3DTEXTURE9	p1;
			LPDIRECT3DTEXTURE9	p2;
			LPDIRECT3DTEXTURE9	p3;
			LPDIRECT3DTEXTURE9	p4;
			LPDIRECT3DTEXTURE9	p5;
			LPDIRECT3DTEXTURE9	p6;
			LPDIRECT3DTEXTURE9	p7;
		};

		LPDIRECT3DTEXTURE9 p[MAX_TEXTURE_LAYER];
	};

	TexLayer();
};


// enum for various skinning modes possible
enum ESkinType
{
	SKINNING_NONINDEXED,
	SKINNING_INDEXED,
	SKINNING_SOFTWARE,
	SKINNING_INDEXEDVS,
	SKINNING_INDEXEDHLSL,
};

class CLcXSkin : public CLcMdl
{
protected:
	FLOAT		m_fElapse	;			// Elapsed Time
	DOUBLE		m_dTimeCur	;			// Current Time

public:
	CLcXSkin();
	virtual ~CLcXSkin();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)	{	return 0;	}
	virtual void	Destroy()										{				}
	virtual INT		FrameMove()										{	return 0;	}
	virtual void	Render()										{				}

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		GetType()				{	return LCM_XFILE;	}

	virtual INT		SetAttrib(char* sCmd, void* pVal);
	virtual INT		GetAttrib(char* sCmd, void* pVal);
};



#endif


