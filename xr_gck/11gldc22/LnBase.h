// Interface for the D3Ds class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcBase_H_
#define _LcBase_H_

#ifndef interface
#define interface struct
#endif


#ifndef LC_CLASS_DESTROYER
#define LC_CLASS_DESTROYER( CLASS_NAME )	\
virtual ~CLASS_NAME(){}
#endif





///////////////////////////////////////////////////////////////////////////////
// Image

interface ILcImage
{
	enum	
	{
		MAX_FILE_LEN	= 160,

		LCIMG_FMT_TGA	= 0X01,
		LCIMG_FMT_BMP	= 0X02,
		LCIMG_FMT_PNG	= 0X04,
		LCIMG_FMT_JPG	= 0X08,

		LCIMG_FMT_RGB	= 0X0101,
		LCIMG_FMT_RGBA	= 0X0102,
	};

	LC_CLASS_DESTROYER(	ILcImage	);
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual INT		Query(char* sCmd, void* pData)=0;

	virtual INT		GetWidth()=0;				// Get Width
	virtual INT		GetHeight()=0;				// Get Height
	virtual INT		GetDepth()=0;				// Get Depth
	virtual INT		GetBitCount()=0;			// Get Pixel Bit Count
	virtual INT		GetFormat()=0;				// rgb, rgba, bgr
	virtual	BYTE*	GetPointer()=0;				// Get Pixel Pointer
	virtual const char*	GetName() const=0;		// Get File Name
};


INT LcDev_CreateImage(char* sCmd
					, ILcImage** pData
					, void* p1		// File Name
					, void* p2 =0	// Not Use
					, void* p3 =0	// Not Use
					, void* p4 =0	// Not Use
					);




///////////////////////////////////////////////////////////////////////////////
// Texture

interface ILC_Texture
{
	enum	{	MAX_TEXTURE_LAYER	=	8,	};

	struct TxLayer
	{
		union
		{
			struct
			{
				void*	p0;	void*	p1;	void*	p2;	void*	p3;
				void*	p4;	void*	p5;	void*	p6;	void*	p7;
			};

			void* p[MAX_TEXTURE_LAYER];
		};

		TxLayer();
	};


	LC_CLASS_DESTROYER(	ILC_Texture	);
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual INT		Query(char* sCmd, void* pData)=0;

	virtual DWORD	GetWidth()=0;				// Get Width
	virtual DWORD	GetHeight()=0;				// Get Height
	virtual DWORD	GetDepth()=0;				// Get Depth
	virtual DWORD	GetMipLevel()=0;			// Get Creation MipLevel
	virtual DWORD	GetFormat()=0;				// Get enumeration from D3DFORMAT
	virtual DWORD	GetResourceType()=0;		// Get enumeration from D3DRESOURCETYPE
	virtual DWORD	GetFileFormat()=0;			// Get enumeration from D3DMXIMAGE_FILEFORMAT

	virtual char*	GetSourceName()=0;			// Get Source Name or File Name
	virtual void*	GetPointer()=0;				// Get D3DTexture Pointer
	
	virtual DWORD	GetColorKey()=0;			// Get Creation color Key
	virtual DWORD	GetFilter()=0;				// Get Creation Filter

	virtual void	GetRECT(RECT* rc)=0;		// Get RECT
};



typedef ILC_Texture*		PLC_TEXTURE;

// LcDev_CreateTex 함수 사용 예제
// LcDev_CreateTex("File"			, &pData, "Test.jpg");
// LcDev_CreateTex("ResourceBmp"	, &pData, &ResourceId);
// LcDev_CreateTex("ResourceCustom"	, &pData, &ResourceId, "UserCustomDefineType");
// LcDev_CreateTex("ResourceCustom"	, &pData, &ResourceId, "PNG");
// LcDev_CreateTex("Memory"			, &pData, pSrcData, &dSourceSize);

INT LcDev_CreateTex(char* sCmd
					, PLC_TEXTURE* pData
					, void* pVal1
					, DWORD dColorKey=0x00FFFFFF
					, DWORD dFilter= (2 << 0)					// D3DX_FILTER_POINT
					, DWORD Pool= 1								// D3DPOOL_MANAGED
					);




interface ILcTexMng
{
	LC_CLASS_DESTROYER(	ILcTexMng	);
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual INT		Query(char* sCmd, void* pData)=0;

	// Management Interface
	virtual INT		Find(char* sName, PLC_TEXTURE* pData)=0;
	virtual INT		FindCreate(char* sCmd
								, PLC_TEXTURE* pData
								, void* pVal1
								, DWORD dColorKey=0x00FFFFFF
								, DWORD dFilter= (2 << 0)		// D3DX_FILTER_POINT
								, DWORD Pool= 1					// D3DPOOL_MANAGED
								)=0;
};


INT LcDev_CreateTexMng(char* sCmd
					, ILcTexMng** pData	// Texture Manager Instance
					, void* p1			// Device
					, void* p2=0		// No Use
					, void* p3=0		// No Use
					, void* p4=0		// No Use
					);




///////////////////////////////////////////////////////////////////////////////
// Texture

// System and Device Interface
interface ILC_Device
{
	struct LcDev
	{
		INT			PosX;		// Position X
		INT			PosY;		// Position Y
		INT			ScnX;		// Client Rect Width
		INT			ScnY;		// Client Rect Height

		HINSTANCE	hInst;
		HWND		hWnd;		// Window Handle. In, out value

		char		sCls[256];	// Class Name

		LcDev();
		LcDev(INT _PosX, INT _PosY, INT _Wx, INT _Wy, const char* _sCls, HINSTANCE _hInst=NULL, HWND _hWnd= NULL);
	};

	LC_CLASS_DESTROYER(	ILC_Device	);

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;

	virtual INT		Init()=0;
	virtual void	Destroy()=0;

	virtual INT		Restore()=0;
	virtual void	Invalidate()=0;

