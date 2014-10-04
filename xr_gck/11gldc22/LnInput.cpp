// Implementation of the CLcInput class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <math.h>


#include "LnBase.h"
#include "LnInput.h"

#include "LnInputA.h"
#include "LnInputD.h"




CLcInput::CLcInput()
{
	m_eType		= LN_INPUT_0;

	m_hInst		= NULL;
	m_hWnd		= NULL;

	memset(m_KeyCur, 0, sizeof m_KeyCur);
	memset(m_KeyOld, 0, sizeof m_KeyOld);
	memset(m_KeyMap, 0, sizeof m_KeyMap);
	
	memset(m_BtnCur, 0, sizeof m_BtnCur);
	memset(m_BtnOld, 0, sizeof m_BtnOld);
	memset(m_BtnMap, 0, sizeof m_BtnMap);
	
	memset(m_vMsCur, 0, sizeof m_vMsCur);
	memset(m_vMsOld, 0, sizeof m_vMsOld);
}

CLcInput::~CLcInput()	{	}
void CLcInput::Destroy(){	}

INT CLcInput::Query(char*, void*)	{	return -1;		}

INT CLcInput::Create(void*, void*, void*, void*)	{	return -1;		}
INT	CLcInput::FrameMove()							{	return -1;		}

INT CLcInput::MsgProc(HWND, UINT, WPARAM, LPARAM)	{	return FALSE;	}




BYTE* CLcInput::GetKeyMap() const
{
	return (BYTE*)m_KeyMap;
}


BOOL CLcInput::KeyDown(INT nKey)
{
	return (INPUTST_DOWN == m_KeyMap[nKey])? 1: 0;
}

BOOL CLcInput::KeyUp(INT nKey)
{
	return (INPUTST_UP == m_KeyMap[nKey])? 1: 0;
}

BOOL CLcInput::KeyPress(INT nKey)
{
	return (INPUTST_PRESS == m_KeyMap[nKey])? 1: 0;
}

BOOL CLcInput::KeyState(int nKey)
{
	return m_KeyMap[nKey];
}




BYTE* CLcInput::GetButtonMap() const
{
	return (BYTE*)m_BtnMap;
}


BOOL CLcInput::ButtonDown(INT nBtn)
{
	return (INPUTST_DOWN == m_BtnMap[nBtn])? 1: 0;
}

BOOL CLcInput::ButtonUp(INT nBtn)
{
	return (INPUTST_UP == m_BtnMap[nBtn])? 1: 0;
}

BOOL CLcInput::ButtonPress(INT nBtn)
{
	return (INPUTST_PRESS == m_BtnMap[nBtn])? 1: 0;
}


BOOL CLcInput::ButtonState(int nBtn)
{
	return m_BtnMap[nBtn];
}


FLOAT* CLcInput::GetMousePos()
{
	return m_vMsCur;
}

FLOAT* CLcInput::GetMouseDelta()
{
	return m_vDelta;
}


BOOL CLcInput::GetMouseMove()
{
	BOOL hr= ( fabs(m_vMsCur[0] - m_vMsOld[0]) >0.f) ||
			 ( fabs(m_vMsCur[1] - m_vMsOld[1]) >0.f) ||
			 ( fabs(m_vMsCur[2] - m_vMsOld[2]) >0.f);

		
	return hr? 1: 0;
}


BOOL CLcInput::IsInRect(INT left, INT top, INT right, INT bottom)
{
	return (	m_vMsCur[0]>=left
			&&	m_vMsCur[1]>=top
			&&	m_vMsCur[0]<=right
			&&	m_vMsCur[1]<=bottom);
}




void CLcInput::ClientRect(HWND hWnd, RECT* rc)
{
	RECT rc1;
	RECT rc2;
	INT FrmW;
	INT FrmH;

	::GetWindowRect(hWnd, &rc1);
	::GetClientRect(hWnd, &rc2);

	FrmW = (rc1.right - rc1.left - rc2.right)/2;
	FrmH = rc1.bottom - rc1.top - rc2.bottom - FrmW;

	// Window Client Rect 시작 위치
	rc->left = rc1.left + FrmW;
	rc->top  = rc1.top  + FrmH;

	// Window Client Rect Width and Height
	rc->right	= rc2.right+ rc2.left;
	rc->bottom	= rc2.bottom+ rc2.top;
}






INT LcInput_Create(char* sCmd
				 , ILcInput** pData
				 , void* p1			// HINSTANCE
				 , void* p2			// HWND
				 , void* p3			// LPDIRECT3DDEVICE9
				 , void* p4			// No Use
				 )
{
	ILcInput* pObj = NULL;

	(*pData) = NULL;

	if(0==_stricmp("LC API", sCmd))
	{
		pObj = new CLcInputA;		
	}

	else if(0==_stricmp("LC Direct", sCmd))
	{
		pObj = new CLcInputD;
	}
	else
	{
		return -1;
	}


	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;
	return 0;
}
