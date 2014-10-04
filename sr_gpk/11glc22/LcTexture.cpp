// Implementation of the ILC_Texture.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_PC_LINUX_)
  #include <GL/gl.h>

#elif defined(__APPLE__) || defined(__IPHONE__)
  #import <OpenGLES/ES1/gl.h>

#elif defined(__BADA__)
  #include <FBaseSys.h>
  #include <FGraphicsOpengl.h>
  using namespace Osp::Graphics::Opengl;

#else
  #include <gles/gl.h>

#endif


#include <LcType.h>
#include <LcMath.h>

#include "LcBase.h"
#include "LcAux.h"
#include "LcTexture.h"

#include "../rc_cmm/08glb22/ImgGpt.h"



#ifndef GL_GENERATE_MIPMAP
#	define GL_GENERATE_MIPMAP	0x8191
#endif


// Generate Mipmap pixels with box filtering
// from book opengl es 2.0(//http://www.opengles-book.com)
// GenMipMap2D()
//
INT _LC_TexGenMipMap(BYTE** pOut, INT* dstW, INT* dstH, const BYTE* pSrc, const INT srcW, const  INT srcH, const  INT srcD = 4)
{
	BYTE*	pDst;
	INT		W, H, D;

	INT		i, j;
	INT		s[4];
	FLOAT	R, G, B, A;
	INT		k;


	// Setup Width
	W = srcW / 2;
	if ( W <= 0 )
		W = 1;

	// Setup Height
	H = srcH / 2;
	if ( H <= 0 )
		H = 1;


	if(2 > W || 2 > H)
		return -1;


	D = srcD;	// Setup Depth
	pDst = (BYTE*)malloc(W * H * D * sizeof(BYTE));

	if(!pDst)
		return -1;


	for( j=0; j < H; ++j)
	{
		for(i=0; i < W; ++i)
		{
			// Sampling Index
			s[0] = ( (j*2 + 0) * srcW + i*2 + 0 ) * D;
			s[1] = ( (j*2 + 0) * srcW + i*2 + 1 ) * D;
			s[2] = ( (j*2 + 1) * srcW + i*2 + 0 ) * D;
			s[3] = ( (j*2 + 1) * srcW + i*2 + 1 ) * D;

			R = 0.F;
			G = 0.F;
			B = 0.F;
			A = 0.F;

			// Sum all pixels
			for(k=0; k < 4; ++k)
			{
				R += pSrc[ s[k] + 0 ];
				G += pSrc[ s[k] + 1 ];
				B += pSrc[ s[k] + 2 ];
				A += pSrc[ s[k] + 3 ];
			}

			// Average results
			R /= 4.F;
			G /= 4.F;
			B /= 4.F;
			A /= 4.F;

			if(R>255.F)	R = 255.F;
			if(G>255.F)	G = 255.F;
			if(B>255.F)	B = 255.F;
			if(A>255.F)	A = 255.F;

			// Store resulting pixels
			pDst[ ( j * W + i ) * D + 0] = (BYTE)(R);
			pDst[ ( j * W + i ) * D + 1] = (BYTE)(G);
			pDst[ ( j * W + i ) * D + 2] = (BYTE)(B);
			pDst[ ( j * W + i ) * D + 3] = (BYTE)(A);
		}
	}



	if(pOut)	*pOut = pDst;
	if(dstW)	*dstW = W;
	if(dstH)	*dstH = H;


	return 0;
}



// Create Mipmap chain: corresponding to gluBuild2DMipmaps()
INT _LC_Build2DMipmaps(INT Format, INT ImageWidth, INT ImageHeight, INT ImageDepth, INT Type, void* SourcePixel)
{
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	BYTE*	pPxlS= (BYTE*)SourcePixel;
	BYTE*	pPxlD= pPxlS;
	INT		srcW = ImageWidth;
	INT		srcH = ImageHeight;
	INT		srcD = ImageDepth;
	INT		dstW = srcW;
	INT		dstH = srcH;
	INT		Level = 0;

	INT		hr=0;

	// Level 0
	glTexImage2D(GL_TEXTURE_2D, Level, Format, dstW, dstH, 0, Format, Type, pPxlD);

	hr = _LC_TexGenMipMap(&pPxlD, &dstW, &dstH, pPxlS, srcW, srcH, srcD);


	//Mipmap failed
	if(0>hr)
		return LC_EFAIL;


	++Level;

	glTexImage2D(GL_TEXTURE_2D, Level, Format, dstW, dstH, 0, Format, Type, pPxlD);

	while(1)
	{
		pPxlS= pPxlD;
		srcW = dstW;
		srcH = dstH;
		pPxlD= NULL;

		++Level;

		hr = _LC_TexGenMipMap(&pPxlD, &dstW, &dstH, pPxlS, srcW, srcH, srcD);
		free(pPxlS);

		if(0>hr)
			break;

		glTexImage2D(GL_TEXTURE_2D, Level, Format, dstW, dstH, 0, Format, Type, pPxlD);
	}


	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

	return LC_OK;
}









