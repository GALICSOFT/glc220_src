// Implementation of the CLcFont2D class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#pragma warning(disable : 4786)
#include <string>
#include <vector>
using namespace std;


#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#include <d3dx9.h>

#include "LnD3Type.h"
#include "LnEuclid.h"
#include "LnBase.h"


#include "LnFont2D.h"


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)	{	if(p){(p)->Release();p = NULL;	}}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif



TLcFont::TLcFont()
: iHeight	(12)
, iWeight	(FW_NORMAL)
, iItalic	(0)
, iThckX	(0)
, iThckY	(0)
, dString	(0xFFFFFFFF)
, dGirth	(0xFF000099)
{
	_tcscpy(sName, "Arial");
}

TLcFont::TLcFont(TCHAR*	_sN						// Font Name
			,	INT		_iHeight				// Height
			,	INT		_iWeight				// Weight, Normal, Bold, thin...
			,	INT		_iItalic				// Is Italic
			,	INT		_iThckX					// Girth of thick X
			,	INT		_iThckY					// Girth of thick Y
			,	DWORD	_dString				// String color
			,	DWORD	_dGirth					// Girth of Font color
			)
			: iHeight	(_iHeight)
			, iWeight	(_iWeight)
			, iItalic	(_iItalic)
			, iThckX	(_iThckX)
			, iThckY	(_iThckY)
			, dString	(_dString)
			, dGirth	(_dGirth)
{
	_tcscpy(sName, _sN);
}

const TLcFont& TLcFont::operator=(const TLcFont& r)	// r: right hand side(rhs)
{	
	iHeight	= r.iHeight;
	iWeight	= r.iWeight;
	iItalic	= r.iItalic;
	iThckX	= r.iThckX;
	iThckY	= r.iThckY;
	dString	= r.dString;
	dGirth	= r.dGirth;
	_tcscpy(sName, r.sName);
	return *this;
}

const TLcFont& TLcFont::operator=(const TLcFont* r)
{	
	iHeight	= r->iHeight;
	iWeight	= r->iWeight;
	iItalic	= r->iItalic;
	iThckX	= r->iThckX;
	iThckY	= r->iThckY;
	dString	= r->dString;
	dGirth	= r->dGirth;
	_tcscpy(sName, r->sName);
	return *this;
}




INT LcDev_CreateFont(char* sCmd
				 , ILC_Font** pData
				 , void* p1		// TLcFont
				 , void* p2		// No Use
				 , void* p3		// No Use
				 , void* p4		// No Use
				 )
{
	(*pData) = NULL;

	if(0==_stricmp("PC Font", sCmd))
	{
		CLcFont2D* pObj = NULL;

		pObj = new CLcFont2D;

		if(FAILED(pObj->Create(p1, p2, p3, p4)))
		{
			// Return Error Notification
			delete pObj;
			return -1;
		}

		(*pData) = pObj;
		
		return 0;
	}

	return -1;
}





CLcFont2D::CLcFont2D()
{
	m_pTxD	= NULL;
	
	m_hFnt	= NULL;
	
	m_iH	= 24;
	m_iW	= 0;

	m_dDff	= 0xFFFFFFFF;
	m_dFrn	= 0xFFFFFF00;
	m_dBck	= 0xFF000099;
	m_iTcX	= 0;
	m_iTcY	= 0;
	
	m_vcP.x = 0;
	m_vcP.y = FLOAT(m_iH-2);
	m_vcA.x = 0;
	m_vcA.y = 0;

	m_iTxX	= m_iW + 2* m_iTcX;
	m_iTxY	= m_iH + 2* m_iTcY + 4;

	m_sSrc	= NULL;
}


CLcFont2D::~CLcFont2D()
{
	Destroy();
}


INT CLcFont2D::Create( void* p1		// TLcFont
					 , void* p2		// No Use
					 , void* p3		// No Use
					 , void* p4		// No Use
				 )
{
	HRESULT hr=0;
	INT		i=0;
	

	TLcFont*	pFnt = (TLcFont*)p1;
	
	if(m_hFnt)
	{
		DeleteObject(m_hFnt);
	}


	m_iH	= pFnt->iHeight	;

	m_iTcX	= pFnt->iThckX	;
	m_iTcY	= pFnt->iThckY	;
	m_dFrn	= pFnt->dString	;
	m_dBck	= pFnt->dGirth	;

	LOGFONT lFont=
	{
		-m_iH
		, 0
		, 0
		, 0
		, pFnt->iWeight
		, pFnt->iItalic
		, FALSE
		, FALSE
		, HANGUL_CHARSET
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, ANTIALIASED_QUALITY
		, FF_DONTCARE
		, "Arial"
	};

	_tcscpy(lFont.lfFaceName, pFnt->sName);

	m_hFnt = CreateFontIndirect(&lFont);


	for(i=0; i<4; ++i)
	{
		m_pVtx[i].p.z = (	0.f	);
		m_pVtx[i].p.w = (	1.f	);
		m_pVtx[i].d = 0xFFFFFFFF;
	}


	return 0;
}





