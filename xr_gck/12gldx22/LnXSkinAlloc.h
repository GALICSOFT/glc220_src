// LcXSkinAlloc.h: interface for the CLcXSkinAlloc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _LcXSkinAlloc_H_
#define _LcXSkinAlloc_H_

// Desc: Structure derived from D3DXFRAME so we can add some app-specific
//       info that will be stored with each frame

struct D3DXFRAME_DERIVED: public D3DXFRAME
{
	D3DXMATRIX        CombinedTransformationMatrix;
};



// Name: struct D3DXMESHCONTAINER_DERIVED
// Desc: Structure derived from D3DXMESHCONTAINER so we can add some app-specific
//       info that will be stored with each mesh

struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
{
	TexLayer*			 pArrTex;			// array of textures, entries are NULL if no texture specified
	LPD3DXMESH           pOrigMesh;			// SkinMesh info

	D3DXMATRIX*          pBoneOffsetMatrices;
	D3DXMATRIX**         ppBoneMatrixPtrs;

	DWORD                NumPaletteEntries;
	DWORD                NumAttributeGroups;
	DWORD                NumInfl;
	LPD3DXBUFFER         pBoneCombinationBuf;

	LPD3DXATTRIBUTERANGE pAttributeTable;
	bool                 UseSoftwareVP;
	DWORD                iAttributeSW;     // used to denote the split between SW and HW if necessary for non-indexed skinning
};




struct CLcXSkinAlloc: public ID3DXAllocateHierarchy
{
public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);

	STDMETHOD(CreateMeshContainer)(THIS_  LPCSTR Name,
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,
		CONST D3DXEFFECTINSTANCE *pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD *pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER *ppNewMeshContainer);

	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);

	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);

public:
	CLcXSkinAlloc(void *pApp) :m_pApp(pApp) {}
	HRESULT AllocateName( LPCSTR Name, LPSTR *pNewName );
public:
	void* m_pApp;
};



#endif