CLcTexture::CLcTexture()
{
	m_nTex	= 0;
	m_nFmt	= 0x0;
	m_Type	= 0x0;
	m_ImgW	= 0;
	m_ImgH	= 0;
	m_ImgD	= 0;
	m_ImgC	= 0x0;
	m_TexW	= 0;
	m_TexH	= 0;
	m_xMip	= 0;

	m_xPool	= 0;
	m_Mem	= NULL;
	m_MemS	= 0;

	memset(m_sName, 0, sizeof m_sName);
}



CLcTexture::~CLcTexture()
{
	Destroy();
}


INT CLcTexture::Query(char* sCmd, LC_HANDLE pData)
{
	if( 0 == LcStr_Stricmp(sCmd, LCQUERY_GET_OWNER))
	{
		if(NULL == m_Own)
			return LC_EFAIL;

		if(pData)
			*((void**)pData) = m_Own;
	}

	else if( 0 == LcStr_Stricmp(sCmd, (char*)"Set Image Width"))
	{
		m_ImgW = *((UINT*)pData);
	}

	else if( 0 == LcStr_Stricmp(sCmd, (char*)"Set Image Height"))
	{
		m_ImgH = *((UINT*)pData);
	}

	else if( 0 == LcStr_Stricmp(sCmd, LCQUERY_INVALID))
	{
		return this->Invalid();
	}
	else if( 0 == LcStr_Stricmp(sCmd, LCQUERY_RESTORE))
	{
		return this->Restore();
	}

	else
		return LC_EFAIL;


	return LC_OK;
}


INT CLcTexture::Release()
{
	INT hr = 0;

	void* pOwn = NULL;
	hr = this->Query(LCQUERY_GET_OWNER, &pOwn);
	if(LC_SUCCEEDED(hr))	// if there is owner then this cannot be released
		return LC_EFAIL;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}




void CLcTexture::Destroy()
{
	if(0 == m_nTex)
		return;

	glDeleteTextures (1, &m_nTex);
	m_nTex = 0;

	#if defined(__ANDROID__)
	SAFE_FREE(	m_Mem	);
	#endif

}



////////////////////////////////////////////////////////////////////////////////



INT CLcTexture::CreateFromFile(   LC_HANDLE p1		// File Name
								, LC_HANDLE p2		// Color Key
								, LC_HANDLE p3		// Mipmap Level
								, LC_HANDLE p4		// Convert 16bit?
						 )
{
	INT			hr		= 0;
	BYTE*		pPxlDst = NULL;
	PLC_IMAGE	pImg	= NULL;

	if(p3)		m_xMip	= *((UINT*)p3);		// Mipmap Level


	hr = LcDev_CreateImageFromFile(NULL, &pImg, p1, NULL, p2, p4);
	if( LC_FAILED(hr))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Load Texture Failed: %s\n", (char*)p1 );
		return LC_CANNT_FIND_RESOURCE;
	}


	m_nFmt	= pImg->GetFormat();
	m_Type	= pImg->GetType();
	m_ImgW	= pImg->GetImgW();
	m_ImgH	= pImg->GetImgH();
	m_ImgD	= pImg->GetImgD();
	m_ImgC	= pImg->GetImgC();
	pPxlDst = pImg->GetPixel();


	// set the texture width and height
	//m_TexW	= LCXQuantum2Power(m_ImgW);
	//m_TexH	= LCXQuantum2Power(m_ImgH);
	m_TexW	= m_ImgW;
	m_TexH	= m_ImgH;



	if(m_ImgW * m_ImgH >( LC_IMG_MAX_WIDTH * LC_IMG_MAX_HEIGHT) )
	{
		LOGW("Warnning !!! %s File size is out of bounds.\n Only can be operating in PC.\n", (char*)p1);
	}


	// is image size power of 2?
	INT n2 = m_ImgW * m_ImgH;
	if( n2 != LCXQuantum2Power(n2) )
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Image Size(%d x %d) is not power of 2: %s\n", m_ImgW, m_ImgH, (char*)p1 );
		SAFE_RELEASE(	pImg	);
		return LC_CANNT_FIND_RESOURCE;
	}


	glGenTextures(1, &m_nTex);
	if(0 == m_nTex)
	{
		SAFE_RELEASE(	pImg	);
		return LC_CANNT_ALLOC;
	}


	if(m_xMip)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		_LC_Build2DMipmaps(m_nFmt, m_ImgW, m_ImgH, m_ImgD, m_Type, pPxlDst);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, m_nTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, m_nFmt, m_ImgW, m_ImgH, 0, m_nFmt, m_Type, pPxlDst);
	}




	// Release bind and delete Image Resource
	glBindTexture(GL_TEXTURE_2D, 0);


	SAFE_RELEASE(	pImg	);

	return LC_OK;
}







