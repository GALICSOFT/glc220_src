// Implementation of the ILC_Font.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#if defined(__BADA__)
  #include <FBaseSys.h>
#endif

#include <LcType.h>
#include <LcMath.h>

#include "LcBase.h"
#include "LcAux.h"

#include "LcFont.h"


LC_HANDLE	CLcFont::m_FTTontLib= NULL;
INT			CLcFont::m_RefFont  = 0;
LC_HANDLE	CLcFont::m_FTTdef	= NULL;


CLcFont::CLcFont()
{
	m_FtFcr		= NULL;
	m_FntOrgH	= 0;
	m_FntDstH	= 0;
	m_resH		= 96;			// Horizental Resolution
	m_resV		= 96;			// Vertical Resolution

	m_StrLen	= 0;			// Source String Length
	m_Str		= NULL;			// Source String
	m_Algn		= LC_FONT_ALIGN_LT;

	m_vDif		= UINT(0XFFFFFFFF);							// Color
	m_vPos		= LCXVECTOR2(0, 0);							// Position

	m_Sclup		= 2.0F;
	m_vScl		= LCXVECTOR2(1.F/m_Sclup, 1.F/m_Sclup);		// Scaling

	m_ChCnt		= 0;
	m_ChBgn		= NULL;

	m_pSpm		= NULL;
	m_pTex		= NULL;
}


CLcFont::~CLcFont()
{
	Destroy();
}



INT CLcFont::Query(char* sCmd, LC_HANDLE pData)
{
	if     (0 == LcStr_Stricmp(sCmd, LCQUERY_INVALID))
	{
		SAFE_RELEASE(	m_pTex	);
		return LC_OK;
	}
	else if(0 == LcStr_Stricmp(sCmd, LCQUERY_RESTORE))
	{
		if(!m_Str)
			return LC_OK;

		char*	sOrg = NULL;
		int		len = int( (m_StrLen+7)/4) * 4;

		sOrg = (char*)malloc(len);
		memset(sOrg, 0, len);
		strcpy(sOrg, m_Str);

		this->SetString(NULL);
		this->SetString(sOrg);

		free(sOrg);

		return LC_OK;
	}


	return LC_EFAIL;
}


INT CLcFont::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


INT CLcFont::SetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if(LC_FONT_ALIGN == dCmd)
	{
		m_Algn = *((INT*)pData);
	}
	else if(LC_FONT_COLOR == dCmd)
	{
		return SetColor( (const LCXCOLOR*)pData);
	}
	else if(LC_FONT_POSITION == dCmd)
	{
		return SetPosition( (const LCXVECTOR2*)pData);
	}
	else if(LC_FONT_SCALING == dCmd)
	{
		return SetScaling( (const LCXVECTOR2*)pData);
	}
	else if(LC_FONT_SCALEUP == dCmd)
	{
		FLOAT scl = *(FLOAT*)pData;

		if(scl == m_Sclup)
			return LC_OK;

		scl = FLOAT( INT(scl));
		if(1> scl || scl >5)
			return LC_EFAIL;


		m_vScl *= m_Sclup;
		m_Sclup  = scl;

		m_vScl	  /= m_Sclup;
		m_FntDstH  = INT(m_FntOrgH * m_Sclup);

		//BuildTexture();
	}
	else
		return LC_EFAIL;


	return LC_OK;
}


INT CLcFont::GetAttrib(UINT dCmd, LC_HANDLE pData)
{
	if(LC_FONT_ALIGN == dCmd)
	{
		*((INT*)pData) = m_Algn;
	}
	else if(LC_FONT_COLOR == dCmd)
	{
		*((LCXCOLOR*)pData) = m_vDif;
	}
	else if(LC_FONT_POSITION == dCmd)
	{
		*((LCXVECTOR2*)pData) = m_vPos;
	}
	else if(LC_FONT_SCALING == dCmd)
	{
		LCXVECTOR2* pOut = (LCXVECTOR2*)pData;
		pOut->x = m_vScl.x * m_Sclup;
		pOut->y = m_vScl.y * m_Sclup;
	}
	else if(LC_FONT_SCALEUP == dCmd)
	{
		*((FLOAT*)pData) = m_Sclup;
	}
	else if(LC_FONT_CHAR_COUNT == dCmd)
	{
		*((INT*)pData) = m_ChCnt;
	}
	else if(LC_FONT_RECT == dCmd)
	{
		memcpy(pData, &m_Rc, sizeof(LCXRECT) );
	}
	else if(LC_FONT_RECT_SCALEUP == dCmd)
	{
		LC_RECTF*  pOut = (LC_RECTF*)pData;
		pOut->x = m_Rc.x/m_Sclup;
		pOut->y = m_Rc.y/m_Sclup;
		pOut->w = m_Rc.w/m_Sclup;
		pOut->h = m_Rc.h/m_Sclup;
	}
	else
		return LC_EFAIL;


	return LC_OK;
}


