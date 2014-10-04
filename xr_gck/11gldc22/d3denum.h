// File: D3DEnum.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _D3DENUM_H_
#define _D3DENUM_H_

// typedef BOOL(* CONFIRMDEVICECALLBACK)(D3DCAPS9* pCaps, VertexProcessingType nType, D3DDeviceCombo* pDeviceCombo);
typedef BOOL(* CONFIRMDEVICECALLBACK)(D3DCAPS9* pCaps, INT nType, void* pDeviceCombo);



// Enumerates available D3D adapters, devices, modes, etc.
class CD3DEnum
{
public:
	// Desc: A growable array
	class CArrayList
	{
	public:
		// Indicates how data should be stored in a CArrayList
		enum ArrayListType
		{
			AL_VALUE,       // entry data is copied into the list
			AL_REFERENCE,   // entry pointers are copied into the list
		};

	protected:
		ArrayListType	m_ArrayListType;
		void*			m_pData;
		UINT			m_BytesPerEntry;
		UINT			m_NumEntries;
		UINT			m_NumEntriesAllocated;

	public:
		CArrayList( ArrayListType Type, UINT BytesPerEntry = 0 );
		~CArrayList();

		HRESULT Add( void* pEntry );
		void	Remove( UINT Entry );
		void*	GetPtr( UINT Entry );
		UINT	Count() { return m_NumEntries; }
		BOOL	Contains( void* pEntryData );
		void	Clear() { m_NumEntries = 0; }
	};



	// Enumeration of all possible D3D vertex processing types.
	enum VertexProcessingType
	{
		SOFTWARE_VP,
		MIXED_VP,
		HARDWARE_VP,
		PURE_HARDWARE_VP
	};


	// Info about a display adapter.
	struct D3DAdapterInfo
	{
		INT						AdapterOrdinal;
		D3DADAPTER_IDENTIFIER9	AdapterIdentifier;
		CArrayList*				pDisplayModeList;	// List of D3DDISPLAYMODEs
		CArrayList*				pDeviceInfoList;	// List of D3DDeviceInfo pointers
		~D3DAdapterInfo();
	};


	// Info about a D3D device, including a list of D3DDeviceCombos (see below) that work with the device.
	struct D3DDeviceInfo
	{
		INT			AdapterOrdinal;
		D3DDEVTYPE	DevType;
		D3DCAPS9	Caps;
		CArrayList* pDeviceComboList; // List of D3DDeviceCombo pointers
		~D3DDeviceInfo();
	};


	// A depth/stencil buffer format that is incompatible with a multisample type.
	struct D3DDSMSConflict
	{
		D3DFORMAT			DSFormat;
		D3DMULTISAMPLE_TYPE MSType;
	};


	// A combination of adapter format, back buffer format, and windowed/fullscreen 
	// that is compatible with a particular D3D device (and the app).
	struct D3DDeviceCombo
	{
		INT			AdapterOrdinal;
		D3DDEVTYPE	DevType;
		D3DFORMAT	AdapterFormat;
		D3DFORMAT	BackBufferFormat;
		BOOL		IsWindowed;
		CArrayList* pDepthStencilFormatList; // List of D3DFORMATs
		CArrayList* pMultiSampleTypeList; // List of D3DMULTISAMPLE_TYPEs
		CArrayList* pMultiSampleQualityList; // List of DWORDs (number of quality 
		// levels for each multisample type)
		CArrayList* pDSMSConflictList; // List of D3DDSMSConflicts
		CArrayList* pVertexProcessingTypeList; // List of VertexProcessingTypes
		CArrayList* pPresentIntervalList; // List of D3DPRESENT_INTERVALs
		
		~D3DDeviceCombo();
	};




private:
	IDirect3D9* m_pD3D;
	
public:
	CArrayList* m_pAdapterInfoList;
	// The following variables can be used to limit what modes, formats, 
	// etc. are enumerated.  Set them to the values you want before calling Enumerate().

	CONFIRMDEVICECALLBACK ConfirmDeviceCallback;

	UINT		AppMinFullscreenWidth;
	UINT		AppMinFullscreenHeight;
	UINT		AppMinColorChannelBits; // min color bits per channel in adapter format
	UINT		AppMinAlphaChannelBits; // min alpha bits per pixel in back buffer format
	UINT		AppMinDepthBits;
	UINT		AppMinStencilBits;
	BOOL		AppUsesDepthBuffer;
	BOOL		AppUsesMixedVP; // whether app can take advantage of mixed vp mode
	BOOL		AppRequiresWindowed;
	BOOL		AppRequiresFullscreen;
	CArrayList* m_pAllowedAdapterFormatList; // list of D3DFORMATs

public:	
	CD3DEnum();
	~CD3DEnum();

