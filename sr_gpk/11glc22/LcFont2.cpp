// Implementation of the ILC_Font.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../rc_cmm/05ftf243/include/ft2build.h"
#include "../../rc_cmm/05ftf243/include/freetype/freetype.h"

#if defined(__BADA__)
  #include <FBaseSys.h>
#endif

#include <LcType.h>
#include <LcMath.h>

#include "LcBase.h"
#include "LcAux.h"

#include "LcFont.h"


struct Tftt_string
{
	int		w;
	int		h;
	int		p;		// Pitch
	BYTE*	b;

	int		of_l;	// Offset Left
	int		of_t;	// Offset Left
    int		of_h;	// horiBearingY
	int		of_x;	// Offset Screen X

	Tftt_string* N;	// Next Pointer


	Tftt_string() : w(0), h(0), p(0), b(NULL), of_l(0), of_t(0), of_h(0), of_x(0), N(NULL){}

	Tftt_string(  int W, int H, int P, BYTE* B
				, int off_l, int off_t, int off_h, int nOff)
				: w(W), h(H), p(P), b(B)
				, of_l(off_l), of_t(off_t), of_h(off_h), of_x(nOff), N(NULL){}

	Tftt_string* FindTail()
	{
		if( N)
			return N->FindTail();

		return this;
	}


	void AddTail(Tftt_string* v)
	{
		Tftt_string* pTail = FindTail();

		pTail->Insert(v);
	}

	void Insert(Tftt_string* v)
	{
		Tftt_string* _t = N;

		this->N = v;

		v->N = _t;

	}
};


extern int            _LcFnt_Han2UniAll(char *src, unsigned short *dst);
extern int            _LcFnt_Han2UniLine(char *src, unsigned short *dst);
extern long           _LcFnt_DefaultFontSize();
extern unsigned char* _LcFnt_DefaultFontBuffer();

INT   _LcFnt_SetupTTFImageChain(Tftt_string** pOut, MCHAR* sDst, void* pFontFace, INT FontHeight, INT wsLen);
void* _LcFnt_CreateFTFaceFromFile(char* Name);
void* _LcFnt_CreateFTFaceFromBuffer(char* Name, unsigned char** buf, int len);
void* _LcFnt_CreateFTFaceDefault();
void  _LcFnt_RemoveFTFace(void*);
void  _LcFnt_RemoveAllFTFace();


// wcslen() is not appropriate. and windows system and linux so, we define new function
LC_INLINE INT LC_wcslen(const MCHAR* w)
{
	const MCHAR *s = w;
	while( *s++ ) ;

	return( (INT)(s - w - 1) );
}







void CLcFont::Destroy()
{
	if(NULL == m_FtFcr)
		return;

	SAFE_RELEASE     ( m_pTex  );
	SAFE_DELETE_ARRAY( m_Str   );
	SAFE_DELETE_ARRAY( m_ChBgn );

	if(m_FtFcr)
	{
		if(m_FtFcr != CLcFont::m_FTTdef)
			_LcFnt_RemoveFTFace( m_FtFcr );

		m_FtFcr = NULL;
	}


	--CLcFont::m_RefFont;
	if(0 == CLcFont::m_RefFont)
		LcDev_TFFontClose();
}


INT CLcFont::CreateFromFile(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	TLC_FONT*	ptFont	= NULL;
	char*		sFile	= NULL;
	char		sMedia[LC_MAX_PATH2]={0};

	if(!p2)
		return LC_EFAIL;


	m_pSpm	= (PLC_SPRITE)p1;
	ptFont	= (TLC_FONT*)p2;
	sFile	= ptFont->sName;


	if(LC_FAILED(LcSys_ResourcePath(sMedia, sFile)))
		return LC_CANNT_FIND_RESOURCE;


	m_FntOrgH = ptFont->iHeight;
	m_FntDstH = (INT)(m_FntOrgH * m_Sclup);

	if(p3)
	{
		INT v	= *((INT* )p2);
		if( 0<v && v <= 1024)
			m_resH	= v;
	}

	if(p4)
	{
		INT v	= *((INT* )p3);
		if( 0<v && v <= 1024)
			m_resV	= v;
	}


	if(0 > (CLcFont::m_RefFont+1))
	{
		LOGE("The reference count of font instance is out of bound.\n");
		return LC_EFAIL;
	}

	if(0 == CLcFont::m_RefFont)
	{
		if(LC_FAILED(LcDev_TFFontOpen()))
		{
			LOGE("Err: %d, %s \t", __LINE__, __FILE__);
			LOGE("LcDev_TFFontOpen() Failed\n");
			return LC_EFAIL;
		}
	}


	//	Load Font Face
	m_FtFcr = (FT_Face)_LcFnt_CreateFTFaceFromFile(sMedia);
	if(NULL == m_FtFcr)
	{
		if(0 == -CLcFont::m_RefFont)
			LcDev_TFFontClose();

		return LC_EFAIL;
	}



	++CLcFont::m_RefFont;

	return LC_OK;
}