	virtual INT		FrameMove()=0;
	virtual INT		Render()=0;

	virtual INT		Query(char* sCmd, void* pData)=0;
	virtual INT		Run()=0;

	virtual INT		BeginScene()=0;
	virtual INT		EndScene()=0;
	virtual INT		Clear(DWORD Count,CONST RECT* pRects,DWORD Flags,DWORD Color,float Z,DWORD Stencil)=0;


	////////////////////////////////////////////////////////////////////////////
	// System Data Interface.
	virtual void*	GetMainObject()=0;						// Main Program Object Pointer
	virtual void*	GetHinst	()=0;						// HINSTANCE: Window Instance
	virtual void*	GetHwnd		()=0;						// HWND: Main Window Handle
	virtual void*	GetDevice	()=0;						// LPDIRECT3DDEVICE9: Direct3D Device
	virtual void*	GetBufferSurface()=0;					// LPDIRECT3DSURFACE9: BackBuffer Surface

	virtual void*	GetSprite	()=0;						// User Define: 2D Sprite Pointer
	virtual void*	GetInput	()=0;						// User Define: Input Pointer
	virtual void*	GetCamera	()=0;						// User Define: Camera Pointer
	virtual void*	GetNetwork	()=0;						// User Define: Network Pointer
	virtual void*	GetIme		()=0;						// User Define: IME Pointer
	virtual void*	GetCommand	()=0;						// User Define: Command Pointer

	virtual void*	GetUserData1()=0;						// User Define: User Data Pointer 1
	virtual void*	GetUserData2()=0;						// User Define: User Data Pointer 2
	virtual void*	GetUserData3()=0;						// User Define: User Data Pointer 3
	virtual void*	GetUserData4()=0;						// User Define: User Data Pointer 4

	virtual INT		SetSprite	(void*)=0;
	virtual INT		SetInput	(void*)=0;
	virtual INT		SetCamera	(void*)=0;
	virtual INT		SetNetwork	(void*)=0;
	virtual INT		SetIme		(void*)=0;
	virtual INT		SetCommand	(void*)=0;

	virtual INT		SetUserData1(void*)=0;
	virtual INT		SetUserData2(void*)=0;
	virtual INT		SetUserData3(void*)=0;
	virtual INT		SetUserData4(void*)=0;
};


INT		LcDev_CreateWindowInstance(char* sCmd
						, ILC_Device** pData
						, void* p1=0		// No Use
						, void* p2=0		// No Use
						, void* p3=0		// No Use
						, void* p4=0		// No Use
						);


INT		LcDev_CreateDevice(char* sCmd
						, ILC_Device** pData
						, void* p1=0		// LcDev pointer
						, void* p2=0		// No Use
						, void* p3=0		// No Use
						, void* p4=0		// No Use
						);



void*	LcDev_GetD3Device();
void	LcDev_SetD3Device(void* pD3Dev);



//Query 대신 시스템 데이터를 함수로 편리하게 사용하기 위해서...
void*	LcDev_GetHinst			(ILC_Device* pDev);
void*	LcDev_GetHwnd			(ILC_Device* pDev);
void*	LcDev_GetBackBufferSurface(ILC_Device* pDev);

void*	LcDev_GetUserSprite		(ILC_Device* pDev);
void*	LcDev_GetUserInput		(ILC_Device* pDev);
void*	LcDev_GetUserCamera		(ILC_Device* pDev);
void*	LcDev_GetUserNetwork	(ILC_Device* pDev);
void*	LcDev_GetUserIme		(ILC_Device* pDev);
void*	LcDev_GetUserCommand	(ILC_Device* pDev);

void*	LcDev_GetUserData1		(ILC_Device* pDev);
void*	LcDev_GetUserData2		(ILC_Device* pDev);
void*	LcDev_GetUserData3		(ILC_Device* pDev);
void*	LcDev_GetUserData4		(ILC_Device* pDev);

INT		LcDev_SetUserSprite		(ILC_Device* pDev, void* p);
INT		LcDev_SetUserInput		(ILC_Device* pDev, void* p);
INT		LcDev_SetUserCamera		(ILC_Device* pDev, void* p);
INT		LcDev_SetUserNetwork	(ILC_Device* pDev, void* p);
INT		LcDev_SetUserIme		(ILC_Device* pDev, void* p);
INT		LcDev_SetUserCommand	(ILC_Device* pDev, void* p);

INT		LcDev_SetUserData1		(ILC_Device* pDev, void* p);
INT		LcDev_SetUserData2		(ILC_Device* pDev, void* p);
INT		LcDev_SetUserData3		(ILC_Device* pDev, void* p);
INT		LcDev_SetUserData4		(ILC_Device* pDev, void* p);




interface ILC_Sprite
{
	LC_CLASS_DESTROYER(	ILC_Sprite	);
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;

	virtual	INT		Begin(DWORD=0)	=0;
	virtual	INT		End(DWORD=0)	=0;

	virtual	INT		OnResetDevice()	=0;
	virtual	INT		OnLostDevice()	=0;

	virtual	INT		Draw( void* pTx0				// 0 Stage Texture(<==ILcTex)
						, const RECT* pRC0			// Draw Region 0 State Texture
						, void* pPos=NULL			// Position
						, DWORD dColor =0xFFFFFFFF	// color
						, DWORD dFilter=0x0			// Filter(<==D3DMTEXF_POINT)
						)=0;
	

	virtual	INT		Draw( void* pTx0				// 0 Stage Texture(<==ILcTex)
						, void* pTx1				// 1 Stage Texture(<==ILcTex)
						, const RECT* pRC0			// Draw Region 0 State Texture
						, const RECT* pRC1			// Draw Region 1 State Texture
						, void* pScl=NULL			// Scaling
						, void* pPos=NULL			// Position
						, DWORD dColor =0xFFFFFFFF	// color
						, DWORD dFilter=0x0			// Filter(<==D3DMTEXF_POINT)
						)=0;

