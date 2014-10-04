// Implementation of the CLcImage class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <windows.h>
#include <stdio.h>
#include <malloc.h>

#include "../../rc_cmm/04png144/png.h"


#include "ImgTga.h"

#include "LnD3Type.h"
#include "LnBase.h"
#include "LnImage.h"





#define SAFE_FREE(p)		 {if(p) { free(p);		 (p) = NULL; } }
#define SAFE_DELETE(p)		 {if(p) { delete (p);	 (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p) {if(p) { delete [] (p); (p) = NULL; } }







INT LcDev_CreateImage(char* sCmd
					, ILcImage** pData
					, void* p1		// File Name
					, void* p2		// Not Use
					, void* p3		// Not Use
					, void* p4		// Not Use
					)
{
	CLcImage* pObj	= NULL;
	(*pData) = NULL;

	pObj	= new CLcImage;

	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;
	return 0;
}










CLcImage::PNG_PIXEL::PNG_PIXEL()
{
	pPixel	= NULL;
	nWidth	= 0;
	nHeight	= 0;
	nChannel= 0;

	BgColorR= 0;
	BgColorG= 0;
	BgColorB= 0;
}

CLcImage::PNG_PIXEL::PNG_PIXEL(const PNG_PIXEL& r)
{
	SAFE_FREE(	pPixel	);

	pPixel	= r.pPixel	;
	nWidth	= r.nWidth	;
	nHeight	= r.nHeight	;
	nChannel= r.nChannel;

	BgColorR= r.BgColorR;
	BgColorG= r.BgColorG;
	BgColorB= r.BgColorB;
}

CLcImage::PNG_PIXEL& CLcImage::PNG_PIXEL::operator=(const PNG_PIXEL& r)
{
	SAFE_FREE(	pPixel	);

	pPixel	= r.pPixel	;
	nWidth	= r.nWidth	;
	nHeight	= r.nHeight	;
	nChannel= r.nChannel;

	BgColorR= r.BgColorR;
	BgColorG= r.BgColorG;
	BgColorB= r.BgColorB;

	return *this;
}

void CLcImage::PNG_PIXEL::Destroy()
{
	SAFE_FREE(	pPixel	);
}

void CLcImage::PNG_PIXEL::SetPixelData(BYTE* _pxl)
{
	SAFE_FREE(	pPixel	);
	pPixel	= _pxl;
}




CLcImage::CLcImage()
{
	m_ImgB	= 0;
	m_ImgW	= 0;
	m_ImgH	= 0;
	m_ImgD	= 0;
	m_ImgF	= 0;
	m_Data	= 0;
	memset(m_sName, 0, sizeof m_sName);
}

CLcImage::~CLcImage()
{
	Destroy();
}


void CLcImage::Destroy()
{
	if(m_Data)
	{
		delete m_Data;
		m_Data = NULL;
	}
}


INT CLcImage::Create(void* p1, void*, void*, void*)
{
	char*	sFile	= (char*)(p1);
	char*	sExt	= strrchr(sFile, '.');

	strcpy(m_sName, sFile);
	sExt	= strrchr(sFile, '.');


	if(0 == _stricmp(sExt, "png"))
	{
		PNG_PIXEL	m_Png;
		INT			nSizePxl;
		INT			n;

		BYTE*		pSrc = NULL;
		BYTE*		pDst = NULL;

		if(SUCCEEDED(LoadPngFile(&m_Png, sFile)))
		{
			nSizePxl = m_Png.nWidth * m_Png.nHeight;

			pDst = new BYTE[nSizePxl * 4];
			pSrc = (BYTE*)m_Png.pPixel;

			// for 32 or 24 bit
			// R,G, B to B, G, R
			// PNG Order: r, g, b, a
			// Window, B, G, R
			for(n = 0; n < nSizePxl; ++n)
			{
				int idx1 =  n * 4;
				int idx2 =  n * m_Png.nChannel;

				pDst[idx1+0] = pSrc[idx2 + 2];
				pDst[idx1+1] = pSrc[idx2 + 1];
				pDst[idx1+2] = pSrc[idx2 + 0];

				if(4 == m_Png.nChannel)
					pDst[idx1+3] = pSrc[idx2 + 3];
				else
					pDst[idx1+3] = 0xFF;

			}

			// ÇÈ¼¿ º¹»ç
			m_Png.Destroy();
		}
		else
			return -1;

		this->m_ImgW = m_Png.nWidth;
		this->m_ImgH = m_Png.nHeight;
		this->m_ImgD = 0;
		this->m_ImgB = 32;
		this->m_ImgF = LCIMG_FMT_PNG;
		this->m_Data = pDst;
	}

	else if(0 == _stricmp(sExt, "bmp"))
	{
	}

	else if(0 == _stricmp(sExt, "jpg") || 0 == _stricmp(sExt, "jpeg"))
	{
	}

	else
	{
		return -1;
	}

	return 0;
}


