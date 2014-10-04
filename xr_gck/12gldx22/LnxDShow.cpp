// Implementation of the CLcxDShow class.
//
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <mmsystem.h>
#include <atlbase.h>
#include <stdio.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"

#include "LnxDShow.h"
//#include "d:/_dxsdk2003sum/Samples/C++/DirectShow/BaseClasses/streams.h"
#include <streams.h>

// Define GUID for Texture Renderer
// {71771540-2017-11cf-AE26-0020AFD79767}
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;



// CDShowRender Class Declarations
class CDShowRender : public CBaseVideoRenderer
{
public:
	LPDIRECT3DTEXTURE9	m_pTx;
	
	LONG				m_lVidW;												// Video width
	LONG				m_lVidH;												// Video Height
	LONG				m_lVidP;												// Video Pitch
	
public:
	CDShowRender(LPUNKNOWN pUnk,HRESULT *phr, LPDIRECT3DDEVICE9 pDev);
	virtual ~CDShowRender();
	
	HRESULT		CheckMediaType(const CMediaType *pmt );							// Format acceptable?
	HRESULT		SetMediaType(const CMediaType *pmt );							// Video format notification
	HRESULT		DoRenderSample(IMediaSample *pMediaSample);						// New video sample
	LPDIRECT3DTEXTURE9	GetTexture();
};


class CDShowClip : public CLcxDShow::ILcxDShowClip
{
protected:
	TCHAR					m_sFile[MAX_PATH];
	HWND					m_hWnd		;
	INT						m_nEventID	;
	DWORD					m_dRepeat	;
	
public:
	CComPtr<IGraphBuilder>	m_pGB;												// GraphBuilder
	CComPtr<IMediaControl>	m_pMC;												// Media Control
	CComPtr<IMediaPosition>	m_pMP;												// Media Position
	CComPtr<IMediaEventEx>	m_pME;												// Media EventEx
	
	CComPtr<IBaseFilter>	m_pBF;												// our custom renderer
	
	CDShowRender*			m_pSR;												// DirectShow Texture renderer
	
public:
	CDShowClip();
	virtual ~CDShowClip();
	
//	virtual INT		Create(void* pDev, INT nEventID, char* sFile);
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		ProcessEvent();
	
public:
	virtual INT		Run();
	virtual INT		Stop();
	virtual INT		Reset();
	
	virtual void*	GetTexture();
	virtual INT		GetVideoW();
	virtual INT		GetVideoH();
	virtual	INT		GetEventID();
	
	virtual void	SetRepeat(DWORD dRepeat= 0xFFFFFFFF/*INFINITE*/);
	
	virtual void	SetScnPosX(INT nVal);
	virtual void	SetScnPosY(INT nVal);
	virtual void	SetScnWidth(INT nVal);
	virtual	void	SetScnHeight(INT nVal);
	virtual	void	SetDiffuse(DWORD nVal);
	virtual INT		Draw();
	
protected:
	INT		m_nScnX;
	INT		m_nScnY;
	INT		m_nScnW;
	INT		m_nScnH;
	DWORD	m_dDff;
	
	struct VtxDRHW
	{
		D3DXVECTOR4		p;
		DWORD			d;
		FLOAT			u,v;
		
		VtxDRHW(): d(0xFFFFFFFF){};
		
		enum	{	FVF = (D3DFVF_XYZRHW| D3DFVF_DIFFUSE| D3DFVF_TEX1),	};
	};
	
	VtxDRHW		m_pVtx[4];
};


////////////////////////////////////////////////////////////////////////////////
//
// Implementation of the CDShowClip class.
//
////////////////////////////////////////////////////////////////////////////////


void CDShowClip::SetScnPosX(INT nVal)
{
	m_nScnX = nVal;
	
	m_pVtx[0].p.x = (FLOAT)m_nScnX	;
	m_pVtx[1].p.x = (FLOAT)m_nScnX + m_nScnW;
	m_pVtx[2].p.x = (FLOAT)m_nScnX			;
	m_pVtx[3].p.x = (FLOAT)m_nScnX + m_nScnW;
}

