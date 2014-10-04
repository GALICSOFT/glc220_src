// Interface for the LCX_Ase class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxAse_H_
#define _LcxAse_H_

#include <vector>


namespace LCX_Ase
{
	struct _AseKey
	{
		char	sKey[40];		// String Keword
		INT		iKey;			// String Size
	};


	// Face: 16-bit Triangle Index Stream
	#if defined(_MSC_VER)

		#pragma pack(push, 2)
		struct Tidx
		{
			union{	struct{ WORD a; WORD b; WORD c;}; WORD m[3]; };

			Tidx() : a(0), b(1), c(2){}
			Tidx(WORD A,WORD B,WORD C) : a(A), b(B), c(C){}
			Tidx(const WORD* v)	{ a = v[0]; b = v[1]; c = v[2];	}

			operator WORD*()			{	return (WORD*)&a;		}
			operator const WORD*() const{	return (const WORD*)&a;	}

			enum { FVF = (LC_FMT_INDEX16), };
		};
		#pragma pack(pop)

	#else

		typedef struct tagTidx
		{
			WORD a; WORD b; WORD c;

			tagTidx() : a(0), b(1), c(2){}
			tagTidx(WORD A,WORD B,WORD C) : a(A), b(B), c(C){}

			operator WORD*()			{	return (WORD*)&a;		}
			operator const WORD*() const{	return (const WORD*)&a;	}

			enum { FVF = (LC_FMT_INDEX16), };
		} __attribute__((packed)) Tidx;

	#endif


	struct _AsePos
	{
		FLOAT	x, y, z;
		_AsePos() : x(0), y(0), z(0){}
	};


	struct _AseNor
	{
		FLOAT	x, y, z;
		INT		a, b, c;
		_AseNor() : x(0), y(0), z(0), a(0), b(0), c(0){}
	};


	struct _AseTvtx
	{
		INT		nT;						// 배열에서 정점 또는 UV의 인덱스
		FLOAT	x, y, z;				// position
		FLOAT	nx, ny, nz;				// normal
		UINT	d;					// color
		FLOAT	u, v, w;				// UVW좌표
		_AseTvtx()
				:	nT(-1)
				,	x(0), y(0), z(0)
				,	nx(0), ny(0), nz(0)
				,	d(0xFFFFFFFF)
				,	u(0), v(0), w(0){}

		_AseTvtx(INT _t
				, FLOAT _x, FLOAT _y, FLOAT _z
				, FLOAT _nx, FLOAT _ny, FLOAT _nz
				, UINT _d
				, FLOAT _u, FLOAT _v, FLOAT _w)
				:	nT(_t)
				,	x ( _x), y ( _y), z ( _z)
				,	nx(_nx), ny(_ny), nz(_nz)
				,	d(_d)
				,	u(_u), v(_v), w(0){}
	};


	struct _AseMtl
	{
		char			sTex[LC_MAX_PATH];
		_AseMtl();
	};


	struct Tvtx
	{
		LCXVECTOR3	p;
		Tvtx() : p(0,0,0){}
		enum	{FVF = (LC_FVF_XYZ),};
	};


	struct TvtxUV
	{
		LCXVECTOR3	p;
		FLOAT	u, v;

		TvtxUV() : p(0,0,0), u(0),v(0){}
		enum	{FVF = (LC_FVF_XYZ|LC_FVF_TEX1),};
	};


	struct _AseTM
	{
		LCXMATRIX	mtW;				// World Matrix
		LCXMATRIX	mtL;				// Local Matirx

		FLOAT	Px, Py, Pz;
		FLOAT	Rx, Ry, Rz, Rw;
		FLOAT	Sx, Sy, Sz;

		_AseTM() : Px(0), Py(0), Pz(0)
				, Rx(0), Ry(0), Rz(0), Rw(0)
				, Sx(0), Sy(0), Sz(0){}
	};


	struct Ttrack4
	{
		INT		nF;						// Frame
		FLOAT	x;
		FLOAT	y;
		FLOAT	z;
		FLOAT	w;

