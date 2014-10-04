// Interface for the LcUtilDx functions and classes.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcUtilDx_H_
#define _LcUtilDx_H_

#pragma warning(disable : 4786)
#include <vector>
using namespace std;



extern DWORD LcD3D_DXVersion();
extern HRESULT LcUtil_GetDXVersion(DWORD* pDXVer, TCHAR* SDXVer, int chDVer);



typedef D3DXVECTOR2						VEC2;
typedef	D3DXVECTOR3						VEC3;
typedef D3DXVECTOR4						VEC4;
typedef D3DXMATRIX						MATA;
typedef D3DXQUATERNION					QUAT;
typedef D3DXCOLOR						DCLR;

typedef LPDIRECT3DDEVICE9				PDEV;
typedef	LPD3DXSPRITE					PDSP;

typedef D3DVIEWPORT9					DVWP;

typedef LPDIRECT3DTEXTURE9				PDTX;
typedef LPDIRECT3DVERTEXBUFFER9			PDVB;
typedef LPDIRECT3DINDEXBUFFER9			PDIB;

typedef LPDIRECT3DSURFACE9				PDSF;
typedef LPD3DXRENDERTOSURFACE			PDRS;
typedef	LPDIRECT3DSWAPCHAIN9			PDSW;									//	Swap chain

typedef LPDIRECT3DVERTEXSHADER9			PDVS;
typedef LPDIRECT3DPIXELSHADER9			PDPS;
typedef LPDIRECT3DVERTEXDECLARATION9	PDVD;
typedef LPD3DXCONSTANTTABLE				PDCT;

//	D3DCREATE_PUREDEVICE
//	Specifies that Direct3D does not support Get* calls for anything that can be stored in state blocks.
//	It also tells Direct3D not to provide any emulation services for vertex processing.
//	This means that if the device does not support vertex processing,
//	then the application can use only post-transformed vertices. 


void	LcD3D_SetWorldIdentity(PDEV pDev, DWORD val=D3DTS_WORLD);
INT		LcD3D_DrawTextBackbuffer(PDEV pDev, INT X, INT Y, LPCTSTR Text, DWORD _color= RGB(255,255,0));


INT	LcD3D_TexLoadFile(PDTX* pTexture, PDEV pDev, char* sFile
						, DWORD dColor		=0x00FFFFFF
						, D3DXIMAGE_INFO* pImg=NULL
						, DWORD Filter		= (D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR)
						, DWORD MipFilter	= (D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR)
						, D3DFORMAT d3Fmt	= D3DFMT_UNKNOWN
						, DWORD Pool		= D3DPOOL_MANAGED);


INT	LcD3D_TexLoadRscBmp(PDTX* pTexture, PDEV pDev, INT nResourceId	//Resource의 비트맵만 가능
						, DWORD dColor=0x00FFFFFF
						, D3DXIMAGE_INFO* pImg=NULL
						, DWORD Filter		= (D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR)
						, DWORD MipFilter	= (D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR)
						, D3DFORMAT d3Fmt	= D3DFMT_UNKNOWN
						, DWORD Pool		= D3DPOOL_MANAGED);



// 비트맵 이외 PNG, JPG, TGA 용
INT		LcD3D_TextureLoadRscCustom(
							PDTX* pTexture, PDEV pDev, INT nResourceId, const char* sType
							, DWORD dColor=0x00FFFFFF
							, D3DXIMAGE_INFO* pSrcInf=NULL
							, DWORD Filter		= (D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR)
							, DWORD MipFilter	= (D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR)
							, D3DFORMAT d3Fmt	= D3DFMT_UNKNOWN
							, DWORD Pool		= D3DPOOL_MANAGED);



INT		LcD3D_TextureLoadMemory(PDTX* pTexture, PDEV pDev, void* pData, UINT dSize
							, DWORD dColor=0x00FFFFFF
							, D3DXIMAGE_INFO* pSrcInf=NULL
							, DWORD Filter		= (D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR)
							, DWORD MipFilter	= (D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR)
							, D3DFORMAT d3Fmt	= D3DFMT_UNKNOWN
							, DWORD Pool		= D3DPOOL_MANAGED);


INT		LcD3D_TextureFill(PDTX pTx, PDEV pDev, DWORD dColor= 0x0);



INT		LcD3D_FontCreate(PDEV pDev, LPD3DXFONT &pDxFnt
						  , char*	sFntName	= "Arial"
						  , HFONT*	pHfont		= NULL
						  , INT		lfHeight	= 12
						  , INT		lfWeight	= FW_NORMAL
						  , DWORD	lfItalic	= FALSE
						  , DWORD	lfCharSet	= HANGEUL_CHARSET  );




HWND	LcD3D_FocusHwndFromDevice(PDEV pDev);
PDEV	LcD3D_DeviceFromDxSprite(PDSP pSp);


