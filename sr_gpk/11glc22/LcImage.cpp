// Implementation of the ILC_Image.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__BADA__)
  #include <FBaseSys.h>
#endif

#include <LcType.h>

#include "LcBase.h"
#include "LcAux.h"


#include "../rc_cmm/08glb22/ImgBmp.h"
#include "../rc_cmm/08glb22/ImgTga.h"
#include "../rc_cmm/08glb22/ImgPng.h"



#if defined(_MSC_VER)
  #include "../rc_cmm/08glb22/ImgJpg.h"
#endif


#include "../rc_cmm/08glb22/ImgGpt.h"


class CLcImage : public ILC_Image
{
protected:
	UINT	m_RscType;				// Source Type
	UINT	m_Format;				// Pixel Format
	UINT	m_Type	;				// Pixel Type

	UINT	m_ImgW	;				// Image Width
	UINT	m_ImgH	;				// Image Height
	UINT	m_ImgD	;				// Image Depth
	UINT	m_ImgC	;				// Image Color Key

	BYTE*	m_pPxl	;				// Pixel Data

public:
	CLcImage();
	virtual ~CLcImage();

public:
	INT		CreateFromFile(LC_HANDLE sFile, UINT colorKey, UINT bCon16);
	INT		CreateFromMemory(LC_HANDLE Memory, INT MemSize, UINT colorKey, UINT bCon16);


	virtual INT		Query(char* sCmd, LC_HANDLE pData);// inherit from the ILC_Object
	virtual INT		Release();							// inherit from the ILC_Object

	virtual UINT	GetRscType(){	return m_RscType;}
	virtual UINT	GetFormat(){	return m_Format;}
	virtual UINT	GetType() {		return m_Type;	}

	virtual UINT	GetImgW() {		return m_ImgW;	}
	virtual UINT	GetImgH() {		return m_ImgH;	}
	virtual UINT	GetImgD() {		return m_ImgD;	}
	virtual UINT	GetImgC() {		return m_ImgC;	}

	virtual BYTE*	GetPixel(){		return m_pPxl;	}

	virtual INT		SaveToFile(char* sFile, INT ImgType, ULONG* time_s);	// Pixel data Save to TGA or GPT

protected:
	void	SetupReverseColor(BYTE** pPxl, INT ImgW, INT ImgH, INT ImgD);	// Blue <--> Red
	void	SetupInverseColor(BYTE** pPxl, INT ImgW, INT ImgH, INT ImgD);	// Inverse Bottom to Top
	void	SetupColorKey(BYTE** pPxl, INT ImgW, INT ImgH, INT ImgD, UINT dColorKey);

	INT		LoadFromFile(BYTE** ppPxl	// Output Pixel
					, UINT* pnRscType	// Output nRscType
					, UINT* pnFormat	// Output Pixel Format
					, UINT* pnType		// Output Pixel Type

					, UINT* pnImgW		// Output Image Width
					, UINT* pnImgH		// Output Image Height
					, UINT* pnImgD		// Output Depth(byte)
					, UINT* pnImgC		// Output Color Key

					, char*	sFile		// Source FileName
					);

	INT		LoadFromMemory(BYTE** ppPxl	// Output Pixel
					, UINT* pnRscType	// Output nRscType
					, UINT* pnFormat	// Output Pixel Format
					, UINT* pnType		// Output Pixel Type

					, UINT* pnImgW		// Output Image Width
					, UINT* pnImgH		// Output Image Height
					, UINT* pnImgD		// Output Depth(byte)
					, UINT* pnImgC		// Output Color Key

					, void*	Memory		// Source Memory
					, INT	iSize		// Size of Memory
					);

};




