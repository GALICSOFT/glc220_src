// Interface for the CLcImage class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcImage_H_
#define _LcImage_H_

class CLcImage : public ILcImage
{
public:
	struct PNG_PIXEL
	{
		BYTE*	pPixel;
		INT		nWidth;
		INT		nHeight;
		INT		nChannel;

		BYTE	BgColorR;
		BYTE	BgColorG;
		BYTE	BgColorB;

		PNG_PIXEL();
		PNG_PIXEL(const PNG_PIXEL& r);
		PNG_PIXEL& operator=(const PNG_PIXEL& r);

		void Destroy();
		void SetPixelData(BYTE* _pxl);
	};

protected:
	INT		m_ImgW;								// Image Width
	INT		m_ImgH;								// Image Height
	INT		m_ImgD;								// Image Depth
	INT		m_ImgB;								// Image BitCount
	INT		m_ImgF;								// Image File Format
	BYTE*	m_Data;								// Image Pixel Data

	char	m_sName[ILcImage::MAX_FILE_LEN];	// File Name

public:
	CLcImage();
	virtual ~CLcImage();
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		Query(char* sCmd, void* pData);

	virtual INT		GetWidth()			{	return m_ImgW;	}
	virtual INT		GetHeight()			{	return m_ImgH;	}
	virtual INT		GetDepth()			{	return m_ImgD;	}
	virtual INT		GetBitCount()		{	return m_ImgB;	}
	virtual INT		GetFormat()			{	return m_ImgF;	}
	virtual BYTE*	GetPointer()		{	return m_Data;	}
	virtual const char*	GetName() const {	return m_sName;	}

protected:
	INT LoadPngFile(CLcImage::PNG_PIXEL* pPngOut, char* sFileName);
};


#endif