INT CLcTexture::CreateFromBuffer(LC_HANDLE p1	// struct TLC_IMAGE Address
								, LC_HANDLE p2	// Mipmap Level
						 )
{
	BYTE*		pPxlDst = NULL;
	TLC_IMAGE*	pSrc	= NULL;


	if(!p1)
		return LC_CANNT_ALLOC;


	pSrc =(TLC_IMAGE*)p1;


	if(p2)	m_xMip= *((UINT*)p2);	// Mipmap Level


	m_nFmt	= pSrc->ImgF;
	m_Type	= pSrc->Type;

	m_ImgW	= pSrc->ImgW;
	m_ImgH	= pSrc->ImgH;
	m_ImgD	= pSrc->ImgD;
	m_ImgC	= pSrc->ImgC;

	pPxlDst	= pSrc->pPxl;


	// set the texture width and height
	//m_TexW	= LCXQuantum2Power(m_ImgW);
	//m_TexH	= LCXQuantum2Power(m_ImgH);
	m_TexW	= m_ImgW;
	m_TexH	= m_ImgH;



	glGenTextures(1, &m_nTex);
	if(0 == m_nTex)
		return LC_CANNT_ALLOC;



	if(m_xMip)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		_LC_Build2DMipmaps(m_nFmt, m_ImgW, m_ImgH, m_ImgD, m_Type, pPxlDst);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, m_nTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, m_nFmt, m_ImgW, m_ImgH, 0, m_nFmt, m_Type, pPxlDst);
	}



	// Release bind and delete Image Resource
	glBindTexture(GL_TEXTURE_2D, 0);

	return LC_OK;
}




INT CLcTexture::CreateFromMemory(LC_HANDLE p1	// Memory Address
								,LC_HANDLE p2	// Memory Size
								,LC_HANDLE p3	// "Name"
								,LC_HANDLE p4	// Color Key
								,LC_HANDLE p5	// Mipmap Level
								,LC_HANDLE p6	// Convert pixel?
						 )
{
	INT			hr		= 0;

	BYTE*		pPxlDst = NULL;
	PLC_IMAGE	pImg	= NULL;


	if(!p1 || !p2 || 0 >= *((UINT*)p2))
		return LC_EFAIL;


	m_Mem =  p1;
	m_MemS=  *((UINT*)p2);

	if(p3)	strcpy(m_sName, (char*)p3);	// save name
	if(p5)	m_xMip= *((UINT*)p5);		// save mipmap Level



#if defined(__ANDROID__)
	m_Mem = malloc( m_MemS);
	memcpy(m_Mem, p1, m_MemS);
#endif
	



	// Load From Memory
	hr = LcDev_CreateImageFromMemory(NULL, &pImg, m_Mem, &m_MemS, p4, p6);
	if( LC_FAILED(hr))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Load Texture Failed\n");
		return LC_CANNT_FIND_RESOURCE;
	}

	m_nFmt	= pImg->GetFormat();
	m_Type	= pImg->GetType();
	m_ImgW	= pImg->GetImgW();
	m_ImgH	= pImg->GetImgH();
	m_ImgD	= pImg->GetImgD();
	m_ImgC	= pImg->GetImgC();
	pPxlDst = pImg->GetPixel();


	// set the texture width and height
	//m_TexW = LCXQuantum2Power(m_ImgW);
	//m_TexH = LCXQuantum2Power(m_ImgH);
	m_TexW	= m_ImgW;
	m_TexH	= m_ImgH;



	if( 0 >= m_ImgW || 0 >= m_ImgH || NULL == pPxlDst)
		return LC_CANNT_ALLOC;


	if(m_ImgW * m_ImgH >( LC_IMG_MAX_WIDTH * LC_IMG_MAX_HEIGHT) )
	{
		LOGW("Warnning !!! Image size is out of bounds.\n Only can be operating in PC.\n");
	}


	glGenTextures(1, &m_nTex);
	if(0 == m_nTex)
	{
		SAFE_RELEASE(	pImg	);
		return LC_CANNT_ALLOC;
	}



	glBindTexture(GL_TEXTURE_2D, m_nTex);


	if(m_xMip)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		_LC_Build2DMipmaps(m_nFmt, m_ImgW, m_ImgH, m_ImgD, m_Type, pPxlDst);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, m_nTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, m_nFmt, m_ImgW, m_ImgH, 0, m_nFmt, m_Type, pPxlDst);
	}



	// Release bind and delete Image Resource
	glBindTexture(GL_TEXTURE_2D, 0);


	SAFE_RELEASE(	pImg	);

	return LC_OK;
}