	virtual	INT		DrawEx( void* pTx0				// 0 Stage Texture(<==ILcTex)
						, void* pTx1				// 1 Stage Texture(<==ILcTex)
						, const RECT* pRC0			// Draw Region 0 State Texture
						, const RECT* pRC1			// Draw Region 1 State Texture
						, void* pScl				// Scaling
						, void* pPos				// Position
						, void* pRot				// Rotation Center
						, FLOAT	fAngle				// Rotation Angle(Radian)
						, DWORD dColor				// color
						, DWORD dFilter=0x0			// Filter(<==D3DMTEXF_POINT)
						)=0;
};


INT	LcDev_CreateSprite(char* sCmd
					, ILC_Sprite** pData
					, void* p1=0						// No Use
					, void* p2=0						// No Use
					, void* p3=0						// No Use
					, void* p4=0						// No Use
					);









///////////////////////////////////////////////////////////////////////////////
// Font

struct TLcFont
{
	INT		iHeight	;						// Height
	INT		iWeight	;						// Weight, Normal, Bold, thin...
	INT		iItalic	;						// Is Italic
	INT		iThckX	;						// Girth of thick X
	INT		iThckY	;						// Girth of thick Y
	DWORD	dString	;						// String color
	DWORD	dGirth	;						// Girth of Font color
	TCHAR	sName	[LF_FACESIZE];

	TLcFont();

	TLcFont(TCHAR*	_sName					// Font Name
			, INT	_iHeight=18				// Height
			, INT	_iWeight=FW_NORMAL		// Weight, Normal, Bold, thin...
			, INT	_iItalic=0				// Is Italic
			, INT	_iThckX=1				// Girth of thick X
			, INT	_iThckY=1				// Girth of thick Y
			, DWORD	_dString=0xFFFFFFFF		// String color
			, DWORD	_dGirth=0xFF000099		// Girth of Font color
		);
	

	const TLcFont& operator=(const TLcFont& r);	// r: right hand side(rhs)
	const TLcFont& operator=(const TLcFont* r);
};



interface ILC_Font
{
	LC_CLASS_DESTROYER(	ILC_Font	);

	typedef enum tagEFntAlgn				// Font Align
	{
		FNT_ALIGN_H_L	= 0x00000000,		// Horizontal Left align
		FNT_ALIGN_H_C	= 0x00000001,		// Horizontal center
		FNT_ALIGN_H_R	= 0x00000002,		// Horizontal Right align

		FNT_ALIGN_V_T	= 0x00000000,		// vertical Top
		FNT_ALIGN_V_C	= 0x00000004,		// vertical center
		FNT_ALIGN_V_B	= 0x00000008,		// vertical bottom
	}EFntAlgn;


	enum ELcFont
	{
		LPFNT_DX=0,
		LPFNT_CE=1,
	};
	
	virtual INT		Create(	  void* p1=0	// LPDIRECT3DDEVICE9
							, void* p2=0	// ID3DXSprite
							, void* p3=0	// TLcFont
							, void* p4=0	// No Use
							)=0;

	virtual void	Destroy()=0;
	virtual void	DrawTxt()=0;

	virtual INT		Query(char* sCmd, void* pData)=0;

	virtual	void	SetString(TCHAR* sStr=NULL)=0;
	virtual	void	SetPosition(FLOAT* pos=NULL/*FLOAT3 or Float2*/)=0;

	virtual	INT		SetColorDiffuse	(FLOAT* pColor=NULL /*rgba <== D3DXCOLOR*/)=0;
	virtual	INT		SetColorFore	(FLOAT* pColor=NULL /*rgba <== D3DXCOLOR*/)=0;
	virtual	INT		SetColorBack	(FLOAT* pColor=NULL /*rgba <== D3DXCOLOR*/)=0;

	virtual void	SetPosAlign(DWORD dwAlign=0)=0;

	virtual	INT		OnLostDevice()=0;
	virtual	INT		OnReset()=0;

	virtual	INT		GetType() = 0;
};


INT LcDev_CreateFont(char* sCmd
				 , ILC_Font** pData
				 , void* p1			// TLcFont
				 , void* p2=NULL	// No Use
				 , void* p3=NULL	// No Use
				 , void* p4=NULL	// No Use
				 );







////////////////////////////////////////////////////////////////////////////////
// Input

interface ILcInput
{
	LC_CLASS_DESTROYER(	ILcInput	);
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual INT		FrameMove()=0;
	virtual INT		Query(char* sCmd, void* pData)=0;
	virtual INT		MsgProc(HWND,UINT,WPARAM,LPARAM)=0;

	virtual BYTE*	GetKeyMap()	const=0;

	virtual BOOL	KeyDown(INT nKey)=0;
	virtual BOOL	KeyUp(INT nKey)=0;
	virtual BOOL	KeyPress(INT nKey)=0;
	virtual BOOL	KeyState(int nKey)=0;

	virtual BYTE*	GetButtonMap()	const=0;

	virtual BOOL	ButtonDown(INT nBtn)=0;
	virtual BOOL	ButtonUp(INT nBtn)=0;
	virtual BOOL	ButtonPress(INT nBtn)=0;
	virtual BOOL	ButtonState(int nBtn)=0;

	virtual FLOAT*	GetMousePos()=0;
	virtual FLOAT*	GetMouseDelta()=0;
	virtual BOOL	GetMouseMove()=0;

	virtual BOOL	IsInRect(INT left, INT top, INT right, INT bottom)=0;	
};


INT LcInput_Create(char* sCmd
				 , ILcInput** pData
				 , void* p1			// HIINSTANCE
				 , void* p2			// HWND
				 , void* p3			// LPDIRECT3DDEVICE9
				 , void* p4=0		// No Use
				 );


////////////////////////////////////////////////////////////////////////////////
// IME

#define MAX_CHAT_LEN	240														// 최대 채팅 길이

