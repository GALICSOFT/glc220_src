// Implementation of the CLcTex class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <d3dx9.h>

#include "LnD3Type.h"
#include "LnUtilDx.h"
#include "LnBase.h"

#include "LnTexture.h"




CLcTex::TLcTex::TLcTex()
{
	m_pTx	= 0;
	m_dC	= 0x00FFFFFF;
	m_dF	= (1<<0);			// D3DMX_FILTER_NONE;
	m_dP	= D3DPOOL_MANAGED;

	memset(m_sF, 0, sizeof m_sF);
	memset(&m_Inf, 0, sizeof m_Inf);
}


void CLcTex::TLcTex::Release()
{
	if(m_pTx)
	{
		m_pTx->Release();
		m_pTx = NULL;
	}
}


CLcTex::CLcTex()
{
	m_nId	= -1;
}

CLcTex::CLcTex(INT _nId, PDTX _pTx, DWORD _dC, DWORD _dF, DWORD _dPool, const DIMG* _Inf,  const char* _sFile)
{
	m_nId		= _nId;
	m_LcTx.m_pTx= _pTx;
	m_LcTx.m_dC	= _dC;
	m_LcTx.m_dF	= _dF;
	m_LcTx.m_dP	= _dPool;

	memcpy(&m_LcTx.m_Inf, _Inf, sizeof m_LcTx.m_Inf);

	if(_sFile)
	{
		memset(m_LcTx.m_sF, 0, sizeof m_LcTx.m_sF);
		strcpy(m_LcTx.m_sF, _sFile);
	}
}




CLcTex::~CLcTex()
{
	CLcTex::Destroy();
}


INT CLcTex::Create(void*, void*, void*, void*)
{
//	printf("CLcTex Create\n");
	return 0;
}

void CLcTex::Destroy()
{
	m_LcTx.Release();	
}

INT CLcTex::Query(char*, void*)
{
//	printf("CLcTex Query:%s\n", sCmd);
	return 0;
}



DWORD	CLcTex::GetWidth()
{
	return m_LcTx.m_Inf.Width;
}

DWORD	CLcTex::GetHeight()
{
	return m_LcTx.m_Inf.Height;
}

DWORD	CLcTex::GetDepth()
{
	return m_LcTx.m_Inf.Depth;
}

DWORD	CLcTex::GetMipLevel()
{
	return m_LcTx.m_Inf.MipLevels;
}

DWORD	CLcTex::GetFormat()
{
	return m_LcTx.m_Inf.Format;
}

DWORD	CLcTex::GetResourceType()
{
	return 0;
}

DWORD	CLcTex::GetFileFormat()
{
	return 0;
}

char*	CLcTex::GetSourceName()
{
	return m_LcTx.m_sF;
}

void*	CLcTex::GetPointer()
{
	return m_LcTx.m_pTx;
}

DWORD	CLcTex::GetColorKey()
{
	return m_LcTx.m_dC;
}


DWORD	CLcTex::GetFilter()
{
	return m_LcTx.m_dF;
}


void CLcTex::GetRECT(RECT* rc)
{
	rc->left=0;
	rc->top=0;
	rc->right=m_LcTx.m_Inf.Width;
	rc->bottom=m_LcTx.m_Inf.Height;
}




void CLcTex::SetId(INT nId)
{
	m_nId = nId;
}

void CLcTex::SetTx(void* pTx)
{
	m_LcTx.m_pTx = (PDTX)pTx;
}

void CLcTex::SetColorKey(DWORD dColorKey)
{
	m_LcTx.m_dC = dColorKey;
}

void CLcTex::SetFilter(DWORD dFilter)
{
	m_LcTx.m_dF = dFilter;
}

void CLcTex::SetInfo(DIMG* pInf)
{
	memcpy(&m_LcTx.m_Inf, pInf, sizeof( m_LcTx.m_Inf));
}

void CLcTex::SetFileName(char* sFile)
{
	if(sFile)
	{
		strcpy(m_LcTx.m_sF, sFile);
	}
}




INT LcDev_CreateTex(char* sCmd
					, PLC_TEXTURE* pData
					, void* pVal1
					, DWORD dColorKey
					, DWORD dFilter
					, DWORD Pool)
{
	(*pData) = NULL;

	CLcTex* pObj = NULL;

	PDEV	pDev = (PDEV)LcDev_GetD3Device();
	PDTX	pTexture;
	DWORD	dColor = dColorKey;
	DWORD	Filter=dFilter;
	DWORD	MipFilter= dFilter;
	DIMG	pSrcInf;


	if(0==_stricmp("File", sCmd))
	{
		char*	sFile=(char*)pVal1;

		if(FAILED(LcD3D_TexLoadFile(&pTexture, pDev, sFile, dColor, &pSrcInf, Filter, MipFilter)))
			return -1;

		pObj = new CLcTex(-1, pTexture, dColor,  Filter, Pool, &pSrcInf, sFile);
	}

	
	(*pData) = pObj;

	return 0;
}




ILC_Texture::TxLayer::TxLayer()
{
	for(int i=0; i<ILC_Texture::MAX_TEXTURE_LAYER; ++i)
	{
		p[i] = NULL;
	}
}








CLcTexMng::CLcTexMng()
{
	m_nId	= -1;

	memset(m_sName, 0, sizeof m_sName);
}



CLcTexMng::~CLcTexMng()
{
	CLcTexMng::Destroy();
}


INT CLcTexMng::Create(void* p1, void*, void*, void*)
{
	return 0;
}

void CLcTexMng::Destroy()
{
	if(!m_lsTex.empty())
	{
		for(itTLcTex it= m_lsTex.begin(); it !=m_lsTex.end(); )
		{
			PLC_TEXTURE pTx = (*it).second;
			pTx->Destroy();
			delete pTx;
			++it;
		}

		m_lsTex.clear();
	}
}


INT CLcTexMng::Query(char*, void*)
{
//	printf("CLcTexMng Query:%s\n", sCmd);
	return 0;
}


INT CLcTexMng::Find(char* sName, PLC_TEXTURE* pData)
{
	char*		pstr = sName;
	itTLcTex	it;

	it = m_lsTex.find(pstr);

	if(it == m_lsTex.end())
	{
		*pData = NULL;
		return -1;
	}

	if(pData)
	{
		*pData = (*it).second;
	}
	
	return 0;
}

INT CLcTexMng::FindCreate(char* sCmd
						, PLC_TEXTURE* pData
						, void* pVal1
						, DWORD dColorKey
						, DWORD dFilter
						, DWORD Pool)
{
	INT hr=-1;
	char*	sName=(char*)pVal1;

	if(SUCCEEDED(Find(sName, pData)))
		return 0;

	// 없으면 파일을 생성하고 있으면 기존의 포인터
	hr = LcDev_CreateTex(sCmd
						, pData
						, pVal1
						, dColorKey
						, dFilter
						, Pool);

	if(FAILED(hr))
		return -1;

	m_lsTex.insert(	mpTLcTex::value_type( sName, *pData));

	return 0;
}











INT LcDev_CreateTexMng(char* sCmd
					, ILcTexMng** pData	// Texture Manager Instance
					, void* p1			// Device
					, void* p2			// No Use
					, void* p3			// No Use
					, void* p4			// No Use
					)
{

//	PDEV		pd3dDev = (PDEV)p1;
	CLcTexMng*	pObj	= NULL;

	*pData = NULL;

	pObj = new CLcTexMng;

	if(pObj->Create(p1, p2, p3, p4))
	{
		delete pObj;
		return -1;
	}

	*pData = pObj;
	
	return 0;
}

