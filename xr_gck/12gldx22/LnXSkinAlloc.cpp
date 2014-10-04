// Implementation of the CLcXSkinAlloc class.
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

#include "LnxEntity.h"

#include "LnMdl.h"
#include "LnXSkin.h"

#include "LnXSkinAlloc.h"
#include "LnXSkinSrc.h"


#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }



// Allocates memory for a string to hold the name of a frame or mesh

HRESULT CLcXSkinAlloc::AllocateName( LPCSTR Name, LPSTR *pNewName )
{
	UINT cbLength;

	if (Name != NULL)
	{
		cbLength = (UINT)strlen(Name) + 1;
		*pNewName = new CHAR[cbLength];
		if (*pNewName == NULL)
			return E_OUTOFMEMORY;
		memcpy(*pNewName, Name, cbLength*sizeof(CHAR));
	}
	else
	{
		*pNewName = NULL;
	}

	return S_OK;
}




HRESULT CLcXSkinAlloc::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
	HRESULT hr = S_OK;
	D3DXFRAME_DERIVED *pFrame;

	*ppNewFrame = NULL;

	pFrame = new D3DXFRAME_DERIVED;
	if (pFrame == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	hr = AllocateName(Name, &pFrame->Name);
	if (FAILED(hr))
		goto e_Exit;

	// initialize other data members of the frame
	D3DXMatrixIdentity(&pFrame->TransformationMatrix);
	D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

	pFrame->pMeshContainer = NULL;
	pFrame->pFrameSibling = NULL;
	pFrame->pFrameFirstChild = NULL;

	*ppNewFrame = pFrame;
	pFrame = NULL;
e_Exit:
	delete pFrame;
	return hr;
}



HRESULT CLcXSkinAlloc::CreateMeshContainer(	LPCSTR Name,
											CONST D3DXMESHDATA *pMeshData,
											CONST D3DXMATERIAL *pMaterials,
											CONST D3DXEFFECTINSTANCE *pEffectInstances,
											DWORD NumMaterials,
											CONST DWORD *pAdjacency,
											LPD3DXSKININFO pSkinInfo,
											LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
	HRESULT hr;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
	UINT NumFaces;
	UINT iMaterial;
	UINT iBone, cBones;
	LPDIRECT3DDEVICE9 pd3dDevice = NULL;

	LPD3DXMESH pMesh = NULL;

	*ppNewMeshContainer = NULL;

	// this sample does not handle patch meshes, so fail when one is found
	if (pMeshData->Type != D3DXMESHTYPE_MESH)
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// get the pMesh interface pointer out of the mesh data structure
	pMesh = pMeshData->pMesh;

	// this sample does not FVF compatible meshes, so fail when one is found
	if (pMesh->GetFVF() == 0)
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// allocate the overloaded structure to return as a D3DXMESHCONTAINER
	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	if (pMeshContainer == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
	memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));

	// make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
	hr = AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
		goto e_Exit;

	pMesh->GetDevice(&pd3dDevice);
	NumFaces = pMesh->GetNumFaces();

	// if no normals are in the mesh, add them
	if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
	{
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		// clone the mesh to make room for the normals
		hr = pMesh->CloneMeshFVF( pMesh->GetOptions(),
			pMesh->GetFVF() | D3DFVF_NORMAL,
			pd3dDevice, &pMeshContainer->MeshData.pMesh );
		if (FAILED(hr))
			goto e_Exit;

		// get the new pMesh pointer back out of the mesh container to use
		// NOTE: we do not release pMesh because we do not have a reference to it yet
		pMesh = pMeshContainer->MeshData.pMesh;

		// now generate the normals for the pmesh
		D3DXComputeNormals( pMesh, NULL );
	}
	else  // if no normals, just add a reference to the mesh for the mesh container
	{
		pMeshContainer->MeshData.pMesh = pMesh;
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		pMesh->AddRef();
	}

	// allocate memory to contain the material information.  This sample uses
	//   the D3D9 materials and texture names instead of the EffectInstance style materials
	pMeshContainer->NumMaterials = max(1, NumMaterials);
	pMeshContainer->pMaterials	= new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->pArrTex		= new TexLayer	[pMeshContainer->NumMaterials];
	pMeshContainer->pAdjacency	= new DWORD		[NumFaces*3];

	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);
	
