// Implementation of the CLcXSkinSrc class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"

#include "LnMdl.h"
#include "LnXSkin.h"

#include "LnXSkinAlloc.h"
#include "LnXSkinSrc.h"
#include "LnXSkinIns.h"


#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }



D3DCAPS9			CLcXSkinSrc::m_d3dCaps;




CLcXSkinSrc::CLcXSkinSrc()
{
	m_pAnimController	= NULL;
	m_pFrameRoot		= NULL;

	m_SkinningMethod	= SKINNING_INDEXEDHLSL;
	m_pBoneMatrices		= NULL;
	m_NumBoneMatricesMax= 0;

	m_pEffect			= NULL;
}



CLcXSkinSrc::~CLcXSkinSrc()
{
	Destroy();
}


void CLcXSkinSrc::Destroy()
{
	if(m_pFrameRoot)
	{
		CLcXSkinAlloc Alloc(this);
		D3DXFrameDestroy(m_pFrameRoot, &Alloc);
	}

	SAFE_RELEASE(m_pAnimController);

	SAFE_DELETE_ARRAY(	m_pBoneMatrices	);
	m_NumBoneMatricesMax= 0;



	INT iSize = (INT)m_vTex.size();								// 텍스처의 갯수를 반환

	for(int i=0; i<iSize; ++i)
	{
		CLcXSkinSrc::_Tex*	pTex = m_vTex[i];
		TexLayer*			pLtx = pTex->pLtx;

		delete	pTex;
	}

	m_vTex.clear();

	m_pEffect = NULL;
}



INT CLcXSkinSrc::Query(char* sCmd, void* pData)
{
	if(0==_stricmp("Get Radius", sCmd))
	{
		FLOAT fR = GetObjRadius();
		*((FLOAT*)pData) = fR;
		return 0;
	}
	else if(0==_stricmp("Get Texture Number", sCmd))
	{
		INT	dVal = GetTextureNum();
		*((INT*)pData) = dVal;
		return 0;
	}

	else if(0==_stricmp("Get Texture Name", sCmd))
	{
		//		INT	nIdx = *((INT*)pData);

		INT	nIdx = -1;
		memcpy(&nIdx, pData, sizeof(INT));
		TCHAR*	srcTexture = (TCHAR*)GetTextureName(nIdx);
		_tcscpy( (TCHAR*)pData, srcTexture);
		return 0;
	}

	else if(0==_stricmp("Set Texture Pointer", sCmd))
	{
		struct _T
		{
			INT	  n;
			void* p0;	void* p1;	void* p2;	void* p3;
			void* p4;	void* p5;	void* p6;	void* p7;
		};

		_T*	t = (_T*)pData;

		INT			nIdx = t->n;
		TexLayer	pLtx;

		pLtx.p0 = (LPDIRECT3DTEXTURE9)t->p0;
		pLtx.p1 = (LPDIRECT3DTEXTURE9)t->p1;
		pLtx.p2 = (LPDIRECT3DTEXTURE9)t->p2;
		pLtx.p3 = (LPDIRECT3DTEXTURE9)t->p3;
		pLtx.p4 = (LPDIRECT3DTEXTURE9)t->p4;
		pLtx.p5 = (LPDIRECT3DTEXTURE9)t->p5;
		pLtx.p6 = (LPDIRECT3DTEXTURE9)t->p6;
		pLtx.p7 = (LPDIRECT3DTEXTURE9)t->p7;

		SetTexturePointer(nIdx, &pLtx);
		return 0;
	}

	else if(0==_stricmp("Set Vertex Shader", sCmd))
	{
		struct _T	{	INT n;	void* p;	};
		_T*	t = (_T*)pData;

		INT						nIdx = t->n;
		LPDIRECT3DVERTEXSHADER9 pshd = (LPDIRECT3DVERTEXSHADER9)t->p;

		SetVtxShader(nIdx, pshd);
		return 0;
	}

	else if(0==_stricmp("Set Effect", sCmd))
	{
		m_pEffect	= (LPD3DXEFFECT)pData;

		if(m_pEffect)
			m_SkinningMethod = SKINNING_INDEXEDHLSL;

		else
			m_SkinningMethod = SKINNING_NONINDEXED;

		return 0;
	}



	else if(0==_stricmp("Get Skinning Method", sCmd))
	{
		*((INT*)pData) = this->m_SkinningMethod;
		return 0;
	}

	else if(0==_stricmp("New Skinng Method", sCmd))
	{
		INT nMethod = *((INT*)pData);

		NewSkinMethod(nMethod);
		return 0;
	}

	return CLcXSkin::Query(sCmd, pData);
}