INT CLcFont::SetString(const char* sStr)
{
	int	len = 0;

	// char	sStr[128]="°¥¸¯Hello World ¾È³ç^^0ùÓí­";
	// Null or string legth is 0
	if(NULL ==sStr || 0 == (len = strlen(sStr) ) )
	{
		SAFE_DELETE_ARRAY(	m_Str	);
		SAFE_RELEASE(	m_pTex	);

		m_ChCnt		= 0;
		SAFE_DELETE_ARRAY(	m_ChBgn	);

		return LC_OK;
	}


	// same old string
	if( NULL != m_Str && 0 == LcStr_Strcmp(m_Str, sStr))
		return LC_OK;


	// clear old string
	SAFE_DELETE_ARRAY(	m_Str	);

	m_StrLen = len;					// save string length

	len = int( (m_StrLen+7)/4) * 4;	// resize length
	m_Str = new char[ len ];		// make multiple of 4
	memset(m_Str, 0, len);
	strcpy(m_Str, sStr);

	return BuildTexture();
}



INT CLcFont::SetColor(const LC_COLORF* v)
{
	if(NULL == v)
		m_vDif = LCXCOLOR(0,0,0,1);
	else
		m_vDif =  *v;		// Color

	return LC_OK;
}


INT CLcFont::SetPosition(const LC_FLOAT2* v)
{
	m_vPos	= *v;		// Position
	return LC_OK;
}


INT CLcFont::SetScaling(const LC_FLOAT2* v)
{
	m_vScl.x = v->x / m_Sclup;		// Scaling
	m_vScl.y = v->y / m_Sclup;		// Scaling

	return LC_OK;
}


INT CLcFont::DrawTxt(PLC_SPRITE pSprite, INT Bgn, INT End, const LC_FLOAT2* vcTrn, const LC_COLORF* color)
{
	INT	hr = LC_EFAIL;

	if(NULL == m_pTex)
		return hr;

	if(NULL == pSprite)
		pSprite = m_pSpm;


	if(NULL == pSprite)
		return hr;


	LCXVECTOR2	vPos(m_vPos);
	LCXCOLOR	vCol(m_vDif);

	LCXRECT		rc(m_Rc);
	LCXRECT		rcD;

	if(vcTrn)
		vPos = *vcTrn;

	if(color)
		vCol = *color;

//  Polygon region
//	End    End + 1
//	+------+
//	|      |
//	|      |
//	|      |
//	+------+

	if(0 > End || End >= m_ChCnt-1)
		End = m_ChCnt;
	else
		End = End+1;


	if(Bgn >= End)
		return hr;


	// setup the original region
	rc.x = m_ChBgn[Bgn];
	rc.w = m_ChBgn[End] - m_ChBgn[Bgn]-1.f;

	// apply the scaleup
	rcD.w = rc.w/m_Sclup;
	rcD.h = rc.h/m_Sclup;

	if(!vcTrn)
		vPos.x += (rc.x + 0.5F)/m_Sclup;


	// Setup the final position
	if     (LC_FONT_ALIGN_RT == m_Algn){ vPos.x -= rcD.w;								}
	else if(LC_FONT_ALIGN_CT == m_Algn){ vPos.x -= rcD.w * .5F;							}
	else if(LC_FONT_ALIGN_LM == m_Algn){ vPos.y -= rcD.h * .5F;							}
	else if(LC_FONT_ALIGN_RM == m_Algn){ vPos.x -= rcD.w;		vPos.y -= rcD.h * .5F;	}
	else if(LC_FONT_ALIGN_CM == m_Algn){ vPos.x -= rcD.w * .5F;	vPos.y -= rcD.h * .5F;	}
	else if(LC_FONT_ALIGN_LB == m_Algn){ vPos.y -= rcD.h;								}
	else if(LC_FONT_ALIGN_RB == m_Algn){ vPos.x -= rcD.w;		vPos.y -= rcD.h;		}
	else if(LC_FONT_ALIGN_CB == m_Algn){ vPos.x -= rcD.w * .5F;	vPos.y -= rcD.h;		}


	hr = pSprite->DrawScaling(m_pTex, &rc, &vPos, &m_vScl, &vCol);

	return hr;
}