//	memset(pMeshContainer->pArrTex, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);

	// if materials provided, copy them
	if (NumMaterials > 0)
	{
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);

		for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++)
		{
			if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
			{
				TCHAR strTexturePath[MAX_PATH] = _T("");
				TCHAR *tszFilename;
#ifdef UNICODE
				TCHAR tszBuf[MAX_PATH];
				tszFilename = tszBuf;
				MultiByteToWideChar( CP_ACP, 0, pMeshContainer->pMaterials[iMaterial].pTextureFilename, -1, tszBuf, MAX_PATH );
				tszBuf[MAX_PATH - 1] = _T('\0');
#else
				tszFilename = pMeshContainer->pMaterials[iMaterial].pTextureFilename;
#endif
//
//				DXUtil_FindMediaFileCb( strTexturePath, sizeof(strTexturePath), tszFilename );
//				if( FAILED( D3DXCreateTextureFromFile( pd3dDevice, strTexturePath,
//					&pMeshContainer->pArrTex[iMaterial] ) ) )
//					pMeshContainer->pArrTex[iMaterial] = NULL;
//
//				// don't remember a pointer into the dynamic memory, just forget the name after loading
//				pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;



				//(E) 2008-01-28 Editor: AFEW
				// 외부에서 텍스처를 생성하고 해제한다.
				pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
				//pMeshContainer->pArrTex[iMaterial] = NULL;

				if(tszFilename && _tcslen(tszFilename)>3)
				{
					TCHAR drive[_MAX_DRIVE];
					TCHAR dir[_MAX_DIR];
					TCHAR fname[_MAX_FNAME];
					TCHAR ext[_MAX_EXT];
					TCHAR sFile[_MAX_EXT];

					_splitpath( tszFilename, drive, dir, fname, ext );

					//_stprintf(sFile, "%s%s", fname, ext);

					// 확장자 제거
					_stprintf(sFile, "%s", fname);
					// 텍스처의 파일 이름과 텍스처포인터의 주소를 저장.
					((CLcXSkinSrc*)m_pApp)->PushTexture(sFile, &pMeshContainer->pArrTex[iMaterial]);
				}

			}
		}
	}
	else // if no materials provided, use a default one
	{
		pMeshContainer->pMaterials[0].pTextureFilename = NULL;
		memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}

	// if there is skinning information, save off the required data and then setup for HW skinning
	if (pSkinInfo != NULL)
	{
		// first save off the SkinInfo and original mesh data
		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		pMeshContainer->pOrigMesh = pMesh;
		pMesh->AddRef();

		// Will need an array of offset matrices to move the vertices from the figure space to the bone's space
		cBones = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
		if (pMeshContainer->pBoneOffsetMatrices == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		// get each of the bone offset matrices so that we don't need to get them later
		for (iBone = 0; iBone < cBones; iBone++)
		{
			pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
		}

		// GenerateSkinnedMesh will take the general skinning information and transform it to a HW friendly version
		hr = ((CLcXSkinSrc*)m_pApp)->GenerateSkinnedMesh(pMeshContainer);
		if (FAILED(hr))
			goto e_Exit;
	}

	*ppNewMeshContainer = pMeshContainer;
	pMeshContainer = NULL;
e_Exit:
	SAFE_RELEASE(pd3dDevice);

	// call Destroy function to properly clean up the memory allocated
	if (pMeshContainer != NULL)
	{
		DestroyMeshContainer(pMeshContainer);
	}

	return hr;
}



HRESULT CLcXSkinAlloc::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	SAFE_DELETE_ARRAY( pFrameToFree->Name );
	SAFE_DELETE( pFrameToFree );
	return S_OK;
}



HRESULT CLcXSkinAlloc::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	SAFE_DELETE_ARRAY( pMeshContainer->Name );
	SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
	SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );
	SAFE_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );

	// release all the allocated textures
	if(pMeshContainer->pArrTex)
	{
		for (UINT iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			TexLayer*	pLayer = &pMeshContainer->pArrTex[iMaterial];

			for(int i=0; i<TexLayer::MAX_TEXTURE_LAYER; ++i)
			{
				LPDIRECT3DTEXTURE9 pTex = pLayer->p[i];
				
				if(pTex)
					pTex->Release();

				pLayer->p[i] = NULL;
			}
		}
	}

	SAFE_DELETE_ARRAY( pMeshContainer->pArrTex );
	SAFE_DELETE_ARRAY( pMeshContainer->ppBoneMatrixPtrs );
	SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );
	SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
	SAFE_RELEASE( pMeshContainer->pSkinInfo );
	SAFE_RELEASE( pMeshContainer->pOrigMesh );
	SAFE_DELETE( pMeshContainer );
	return S_OK;
}




