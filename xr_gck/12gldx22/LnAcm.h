// Interface for the ILcAcm class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcAcm_H_
#define _LcAcm_H_

#pragma warning(disable : 4530)
#pragma warning(disable : 4786)
#pragma warning(disable : 4996)

#include <vector>
#include <set>
#include <map>
#include <string>
using namespace	std;


class CLcAcm : public CLcMdl
{
public:
	struct TaniRot
	{
		INT		nF;						// Frame
		QUAT	vT;						// Rotation is Quatanion
	};

	struct TaniScl
	{
		INT		nF;						// Frame
		VEC3	vT;						// Rotation is Quatanion
	};

	struct TaniTrn
	{
		INT		nF;						// Frame
		VEC3	vT;						// Rotation is Quatanion
	};

	struct TboneW
	{
		INT		n;						// Bone Index
		FLOAT	w;						// Weight

		TboneW() : n(-1), w(0){}
		TboneW(INT nBone, FLOAT fW) : n(nBone), w(fW) {}
	};

	struct LcmBoneW
	{
		vector<TboneW>	vB;				// Bone Index
	};

	struct LcmHeader
	{
		char		sVer[16];			// Version
		INT			iNmtl;				// Merterial Num
		INT			iNgeo;				// Geometry Num

		INT			nFrmF;				// First Frame
		INT			nFrmL;				// Last Frame
		INT			nFrmS;				// Frame Rate(Speed: Frame/Sceond, FPS)
		INT			nFrmT;				// Tick Frame
		
		LcmHeader();
	};

	struct TaniIfl
	{
		INT			nF;					// Frame
		PDTX		pT;					// Texture
		TaniIfl();
		TaniIfl(INT, PDTX);
	};

	struct LcMtl								// 머티리얼 자료구조
	{
		INT				nIdx	;				// Material 인덱스
		DMTL			dMtl	;
		char			sDif[LCM_TEX_NAME];		// Diffuse Texture File
		char			sNor[LCM_TEX_NAME];		// Normal Texture File
		char			sSpc[LCM_TEX_NAME];		// Specular Texture File
		char			sEms[LCM_TEX_NAME];		// Emissive Texture File

		PDTX			pDif	;
		PDTX			pNor	;
		PDTX			pSpc	;
		PDTX			pEms	;

		vector<TaniIfl>* pvIfl	;

		LcMtl();
	};

	struct TaniTx
	{
		INT				nF	;					// Frame
		LcMtl*			pM	;					// Material Pointer
		PDTX			pT	;					// Texture Pointer
		TaniTx();
		TaniTx(INT, LcMtl*);
	};

	struct LcFceR
	{
		int				nBgn	;				// Start of Face
		int				nNum	;				// Number of Face
		int				nMtl	;				// Index of Face
		
		LcFceR() : nBgn(0), nNum(0), nMtl(0){}
		LcFceR(INT _B, INT _N, INT _M) : nBgn(_B), nNum(_N), nMtl(_M){}
	};
	
	struct LcGeo								// Geometry 자료구조
	{
		char			sName[32]	;			// Node Name
		char			sPrnt[32]	;			// Parent Name
		INT				nIdx	;				// Index;
		INT				nPrn	;				// Parent Index
		INT				nMtl	;				// Material index
		INT				nMtlS	;				// Material = 0, Face = 1

		MATA			mtLcl	;				// Local 행렬
		MATA			mtWld	;				// World 행렬
		MATA			mtOrn	;				// Orient Direction World Matrix
		MATA			mtOrI	;				// Orient Direction World Matrix Inverse
		MATA			mtPvt	;				// Pivot Matrix

		DWORD			dFVF	;				// FVF
		INT				iNfce	;				// Face Number
		INT				iNvtx	;				// Vertex Number

		VtxIdx*			pFce	;
		VtxD*			pVxD	;				// Diffuse
		VtxNDUV1i*		pVxA	;				// Rendering with HLSL

		INT				nBsrc	;				// Bone에 영향받는 버텍스 수
		LcmBoneW*		pBdst	;				// Dest

		vector<QUAT>*	vAniRot	;
		vector<VEC3>*	vAniTrn	;
		vector<LcFceR>*	vFceRnd	;

		BOOL			nBlend	;				// Animation Type
		
		LcGeo();
	};

protected:
	char				m_sName[LCM_FILE_NAME];

	CLcAcm::LcmHeader	m_pLcHead	;
	CLcAcm::LcMtl*		m_pLcMtl	;
	CLcAcm::LcGeo*		m_pLcGeo	;
	vector<_Tframe>*	m_pFrame	;
	map<string,PDTX>	m_mpTxOrg	;

	MATA*				m_mtPrn		;			// Parent World Matrix
	MATA*				m_pmtWld	;
	
	DOUBLE				m_dFrmCur	;			// Current Frame
	DOUBLE				m_dTimeCur	;			// Current Time
	INT					m_nActM		;			// Action Master Index Current


	DWORD				m_dIflTime	;			// Time Start
	DWORD				m_dIflIntv	;			// Time Interval
	vector<TaniTx>		m_vIfl		;

	PDEF				m_pEffct	;			// 이펙트

public:
	CLcAcm();
	virtual ~CLcAcm();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		FrameMove();
	virtual void	Render();

	virtual INT		Query(char* sCmd, void* pVal);
	virtual INT		GetType()				{	return LCM_ACM;	}
	virtual const char*	const GetName()const{	return m_sName;	}
	virtual ILcMdl* GetOriginal()			{	return this;	}

	virtual INT		SetAttrib(char* sCmd, void* pVal);
	virtual INT		GetAttrib(char* sCmd, void* pVal);

public:
	CLcAcm::LcmHeader*	GetHeader()		{	return &m_pLcHead;	}
	CLcAcm::LcMtl*		GetMtrl()		{	return m_pLcMtl	;	}
	CLcAcm::LcGeo*		GetGeometry()	{	return m_pLcGeo	;	}
	vector<_Tframe>*	GetFrame()		{	return m_pFrame;	}
	map<string, PDTX>*	GetTextures()	{	return &m_mpTxOrg;	}
	PDEF				GetEffect()		{	return m_pEffct	;	}

	void	RederingHlsl(D3DXMATRIX* pmtRot, D3DXMATRIX* pmtWldBase, D3DXMATRIX* pmtWldLst, D3DXMATRIX* pmtPrnt);
	void	RederingBone(D3DXMATRIX* pmtWldBase, D3DXMATRIX* pmtWldLst, D3DXMATRIX* pmtPrnt);

protected:
	INT		LoadFromFile(char* sPath);
	INT		LoadTexture(char* sPath);
	
	INT		TextureLoad(char* sFile, LPDIRECT3DTEXTURE9*, DWORD dFault=0xFFFFFFFF);
	INT		TextureLoadIfl(LcMtl* pMtl, char* sDriv, char* sDirc, char* sName);

protected:
	INT		m_bTex;						// for Viewer: App Texture
	INT		m_bBone;					// for Viewer: Show Bone
	INT		m_bModel;					// for Viewer: Show Model
	INT		m_bAnima;					// for Viewer: Animation
};


#endif