void CLcFont2D::Destroy()
{
	if(m_hFnt)
	{
		DeleteObject(	m_hFnt	);
		m_hFnt	= NULL;
	}

	SAFE_DELETE_ARRAY(	m_sSrc	);
	SAFE_RELEASE(	m_pTxD	);
}


void CLcFont2D::DrawTxt()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	if(NULL==m_pTxD)
		return;
	
	DWORD	tRs[8]={0};
	pDev->GetRenderState(D3DRS_CULLMODE, &tRs[0]);
	pDev->GetRenderState(D3DRS_LIGHTING, &tRs[1]);
	pDev->GetRenderState(D3DRS_ZENABLE, &tRs[2]);
	pDev->GetRenderState(D3DRS_ALPHATESTENABLE, &tRs[3]);
	pDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &tRs[4]);


	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);

	pDev->SetTexture(0, m_pTxD);

	pDev->SetFVF(CLcFont2D::VtxDRHW::FVF);
	pDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_pVtx, sizeof (CLcFont2D::VtxDRHW));

	pDev->SetTexture(0, NULL);

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);


	pDev->SetRenderState(D3DRS_CULLMODE, tRs[0]);
	pDev->SetRenderState(D3DRS_LIGHTING, tRs[1]);
	pDev->SetRenderState(D3DRS_ZENABLE, tRs[2]);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, tRs[3]);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, tRs[4]);
}


INT CLcFont2D::Query(char* sCmd, void* pData)
{
	if(0==_stricmp("Set Color Diffuse", sCmd))
	{
		FLOAT*	pColor = (FLOAT*)pData;

//		LPXCOLOR rgba;
//		rgba.r = pColor[0];
//		rgba.g = pColor[1];
//		rgba.b = pColor[2];
//		rgba.a = pColor[3];

		SetColorDiffuse(pColor);

		return 0;
	}

	
	else if(0==_stricmp("Set Color Fore", sCmd))
	{
		FLOAT*	pColor = (FLOAT*)pData;

		SetColorFore(pColor);

		return 0;
	}
	
	else if(0==_stricmp("Set Color Back", sCmd))
	{
		FLOAT*	pColor = (FLOAT*)pData;

		SetColorBack(pColor);

		return 0;
	}


	return -1;
}


INT CLcFont2D::SetColorDiffuse(FLOAT* pColor)
{
	LCXCOLOR rgba;
	rgba.r = pColor[0];
	rgba.g = pColor[1];
	rgba.b = pColor[2];
	rgba.a = pColor[3];

	DWORD dDiff = rgba;

	bool bUdate =false;

	if(m_dDff != dDiff)
		bUdate = true;

	if(false == bUdate)
		return 0;

	m_dDff	= dDiff;
	
	return SetDiffuse();
}


INT CLcFont2D::SetColorFore(FLOAT* pColor)
{
	LCXCOLOR rgba;
	rgba.r = pColor[0];
	rgba.g = pColor[1];
	rgba.b = pColor[2];
	rgba.a = pColor[3];

	DWORD dwFrnt = rgba;

	bool bUdate =false;

	if(m_dFrn != dwFrnt)
		bUdate = true;

	if(false == bUdate)
		return 0;

	m_dFrn	= dwFrnt;
	
	return SetTexture();
}

INT CLcFont2D::SetColorBack(FLOAT* pColor)
{
	LCXCOLOR rgba;
	rgba.r = pColor[0];
	rgba.g = pColor[1];
	rgba.b = pColor[2];
	rgba.a = pColor[3];

	DWORD dBack = rgba;

	bool bUdate =false;

	if(m_dBck != dBack)
		bUdate = true;

	if(false == bUdate)
		return 0;

	m_dBck	= dBack;
	
	return SetTexture();
}


