// Interface for the CLcxAsb class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#if defined(_MSC_VER) && _MSC_VER > 1000
  #pragma warning(disable: 4530)
  #pragma warning(disable: 4786)
#endif


#ifndef _LcxAcm_H_
#define _LcxAcm_H_

#include <vector>


typedef	LCXVECTOR2		VEC2;
typedef	LCXVECTOR3		VEC3;
typedef	LCXVECTOR4		VEC4;
typedef	LCXMATRIX		MATA;
typedef	LCXQUATERNION	QUAT;

typedef	LC_INT4		INT4;


class CLcxAcm : public ILCX_Mdl
{
public:
	enum ELcm
	{
		LCX_ETC		= 0,		// Geometry Type
		LCX_MESH	= 1,
		LCX_BONE	= 2,

		LCM_MAX_BLEND		= 128,
		LCM_TEX_NAME		= 128,
	};



	// Face: 16-bit Triangle Index Stream
	#if defined(_MSC_VER)

		#pragma pack(push, 2)
		struct Tidx
		{
			WORD a;	WORD b;	WORD c;
			Tidx() : a(0), b(1), c(2){}
		};
		#pragma pack(pop)

	#else

		typedef struct tagTidx
		{
			WORD a;	WORD b;	WORD c;
			tagTidx() : a(0), b(1), c(2){}
		} __attribute__((packed)) Tidx;

	#endif


	struct Tbone
	{
		INT		nB;						// Bone Index
		FLOAT	fW;						// Weight

		Tbone() : nB(-1), fW(0.f){}
		Tbone(INT b, FLOAT w): nB(b), fW(w){}
	};

	struct LcmBone
	{
		std::vector<Tbone>	vB;			// Bone Index
	};

	struct Ttrack
	{
		INT				nF;				// Frame
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



	struct TInfo
	{
		char	sVer[16];		// For Converting Binary version
		INT		nFrmF	;		// First Frame
		INT		nFrmL	;		// Last Frame
		INT		nFrmS	;		// Frames per one scecond
		INT		nFrmT	;		// Tick per one Frame

		INT		nMtl	;		// Number of Material
		INT		nGeo	;		// Number of Geometry

		TInfo():nFrmF(0),nFrmL(0),nFrmS(0),nFrmT(0), nMtl(0),nGeo(0)
		{
			memset(sVer, 0, sizeof sVer);
			strcpy(sVer, "LcAsm00.01");
		}
	};

	struct Tmtl
	{
		char			sTex[LCM_TEX_NAME];
		PLC_TEXTURE	pTex;

		Tmtl();
		Tmtl(char* s, LC_HANDLE p);
		~Tmtl();
	};


	struct Tgeo
	{
		char		sName[32];	// Node Name
		INT			nType;		// 1:Geometry, 2: Bone, 0: Etc

		INT			nPrn;		// Parent Index
		Tgeo*		pPrn;		// Parent Node

		INT			nFce;		// Number of Face
		INT			nVtx;		// Vertex Number
		UINT		dVtx;		// Stride of Vertex

		Tidx*		pFce;		// Face List

		VEC3*		pVsP;		// Vertex Source Position
		VEC4*		pVsW;		// BLEND WEIGHT
		INT4*		pVsB;		// Blend MATRIX Index
		VEC3*		pVsN;		// Vertex Source Normal vector
		VEC2*		pVsT;		// Vertex Source Texture Coordinate

		VEC3*		pVdP;		// Vertex Dest Position

		INT			nBnd;		// take effect for Vertex Number for Bone
		LcmBone*	pBnd;		// Dest

		INT			nAni;		// Number of Animation
		Ttrack*		vAni;		// Animation Matrix

		INT			nMtl;		// Texture File Index


		MATA		mtW;		// Blending World Matrix

		Tgeo();
		~Tgeo();
	};


protected:
	char			m_sFile[LC_MAX_PATH];

	CLcxAcm::TInfo	m_Info		;
	CLcxAcm::Tgeo*	m_pGeo		;		// Geometry Data
	CLcxAcm::Tmtl*	m_pMtl		;


	INT				m_BlndNum	;


	DOUBLE			m_TimeC		;		// Current Time
	FLOAT			m_FrameC	;		// Current Frame
	INT				m_FrameB	;		// Animation Begin Frame
	INT				m_FrameE	;		// Animation End Frame
	LCXMATRIX		m_mtWld		;		// World Matrix


public:
	CLcxAcm();
	virtual ~CLcxAcm();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	virtual void	Destroy();

	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);
	virtual INT		GetRscType()	{	return LCXM_SRC_ACM;	}


public:
	const CLcxAcm::TInfo*	GetInfo()	 const	{	return &m_Info; }
	CLcxAcm::Tmtl*			GetMtrl()	 const	{	return m_pMtl;	}
	CLcxAcm::Tgeo*			GetGeometry() const	{	return m_pGeo;	}

	INT		GetAniMatrix(LC_HANDLE OutTm, FLOAT dFrame, CLcxAcm::Tgeo* pGeo);

protected:
	INT		Load();
	INT		LoadTexture(LC_HANDLE pOption);
};

#endif