INT CLcFont::GetRect(LC_RECTF* pRc, INT Idx)
{
	LCXVECTOR2	vPos(m_vPos);
	LCXRECT		rc(m_Rc);
	LCXRECT		rcD;

	INT Bgn=0, End=0;

	if(0>Idx)
	{
		Bgn = 0;
		End = m_ChCnt;
	}

	else if(Idx >= m_ChCnt-1)
	{
		Bgn = m_ChCnt-1;
		End = Bgn + 1;
	}
	else
	{
		Bgn = Idx;
		End = Bgn+1;
	}


	if(End == Bgn)
	{
		pRc->x = 0;
		pRc->y = 0;
		pRc->w = 0;
		pRc->h = 0;
		return LC_EFAIL;
	}

	// setup the original width and height
	rc.x = m_ChBgn[Bgn];
	rc.w = m_ChBgn[End] - m_ChBgn[Bgn];

	// apply the scaleup
	rcD.w = rc.w/m_Sclup;
	rcD.h = rc.h/m_Sclup;

	// add the position
	vPos.x += (rc.x + 0.5F)/m_Sclup;


	if     (LC_FONT_ALIGN_RT == m_Algn){ vPos.x -= rcD.w;								}
	else if(LC_FONT_ALIGN_CT == m_Algn){ vPos.x -= rcD.w * .5F;							}
	else if(LC_FONT_ALIGN_LM == m_Algn){ vPos.y -= rcD.h * .5F;							}
	else if(LC_FONT_ALIGN_RM == m_Algn){ vPos.x -= rcD.w;		vPos.y -= rcD.h * .5F;	}
	else if(LC_FONT_ALIGN_CM == m_Algn){ vPos.x -= rcD.w * .5F;	vPos.y -= rcD.h * .5F;	}
	else if(LC_FONT_ALIGN_LB == m_Algn){ vPos.y -= rcD.h;								}
	else if(LC_FONT_ALIGN_RB == m_Algn){ vPos.x -= rcD.w;		vPos.y -= rcD.h;		}
	else if(LC_FONT_ALIGN_CB == m_Algn){ vPos.x -= rcD.w * .5F;	vPos.y -= rcD.h;		}

	rc.x = vPos.x;
	rc.y = vPos.y;
	rc.r = rc.x + rcD.w;
	rc.b = rc.y + rcD.h;

	*pRc = rc;

	return LC_OK;
}



