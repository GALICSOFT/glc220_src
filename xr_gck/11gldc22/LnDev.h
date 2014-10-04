// Interface for the CLcDev class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcDev_H_
#define _LcDev_H_

#pragma warning(disable : 4786)
#include <vector>
using namespace std;


typedef LPDIRECT3D9				PD3D	;
typedef LPDIRECT3DDEVICE9		PDEV	;
typedef D3DPRESENT_PARAMETERS	D3PP	;
typedef LPDIRECT3DSURFACE9		PDSF	;


class CLcDev : public ILC_Device
{
protected:													// Function pointers for Host Program.
	ULONG_PTR	(*m_pFncMsgProc)(HWND,UINT,WPARAM,LPARAM);		// For Window Message Procedure.
	ULONG_PTR	(*m_pFncInit)();								// For Init.
	ULONG_PTR	(*m_pFncDestroy)();								// For Destroy.
	ULONG_PTR	(*m_pFncRestore)();								// For Restore.
	ULONG_PTR	(*m_pFncInvalidate)();							// For Invalidate.
	ULONG_PTR	(*m_pFncFrameMove)();							// For FrameMove.
	ULONG_PTR	(*m_pFncRender)();								// For Render.

	ULONG_PTR	(*m_pFncNetCreate)(void*);						// For Network Create

public:
	static HINSTANCE	m_hInst		;			// Window Instance
	static HWND	m_hWnd		;			// Main Window Handle
	static PDEV	m_pDev		;			// Direct3D Device
	static PDSF	m_pBcBuf	;			// BackBuffer Surface
	static PD3D	m_pD3D		;			// Direct3D

protected:
	D3PP		m_d3dpp		;			// D3D Present Parameters

protected:
	void*		m_pMain		;			// Main Application Pointer
	void*		m_pDev2D	;			// 2D Sprite Pointer
	void*		m_pInput	;			// Input Pointer
	void*		m_pCam		;			// Camera Pointer
	void*		m_pNet		;			// Network Pointer
	void*		m_pIme		;			// Ime Pointer
	void*		m_pCmd		;			// 비형식 Message and Command를 처리하기 위한 명령어

	void*		m_pUser1	;			// User Data Pointer 1
	void*		m_pUser2	;			// User Data Pointer 2
	void*		m_pUser3	;			// User Data Pointer 3
	void*		m_pUser4	;			// User Data Pointer 4


	vector<char* > m_vScFile;		// Lua Script Files.

	HANDLE		m_ConsoleH	;			// Console Window Handle
	BOOL		m_ConsoleB	;
	INT			m_ConsoleR	;
	FILE*		m_ConsoleF	;
	DWORD		m_ConsoleW	;

	DOUBLE		m_fTimeSrt	;			// Start Time

public:
	CLcDev();
	virtual ~CLcDev();
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);

	virtual INT		Init();
	virtual void	Destroy();

	virtual INT		Restore();
	virtual void	Invalidate();

	virtual INT		FrameMove();
	virtual INT		Render();

	virtual	INT		Run();

	virtual INT		Query(char* sCmd, void* pData);


	virtual INT		BeginScene();
	virtual INT		EndScene();
	virtual INT		Clear(DWORD Count,CONST RECT* pRects,DWORD Flags,DWORD Color,float Z,DWORD Stencil);


public:
	virtual void*	GetMainObject();						// Main Program Object Pointer
	virtual void*	GetHinst	();
	virtual void*	GetHwnd		();
	virtual void*	GetDevice	();
	virtual void*	GetBufferSurface();

	virtual void*	GetSprite	()	;						// 2D Sprite Pointer
	virtual void*	GetInput	()	;						// Input Pointer
	virtual void*	GetCamera	()	;						// Camera Pointer
	virtual void*	GetNetwork	()	;						// Network Pointer
	virtual void*	GetIme		()	;						// IME Pointer
	virtual void*	GetCommand	()	;						// Command Pointer

	virtual void*	GetUserData1()	;						// User Data Pointer 1
	virtual void*	GetUserData2()	;						// User Data Pointer 2
	virtual void*	GetUserData3()	;						// User Data Pointer 3
	virtual void*	GetUserData4()	;						// User Data Pointer 4

	virtual INT		SetMainObject(void*);					// Main Program Object Pointer
	virtual INT		SetSprite	(void*)	;
	virtual INT		SetInput	(void*)	;
	virtual INT		SetCamera	(void*)	;
	virtual INT		SetNetwork	(void*)	;
	virtual INT		SetIme		(void*)	;
	virtual INT		SetCommand	(void*)	;

	virtual INT		SetUserData1(void*)	;
	virtual INT		SetUserData2(void*)	;
	virtual INT		SetUserData3(void*)	;
	virtual INT		SetUserData4(void*)	;

	DOUBLE TimeGetTime();									// 하드웨어에서 지원되는 PerformanceCounter를 이용한 FPS구하기
};



#endif