interface ILcIme
{
	LC_CLASS_DESTROYER(	ILcIme	);

	enum EImeState
	{
		IHS_ENG = 0,
		IHS_KOR,			// Korean
		IHS_JPN,			// Japanese.	It's not Implemental
		IHS_CHR,			// Chinese.		It's not Implemental
	};

	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual INT		MsgProc(HWND,UINT,WPARAM,LPARAM)=0;

	virtual INT		Query(char* sCmd, void* pData)=0;

	virtual	void	Set()=0;
	virtual	void	Reset()=0;

	virtual void	SetState(BOOL)=0;
	virtual INT		GetState()=0;

	virtual	INT		OutString(char*)=0;
	virtual	INT		OutStringBeam(char*)=0;
	virtual	INT		OutStringStar(char*)=0;
};


INT LcIme_Create(char* sCmd
				 , ILcIme** pData
				 , void* p1=0		// String Table Back space Id
				 , void* p2=0		// No Use
				 , void* p3=0		// No Use
				 , void* p4=0		// No Use
				 );




///////////////////////////////////////////////////////////////////////////////
// Application



// Application or Game Phase(Stage) Interface
interface ILcApp
{
	LC_CLASS_DESTROYER(	ILcApp	);

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;	

	virtual INT		Init()=0;
	virtual INT		Destroy()=0;

	virtual INT		Restore()=0;
	virtual INT		Invalidate()=0;

	virtual INT		FrameMove()=0;
	virtual	INT		Render()=0;

	virtual INT		Query(char* sCmd, void* pData)=0;
};




///////////////////////////////////////////////////////////////////////////////
// Input constants

// Not defined Virtual Keys. So U'll make it.

// - on main keyboard
#define VK_MINUS		0xBD
#define VK_EQUALS		0xBB
#define VK_BACKSLASH	0xDC

#define VK_LBRACKET		0xDB
#define VK_RBRACKET		0xDD

#define VK_SEMICOLON	0xBA
#define VK_APOSTROPHE	0xDE

#define VK_COMMA		0xBC

// . on main keyboard
#define VK_PERIOD		0xBE
#define VK_SLASH		0xBF

#define VK_0			0x30
#define VK_1			0x31
#define VK_2			0x32
#define VK_3			0x33
#define VK_4			0x34
#define VK_5			0x35
#define VK_6			0x36
#define VK_7			0x37
#define VK_8			0x38
#define VK_9			0x39

#define VK_A			0x41
#define VK_B			0x42
#define VK_C			0x43
#define VK_D			0x44
#define VK_E			0x45
#define VK_F			0x46
#define VK_G			0x47

#define VK_H			0x48
#define VK_I			0x49
#define VK_J			0x4A
#define VK_K			0x4B
#define VK_L			0x4C
#define VK_M			0x4D
#define VK_N			0x4E

#define VK_O			0x4F
#define VK_P			0x50
#define VK_Q			0x51
#define VK_R			0x52
#define VK_S			0x53
#define VK_T			0x54
#define VK_U			0x55
#define VK_V			0x56
#define VK_W			0x57
#define VK_X			0x58
#define VK_Y			0x59
#define VK_Z			0x5A




