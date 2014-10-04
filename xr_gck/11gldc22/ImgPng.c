// Implementation of the Png.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../rc_cmm/04png144/png.h"
#include "ImgPng.h"



_png_int Png_FileRead(_png_byte** pOutPxl	// Output Pixel
				,	_png_dword*	pOutImgW	// Image Width
				,	_png_dword*	pOutImgH	// Image Height
				,	_png_dword*	pOutImgC	// Image Channel(byte)
				,	_png_byte*	pOutBgR		// Background Color Red
				,	_png_byte*	pOutBgG		// Background Color Green
				,	_png_byte*	pOutBgB		// Background Color Blue
				,	char* sFileName			// Image File Name
				)
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

	unsigned long	i = 0;
	int  nRead = 0;

	fp = fopen(sFileName, "rb");

	if (NULL == fp)
		return -1;


	// first check the eight byte PNG signature
	nRead = fread(pbSig, 1, 8, fp);

	//if (!png_check_sig(pbSig, 8))
	if (!!png_sig_cmp(pbSig, 0, 8))
		goto LOAD_IMAGE_ERROR;

	// create the two png(-info) structures
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL
							, (png_error_ptr)NULL, (png_error_ptr)NULL);

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
		, (png_uint_32 *)pOutImgW
		, (png_uint_32 *)pOutImgH
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

		if(pOutBgR)	*pOutBgR	= (png_byte) pBackground->red;
		if(pOutBgG)	*pOutBgG	= (png_byte) pBackground->green;
		if(pOutBgB)	*pOutBgB	= (png_byte) pBackground->blue;
	}

	// if required set gamma conversion
	if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
		png_set_gamma(png_ptr, (double) 2.2, dGamma);

	// after the transformations have been registered update info_ptr data
	png_read_update_info(png_ptr, info_ptr);

	// get again width, height and the new bit-depth and color-type
	png_get_IHDR(png_ptr
		, info_ptr
		, (png_uint_32 *)pOutImgW
		, (png_uint_32 *)pOutImgH
		, &iBitDepth
		, &iColorType
		, NULL, NULL, NULL);


	// row_bytes is the width x number of channels
	ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
	ulChannels = png_get_channels(png_ptr, info_ptr);


	// Setup Channel
	*pOutImgC= ulChannels;


	// now we can allocate memory to store the image
	pPixel = (png_byte*)malloc(ulRowBytes * (*pOutImgH) * sizeof(png_byte));

	if(NULL == pPixel)
		goto LOAD_IMAGE_ERROR;


	// and allocate memory for an array of row-pointers
	ppbRowPointers = (png_bytepp)malloc( (*pOutImgH) * sizeof(png_bytep));

	if( NULL == ppbRowPointers)
		goto LOAD_IMAGE_ERROR;




	// set the individual row-pointers to point at the correct offsets
	for(i = 0; i < (*pOutImgH); i++)
		ppbRowPointers[i] = pPixel + i * ulRowBytes;


	// now we can go ahead and just read the whole image
	png_read_image(png_ptr, ppbRowPointers);

	// read the additional chunks in the PNG file (not really needed)
	png_read_end(png_ptr, NULL);

	// yepp, done
	fclose (fp);



	// and we're done
	free(	ppbRowPointers	);
	ppbRowPointers = NULL;

	// free
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);


	// Setup Output Data
	*pOutPxl = pPixel;

	return 0;

LOAD_IMAGE_ERROR:

	fclose(fp);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	free(	ppbRowPointers	);
	ppbRowPointers = NULL;


	return -1;
}


static void 
png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	//	png_size_t check;

	/* fread() returns 0 on error, so it is OK to store this in a png_size_t
	* instead of an int, which is what fread() actually returns.
	*/

	char* it = NULL;
	
	it = *((char**)png_ptr->io_ptr);

	memcpy(data, it, length);

	it += length;

	*((char**)png_ptr->io_ptr) = it;


	//if (check != length)
	//{
	//	png_error(png_ptr, "Read Error");
	//}
}