INT CLcImage::Query(char*, void*)
{
	return -1;
}




INT CLcImage::LoadPngFile(CLcImage::PNG_PIXEL* pPngOut, char* sFileName)
{
	FILE*			fp;
	png_byte        pbSig[8];
	int             iBitDepth;
	int             iColorType;
	double          dGamma;
	png_color_16*	pBackground;
	png_uint_32		ulChannels;
	png_uint_32		ulRowBytes;
	png_byte*		pPixel		= NULL;
	png_byte**		ppbRowPointers = NULL;

	png_structp		png_ptr = NULL;
	png_infop		info_ptr = NULL;

	int i = 0;

	fp = fopen(sFileName, "rb");

	if (NULL == fp)
		return -1;


	// first check the eight byte PNG signature
	fread(pbSig, 1, 8, fp);

	if (!png_check_sig(pbSig, 8))
		goto LOAD_IMAGE_ERROR;

	// create the two png(-info) structures
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, (png_error_ptr)NULL, (png_error_ptr)NULL);

	if (!png_ptr)
		goto LOAD_IMAGE_ERROR;


	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr)
		goto LOAD_IMAGE_ERROR;


	// initialize the png structure
	png_set_read_fn(png_ptr, (png_voidp)fp, NULL);



	png_set_sig_bytes(png_ptr, 8);


	// read all PNG info up to image data
	png_read_info(png_ptr, info_ptr);

	// get width, height, bit-depth and color-type
	png_get_IHDR(png_ptr
		, info_ptr
		, (png_uint_32 *)&pPngOut->nWidth
		, (png_uint_32 *)&pPngOut->nHeight
		, &iBitDepth
		, &iColorType
		, NULL, NULL, NULL);

	// expand images of all color-type and bit-depth to 3x8 bit RGB images
	// let the library process things like alpha, transparency, background

	if (iBitDepth == 16)
		png_set_strip_16(png_ptr);
	
	if (iColorType == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);
	
	if (iBitDepth < 8)
		png_set_expand(png_ptr);
	
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand(png_ptr);

	if (iColorType == PNG_COLOR_TYPE_GRAY ||
		iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	// set the background color to draw transparent and alpha images over.
	if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
	{
		png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
		pPngOut->BgColorR	= (png_byte) pBackground->red;
		pPngOut->BgColorG	= (png_byte) pBackground->green;
		pPngOut->BgColorB	= (png_byte) pBackground->blue;
	}

	// if required set gamma conversion
	if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
		png_set_gamma(png_ptr, (double) 2.2, dGamma);

	// after the transformations have been registered update info_ptr data
	png_read_update_info(png_ptr, info_ptr);

	// get again width, height and the new bit-depth and color-type
	png_get_IHDR(png_ptr
		, info_ptr
		, (png_uint_32 *)&pPngOut->nWidth
		, (png_uint_32 *)&pPngOut->nHeight
		, &iBitDepth
		, &iColorType
		, NULL, NULL, NULL);


	// row_bytes is the width x number of channels
	ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
	ulChannels = png_get_channels(png_ptr, info_ptr);


	// Setup Channel
	pPngOut->nChannel= ulChannels;


	// now we can allocate memory to store the image
	pPixel = (png_byte*)malloc(ulRowBytes * pPngOut->nHeight * sizeof(png_byte));

	if(NULL == pPixel)
		goto LOAD_IMAGE_ERROR;


	// and allocate memory for an array of row-pointers
	ppbRowPointers = (png_bytepp)malloc(pPngOut->nHeight * sizeof(png_bytep));

	if( NULL == ppbRowPointers)
		goto LOAD_IMAGE_ERROR;




	// set the individual row-pointers to point at the correct offsets
	for(i = 0; i < pPngOut->nHeight; i++)
		ppbRowPointers[i] = pPixel + i * ulRowBytes;

	
	// now we can go ahead and just read the whole image
	png_read_image(png_ptr, ppbRowPointers);

	// read the additional chunks in the PNG file (not really needed)
	png_read_end(png_ptr, NULL);

	// yepp, done
	fclose (fp);


	
	// and we're done
	SAFE_FREE(	ppbRowPointers	);

	// free
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	
	// Setup Output Data
	pPngOut->pPixel = pPixel;

	return 0;

LOAD_IMAGE_ERROR:
	
	fclose(fp);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	SAFE_FREE(	ppbRowPointers	);

	return -1;
}