INT CLcFont::CreateFromMemory(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	TLC_FONT*		ptFont	= NULL;
	char*			RscName	= NULL;
	unsigned char**	RscBuf	= NULL;
	INT				RscLen	= 0;

	if(!p2)
		return LC_EFAIL;

	m_pSpm	= (PLC_SPRITE)p1;
	ptFont	= (TLC_FONT*)p2;

	RscName	= ptFont->sName;
	RscBuf	= ptFont->pBuf;
	RscLen	= ptFont->iLen;

	m_FntOrgH = ptFont->iHeight;
	m_FntDstH = (INT)(m_FntOrgH * m_Sclup);


	if(p3)
	{
		INT v	= *((INT* )p2);
		if( 0<v && v <= 1024)
			m_resH	= v;
	}

	if(p4)
	{
		INT v	= *((INT* )p3);
		if( 0<v && v <= 1024)
			m_resV	= v;
	}



	if(0 != LcStr_Stricmp(LCCMD_FONT_DEFAULT, RscName) )
	{
		if(NULL == *RscBuf || 0 == RscLen)
		{
			LOGE("Err: CreateFromMemory(). The font memory is NULL or size is zero.\n");
			return LC_EFAIL;
		}
	}



	if(0 > (CLcFont::m_RefFont+1))
	{
		LOGE("The reference count of font instance is out of bound.\n");
		return LC_EFAIL;
	}

	if(0 == CLcFont::m_RefFont)
	{
		if(LC_FAILED(LcDev_TFFontOpen()))
		{
			LOGE("Err: %d, %s \t", __LINE__, __FILE__);
			LOGE("LcDev_TFFontOpen() Failed\n");
			return LC_EFAIL;
		}
	}


	//	Load Font Face

	if(0 != LcStr_Stricmp(LCCMD_FONT_DEFAULT, RscName) )
		m_FtFcr = (FT_Face)_LcFnt_CreateFTFaceFromBuffer(RscName, RscBuf, RscLen);
	else
		m_FtFcr = (FT_Face)_LcFnt_CreateFTFaceDefault();


	if(NULL == m_FtFcr)
	{
		if(0 == -CLcFont::m_RefFont)
			LcDev_TFFontClose();

		return LC_EFAIL;
	}


	++CLcFont::m_RefFont;

	return LC_OK;
}