INT		LcD3D_DeviceFormat(PDEV pDev, DWORD* FMTColor, DWORD* FMTDepthStencil=NULL, UINT* Width=NULL, UINT* Height=NULL);


HRESULT	LcD3D_FontBegin(ID3DXFont* pFnt);
HRESULT	LcD3D_FontDraw(ID3DXFont* pFnt, char* pString, LPRECT pRect, DWORD Format=DT_NOCLIP, D3DCOLOR Color=0xFFFFFFFF, INT Count=-1);
HRESULT	LcD3D_FontEnd(ID3DXFont* pFnt);




void	LcD3D_VBCreate(PDEV pDev, PDVB& pVB, INT nSize, DWORD fvf, void* pVtx=NULL, D3DPOOL usage=D3DPOOL_MANAGED);
void	LcD3D_VBLock(PDVB& pVB, INT nSize, void* pVtx);
void	LcD3D_IBCreate(PDEV pDev, PDIB& pIB, INT nSize, void* pIdx=NULL, D3DFORMAT fmt=D3DFMT_INDEX16, D3DPOOL usage= D3DPOOL_MANAGED);
void	LcD3D_IBLock(PDIB& pIB, INT nSize, void* pIdx);



void	LcD3D_ScreenCapture(PDSF pSf);
void	LcD3D_ScreenCapture(PDEV pDev);



DWORD	LcD3D_GetVertexShaderVersion(PDEV pDev);
DWORD	LcD3D_GetVertexShaderVersionMajor(PDEV pDev);
DWORD	LcD3D_GetVertexShaderVersionMinor(PDEV pDev);

DWORD	LcD3D_GetPixelShaderVersion(PDEV pDev);
DWORD	LcD3D_GetPixelShaderVersionMajor(PDEV pDev);
DWORD	LcD3D_GetPixelShaderVersionMinor(PDEV pDev);


PDVD	LcD3D_GetVertexDeclarator(PDEV pDev, DWORD fvf);
void*	LcD3D_BuildShader(PDEV pDev, char* sStrAssem, int iLen, char* sShader);
void*	LcD3D_BuildShaderFromFile(PDEV pDev, char* sStrFile, char* sShader);

void*	LcD3D_BuildHLSL(PDEV pDev, char* sHLSL, int iLen, char* sFunction, char* sShader, PDCT* pTbl = NULL/*Out*/);
void*	LcD3D_BuildHLSLFromFile(PDEV pDev, char* sStrFile, char* sFunction, char* sShader, PDCT* pTbl = NULL/*Out*/);

void	LcD3D_SetVertexShaderConstant(PDEV pDev, UINT uR, MATA* v);
void	LcD3D_SetVertexShaderConstant(PDEV pDev, UINT uR, DCLR* v);
void	LcD3D_SetVertexShaderConstant(PDEV pDev, UINT uR, VEC4* v);
void	LcD3D_SetVertexShaderConstant(PDEV pDev, UINT uR, FLOAT* v);

void	LcD3D_SetPixelShaderConstant(PDEV pDev, UINT uR , DCLR* v);
void	LcD3D_SetPixelShaderConstant(PDEV pDev, UINT uR , FLOAT* v);
void	LcD3D_SetPixelShaderConstant(PDEV pDev, UINT uR , INT* v);

void	LcD3D_SetHlslTableConstant(PDCT& pTbl, PDEV pDev, char* sValue, MATA* matrix);
void	LcD3D_SetHlslVector(PDCT& pTbl, PDEV pDev, char* sValue, VEC4* value);

void*	LcD3D_EffectStringBuild(PDEV pDev, char* sStr, int iLen);

void*	LcD3D_EffectFileBuild(PDEV pDev, char* sStrFile);
void	LcD3D_EffectFileDestroy();
void	LcD3D_EffectFileLostDevice();
void	LcD3D_EffectFileResetDevice();


void	LcD3D_GetDxError(HRESULT hr, char* pBufferPointer=NULL);


// Redefine functions from d3dutil.cpp
void	LcD3D_InitMaterial( D3DMATERIAL9& mtrl,FLOAT r=0,FLOAT g=0,FLOAT b=0,FLOAT a=1);
void	LcD3D_InitLight(D3DLIGHT9& light, D3DLIGHTTYPE ltType, FLOAT x=0,FLOAT y=0, FLOAT z=0);





////////////////////////////////////////////////////////////////////////////////
//
// Interface for Swap chain window class.
//
////////////////////////////////////////////////////////////////////////////////
 
struct TswpWn
{
	PDSW	pC;																	// Swap chain
	PDSF	pB;																	// Back buffer surface
	PDSF	pS;																	// Stencil buffer surface
	HWND	hW;																	// Window Handle

	PDEV	pD;																	// Device
	PDSF	pOT;																// Original color buffer from Device
	PDSF	pOS;																// Original depth stencil buffer from Device
	DWORD	dCM;																// Clear Mode
	DWORD	dCc;																// Clear Color
	