INT CLcXSkinSrc::Create(void* p1, void* p2, void* p3, void* p4)
{
	HRESULT hr= -1;
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();


	pDev->GetDeviceCaps(&CLcXSkinSrc::m_d3dCaps);

	char* sFile	= (char*)p1;
	m_pEffect	= (LPD3DXEFFECT)p2;

	if(!m_pEffect)
		m_SkinningMethod = SKINNING_NONINDEXED;

	CLcXSkinAlloc Alloc(this);


	hr = D3DXLoadMeshHierarchyFromX(sFile, D3DXMESH_MANAGED, pDev, &Alloc, NULL, &m_pFrameRoot, &m_pAnimController);
	if (FAILED(hr))
		return hr;

	hr = SetupBoneMatrixPointers(m_pFrameRoot);
	if (FAILED(hr))
		return hr;

	hr = D3DXFrameCalculateBoundingSphere(m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius);
	if (FAILED(hr))
		return hr;

	return S_OK;
}




INT CLcXSkinSrc::FrameMove()
{
	return S_OK;
}





void CLcXSkinSrc::Render()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	if (m_pAnimController)
		m_pAnimController->AdvanceTime(m_fElapse, NULL);

	UpdateFrameMatrices(m_pFrameRoot, &m_mtWld);

	DrawFrame(m_pFrameRoot);


	static	D3DXMATRIX	mtI(1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1);
	pDev->SetTransform(D3DTS_WORLD, &mtI);
}




void CLcXSkinSrc::NewSkinMethod(INT nMethod)
{
	if (m_SkinningMethod == nMethod)
		return;

	m_SkinningMethod = nMethod;
	UpdateSkinningMethod(m_pFrameRoot);
}




void CLcXSkinSrc::PushTexture(TCHAR*	sTxFile, TexLayer* pLtx)
{
	CLcXSkinSrc::_Tex*	pTxS = new CLcXSkinSrc::_Tex;
	_tcscpy(pTxS->sTxFile, sTxFile);						// 텍스처 이름을 복사하고

	pTxS->pLtx	= (TexLayer*)pLtx;							// 텍스처 번지를 복사한다.

	m_vTex.push_back(pTxS);
}


INT CLcXSkinSrc::GetTextureNum()
{
	return (INT)m_vTex.size();								// 텍스처의 갯수를 반환
}

const TCHAR* CLcXSkinSrc::GetTextureName(INT nIdx)			// 텍스처의 이름을 전달
{
	return m_vTex[nIdx]->sTxFile;
}

void CLcXSkinSrc::SetTexturePointer(INT nIdx, TexLayer* pLtx)
{
	CLcXSkinSrc::_Tex*	pTxS = m_vTex[nIdx];

	for(int i=0; i<TexLayer::MAX_TEXTURE_LAYER; ++i)
		pTxS->pLtx->p[i]	= pLtx->p[i];						// 텍스처 포인터를 설정.
}




void* CLcXSkinSrc::CreateNewInstance()
{
	CLcXSkinIns* pAniInst = NULL;
	HRESULT hr=-1;

	if(!m_pAnimController)
		return NULL;

	pAniInst = new CLcXSkinIns;
	hr = pAniInst->Create(this, m_pAnimController, m_pFrameRoot);

	if( FAILED( hr ) )
	{
		delete pAniInst;
		return NULL;
	}

	return pAniInst;
}







// Called either by CreateMeshContainer when loading a skin mesh, or when
// changing methods.  This function uses the pSkinInfo of the mesh
// container to generate the desired drawable mesh and bone combination
// table.

