// Interface for the ILcMdl class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcMdl_H_
#define _LcMdl_H_

typedef D3DXVECTOR2						VEC2;
typedef D3DXVECTOR3						VEC3;
typedef D3DXVECTOR4						VEC4;

typedef D3DXMATRIX						MATA;
typedef D3DXQUATERNION					QUAT;
typedef D3DXCOLOR						DCLR;
typedef D3DXPLANE						DPLN;

typedef D3DMATERIAL9					DMTL;
typedef LPDIRECT3DTEXTURE9				PDTX;

typedef	LPDIRECT3DDEVICE9				PDEV;
typedef LPDIRECT3DVERTEXDECLARATION9	PDEC;
typedef LPD3DXEFFECT					PDEF;


struct VtxIdx																	// 16bit Index structure
{
	union	{	struct	{	WORD a;	WORD b;	WORD c;	};	WORD m[3];	};

	VtxIdx()						: a(0),b(1),c(2){}
	VtxIdx(WORD A,WORD B,WORD C)	: a(A),b(B),c(C){}
	VtxIdx(WORD* R)					: a(R[0]),b(R[1]),c(R[2]){}

	operator WORD* ()				{	return (WORD *) &a;			}
	operator const WORD* () const	{	return (const WORD *) &a;	}

	enum	{FVF = (D3DFMT_INDEX16),};
};

struct Vtx
{
	VEC3	p;
	Vtx() : p(0,0,0){}
	enum	{FVF = (D3DFVF_XYZ),};
};

struct VtxD
{
	VEC3	p;
	DWORD	d;
	
	VtxD() : p(0,0,0), d(0xFFFFFFFF){}
	VtxD(FLOAT X,FLOAT Y,FLOAT Z,DWORD D=0xFFFFFFFF) : p(X,Y,Z), d(D){}
	enum	{FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE),};
};

struct VtxUV1			// Texture 1
{
	VEC3	p;
	FLOAT	u, v;

	VtxUV1()				: p(0,0,0),u(0),v(0){}
	VtxUV1(FLOAT X,FLOAT Y,FLOAT Z
		,FLOAT U,FLOAT V)	: p(X,Y,Z),u(U),v(V){}

	enum { FVF = (D3DFVF_XYZ|D3DFVF_TEX1) };
};

struct VtxDUV1			// Diffuse + Texture 1
{
	VEC3	p;
	DWORD	d;
	FLOAT	u, v;

	VtxDUV1()						: p(0,0,0),u(0),v(0),d(0xFFFFFFFF){}
	VtxDUV1(FLOAT X,FLOAT Y,FLOAT Z
		,FLOAT U,FLOAT V, DWORD D=0xFFFFFFFF)	: p(X,Y,Z),u(U),v(V),d(D){}

	enum { FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1) };
};

struct VtxNDUV1																	// Normal + Diffuse + Texture1
{
	VEC3	p;
	VEC3	n;
	DWORD	d;
	FLOAT	u,v;

	VtxNDUV1()						: p(0,0,0),n(0,0,0),u(0),v(0),d(0xFFFFFFFF){}
	VtxNDUV1(FLOAT X,FLOAT Y,FLOAT Z
			,FLOAT nX,FLOAT nY,FLOAT nZ
			,FLOAT U,FLOAT V,DWORD D=0xFFFFFFFF) : p(X,Y,Z),n(nX,nY,nZ),u(U),v(V),d(D){}

	enum { FVF = (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1) };
};

struct VtxNDUV1i
{
	VEC3  p;						// Position

	VEC3	n;						// Normal
	DWORD	d;						// Diffuse
	FLOAT	u,v;					// Texture Coordinate

	VEC4	w;						// blend weight
	VEC4	g;						// blend index

	VtxNDUV1i()	: p(0,0,0)
				, n(0,0,0)
				, u(0),v(0)
				, w(0,0,0,0)
				, g(0,0,0,0)
				, d(0xFFFFFFFF){}