	TswpWn();
	virtual ~TswpWn();

	INT		Create(PDEV pDev, HWND hWnd, DWORD dClearMode=D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL);

	INT		OnResetDevice();
	INT		OnLostDevice();
	void	Release();

	void	SetClearMode(DWORD d);
	void	SetClearColor(DWORD d);

	INT		BeginScene();				// BegineScene을 먼저 호출
	INT		Clear();					// 다음 Clear()호출
	INT		EndScene();					// 렌더링후 마지막으로 EndScene호출

	PDSF	GetBackBuffer()  const;
	PDSF	GetDepthStencil()  const;
};



typedef vector<TswpWn >		lsTswpWn;
typedef lsTswpWn::iterator	itTswpWn;





////////////////////////////////////////////////////////////////////////////////
//
// Interface for Shader class.
//
////////////////////////////////////////////////////////////////////////////////

struct TShd
{
	char	sVtx[512];
	char	sPxl[512];

	PDVD	pDcl;
	PDVS	pShd;
	PDPS	pPxl;

	TShd();
	~TShd();

	void	SetFileVtx(char*	sFile);
	void	SetFilePxl(char*	sFile);
	INT		SetDeclarator(PDEV pDev, DWORD dFVF);

	INT		CreateVertexShader(PDEV pd3dDev);
	INT		CreatePixelShader(PDEV pd3dDev);

};



////////////////////////////////////////////////////////////////////////////////
//
// Interface for Window Rect and Viewport class.
//
////////////////////////////////////////////////////////////////////////////////

struct TwinRc
{
	RECT	rc;
	DVWP	vp;

	TwinRc();
	TwinRc(const RECT& Rc, const DVWP& Vp);
	TwinRc(LONG X1,LONG Y1,LONG X2,LONG Y2);

	TwinRc(LONG X1,LONG Y1,LONG X2,LONG Y2
		, DWORD X,DWORD Y,DWORD W,DWORD H, FLOAT Min=0,FLOAT Max=1);

	RECT	GetRect();

};







////////////////////////////////////////////////////////////////////////////////
//
// Interface for the Render Target class.
//
////////////////////////////////////////////////////////////////////////////////

struct IrenderTarget
{
	virtual ~IrenderTarget();

	virtual	INT		Create(void* p1=NULL, void* p2=NULL, void* p3=NULL, void* p4=NULL)=0;
	virtual	void	Destroy()=0;

	virtual	INT		OnResetDevice()=0;
	virtual	INT		OnLostDevice()=0;

//	D3DCLEAR_TARGET	=0x00000001L;
//	D3DCLEAR_ZBUFFER=0x00000002L;
//	D3DCLEAR_STENCIL=0x00000004l;
	virtual	INT		BeginScene(DWORD dClearMode = (0x1L|0x2L|0x4L), DWORD dClearColor = 0xFF006699)=0;
	virtual	INT		EndScene()=0;

	virtual	INT		GetWidth()=0;				// Get Image Width
	virtual	INT		GetHeight()=0;				// Get Image Height
	virtual	DWORD	GetFmtColor()=0;			// Get Image Color Format
	virtual	DWORD	GetFmtDepth()=0;			// Get Image Depth Format

	virtual	void*	GetTexture() const=0;		// Return Texture pointer
	virtual	void*	GetSurface() const=0;		// Retrurn Surface pointer in Texture.
};

// IF INT iWidth=-1, INT iHeight=-1 then Width will be Backbuffer Width, Height will be Backbuffer Height.
//
INT LcD3D_CreateRenderTarget(char* sCmd, IrenderTarget** pData, void* pDevice, INT iWidth=-1, INT iHeight=-1);




////////////////////////////////////////////////////////////////////////////////
//
// Interface for the Render Target class2.
//
////////////////////////////////////////////////////////////////////////////////


class TcolorMap
{
protected:
	PDEV		m_pDev	;		// Device
	INT			m_iW	;		// Width
	INT			m_iH	;		// Height

	PDTX		m_pTxT	;		// 임시 텍스춰
	PDTX		m_pTx	;		// Rendering용 텍스춰
	PDSF		m_pSf	;		// Rendering용 텍스춰 surface

	PDSF		m_pBckC	;		// Back Buffer Surface
	PDSF		m_pBckD	;		// Back Buffer Depth and Stencil

public:
	TcolorMap();
	virtual ~TcolorMap();

	INT		Create(PDEV pDev, INT iW, INT iH);
	void	Destroy();

	INT		OnResetDevice();
	INT		OnLostDevice();

	HRESULT	Begin(DWORD dClearMode = (D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER));
	HRESULT	End();

	PDTX	GetTexture() const;
};


FLOAT	LcD3D_AngleFromXAxis(const VEC3* pLook, const VEC3* pEye);				// X축과 각도


#endif




