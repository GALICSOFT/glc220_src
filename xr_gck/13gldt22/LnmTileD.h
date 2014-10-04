// Interface for the CLcmTileD(District) class.
// District Class
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcmTileD_H_
#define _LcmTileD_H_

#pragma warning(disable : 4786)
#include <vector>
using namespace std;


class CLcmTileD : public ILcmFld		// Lc Map District Field Mananger
{
public:
	enum
	{
		LCXM_MAP_NAME		= 64,
		LCXM_TEX_NAME		= 128,
		LCXM_MDL_NAME		= 128,
		LCXM_MAP_LIGHT		= 16,

		LCXM_FILE_OFFSET	= 256,
	};

	struct _Inf							// Field Information
	{
		struct
		{
			INT		nI		;			// Index
			char	sN[LCXM_MAP_NAME];	// Name
			INT		iNfX	;			// Number of Field for X Direction
			INT		iNfZ	;			// Number of Field for Z Direction
			INT		iExt	;			// Extent of Local for X Direction
			INT		iNx		;			// Number of Cells in Block for X or Z Direction
		};

		_Inf();
	};


	struct _VecT
	{
		INT			iZ;
		INT			iX;
		INT			iK;
		VEC3		p;
		FLOAT		iE;

		_VecT();
		_VecT(INT Z, INT X, INT K, const VEC3& P, INT E=0);
	};


	struct _Pck																	// Picking Triangle
	{
		VEC3	vcPk;																// Get Position
		VEC3	p0;																	// Triangle
		VEC3	p1;																	// Triangle
		VEC3	p2;																	// Triangle
		FLOAT		fStlSrtR;															// Distance From Camera

		_Pck();
		_Pck(VEC3& vcP, VEC3& P0, VEC3& P1, VEC3& P2, FLOAT& _fD);
	};


	typedef vector<_Pck >		lsTlPkT;
	typedef lsTlPkT::iterator	itTlPkT;


	struct _Hgt				// Height Filed Setup structure
	{
		INT 	iFlat;		// 0 or 1
		INT 	iRng;		// Radius or range
		INT 	bAc;		// Active Color
		FLOAT	fH;			// Increase Height Value
		FLOAT	fDH;		// Direction : 1 or -1
	};

	struct _Tex
	{
		LPDIRECT3DTEXTURE9	pTex;			// D3DTexture Pointer
		D3DXIMAGE_INFO		Image;			// Image Info
		char	sName[LCXM_TEX_NAME];		// File Name

		_Tex();
		_Tex(const _Tex&);
		_Tex(const _Tex*);
		void operator =(const _Tex&);
		void operator =(const _Tex*);
	};


protected:
	CLcmTileD::_Inf	m_Inf	;
	CLcmTileB**		m_ppLcl	;
	CLcmTileD::_Tex	m_pTex	;


	CLcmTileB*		m_pPkLc	;			// Picking Local
	CLcmTileD::lsTlPkT	m_vPkLc	;			// Picking Triangle from Local
	BOOL			m_bClck	;
	VEC3			m_vcPck	;			// Final Picking Position from Local

protected:
	vector<CLcmTileB*> m_vLcl	;
	FLOAT			m_fRangeMax	;		// Max View Range

	PDVD			m_pFVF	;
	LPD3DXEFFECT	m_pEft	;

	lsLcMdl			m_vObj	;			// Object List

public:
	CLcmTileD();
	virtual ~CLcmTileD();

	virtual	INT		Create(void* p1=0, void* p2=0, void* p3=0, void* p4=0);
	virtual	void	Destroy();

	virtual	INT		FrameMove();
	virtual	void	Render();

	virtual INT		Query(char* sCmd, void* pData);
	virtual	INT		GetHeight(float* vcOut, const float* vcIn);

public:
	INT			FileLoad(char* sFile);
	void		CreateField(CLcmTileD::_Inf*, CLcmTileD::_Tex*);			// Create Field

	void		RenderPck();

	void*		GetFldInf();
	char*		GetFldName();
	void*		GetVertexDeclarator();
	void*		GetD3DXEffect();
	void*		GetTexture();
	void*		GetCam();
	
	void		AddObj(ILcMdl* pMdB);

protected:
	INT			PickField();
	INT			PickPosition(CLcmTileB**		pBlock
							, CLcmTileD::lsTlPkT*	pPickInf	// Picking Triangle
							, VEC3*				pvcOut		// Picking position
							, const VEC3*		pvcRayPos	// Ray Begin Position
							, const VEC3*		pvcRayDir	// Ray Direction
						   );

	INT			PickLocal();

public:
	FLOAT		m_fCull			;
	BOOL		m_bCtrlCull		;
	BOOL		m_bCtrlPck		;

	BOOL		m_bCtrlHeight	;
	BOOL		m_bCtrlSpoid	;

	DWORD		m_dFillColor	;
	BOOL		m_bCtrlBrush	;
	BOOL		m_bCtrlModel	;

	BOOL		m_bCtrlTileFront;
	BOOL		m_bCtrlTileBack	;
	BOOL		m_bCtrlBndBox	;

	BOOL		m_bCtrlLclLine	;

	_Hgt		m_Hgt		;
	VEC3		m_vcRayDir	;
	VEC3		m_vcCamPos	;

	ILcMdl*		m_pMdB		;
	ILcCam*		m_pCam		;
	ILcTbl*		m_pTbMdl	;

	INT			m_nTile		;


protected:
	void	SetupHeight();
	void	SetupSpoid();
	void	SetupBrush();
	void	SetupTileFront();
	void	SetupTileBack();
	void	SetupModel();

	void	SetupTexture(CLcmTileD::_Tex* pTex);
	INT		LoadTexture(char* sFile);
};

#endif