INT CLcTexture::Invalid()
{
#if !defined(__ANDROID__)
	return LC_OK;
#endif

	if(m_nTex)
	{
		glDeleteTextures (1, &m_nTex);
		m_nTex = 0;
	}

	return LC_OK;
}


INT CLcTexture::Restore()
{
#if !defined(__ANDROID__)
	return LC_EFAIL;
#endif

	INT			hr		= 0;

	BYTE*		pPxlDst = NULL;
	PLC_IMAGE	pImg	= NULL;
	UINT		Use16	= 0;



	if(2 == this->m_ImgD)
		Use16 = LC_FMT_INDEX16;


	// Load From Memory
	hr = LcDev_CreateImageFromMemory(NULL, &pImg, m_Mem, &m_MemS, &m_ImgC, &Use16);

	if( LC_FAILED(hr))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Load Texture Failed\n");
		return LC_CANNT_FIND_RESOURCE;
	}

	m_nFmt	= pImg->GetFormat();
	m_Type	= pImg->GetType();
	m_ImgW	= pImg->GetImgW();
	m_ImgH	= pImg->GetImgH();
	m_ImgD	= pImg->GetImgD();
	m_ImgC	= pImg->GetImgC();
	pPxlDst = pImg->GetPixel();


	m_TexW	= m_ImgW;
	m_TexH	= m_ImgH;


	if( 0 >= m_ImgW || 0 >= m_ImgH || NULL == pPxlDst)
		return LC_CANNT_ALLOC;


	if(m_ImgW * m_ImgH >( LC_IMG_MAX_WIDTH * LC_IMG_MAX_HEIGHT) )
	{
		LOGW("Warnning !!! Image size is out of bounds.\n Only can be operating in PC.\n");
	}


	glGenTextures(1, &m_nTex);
	if(0 == m_nTex)
	{
		SAFE_RELEASE(	pImg	);
		return LC_CANNT_ALLOC;
	}



	glBindTexture(GL_TEXTURE_2D, m_nTex);


	if(m_xMip)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		_LC_Build2DMipmaps(m_nFmt, m_ImgW, m_ImgH, m_ImgD, m_Type, pPxlDst);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, m_nTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, m_nFmt, m_ImgW, m_ImgH, 0, m_nFmt, m_Type, pPxlDst);
	}



	// Release bind and delete Image Resource
	glBindTexture(GL_TEXTURE_2D, 0);


	SAFE_RELEASE(	pImg	);

	return LC_OK;
}




INT CLcTexture::CreateRenderCopy(LC_HANDLE p1, LC_HANDLE p2)
{
	LcVar_WindowSize((INT*)&m_TexW, (INT*)&m_TexH);

	m_nFmt	= GL_RGB;
	m_Type	= GL_UNSIGNED_BYTE;


	if(p1)	m_ImgW	= *((INT*)p1);
	if(p2)	m_ImgH	= *((INT*)p2);

	m_TexW	= m_ImgW;
	m_TexH	= m_ImgH;

	m_ImgD	= 3;
	m_ImgC	= 0x0;

	if(m_ImgW * m_ImgH >( LC_IMG_MAX_WIDTH * LC_IMG_MAX_HEIGHT) )
		LOGW("Warnning !!! Texture size is out of bounds.\n Only can be operating in PC.\n");


	glGenTextures(1, &m_nTex);

	glBindTexture(GL_TEXTURE_2D, m_nTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, m_nFmt, m_ImgW, m_ImgH, 0, m_nFmt, m_Type, NULL);

	// Release bind and delete Image Resource
	glBindTexture(GL_TEXTURE_2D, 0);

	return LC_OK;
}





INT CLcTexture::BindTexture(INT modulate, INT smp_addrS, INT smp_addrT, INT filter_S, INT filter_T)
{
	if(0 == modulate)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		return LC_OK;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_nTex);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, modulate);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, smp_addrS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, smp_addrT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_T);

	return LC_OK;
}


