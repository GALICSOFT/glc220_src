// Load Image Pixel.
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _LcImg_H_
#define _LcImg_H_


enum ELcImgType
{
	LC_IMG_UNKNOWN	= 0,
	LC_IMG_BMP		,
	LC_IMG_TGA		,
	LC_IMG_PNG		,
};


// Output Pixel Memory is created by malloc() function.
// need free()
int	LcImg_CreateImageFromFile(unsigned char** pPxl		// Output Pixel
					  , unsigned int* nType				// Output Type
					  , unsigned int* nImgW				// Output Image Width
					  , unsigned int* nImgH				// Output Image Height
					  , unsigned int* nImgD				// Output Depth(byte)
					  , char*	sSrcFile				// Source FileName
					  );


#endif _LcImg_H_