		Ttrack4() : nF(0), x(0), y(0), z(0), w(0){}
		Ttrack4(INT F,FLOAT X,FLOAT Y,FLOAT Z,FLOAT W):nF(F),x(X),y(Y),z(Z),w(W){}
	};


	struct Ttrack
	{
		INT		nF;						// Frame
		LCXMATRIX4X3	tm;

		Ttrack()
		{
			nF = 0;
			memset(&tm, 0, sizeof(LCXMATRIX4X3) );
		}

		Ttrack(INT F, const LCXMATRIX4X3* _tm) :nF(F)
		{
			memcpy(&tm, _tm, sizeof(LCXMATRIX4X3) );
		}
	};


	struct _AseGeo
	{
		char		sNodeCur[32];		// Current Node Name
		char		sNodePrn[32];		// Parent Node Name
		INT			nNodePrn;			// Parent Index

		INT			nMtlRef	;			// Material Index

		// for ASE Parsing
		INT			iNumVtx	;			// Number of Vertex
		INT			iNumFce	;			// Number of Index

		INT			iNumTvtx;			// Number of Vertex
		INT			iNumTfce;			// Number of Index

		_AsePos*	pLstVtx	;			// Vertex List
		Tidx*		pLstFce	;			// Vertex Face List
		_AseTvtx*	pLstTvtx;			// UV List
		Tidx*		pLstTfce;			// UV Face


		_AseGeo*		pGeoPrn;
		_AseTM			TmInf;			// Transform and Movement
		std::vector<Ttrack4 > vRot;		// Rotation
		std::vector<Ttrack4 > vTrs;		// Translation
		std::vector<Ttrack4 > vScl;		// Scaling



		// Final Rendering Geometry Info
		LCXMATRIX	mtW		;			// World Matrix
		LCXMATRIX	mtL		;			// Local Matrix

		_AseGeo();
		~_AseGeo();
	};


	struct TInfo
	{
		char	sVer[16];				// For Converting Binary version
		INT		nFrmF	;				// First Frame
		INT		nFrmL	;				// Last Frame
		INT		nFrmS	;				// Frames per one scecond
		INT		nFrmT	;				// Tick per one Frame

		INT		nMtl	;				// Number of Material
		INT		nGeo	;				// Number of Geometry

		TInfo():nFrmF(0),nFrmL(0),nFrmS(0),nFrmT(0), nMtl(0),nGeo(0)
		{
			memset(sVer, 0, sizeof sVer);
			strcpy(sVer, "LcAsm00.01");
		}
	};



	struct Tmtl
	{
		char		sTex[LCXM_TEXNAME];
		PLC_TEXTURE	pTex;

		Tmtl();
		~Tmtl();
	};



	struct Tgeo
	{
		char		sName[32];			// Current Node Name
		INT			nPrn	;			// Parent Index
		INT			nMtl	;			// Material Index

		INT			iFce	;			// Number of Face
		INT			iNvx	;			// Number of Vertex
		Tidx*		pIdx	;			// indexed buffer
		void*		pVtx	;			// vertex buffer
		UINT		dFVF	;			// Flexible Vertex Format
		UINT		dVtx	;			// Zero Stride

		LCXMATRIX	mtTmL	;			// Tm Local
		LCXMATRIX	mtW		;			// World Matrix
		LCXMATRIX	mtL		;			// Local Matrix

		INT			nAni	;			// Rotation Number
		Ttrack*		vAni	;

		Tgeo();
		~Tgeo();
	};


	struct TlinkTm
	{
		LCXMATRIX	mtW		;		// World Matrix
		LCXMATRIX	mtL		;		// Local Matrix

		INT			nPrn	;		// Parent Node Index

		TlinkTm();
	};




	INT GetAniFrame(INT* First, INT* Last, INT* oAni, INT** oPivot, LCX_Ase::_AseGeo* pSrc);
	INT	GetAniTrack(LC_HANDLE OutTm, FLOAT dFrame, LCX_Ase::_AseGeo* pSrc);
	INT GetAniMatrix(LC_HANDLE OutTm, FLOAT dFrame, LCX_Ase::Tgeo* pGeo);
};


#endif