void CDShowClip::SetScnPosY(INT nVal)
{
	m_nScnY = nVal;
	
	m_pVtx[0].p.y = (FLOAT)m_nScnY	;
	m_pVtx[1].p.y = (FLOAT)m_nScnY			;
	m_pVtx[2].p.y = (FLOAT)m_nScnY + m_nScnH;
	m_pVtx[3].p.y = (FLOAT)m_nScnY + m_nScnH;
}

void CDShowClip::SetScnWidth(INT nVal)
{
	m_nScnW = nVal;
	
	m_pVtx[0].p.x = (FLOAT)m_nScnX	;
	m_pVtx[1].p.x = (FLOAT)m_nScnX + m_nScnW;
	m_pVtx[2].p.x = (FLOAT)m_nScnX			;
	m_pVtx[3].p.x = (FLOAT)m_nScnX + m_nScnW;
}

void CDShowClip::SetScnHeight(INT nVal)
{
	m_nScnH = nVal;
	
	m_pVtx[0].p.y = (FLOAT)m_nScnY	;
	m_pVtx[1].p.y = (FLOAT)m_nScnY			;
	m_pVtx[2].p.y = (FLOAT)m_nScnY + m_nScnH;
	m_pVtx[3].p.y = (FLOAT)m_nScnY + m_nScnH;
}


void CDShowClip::SetDiffuse(DWORD nVal)
{
	m_dDff = nVal;
	
	m_pVtx[0].d = m_dDff;
	m_pVtx[1].d = m_dDff;
	m_pVtx[2].d = m_dDff;
	m_pVtx[3].d = m_dDff;
}


INT CDShowClip::Draw()
{
	LPDIRECT3DDEVICE9	pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	LPDIRECT3DTEXTURE9	pTx = (LPDIRECT3DTEXTURE9)GetTexture();
	
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	pDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	//	pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	
	pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	
	pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	
	
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	
	
	pDev->SetTexture(0, pTx);
	//	pDev->SetTexture(0, NULL);
	
	pDev->SetFVF(CDShowClip::VtxDRHW::FVF);
	pDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_pVtx, sizeof (CDShowClip::VtxDRHW));
	
	
	pDev->SetTexture(0, NULL);
	
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	
	pDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
	
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	
	long lEventCode;
	long lParam1;
	long lParam2;
	HRESULT hr;
	
	if (!m_pME)
		return 0;
	
	// Check for completion events
	hr = m_pME->GetEvent(&lEventCode, (LONG_PTR *) &lParam1, (LONG_PTR *) &lParam2, 0);

	if (SUCCEEDED(hr))
	{
		// If we have reached the end of the media file, reset to beginning
		if (EC_COMPLETE == lEventCode) 
		{
			//hr = m_pMP->put_CurrentPosition(0);
			hr = m_pME->FreeEventParams(lEventCode, lParam1, lParam2);
			
//			if(m_pSR->m_pTx)
//			{
//				m_pSR->m_pTx->Release();
//				m_pSR->m_pTx = NULL;
//			}
			
			return -1;
		}
	}
	
	return 0;
}




CDShowClip::CDShowClip()
{
	m_hWnd		= NULL;
	m_nEventID	= NULL;
	m_dRepeat	= 1;
	
	m_nScnX	= 0;
	m_nScnY	= 0;
	m_nScnW	= 400;
	m_nScnH	= 300;
	m_dDff	= 0xFFFFFFFF;
	
}



CDShowClip::~CDShowClip()
{
	Destroy();
}


void CDShowClip::Destroy()
{
	Stop();
	
	if (m_pME)
		m_pME->SetNotifyWindow((OAHWND)NULL, 0, 0);
	
	if(m_pSR && m_pSR->m_pTx)
	{
		m_pSR->m_pTx->Release();
		m_pSR->m_pTx = NULL;
	}
}