INT CLcFont::BuildTexture()
{
	INT		hr = LC_EFAIL;
	INT		len = INT( (m_StrLen+7)/4) * 4;	// resize length
	MCHAR*	sDst=NULL;

	if(NULL == m_Str)
		return LC_EFAIL;


	sDst = new MCHAR[len];
	memset(sDst, 0, sizeof(MCHAR) * len);


//	printf("%s\n", m_sSrc);

	// Multi Byte to Unicode
	//
//#if defined(_WIN32)
//
//	INT iLen = strlen(sSrc);
//	MultiByteToWideChar( CP_ACP, 0, sSrc, -1, sDst, iLen );
//
//#else

	//_LcFnt_Han2UniAll(m_sSrc, sDst);
	sDst[0] = 0xD55C;	// "한충"
	sDst[1] = 0xCDA9;
	_LcFnt_Han2UniLine(m_Str, &sDst[2]);

//#endif



	// setup character size
	int	Height_f = m_FntDstH * 64;
	if( 0 != FT_Set_Char_Size((FT_Face)m_FtFcr, 0, Height_f, m_resH, m_resV))
	{
		delete [] sDst;
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("FT_Set_Char_Size() Failed\n");
		return LC_EFAIL;
	}


	// make Chain
	Tftt_string* pHead	= NULL;
	Tftt_string* pObj	= NULL;

	INT wsLen = LC_wcslen(sDst);
	INT h_w = _LcFnt_SetupTTFImageChain(&pHead, sDst, m_FtFcr, m_FntDstH, wsLen);
	delete [] sDst;

	if(0>h_w)
		return LC_EFAIL;


	m_ChCnt = wsLen -2;

	delete [] m_ChBgn;
	m_ChBgn = new FLOAT[m_ChCnt+2];
	memset(m_ChBgn, 0, (m_ChCnt+2) * sizeof(FLOAT) );


	// Create string buffer
	INT MaxWidth = (0x0000FFFF & h_w);
	INT MaxHeight= (0x00000FFF & h_w>>16);

	UINT nImgW	= ((MaxWidth + 7)/4) * 4;			// make multiple of 4
	UINT nImgH	= ((MaxHeight+15)/4) * 4;			// make multiple of 4

	INT nTexW	= LCXQuantum2Power(nImgW+4);		// Texture Width. make power of 2
	INT	nTexH	= LCXQuantum2Power(nImgH);			// Texture Height. make power of 2

	INT	nOffy	= nTexH-MaxHeight+1;				// Offset Y
	INT nPxlBuf = nTexH * nTexW;


	// Temp buffer for texture pixel of string
	BYTE* pPxlSrc = NULL;							// pixel from TTF Library
	BYTE* pPxlAnt = NULL;							// Applied Anti-Aliasing Pixel
	BYTE* pPxlFnt = NULL;							// for finale font texture pixel

	pPxlSrc = new BYTE[nPxlBuf];					// temp buffer for font chain
	memset(pPxlSrc, 0x0, sizeof(BYTE) * nPxlBuf);	// init to 0x0

	pPxlFnt	= pPxlSrc;								// first, set the font pixel to Anti-Aliasing pixel



	// gether pixel from font Chain
	pObj = pHead->N;

	INT x, y, t, n =0;
	INT	nOffsetX, nOffsetY, nSrc;
	INT ColorDst;

	int color=0;
	int width=0, height=0, pitch=0;
	int	bgn_x=0;
	int bgn_y=0;
	FLOAT rcW = 0.0F;
	BYTE* buf=NULL;

	pObj = pObj->N;		// skip "한"
	pObj = pObj->N;		// skip "충"

	while(pObj)
	{
		width = pObj->w;
		height= pObj->h;
		pitch = pObj->p;
		buf	  = pObj->b;

		bgn_x = pObj->of_x + pObj->of_l;
		bgn_y = nOffy      + pObj->of_t;

		m_ChBgn[n++] = (FLOAT)bgn_x;		// save start position


		// one character
		for(y=0;y<height; ++y)
		{
			for(x=0;x<width; ++x)
			{
				BYTE Mono = buf[y*pitch+ x/8];

				t = x%8;
				color = (Mono >>(7-t)) & 0x01;

				if(0 == color)
					continue;


				nOffsetX = bgn_x + x;
				nOffsetY = bgn_y - y;


				nSrc = nTexW * nOffsetY + nOffsetX;

				// Out of Buffer
				if(nSrc<0 || nSrc >=nPxlBuf)
					continue;

				pPxlSrc[nSrc] = 0xFF;
			}
		}

		pObj = pObj->N;
	}

	// the last...
	//m_ChBgn[n] = (FLOAT)nImgW;
	rcW  = bgn_x + width + 1.6F * m_Sclup;
	m_ChBgn[n] = rcW;


	// apply to Anti-Aliasing for Font height over 14
	FLOAT _0, _1, _2, _3;										// 4-con sampling index
	FLOAT _4, _5, _6, _7;										// 8-con sampling index
	FLOAT _8;													// 9-con sampling index
	FLOAT _tmp;


	if(18 <= m_FntDstH)
	{
		pPxlAnt = new BYTE[nPxlBuf];
		memset(pPxlAnt, 0x00, sizeof(BYTE) * nPxlBuf);		// init to 0x0
		pPxlFnt	= pPxlAnt;									// set pixel to Anitialiasing pixel

		for(y=1;y<nTexH-1; ++y)
		{
			for(x=1;x<nTexW-1; ++x)
			{
				nSrc = nTexW * y + x;

				ColorDst = pPxlSrc[nSrc];

				// 4-con sampling
				//if(0x00 == ColorDst)
				/*{
					_0 = pPxlSrc[nTexW * (y-1) + x+0];
					_1 = pPxlSrc[nTexW * (y+0) + x-1];
					_2 = pPxlSrc[nTexW * (y+0) + x+1];
					_3 = pPxlSrc[nTexW * (y+1) + x+0];

					_8 = pPxlSrc[nTexW * (y+0) + x+0];


					_tmp  = (_0 + _1 + _2  +_3)*3.0F;
					_tmp += _8 * 6.0F;
					_tmp /= 18.0F;

					ColorDst = int(_tmp);

					if(255<ColorDst)
						ColorDst = 255;
				}*/

				//9-con sampling
				{
					_0 = pPxlSrc[nTexW * (y-1) + x+0];
					_1 = pPxlSrc[nTexW * (y+0) + x-1];
					_2 = pPxlSrc[nTexW * (y+0) + x+1];
					_3 = pPxlSrc[nTexW * (y+1) + x+0];

					_4 = pPxlSrc[nTexW * (y-1) + x-1];
					_5 = pPxlSrc[nTexW * (y-1) + x+1];
					_6 = pPxlSrc[nTexW * (y+1) + x-1];
					_7 = pPxlSrc[nTexW * (y+1) + x+1];

					_8 = pPxlSrc[nTexW * (y+0) + x+0];

					_tmp  = (_0 + _1 + _2  +_3) * 15.0F;
					_tmp += (_4 + _5 + _6  +_7) * 10.0F;
					_tmp += _8 * 25.0F;
					_tmp /= 125.0F;

					ColorDst = int(_tmp);

					if(255<ColorDst)
						ColorDst = 255;
				}

				pPxlAnt[nSrc] = ColorDst;
			}
		}

	}


	// save variable and create texture.
	PLC_TEXTURE pTexNew = NULL;
	TLC_IMAGE tex_inf=
	{
		0x1906				// GL_ALPHA
		, 0x1401			// GL_UNSIGNED_BYTE
		, nTexW
		, nTexH
		, 1
		, 0x0
		, pPxlFnt
	};


	hr = LcDev_CreateTextureFromBuffer(NULL, &pTexNew, &tex_inf);

	// finally, setup the draw region
	//nImgH = (nImgH * 10)/8;
	m_Rc = LCXRECT(0,0, rcW, (FLOAT)nImgH);


	SAFE_RELEASE(	m_pTex	);		// release current texture and new create texture

	m_pTex = pTexNew;				// setup the new point


	// release temp buffer
	SAFE_DELETE_ARRAY(	pPxlSrc	);
	SAFE_DELETE_ARRAY(	pPxlAnt	);


	// release Chain
	pObj = pHead->N;
	while(pObj)
	{
		Tftt_string* tt = pObj;
		pObj = pObj->N;

		if(tt->b)
			free(tt->b);

		delete tt;
	}

	delete pHead;

	return hr;
}