HRESULT CLcXSkinSrc::GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer)
{
	HRESULT hr = S_OK;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	if (pMeshContainer->pSkinInfo == NULL)
		return hr;

	SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
	SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );

	// if non-indexed skinning mode selected, use ConvertToBlendedMesh to generate drawable mesh
	if (m_SkinningMethod == SKINNING_NONINDEXED)
	{

		hr = pMeshContainer->pSkinInfo->ConvertToBlendedMesh
			(
			pMeshContainer->pOrigMesh,
			D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE,
			pMeshContainer->pAdjacency,
			NULL, NULL, NULL,
			&pMeshContainer->NumInfl,
			&pMeshContainer->NumAttributeGroups,
			&pMeshContainer->pBoneCombinationBuf,
			&pMeshContainer->MeshData.pMesh
			);
		if (FAILED(hr))
			goto e_Exit;


			/* If the device can only do 2 matrix blends, ConvertToBlendedMesh cannot approximate all meshes to it
			Thus we split the mesh in two parts: The part that uses at most 2 matrices and the rest. The first is
		drawn using the device's HW vertex processing and the rest is drawn using SW vertex processing. */
		LPD3DXBONECOMBINATION rgBoneCombinations  = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

		// look for any set of bone combinations that do not fit the caps
		for (pMeshContainer->iAttributeSW = 0; pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups; pMeshContainer->iAttributeSW++)
		{
			DWORD cInfl   = 0;

			for (DWORD iInfl = 0; iInfl < pMeshContainer->NumInfl; iInfl++)
			{
				if (rgBoneCombinations[pMeshContainer->iAttributeSW].BoneId[iInfl] != UINT_MAX)
				{
					++cInfl;
				}
			}

			if (cInfl > m_d3dCaps.MaxVertexBlendMatrices)
			{
				break;
			}
		}

		// if there is both HW and SW, add the Software Processing flag
		if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
		{
			LPD3DXMESH pMeshTmp;

			hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING|pMeshContainer->MeshData.pMesh->GetOptions(),
				pMeshContainer->MeshData.pMesh->GetFVF(),
				pDev, &pMeshTmp);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			pMeshContainer->MeshData.pMesh->Release();
			pMeshContainer->MeshData.pMesh = pMeshTmp;
			pMeshTmp = NULL;
		}
	}
	// if indexed skinning mode selected, use ConvertToIndexedsBlendedMesh to generate drawable mesh
	else if (m_SkinningMethod == SKINNING_INDEXED)
	{
		DWORD NumMaxFaceInfl;
		DWORD Flags = D3DXMESHOPT_VERTEXCACHE;

		LPDIRECT3DINDEXBUFFER9 pIB;
		hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB);
		if (FAILED(hr))
			goto e_Exit;

		hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB, pMeshContainer->pOrigMesh->GetNumFaces(), &NumMaxFaceInfl);
		pIB->Release();
		if (FAILED(hr))
			goto e_Exit;

		// 12 entry palette guarantees that any triangle (4 independent influences per vertex of a tri)
		// can be handled
		NumMaxFaceInfl = min(NumMaxFaceInfl, 12);

		if (m_d3dCaps.MaxVertexBlendMatrixIndex + 1 < NumMaxFaceInfl)
		{
			// HW does not support indexed vertex blending. Use SW instead
			pMeshContainer->NumPaletteEntries = min(256, pMeshContainer->pSkinInfo->GetNumBones());
			pMeshContainer->UseSoftwareVP = true;
			Flags |= D3DXMESH_SYSTEMMEM;
		}
		else
		{
			// using hardware - determine palette size from caps and number of bones
			// If normals are present in the vertex data that needs to be blended for lighting, then
			// the number of matrices is half the number specified by MaxVertexBlendMatrixIndex.
			pMeshContainer->NumPaletteEntries = min( ( m_d3dCaps.MaxVertexBlendMatrixIndex + 1 ) / 2,
				pMeshContainer->pSkinInfo->GetNumBones() );
			pMeshContainer->UseSoftwareVP = false;
			Flags |= D3DXMESH_MANAGED;
		}

		hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
			(
			pMeshContainer->pOrigMesh,
			Flags,
			pMeshContainer->NumPaletteEntries,
			pMeshContainer->pAdjacency,
			NULL, NULL, NULL,
			&pMeshContainer->NumInfl,
			&pMeshContainer->NumAttributeGroups,
			&pMeshContainer->pBoneCombinationBuf,
			&pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;
	}
	// if vertex shader indexed skinning mode selected, use ConvertToIndexedsBlendedMesh to generate drawable mesh
	else if ((m_SkinningMethod == SKINNING_INDEXEDVS) || (m_SkinningMethod == SKINNING_INDEXEDHLSL))
	{
		// Get palette size
		// First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
		// (96 - 9) /3 i.e. Maximum constant count - used constants
		UINT MaxMatrices = 26;
		pMeshContainer->NumPaletteEntries = min(MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones());

		DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
		if (m_d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
		{
			pMeshContainer->UseSoftwareVP = false;
			Flags |= D3DXMESH_MANAGED;
		}
		else
		{
			pMeshContainer->UseSoftwareVP = true;
			Flags |= D3DXMESH_SYSTEMMEM;
		}

		SAFE_RELEASE(pMeshContainer->MeshData.pMesh);

		hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
			(
			pMeshContainer->pOrigMesh,
			Flags,
			pMeshContainer->NumPaletteEntries,
			pMeshContainer->pAdjacency,
			NULL, NULL, NULL,
			&pMeshContainer->NumInfl,
			&pMeshContainer->NumAttributeGroups,
			&pMeshContainer->pBoneCombinationBuf,
			&pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;


		// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
		DWORD NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
		if (NewFVF != pMeshContainer->MeshData.pMesh->GetFVF())
		{
			LPD3DXMESH pMesh;
			hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF, pDev, &pMesh);
			if (!FAILED(hr))
			{
				pMeshContainer->MeshData.pMesh->Release();
				pMeshContainer->MeshData.pMesh = pMesh;
				pMesh = NULL;
			}
		}

		D3DVERTEXELEMENT9 pDcl[MAX_FVF_DECL_SIZE]={0};
		LPD3DVERTEXELEMENT9 pDeclCur;
		hr = pMeshContainer->MeshData.pMesh->GetDeclaration(pDcl);
		if (FAILED(hr))
			goto e_Exit;

		// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type
		//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
		//    this is more of a "cast" operation
		pDeclCur = pDcl;
		while (pDeclCur->Stream != 0xff)
		{
			if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
				pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
			pDeclCur++;
		}

		hr = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDcl);
		if (FAILED(hr))
			goto e_Exit;

		// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
		if (m_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones())
		{
			m_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

			// Allocate space for blend matrices
			if(m_pBoneMatrices)
				delete []m_pBoneMatrices;

			m_pBoneMatrices  = new D3DXMATRIX[m_NumBoneMatricesMax];

			if (m_pBoneMatrices == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}
		}

	}
	// if software skinning selected, use GenerateSkinnedMesh to create a mesh that can be used with UpdateSkinnedMesh
	else if (m_SkinningMethod == SKINNING_SOFTWARE)
	{
		hr = pMeshContainer->pOrigMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMeshContainer->pOrigMesh->GetFVF(),
			pDev, &pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;

		hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(NULL, &pMeshContainer->NumAttributeGroups);
		if (FAILED(hr))
			goto e_Exit;

		delete[] pMeshContainer->pAttributeTable;
		pMeshContainer->pAttributeTable  = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
		if (pMeshContainer->pAttributeTable == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(pMeshContainer->pAttributeTable, NULL);
		if (FAILED(hr))
			goto e_Exit;

		// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
		if (m_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones())
		{
			m_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

			// Allocate space for blend matrices
			if(m_pBoneMatrices)
				delete []m_pBoneMatrices;

			m_pBoneMatrices  = new D3DXMATRIX[m_NumBoneMatricesMax];

			if (m_pBoneMatrices == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}
		}
	}
	else  // invalid m_SkinningMethod value
	{
		// return failure due to invalid skinning method value
		hr = E_INVALIDARG;
		goto e_Exit;
	}

e_Exit:
	return hr;
}