INT CLcFont2D::OnLostDevice()
{
	return -1;
}

INT CLcFont2D::OnReset()
{
	return -1;
}


INT CLcFont2D::GetType()
{
	return ILC_Font::ELcFont::LPFNT_CE;
}



void CLcFont2D::SetString(TCHAR* sSrc)
{
	HDC			hDC = NULL;
	
	SIZE		sz;
	TEXTMETRIC	tm;
	INT			Len;


	// Null or string legth is 0
	if(NULL ==sSrc || 0 == (m_iLen = _tcslen(sSrc) ) )
	{
		SAFE_DELETE_ARRAY(	m_sSrc	);
		SAFE_RELEASE(	m_pTxD	);
		return;
	}

	
	// same old string
	if( NULL != m_sSrc && 0 == _tcsicmp(m_sSrc, sSrc))
		return;


	// clear old string
	SAFE_DELETE_ARRAY(	m_sSrc	);


	Len    = (3 + m_iLen/4)*4;
	m_sSrc = new TCHAR[ Len ];	// make multiple of 4
	memset(m_sSrc, 0, Len);
	_tcscpy(m_sSrc, sSrc);


	hDC = GetDC(NULL);
	SelectObject(hDC, m_hFnt);													// Parent window의 DC를 이용해 스트링의 정확한 픽셀 사이즈를 얻어 온다.

	GetTextExtentPoint32(hDC, m_sSrc, m_iLen, &sz);
	m_iW = sz.cx;
	
	GetTextMetrics(hDC, &tm);
	m_iW -= tm.tmOverhang;
	ReleaseDC(NULL, hDC);
	
	SetTexture();
}


void CLcFont2D::SetThickX(INT iThickX)
{
	m_iTcX	= iThickX;
}

void CLcFont2D::SetThickY(INT iThickY)
{
	m_iTcY	= iThickY;
}

void CLcFont2D::SetThick(INT iThick)
{
	m_iTcX	= m_iTcY = iThick;
}

void CLcFont2D::SetHeight(INT Height)
{
	m_iH	= Height;
}



void CLcFont2D::SetPosition(FLOAT* vcPos)
{
	m_vcP.x = vcPos[0];
	m_vcP.y = vcPos[1];


	D3DXVECTOR3 pos;
	
	pos = m_vcP + m_vcA;

	FLOAT	fX	= FLOAT(m_iTxX);
	FLOAT	fY	= FLOAT(m_iTxY);

	m_pVtx[0].p.x = (	pos.x		);
	m_pVtx[0].p.y = (	pos.y		);

	m_pVtx[1].p.x = (	pos.x + fX	);
	m_pVtx[1].p.y = (	pos.y + 0.f	);

	m_pVtx[2].p.x = (	pos.x + 0.f	);
	m_pVtx[2].p.y = (	pos.y + fY	);

	m_pVtx[3].p.x = (	pos.x + fX	);
	m_pVtx[3].p.y = (	pos.y + fY	);
}


void CLcFont2D::SetPosAlign(DWORD dwAlign)
{
	m_dAln = dwAlign;
	
	m_vcA.x = 0.f;
	m_vcA.y = 0.f;
	
	if( dwAlign & FNT_ALIGN_H_R)
		m_vcA.x = -m_iW*1.f;
	
	if( dwAlign & FNT_ALIGN_H_C)
		m_vcA.x = -m_iW/2.f;
	
	if( dwAlign & FNT_ALIGN_V_B)
		m_vcA.y = -m_iH*1.f;
	
	if( dwAlign & FNT_ALIGN_V_C)
		m_vcA.y = -m_iH/2.f;


	D3DXVECTOR3 pos;
	
	pos = m_vcP + m_vcA;

	FLOAT	fX	= FLOAT(m_iTxX);
	FLOAT	fY	= FLOAT(m_iTxY);

	m_pVtx[0].p.x = (	pos.x		);
	m_pVtx[0].p.y = (	pos.y		);

	m_pVtx[1].p.x = (	pos.x + fX	);
	m_pVtx[1].p.y = (	pos.y + 0.f	);

	m_pVtx[2].p.x = (	pos.x + 0.f	);
	m_pVtx[2].p.y = (	pos.y + fY	);

	m_pVtx[3].p.x = (	pos.x + fX	);
	m_pVtx[3].p.y = (	pos.y + fY	);
}