////////////////////////////////////////////////////////////////////////////////

INT	 LcDev_TFFontOpen()		// Free Type Font Library Open
{
	if(CLcFont::m_FTTontLib)
		return LC_OK;

	if( FT_Init_FreeType((FT_Library*)&CLcFont::m_FTTontLib))
		return LC_CANNT_ALLOC;


	return LC_OK;
}


INT LcDev_TFFontClose()		// FreeTypeFont Liberay Close
{
	if(NULL == CLcFont::m_FTTontLib)
		return LC_OK;

	_LcFnt_RemoveAllFTFace();


	if(CLcFont::m_FTTdef)
	{
		FT_Done_Face((FT_Face)CLcFont::m_FTTdef );
		CLcFont::m_FTTdef = NULL;
	}


	FT_Done_FreeType((FT_Library)CLcFont::m_FTTontLib);
	CLcFont::m_FTTontLib = NULL;

	return LC_OK;
}




struct TFTface
{
	char	Name[LC_DEFAULT_NAME];
	FT_Face Font;
	int		Ref;

	TFTface()
	{
		memset(Name, 0, sizeof Name);
		Font = NULL;
		Ref = 1;
	}

	TFTface(char* s, FT_Face p)
	{
		memset(Name, 0, sizeof Name);
		strcpy(Name, s);
		Font  = p;
		Ref = 1;
	}
};


