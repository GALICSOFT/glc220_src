// Implementation of the Bmp.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ImgBmp.h"


//	BMP Read
//	Offset Field Size	Brief
//		Contents
//
//
//	bmp_file -----------------------------------------------------------------------
//	0000h 2 byte    Identifier
//		The characters identifying the bitmap. The following entries are possible:
//		¡®BM¡¯ - Windows 3.1x, 95, NT, ¡¦
//		¡®BA¡¯ - OS/2 Bitmap Array
//		¡®CI¡¯ - OS/2 Color Icon
//		¡®CP¡¯ - OS/2 Color Pointer
//		¡®IC¡¯ - OS/2 Icon
//		¡®PT¡¯ - OS/2 Pointer
//
//	0002h 4 byte(dword)    File Size
//		Complete file size in bytes.
//
//	0006h 4 byte(dword)    Reserved
//		Reserved for later use.
//
//	000Ah 4 byte(dword)	Bitmap Data Offset
//		Offset from beginning of file to the beginning of the bitmap data.
//
//
//
//	bmp_info -----------------------------------------------------------------------
//	000Eh 4 byte(dword)	Bitmap Header Size
//		Length of the Bitmap Info Header used to describe the bitmap colors, compression, ¡¦ The following sizes are possible:
//		28h - Windows 3.1x, 95, NT, ¡¦
//		0Ch - OS/2 1.x
//		F0h - OS/2 2.x
//
//	0012h 4 byte(dword)	Width
//		Horizontal width of bitmap in pixels.
//
//	0016h 4 byte(dword)	Height
//		Vertical height of bitmap in pixels.
//
//	001Ah 2 byte(word)	Planes
//		Number of planes in this bitmap.
//
//	001Ch 2 byte(word)	Bits Per Pixel
//		Bits per pixel used to store palette entry information. This also identifies in an indirect way the number of possible colors. Possible values are:
//		1 - Monochrome bitmap
//		4 - 16 color bitmap
//		8 - 256 color bitmap
//		16 - 16bit (high color) bitmap
//		24 - 24bit (true color) bitmap
//		32 - 32bit (true color) bitmap
//
//	001Eh 4 byte(dword)	Compression
//		Compression specifications. The following values are possible:
//		0 - none (Also identified by BI_RGB)
//		1 - RLE 8-bit / pixel (Also identified by BI_RLE4)
//		2 - RLE 4-bit / pixel (Also identified by BI_RLE8)
//		3 - Bitfields  (Also identified by BI_BITFIELDS)
//
//	0022h 4 byte(dword)	Bitmap Data Size
//		Size of the bitmap data in bytes. This number must be rounded to the next 4 byte boundary.
//
//	0026h 4 byte(dword)	HResolution
//		Horizontal resolution expressed in pixel per meter.
//
//	002Ah 4 byte(dword)	VResolution
//		Vertical resolution expressed in pixels per meter.
//
//	002Eh 4 byte(dword)	Colors
//		Number of colors used by this bitmap. For a 8-bit / pixel bitmap this will be 100h or 256.
//
//	0032h 4 byte(dword)	Important Colors
//		Number of important colors. This number will be equal to the number of colors when every color is important.
//
//	0036h N * 4 byte	Palette
//		The palette specification. For every entry in the palette four bytes are used to describe the RGB values of the color in the following way:
//		1 byte for blue component
//		1 byte for green component
//		1 byte for red component
//		1 byte filler which is set to 0 (zero)
//
//	0436h x bytes	Bitmap Data
//		Depending on the compression specifications, this field contains all the bitmap data bytes which represent indices in the color palette.