INT CLcFont2D::Quntum2Power(INT v)
{
	INT n=1;
	for(; n<v; n <<= 1);
	return n;
}


INT CLcFont2D::SetTexture()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	HRESULT	hr = -1;
	INT		i, j;
	HDC		hDC=0;

	PDTX	pSource	= NULL;
	PDSF	pSfTxtS	= NULL;
	PDSF	pSfTxtD	= NULL;

	SAFE_RELEASE(	m_pTxD	);


	D3DSURFACE_DESC		dsc;
	D3DLOCKED_RECT		dmrc;

	D3DPOOL				d3dPool;

//	D3DCAPS9			caps;
//	if( FAILED( pDev->GetDeviceCaps(&caps) ) )
//		return -1;
//
//	d3dPool = (caps.SurfaceCaps & D3DSURFCAPS_VIDTEXTURE) ? D3DPOOL_VIDEOMEM : D3DPOOL_SYSTEMMEM;
	d3dPool = D3DPOOL_MANAGED;


	m_iTxX	= m_iW + 2* m_iTcX + 2;
	m_iTxY	= m_iH + 2* m_iTcY + 4;

	m_iSx = Quntum2Power( m_iTxX );
	m_iSy = Quntum2Power( m_iTxY );

	// 불투명 Texture를 만든다.
//	hr = pDev->CreateImageSurface(m_iSx, m_iSy, D3DFMT_X1R5G5B5, &pSfTxtS);
	hr = D3DXCreateTexture(pDev, m_iSx, m_iSy, 1, 0, D3DFMT_X1R5G5B5, d3dPool, &pSource);

	if(FAILED(hr))
		return hr;

	hr	= pSource->GetLevelCount();
	hr	= pSource->GetSurfaceLevel(0, &pSfTxtS);
	hr	= pSfTxtS->GetDesc(&dsc);

	hr  = pSfTxtS->LockRect(&dmrc, 0, 0 );
	
	if(SUCCEEDED(hr))
	{
		WORD*	pData = (WORD*) dmrc.pBits;
		memset(pData, 0x00, sizeof(WORD) * dsc.Width * dsc.Height);
		hr  = pSfTxtS->UnlockRect();
	}


	hDC = NULL;
	hr  = pSfTxtS->GetDC(&hDC);

	if(FAILED(hr))
	{
//		int c=0;
//
//		switch(hr)
//		{
//			case D3DERR_ALREADYLOCKED                   :	c=0;	break;
//			case D3DERR_CONFLICTINGRENDERSTATE          :	c=1;	break;
//			case D3DERR_CONFLICTINGTEXTUREFILTER        :	c=2;	break;
//			case D3DERR_CONFLICTINGTEXTUREPALETTE       :	c=3;	break;
//			case D3DERR_DEVICELOST                      :	c=4;	break;
//			case D3DERR_DEVICENOTRESET                  :	c=5;	break;
//			case D3DERR_DRIVERINTERNALERROR             :	c=6;	break;
//			case D3DERR_DRIVERINVALIDCALL               :	c=7;	break;
//			case D3DERR_DRIVERUNSUPPORTED               :	c=8;	break;
//			case D3DERR_INVALIDCALL                     :	c=9;	break;
//			case D3DERR_INVALIDDEVICE                   :	c=10;	break;
//			case D3DERR_MEMORYPOOLEMPTY                 :	c=11;	break;
//			case D3DERR_MOREDATA                        :	c=12;	break;
//			case D3DERR_NOTAVAILABLE                    :	c=13;	break;
//			case D3DERR_NOTFOUND                        :	c=14;	break;
//			case D3DERR_TOOMANYOPERATIONS               :	c=15;	break;
//			case D3DERR_UNSUPPORTEDALPHAARG             :	c=16;	break;
//			case D3DERR_UNSUPPORTEDALPHAOPERATION       :	c=17;	break;
//			case D3DERR_UNSUPPORTEDCOLORARG             :	c=18;	break;
//			case D3DERR_UNSUPPORTEDCOLOROPERATION       :	c=19;	break;
//			case D3DERR_UNSUPPORTEDFACTORVALUE          :	c=20;	break;
//			case D3DERR_UNSUPPORTEDTEXTUREFILTER        :	c=21;	break;
//			case D3DERR_WRONGTEXTUREFORMAT              :	c=22;	break;
//		}

		if(!hDC)
		{
			if(	pSfTxtS )
			{
				pSfTxtS->Release();
				pSfTxtS = NULL;
			}
			
			return -1;
		}
	}
	
	
	SelectObject(hDC, m_hFnt);
	SetBkMode(hDC, TRANSPARENT);
	
	
	COLORREF dBck;
	COLORREF dFrn;
	BYTE*	pClr;
	
	pClr = GetColorFromARGB(m_dBck);
	dBck = RGB( pClr[1],  pClr[2],  pClr[3]);
	
	pClr = GetColorFromARGB(m_dFrn);
	dFrn = RGB( pClr[1],  pClr[2],  pClr[3]);

	for(i=-m_iTcX; i<=m_iTcX; ++i)
	{
		for(j=-m_iTcY; j<=m_iTcY ; ++j)
		{
			if(i==0 && i==j)
				continue;
			
			SetTextColor(hDC, dBck );
			::ExtTextOut(hDC, m_iTcX+ i, m_iTcY+j, 0, NULL, m_sSrc, m_iLen, NULL);
		}
	}
	
	SetTextColor(hDC, dFrn);
