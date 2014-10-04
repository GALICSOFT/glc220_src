// Interface for the CLcmSptD class.
// District Class
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcmSptD_H_
#define _LcmSptD_H_

#pragma warning(disable : 4786)
#include <vector>
using namespace std;


class CLcmSptD : public ILcmFld		// Lcx Map District Field Mananger
{
public:
	struct _Inf
	{
		union
		{
			struct
			{
				INT		nI							;		// Index
				char	sN[CLcmSptB::LCXM_MAP_NAME]	;		// Name

				INT		iNfX	;		// Number of Field for X Direction
				INT		iNfZ	;		// Number of Field for Z Direction
				INT		iExt	;		// Extent of Local for X Direction
				INT		iNx		;		// Number of Cells in Block for X or Z Direction
				FLOAT	fUV		;		// UV value
			};

			char		_t[CLcmSptB::LCXM_FILE_OFFSET];		// Map 크기를 맞추기 위한 Offset Value
		};

		_Inf();
	};



	struct _Fog
	{
		DWORD		m_dFt		;		// Fog Table Mode
		DWORD		m_dFc		;		// Fog Color
		FLOAT		m_fFb		;		// Fog Begin
		FLOAT		m_fFe		;		// Fog End
		FLOAT		m_fFd		;		// Fog Density

		_Fog();

		void	SetFog(void* pDev);
	};


	// Material Redefine
	union _Mtl
	{
		struct
		{
			D3DXCOLOR	dcD	;			// Diffuse color RGBA
			D3DXCOLOR	dcA	;			// Ambient color RGB
			D3DXCOLOR	dcS	;			// Specular 'shininess'
			D3DXCOLOR	dcE	;			// Emissive color RGB
			FLOAT		fPw	;			// Sharpness if specular highlight
		};

		D3DMATERIAL9 m;
	};

	union _Lgt
	{
		struct
		{
			UINT		Type	;		// Type of light source
			D3DXCOLOR	dDif	;		// Diffuse color of light
			D3DXCOLOR	dSpc	;		// Specular color of light
			D3DXCOLOR	dAmb	;		// Ambient color of light
			D3DXVECTOR3	vPos	;		// Position in world space
			D3DXVECTOR3	vDir	;		// Direction in world space
	//		FLOAT		vEtc[7]	;
			FLOAT		Range	;		// Cutoff range
			FLOAT		Falloff	;		// Falloff
			FLOAT		Attn0	;		// constant attenuation
			FLOAT		Attn1	;		// Linear attenuation
			FLOAT		Attn2	;		// Quadratic attenuation
			FLOAT		Theta	;		// Inner angle of spotlight cone
			FLOAT		Phi		;		// Outer angle of spotlight cone
		};

		D3DLIGHT9		l;				// Lighting

		operator D3DLIGHT9* ()
		{
			return (D3DLIGHT9 *) &Type;
		}

		operator const D3DLIGHT9* () const
		{
			return (const D3DLIGHT9 *) &Type;
		}
	};


	struct _Env
	{
		DWORD		m_dAb	;			// Ambient color
		_Mtl		m_Mtl	;			// Material

		_Fog		m_Fog	;
		INT			m_iLg	;			// Lighting Num
		_Lgt		m_pLg[CLcmSptB::LCXM_MAP_LIGHT];			// Lighting

		_Env();
	};


	struct _VecT
	{
		INT			iZ;
		INT			iX;
		INT			iK;
		D3DXVECTOR3	p;
		FLOAT		iE;

		_VecT();
		_VecT(INT Z, INT X, INT K, const D3DXVECTOR3& P, INT E=0);
	};


	struct _Pck																	// Picking Triangle
	{
		D3DXVECTOR3	vcPk;														// Get Position
		D3DXVECTOR3	p0;															// Triangle
		D3DXVECTOR3	p1;															// Triangle
		D3DXVECTOR3	p2;															// Triangle
		FLOAT		fStlSrtR;													// Distance From Camera

		_Pck();
		_Pck(D3DXVECTOR3& vcP, D3DXVECTOR3& P0, D3DXVECTOR3& P1, D3DXVECTOR3& P2, FLOAT& _fD);
	};