INT	CLcImage::LoadFromFile(BYTE** ppPxl		// Output Pixel
						, UINT* pnRscType	// Output nRscType
						, UINT* pnFormat	// Output Pixel Format
						, UINT* pnType		// Output Pixel Type

						, UINT* pnImgW		// Output Image Width
						, UINT* pnImgH		// Output Image Height
						, UINT* pnImgD		// Output Depth(byte)
						, UINT* pnImgC		// Output Color Key

						, char*	sFile		// Source FileName
					  )
{
	INT		hr		= -1;

	INT		nRscType= LC_UNKNOWN;	// Resource Type
	INT		nFormat	= 0x0;			// Pixel Format
	INT		nType	= 0x0;			// Pixel Type

	INT		nImgW	= 0;
	INT		nImgH	= 0;
	INT		nImgD	= 0;			// Channel(byte)
	COLOR32	nImgC	= 0x0;			// Color Key

	BYTE*	pPxlS	= NULL;			// Pixel Data


	char		Ext[16]={0};
	LcStr_SplitPath(sFile, NULL, NULL, NULL, Ext );


	if     (0== LcStr_Stricmp(Ext, ".gptf"))	nRscType = LC_IMG_GPT;
	else if(0== LcStr_Stricmp(Ext, ".bmp" ))	nRscType = LC_IMG_BMP;
	else if(0== LcStr_Stricmp(Ext, ".tga" ))	nRscType = LC_IMG_TGA;
	else if(0== LcStr_Stricmp(Ext, ".png" ))	nRscType = LC_IMG_PNG;

#if defined(_MSC_VER)
	else if(0== LcStr_Stricmp(Ext, ".jpg" ))	nRscType = LC_IMG_JPG;
#endif

	else
		return LC_CANNT_FIND_RESOURCE;





	if(nRscType == LC_IMG_GPT)
		hr = Gpt_FileRead(&pPxlS, &nFormat, &nType, &nImgW, &nImgH
							, &nImgD, &nImgC, NULL, sFile);

	else if(nRscType == LC_IMG_BMP)
		hr = Bmp_FileRead(&pPxlS, &nImgW, &nImgH, &nImgD, sFile);

	else if(nRscType == LC_IMG_TGA)
		hr = Tga_FileRead(&pPxlS, &nImgW, &nImgH, &nImgD, sFile);

	else if(nRscType == LC_IMG_PNG)
	{
		_png_dword	_W	= 0, _H	= 0, _D	= 0;
		_png_byte	_R	= 0, _G	= 0, _B	= 0;
		hr = Png_FileRead((_png_byte**)&pPxlS, &_W, &_H, &_D, &_R, &_G, &_B, (char*)sFile);
		nImgW = _W;	nImgH = _H;	nImgD = _D;
	}



#if defined(_MSC_VER)
	else if(nRscType == LC_IMG_JPG)
		hr = Jpg_FileRead(&pPxlS, &nImgW, &nImgH, &nImgD, sFile);
#endif


	if(hr<0)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Failed LoadFromFile: %s\n", sFile);
		return LC_CANNT_ALLOC;
	}


	*ppPxl		= pPxlS;
	*pnRscType	= nRscType;
	*pnImgW		= nImgW;
	*pnImgH		= nImgH;
	*pnImgD		= nImgD;
	*pnFormat	= nFormat;
	*pnType		= nType;
	*pnImgC		= nImgC;

	return LC_OK;
}