//	TextOut(hDC, m_iTcX, m_iTcY, m_sMsg.c_str(), m_iLen);
	::ExtTextOut(hDC, m_iTcX, m_iTcY, 0, NULL, m_sSrc, m_iLen, NULL);

	hr = pSfTxtS->ReleaseDC(hDC);
	
	if(FAILED(hr))
		return -1;
	
	// 반투명 텍스춰를 만든다.
	hr = D3DXCreateTexture(pDev, m_iSx, m_iSy, 1, 0, D3DFMT_A1R5G5B5, d3dPool, &m_pTxD);
	
	m_pTxD->GetSurfaceLevel(0, &pSfTxtD);
	pSfTxtD->GetDesc(&dsc);
	
	
	// 반투명 텍스춰의 영역으로 그리기 영역을 설정한다.
	m_rc.left	= 0;
	m_rc.top	= 0;
	m_rc.right	= dsc.Width;
	m_rc.bottom = dsc.Height;
	
	// 불투명 텍스춰를 반투명으로 옮기되 불투명의 배경으로 사용한 검정색을 컬러키로 설정한다.
	hr = D3DXLoadSurfaceFromSurface( pSfTxtD, NULL, NULL, pSfTxtS,  NULL, NULL, D3DX_DEFAULT, 0xFF000000);

	SAFE_RELEASE(	pSfTxtD	);
	SAFE_RELEASE(	pSfTxtS	);
	SAFE_RELEASE(	pSource	);

	return SetDiffuse();
}


INT CLcFont2D::SetDiffuse()
{	
	D3DXVECTOR3 pos;
	
	pos = m_vcP + m_vcA;

	FLOAT	fX	= FLOAT(m_iTxX);
	FLOAT	fY	= FLOAT(m_iTxY);

	m_pVtx[0].u	= (	(0.5F)/m_iSx	);
	m_pVtx[0].v	= (	(0.5F)/m_iSy	);

	m_pVtx[3].u	= (	(m_iTxX -0.5F)/m_iSx	);
	m_pVtx[3].v	= (	(m_iTxY -0.5F)/m_iSy	);

	m_pVtx[1].u	= m_pVtx[3].u;
	m_pVtx[1].v	= m_pVtx[0].v;

	m_pVtx[2].u	= m_pVtx[0].u;
	m_pVtx[2].v	= m_pVtx[3].v;


	m_pVtx[0].p.x = (	pos.x		);
	m_pVtx[0].p.y = (	pos.y		);

	m_pVtx[1].p.x = (	pos.x + fX	);
	m_pVtx[1].p.y = (	pos.y + 0.f	);

	m_pVtx[2].p.x = (	pos.x + 0.f	);
	m_pVtx[2].p.y = (	pos.y + fY	);

	m_pVtx[3].p.x = (	pos.x + fX	);
	m_pVtx[3].p.y = (	pos.y + fY	);

	for(int i=0; i<4; ++i)
		m_pVtx[i].d = m_dDff;

	return 0;
}



// ARGB <--> BGR
BYTE* CLcFont2D::GetColorFromARGB(DWORD& _argb)
{
	static BYTE argb[4];
	BYTE* pClr = (BYTE*)&_argb;
	
	argb[0] = pClr[3];
	argb[1] = pClr[2];
	argb[2] = pClr[1];
	argb[3] = pClr[0];
	
	return argb;
}