	typedef vector<_Pck >		lsTlPkT;
	typedef lsTlPkT::iterator	itTlPkT;


	struct MpHgt			// Height Filed Setup structure
	{
		INT 	iFlat;		// 0 or 1
		INT 	iRng;		// Radius or range
		INT 	bAc;		// Active Color
		FLOAT	fH;			// Increase Height Value
		FLOAT	fDH;		// Direction : 1 or -1
	};



protected:
	CLcmSptD::_Inf	m_Inf	;
	CLcmSptD::_Env	m_Env	;
	CLcmSptB**		m_ppLcl	;

	CLcmSptB*		m_pLcT	;			// Local Temp
	CLcmSptB*		m_pPkLc	;			// Picking Local
	CLcmSptD::lsTlPkT	m_vPkLc	;			// Picking Triangle from Local
	BOOL			m_bClck	;
	VEC3			m_vcPck	;			// Final Picking Position from Local

public:
	lsPkBndA		m_vPkObj;			// Picking Object Bound Box
	ILcMdl*			m_pPkObj;			// Picking Object
	VEC3			m_vcPkObj;			// Final Picking Object Position

	lsLcMdl			m_vObj2D;			// 2D Object List
	lsLcMdl			m_vObj3D;			// 3D Object List
	lsLcMdl			m_vObj	;			// Object List

protected:
	vector<CLcmSptB*> m_vLcl	;
	FLOAT			m_fRangeMax	;		// Max View Range

	PDVD			m_pFVF	;
	LPD3DXEFFECT	m_pEft	;

public:
	CLcmSptD();
	virtual ~CLcmSptD();

	virtual	INT		Create(void* p1=0, void* p2=0, void* p3=0, void* p4=0);
	virtual	void	Destroy();

	virtual	INT		FrameMove();
	virtual	void	Render();

	virtual INT		Query(char* sCmd, void* pData);
	virtual	INT		GetHeight(float* vcOut, const float* vcIn);

public:
	INT				FileLoad(char* sFile);
	INT				FileSave(char* sFile);
	void			CreateField(CLcmSptD::_Inf*, CLcmSptD::_Env*, CLcmSptB::lsMpTex*);			// Create Field

	INT				Update();
	void			AddObj2D(ILcMdl* pMdB);
	void			AddObj3D(ILcMdl* pMdB);
	void			RenderPck();

	CLcmSptD::_Inf*	GetFldInf();
	char*			GetFldName();

	void			EraseObj();
	void*			GetPckObj();

	PDVD			GetVertexDeclarator();
	LPD3DXEFFECT	GetD3DXEffect();
	ILcCam*		GetCam();

protected:
	void		Clear();
	INT			PickField();
	INT			PickPosition(CLcmSptB**			pBlock
							, CLcmSptD::lsTlPkT*	pPickInf	// Picking Triangle
							, VEC3*				pvcOut		// Picking position
							, const VEC3*		pvcRayPos	// Ray Begin Position
							, const VEC3*		pvcRayDir	// Ray Direction
						   );

	INT			PickLocal();
	INT			PickObj();

public:
	FLOAT		m_fCull			;
	BOOL		m_bCtrlCull		;
	BOOL		m_bCtrlPck		;
	BOOL		m_bCtrlHeight	;
	BOOL		m_bCtrlSpoid	;
	DWORD		m_dFillColor	;
	BOOL		m_bCtrlBrush	;
	BOOL		m_bCtrlModel	;
	BOOL		m_bCtrlTile		;
	INT			m_nCtrlTile		;
	BOOL		m_bCtrlBndBox	;
	BOOL		m_bCtrlLclLine	;

	CLcmSptD::MpHgt m_Hgt		;
	VEC3		m_vcRayDir	;
	VEC3		m_vcCamPos	;
	ILcMdl*		m_pMdB		;
	ILcCam*	m_pCam		;
	ILcTbl*	m_pTbTex	;
	ILcTbl*	m_pTbMdl	;

protected:
	void	SetupHeight();
	void	SetupSpoid();
	void	SetupBrush();
	void	SetupModel();
	void	SetupTile();
};

#endif