bmp_int	Bmp_FileRead(bmp_byte** pOutPxl		// Output Pixel
				,	bmp_int*	pOutImgW	// Output Image Width
				,	bmp_int*	pOutImgH	// Output Image Height
				,	bmp_int*	pOutImgC	// Output Image Channel(byte)
				,	char* sFile				// Source File
				)
{
	FILE*		fp			;
	bmp_long	lSize		;
	bmp_int		nImgC		;		// Channel Count
	bmp_byte*	pPxlS = NULL;


	struct bmp_file		fh	= {0};
	struct bmp_info		ih	= {0};
	bmp_byte*			pPalette= NULL;

	int	nSize=0;
	int	i=0;
	int nRead =0;

	fp= fopen(sFile, "rb" );

	if(NULL == fp)
		return -1;

	fseek(fp, 0, SEEK_END);
	lSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if(lSize<40)
	{
		fclose(fp);
		return -1;
	}


//	fread(&fh.bfType		, 1, sizeof(bmp_word )		, fp );
//	fread(&fh.bfSize		, 1, sizeof(bmp_dword)		, fp );
//	fread(&fh.bfReserved1	, 1, sizeof(bmp_word )		, fp );
//	fread(&fh.bfReserved2	, 1, sizeof(bmp_word )		, fp );
//	fread(&fh.bfOffBits		, 1, sizeof(bmp_dword)		, fp );

//	fread(&ih				, 1, sizeof(struct bmp_info), fp );



	// File Header
	nRead = fread(&fh.bfType		, 2, 1, fp );	//	word    bfType		;
	nRead = fread(&fh.bfSize		, 4, 1, fp );	//	dword   bfSize		;
	nRead = fread(&fh.bfReserved1	, 2, 1, fp );	//	word    bfReserved1	;
	nRead = fread(&fh.bfReserved2	, 2, 1, fp );	//	word    bfReserved2	;
	nRead = fread(&fh.bfOffBits		, 4, 1, fp );	//	dword   bfOffBits	;


	// Bmp Info
	nRead = fread(&ih.biSize			, 4, 1, fp );	//	dword biSize			;
	nRead = fread(&ih.biWidth			, 4, 1, fp );	//	long  biWidth			;
	nRead = fread(&ih.biHeight			, 4, 1, fp );	//	long  biHeight			;

	nRead = fread(&ih.biPlanes			, 2, 1, fp );	//	word  biPlanes			;
	nRead = fread(&ih.biBitCount		, 2, 1, fp );	//	word  biBitCount		;
	nRead = fread(&ih.biCompression		, 4, 1, fp );	//	dword biCompression	;
	nRead = fread(&ih.biSizeImage		, 4, 1, fp );	//	dword biSizeImage		;

	nRead = fread(&ih.biXPelsPerMeter	, 4, 1, fp );	//	long  biXPelsPerMeter	;
	nRead = fread(&ih.biYPelsPerMeter	, 4, 1, fp );	//	long  biYPelsPerMeter	;
	nRead = fread(&ih.biClrUsed			, 4, 1, fp );	//	dword biClrUsed			;
	nRead = fread(&ih.biClrImportant	, 4, 1, fp );	//	dword biClrImportant	;


	nImgC	= ih.biBitCount>>3;
	nSize	= fh.bfSize;


	// Read Palette
	if(1 >= nImgC)
	{
		int num_colors = ih.biClrImportant;
		if(0 == num_colors || num_colors >256)
			num_colors = 256;

		pPalette = (bmp_byte*)malloc(num_colors * 4);
		nRead = fread(pPalette, 4, num_colors, fp);
	}


	fseek(fp, fh.bfOffBits, SEEK_SET);

	// Read Data
	pPxlS = (bmp_byte*)malloc( nSize );				// fh.bfSize is greater then (*pOutImgW) * (*pOutImgH) * nImgB.
	nRead = fread(pPxlS, 1, nSize, fp);

	fclose(fp);


	// Change Palette to 24 bit color.
	// the color byte is color index in 256 color.
	// palette has rgba color but the converting image format is rgb 24bit

	if(1 >= nImgC)
	{
		bmp_byte*	pPxIdx = pPxlS;					// change to index
		int			nIdx  = 0;

		nImgC = 3;									// not 4. setting to 24 bit
		pPxlS = (bmp_byte*)malloc(nSize * nImgC);	// re-alloc color.

		for(i=0; i<nSize; ++i)						// a Palette consists of 4 byte r,g,b,a.
		{
			nIdx = (int)(pPxIdx[i]) *4;

			pPxlS[i*nImgC + 0] = pPalette[ nIdx+0 ];
			pPxlS[i*nImgC + 1] = pPalette[ nIdx+1 ];
			pPxlS[i*nImgC + 2] = pPalette[ nIdx+2 ];
		}


		free(pPxIdx);							// release index
		free(pPalette);							// release Palette
	}


	*pOutImgW	= ih.biWidth;					// copy to output data.
	*pOutImgH	= ih.biHeight;
	*pOutImgC	= nImgC;
	*pOutPxl	= pPxlS;

	return 0;
}


