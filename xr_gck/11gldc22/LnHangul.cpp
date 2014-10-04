// Implementation of the CLcHangul class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <windows.h>


#include "LnBase.h"
#include "LnHangul.h"


CLcHangul::CLcHangul()
{
	m_bUse	= FALSE;
	
	memset(m_sWrd, 0, sizeof m_sWrd);
	memset(m_sStr, 0, sizeof m_sStr);
	memset(m_sOut, 0, sizeof m_sOut);

	m_dStB	= 0;
}


CLcHangul::~CLcHangul()
{
	CLcHangul::Destroy();
}


void CLcHangul::Destroy()
{
	Reset();
}


INT CLcHangul::Create(void* p1, void*, void*, void*)
{
	if(p1)
		m_dStB = *((DWORD*)p1);

	return 0;
}


void CLcHangul::Set()
{
	m_bUse	= TRUE;
	
	memset(m_sWrd, 0, sizeof(m_sWrd));
	memset(m_sStr, 0, sizeof(m_sStr));
	memset(m_sOut, 0, sizeof(m_sOut));
}


void CLcHangul::Reset()
{
	m_bUse	= FALSE;
	
	memset(m_sWrd, 0, sizeof m_sWrd);
	memset(m_sStr, 0, sizeof m_sStr);
	memset(m_sOut, 0, sizeof m_sOut);
}


BOOL CLcHangul::GetState()			{ return m_bUse;	}
void CLcHangul::SetState(BOOL use)	{ m_bUse = use;		}



void CLcHangul::EraseWord()
{
	if( strlen(m_sWrd) )
		memset(m_sWrd, 0, sizeof(m_sWrd));
	
	else
	{
		int iLength = strlen(m_sStr);
		
		if( iLength > 0)
		{
			if(m_sStr[iLength - 1] < 0)											// 한글은 2바이트 코드...
			{
				m_sStr[iLength - 1] = 0;
				m_sStr[iLength - 2] = 0;
			}
			
			else																// 1바이트 코드들...
				m_sStr[iLength - 1] = 0;
		}
	}
}



INT CLcHangul::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(!m_bUse)
		return FALSE;

	INT		len = 0;
	HIMC	hIMC= 0;
	WPARAM	wLo = LOWORD(wParam);


	if(m_dStB && WM_COMMAND == uMsg && m_dStB == wLo)							// String Table를 사용하고 Back space로 정의된 이벤트가 발생한 경우
	{
		EraseWord();
		return FALSE;
	}
	
	
	if(WM_IME_STARTCOMPOSITION == uMsg)
		return TRUE;
		
	else if(WM_IME_COMPOSITION == uMsg)											// 글씨 조합 중
	{
		hIMC = ImmGetContext(hWnd);												// Get IME Handle
		
		if (lParam & GCS_RESULTSTR)
		{
			len = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);

			if( 0 < len)														// 글씨가 조합이 되었으면
			{
				ImmGetCompositionString(hIMC, GCS_RESULTSTR, m_sWrd, len);		// 현재 IME의 스트링 길이를 얻는다

				if( strlen(m_sStr) <= MAX_CHAT_LEN)
					strcat(m_sStr, m_sWrd);

				memset(m_sWrd,0, sizeof(m_sWrd));
			}
		}
			
		else if (lParam & GCS_COMPSTR)											// 조합중이면;
		{
			len = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);			// 조합중인 길이를 얻는다.
			
			if(0==len)															// 조합중에 Back space가 온 경우
				EraseWord();

			else
				ImmGetCompositionString(hIMC, GCS_COMPSTR, m_sWrd, len);		// str에  조합중인 문자를 얻는다.
		}
		
		ImmReleaseContext(hWnd, hIMC);											// IME 핸들 반환!!
		
		return TRUE;
	}

	else if(WM_CHAR == uMsg)													// 한글이 조합중이 아닐 때(영문자, 기타 키 이벤트)
	{
		if( strlen(m_sStr) <= MAX_CHAT_LEN)
		{
			if(wParam>=32 && wParam<127)
				m_sStr[strlen(m_sStr)] = wParam;

			else if(!m_dStB && VK_BACK ==wParam)								// 스트링 테이블를 사용하지 않고 백스페이스인 경우
			{
				INT c;
				EraseWord();
				c=0;
			}
		}	
	}
	
	return FALSE;
}




int CLcHangul::OutStringBeam(char* sOut)
{
	strcpy(m_sOut, m_sStr);
	strcat(m_sOut, m_sWrd);

	if(!m_bUse)
	{
		sOut[0] = 0;
		sOut[1] = 0;
		return 0;
	}

	strcpy(sOut, m_sOut);

	static DWORD start = timeGetTime();
	static DWORD end;
	
	end = timeGetTime();
	
	if(end-start>1200)
		start = end;
	
	if(end-start>600)
		strcat(sOut, " ");
	
	else
		strcat(sOut, "|");

	return strlen(m_sOut);
}



INT CLcHangul::OutStringStar(char* sOut)
{
	char sOut1[512]={0};
	
	strcpy(sOut1, m_sStr);
	strcat(sOut1, m_sWrd);
	
	memset(sOut, '*', strlen(sOut1));
	
	static DWORD start = timeGetTime();
	static DWORD end;
	
	end = timeGetTime();
	
	if(end-start>1000)
		start = end;
	
	if(end-start>500)
		strcat(sOut, "|");

	else
		strcat(sOut, " ");

	return strlen(m_sOut);
}


int CLcHangul::OutString(char* sOut)
{
	strcpy(m_sOut, m_sStr);
	strcat(m_sOut, m_sWrd);

	if(!m_bUse)
	{
		sOut[0] = 0;
		sOut[1] = 0;
		return 0;
	}
	
	strcpy(sOut, m_sOut);
	
	return strlen(m_sOut);
}






INT CLcHangul::Query(char*, void*)
{
	return -1;
}







INT LcIme_Create(char* sCmd
				 , ILcIme** pData
				 , void* p1			// String Table Back space Id
				 , void* p2			// No Use
				 , void* p3			// No Use
				 , void* p4			// No Use
				 )
{
	ILcIme*	pObj = NULL;

	(*pData) = NULL;

	if(0==_stricmp("Hangul", sCmd))
	{
		pObj = new CLcHangul;	
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