// Called to render a mesh in the hierarchy

void CLcXSkinSrc::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();


	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	UINT iMaterial;
	UINT NumBlend;
	UINT iAttrib;
	DWORD AttribIdPrev;
	LPD3DXBONECOMBINATION pBoneComb;

	UINT iMatrixIndex;
	UINT iPaletteEntry;
	D3DXMATRIX matTemp;

	D3DXMATRIX	mtView;

	pDev->GetTransform( D3DTS_VIEW, &mtView );

	// first check for skinning
	/// standard mesh, just draw it after setting material properties
	if(!pMeshContainer->pSkinInfo)
	{
		pDev->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);

		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			pDev->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D );

			TexLayer*			pLayer = &pMeshContainer->pArrTex[iMaterial];
			LPDIRECT3DTEXTURE9	p0= pLayer->p0;
			pDev->SetTexture( 0, p0);

			pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial);
		}

		return;
	}


	if(m_SkinningMethod == SKINNING_NONINDEXED)
	{
		AttribIdPrev = UNUSED32;
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

		// Draw using default vtx processing of the device (typically HW)
		for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			NumBlend = 0;
			for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
			{
				if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
				{
					NumBlend = i;
				}
			}

			if (m_d3dCaps.MaxVertexBlendMatrices >= NumBlend + 1)
			{
				// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
				for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
					if (iMatrixIndex != UINT_MAX)
					{
						D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
						pDev->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
					}
				}

				pDev->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

				// lookup the material used for this subset of faces
				if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
				{
					pDev->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D );

					TexLayer*	pLayer = &pMeshContainer->pArrTex[pBoneComb[iAttrib].AttribId];
					LPDIRECT3DTEXTURE9	p0= pLayer->p0;

					pDev->SetTexture( 0,  p0);

					AttribIdPrev = pBoneComb[iAttrib].AttribId;
				}

				// draw the subset now that the correct material and matrices are loaded
				pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
			}
		}

		// If necessary, draw parts that HW could not handle using SW
		if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
		{
			AttribIdPrev = UNUSED32;
			pDev->SetSoftwareVertexProcessing(TRUE);
			for (iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
			{
				NumBlend = 0;
				for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
				{
					if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
					{
						NumBlend = i;
					}
				}

				if (m_d3dCaps.MaxVertexBlendMatrices < NumBlend + 1)
				{
					// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
					for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
					{
						iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
						if (iMatrixIndex != UINT_MAX)
						{
							D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
							pDev->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
						}
					}

					pDev->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

					// lookup the material used for this subset of faces
					if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
					{
						pDev->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D );

						TexLayer*			pLayer = &pMeshContainer->pArrTex[pBoneComb[iAttrib].AttribId];
						LPDIRECT3DTEXTURE9	p0= pLayer->p0;

						pDev->SetTexture( 0, p0 );

						AttribIdPrev = pBoneComb[iAttrib].AttribId;
					}

					// draw the subset now that the correct material and matrices are loaded
					pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
				}
			}
			pDev->SetSoftwareVertexProcessing(FALSE);
		}

		pDev->SetRenderState(D3DRS_VERTEXBLEND, 0);
	}
	else if (m_SkinningMethod == SKINNING_INDEXED)
	{
		// if hw doesn't support indexed vertex processing, switch to software vertex processing
		if (pMeshContainer->UseSoftwareVP)
		{
			pDev->SetSoftwareVertexProcessing(TRUE);
		}

		// set the number of vertex blend indices to be blended
		if (pMeshContainer->NumInfl == 1)
			pDev->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);
		else
			pDev->SetRenderState(D3DRS_VERTEXBLEND, pMeshContainer->NumInfl - 1);

		if (pMeshContainer->NumInfl)
			pDev->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);

		// for each attribute group in the mesh, calculate the set of matrices in the palette and then draw the mesh subset
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			// first calculate all the world matrices
			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
					pDev->SetTransform( D3DTS_WORLDMATRIX( iPaletteEntry ), &matTemp );
				}
			}

			// setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
			pDev->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D );

			TexLayer*			pLayer = &pMeshContainer->pArrTex[pBoneComb[iAttrib].AttribId];
			LPDIRECT3DTEXTURE9	p0= pLayer->p0;

			pDev->SetTexture( 0,  p0);

			// finally draw the subset with the current world matrix palette and material state
			pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );
		}

		// reset blending state
		pDev->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		pDev->SetRenderState(D3DRS_VERTEXBLEND, 0);

		// remember to reset back to hw vertex processing if software was required
		if (pMeshContainer->UseSoftwareVP)
		{
			pDev->SetSoftwareVertexProcessing(FALSE);
		}
	}
	
	else if (m_SkinningMethod == SKINNING_SOFTWARE)
	{
		D3DXMATRIX  Identity;
		DWORD       cBones  = pMeshContainer->pSkinInfo->GetNumBones();
		DWORD       iBone;
		PBYTE       pbVerticesSrc;
		PBYTE       pbVerticesDest;

		// set up bone transforms
		for (iBone = 0; iBone < cBones; ++iBone)
		{
			D3DXMatrixMultiply
				(
				&m_pBoneMatrices[iBone],                 // output
				&pMeshContainer->pBoneOffsetMatrices[iBone],
				pMeshContainer->ppBoneMatrixPtrs[iBone]
				);
		}

		// set world transform
		D3DXMatrixIdentity(&Identity);
		pDev->SetTransform(D3DTS_WORLD, &Identity);

		pMeshContainer->pOrigMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbVerticesSrc);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, (LPVOID*)&pbVerticesDest);

		// generate skinned mesh
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(m_pBoneMatrices, NULL, pbVerticesSrc, pbVerticesDest);

		pMeshContainer->pOrigMesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();

		for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			pDev->SetMaterial(&(pMeshContainer->pMaterials[pMeshContainer->pAttributeTable[iAttrib].AttribId].MatD3D));

			TexLayer*			pLayer = &pMeshContainer->pArrTex[pMeshContainer->pAttributeTable[iAttrib].AttribId];
			LPDIRECT3DTEXTURE9	p0= pLayer->p0;

			pDev->SetTexture(0, p0);

			pMeshContainer->MeshData.pMesh->DrawSubset(pMeshContainer->pAttributeTable[iAttrib].AttribId);
		}
	}

	else if (m_SkinningMethod == SKINNING_INDEXEDVS)
	{
		// Use COLOR instead of UBYTE4 since Geforce3 does not support it
		// vConst.w should be 3, but due to COLOR/UBYTE4 issue, mul by 255 and add epsilon
		D3DXVECTOR4 vConst( 1.0f, 0.0f, 0.0f, 765.01f );

		if (pMeshContainer->UseSoftwareVP)
		{
			pDev->SetSoftwareVertexProcessing(TRUE);
		}

		pDev->SetVertexShader(m_pIndexedVertexShader[pMeshContainer->NumInfl - 1]);

		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			// first calculate all the world matrices
			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
					D3DXMatrixMultiplyTranspose(&matTemp, &matTemp, &mtView);
					pDev->SetVertexShaderConstantF(iPaletteEntry*3 + 9, (float*)&matTemp, 3);
				}
			}

			// Sum of all ambient and emissive contribution
			D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
			D3DXCOLOR color2(.25, .25, .25, 1.0);
			D3DXCOLOR ambEmm;
			D3DXColorModulate(&ambEmm, &color1, &color2);
			ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

			// set material color properties
			pDev->SetVertexShaderConstantF(8, (float*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse), 1);
			pDev->SetVertexShaderConstantF(7, (float*)&ambEmm, 1);
			vConst.y = pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Power;
			pDev->SetVertexShaderConstantF(0, (float*)&vConst, 1);

			TexLayer*			pLayer = &pMeshContainer->pArrTex[pBoneComb[iAttrib].AttribId];
			LPDIRECT3DTEXTURE9	p0= pLayer->p0;

			pDev->SetTexture(0, p0);

			// finally draw the subset with the current world matrix palette and material state
			pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );

		}

		// remember to reset back to hw vertex processing if software was required
		if (pMeshContainer->UseSoftwareVP)
		{
			pDev->SetSoftwareVertexProcessing(FALSE);
		}

		pDev->SetVertexShader(NULL);
	}

	else if (m_SkinningMethod == SKINNING_INDEXEDHLSL)
	{
		if (pMeshContainer->UseSoftwareVP)
		{
			pDev->SetSoftwareVertexProcessing(TRUE);
		}

		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			// first calculate all the world matrices
			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
					D3DXMatrixMultiply(&m_pBoneMatrices[iPaletteEntry], &matTemp, &mtView);
				}
			}
			m_pEffect->SetMatrixArray( "mWorldMatrixArray", m_pBoneMatrices, pMeshContainer->NumPaletteEntries);

			// Sum of all ambient and emissive contribution
			D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
			D3DXCOLOR color2(.25, .25, .25, 1.0);
			D3DXCOLOR ambEmm;
			D3DXColorModulate(&ambEmm, &color1, &color2);
			ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

			// set material color properties
			m_pEffect->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse));
			m_pEffect->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);

			// Set CurNumBones to select the correct vertex shader for the number of bones
			m_pEffect->SetInt( "CurNumBones", pMeshContainer->NumInfl -1);




			
