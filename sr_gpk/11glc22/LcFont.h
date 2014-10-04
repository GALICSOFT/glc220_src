// Interface for LcFont.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcFont_H_
#define _LcFont_H_

#include <LcType.h>


class CLcFont : public ILC_Font
{
protected:
	LC_HANDLE		m_FtFcr;		// FT_Face
	INT				m_FntOrgH;		// Original Font Height
	INT				m_FntDstH;		// Destination Font Height
	INT				m_resH;			// Horizental Resolution
	INT				m_resV;			// Vertical Resolution

	INT				m_StrLen;		// Source String Length
	char*			m_Str;			// Source String
	INT				m_Algn;			// Alignment
	LCXCOLOR		m_vDif;			// Color
	LCXVECTOR2		m_vPos;			// Position
	LCXVECTOR2		m_vScl;			// Scaling
	INT				m_ChCnt;		// Count of characters
	FLOAT*			m_ChBgn;		// Begin position of character

	PLC_SPRITE	m_pSpm;
	PLC_TEXTURE		m_pTex;
	LCXRECT			m_Rc;

	FLOAT			m_Sclup;		// font scaling up

public:
	CLcFont();
	virtual ~CLcFont();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);	// Write Attribute Value
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);	// Read Attribute Value

	virtual INT		SetString  (const char* sMsg);
	virtual	INT		SetColor   (const LC_COLORF* v);
	virtual INT		SetPosition(const LC_FLOAT2*);
	virtual INT		SetScaling (const LC_FLOAT2*);

	virtual INT		DrawTxt    (PLC_SPRITE pSprite= NULL		// Rendering with sprite pointer
								, INT	Begin= 0				// Begin String Index	start is 0
								, INT	End  =-1				// End string Index
								, const LC_FLOAT2* vcTrn = NULL	// Position
								, const LC_COLORF* color = NULL	// Color
								);

	virtual const char * const GetString  () const { return m_Str;		}
	virtual const FLOAT* const GetColor   () const { return &m_vDif.r;	}
	virtual const FLOAT* const GetPosition() const { return &m_vPos.x;	}
	virtual PLC_TEXTURE        GetTexture()  const { return m_pTex;		}

	virtual INT	GetRect(LC_RECTF* pRc, INT Idx= LC_DEFAULT);
	virtual INT SetupVtxLst(LC_FLOAT2* OutPos, LC_FLOAT2* OutTex);

public:
	INT		CreateFromFile(LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0);
	INT		CreateFromMemory(LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0);
	void	Destroy();

	static LC_HANDLE	m_FTTontLib;		// FT_Library
	static LC_HANDLE	m_FTTdef;			// FT_Face
	static INT			m_RefFont;

protected:
	INT		BuildTexture();
};


#endif