enum EDirectInput
{
	DDIK_ESCAPE          = 0x01,
	DDIK_1               = 0x02,
	DDIK_2               = 0x03,
	DDIK_3               = 0x04,
	DDIK_4               = 0x05,
	DDIK_5               = 0x06,
	DDIK_6               = 0x07,
	DDIK_7               = 0x08,
	DDIK_8               = 0x09,
	DDIK_9               = 0x0A,
	DDIK_0               = 0x0B,
	DDIK_MINUS           = 0x0C,   /* - on main keyboard */
	DDIK_EQUALS          = 0x0D,
	DDIK_BACK            = 0x0E,   /* backspace */
	DDIK_TAB             = 0x0F,
	DDIK_Q               = 0x10,
	DDIK_W               = 0x11,
	DDIK_E               = 0x12,
	DDIK_R               = 0x13,
	DDIK_T               = 0x14,
	DDIK_Y               = 0x15,
	DDIK_U               = 0x16,
	DDIK_I               = 0x17,
	DDIK_O               = 0x18,
	DDIK_P               = 0x19,
	DDIK_LBRACKET        = 0x1A,
	DDIK_RBRACKET        = 0x1B,
	DDIK_RETURN          = 0x1C,    /* Enter on main keyboard */
	DDIK_LCONTROL        = 0x1D,
	DDIK_A               = 0x1E,
	DDIK_S               = 0x1F,
	DDIK_D               = 0x20,
	DDIK_F               = 0x21,
	DDIK_G               = 0x22,
	DDIK_H               = 0x23,
	DDIK_J               = 0x24,
	DDIK_K               = 0x25,
	DDIK_L               = 0x26,
	DDIK_SEMICOLON       = 0x27,
	DDIK_APOSTROPHE      = 0x28,
	DDIK_GRAVE           = 0x29,    /* accent grave */
	DDIK_LSHIFT          = 0x2A,
	DDIK_BACKSLASH       = 0x2B,
	DDIK_Z               = 0x2C,
	DDIK_X               = 0x2D,
	DDIK_C               = 0x2E,
	DDIK_V               = 0x2F,
	DDIK_B               = 0x30,
	DDIK_N               = 0x31,
	DDIK_M               = 0x32,
	DDIK_COMMA           = 0x33,
	DDIK_PERIOD          = 0x34,    /* . on main keyboard */
	DDIK_SLASH           = 0x35,    /* / on main keyboard */
	DDIK_RSHIFT          = 0x36,
	DDIK_MULTIPLY        = 0x37,    /* * on numeric keypad */
	DDIK_LMENU           = 0x38,    /* left Alt */
	DDIK_SPACE           = 0x39,
	DDIK_CAPITAL         = 0x3A,
	DDIK_F1              = 0x3B,
	DDIK_F2              = 0x3C,
	DDIK_F3              = 0x3D,
	DDIK_F4              = 0x3E,
	DDIK_F5              = 0x3F,
	DDIK_F6              = 0x40,
	DDIK_F7              = 0x41,
	DDIK_F8              = 0x42,
	DDIK_F9              = 0x43,
	DDIK_F10             = 0x44,
	DDIK_NUMLOCK         = 0x45,
	DDIK_SCROLL          = 0x46,    /* Scroll Lock */
	DDIK_NUMPAD7         = 0x47,
	DDIK_NUMPAD8         = 0x48,
	DDIK_NUMPAD9         = 0x49,
	DDIK_SUBTRACT        = 0x4A,    /* - on numeric keypad */
	DDIK_NUMPAD4         = 0x4B,
	DDIK_NUMPAD5         = 0x4C,
	DDIK_NUMPAD6         = 0x4D,
	DDIK_ADD             = 0x4E,    /* + on numeric keypad */
	DDIK_NUMPAD1         = 0x4F,
	DDIK_NUMPAD2         = 0x50,
	DDIK_NUMPAD3         = 0x51,
	DDIK_NUMPAD0         = 0x52,
	DDIK_DECIMAL         = 0x53,    /* . on numeric keypad */
	DDIK_OEM_102         = 0x56,    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
	DDIK_F11             = 0x57,
	DDIK_F12             = 0x58,
	DDIK_F13             = 0x64,    /*                     (NEC PC98) */
	DDIK_F14             = 0x65,    /*                     (NEC PC98) */
	DDIK_F15             = 0x66,    /*                     (NEC PC98) */
	DDIK_KANA            = 0x70,    /* (Japanese keyboard)            */
	DDIK_ABNT_C1         = 0x73,    /* /? on Brazilian keyboard */
	DDIK_CONVERT         = 0x79,    /* (Japanese keyboard)            */
	DDIK_NOCONVERT       = 0x7B,    /* (Japanese keyboard)            */
	DDIK_YEN             = 0x7D,    /* (Japanese keyboard)            */
	DDIK_ABNT_C2         = 0x7E,    /* Numpad . on Brazilian keyboard */
	DDIK_NUMPADEQUALS    = 0x8D,    /* = on numeric keypad (NEC PC98) */
	DDIK_PREVTRACK       = 0x90,    /* Previous Track (DDIK_CIRCUMFLEX on Japanese keyboard) */
	DDIK_AT              = 0x91,    /*                     (NEC PC98) */
	DDIK_COLON           = 0x92,    /*                     (NEC PC98) */
	DDIK_UNDERLINE       = 0x93,    /*                     (NEC PC98) */
	DDIK_KANJI           = 0x94,    /* (Japanese keyboard)            */
	DDIK_STOP            = 0x95,    /*                     (NEC PC98) */
	DDIK_AX              = 0x96,    /*                     (Japan AX) */
	DDIK_UNLABELED       = 0x97,    /*                        (J3100) */
	DDIK_NEXTTRACK       = 0x99,    /* Next Track */
	DDIK_NUMPADENTER     = 0x9C,    /* Enter on numeric keypad */
	DDIK_RCONTROL        = 0x9D,
	DDIK_MUTE            = 0xA0,    /* Mute */
	DDIK_CALCULATOR      = 0xA1,    /* Calculator */
	DDIK_PLAYPAUSE       = 0xA2,    /* Play / Pause */
	DDIK_MEDIASTOP       = 0xA4,    /* Media Stop */
	DDIK_VOLUMEDOWN      = 0xAE,    /* Volume - */
	DDIK_VOLUMEUP        = 0xB0,    /* Volume + */
	DDIK_WEBHOME         = 0xB2,    /* Web home */
	DDIK_NUMPADCOMMA     = 0xB3,    /* , on numeric keypad (NEC PC98) */
	DDIK_DIVIDE          = 0xB5,    /* / on numeric keypad */
	DDIK_SYSRQ           = 0xB7,
	DDIK_RMENU           = 0xB8,    /* right Alt */
	DDIK_PAUSE           = 0xC5,    /* Pause */
	DDIK_HOME            = 0xC7,    /* Home on arrow keypad */
	DDIK_UP              = 0xC8,    /* UpArrow on arrow keypad */
	DDIK_PRIOR           = 0xC9,    /* PgUp on arrow keypad */
	DDIK_LEFT            = 0xCB,    /* LeftArrow on arrow keypad */
	DDIK_RIGHT           = 0xCD,    /* RightArrow on arrow keypad */
	DDIK_END             = 0xCF,    /* End on arrow keypad */
	DDIK_DOWN            = 0xD0,    /* DownArrow on arrow keypad */
	DDIK_NEXT            = 0xD1,    /* PgDn on arrow keypad */
	DDIK_INSERT          = 0xD2,    /* Insert on arrow keypad */
	DDIK_DELETE          = 0xD3,    /* Delete on arrow keypad */
	DDIK_LWIN            = 0xDB,    /* Left Windows key */
	DDIK_RWIN            = 0xDC,    /* Right Windows key */
	DDIK_APPS            = 0xDD,    /* AppMenu key */
	DDIK_POWER           = 0xDE,    /* System Power */
	DDIK_SLEEP           = 0xDF,    /* System Sleep */
	DDIK_WAKE            = 0xE3,    /* System Wake */
	DDIK_WEBSEARCH       = 0xE5,    /* Web Search */
	DDIK_WEBFAVORITES    = 0xE6,    /* Web Favorites */
	DDIK_WEBREFRESH      = 0xE7,    /* Web Refresh */
	DDIK_WEBSTOP         = 0xE8,    /* Web Stop */
	DDIK_WEBFORWARD      = 0xE9,    /* Web Forward */
	DDIK_WEBBACK         = 0xEA,    /* Web Back */
	DDIK_MYCOMPUTER      = 0xEB,    /* My Computer */
	DDIK_MAIL            = 0xEC,    /* Mail */
	DDIK_MEDIASELECT     = 0xED,    /* Media Select */
	
