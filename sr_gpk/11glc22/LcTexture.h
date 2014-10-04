//	Copyright(C) GaLic Soft. All rights reserved.
//
//	for Texture.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcTexture_H_
#define _LcTexture_H_


class CLcTexture : public ILC_Texture
{
protected:
	UINT	m_nTex	;				// Id
	UINT	m_nFmt	;				// Pixel Format
	UINT	m_Type	;				// Pixel Type


	UINT	m_ImgW	;				// Image Width
	UINT	m_ImgH	;				// Image Height
	UINT	m_ImgD	;				// Image Depth
	UINT	m_ImgC	;				// Image ColorKey
	UINT	m_TexW	;				// Texture Width
	UINT	m_TexH	;				// Texture Height
	INT		m_xMip	;				// Mipmap Level

	LC_HANDLE m_Mem	;				// Owner
	UINT	  m_MemS;				// Texture Height
	UINT      m_xPool;				// Memory Pool

	char	m_sName[LC_MAX_PATH];	// File Name for Search


public:
	CLcTexture ();
	virtual ~CLcTexture();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);// inherit from the ILC_Object
	virtual INT		Release();							// inherit from the ILC_Object



	virtual const char* const GetName()const{	return m_sName;	}
	virtual UINT	GetId()			{	return m_nTex;	}
	virtual	UINT	GetFormat()		{	return m_nFmt;	}
	virtual UINT	GetType()		{	return m_Type;	}

	virtual UINT	GetImgW()		{	return m_ImgW;	}
	virtual UINT	GetImgH()		{	return m_ImgH;	}
	virtual UINT	GetImgD()		{	return m_ImgD;	}
	virtual UINT	GetImgC()		{	return m_ImgC;	}
	virtual UINT	GetTexW()		{	return m_TexW;	}
	virtual UINT	GetTexH()		{	return m_TexH;	}

	virtual	INT		BindTexture(INT modulate, INT smp_addrS, INT smp_addrT, INT filter_S, INT filter_T);	// Binding Texture: nState is not supported. If moduate =0  then Release Binding
	virtual	INT		CopyTexImage2D();

public:
	INT		CreateFromFile(LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0);
	INT		CreateFromBuffer(LC_HANDLE =0, LC_HANDLE =0);
	INT		CreateFromMemory( LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0
							, LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0);

	INT		CreateRenderCopy(LC_HANDLE =0, LC_HANDLE =0);

protected:
	void	Destroy();

	INT		Invalid();
	INT		Restore();
};


#endif


