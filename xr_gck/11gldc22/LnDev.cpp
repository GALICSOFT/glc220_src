// Implementation of the CLcDev class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "LnBase.h"
#include "LnDev.h"






ILC_Device::LcDev::LcDev()
{
	PosX	= 0;
	PosY	= 0;
	ScnX	= 0;
	ScnY	= 0;

	hInst	= NULL;
	hWnd	= NULL;
	memset(sCls, 0, sizeof sCls);
}


ILC_Device::LcDev::LcDev(INT _PosX, INT _PosY, INT _Wx, INT _Wy, const char* _sCls, HINSTANCE _hInst, HWND _hWnd)
{
	PosX	= _PosX;
	PosY	= _PosY;
	ScnX	= _Wx;
	ScnY	= _Wy;

	hInst	= _hInst;
	hWnd	= _hWnd;
	memset(sCls, 0, sizeof sCls);

	if(_sCls && strlen(_sCls))
		strcpy(sCls, _sCls);
}







HINSTANCE	CLcDev::m_hInst		= NULL;
HWND		CLcDev::m_hWnd		= NULL;
PD3D		CLcDev::m_pD3D		= NULL;
PDEV		CLcDev::m_pDev		= NULL;
PDSF		CLcDev::m_pBcBuf	= NULL;


void* LcDev_GetD3Device()
{
	return CLcDev::m_pDev;
}


void LcDev_SetD3Device(void* pD3Dev)
{
	CLcDev::m_pDev = (PDEV)pD3Dev;
}



CLcDev::CLcDev()
{
	m_pFncMsgProc	= NULL;
	m_pFncInit		= NULL;
	m_pFncDestroy	= NULL;
	m_pFncRestore	= NULL;
	m_pFncInvalidate= NULL;
	m_pFncFrameMove	= NULL;
	m_pFncRender	= NULL;
	m_pFncNetCreate	= NULL;

	m_pMain		= NULL;

	m_pDev2D	= NULL;
	m_pInput	= NULL;
	m_pCam		= NULL;
	m_pNet		= NULL;
	m_pIme		= NULL;
	m_pCmd		= NULL;

	m_pUser1	= NULL;
	m_pUser2	= NULL;
	m_pUser3	= NULL;
	m_pUser4	= NULL;


	m_ConsoleH	= NULL	;
	m_ConsoleB	= 0		;
	m_ConsoleR	= 0		;
	m_ConsoleF	= NULL	;
	m_ConsoleW	= 0		;

	m_fTimeSrt	= TimeGetTime();
}

CLcDev::~CLcDev()
{
	CLcDev::Destroy();
}


INT CLcDev::Create(void*,void*,void*,void*)	{	return -1;	}
INT CLcDev::Init()			{	return -1;	}
void CLcDev::Destroy()		{				}
INT CLcDev::Restore()		{	return -1;	}
void CLcDev::Invalidate()	{				}
INT	CLcDev::FrameMove()		{	return -1;	}
INT CLcDev::Render()		{	return -1;	}
INT CLcDev::Run()			{	return -1;	}

INT CLcDev::BeginScene()	{	return -1;	}
INT CLcDev::EndScene()		{	return -1;	}
INT CLcDev::Clear(DWORD,CONST RECT*,DWORD, DWORD,float,DWORD){	return -1;	}


void* CLcDev::GetMainObject()	{	return m_pMain	;	}
void* CLcDev::GetHinst()		{	return m_hInst  ;	}
void* CLcDev::GetHwnd()			{	return m_hWnd	;	}
void* CLcDev::GetDevice()		{	return m_pDev	;	}
void* CLcDev::GetBufferSurface(){	return m_pBcBuf	;	}

void* CLcDev::GetSprite()		{	return m_pDev2D	;	}
void* CLcDev::GetInput()		{	return m_pInput	;	}
void* CLcDev::GetCamera()		{	return m_pCam	;	}
void* CLcDev::GetNetwork()		{	return m_pNet	;	}
void* CLcDev::GetIme()			{	return m_pIme	;	}
void* CLcDev::GetCommand()		{	return m_pCmd	;	}

void* CLcDev::GetUserData1()	{	return m_pUser1	;	}
void* CLcDev::GetUserData2()	{	return m_pUser2	;	}
void* CLcDev::GetUserData3()	{	return m_pUser3	;	}
void* CLcDev::GetUserData4()	{	return m_pUser4	;	}

INT	CLcDev::SetMainObject(void* v){	m_pMain	= v; return 0;	}
INT	CLcDev::SetSprite (void* v)	{	m_pDev2D= v; return 0;	}
INT	CLcDev::SetInput  (void* v)	{	m_pInput= v; return 0;	}
INT	CLcDev::SetCamera (void* v)	{	m_pCam	= v; return 0;	}
INT	CLcDev::SetNetwork(void* v)	{	m_pNet	= v; return 0;	}
INT	CLcDev::SetIme	  (void* v)	{	m_pIme	= v; return 0;	}
INT	CLcDev::SetCommand(void* v)	{	m_pCmd	= v; return 0;	}

INT	CLcDev::SetUserData1(void* v){	m_pUser1= v; return 0;	}
INT	CLcDev::SetUserData2(void* v){	m_pUser2= v; return 0;	}
INT	CLcDev::SetUserData3(void* v){	m_pUser3= v; return 0;	}
INT	CLcDev::SetUserData4(void* v){	m_pUser4= v; return 0;	}



