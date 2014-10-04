// Implementation of the CParticle class.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gles/gl.h>

#include <LcType.h>

#include "Particle.h"


extern int Tga_FileRead(unsigned char** pOutPxl		// Output Pixel
					,	int*	pOutImgW			// Output Image Width
					,	int*	pOutImgH			// Output Image Height
					,	int*	pOutImgC			// Output Image Channel(byte)
					,	char* sFile					// Source File
					);



CParticle::CParticle()
{
	m_TexID	= 0;

	m_PtcN	= 0;
	m_Ptc	= NULL;
}


CParticle::~CParticle()
{
	Destroy();
}


void CParticle::Destroy()
{
	if(m_TexID)
	{
		glDeleteTextures(1, &m_TexID);
		m_TexID = 0;
	}

	if(m_Ptc)
	{
		delete [] m_Ptc;
		m_Ptc = NULL;
	}
}


int CParticle::Create(char* FontTexture, int ParticleNumber)
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, ImgF, ImgW, ImgH, 0, ImgF, GL_UNSIGNED_BYTE, pPxl);

	free(pPxl);


	m_PtcN = ParticleNumber;

	m_Ptc = new CParticle::Tptc[m_PtcN];


	for(int n=0; n<m_PtcN; ++n)
		InitPtc(n);

/*

	printf("\n\n--------------\n");

	char* ext = (char*)glGetString(GL_EXTENSIONS);
	char* bgn = ext;

	char  Buf[128]={0};
	char* str;

	str = Buf;

    for(; *bgn; ++bgn)
	{
		if(' ' == *bgn ||'\0' == *bgn )
		{
			*str = 0;

			char* dst = strstr(Buf, "GL_ARB_multisample");
			
			//if(dst)
			{
				printf("%s\n", Buf);
			}
//			else
//			{
//				dst = strstr(Buf, "matrix");
//				if(dst)
//					printf("%s\n", Buf);
//			}
				


			str = Buf;
			memset(Buf, 0, sizeof(Buf));
			continue;
		}

		*(str++) = *bgn;
    }

	printf("\n\n--------------\n");
*/
	
	return LC_OK;
}


int CParticle::FrameMove()
{
	for(int n=0; n<m_PtcN; ++n)
	{
		m_Ptc[n].pos.x += m_Ptc[n].vec.x;
		m_Ptc[n].pos.y += m_Ptc[n].vec.y;
		m_Ptc[n].col.a -= 0.002F;
		
		if(500 < m_Ptc[n].pos.y || 0.F >= m_Ptc[n].col.a)
			Reset(n);
	}

	return LC_OK;
}


void CParticle::Render()
{
	int		hr = 0;

	// set up projection matrix in 3d pipeline
	float	temp[16]={0};
	float	w, h;
	glGetFloatv(GL_VIEWPORT, temp);		// get viewport to set the screen width and height.
	w = temp[2];
	h = temp[3];


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrthof( 0.F,   w
			,   h, 0.F
			, 0.F, 1.F);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	hr = glGetError();
	hr = glGetError();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_TexID);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	//glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);


	//Setup point sprites coordinate generation
	glEnable(GL_POINT_SPRITE_OES);

	glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE);
	glPointParameterf ( GL_POINT_SIZE_MAX , 135);
	glPointSize(24);



	char* pVtx = (char*)m_Ptc;
	glEnableClientState(GL_VERTEX_ARRAY);	glVertexPointer(2, GL_FLOAT, sizeof(CParticle::Tptc), pVtx);	pVtx += sizeof(Tvector2) * 2;
	glEnableClientState(GL_COLOR_ARRAY);	glColorPointer (4, GL_FLOAT, sizeof(CParticle::Tptc), pVtx);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glDrawArrays(GL_POINTS, 0, m_PtcN);


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);


	glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);


	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glDisable(GL_POINT_SPRITE_OES);

	
	glDisable(GL_BLEND);

	glColor4f(1, 1, 1, 1);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}



void CParticle::InitPtc(int n)
{
	float	temp[16]={0};
	float	w;
	glGetFloatv(GL_VIEWPORT, temp);
	w = temp[2];



	m_Ptc[n].pos.x = (rand()%(m_PtcN+1)) * w/m_PtcN;
	m_Ptc[n].pos.y = -300.F + rand()%(301);

	m_Ptc[n].vec.x = 0;
	m_Ptc[n].vec.y = (100 + rand()%(m_PtcN+1)) * 0.01F;


	m_Ptc[n].col.r = (128 + rand()%128)/255.F	;
	m_Ptc[n].col.g = (128 + rand()%128)/255.F	;
	m_Ptc[n].col.b = (128 + rand()%128)/255.F	;
	m_Ptc[n].col.a = (128 + rand()%128)/255.F	;

}


void CParticle::Reset(int n)
{
	float	temp[16]={0};
	float	w;
	glGetFloatv(GL_VIEWPORT, temp);
	w = temp[2];

	m_Ptc[n].pos.x = (rand()%(m_PtcN+1)) * w/m_PtcN;
	m_Ptc[n].pos.y = 0;		// Vertex Position List

	m_Ptc[n].vec.x = 0;
	m_Ptc[n].vec.y = (100 + rand()%(m_PtcN+1)) * 0.01F;


	m_Ptc[n].col.r = (128 + rand()%128)/255.F	;
	m_Ptc[n].col.g = (128 + rand()%128)/255.F	;
	m_Ptc[n].col.b = (128 + rand()%128)/255.F	;
	m_Ptc[n].col.a = (128 + rand()%128)/255.F	;

}