//			// Only Vertex Shader
//			// setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
			TexLayer*			pLayer = &pMeshContainer->pArrTex[pBoneComb[iAttrib].AttribId];
			LPDIRECT3DTEXTURE9	p0= pLayer->p0;

			pDev->SetTexture( 0, p0);
//
//
//			//(E) 2008-01-29 Editor: AFEW
//			//	TexLayer*	pLayer = pMeshContainer->pArrTex[pBoneComb[iAttrib].AttribId];
			LPDIRECT3DTEXTURE9	pTexDif	= pLayer->p0;
			LPDIRECT3DTEXTURE9	pTexNor	= pLayer->p1;
			LPDIRECT3DTEXTURE9	pTexSpc	= pLayer->p2;
			LPDIRECT3DTEXTURE9	pTexEms	= pLayer->p3;

			m_pEffect->SetTexture( "m_TxDif", pTexDif);		// 색상맵
			m_pEffect->SetTexture( "m_TxNor", pTexNor);		// 법선 맵
			m_pEffect->SetTexture( "m_TxSpc", pTexSpc);		// 스페큘러 맵
			m_pEffect->SetTexture( "m_TxEms", pTexEms);		// Emission 맵


			// Start the effect now all parameters have been updated
			UINT numPasses;
			m_pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );

			for( UINT iPass = 0; iPass < numPasses; iPass++ )
			{
				m_pEffect->Pass( iPass );
				// draw the subset with the current world matrix palette and material state
				pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );
			}

			m_pEffect->End();

			pDev->SetVertexShader(NULL);
			pDev->SetPixelShader(NULL);
		}

		// remember to reset back to hw vertex processing if software was required
		if (pMeshContainer->UseSoftwareVP)
		{
			pDev->SetSoftwareVertexProcessing(FALSE);
		}
	}

}




