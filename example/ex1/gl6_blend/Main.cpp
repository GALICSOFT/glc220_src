// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"


int Tga_FileRead(unsigned char** pOutPxl		// Output Pixel
				,	int*	pOutImgW	// Output Image Width
				,	int*	pOutImgH	// Output Image Height
				,	int*	pOutImgC	// Output Image Channel(byte)
				,	char* sFile				// Source File
				);



#define MEDIA_DIR	"../../media/"



CMain::CMain()
{
	m_TexID = 0;
}


int	CMain::Init()
{
	int	hr = 0;


	// texture 0
	{
		int	ImgW	= 0;
		int	ImgH	= 0;
		int	ImgC	= 0;
		int	ImgF	= 0;
		unsigned char* pPxl = NULL;


		hr = Tga_FileRead(&pPxl, &ImgW, &ImgH, &ImgC, (char*)MEDIA_DIR"tex/star128.tga");

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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, ImgF, ImgW, ImgH, 0, ImgF, GL_UNSIGNED_BYTE, pPxl);

		free(pPxl);
	}


	// Create Particle
	for(int n=0; n<MAX_STAR_NUM; ++n)
	{
		m_pStar[n].angle= n /float(MAX_STAR_NUM-1) * 720.F;
		m_pStar[n].dist=((float)n/MAX_STAR_NUM)* 5.0f;
		m_pStar[n].r = (rand() % 256 )/255.F;
		m_pStar[n].g = (rand() % 256 )/255.F;
		m_pStar[n].b = (rand() % 256 )/255.F;
	}

	return LC_OK;
}


int	CMain::Destroy()
{
	if(m_TexID)
	{
		glDeleteTextures(1, &m_TexID);
		m_TexID = 0;
	}

	return LC_OK;
}

int	CMain::FrameMove()
{
#if defined(_WIN32)
	LcSys_Sleep(10);
#endif


	static int c=0;
	++c;

	//	if(c>20*60*20)
	if(c>8*60*2)
	{
		printf("Game Process Terminated\n");
		return LC_EFAIL;
	}


	static int n = 0;
	++n;

	if(n>5)
	{
		printf("FPS: %3.f\n", m_fFPS);
		n = 0;
	}


	return LC_OK;
}


int	CMain::Render()
{
	glShadeModel(GL_SMOOTH);
	glDisable( GL_DEPTH_TEST);
	glDisable( GL_CULL_FACE );

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	SetPerspective();


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// Really Nice Perspective Calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);


	static float spin=0;

	float texcoords[4][2];
	float vertices[4][3];


	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	glActiveTexture(GL_TEXTURE0);
	//glClientActiveTexture(GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, m_TexID);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);



	for(int n=0; n<MAX_STAR_NUM; n++)
	{
		glLoadIdentity();

		glTranslatef(0.0f, 0.0f, -12);


		glRotatef(90, 1.0f, 0.0f, 0.0f);
		glRotatef(m_pStar[n].angle, 0.0f, 1.0f, 0.0f);

		glTranslatef(m_pStar[n].dist, 0.0f, 0.0f);
		glRotatef(-m_pStar[n].angle, 0.0f, 1.0f, 0.0f);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);


		glRotatef(spin, 0.0f, 0.0f, 1.0f);

		glColor4f(m_pStar[n].r, m_pStar[n].g, m_pStar[n].b, 1.0f);


		texcoords[0][0]=0.0f; texcoords[0][1]=0.0f;		vertices[0][0]=-1.0f; vertices[0][1]=-1.0f; vertices[0][2]=0.0f;
		texcoords[1][0]=1.0f; texcoords[1][1]=0.0f;		vertices[1][0]= 1.0f; vertices[1][1]=-1.0f; vertices[1][2]=0.0f;
		texcoords[2][0]=1.0f; texcoords[2][1]=1.0f;		vertices[2][0]= 1.0f; vertices[2][1]= 1.0f; vertices[2][2]=0.0f;
		texcoords[3][0]=0.0f; texcoords[3][1]=1.0f;		vertices[3][0]=-1.0f; vertices[3][1]= 1.0f; vertices[3][2]=0.0f;


		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


		spin+=0.02f;

		m_pStar[n].angle+=(float)n/MAX_STAR_NUM;

		m_pStar[n].dist-=0.01f;


		if (m_pStar[n].dist<0.0f)
		{
			m_pStar[n].dist+=5.0f;
			m_pStar[n].r= (rand()%256)/255.F;
			m_pStar[n].g= (rand()%256)/255.F;
			m_pStar[n].b= (rand()%256)/255.F;
		}
	}


	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_BLEND);


	return LC_OK;
}




void CMain::SetPerspective()
{
	// set up projection matrix in 3d pipeline
	float	temp[16]={0};
	float	width, height;
	glGetFloatv(GL_VIEWPORT, temp);		// get viewport to set the screen width and height.
	width = temp[2];
	height= temp[3];


	float	Asp = (float)width/height;
	float	Near = 1.F;
	float	Far	 = 1000.F;
	float	Fov  = 45.F;

	float	xmin, xmax, ymin, ymax;

	ymax = Near * (float)tan(Fov* 3.1415962 / 360.0);
	ymin = -ymax;

	xmin = ymin * Asp;
	xmax = ymax * Asp;

	glMatrixMode(GL_PROJECTION);					// specify which matrix is the current matrix.
	glLoadIdentity();								// replace the current matrix by the identity matrix
	glFrustumf(xmin, xmax, ymin, ymax, Near, Far);	// multiply the current matrix by the perspective matrix.


	glMatrixMode(GL_MODELVIEW);
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

