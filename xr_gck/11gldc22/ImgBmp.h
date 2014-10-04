//
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _ImgBmp_H_
#define _ImgBmp_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef	int					bmp_int;
typedef	long				bmp_long;
typedef	unsigned long		bmp_dword;
typedef	unsigned char		bmp_byte;
typedef	unsigned short int	bmp_ushort;
typedef	unsigned short int	bmp_word;


struct bmp_file
{
	bmp_word    bfType		;
	bmp_dword   bfSize		;
	bmp_word    bfReserved1	;
	bmp_word    bfReserved2	;
	bmp_dword   bfOffBits	;
};

struct bmp_info
{
	bmp_dword      biSize		;			// Image Size
	bmp_long       biWidth		;			// Image Width
	bmp_long       biHeight		;			// Image Height
	bmp_word       biPlanes		;
	bmp_word       biBitCount	;			// Bit Count(24)
	bmp_dword      biCompression	;
	bmp_dword      biSizeImage		;
	bmp_long       biXPelsPerMeter	;
	bmp_long       biYPelsPerMeter	;
	bmp_dword      biClrUsed		;
	bmp_dword      biClrImportant	;
};


//struct bmp_palette
//{
//	bmp_byte	r;
//	bmp_byte	g;
//	bmp_byte	b;
//	bmp_byte	f;
//};
//
//struct bmp_RGBQUAD
//{
//	bmp_byte	b;
//	bmp_byte	g;
//	bmp_byte	r;
//	bmp_byte	t;	// unused
//};


bmp_int	Bmp_FileRead(bmp_byte** pOutPxl		// Output Pixel
				,	bmp_int*    pOutImgW	// Output Image Width
				,	bmp_int*    pOutImgH	// Output Image Height
				,	bmp_int*    pOutImgB	// Output Image Byte
				,	char* sFile				// Source File
				);


bmp_int	Bmp_MemRead(bmp_byte**  pOutPxl		// Output Pixel
				,	bmp_int*    pOutImgW	// Output Image Width
				,	bmp_int*    pOutImgH	// Output Image Height
				,	bmp_int*    pOutImgC	// Output Image Channel(byte)
				,	void*       Memory		// Memory buffer
				,	int         MemSize		// Memory Size
				);




#ifdef __cplusplus
}
#endif


#endif