	enum	{
			FVF = (	D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|
					D3DFVF_TEX3|				//
					D3DFVF_TEXCOORDSIZE2(0)|	// texture
					D3DFVF_TEXCOORDSIZE4(1)|	// blend weight
					D3DFVF_TEXCOORDSIZE4(2)		// blend index
					),
			};
};
	
	
	struct CLcmId
{
	union 
	{
		struct 
		{
			union { struct { union { WORD _d; WORD _I4; }; union { WORD _c;	WORD _I3; }; }; DWORD S; };
			union { struct { union { WORD _b; WORD _I2; }; union { WORD _a; WORD _I1; }; }; DWORD M; };
		};

		ULONGLONG _Id;
	};

	CLcmId(): _Id(0xFFFFFFFFFFFFFFFF)	{}
	CLcmId(WORD I1,WORD I2=0xFFFF,WORD I3=0xFFFF,WORD I4=0xFFFF): _I1(I1),_I2(I2),_I3(I3),_I4(I4)	{}
	CLcmId(ULONGLONG I) : _Id(I){}

	void		SetId(WORD I1=0xFFFF,WORD I2=0xFFFF,WORD I3=0xFFFF,WORD I4=0xFFFF){_I1=I1;_I2=I2;_I3=I3;_I4=I4;}
	void		SetId(ULONGLONG Id=0xFFFFFFFFFFFFFFFF){	_Id = Id;	}
	void		SetId1(WORD I1=0xFFFF)	{	_I1 = I1;	}
	void		SetId2(WORD I2=0xFFFF)	{	_I2 = I2;	}
	void		SetId3(WORD I3=0xFFFF)	{	_I3 = I3;	}
	void		SetId4(WORD I4=0xFFFF)	{	_I4 = I4;	}

	void		SetM(DWORD	m=0xFFFFFFFF){	M = m;	}
	void		SetS(DWORD	s=0xFFFFFFFF){	S = s;	}

	ULONGLONG	GetId()		{	return _Id;	}
	WORD		GetId1()	{	return _I1;	}
	WORD		GetId2()	{	return _I2;	}
	WORD		GetId3()	{	return _I3;	}
	WORD		GetId4()	{	return _I4;	}

	DWORD		GetM()		{	return M;	}
	DWORD		GetS()		{	return S;	}
};


	
class CLcMdl : public ILcMdl, public CLcmId
{
public:
	FLOAT		m_fStlSrtR;				// Z Direction Value for Sort...

protected:
	void*		m_pCam	;				// Camera Pointer

	VEC4		m_vcRot	;				// Rotation: Axis and angle
	VEC3		m_vcScl	;				// Scaling
	VEC3		m_vcTrn	;				// Translation

	MATA		m_mtRot	;				// Rotation MatrixMATA		m_mtRot;														// Rotation Matrix
	MATA		m_mtWld	;				// World which is mtW = mtR * mtS, mtW_41 = pos.x, ....

	INT			m_nRnd	;				// Rendering Level
	TbndAB		m_BndInf;

public:
	CLcMdl();
	virtual ~CLcMdl();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		GetType()				{	return LCM_NONE;}
	virtual const char*	const GetName()const{	return NULL;	}
	virtual ILcMdl* GetOriginal()			{	return NULL;	}

	virtual INT		SetAttrib(char* sCmd, void* pVal);
	virtual INT		GetAttrib(char* sCmd, void* pVal);

	virtual INT		SetPosition(FLOAT* float3);
	virtual INT		SetRotation(FLOAT* float4, INT nOpt=LCM_ROT_XYZ);
	virtual INT		SetScaling(FLOAT* float3);

	virtual INT		GetPosition(FLOAT* float3);
	virtual INT		GetRotation(FLOAT* float4);
	virtual INT		GetScaling(FLOAT* float3);
	virtual	FLOAT	GetSortValue();
	virtual	const TbndAB* const	GetBndInf() const{	return &m_BndInf;	}

protected:
	void		WorldMatrixSetup();
	void		SetupSortValue();
	void		SetupReneringLevel(void* p);
};


void Lcm_BoundBoxTransform(TbndAB* pDst, const TbndAB* pSrc, const D3DXMATRIX* mtWld, const VEC3* vcEps);
void Lcm_BoundBoxRender(PDEV pDev, TbndAB* pBnd);
void Lcm_BoundBoxSetup(TbndAB* pDst, const TbndAB* pSrc, void* Owner);

INT LoadAtc(void** pOut, char* sFile);


#endif