bmp_int	Bmp_MemRead(bmp_byte** pOutPxl		// Output Pixel
				,	bmp_int*	pOutImgW	// Output Image Width
				,	bmp_int*	pOutImgH	// Output Image Height
				,	bmp_int*	pOutImgC	// Output Image Channel(byte)
				,	void*		Memory		// Memory buffer
				,	int			MemSize		// Memory Size
				)
{
	bmp_long	lSize = MemSize;
	bmp_int		nImgC		;		// Channel Count
	bmp_byte*	pPxlS= NULL;
	char*		pSrc = (char*)Memory;
	char*		it	 = pSrc;


	struct bmp_file		fh	= {0};
	struct bmp_info		ih	= {0};
	bmp_byte*			pPalette= NULL;

	int	nSize=0;
	int	i=0;


	if(lSize<40)
		return -1;


	// File Header
	memcpy(&fh.bfType			, it , 2);	it += 2;	//	word    bfType		;
	memcpy(&fh.bfSize			, it , 4);	it += 4;	//	dword   bfSize		;
	memcpy(&fh.bfReserved1		, it , 2);	it += 2;	//	word    bfReserved1	;
	memcpy(&fh.bfReserved2		, it , 2);	it += 2;	//	word    bfReserved2	;
	memcpy(&fh.bfOffBits		, it , 4);	it += 4;	//	dword   bfOffBits	;


	// Bmp Info
	memcpy(&ih.biSize			, it, 4);	it += 4;	//	dword biSize			;
	memcpy(&ih.biWidth			, it, 4);	it += 4;	//	long  biWidth			;
	memcpy(&ih.biHeight			, it, 4);	it += 4;	//	long  biHeight			;

	memcpy(&ih.biPlanes			, it, 2);	it += 2;	//	word  biPlanes			;
	memcpy(&ih.biBitCount		, it, 2);	it += 2;	//	word  biBitCount		;
	memcpy(&ih.biCompression	, it, 4);	it += 4;	//	dword biCompression	;
	memcpy(&ih.biSizeImage		, it, 4);	it += 4;	//	dword biSizeImage		;

	memcpy(&ih.biXPelsPerMeter	, it, 4);	it += 4;	//	long  biXPelsPerMeter	;
	memcpy(&ih.biYPelsPerMeter	, it, 4);	it += 4;	//	long  biYPelsPerMeter	;
	memcpy(&ih.biClrUsed		, it, 4);	it += 4;	//	dword biClrUsed			;
	memcpy(&ih.biClrImportant	, it, 4);	it += 4;	//	dword biClrImportant	;


	nImgC	= ih.biBitCount>>3;
	nSize	= fh.bfSize;


	// Read Palette
	if(1 >= nImgC)
	{
		int num_colors = ih.biClrImportant;
		if(0 == num_colors || num_colors >256)
			num_colors = 256;

		pPalette = (bmp_byte*)malloc(num_colors * 4);
		memcpy(pPalette, it, num_colors * 4);	it += num_colors * 4;
	}


	it = pSrc + fh.bfOffBits;

	// Read Data
	pPxlS = (bmp_byte*)malloc( nSize );				// fh.bfSize is greater then (*pOutImgW) * (*pOutImgH) * nImgB.
	memcpy(pPxlS, it, nSize);



	// Change Palette to 24 bit color.
	// the color byte is color index in 256 color.
	// palette has rgba color but the converting image format is rgb 24bit

	if(1 >= nImgC)
	{
		bmp_byte*	pPxIdx = pPxlS;					// change to index
		int			nIdx  = 0;

		nImgC = 3;									// not 4. setting to 24 bit
		pPxlS = (bmp_byte*)malloc(nSize * nImgC);	// re-alloc color.

		for(i=0; i<nSize; ++i)						// a Palette consists of 4 byte r,g,b,a.
		{
			nIdx = (int)(pPxIdx[i]) *4;

			pPxlS[i*nImgC + 0] = pPalette[ nIdx+0 ];
			pPxlS[i*nImgC + 1] = pPalette[ nIdx+1 ];
			pPxlS[i*nImgC + 2] = pPalette[ nIdx+2 ];
		}


		free(pPxIdx);							// release index
		free(pPalette);							// release Palette
	}


	*pOutImgW	= ih.biWidth;					// copy to output data.
	*pOutImgH	= ih.biHeight;
	*pOutImgC	= nImgC;
	*pOutPxl	= pPxlS;

	return 0;
}