INT	CLcImage::LoadFromMemory(BYTE** ppPxl		// Output Pixel
							, UINT* pnRscType	// Output nRscType
							, UINT* pnFormat	// Output Pixel Format
							, UINT* pnType		// Output Pixel Type

							, UINT* pnImgW		// Output Image Width
							, UINT* pnImgH		// Output Image Height
							, UINT* pnImgD		// Output Depth(byte)
							, UINT* pnImgC		// Output Color Key

							, void*	Memory		// Source Memory
							, INT	iSize		// Size of Memory
					  )
{
	INT		hr		= -1;

	INT		nRscType= LC_UNKNOWN;	// Resource Type
	INT		nFormat	= 0x0;			// Pixel Format
	INT		nType	= 0x0;			// Pixel Type

	INT		nImgW	= 0;
	INT		nImgH	= 0;
	INT		nImgD	= 0;			// Channel(byte)
	COLOR32	nImgC	= 0x0;			// Color Key

	BYTE*	pPxlS	= NULL;			// Pixel Data


	// read header
	{
		INT ver[2]={0};
		memcpy(ver, Memory, sizeof(ver) );

		if(LC_RSC_GPACK == ver[0])
			nRscType = LC_IMG_GPT;

		else if( Png_CheckSignature((unsigned char*)ver) )
				nRscType = LC_IMG_PNG;

		else if(0x4d42 == (0x0000FFFF & ver[0]))
			nRscType = LC_IMG_BMP;

		else if(  0x000F0000 & ver[0])
				nRscType = LC_IMG_TGA;
	}

	if(LC_UNKNOWN == nRscType)
		return -1;





	if(LC_IMG_GPT == nRscType)
		hr = Gpt_MemRead(&pPxlS, &nFormat, &nType, &nImgW, &nImgH
							, &nImgD, &nImgC, NULL, Memory, iSize);

	else if(LC_IMG_BMP == nRscType)
		hr = Bmp_MemRead(&pPxlS, &nImgW, &nImgH, &nImgD, Memory, iSize);

	else if(nRscType == LC_IMG_TGA)
		hr = Tga_MemRead(&pPxlS, &nImgW, &nImgH, &nImgD, Memory, iSize);

	else if(nRscType == LC_IMG_PNG)
	{
		_png_dword	_W	= 0, _H	= 0, _D	= 0;
		_png_byte	_R	= 0, _G	= 0, _B	= 0;
		hr = Png_MemRead((_png_byte**)&pPxlS, &_W, &_H, &_D, &_R, &_G, &_B, Memory, iSize);
		nImgW = _W;	nImgH = _H;	nImgD = _D;
	}






	if(0>hr)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Failed LoadFromMemory\n");
		return LC_CANNT_ALLOC;
	}


	*ppPxl		= pPxlS;
	*pnRscType	= nRscType;
	*pnImgW		= nImgW;
	*pnImgH		= nImgH;
	*pnImgD		= nImgD;
	*pnFormat	= nFormat;
	*pnType		= nType;
	*pnImgC		= nImgC;

	return LC_OK;
}




CLcImage::CLcImage()
{
	m_RscType= 0;
	m_Format= 0;
	m_Type	= 0;

	m_ImgW	= 0;
	m_ImgH	= 0;
	m_ImgD	= 0;
	m_ImgC	= 0;
	m_pPxl	= NULL;
}


CLcImage::~CLcImage()
{
	SAFE_FREE(	m_pPxl	);
}



INT CLcImage::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcImage::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}





// return channel
// make 32bit rgba color
INT CLcImage::CreateFromFile(LC_HANDLE orgFile, UINT colorkey, UINT bCon16)
{
	INT		hr		= -1;

	char	sFileDst[LC_MAX_PATH2]={0};
	char	sMedia[LC_MAX_PATH2]={0};

	LcStr_ReplacePath(sFileDst, (const char*)orgFile, NULL, NULL, NULL, ".gptf");


#if !defined(_MSC_VER) && !defined(_PC_LINUX_)

	if(LC_FAILED(LcSys_ResourcePath(sMedia, sFileDst)))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("CLcImage::CreateFromFile FindResource Failed: %s\n", (char*)sMedia);
		return LC_CANNT_FIND_RESOURCE;
	}

	hr = LoadFromFile((BYTE**)&m_pPxl
					, (UINT*)&m_RscType
					, (UINT*)&m_Format
					, (UINT*)&m_Type

					, (UINT*)&m_ImgW
					, (UINT*)&m_ImgH
					, (UINT*)&m_ImgD
					, (UINT*)&m_ImgC

					, (char*)sMedia
					);

	return hr;
