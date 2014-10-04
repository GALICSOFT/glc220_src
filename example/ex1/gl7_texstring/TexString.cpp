// Implementation of the CTexString class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma warning(disable:4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gles/gl.h>

#include <LcType.h>

#include "TexString.h"


int Tga_FileRead(unsigned char** pOutPxl	// Output Pixel
				,	int*	pOutImgW		// Output Image Width
				,	int*	pOutImgH		// Output Image Height
				,	int*	pOutImgC		// Output Image Channel(byte)
				,	char* sFile				// Source File
				);



CTexString::TvtxChar CTexString::m_CharL[];
int					 CTexString::m_CharI =0;


CTexString::CTexString()
{
	m_TexID	= 0;
	m_Red	= 1.F;
	m_Green	= 1.F;
	m_Blue	= 1.F;
	m_Alpha	= 1.F;

	m_X		= 0;
	m_Y		= 0;

	m_Hgt	= 12;

	m_Str	= NULL;
	m_VtxC	= 0;
	m_VtxP	= NULL;
	m_VtxT	= NULL;
}


CTexString::~CTexString()
{
	Destroy();
}


void CTexString::Destroy()
{
	if(m_TexID)
	{
		glDeleteTextures(1, &m_TexID);
		m_TexID = 0;
	}

	if(m_Str)
	{
		delete [] m_Str;
		m_Str = NULL;
	}

	ClearVertex();
}


int CTexString::Create(char* FontTexture, int Height)
{
	int	hr = 0;


	int	ImgW	= 0;
	int	ImgH	= 0;
	int	ImgC	= 0;
	int	ImgF	= 0;
	unsigned char* pPxl = NULL;


	hr = Tga_FileRead(&pPxl, &ImgW, &ImgH, &ImgC, FontTexture);

	if(0>hr)
		return LC_EFAIL;


	if(3 == ImgC)
		ImgF = GL_RGB;
	else if(4 == ImgC)
		ImgF = GL_RGBA;
	else
		ImgF = GL_LUMINANCE;

	// Generate Texture
	glGenTextures(1, &m_TexID);
	glBindTexture(GL_TEXTURE_2D, m_TexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, ImgF, ImgW, ImgH, 0, ImgF, GL_UNSIGNED_BYTE, pPxl);

	free(pPxl);


	m_Hgt = (float)Height;


	if(!CTexString::m_CharI)
		InitChar();

	return LC_OK;
}

void CTexString::SetString(char* Str)
{
	// Clear String
	if(NULL == Str ||
		(Str && (*Str == '\0' || *Str == '\n' || *Str == '\r')  )
		)
	{
		if(m_Str)
			delete m_Str;

		m_Str = NULL;

		ClearVertex();
		return;
	}

	// Is same to m_Str?
	if(m_Str && 0 == strcmp(m_Str, Str) )
		return;


	int nStr = strlen(Str);

	nStr = ( (nStr + 3)/4 +1)*4;		// make  multiple of 4.

	// delete String, remake and copy string
	if(m_Str)
		delete [] m_Str;

	m_Str = new char[nStr];
	memset(m_Str, 0, sizeof(char) * nStr);
	strcpy(m_Str, Str);

	// Clear vertex List
	ClearVertex();

	// remake vertex List
	SetupVertex();
}




void CTexString::SetPosition(float x, float y)
{
	m_X = x;
	m_Y = y;
}

void CTexString::SetColor(float r, float g, float b, float a)
{
	m_Red  = r;
	m_Green= g;
	m_Blue = b;
	m_Alpha= a;
}



int CTexString::DrawTxt()
{
	if(0 == m_VtxC)
		return LC_OK;


	// set up projection matrix in 3d pipeline
	float	temp[16]={0};
	float	w, h;
	glGetFloatv(GL_VIEWPORT, temp);		// get viewport to set the screen width and height.
	w = temp[2];
	h = temp[3];


	glMatrixMode(GL_PROJECTION);

	glPushMatrix();
	{

		glLoadIdentity();

		glOrthof( 0.F,   w
				,   h, 0.F
				, 0.F, 1.F);

		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
		{
			glLoadIdentity();


			glTranslatef(m_X, m_Y, 0);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glDisable(GL_CULL_FACE);

			//glBlendFunc(GL_ONE, GL_ONE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



			glBindTexture(GL_TEXTURE_2D, m_TexID);
			glVertexPointer(2, GL_FLOAT, 0, m_VtxP);
			glTexCoordPointer(2, GL_FLOAT, 0, m_VtxT);


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


			glColor4f(m_Red, m_Green, m_Blue, m_Alpha);

			glDrawArrays(GL_TRIANGLES, 0, m_VtxC * 6);


			glDisable(GL_TEXTURE_2D);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_BLEND);

			glColor4f(1, 1, 1, 1);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glPopMatrix();
	}
	glPopMatrix();


	return LC_OK;
}




