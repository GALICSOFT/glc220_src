// Interface for the CLcAseB class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcAseB_H_
#define _LcAseB_H_

#define D3DFVF_XYZ			0x002
#define D3DFVF_NORMAL		0x010
#define D3DFVF_DIFFUSE		0x040
#define D3DFVF_TEX1			0x100
#define D3DFVF_TEX2			0x200

#pragma warning(disable : 4786)
#include <vector>
using namespace	std;


class CLcAseB : public CLcMdl
{
public:
	struct Tmtl
	{
		char		sTex[LCM_TEX_NAME];
		PLC_TEXTURE	pTex;

		Tmtl();
		~Tmtl();
	};

	struct Ttrack
	{
		INT		nF;																// Frame
		FLOAT	x;
		FLOAT	y;
		FLOAT	z;
		FLOAT	w;

		Ttrack() : nF(0), x(0), y(0), z(0), w(0){}
	};

	struct Tgeo
	{
		char		sName[32];				// Current Node Name
		INT			nPrn	;				// Parent Index
		INT			nMtl	;				// Material Index

		INT			iNix	;				// Number of Index
		INT			iNvx	;				// Number of Vertex
		DWORD		dFVF	;				// Flexible Vertex Format
		DWORD		dVtx	;				// Zero Stride

		D3DXMATRIX	mtTmL	;				// Tm Local
		D3DXMATRIX	mtW		;				// World Matrix
		D3DXMATRIX	mtL		;				// Local Matrix
		
		INT			nRot	;				// Rotation Number
		INT			nTrs	;				// Translation Number

		VtxIdx*		pIdx	;				// indexed buffer
		void*		pVtx	;				// vertex buffer
		Ttrack*		vRot	;				// Rotation
		Ttrack*		vTrs	;				// Translation

		Tgeo();
		~Tgeo();
	};

	struct Thead
	{
		char		sVer[16]	;
		INT			nFrmF		;			// First Frame
		INT			nFrmL		;			// Last Frame
		INT			nFrmS		;			// Frame Speed
		INT			nFrmT		;			// Tick per Frame

		INT			nMtl		;			// Material Number
		INT			nGeo		;			// Geometry Number

		Thead():nFrmF(0),nFrmL(0),nFrmS(0),nFrmT(0), nMtl(0), nGeo(0)
		{
			memset(sVer, 0, sizeof sVer);
		}
	};


protected:
	char		m_sName[LCM_FILE_NAME];

	Thead		m_Head		;
	Tmtl*		m_pMtl		;
	Tgeo*		m_pGeo		;
	vector<_Tframe>* m_pFrame;

	DOUBLE		m_dFrmCur	;				// Current Frame
	DOUBLE		m_dTimeCur	;				// Current Time
	INT			m_nActM		;				// Action Master Index Current

public:
	CLcAseB();
	virtual ~CLcAseB();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual	void	Destroy();
	virtual	INT		FrameMove();
	virtual	void	Render();

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		GetType()				{	return LCM_ASE;	}
	virtual const char*	const GetName()const{	return m_sName;	}
	virtual ILcMdl* GetOriginal()			{	return this;	}

	virtual INT		SetAttrib(char* sCmd, void* pVal);
	virtual INT		GetAttrib(char* sCmd, void* pVal);

public:
	Thead*	GetHeader() const	{	return (Thead*)&m_Head;	}
	Tmtl*	GetMtrl()	 const	{	return m_pMtl;	}
	Tgeo*	GetGeometry() const	{	return m_pGeo;	}
	INT		GetAniTrack(void* mtA, INT nGeo, FLOAT dFrame);

	vector<_Tframe>* GetFrame() const	{	return m_pFrame	;	}

protected:
	INT		LoadTexture(char* sPath);
	INT		LoadFromFile(char* sPath);
};

#endif