#endif

	ULONG st_time[2]={0};
	gpt_header pHeader;

	hr = LcFile_Info((char*)orgFile, NULL, &st_time[0], &st_time[1]);
	if(LC_FAILED(hr))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("CLcImage::CreateFromFile FindResource Failed: %s\n", (char*)orgFile);
		return LC_CANNT_FIND_RESOURCE;
	}


	hr = LcFile_Info((char*)sFileDst, NULL, NULL, NULL);
	if(LC_SUCCEEDED(hr))
	{
		hr = Gpt_FileInfo(&pHeader, sFileDst);

		// if there exist dest file and same information of original source file
		if(LC_SUCCEEDED(hr) &&
			st_time[0] == pHeader.time_s[0] &&
			st_time[1] == pHeader.time_s[1])
		{
			hr = LoadFromFile((BYTE**)&m_pPxl
					, (UINT*)&m_RscType
					, (UINT*)&m_Format
					, (UINT*)&m_Type

					, (UINT*)&m_ImgW
					, (UINT*)&m_ImgH
					, (UINT*)&m_ImgD
					, (UINT*)&m_ImgC

					, (char*)sFileDst
					);

			if(LC_SUCCEEDED(hr))
				return hr;


			LOGE("Err: %d, %s \t", __LINE__, __FILE__);
			LOGE("Image Failed: %s\n", (char*)orgFile);
		}
	}


	// if Failed load gptf then load original file.

	hr = LcSys_ResourcePath(sMedia, (char*)orgFile);

	hr = LoadFromFile((BYTE**)&m_pPxl
					, (UINT*)&m_RscType
					, (UINT*)&m_Format
					, (UINT*)&m_Type

					, (UINT*)&m_ImgW
					, (UINT*)&m_ImgH
					, (UINT*)&m_ImgD
					, (UINT*)&m_ImgC

					, (char*)sMedia
					);

	if(LC_FAILED(hr))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("LoadFromFile Failed: %s\n", (char*)sMedia);
		return LC_CANNT_FIND_RESOURCE;
	}


	////////////////////////////////////////////////////////////////////////////
	//convert pixel to dest device.

	// setting color key
	if(LC_IMG_GPT != m_RscType && 0x0 == m_ImgC && colorkey)
		m_ImgC = colorkey;


	// convert color to use in opengl
	if(LC_IMG_GPT != m_RscType)
	{
		// change the color bit to use in opengl.
		if(LC_IMG_BMP == m_RscType)
			SetupReverseColor(&m_pPxl, m_ImgW, m_ImgH, m_ImgD);

		else if(LC_IMG_TGA == m_RscType)
			SetupReverseColor(&m_pPxl, m_ImgW, m_ImgH, m_ImgD);

		else if(LC_IMG_PNG == m_RscType)
			SetupInverseColor(&m_pPxl, m_ImgW, m_ImgH, m_ImgD);

		// There is not a pertinent data in JPG
		else if(LC_IMG_JPG == m_RscType)
		{
		}
	}

	// setup the format and type
	if(LC_IMG_GPT != m_RscType)
	{
		if(m_ImgC)
		{
			SetupColorKey(&m_pPxl, m_ImgW, m_ImgH, m_ImgD, m_ImgC);
			m_ImgD= 4;
		}


		// setup format and type
		m_Type	= LC_FMT_UBYTE;	// GL_UNSIGNED_BYTE;
		m_Format= LC_FMT_RGB;

		if(4 == m_ImgD)
			m_Format = LC_FMT_RGBA;
	}


	// setup to 16bit pixel
	if( LC_IMG_GPT     != m_RscType &&
		LC_FMT_INDEX16 == bCon16)
	{
		BYTE*	pPxl = (BYTE*)malloc(m_ImgW * m_ImgH * sizeof(WORD));	// Make 16bit pixel
		UINT	oFormat=0, oType=0;

		UINT dInFormat = 0;

		if(m_ImgC)									// if the color key was specified.
			dInFormat = LC_FMT_WORD_5551;

		hr = LcDev_Convert32to16Color((WORD*)pPxl	// convert 32bit to 16 bit pixel and set up format, type.
								, &oFormat			// format
								, &oType			// type
								, m_pPxl
								, m_ImgW
								, m_ImgH
								, m_ImgD
								, m_ImgC
								, dInFormat);

		SAFE_FREE(	m_pPxl	);					// free 32bit original pixel

		if(LC_FAILED(hr))
		{
			SAFE_FREE(	pPxl );
			return LC_EFAIL;
		}

		// Re-setup Pixel, format, type and depth
		m_pPxl	= pPxl;
		m_Type	= oType;
		m_Format= oFormat;
		m_ImgD	= 2;
	}


	// save pixels to gptf file format free 16bit pixel
	// to reload gptf in Windows system
	if(LC_IMG_GPT != m_RscType)
	{
		::Gpt_FileWrite(m_pPxl, m_Format, m_Type, m_ImgW, m_ImgH
						, m_ImgD, m_ImgC, st_time, sFileDst);

		SAFE_FREE(	m_pPxl	);
	}


	// reload gptf file
	hr = LoadFromFile((BYTE**)&m_pPxl
					  , &m_RscType
					  , &m_Format
					  , &m_Type

					  , &m_ImgW
					  , &m_ImgH
					  , &m_ImgD
					  , &m_ImgC

					  , sFileDst
					  );

	if(hr<0)
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Image Failed: %s\n", (char*)orgFile);
		return LC_CANNT_FIND_RESOURCE;
	}

	return m_ImgD;
}