_png_int Png_MemRead(_png_byte** pOutPxl	// Output Pixel
				,	_png_dword*	pOutImgW	// Image Width
				,	_png_dword*	pOutImgH	// Image Height
				,	_png_dword*	pOutImgC	// Image Channel(byte)
				,	_png_byte*	pOutBgR		// Background Color Red
				,	_png_byte*	pOutBgG		// Background Color Green
				,	_png_byte*	pOutBgB		// Background Color Blue
				,	void*       Memory		// Memory buffer
				,	int         MemSize		// Memory Size
				)
{
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

	unsigned long	i = 0;


	char*		pSrc = (char*)Memory;
	char*		it	 = pSrc;
	//long		lSize= MemSize;



	// first check the eight byte PNG signature
	memcpy(pbSig, it, 8);		it += 8;

	//if (!png_check_sig(pbSig, 8))
	if (!!png_sig_cmp(pbSig, 0, 8))
		goto LOAD_IMAGE_ERROR;

	// create the two png(-info) structures
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, (png_error_ptr)NULL, (png_error_ptr)NULL);

	if (!png_ptr)
		goto LOAD_IMAGE_ERROR;


	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr)
		goto LOAD_IMAGE_ERROR;


	// initialize the png structure
	png_set_read_fn(png_ptr, (png_voidp)&it, png_read_data);



	png_set_sig_bytes(png_ptr, 8);


	// read all PNG info up to image data
	png_read_info(png_ptr, info_ptr);

	// get width, height, bit-depth and color-type
	png_get_IHDR(png_ptr
		, info_ptr
		, (png_uint_32 *)pOutImgW
		, (png_uint_32 *)pOutImgH
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

		if(pOutBgR)	*pOutBgR	= (png_byte) pBackground->red;
		if(pOutBgG)	*pOutBgG	= (png_byte) pBackground->green;
		if(pOutBgB)	*pOutBgB	= (png_byte) pBackground->blue;
	}

	// if required set gamma conversion
	if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
		png_set_gamma(png_ptr, (double) 2.2, dGamma);

	// after the transformations have been registered update info_ptr data
	png_read_update_info(png_ptr, info_ptr);

	// get again width, height and the new bit-depth and color-type
	png_get_IHDR(png_ptr
		, info_ptr
		, (png_uint_32 *)pOutImgW
		, (png_uint_32 *)pOutImgH
		, &iBitDepth
		, &iColorType
		, NULL, NULL, NULL);


	// row_bytes is the width x number of channels
	ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
	ulChannels = png_get_channels(png_ptr, info_ptr);


	// Setup Channel
	*pOutImgC= ulChannels;


	// now we can allocate memory to store the image
	pPixel = (png_byte*)malloc(ulRowBytes * (*pOutImgH) * sizeof(png_byte));

	if(NULL == pPixel)
		goto LOAD_IMAGE_ERROR;


	// and allocate memory for an array of row-pointers
	ppbRowPointers = (png_bytepp)malloc( (*pOutImgH) * sizeof(png_bytep));

	if( NULL == ppbRowPointers)
		goto LOAD_IMAGE_ERROR;




	// set the individual row-pointers to point at the correct offsets
	for(i = 0; i < (*pOutImgH); i++)
		ppbRowPointers[i] = pPixel + i * ulRowBytes;


	// now we can go ahead and just read the whole image
	png_read_image(png_ptr, ppbRowPointers);

	// read the additional chunks in the PNG file (not really needed)
	png_read_end(png_ptr, NULL);

	// yepp, done
	//fclose (fp);



	// and we're done
	free(	ppbRowPointers	);
	ppbRowPointers = NULL;

	// free
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);


	// Setup Output Data
	*pOutPxl = pPixel;

	return 0;

LOAD_IMAGE_ERROR:

	//fclose(fp);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	free(	ppbRowPointers	);
	ppbRowPointers = NULL;


	return -1;
}