// Called to render a frame in the hierarchy

void CLcXSkinSrc::DrawFrame(LPD3DXFRAME pFrame)
{
	LPD3DXMESHCONTAINER pMeshContainer;

	pMeshContainer = pFrame->pMeshContainer;

	while (pMeshContainer)
	{
		DrawMeshContainer(pMeshContainer, pFrame);
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	if(pFrame->pFrameSibling)
		DrawFrame(pFrame->pFrameSibling);

	if (pFrame->pFrameFirstChild)
		DrawFrame(pFrame->pFrameFirstChild);
}



// Called to setup the pointers for a given bone to its transformation matrix

HRESULT CLcXSkinSrc::SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	UINT iBone, cBones;
	D3DXFRAME_DERIVED *pFrame;

	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	// if there is a skinmesh, then setup the bone matrices
	if (pMeshContainer->pSkinInfo)
	{
		cBones = pMeshContainer->pSkinInfo->GetNumBones();

		pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];
		if (pMeshContainer->ppBoneMatrixPtrs == NULL)
			return E_OUTOFMEMORY;

		for (iBone = 0; iBone < cBones; iBone++)
		{
			pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone));
			if (pFrame == NULL)
				return E_FAIL;

			pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
	}

	return S_OK;
}



// Called to setup the pointers for a given bone to its transformation matrix