INT CLcImage::CreateFromMemory(LC_HANDLE pMem, INT MemSize, UINT colorkey, UINT bCon16)
{
	INT	hr = 0;


	// if Faid load gptf then load original file.
	hr = LoadFromMemory((BYTE**)&m_pPxl
					  , (UINT*)&m_RscType
					  , (UINT*)&m_Format
					  , (UINT*)&m_Type

					  , (UINT*)&m_ImgW
					  , (UINT*)&m_ImgH
					  , (UINT*)&m_ImgD
					  , (UINT*)&m_ImgC

					  , pMem, MemSize
					  );

	if(LC_FAILED(hr))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("LoadFromMemory() Failed\n");
		return LC_CANNT_FIND_RESOURCE;
	}





	// setting color key
	if(LC_IMG_GPT != m_RscType && 0x0 == m_ImgC && colorkey)
		m_ImgC = colorkey;


	// convert color to use in opengl
	if(LC_IMG_GPT != m_RscType)
	{
		// change the color bit to use in opengl.
		if(LC_IMG_BMP == m_RscType)
			SetupReverseColor(&m_pPxl, m_ImgW, m_ImgH, m_ImgD);

		else if(LC_IMG_TGA == m_RscType)
			SetupReverseColor(&m_pPxl, m_ImgW, m_ImgH, m_ImgD);

		else if(LC_IMG_PNG == m_RscType)
			SetupInverseColor(&m_pPxl, m_ImgW, m_ImgH, m_ImgD);

		// There is not a pertinent data in JPG
		else if(LC_IMG_JPG == m_RscType)
		{
		}
	}



	// setup the format and type
	if(LC_IMG_GPT != m_RscType)
	{
		if(m_ImgC)
		{
			SetupColorKey(&m_pPxl, m_ImgW, m_ImgH, m_ImgD, m_ImgC);
			m_ImgD= 4;
		}


		// setup format and type
		m_Type	= LC_FMT_UBYTE;	// GL_UNSIGNED_BYTE;
		m_Format= LC_FMT_RGB;

		if(4 == m_ImgD)
			m_Format = LC_FMT_RGBA;
	}



	// setup to 16bit pixel
	if(LC_IMG_GPT != m_RscType && LC_FMT_INDEX16 == bCon16)
	{
		BYTE*	pPxl = (BYTE*)malloc(m_ImgW * m_ImgH * sizeof(WORD));	// Make 16bit pixel
		UINT	oFormat=0, oType=0;

		hr = LcDev_Convert32to16Color((WORD*)pPxl	// convert 32bit to 16 bit pixel and set up format, type.
								, &oFormat		// format
								, &oType		// type
								, m_pPxl
								, m_ImgW
								, m_ImgH
								, m_ImgD
								, m_ImgC);

		SAFE_FREE(	m_pPxl	);					// free 32bit original pixel

		if(LC_FAILED(hr))
		{
			SAFE_FREE(	pPxl );
			return LC_EFAIL;
		}

		// Re-setup Pixel, format, type and depth
		m_pPxl	= pPxl;
		m_Type	= oType;
		m_Format= oFormat;
		m_ImgD	= 2;
	}


	return m_ImgD;
}