//INT	 CDShowClip::Create(void* pDev, INT nEventID, char* sFile)
INT CDShowClip::Create(void* p1, void* p2, void* p3, void* p4)
{
	HRESULT hr = S_OK;

	char*					sFile=NULL;
	CComPtr<IBaseFilter>    pFSrc;          // Source Filter
	CComPtr<IPin>           pFSrcPinOut;    // Source Filter Output Pin   
	D3DDEVICE_CREATION_PARAMETERS d3dParam;
	

	m_nEventID	= *((INT*)p1);
	sFile		= (char*)p2;
	
	D3DPOOL d3dPool = D3DPOOL_MANAGED;
	
	for(INT i=0; i<4; ++i)
	{
		m_pVtx[i].p.z = (	0.f	);
		m_pVtx[i].p.w = (	1.f	);
		m_pVtx[i].d = m_dDff;
	}
	
	m_pVtx[0].p.x = (FLOAT)m_nScnX	;
	m_pVtx[0].p.y = (FLOAT)m_nScnY	;
	m_pVtx[0].u	  = 0.f;
	m_pVtx[0].v	  = 1.f - 0.f;
	
	m_pVtx[1].p.x = (FLOAT)m_nScnX + m_nScnW;
	m_pVtx[1].p.y = (FLOAT)m_nScnY			;
	m_pVtx[1].u	  = 1.f;
	m_pVtx[1].v	  = 1.f - 0.f;
	
	m_pVtx[2].p.x = (FLOAT)m_nScnX			;
	m_pVtx[2].p.y = (FLOAT)m_nScnY + m_nScnH;
	m_pVtx[2].u	  = 0.f;
	m_pVtx[2].v	  = 1.f - 1.f;
	
	m_pVtx[3].p.x = (FLOAT)m_nScnX + m_nScnW;
	m_pVtx[3].p.y = (FLOAT)m_nScnY + m_nScnH;
	m_pVtx[3].u	  = 1.f;
	m_pVtx[3].v	  = 1.f - 1.f;
	
	m_pVtx[0].d = m_dDff;
	m_pVtx[1].d = m_dDff;
	m_pVtx[2].d = m_dDff;
	m_pVtx[3].d = m_dDff;

	
	strcpy(m_sFile, sFile);


	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	
	if(SUCCEEDED(pDev->GetCreationParameters(&d3dParam)))
		m_hWnd = d3dParam.hFocusWindow;
	
	
	// Create the filter graph
	if (FAILED(m_pGB.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)))
	{
		MessageBox(m_hWnd, "CoCreateInstance Failed.\nWe need CoInitialize.", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	
	// Create the Texture Renderer object
	m_pSR = new CDShowRender(NULL, &hr, pDev);
	
	
	if (FAILED(hr) || !m_pSR)
	{
		MessageBox(m_hWnd, "Could not create texture renderer object", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	
	// Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
	m_pBF = m_pSR;
	if (FAILED(hr = m_pGB->AddFilter(m_pBF, L"TEXTURERENDERER")))
	{
		MessageBox(m_hWnd, "Could not add renderer filter to graph", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	
	
	WCHAR wFileName[MAX_PATH];
	m_sFile[MAX_PATH-1] = 0;  // NULL-terminate
	wFileName[MAX_PATH-1] = 0;    // NULL-terminate
	
	USES_CONVERSION;
	wcsncpy(wFileName, T2W(m_sFile), NUMELMS(wFileName));
	
	// Add the source filter to the graph.
	hr = m_pGB->AddSourceFilter (wFileName, L"SOURCE", &pFSrc);
	
	// If the media file was not found, inform the user.
	if (hr == VFW_E_NOT_FOUND || FAILED(hr))
	{
		MessageBox(m_hWnd, "Could not add source filter to graph", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	
	if (FAILED(pFSrc->FindPin(L"Output", &pFSrcPinOut)))
	{
		MessageBox(m_hWnd, "Could not output pin", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	
	
	// Render the source filter's output pin.  The Filter Graph Manager
	// will connect the video stream to the loaded CDShowRender
	// and will load and connect an audio renderer (if needed).
	
	if (FAILED(m_pGB->Render(pFSrcPinOut)))
	{
		MessageBox(m_hWnd, "Could not render source output pin", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	
	
	// Get the graph's media control, event & position interfaces
	m_pGB.QueryInterface(&m_pMC);
	m_pGB.QueryInterface(&m_pMP);
	m_pGB.QueryInterface(&m_pME);
	
	m_pME->SetNotifyWindow((OAHWND)m_hWnd, m_nEventID, (LONG)this);
	
	// Start the graph running;
	if (FAILED(hr = m_pMC->Run()))
	{
		MessageBox(m_hWnd, "Could not run the DirectShow graph", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	
	return 0;
}



INT CDShowClip::ProcessEvent()
{
	long lEventCode;
	long lParam1;
	long lParam2;
	HRESULT hr;
	
	if (!m_pME)
		return 0;
	
	
	// Check for completion events
	while(SUCCEEDED( m_pME->GetEvent(&lEventCode, (LONG_PTR *) &lParam1, (LONG_PTR *) &lParam2, 0)))
	{
		// Free any memory associated with this event
		hr = m_pME->FreeEventParams(lEventCode, lParam1, lParam2);
		
		if(EC_PAUSED == lEventCode)
		{
			return 0;
		}
		
		if(EC_CLOCK_CHANGED == lEventCode)
		{
			return 0;
		}
		
		// If we have reached the end of the media file, reset to beginning
		if (EC_COMPLETE == lEventCode) 
		{
			if(0xFFFFFFFF != m_dRepeat)
			{
				hr = m_pME->FreeEventParams(lEventCode, lParam1, lParam2);
				
				if(m_pSR->m_pTx)
				{
					m_pSR->m_pTx->Release();
					m_pSR->m_pTx = NULL;
				}
				
				return -1;
			}
			
			hr = m_pMP->put_CurrentPosition(0);
			
			if (FAILED(hr))
			{
				if (FAILED(hr = m_pMC->Stop()))
					break;
				
				if (FAILED(hr = m_pMC->Run()))
					break;
			}
			
		}
	}
	
	return 0;
}


void* CDShowClip::GetTexture()
{
	if(m_pSR)
		return m_pSR->GetTexture();
	
	return NULL;
}


INT CDShowClip::GetVideoW()
{
	return m_pSR->m_lVidW;
}

INT CDShowClip::GetVideoH()
{
	return m_pSR->m_lVidH;
}


INT CDShowClip::GetEventID()
{
	return m_nEventID;
}


INT CDShowClip::Run()
{
	if(m_pMC)
		return m_pMC->Run();
	
	return 0;
}


INT CDShowClip::Stop()
{
	if(m_pMC)
		return m_pMC->Stop();
	
	return 0;
}


INT CDShowClip::Reset()
{
	// 멈추고
	if (!(!m_pMC))
		m_pMC->Stop();
	
	// 클립을 처음으로 이동
	if (!(!m_pMP))
		m_pMP->put_CurrentPosition(0);
	
	// 다시 런
	if (!(!m_pMC))
		m_pMC->Run();
	
	return 0;
}



void CDShowClip::SetRepeat(DWORD dRepeat)
{
	m_dRepeat = dRepeat;
}



////////////////////////////////////////////////////////////////////////////////
//
// Implementation of the CDShowRender class.
//
////////////////////////////////////////////////////////////////////////////////

CDShowRender::CDShowRender( LPUNKNOWN pUnk, HRESULT *phr, LPDIRECT3DDEVICE9 pDev)
: CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer), 
					 NAME("Texture Renderer"), pUnk, phr)
{
	ASSERT(phr);
	if (phr)
		*phr = S_OK;
	
	pDev	= pDev;
	m_pTx	= NULL;
}


CDShowRender::~CDShowRender()
{
}


HRESULT CDShowRender::CheckMediaType(const CMediaType *pmt)
{
	HRESULT   hr = -1;
	VIDEOINFO *pvi=0;
	
	CheckPointer(pmt,E_POINTER);
	
	// Reject the connection if this is not a video type
	if( *pmt->FormatType() != FORMAT_VideoInfo )
	{
		return E_INVALIDARG;
	}
	
	// Only accept RGB24 video
	pvi = (VIDEOINFO *)pmt->Format();
	
	if(IsEqualGUID( *pmt->Type(),    MEDIATYPE_Video)  &&
		IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24))
	{
		hr = S_OK;
	}
	
	return hr;
}


HRESULT CDShowRender::SetMediaType(const CMediaType *pmt)
{
	HRESULT hr;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	
	UINT uintWidth = 2;
	UINT uintHeight = 2;
	
	// Retrive the size of this media type
	
	VIDEOINFO *pviBmp;                      // Bitmap info header
	pviBmp = (VIDEOINFO *)pmt->Format();
	
	m_lVidW  = pviBmp->bmiHeader.biWidth;
	m_lVidH = abs(pviBmp->bmiHeader.biHeight);
	m_lVidP  = (m_lVidW * 3 + 3) & ~(3); // We are forcing RGB24
	
	hr = pDev->CreateTexture(m_lVidW, m_lVidH, 1, 0, D3DFMT_X8R8G8B8,D3DPOOL_MANAGED,	&m_pTx, NULL);
	
	if( FAILED(hr))
	{
		HWND hWnd = NULL;
		D3DDEVICE_CREATION_PARAMETERS d3dParam;	
		
		if(pDev && SUCCEEDED(pDev->GetCreationParameters(&d3dParam)))
		{
			hWnd = d3dParam.hFocusWindow;
		}
		
		MessageBox(hWnd, "Could not create the DShow texture", "Err", MB_ICONEXCLAMATION);
		return -1;
	}
	
	return S_OK;
}


// DoRenderSample: A sample has been delivered. Copy it to the texture.

HRESULT CDShowRender::DoRenderSample( IMediaSample * pSample )
{
	BYTE  *pBmpBuffer, *pTxtBuffer; // Bitmap buffer, texture buffer
	LONG  lTxtPitch;                // Pitch of bitmap, texture
	
	BYTE  * pbS = NULL;
	DWORD * pdwS = NULL;
	DWORD * pdwD = NULL;
	UINT row, col, dwordWidth;
	
	CheckPointer(pSample,E_POINTER);
	CheckPointer(m_pTx,E_UNEXPECTED);
	
	// Get the video bitmap buffer
	pSample->GetPointer( &pBmpBuffer );
	
	// Lock the Texture
	D3DLOCKED_RECT d3dlr;
	
	if( FAILED(m_pTx->LockRect(0, &d3dlr, 0, 0)))
		return -1;
	
	
	// Get the texture buffer & pitch
	pTxtBuffer = static_cast<byte *>(d3dlr.pBits);
	lTxtPitch = d3dlr.Pitch;
	
	
	// Copy the bits    
	dwordWidth = m_lVidW / 4; // aligned width of the row, in DWORDS
	// (pixel by 3 bytes over sizeof(DWORD))
	
	for( row = 0; row< (UINT)m_lVidH; row++)
	{
		pdwS = ( DWORD*)pBmpBuffer;
		pdwD = ( DWORD*)pTxtBuffer;
		
		for( col = 0; col < dwordWidth; col ++ )
		{
			pdwD[0] =  pdwS[0] | 0xFF000000;
			pdwD[1] = ((pdwS[1]<<8)  | 0xFF000000) | (pdwS[0]>>24);
			pdwD[2] = ((pdwS[2]<<16) | 0xFF000000) | (pdwS[1]>>16);
			pdwD[3] = 0xFF000000 | (pdwS[2]>>8);
			pdwD +=4;
			pdwS +=3;
		}
		
		// we might have remaining (misaligned) bytes here
		pbS = (BYTE*) pdwS;
		for( col = 0; col < (UINT)m_lVidW % 4; col++)
		{
			*pdwD = 0xFF000000 |
				(pbS[2] << 16) |
				(pbS[1] <<  8) |
				(pbS[0]);
			pdwD++;
			pbS += 3;           
		}
		
		pBmpBuffer  += m_lVidP;
		pTxtBuffer += lTxtPitch;
	}// for rows
	
	
	// Unlock the Texture
	if (FAILED(m_pTx->UnlockRect(0)))
		return -1;
	
	return S_OK;
}


LPDIRECT3DTEXTURE9 CDShowRender::GetTexture()
{
	return m_pTx;
}




////////////////////////////////////////////////////////////////////////////////
//
// Implementation of the CLcxDShow class.
//
////////////////////////////////////////////////////////////////////////////////

CLcxDShow::CLcxDShow()
{
	m_pVid		= NULL;
}

CLcxDShow::~CLcxDShow()
{
	Destroy();	
}


void CLcxDShow::Destroy()
{
	if(m_pVid)
	{
		delete m_pVid;
		m_pVid = NULL;
	}
}



DWORD CLcxDShow::GetType()
{
	return LC_SMD_SH_A;
}


void CLcxDShow::Play()
{
	if(m_pVid)
	{
		m_pVid->Run();
	}
}

void CLcxDShow::Stop()
{
	if(m_pVid)
	{
		m_pVid->Stop();
	}
}

void CLcxDShow::Reset()
{
	if(m_pVid)
	{
		m_pVid->Reset();
	}
}

void CLcxDShow::Pause()
{
}

void CLcxDShow::SetVolume(LONG dVol)
{
}

LONG CLcxDShow::GetVolume()
{
	return 0;
}

void CLcxDShow::SetRepeat(DWORD dRepeat)
{
}


DWORD CLcxDShow::GetStatus()
{
	return 0;
}


void* CLcxDShow::GetTexture()
{
	if(m_pVid)
		return m_pVid->GetTexture();
	
	return NULL;
}



INT CLcxDShow::GetVideoW()
{
	return m_pVid->GetVideoW();
}

INT CLcxDShow::GetVideoH()
{
	return m_pVid->GetVideoH();
}



INT CLcxDShow::Query(char* sCmd, void* pData)
{
	if(0==_stricmp("Get Process Event", sCmd))
	{
		*((INT*)pData) = ProcessEvent();
		return 0;
	}
	
	else if(0==_stricmp("Get Event ID", sCmd))
	{
		*((INT*)pData) = GetEventID();
		return 0;
	}
	
	
	
	else if(0==_stricmp("Set Screen Pos X", sCmd))
	{
		INT	nVal = *((INT*)pData);
		m_pVid->SetScnPosX(nVal);
		return 0;
	}
	
	else if(0==_stricmp("Set Screen Pos Y", sCmd))
	{
		INT	nVal = *((INT*)pData);
		m_pVid->SetScnPosY(nVal);
		return 0;
	}
	
	else if(0==_stricmp("Set Screen Width", sCmd))
	{
		INT	nVal = *((INT*)pData);
		m_pVid->SetScnWidth(nVal);
		return 0;
	}
	
	else if(0==_stricmp("Set Screen Height", sCmd))
	{
		INT	nVal = *((INT*)pData);
		m_pVid->SetScnHeight(nVal);
		return 0;
	}

	else if(0==_stricmp("Set Diffuse", sCmd))
	{
		DWORD	nVal = *((INT*)pData);
		m_pVid->SetDiffuse(nVal);
		return 0;
	}
	
	else if(0==_stricmp("Draw", sCmd))
	{
		*((INT*)pData) = m_pVid->Draw();
		return 0;
	}
	
	return -1;
}





//INT CLcxDShow::Create(LPDIRECT3DDEVICE9 pDev, TCHAR* sFile)
INT CLcxDShow::Create(void* p1, void* p2, void* p3, void* p4)
{
	if(!m_pVid)
	{
		m_pVid = new CDShowClip;
		
		if(FAILED(m_pVid->Create(p1, p2, p3, p4)))
			return -1;
	}
	
	return 0;
}


INT CLcxDShow::ProcessEvent()
{
	if(m_pVid)
		return m_pVid->ProcessEvent();
	
	return 0;
}



INT CLcxDShow::GetEventID()
{
	if(m_pVid)
		return m_pVid->GetEventID();
	
	return 0;
}





INT	LcSmd_CreateDShowFromFile(ILcSmd** pData
							  , void* pDev			// LPDIRECT3DDEVICE9 pointer
							  , INT nEvntId
							  , char* sFile)
{
	*pData = NULL;

	ILcSmd*	pObj = NULL;

	pObj = new CLcxDShow;

	if(FAILED( pObj->Create(pDev, &nEvntId, sFile) ))
	{
		delete pObj;
		return -1;
	}

	*pData = pObj;
	return 0;
}