struct TFTbuff
{
	char			Name[LC_DEFAULT_NAME];
	unsigned char*	buf;
	int				len;
	int				Ref;

	TFTbuff()
	{
		memset(Name, 0, sizeof Name);
		buf = NULL;
		len = 0;
		Ref = 1;
	}

	TFTbuff(char* s, unsigned char* _b, int _l)
	{
		memset(Name, 0, sizeof Name);
		strcpy(Name, s);
		buf = (unsigned char*)_b;
		len = _l;
		Ref = 1;
	}
};


static std::vector<TFTface > g_vFTface;
static std::vector<TFTbuff > g_vFTbuff;



TFTbuff* _LcFnt_FindBuff(char* Name)
{
	int iSize = g_vFTbuff.size();

	for(int i=0; i<iSize; ++i)
	{
		TFTbuff* pTftB = &g_vFTbuff[i];

		if(0 == LcStr_Stricmp(Name, pTftB->Name) )
			return pTftB;
	}
	return NULL;
}


TFTface* _LcFnt_FindFTFace(char* Name)
{
	int iSize = g_vFTface.size();

	for(int i=0; i<iSize; ++i)
	{
		TFTface* pFace = &g_vFTface[i];
		if(0 == LcStr_Stricmp(Name, pFace->Name))
			return pFace;
	}

	return NULL;
}


void _LcFnt_RemoveFTFace(void* FontRc)
{
	if(NULL == FontRc)
		return;


	int iSize = g_vFTface.size();

	for(int i=0; i<iSize; ++i)
	{
		TFTface* pFace = &g_vFTface[i];

		if(pFace->Font == FontRc)
		{
			--pFace->Ref;

			if(0 >= pFace->Ref)
			{
				FT_Done_Face((FT_Face)FontRc );
				g_vFTface.erase( g_vFTface.begin() + i);
				break;
			}
		}
	}
}



void _LcFnt_RemoveAllFTFace()
{
	int	i;
	int iSize;

	TFTface* pFace = NULL;
	TFTbuff* pBuff = NULL;

	iSize = g_vFTface.size();

	for(i=0; i<iSize; ++i)
	{
		pFace = &g_vFTface[i];
		if(NULL == pFace->Font)
			continue;

		FT_Done_Face((FT_Face)pFace->Font );
	}

	g_vFTface.clear();



	iSize = g_vFTbuff.size();

	for(i=0; i<iSize; ++i)
	{
		pBuff = &g_vFTbuff[i];
		if(NULL == pBuff->buf)
			continue;

		free(pBuff->buf);
	}

	g_vFTbuff.clear();
}



void* _LcFnt_CreateFTFaceFromFile(char* Name)
{
	FT_Face  FontRc = NULL;
	TFTface* _t = _LcFnt_FindFTFace(Name);

	if(_t)
	{
		FontRc = _t->Font;
		++_t->Ref;

		return FontRc;
	}



	//	Load Font Face
	if( 0 != FT_New_Face((FT_Library)CLcFont::m_FTTontLib, Name, 0, &FontRc))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("FT_New_Face() Failed\n");
		return NULL;
	}


	TFTface tface = TFTface(Name, FontRc);
	g_vFTface.push_back(tface);

	return FontRc;
}




void* _LcFnt_CreateFTFaceDefault()
{
	long			len = _LcFnt_DefaultFontSize();
	unsigned char*	buf = _LcFnt_DefaultFontBuffer();


	//	Load default font face
	if(NULL == CLcFont::m_FTTdef)
	{
		if( FT_New_Memory_Face((FT_Library)CLcFont::m_FTTontLib, buf, len, 0, (FT_Face*)&CLcFont::m_FTTdef))
		{
			LOGE("Err: %d, %s \t", __LINE__, __FILE__);
			LOGE("FT_New_Face() Failed\n");
			return NULL;
		}
	}

	return CLcFont::m_FTTdef;
}