INT CLcImage::SaveToFile(char* sFile, INT ImgType, ULONG* time_s)
{
	if(LC_IMG_GPT == ImgType)
	{
		return ::Gpt_FileWrite(m_pPxl, m_Format, m_Type, m_ImgW, m_ImgH
								, m_ImgD, m_ImgC, time_s, sFile);
	}
	else if(LC_IMG_TGA == ImgType)
	{
	}

	return LC_OK;
}



void CLcImage::SetupReverseColor(BYTE** pPxlD, INT ImgW, INT ImgH, INT ImgD)
{
	INT	x=0, y=0, n=0;
	BYTE B;

	BYTE* pPxl = *pPxlD;

	// convert BGR or BGRA --> RGB
	for(y=0; y<ImgH; ++y)
	{
		for(x=0; x<ImgW; ++x)
		{
			n = (y*ImgW + x)* ImgD;

			B = pPxl[n+0];
			pPxl[n+0] = pPxl[n+2];	// red
			pPxl[n+2] = B;			// blue
		}
	}
}


void CLcImage::SetupInverseColor(BYTE** pPxlD, INT ImgW, INT ImgH, INT ImgD)
{
	INT	x=0, y=0;
	INT n1, n2;
	BYTE* pPxlT = NULL;

	BYTE* pPxl = *pPxlD;

	pPxlT	= (BYTE*)malloc(ImgW * ImgH * ImgD);
	memcpy(pPxlT, pPxl, ImgW * ImgH * ImgD);

	for(y=0; y < ImgH; ++y)
	{
		for(x=0; x < ImgW; ++x)
		{
			n1 =  ((       y) * ImgW + x) * ImgD;
			n2 =  ((ImgH-1-y) * ImgW + x) * ImgD;

			memcpy( &pPxl[n1], &pPxlT[n2], ImgD);
		}
	}

	free(pPxlT);
}


void CLcImage::SetupColorKey(BYTE** pPxlD, INT ImgW, INT ImgH, INT ImgD, UINT dColorKey)
{
	UINT	R, G, B;
	UINT	dR, dG, dB;

	dR = (dColorKey & 0x00FF0000) >>16;
	dG = (dColorKey & 0x0000FF00) >> 8;
	dB = (dColorKey & 0x000000FF) >> 0;

	INT	x=0, y=0, n=0;
	INT n1, n2;

	BYTE* pPxl = NULL;

	if(4 != ImgD)
	{
		BYTE* pPxlS = *pPxlD;

		pPxl = (BYTE*)malloc(ImgW * ImgH * 4);

		for(y=0; y<ImgH; ++y)
		{
			for(x=0; x<ImgW; ++x)
			{
				n1 = (y*ImgW + x)* ImgD;
				n2 = (y*ImgW + x)* 4;


				*((UINT*)&pPxl[n2]) = 0XFFFFFFFF;

				for(n=0;n<ImgD; ++n)
					pPxl[n2+n] = pPxlS[n1+n];
			}
		}

		free(*pPxlD);
		*pPxlD = pPxl;
	}
	else
		pPxl = *pPxlD;


	// Set alpha channel to zero
	for(y=0; y<ImgH; ++y)
	{
		for(x=0; x<ImgW; ++x)
		{
			n = (y*ImgW + x)* 4;

			R = pPxl[n+0];
			G = pPxl[n+1];
			B = pPxl[n+2];

			if(R == dR && G == dG && B == dB)
				pPxl[n+3] = 0x0;
		}
	}
}