	DDIK_BACKSPACE       = DDIK_BACK    ,    /* backspace */
	DDIK_NUMPADSTAR      = DDIK_MULTIPLY,    /* * on numeric keypad */
	DDIK_LALT            = DDIK_LMENU   ,    /* left Alt */
	DDIK_CAPSLOCK        = DDIK_CAPITAL ,    /* CapsLock */
	DDIK_NUMPADMINUS     = DDIK_SUBTRACT,    /* - on numeric keypad */
	DDIK_NUMPADPLUS      = DDIK_ADD     ,    /* + on numeric keypad */
	DDIK_NUMPADPERIOD    = DDIK_DECIMAL ,    /* . on numeric keypad */
	DDIK_NUMPADSLASH     = DDIK_DIVIDE  ,    /* / on numeric keypad */
	DDIK_RALT            = DDIK_RMENU   ,    /* right Alt */
	DDIK_UPARROW         = DDIK_UP      ,    /* UpArrow on arrow keypad */
	DDIK_PGUP            = DDIK_PRIOR   ,    /* PgUp on arrow keypad */
	DDIK_LEFTARROW       = DDIK_LEFT    ,    /* LeftArrow on arrow keypad */
	DDIK_RIGHTARROW      = DDIK_RIGHT   ,    /* RightArrow on arrow keypad */
	DDIK_DOWNARROW       = DDIK_DOWN    ,    /* DownArrow on arrow keypad */
	DDIK_PGDN            = DDIK_NEXT    ,    /* PgDn on arrow keypad */
};






#ifdef USE_DIRECT_INPUT8

#define LCK_ESCAPE			DDIK_ESCAPE
#define LCK_1				DDIK_1
#define LCK_2				DDIK_2
#define LCK_3				DDIK_3
#define LCK_4				DDIK_4
#define LCK_5				DDIK_5
#define LCK_6				DDIK_6
#define LCK_7				DDIK_7
#define LCK_8				DDIK_8
#define LCK_9				DDIK_9
#define LCK_0				DDIK_0
#define LCK_MINUS			DDIK_MINUS
#define LCK_EQUALS			DDIK_EQUALS
#define LCK_BACK			DDIK_BACK
#define LCK_TAB				DDIK_TAB
#define LCK_Q				DDIK_Q
#define LCK_W				DDIK_W
#define LCK_E				DDIK_E
#define LCK_R				DDIK_R
#define LCK_T				DDIK_T
#define LCK_Y				DDIK_Y
#define LCK_U				DDIK_U
#define LCK_I				DDIK_I
#define LCK_O				DDIK_O
#define LCK_P				DDIK_P
#define LCK_LBRACKET		DDIK_LBRACKET
#define LCK_RBRACKET		DDIK_RBRACKET
#define LCK_RETURN			DDIK_RETURN
#define LCK_LCONTROL		DDIK_LCONTROL
#define LCK_A				DDIK_A
#define LCK_S				DDIK_S
#define LCK_D				DDIK_D
#define LCK_F				DDIK_F
#define LCK_G				DDIK_G
#define LCK_H				DDIK_H
#define LCK_J				DDIK_J
#define LCK_K				DDIK_K
#define LCK_L				DDIK_L
#define LCK_SEMICOLON		DDIK_SEMICOLON
#define LCK_APOSTROPHE		DDIK_APOSTROPHE
#define LCK_GRAVE			DDIK_GRAVE
#define LCK_LSHIFT			DDIK_LSHIFT
#define LCK_BACKSLASH		DDIK_BACKSLASH
#define LCK_Z				DDIK_Z
#define LCK_X				DDIK_X
#define LCK_C				DDIK_C
#define LCK_V				DDIK_V
#define LCK_B				DDIK_B
#define LCK_N				DDIK_N
#define LCK_M				DDIK_M
#define LCK_COMMA			DDIK_COMMA
#define LCK_PERIOD			DDIK_PERIOD
#define LCK_SLASH			DDIK_SLASH
#define LCK_RSHIFT			DDIK_RSHIFT
#define LCK_MULTIPLY		DDIK_MULTIPLY
#define LCK_LMENU			DDIK_LMENU
#define LCK_SPACE			DDIK_SPACE
#define LCK_CAPITAL			DDIK_CAPITAL
#define LCK_F1				DDIK_F1
#define LCK_F2				DDIK_F2
#define LCK_F3				DDIK_F3
#define LCK_F4				DDIK_F4
#define LCK_F5				DDIK_F5
#define LCK_F6				DDIK_F6
#define LCK_F7				DDIK_F7
#define LCK_F8				DDIK_F8
#define LCK_F9				DDIK_F9
#define LCK_F10				DDIK_F10
#define LCK_NUMLOCK			DDIK_NUMLOCK
#define LCK_SCROLL			DDIK_SCROLL
#define LCK_NUMPAD7			DDIK_NUMPAD7
#define LCK_NUMPAD8			DDIK_NUMPAD8
#define LCK_NUMPAD9			DDIK_NUMPAD9
#define LCK_SUBTRACT		DDIK_SUBTRACT
#define LCK_NUMPAD4			DDIK_NUMPAD4
#define LCK_NUMPAD5			DDIK_NUMPAD5
#define LCK_NUMPAD6			DDIK_NUMPAD6
#define LCK_ADD				DDIK_ADD
#define LCK_NUMPAD1			DDIK_NUMPAD1
#define LCK_NUMPAD2			DDIK_NUMPAD2
#define LCK_NUMPAD3			DDIK_NUMPAD3
#define LCK_NUMPAD0			DDIK_NUMPAD0
#define LCK_DECIMAL			DDIK_DECIMAL
#define LCK_OEM_102			DDIK_OEM_102
#define LCK_F11				DDIK_F11
#define LCK_F12				DDIK_F12
#define LCK_F13				DDIK_F13
#define LCK_F14				DDIK_F14
#define LCK_F15				DDIK_F15
#define LCK_KANA			DDIK_KANA
#define LCK_ABNT_C1			DDIK_ABNT_C1
#define LCK_CONVERT			DDIK_CONVERT
#define LCK_NOCONVERT		DDIK_NOCONVERT
#define LCK_YEN				DDIK_YEN
#define LCK_ABNT_C2			DDIK_ABNT_C2
#define LCK_NUMPADEQUALS    DDIK_NUMPADEQUALS
#define LCK_PREVTRACK		DDIK_PREVTRACK
#define LCK_AT				DDIK_AT
#define LCK_COLON			DDIK_COLON
#define LCK_UNDERLINE		DDIK_UNDERLINE
#define LCK_KANJI			DDIK_KANJI
#define LCK_STOP			DDIK_STOP
#define LCK_AX				DDIK_AX
#define LCK_UNLABELED		DDIK_UNLABELED
#define LCK_NEXTTRACK		DDIK_NEXTTRACK
#define LCK_NUMPADENTER		DDIK_NUMPADENTER
#define LCK_RCONTROL		DDIK_RCONTROL
#define LCK_MUTE			DDIK_MUTE
#define LCK_CALCULATOR		DDIK_CALCULATOR
#define LCK_PLAYPAUSE		DDIK_PLAYPAUSE
#define LCK_MEDIASTOP		DDIK_MEDIASTOP
#define LCK_VOLUMEDOWN		DDIK_VOLUMEDOWN
#define LCK_VOLUMEUP		DDIK_VOLUMEUP
#define LCK_WEBHOME			DDIK_WEBHOME
#define LCK_NUMPADCOMMA		DDIK_NUMPADCOMMA
#define LCK_DIVIDE			DDIK_DIVIDE
#define LCK_SYSRQ			DDIK_SYSRQ
#define LCK_RMENU			DDIK_RMENU
#define LCK_PAUSE			DDIK_PAUSE
#define LCK_HOME			DDIK_HOME
#define LCK_UP				DDIK_UP
#define LCK_PRIOR			DDIK_PRIOR
#define LCK_LEFT			DDIK_LEFT
#define LCK_RIGHT			DDIK_RIGHT
#define LCK_END				DDIK_END
#define LCK_DOWN			DDIK_DOWN
#define LCK_NEXT			DDIK_NEXT
#define LCK_INSERT			DDIK_INSERT
#define LCK_DELETE			DDIK_DELETE
#define LCK_LWIN			DDIK_LWIN
#define LCK_RWIN			DDIK_RWIN
#define LCK_APPS			DDIK_APPS
#define LCK_POWER			DDIK_POWER
#define LCK_SLEEP			DDIK_SLEEP
#define LCK_WAKE			DDIK_WAKE
#define LCK_WEBSEARCH		DDIK_WEBSEARCH
#define LCK_WEBFAVORITES	DDIK_WEBFAVORITES
#define LCK_WEBREFRESH		DDIK_WEBREFRESH
#define LCK_WEBSTOP			DDIK_WEBSTOP
#define LCK_WEBFORWARD		DDIK_WEBFORWARD
#define LCK_WEBBACK			DDIK_WEBBACK
#define LCK_MYCOMPUTER		DDIK_MYCOMPUTER
#define LCK_MAIL			DDIK_MAIL
#define LCK_MEDIASELECT		DDIK_MEDIASELECT