	void SetD3D(IDirect3D9* pD3D) { m_pD3D = pD3D; }
	HRESULT Enumerate();

private:
	HRESULT EnumerateDevices( D3DAdapterInfo* pAdapterInfo, CArrayList* pAdapterFormatList );
	HRESULT EnumerateDeviceCombos( D3DDeviceInfo* pDeviceInfo, CArrayList* pAdapterFormatList );
	void BuildDepthStencilFormatList( D3DDeviceCombo* pDeviceCombo );
	void BuildMultiSampleTypeList( D3DDeviceCombo* pDeviceCombo );
	void BuildDSMSConflictList( D3DDeviceCombo* pDeviceCombo );
	void BuildVertexProcessingTypeList( D3DDeviceInfo* pDeviceInfo, D3DDeviceCombo* pDeviceCombo );
	void BuildPresentIntervalList( D3DDeviceInfo* pDeviceInfo, D3DDeviceCombo* pDeviceCombo );
};




// Desc: Current D3D settings: adapter, device, mode, formats, etc.
class CD3DSettings
{
public:
	BOOL IsWindowed;
	
	CD3DEnum::D3DAdapterInfo* pWindowed_AdapterInfo;
	CD3DEnum::D3DDeviceInfo* pWindowed_DeviceInfo;
	CD3DEnum::D3DDeviceCombo* pWindowed_DeviceCombo;
	
	D3DDISPLAYMODE		Windowed_DisplayMode; // not changable by the user
	D3DFORMAT			Windowed_DepthStencilBufferFormat;
	D3DMULTISAMPLE_TYPE	Windowed_MultisampleType;
	DWORD				Windowed_MultisampleQuality;

	INT					Windowed_VertexProcessingType;
	UINT				Windowed_PresentInterval;
	INT					Windowed_Width;
	INT					Windowed_Height;
	
	CD3DEnum::D3DAdapterInfo* pFullscreen_AdapterInfo;
	CD3DEnum::D3DDeviceInfo* pFullscreen_DeviceInfo;
	CD3DEnum::D3DDeviceCombo* pFullscreen_DeviceCombo;
	
	D3DDISPLAYMODE		Fullscreen_DisplayMode; // changable by the user
	D3DFORMAT			Fullscreen_DepthStencilBufferFormat;
	D3DMULTISAMPLE_TYPE	Fullscreen_MultisampleType;
	DWORD				Fullscreen_MultisampleQuality;
	INT					Fullscreen_VertexProcessingType;
	UINT				Fullscreen_PresentInterval;
	
	CD3DEnum::D3DAdapterInfo* PAdapterInfo() { return IsWindowed ? pWindowed_AdapterInfo : pFullscreen_AdapterInfo; }
	CD3DEnum::D3DDeviceInfo* PDeviceInfo() { return IsWindowed ? pWindowed_DeviceInfo : pFullscreen_DeviceInfo; }
	CD3DEnum::D3DDeviceCombo* PDeviceCombo() { return IsWindowed ? pWindowed_DeviceCombo : pFullscreen_DeviceCombo; }
	
	INT			AdapterOrdinal() { return PDeviceCombo()->AdapterOrdinal; }
	D3DDEVTYPE	DevType() { return PDeviceCombo()->DevType; }
	D3DFORMAT	BackBufferFormat() { return PDeviceCombo()->BackBufferFormat; }
	
	D3DDISPLAYMODE	DisplayMode() { return IsWindowed ? Windowed_DisplayMode : Fullscreen_DisplayMode; }
	void			SetDisplayMode(D3DDISPLAYMODE value) { if (IsWindowed) Windowed_DisplayMode = value; else Fullscreen_DisplayMode = value; }
	
	D3DFORMAT	DepthStencilBufferFormat() { return IsWindowed ? Windowed_DepthStencilBufferFormat : Fullscreen_DepthStencilBufferFormat; }
	void		SetDepthStencilBufferFormat(D3DFORMAT value) { if (IsWindowed) Windowed_DepthStencilBufferFormat = value; else Fullscreen_DepthStencilBufferFormat = value; }
	
	D3DMULTISAMPLE_TYPE MultisampleType() { return IsWindowed ? Windowed_MultisampleType : Fullscreen_MultisampleType; }
	void				SetMultisampleType(D3DMULTISAMPLE_TYPE value) { if (IsWindowed) Windowed_MultisampleType = value; else Fullscreen_MultisampleType = value; }
	
	DWORD	MultisampleQuality() { return IsWindowed ? Windowed_MultisampleQuality : Fullscreen_MultisampleQuality; }
	void	SetMultisampleQuality(DWORD value) { if (IsWindowed) Windowed_MultisampleQuality = value; else Fullscreen_MultisampleQuality = value; }
	
	INT		GetVertexProcessingType() { return IsWindowed ? Windowed_VertexProcessingType : Fullscreen_VertexProcessingType; }
	void	SetVertexProcessingType(INT value) { if (IsWindowed) Windowed_VertexProcessingType = value; else Fullscreen_VertexProcessingType = value; }
	
	UINT	PresentInterval() { return IsWindowed ? Windowed_PresentInterval : Fullscreen_PresentInterval; }
	void	SetPresentInterval(UINT value) { if (IsWindowed) Windowed_PresentInterval = value; else Fullscreen_PresentInterval = value; }
};

#endif