// convert pixel bit 32 ==> 16
//	Color Key: 0x00000000 ==> disable the colorkey
//	Channel == 3
//		Color Key enable  ==> GL_UNSIGNED_SHORT_5_6_5
//		Color Key disable ==> GL_UNSIGNED_SHORT_5_5_5_1
//	Channel == 4
//		regardless of Color Key ==> GL_UNSIGNED_SHORT_4_4_4_4
INT LcDev_Convert32to16Color(WORD* pDst, UINT* oFormat, UINT* oType, const BYTE* pSrc, INT ImgW, INT ImgH, INT ImgD, UINT ColorKey, UINT dInFormat)
{
	INT		COLOR_BIT = 0;
	INT		x, y;

	INT		n1, n2;
	UINT	R, G, B, A;

	UINT	dA = (ColorKey & 0xFF000000) >>24;
	UINT	dR = (ColorKey & 0x00FF0000) >>16;
	UINT	dG = (ColorKey & 0x0000FF00) >> 8;
	UINT	dB = (ColorKey & 0x000000FF) >> 0;


	// Setup Pixel Format;
	if( 0 == dInFormat)
	{
		if(3 == ImgD)
		{
			if(0x0 == ColorKey)
				COLOR_BIT = LC_FMT_WORD_565;
			else
				COLOR_BIT = LC_FMT_WORD_5551;
		}
		else if(4 == ImgD)
			COLOR_BIT = LC_FMT_WORD_4444;

		else
			return LC_EFAIL;
	}
	else
		COLOR_BIT = dInFormat;



	if(LC_FMT_WORD_565 != COLOR_BIT && LC_FMT_WORD_5551 != COLOR_BIT && LC_FMT_WORD_4444 != COLOR_BIT)
		return LC_EFAIL;



	if(LC_FMT_WORD_565 == COLOR_BIT)
	{
		for(y=0; y<ImgH; ++y)
		{
			for(x=0; x<ImgW; ++x)
			{
				n1 = (y * ImgW + x) * ImgD;
				n2 = (y * ImgW + x);

				R = pSrc[n1+0];
				G = pSrc[n1+1];
				B = pSrc[n1+2];

				R >>= 3;
				G >>= 2;
				B >>= 3;
				pDst[n2] = (B<<0) | (G<<5) | (R<<11);
			}
		}
	}

	else if(LC_FMT_WORD_5551 == COLOR_BIT)
	{
		for(y=0; y<ImgH; ++y)
		{
			for(x=0; x<ImgW; ++x)
			{
				n1 = (y * ImgW + x) * ImgD;
				n2 = (y * ImgW + x);

				R = pSrc[n1+0];
				G = pSrc[n1+1];
				B = pSrc[n1+2];

				A = 0xFF;
				if(4 == ImgD)
					A = pSrc[n1+3];

				//  opaque pixel if is not mismatched Red, green and blue
				if(ColorKey && dA && dR == R && dG == G && dB == B )
					A = 0x00;

				R >>= 3;
				G >>= 3;
				B >>= 3;
				A >>= 7;
				pDst[n2] = (A<<0) | (B<<1) | (G<<6) | (R<<11);
			}
		}
	}

	else if(LC_FMT_WORD_4444 == COLOR_BIT)
	{
		for(y=0; y<ImgH; ++y)
		{
			for(x=0; x<ImgW; ++x)
			{
				n1 = (y * ImgW + x) * ImgD;
				n2 = (y * ImgW + x);

				R = pSrc[n1+0];
				G = pSrc[n1+1];
				B = pSrc[n1+2];

				A = 0xFF;
				if(4 == ImgD)
					A = pSrc[n1+3];

				//  opaque pixel if is not mismatched Red, green and blue
				if(ColorKey && dA && dR == R && dG == G && dB == B )
					A = 0x00;

				R >>= 4;
				G >>= 4;
				B >>= 4;
				A >>= 4;
				pDst[n2] = (A<<0) | (B<<4) | (G<<8) | (R<<12);
			}
		}
	}


	// setup Type
	if(oType)
		*oType = COLOR_BIT;

	// setup format
	if(oFormat)
	{
		if	   (LC_FMT_WORD_565  == COLOR_BIT)	*oFormat = LC_FMT_RGB;
		else if(LC_FMT_WORD_5551 == COLOR_BIT)	*oFormat = LC_FMT_RGBA;
		else if(LC_FMT_WORD_4444 == COLOR_BIT)	*oFormat = LC_FMT_RGBA;
	}


	// return Color Bit Format
	return COLOR_BIT;
}




