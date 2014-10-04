// Interface for the PNG File.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _ImgPng_H_
#define _ImgPng_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef	int					_png_int;
typedef	unsigned long		_png_dword;
typedef	unsigned char		_png_byte;

_png_int Png_FileRead(_png_byte** pOutPxl	// Output Pixel
				,	_png_dword*	pOutImgW	// Image Width
				,	_png_dword*	pOutImgH	// Image Height
				,	_png_dword*	pOutImgC	// Image Channel(byte)
				,	_png_byte*	pOutBgR		// Background Color Red
				,	_png_byte*	pOutBgG		// Background Color Green
				,	_png_byte*	pOutBgB		// Background Color Blue
				,	char* sFileName			// Image File Name
				);


_png_int Png_MemRead(_png_byte** pOutPxl	// Output Pixel
				,	_png_dword*	pOutImgW	// Image Width
				,	_png_dword*	pOutImgH	// Image Height
				,	_png_dword*	pOutImgC	// Image Channel(byte)
				,	_png_byte*	pOutBgR		// Background Color Red
				,	_png_byte*	pOutBgG		// Background Color Green
				,	_png_byte*	pOutBgB		// Background Color Blue
				,	void*       Memory		// Memory buffer
				,	int         MemSize		// Memory Size
				);


#ifdef __cplusplus
}
#endif


#endif



