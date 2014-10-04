// Implementation of the CLcImg class.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "ImgPng.h"
#include "ImgBmp.h"
#include "ImgTga.h"

#include "LnImg.h"


typedef int				lc_int;
typedef unsigned int	lc_uint;
typedef unsigned char	lc_byte;
typedef unsigned long	lc_ulong;

int	LcImg_CreateImageFromFile(lc_byte** ppPxl		// Output Pixel
							, lc_uint* pnType		// Output nType
							, lc_uint* pnImgW		// Output Image Width
							, lc_uint* pnImgH		// Output Image Height
							, lc_uint* pnImgD		// Output Depth(byte)
							, char*	sFile			// Source FileName
					  )
{
	lc_int		hr		= 0;
	lc_int		nType	= LC_IMG_UNKNOWN;
	lc_int		nImgW	= 0;
	lc_int		nImgH	= 0;
	lc_int		nImgD	= 0;			// Channel(byte)
	lc_byte		nBgR	= 0;
	lc_byte		nBgG	= 0;
	lc_byte		nBgB	= 0;
	lc_byte*	pPxlS	= NULL;			// Pixel Data


	char Ext[8];

	char* p  = ::strchr(sFile, '.' );

	if(p)
		strcpy(Ext, p+1);

	if(0== ::strcmp(Ext, "bmp" ))
		nType = LC_IMG_BMP;
	else if(0== ::strcmp(Ext, "tga" ))
		nType = LC_IMG_TGA;
	else if(0== ::strcmp(Ext, "png" ))
		nType = LC_IMG_PNG;
	else
		return -1;



	if(nType == LC_IMG_BMP)
		hr = Bmp_FileRead(&pPxlS, &nImgW, &nImgH, &nImgD, sFile);

	else if(nType == LC_IMG_TGA)
		hr = Tga_FileRead(&pPxlS, &nImgW, &nImgH, &nImgD, sFile);

	else if(nType == LC_IMG_PNG)
		hr = Png_FileRead(&pPxlS, (lc_ulong*)&nImgW, (lc_ulong*)&nImgH, (lc_ulong*)&nImgD, &nBgR, &nBgG, &nBgB, sFile);



	if(hr<0)
		return -1;


	*ppPxl	= pPxlS;
	*pnType	= nType;
	*pnImgW	= nImgW;
	*pnImgH	= nImgH;
	*pnImgD	= nImgD;

	return 0;
}