//Query 대신 시스템 데이터를 함수로 편리하게 사용하기 위해서...
void* LcDev_GetHinst		(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetHinst()		;	}
void* LcDev_GetHwnd			(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetHwnd()		;	}
void* LcDev_GetBackBufferSurface(ILC_Device* pDev)	{	if(!pDev) return  0;	return pDev->GetBufferSurface();}
void* LcDev_GetUserSprite	(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetSprite()	;	}
void* LcDev_GetUserInput	(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetInput()		;	}
void* LcDev_GetUserCamera	(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetCamera()	;	}
void* LcDev_GetUserNetwork	(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetNetwork()	;	}
void* LcDev_GetUserIme		(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetIme()		;	}
void* LcDev_GetUserCommand	(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetCommand()	;	}

void* LcDev_GetUserData1	(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetUserData1()	;	}
void* LcDev_GetUserData2	(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetUserData2()	;	}
void* LcDev_GetUserData3	(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetUserData3()	;	}
void* LcDev_GetUserData4	(ILC_Device* pDev)		{	if(!pDev) return  0;	return pDev->GetUserData4()	;	}

INT	  LcDev_SetUserSprite	(ILC_Device* pDev,void* p){	if(!pDev) return -1;	return pDev->SetSprite(p)	;	}
INT	  LcDev_SetUserInput	(ILC_Device* pDev,void* p){	if(!pDev) return -1;	return pDev->SetInput(p)	;	}
INT	  LcDev_SetUserCamera	(ILC_Device* pDev,void* p){	if(!pDev) return -1;	return pDev->SetCamera(p)	;	}
INT	  LcDev_SetUserNetwork	(ILC_Device* pDev,void* p){	if(!pDev) return -1;	return pDev->SetNetwork(p)	;	}
INT	  LcDev_SetUserIme		(ILC_Device* pDev,void* p){	if(!pDev) return -1;	return pDev->SetIme(p)		;	}
INT	  LcDev_SetUserCommand	(ILC_Device* pDev,void* p){	if(!pDev) return -1;	return pDev->SetCommand(p)	;	}

INT	  LcDev_SetUserData1	(ILC_Device* pDev,void* p){	if(!pDev) return -1;	return pDev->SetUserData1(p);	}
INT	  LcDev_SetUserData2	(ILC_Device* pDev,void* p){	if(!pDev) return -1;	return pDev->SetUserData2(p);	}
INT	  LcDev_SetUserData3	(ILC_Device* pDev,void* p){	if(!pDev) return -1;	return pDev->SetUserData3(p);	}
INT	  LcDev_SetUserData4	(ILC_Device* pDev,void* p){	if(!pDev) return -1;	return pDev->SetUserData4(p);	}




INT CLcDev::Query(char* sCmd,void* pData)
{
	if(0==_stricmp("Create Network Instance", sCmd))
	{
		if(NULL == m_pFncNetCreate)
			return -1;

		return m_pFncNetCreate(pData);
	}

	return -1;
}












//	Timers
//	Windows NT Workstation version 3.5 makes it possible for applications to use the basic system timer with the GetTickCount( ) API.
//	The resolution of this timer is 10 milliseconds.
//	Several CPUs support a high-resolution counter that can be used to get very granular resolution.
//	The Win32 QueryPerformanceCounter( ) API returns the resolution of a high-resolution performance counter.
//	For Intel®-based CPUs, the resolution is about 0.8 microseconds.
//	For MIPS®-based CPUs, the resolution is about twice the clock speed of the processor.
//	You need to call QueryPerformanceFrequency( ) to get the frequency of the high-resolution performance counter.

//ULONGLONG RdtscClock()
//{
//	ULONGLONG	uTick;
//	_asm
//	{
//		rdtsc
//		mov DWORD PTR uTick    , eax
//		mov DWORD PTR uTick + 4, edx
//	}
//
//	return uTick;
//
////	_asm rdtsc;
//}



DOUBLE CLcDev::TimeGetTime()	// 하드웨어에서 지원되는 PerformanceCounter를 이용한 FPS구하기
{
//	typedef union _LARGE_INTEGER
//	{
//		struct
//		{
//			DWORD LowPart;
//			LONG HighPart;
//		};
//
//		struct
//		{
//			DWORD LowPart;
//			LONG HighPart;
//		} u;
//
//	} LARGE_INTEGER;


	static LARGE_INTEGER	lFreq	={0};
	static BOOL				bQuery	=FALSE;
	static BOOL				bGetTime=FALSE;

	static LARGE_INTEGER	lBgn	={0};
	LARGE_INTEGER			lEnd	={0};
	DOUBLE					lElapse = 0.;

	// QueryPerformanceFrequency를 쓸 수 없다면 timeGetTime을 사용한다.
	if(FALSE == bQuery && FALSE ==bGetTime)
	{
		bQuery = QueryPerformanceFrequency( &lFreq);

		if(FALSE == bQuery)
		{
			bGetTime	= TRUE;
			lBgn.LowPart= ::timeGetTime();
		}

		else
			QueryPerformanceCounter(&lBgn);
	}

	if(bQuery)
	{
		QueryPerformanceCounter( &lEnd);
		lElapse = DOUBLE( (lEnd.QuadPart-lBgn.QuadPart) * 1000.)/ lFreq.QuadPart;
	}
	else
	{
		lEnd.LowPart= ::timeGetTime();
		lElapse		= lEnd.LowPart-lBgn.LowPart;
	}


	return lElapse;
}




	