void CTexString::SetupVertex()
{
	char* s = m_Str;

	// setup char count
	int		n = 0;
	while ( *s != '\0' && *s != '\n' && *s != '\r')
	{
		++n;
		++s;
	}

	// Triangle list
	m_VtxC = n;

	// Fill Vertex and UV
	m_VtxP = new CTexString::TvtxChar[m_VtxC];
	m_VtxT = new CTexString::TvtxChar[m_VtxC];


	s = m_Str;
	for(n = 0; n<m_VtxC; ++n)
	{
		SetupVtxChar(n, *s);
		++s;
	}
}



void CTexString::ClearVertex()
{
	m_VtxC	= 0;

	if(m_VtxP)
	{
		delete [] m_VtxP;
		m_VtxP = NULL;
	}

	if(m_VtxT)
	{
		delete [] m_VtxT;
		m_VtxT = NULL;
	}
}



void CTexString::SetupVtxChar(int n, int c)
{
	c -= 32;

	if(0>c)
		return;


	CTexString::TvtxChar* pVtxS = &CTexString::m_CharL[c];
	CTexString::TvtxChar* pVtxP = &m_VtxP[n];
	CTexString::TvtxChar* pVtxT = &m_VtxT[n];

	memcpy(pVtxT, pVtxS, sizeof(CTexString::TvtxChar));


	float	h = m_Hgt;
	float	w = h * 0.95F;

	pVtxP->v[0].x = (n + 0) * w;	pVtxP->v[0].y = h;
	pVtxP->v[1].x = (n + 1) * w;	pVtxP->v[1].y = h;
	pVtxP->v[2].x = (n + 0) * w;	pVtxP->v[2].y = 0;
	pVtxP->v[3].x = (n + 1) * w;	pVtxP->v[3].y = 0;

	pVtxP->v[4]   = pVtxP->v[2];
	pVtxP->v[5]   = pVtxP->v[1];

	c  =c;
}



void CTexString::InitChar()
{
	float row = 8.0F;
	float col = 16.0F;

	float epX = 1.5F/row;		// Epsilon X
	float epY = 0.F/col;		// Epsilon Y


	for(int i=0;i<128; ++i)
	{
		CTexString::TvtxChar* pChar = &CTexString::m_CharL[i];

		float x = (float)(i % 16);
		float y = col - (float)(i / 16);

		pChar->v[0].x = (x + 0.F + epX)/(col + 0.01f);
		pChar->v[0].y = (y - 1.F - epY)/(row + 0.01f);

		pChar->v[1].x = (x + 1.F - epX)/(col + 0.01f);
		pChar->v[1].y = (y - 1.F - epY)/(row + 0.01f);

		pChar->v[2].x = (x + 0.F + epX)/(col + 0.01f);
		pChar->v[2].y = (y - 0.F + epY)/(row + 0.01f);

		pChar->v[3].x = (x + 1.F - epX)/(col + 0.01f);
		pChar->v[3].y = (y - 0.F + epY)/(row + 0.01f);

		pChar->v[4] = pChar->v[2];
		pChar->v[5] = pChar->v[1];
	}
}