////////////////////////////////////////////////////////////////////////////////


INT LcDev_CreateImage(char* sCmd					// Null is File, "Resouce" is Memory buffer. "Buffer"
				   , PLC_IMAGE* pOut			// Output instance
				   , LC_HANDLE p1				// sCmd => NULL File Name, "Resource" Resource ID, "Buffer"  Buffer Width
				   , LC_HANDLE p2				// "Buffer"  Buffer Height
				   , LC_HANDLE p3				// Color Key
				   , LC_HANDLE p4				// Convert pixel bit 32 ==> 16 option
					 )
{
	INT			hr = 0;
	CLcImage*	pObj = NULL;

	INT			nSize = 0;
	UINT		ColorKey = 0;		// Pixel Color Key
	UINT		bCon16= 0;			// Convert pixel bit 32 ==> 16 option

	*pOut	= NULL;

	if(p2)	nSize    = *((INT* )p2);
	if(p3)	ColorKey = *((UINT*)p3);
	if(p4)	bCon16   = *((UINT*)p4);


	pObj = new CLcImage;


	if(NULL == sCmd)
		hr = pObj->CreateFromFile(p1, ColorKey, bCon16);

	else if( 0 == LcStr_Stricmp(sCmd, LCCMD_IMAGE_RESOURCE))
		hr = LC_NOT_IMPLEMENTED;

	else if( 0 == LcStr_Stricmp(sCmd, LCCMD_IMAGE_MEMORY))
	{
		if(0 == nSize)
			hr = LC_CANNT_ALLOC;
		else
			hr = pObj->CreateFromMemory(p1, nSize, ColorKey, bCon16);
	}

	else
		hr = LC_INVALID_CALL;


	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}



	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}



// Create Image From File
INT LcDev_CreateImageFromFile(char* sCmd			// absolutely, you must set the value to Null.
					   , PLC_IMAGE* pOut		// Output instance
					   , LC_HANDLE p1			// File Name
					   , LC_HANDLE p2			// absolutely, you must set the value to Null.
					   , LC_HANDLE p3			// Color Key
					   , LC_HANDLE p4			// Convert pixel bit 32 ==> 16 option
					 )
{
	INT			hr = 0;
	CLcImage*	pObj = NULL;

	UINT		ColorKey = 0;		// Pixel Color Key
	UINT		bCon16= 0;			// Convert pixel bit 32 ==> 16 option

	*pOut	= NULL;

	if(p3)	ColorKey = *((UINT*)p3);
	if(p4)	bCon16   = *((UINT*)p4);

	pObj = new CLcImage;

	hr = pObj->CreateFromFile(p1, ColorKey, bCon16);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}





INT LcDev_CreateImageFromMemory(char* sCmd	// Null
					   , PLC_IMAGE* pOut	// Output instance
					   , LC_HANDLE p1		// memory
					   , LC_HANDLE p2		// memory size
					   , LC_HANDLE p3		// Color Key
					   , LC_HANDLE p4		// Convert pixel 16?
					 )
{
	INT			hr = 0;
	CLcImage*	pObj = NULL;

	UINT		nSize = 0;
	UINT		ColorKey = 0;		// Pixel Color Key
	UINT		bCon16	= 0;		// Convert pixel 16?

	*pOut	= NULL;

	if(!p1 || !p2)
		return LC_EFAIL;

	if(p2)	nSize    = *((UINT*)p2);
	if(p3)	ColorKey = *((UINT*)p3);
	if(p4)	bCon16   = *((UINT*)p4);

	pObj = new CLcImage;

	hr = pObj->CreateFromMemory(p1, nSize, ColorKey, bCon16);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}


