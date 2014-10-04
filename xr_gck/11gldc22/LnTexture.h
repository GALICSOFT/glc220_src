// Interface for the CLcTex class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcTex_H_
#define _LcTex_H_

#pragma warning(disable : 4786)
#include <string>
#include <map>
using namespace std;



class CLcTex : public ILC_Texture
{
public:
	struct TLcTex
	{
		PDTX		m_pTx;					// D3DTexture Pointer
		DWORD		m_dC;					// Creation color Key
		DWORD		m_dF;					// Creation Filter

		DWORD		m_dP;					// Pool Types

		DIMG		m_Inf;
		char		m_sF[MAX_PATH];			// File Name

		TLcTex();
		void Release();
	};


protected:
	INT			m_nId;
	TLcTex		m_LcTx;

public:
	CLcTex();
	CLcTex(	INT _nId
			, PDTX _pTx
			, DWORD _dC
			, DWORD _dF
			, DWORD _dPool
			, const DIMG* _Inf
			, const char* _sFile);

	virtual ~CLcTex();
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		Query(char* sCmd, void* pData);


	virtual DWORD	GetWidth();					// Get Width
	virtual DWORD	GetHeight();				// Get Height
	virtual DWORD	GetDepth();					// Get Depth
	virtual DWORD	GetMipLevel();				// Get Creation MipLevel
	virtual DWORD	GetFormat();				// Get enumeration from D3DFORMAT
	virtual DWORD	GetResourceType();			// Get enumeration from D3DRESOURCETYPE
	virtual DWORD	GetFileFormat();			// Get enumeration from D3DXIMAGE_FILEFORMAT

	virtual char*	GetSourceName();			// Get Source Name or File Name
	virtual void*	GetPointer();				// Get D3DTexture Pointer

	virtual DWORD	GetColorKey();				// Get Creation color Key
	virtual DWORD	GetFilter();				// Get Creation Filter
	virtual void	GetRECT(RECT* rc);			// Get RECT


	void	SetId(INT nId);
	void	SetTx(void* pTx);
	void	SetColorKey(DWORD dColorKey);
	void	SetFilter(DWORD dFilter);
	void	SetInfo(DIMG* pInf);
	void	SetFileName(char* sFile);
};





typedef	map<string, PLC_TEXTURE >	mpTLcTex;
typedef	mpTLcTex::iterator			itTLcTex;

class CLcTexMng : public ILcTexMng
{
protected:
	INT			m_nId;					// Manager Id
	char		m_sName[MAX_PATH];		// Manager Name

	mpTLcTex	m_lsTex;

public:
	CLcTexMng();
	virtual ~CLcTexMng();
	
	// Basic Interface
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();

	virtual INT		Query(char* sCmd, void* pData);


	// Management Interface
	virtual INT		Find(char* sName, PLC_TEXTURE* pData);
	virtual INT		FindCreate(char* sCmd
								, PLC_TEXTURE* pData
								, void* pVal1
								, DWORD dColorKey=0x00FFFFFF
								, DWORD dFilter= (1<<0)				// D3DMX_FILTER_NONE
								, DWORD Pool= 2						// D3DPOOL_SYSTEMMEM
								);

};



#endif