int Tga_FileRead(unsigned char** pOutPxl	// Output Pixel
				,	int*	pOutImgW		// Output Image Width
				,	int*	pOutImgH		// Output Image Height
				,	int*	pOutImgC		// Output Image Channel(byte)
				,	char* sFile				// Source File
				)
{
	FILE*		fp =NULL;

	// Header
	unsigned char	IDLength	=0;		// ID Length
	unsigned char	ColorMapType=0;		// Color Map Type
	unsigned char	ImageType	=0;		// Image type


	// Color Map Specification
	unsigned short	OffsetMapTable=0;
	unsigned short	ColorMapLength=0;
	unsigned char	ColorBits=0;


	// Image dimensions and format
	unsigned short	x_org =0;			// absolute coordinate of lower-left corner for displays where origin is at the lower left
	unsigned short	y_org =0;			// as for X-origin
//	unsigned short	nImgW =0;			// Image Width
//	unsigned short	nImgH =0;			// Image Height
	unsigned char	nDepth=0;			// Pixel depth: bits per pxl
	unsigned char	ImgDsc=0;			// Image descriptor (1 byte): bits 3-0 give the alpha channel depth, bits 5-4 give direction



	// For Final Targa Data
	unsigned short	nImgW =0;			// Image Width
	unsigned short	nImgH =0;			// Image Height
	unsigned char	nImgC =0;			// Channel Count
	long		nImgSize=0;			// size of TGA image
	unsigned char*	pPxlS = NULL;		// Dest Pixel
	unsigned char*	pPxlT = NULL;		// Temp Pixel


	unsigned char src[8]={0};
	int n=0, i=0, j=0;
	int nRead =0;


	// open the TGA file
	fp = fopen(sFile, "rb" );

	if (!fp)
	{
		printf("TGA File Open Err:%s\n", (char*)sFile);
		return LC_EFAIL;
	}

	// Read 3 bytes
	nRead = fread(&IDLength		, 1, 1, fp);	// Length of the image Id field
	nRead = fread(&ColorMapType	, 1, 1, fp);	// Whether a color map is included
	nRead = fread(&ImageType	, 1, 1, fp);	// Compression and color types

	// Read Color Map Specification
	nRead = fread(&OffsetMapTable, 2, 1, fp);	// First entry index (2 bytes): Offset into the color map table
	nRead = fread(&ColorMapLength, 2, 1, fp);	// Color map length (2 bytes): number of entries
	nRead = fread(&ColorBits     , 1, 1, fp);	// Color map entry size (1 byte): number of bits per pxl

	// Read Image Specification
	nRead = fread(&x_org  , 2, 1, fp);			// X-origin (2 bytes): absolute coordinate of lower-left corner for displays where origin is at the lower left
	nRead = fread(&y_org  , 2, 1, fp);			// Y-origin (2 bytes): as for X-origin
	nRead = fread(&nImgW  , 2, 1, fp);			// Image width (2 bytes): width in pixels
	nRead = fread(&nImgH  , 2, 1, fp);			// Image height (2 bytes): height in pixels
	nRead = fread(&nDepth , 1, 1, fp);			// Pixel depth (1 byte): bits per pxl
	nRead = fread(&ImgDsc , 1, 1, fp);			// Image descriptor (1 byte): bits 3-0 give the alpha channel depth, bits 5-4 give direction


	//	ImageType:
	//	0 no image data is present,
	//	1 uncompressed, color-mapped image,
	//	2 uncompressed, true-color image,
	//	3 uncompressed, black-and-white image,
	//	9 run-length encoded, color-mapped Image,
	//	10 run-length encoded, true-color image and,
	//	11 run-length encoded, black-and-white Image
	if( !(2 == ImageType || 10 == ImageType))
	{
		printf("TGA Err: We can only support only true-color(2 or 10).\n");
		fclose(fp);
		return LC_EFAIL;
	}


	fseek(fp, IDLength + ColorMapType * ColorMapLength, SEEK_CUR);


	// colormode -> 3 = BGR, 4 = BGRA
	nImgC		= nDepth>>3;
	nImgSize	= nImgW * nImgH * nImgC;
	pPxlS		= (unsigned char*)malloc(nImgSize);


	// Uncompressed
	if(2 == ImageType)
	{
		nRead = fread(pPxlS, sizeof(unsigned char), nImgSize, fp);
	}

	// Compressed
	else if(10 == ImageType)
	{
		pPxlT	= pPxlS;

		while (n < nImgW * nImgH)
		{

			if (fread(src, 1, nImgC+1, fp) != (size_t)(nImgC+1))
				break;

			j = src[0] & 0x7f;

			memcpy(&pPxlT[n*nImgC], &src[1], nImgC);
			++n;

			// RLE chunk
			if (src[0] & 0x80)
			{
				for (i=0;i<j;++i)
				{
					memcpy(&pPxlT[n*nImgC], &src[1], nImgC);
					++n;
				}
			}

			// Normal chunk
			else
			{
				for (i=0;i<j;++i)
				{
					if (fread(src, 1, nImgC, fp) != nImgC)
						break;

					memcpy( &pPxlT[n*nImgC], src, nImgC);
					++n;
				}
			}

		}// while

	}

	fclose(fp);


	// bgr to rgb
	for(int y=0; y<nImgH; ++y)
	{
		for(int x=0; x<nImgW; ++x)
		{
			if(3 == nImgC)
			{
				int n = y * nImgW + x;

				int r  = pPxlS[n * nImgC + 0];
				int g  = pPxlS[n * nImgC + 1];
				int b  = pPxlS[n * nImgC + 2];

				pPxlS[n * nImgC + 0] = b;
				pPxlS[n * nImgC + 1] = g;
				pPxlS[n * nImgC + 2] = r;
			}
		}
	}


	// Setting Pixel and Pixel Info
	*pOutPxl  = pPxlS;
	*pOutImgW = nImgW;
	*pOutImgH = nImgH;
	*pOutImgC = nImgC;

	return LC_OK;
}