INT CLcTexture::CopyTexImage2D()
{
	if(1>m_nTex)
		return LC_EFAIL;

	glBindTexture(GL_TEXTURE_2D, m_nTex);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, m_ImgW, m_ImgH, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return LC_OK;
}



// Rendering Copy Texture
INT LcDev_CreateTextureTarget(  char* sCmd			// Null
							, PLC_TEXTURE* pOut		// Output instance
							, LC_HANDLE p1			// width
							, LC_HANDLE p2			// height
							 )
{
	INT			hr = 0;
	CLcTexture*	pObj = NULL;

	*pOut	= NULL;

	pObj = new CLcTexture;

	hr = pObj->CreateRenderCopy(p1, p2);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}



// Create Texture from merged file:
INT LcDev_CreateTextureFromFile(char* sCmd		// Null
					   , PLC_TEXTURE* pOut		// Output instance
					   , LC_HANDLE p1			// File Name
					   , LC_HANDLE p2			// Color Key
					   , LC_HANDLE p3			// Mipmap Level
					   , LC_HANDLE p4			// Convert 16bit?
					 )
{
	INT			hr = 0;

	CLcTexture*	pObj = NULL;

	*pOut		= NULL;


#if defined(__ANDROID__)
	char*		orgFile	= (char*)p1;
	char		sMedia[LC_MAX_PATH2]={0};

	BYTE*		buf = NULL;
	ULONG		len = 0;
	const char*	sApkFile = LcSys_AndroidApkFile(NULL);

	LcStr_ReplacePath(sMedia, (const char*)orgFile, NULL, NULL, NULL, ".gptf");

	hr = Lzip_UnzipbyName(&buf, &len, sApkFile, sMedia, LC_TRUE);
	//LOGI("Create Texture Unzip: %x %ld %d %s\n", (UINT)buf, len, hr, sMedia);


	if(LC_FAILED(hr))
		return LC_CANNT_ALLOC;


	hr = LcDev_CreateTextureFromMemory(NULL, (PLC_TEXTURE*)&pObj, buf, &len, sMedia, p2, p3, p4);
	LOGI("Create Texture From Memory hr: %d\n", hr);

	SAFE_FREE( buf );

	if(LC_FAILED(hr))
		return LC_CANNT_ALLOC;


	*pOut = pObj;
	return LC_OK;
#endif


	pObj = new CLcTexture;

	hr = pObj->CreateFromFile(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}


// Create Texture from specified
// TLC_IMAGE struct value
INT LcDev_CreateTextureFromBuffer(char* sCmd
							  , PLC_TEXTURE* pOut		// Output instance
							  , LC_HANDLE p1			// struct TLC_IMAGE Address
							  , LC_HANDLE p2			// Mipmap Level (NULL == 0 Level, unsigned 32bit)
							 )
{
	INT			hr = 0;
	CLcTexture*	pObj = NULL;

	*pOut	= NULL;
	pObj = new CLcTexture;

	hr = pObj->CreateFromBuffer(p1, p2);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}



// Create Texture from Pixel in Stored Memory
// Only Support LC_IMG_GPT format
INT LcDev_CreateTextureFromMemory(char*        sCmd		// Null
								, PLC_TEXTURE* pOut		// Output instance
								, LC_HANDLE    p1		// Memory Address
								, LC_HANDLE    p2		// Memory Size(UINT)
								, LC_HANDLE    p3		// "Name"
								, LC_HANDLE    p4		// Color Key
								, LC_HANDLE    p5 		// Mipmap Level (NULL == 0 Level, unsigned 32bit)
								, LC_HANDLE    p6	 	// Convert pixel bit 32 ==> 16 option
							 )
{
	INT			hr = 0;
	CLcTexture*	pObj = NULL;

	*pOut	= NULL;
	pObj = new CLcTexture;

	hr = pObj->CreateFromMemory(p1, p2, p3, p4, p5, p6);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}


INT LcDev_CreateRenderCopyTexture(  char*         sCmd	// Null
								, PLC_TEXTURE* pOut	// Output instance.
								, LC_HANDLE      p1	// Texture Width Address. ex) int Width = 512, => &Width
								, LC_HANDLE      p2	// Texture Height Address. ex) int Height = 512, => &Height
							 )
{
	INT			hr = 0;
	CLcTexture*	pObj = NULL;

	*pOut	= NULL;
	pObj = new CLcTexture;

	hr = pObj->CreateRenderCopy(p1, p2);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}