#define LCK_BACKSPACE		DDIK_BACKSPACE
#define LCK_NUMPADSTAR		DDIK_NUMPADSTAR
#define LCK_LALT			DDIK_LALT
#define LCK_CAPSLOCK		DDIK_CAPSLOCK
#define LCK_NUMPADMINUS		DDIK_NUMPADMINUS
#define LCK_NUMPADPLUS		DDIK_NUMPADPLUS
#define LCK_NUMPADPERIOD    DDIK_NUMPADPERIOD
#define LCK_NUMPADSLASH		DDIK_NUMPADSLASH
#define LCK_RALT			DDIK_RALT
#define LCK_UPARROW			DDIK_UPARROW
#define LCK_PGUP			DDIK_PGUP
#define LCK_LEFTARROW		DDIK_LEFTARROW
#define LCK_RIGHTARROW		DDIK_RIGHTARROW
#define LCK_DOWNARROW		DDIK_DOWNARROW
#define LCK_PGDN			DDIK_PGDN
#define LCK_CIRCUMFLEX		DDIK_CIRCUMFLEX

#else

#define LCK_LBUTTON			VK_LBUTTON
#define LCK_RBUTTON			VK_RBUTTON
#define LCK_CANCEL			VK_CANCEL
#define LCK_MBUTTON			VK_MBUTTON

#define LCK_BACK			VK_BACK
#define LCK_TAB				VK_TAB

#define LCK_CLEAR			VK_CLEAR
#define LCK_RETURN			VK_RETURN

#define LCK_SHIFT			VK_SHIFT
#define LCK_CONTROL			VK_CONTROL
#define LCK_MENU			VK_MENU
#define LCK_PAUSE			VK_PAUSE
#define LCK_CAPITAL			VK_CAPITAL

#define LCK_KANA			VK_KANA
#define LCK_HANGEUL			VK_HANGEUL
#define LCK_HANGUL			VK_HANGUL
#define LCK_JUNJA			VK_JUNJA
#define LCK_FINAL			VK_FINAL
#define LCK_HANJA			VK_HANJA
#define LCK_KANJI			VK_KANJI

#define LCK_ESCAPE			VK_ESCAPE

#define LCK_CONVERT			VK_CONVERT
#define LCK_NONCONVERT		VK_NONCONVERT
#define LCK_ACCEPT			VK_ACCEPT
#define LCK_MODECHANGE		VK_MODECHANGE