HRESULT CLcXSkinSrc::SetupBoneMatrixPointers(LPD3DXFRAME pFrame)
{
	HRESULT hr;

	if (pFrame->pMeshContainer)
	{
		hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameSibling)
	{
		hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameFirstChild)
	{
		hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);
		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}




// update the frame matrices

void CLcXSkinSrc::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

	if (pParentMatrix)
		D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);
	else
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

	if (pFrame->pFrameSibling)
	{
		UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
	}

	if (pFrame->pFrameFirstChild)
	{
		UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
	}
}



// update the skinning method

void CLcXSkinSrc::UpdateSkinningMethod(LPD3DXFRAME pFrameBase)
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer;

	pMeshContainer = (D3DXMESHCONTAINER_DERIVED *)pFrame->pMeshContainer;

	while (pMeshContainer)
	{
		GenerateSkinnedMesh(pMeshContainer);

		pMeshContainer = (D3DXMESHCONTAINER_DERIVED *)pMeshContainer->pNextMeshContainer;
	}

	if (pFrame->pFrameSibling)
	{
		UpdateSkinningMethod(pFrame->pFrameSibling);
	}

	if (pFrame->pFrameFirstChild)
	{
		UpdateSkinningMethod(pFrame->pFrameFirstChild);
	}
}



