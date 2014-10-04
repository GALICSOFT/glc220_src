// Implementation of the CLcInputA class.
//
////////////////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT			0x0400

#include <windows.h>
#include <d3dx9.h>

#include "LnBase.h"
#include "LnInput.h"
#include "LnInputA.h"





CLcInputA::CLcInputA()
{
	m_eType		= LN_INPUT_A;
	m_fWheel	= 0.f;
}


CLcInputA::~CLcInputA()
{
	Destroy();
}


void CLcInputA::Destroy()
{
}


INT CLcInputA::Query(char* sCmd, void* pData)
{
	if(0==_stricmp("Reset", sCmd))
	{
		OnReset();
		return 0;
	}

	return -1;
}


INT CLcInputA::Create(void* p1, void* p2, void*, void*)
{
	m_hInst= (HINSTANCE)p1;
	m_hWnd = (HWND)p2;

	return 0;
}

INT CLcInputA::FrameMove()
{
	INT		i=0;
	POINT	pt;
	SHORT	Val = 0;

	memcpy(m_KeyOld, m_KeyCur, sizeof m_KeyOld);
	memcpy(m_BtnOld, m_BtnCur, sizeof m_BtnOld);
	memcpy(m_vMsOld, m_vMsCur, sizeof m_vMsOld);

	memset(m_KeyCur,		0, sizeof m_KeyCur);
	memset(m_BtnCur,		0, sizeof m_BtnCur);
	memset(m_KeyMap,		0, sizeof m_KeyMap);
	memset(m_BtnMap,		0, sizeof m_BtnMap);


	HWND hWnd = GetActiveWindow();

	if(!m_hWnd || m_hWnd != hWnd)
		return 0;

	for(i=0; i<256; ++i)
	{
		Val = GetAsyncKeyState(i) & 0x8000;
		m_KeyCur[i] = (Val)? 1: 0;
	
		
		INT nOld = m_KeyOld[i];
		INT nCur = m_KeyCur[i];

		if		(0 == nOld && 1 ==nCur)	m_KeyMap[i] = INPUTST_DOWN;		// Down
		else if	(1 == nOld && 0 ==nCur)	m_KeyMap[i] = INPUTST_UP;		// Up
		else if	(1 == nOld && 1 ==nCur)	m_KeyMap[i] = INPUTST_PRESS;	// Press
	}

	m_BtnMap[0] = m_KeyMap[VK_LBUTTON];
	m_BtnMap[1] = m_KeyMap[VK_RBUTTON];
	m_BtnMap[2] = m_KeyMap[VK_MBUTTON];


	::GetCursorPos(&pt);
	::ScreenToClient( m_hWnd, &pt);
	
	m_vMsCur[0] = FLOAT(pt.x);
	m_vMsCur[1] = FLOAT(pt.y);

	if(-786420000.f<m_vMsCur[2] && m_vMsCur[2] <786420000.f)
		m_vMsCur[2] += m_fWheel;
	

	m_vDelta[0] = m_vMsCur[0] - m_vMsOld[0];
	m_vDelta[1] = m_vMsCur[1] - m_vMsOld[1];
	m_vDelta[2] = m_vMsCur[2] - m_vMsOld[2];

	m_fWheel	= 0.f;

	return 0;
}


void CLcInputA::AddWheelPos(INT d)
{
	m_fWheel = FLOAT(d);
}



void CLcInputA::SetMousePos(FLOAT* vcPos)
{
	OnReset();

	m_vMsCur[0] = vcPos[0];
	m_vMsCur[1] = vcPos[1];
	m_vMsCur[2] = vcPos[2];

	POINT	pt;
	pt.x= (INT)vcPos[0];
	pt.y= (INT)vcPos[1];

	::ClientToScreen(m_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}



void CLcInputA::OnReset()
{
	// 키보드는 전부 0
	SetKeyboardState(m_KeyCur);
	
	memset(m_KeyCur, 0, sizeof m_KeyCur);
	memset(m_KeyOld, 0, sizeof m_KeyOld);
	memset(m_KeyMap, 0, sizeof m_KeyMap);
	
	memset(m_BtnCur, 0, sizeof m_BtnCur);
	memset(m_BtnOld, 0, sizeof m_BtnOld);
	memset(m_BtnMap, 0, sizeof m_BtnMap);

	// 마우스는 현재 위치 유지
	m_fWheel = 0;

	POINT	pt;

	::GetCursorPos(&pt);
	::ScreenToClient( m_hWnd, &pt);

	m_vMsCur[0] = FLOAT(pt.x);
	m_vMsCur[1] = FLOAT(pt.y);
	m_vMsCur[2] = 0.f;

	memcpy(m_vMsOld, m_vMsCur, sizeof m_vMsCur);
	memset(m_vDelta,        0, sizeof m_vDelta);
}




INT CLcInputA::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
		case WM_MOUSEWHEEL:
		{
			AddWheelPos( SHORT( HIWORD(wParam) ));
			return FALSE;
		}
	}
	
	return FALSE;
}