void* _LcFnt_CreateFTFaceFromBuffer(char* Name, unsigned char** buf, int len)
{
	TFTbuff*		pTftB = NULL;
	unsigned char*	pDstB = NULL;

	FT_Face		FontRc = NULL;
	TFTface*    _t = _LcFnt_FindFTFace(Name);

	if(_t)
	{
		FontRc = _t->Font;
		++_t->Ref;

		return FontRc;
	}


	pDstB = *buf;


	// find from resource
	pTftB = _LcFnt_FindBuff(Name);
	if(NULL == pTftB && *buf&& 0 <len)	// 새로운 버퍼 추가
	{
		TFTbuff tbuff = TFTbuff(Name, *buf, len);
		g_vFTbuff.push_back(tbuff);
	}
	else if(*buf == pTftB->buf)			// 같은 버퍼. ref 증가 시킴.
	{
		++pTftB->Ref;
	}
	else if(*buf != pTftB->buf)			// 이름이 같은 다름 버퍼. 새로운 버퍼는 해제시킴
	{
		++pTftB->Ref;

		if(*buf)
		{
			free(*buf);
			*buf = NULL;
		}

		pDstB = pTftB->buf;				// dest buffer는 저장된 버퍼
		len	  = pTftB->len;				// dest length 도 저장된 길이
	}



	if(NULL == pDstB || 0 >= len)
		return NULL;


	//	Load Font Face
	if( 0 != FT_New_Memory_Face((FT_Library)CLcFont::m_FTTontLib, pDstB, len, 0, &FontRc))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("FT_New_Face() Failed\n");
		return NULL;
	}


	g_vFTface.push_back(TFTface(Name, FontRc) );

	return FontRc;
}




INT _LcFnt_SetupTTFImageChain(Tftt_string** pOut, MCHAR* sDst, void* pFontFace, INT FontHeight, INT wsLen)
{
	INT		hr;
	INT		n;
	UINT	index=0;
	INT		offsetX = 0;
	INT		MaxHeight= 0;

	INT		iLen = wsLen;

	FT_Face		 FtFace= (FT_Face)pFontFace;
	Tftt_string* pHead = NULL;


	if(iLen<1)
		return LC_EFAIL;


	pHead = new Tftt_string;


	for(n=0; n<iLen; ++n)
	{
		// Glyph index
		index = FT_Get_Char_Index(FtFace,sDst[n]);	// Error: index ==> 0


		// Glyph load(FtFace->glyph->bitmap.buffer is released in FT_Load_Glyph() function)
//		hr=FT_Load_Glyph(FtFace,index,FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP | FT_LOAD_NO_AUTOHINT);
		hr=FT_Load_Glyph(FtFace,index,FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP | FT_LOAD_MONOCHROME| FT_LOAD_NO_AUTOHINT);


		// Rendering(FtFace->glyph->bitmap.buffer created FT_Render_Glyph() with malloc)
//		hr=FT_Render_Glyph(FtFace->glyph, FT_RENDER_MODE_NORMAL);
		hr=FT_Render_Glyph(FtFace->glyph, FT_RENDER_MODE_MONO);


		// output to sreen
		int width, height;
		width =FtFace->glyph->bitmap.width;
		height=FtFace->glyph->bitmap.rows;

		MaxHeight = lc_max(MaxHeight, height);	// Make Max Height

		width += FtFace->glyph->bitmap_left;

		if(0 == width)		// Characters such as spaces for memory is 0 so just increase the width to the proper value
			width = int(FontHeight *0.333F);

		width += int(FontHeight*0.1 + 2);


		Tftt_string* pObj = new Tftt_string(FtFace->glyph->bitmap.width
											, FtFace->glyph->bitmap.rows
											, FtFace->glyph->bitmap.pitch
											, FtFace->glyph->bitmap.buffer
											, FtFace->glyph->bitmap_left
											, FtFace->glyph->bitmap_top
											, FtFace->glyph->metrics.horiBearingY/64
											, offsetX
											);
		pHead->AddTail(pObj);

		if(2 <= n)							// skip two characters
			offsetX += width;				// increase Offset

		FtFace->glyph->bitmap.buffer = NULL;	// set to null to prevent release memory in FT_Load_Glyph()
	}

	*pOut = pHead;

	hr =  (0x0FFF0000 & MaxHeight<<16) + (0x0000FFFF & offsetX);	// setup Height, Offset values

	return hr;
}

