//
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _ImgTga_H_
#define _ImgTga_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef	int					tga_int;
typedef	unsigned long		tga_dword;
typedef	unsigned char		tga_byte;
typedef	unsigned short int	tga_ushort;
typedef	unsigned short int	tga_word;


struct tga_header
{
	tga_byte		cIdLen;				// Id Length
	tga_byte		cMapType;			// Color Map type
	tga_byte		cImgType;			// Image Type

	tga_ushort		nMapOrg;			// color Map Origin
	tga_ushort		nMapLen;			// Color Map Length
	tga_byte		cMapSize;			// Color Map Entry size

	tga_ushort		nImgX;				// Image X of Origin
	tga_ushort		nImgY;				// Image Y of Origin;
	tga_ushort		nImgW;				// Image Width;
	tga_ushort		nImgH;				// Image Height
	tga_byte		nImgD;				// Image Depth

	tga_byte		cImgDsc;			// Image Descriptor
	tga_byte*		sImdId;				// Image Id
};


struct tga_image
{
	tga_int				m_bByteSwap;	// always TRUE

	struct tga_header	m_Header;
	tga_byte*			m_pColorMap;
	tga_byte*			m_pData;
};

tga_int	Tga_FileRead(tga_byte** pOutPxl		// Output Pixel
				,	tga_int*	pOutImgW	// Output Image Width
				,	tga_int*	pOutImgH	// Output Image Height
				,	tga_int*	pOutImgC	// Output Image Channel(byte)
				,	char* sFile				// Source File
				);

tga_int	Tga_MemRead(tga_byte**  pOutPxl		// Output Pixel
				,	tga_int*	pOutImgW	// Output Image Width
				,	tga_int*	pOutImgH	// Output Image Height
				,	tga_int*	pOutImgC	// Output Image Channel(byte)
				,	void*       Memory		// Memory buffer
				,	int         MemSize		// Memory Size
				);


#ifdef __cplusplus
}
#endif


#endif