#define LCK_SPACE			VK_SPACE
#define LCK_PRIOR			VK_PRIOR
#define LCK_NEXT			VK_NEXT
#define LCK_END				VK_END
#define LCK_HOME			VK_HOME
#define LCK_LEFT			VK_LEFT
#define LCK_UP				VK_UP
#define LCK_RIGHT			VK_RIGHT
#define LCK_DOWN			VK_DOWN
#define LCK_SELECT			VK_SELECT
#define LCK_PRINT			VK_PRINT
#define LCK_EXECUTE			VK_EXECUTE
#define LCK_SNAPSHOT		VK_SNAPSHOT
#define LCK_INSERT			VK_INSERT
#define LCK_DELETE			VK_DELETE
#define LCK_HELP			VK_HELP

#define LCK_LWIN			VK_LWIN
#define LCK_RWIN			VK_RWIN
#define LCK_APPS			VK_APPS

#define LCK_NUMPAD0			VK_NUMPAD0
#define LCK_NUMPAD1			VK_NUMPAD1
#define LCK_NUMPAD2			VK_NUMPAD2
#define LCK_NUMPAD3			VK_NUMPAD3
#define LCK_NUMPAD4			VK_NUMPAD4
#define LCK_NUMPAD5			VK_NUMPAD5
#define LCK_NUMPAD6			VK_NUMPAD6
#define LCK_NUMPAD7			VK_NUMPAD7
#define LCK_NUMPAD8			VK_NUMPAD8
#define LCK_NUMPAD9			VK_NUMPAD9
#define LCK_MULTIPLY		VK_MULTIPLY
#define LCK_ADD				VK_ADD
#define LCK_SEPARATOR		VK_SEPARATOR
#define LCK_SUBTRACT		VK_SUBTRACT
#define LCK_DECIMAL			VK_DECIMAL
#define LCK_DIVIDE			VK_DIVIDE
#define LCK_F1				VK_F1
#define LCK_F2				VK_F2
#define LCK_F3				VK_F3
#define LCK_F4				VK_F4
#define LCK_F5				VK_F5
#define LCK_F6				VK_F6
#define LCK_F7				VK_F7
#define LCK_F8				VK_F8
#define LCK_F9				VK_F9
#define LCK_F10				VK_F10
#define LCK_F11				VK_F11
#define LCK_F12				VK_F12
#define LCK_F13				VK_F13
#define LCK_F14				VK_F14
#define LCK_F15				VK_F15
#define LCK_F16				VK_F16
#define LCK_F17				VK_F17
#define LCK_F18				VK_F18
#define LCK_F19				VK_F19
#define LCK_F20				VK_F20
#define LCK_F21				VK_F21
#define LCK_F22				VK_F22
#define LCK_F23				VK_F23
#define LCK_F24				VK_F24

#define LCK_NUMLOCK			VK_NUMLOCK
#define LCK_SCROLL			VK_SCROLL

#define LCK_LSHIFT			VK_LSHIFT
#define LCK_RSHIFT			VK_RSHIFT
#define LCK_LCONTROL		VK_LCONTROL
#define LCK_RCONTROL		VK_RCONTROL
#define LCK_LMENU			VK_LMENU
#define LCK_RMENU			VK_RMENU
#define LCK_LALT			VK_LMENU
#define LCK_RALT			VK_RMENU

#define LCK_PROCESSKEY		VK_PROCESSKEY

#define LCK_ATTN			VK_ATTN
#define LCK_CRSEL			VK_CRSEL
#define LCK_EXSEL			VK_EXSEL
#define LCK_EREOF			VK_EREOF
#define LCK_PLAY			VK_PLAY
#define LCK_ZOOM			VK_ZOOM
#define LCK_NONAME			VK_NONAME
#define LCK_PA1				VK_PA1
#define LCK_OEM_CLEAR		VK_OEM_CLEAR



#define LCK_MINUS			VK_MINUS
#define LCK_EQUALS			VK_EQUALS
#define LCK_BACKSLASH		VK_BACKSLASH
#define LCK_LBRACKET		VK_LBRACKET
#define LCK_RBRACKET		VK_RBRACKET
#define LCK_SEMICOLON		VK_SEMICOLON
#define LCK_APOSTROPHE		VK_APOSTROPHE
#define LCK_COMMA			VK_COMMA
#define LCK_PERIOD			VK_PERIOD
#define LCK_SLASH			VK_SLASH
#define LCK_SYSRQ			VK_SNAPSHOT
#define LCK_GRAVE			VK_SELECT


#define LCK_0				VK_0
#define LCK_1				VK_1
#define LCK_2				VK_2
#define LCK_3				VK_3
#define LCK_4				VK_4
#define LCK_5				VK_5
#define LCK_6				VK_6
#define LCK_7				VK_7
#define LCK_8				VK_8
#define LCK_9				VK_9

#define LCK_A				VK_A
#define LCK_B				VK_B
#define LCK_C				VK_C
#define LCK_D				VK_D
#define LCK_E				VK_E
#define LCK_F				VK_F
#define LCK_G				VK_G
#define LCK_H				VK_H
#define LCK_I				VK_I
#define LCK_J				VK_J
#define LCK_K				VK_K
#define LCK_L				VK_L
#define LCK_M				VK_M
#define LCK_N				VK_N
#define LCK_O				VK_O
#define LCK_P				VK_P
#define LCK_Q				VK_Q
#define LCK_R				VK_R
#define LCK_S				VK_S
#define LCK_T				VK_T
#define LCK_U				VK_U
#define LCK_V				VK_V
#define LCK_W				VK_W
#define LCK_X				VK_X
#define LCK_Y				VK_Y
#define LCK_Z				VK_Z

#endif


#define LCK_BUTTONL			0
#define LCK_BUTTONR			1
#define LCK_BUTTONM			2

#define LCK_KEYNONE			0
#define LCK_KEYDOWN			1
#define LCK_KEYUP			2
#define LCK_KEYPRESS		3





#endif