// setup the Output Position and Texture Coordinate List
INT CLcFont::SetupVtxLst(LC_FLOAT2* OutPos, LC_FLOAT2* OutTex)
{
	LCXVECTOR2* oPos = (LCXVECTOR2*)OutPos;
	LCXVECTOR2* oTex = (LCXVECTOR2*)OutTex;

	FLOAT	ImgW = 0;
	FLOAT	ImgH = 0;
	FLOAT	TexW = 0;
	FLOAT	TexH = 0;
	UINT	nTex = 0;

	if(NULL == m_pTex)
		return LC_CANNT_FIND_RESOURCE;

	ImgW = (FLOAT)m_pTex->GetImgW();
	ImgH = (FLOAT)m_pTex->GetImgH();
	TexW = (FLOAT)m_pTex->GetTexW();
	TexH = (FLOAT)m_pTex->GetTexH();
	nTex = m_pTex->GetId();

	if(0==nTex)
		return LC_CANNT_FIND_RESOURCE;


	// resize width and height
	FLOAT	tw = ImgW / m_Sclup;
	FLOAT	th = ImgH / m_Sclup;

	// Setup the final position
	if     (LC_FONT_ALIGN_RT == m_Algn){ oPos[0].x -= tw;								}
	else if(LC_FONT_ALIGN_CT == m_Algn){ oPos[0].x -= tw * .5F;							}
	else if(LC_FONT_ALIGN_LM == m_Algn){ oPos[0].y -= th * .5F;							}
	else if(LC_FONT_ALIGN_RM == m_Algn){ oPos[0].x -= tw;		oPos[0].y -= th * .5F;	}
	else if(LC_FONT_ALIGN_CM == m_Algn){ oPos[0].x -= tw * .5F;	oPos[0].y -= th * .5F;	}
	else if(LC_FONT_ALIGN_LB == m_Algn){ oPos[0].y -= th;								}
	else if(LC_FONT_ALIGN_RB == m_Algn){ oPos[0].x -= tw;		oPos[0].y -= th;		}
	else if(LC_FONT_ALIGN_CB == m_Algn){ oPos[0].x -= tw * .5F;	oPos[0].y -= th;		}

	oPos[0] = m_vPos;
	oPos[1] = oPos[0] + LCXVECTOR2(tw,  0);
	oPos[2] = oPos[0] + LCXVECTOR2(tw, th);
	oPos[3] = oPos[0] + LCXVECTOR2( 0, th);

	oTex[0] = LCXVECTOR2(0.0F     , 1.0F);
	oTex[1] = LCXVECTOR2(ImgW/TexW, 1.0F);
	oTex[2] = LCXVECTOR2(ImgW/TexW, 1.0F - ImgH/TexH);
	oTex[3] = LCXVECTOR2(0.0F     , 1.0F - ImgH/TexH);

	return LC_OK;
}




////////////////////////////////////////////////////////////////////////////////

// Create Font
INT LcDev_CreateFontFromFile(char* sCmd
					, PLC_FONT* pOut		// Output instance
					, LC_HANDLE p1			// PLC_SPRITE
					, LC_HANDLE p2			// TLC_Font*
					, LC_HANDLE p3			// No Use
					, LC_HANDLE p4			// No Use
					 )
{
	INT			hr = 0;
	TLC_FONT*	ptFont =(TLC_FONT*)p2;
	CLcFont*	pObj = NULL;

	*pOut		= NULL;

	if(NULL == ptFont)
	{
		LOGE("Err: LcDev_CreateFontFromFile(). The font information is NULL or not sufficient.\n");
		return LC_EFAIL;
	}


	// In default font
	if(0 == LcStr_Stricmp(LCCMD_FONT_DEFAULT, ptFont->sName) )
		return LcDev_CreateFontFromMemory(NULL, pOut, p1, p2, p3, p4);


#if defined(__ANDROID__)
	char*		sFile = ptFont->sName;
	char		sMedia[LC_MAX_PATH2]={0};

	BYTE*		buf = NULL;
	ULONG		len = 0;
	const char*	sApkFile = LcSys_AndroidApkFile(NULL);

	hr = Lzip_UnzipbyName(&buf, &len, sApkFile, sFile, LC_TRUE);
	//LOGI("Create Font Unzip: %x %ld %d %s\n", (UINT)buf, len, hr, sFile);

	if(LC_FAILED(hr))
		return hr;


	ptFont->sName = sMedia;
	ptFont->pBuf  = &buf;
	ptFont->iLen  = len;

	hr = LcDev_CreateFontFromMemory(NULL, (PLC_FONT*)&pObj, p1, ptFont, p3, p4);
	LOGI("Create Font From Memory hr: %d\n", hr);

	//SAFE_FREE( buf ); Memory will be detroyed in font object.

	if(LC_FAILED(hr))
		return LC_CANNT_ALLOC;


	*pOut = pObj;
	return LC_OK;
#endif


	pObj = new CLcFont;

	hr = pObj->CreateFromFile(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}



// Create Font
INT LcDev_CreateFontFromMemory(char* sCmd
					, PLC_FONT* pOut		// Output instance
					, LC_HANDLE p1			// PLC_SPRITE
					, LC_HANDLE p2			// TLC_Font*
					, LC_HANDLE p3			// No Use
					, LC_HANDLE p4			// No Use
					 )
{
	INT			hr = 0;
	CLcFont*	pObj = NULL;

	*pOut		= NULL;


	pObj = new CLcFont;

	hr = pObj->CreateFromMemory(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}





