// Implementation of the CLcNet class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>

#include <LnUtil.h>

#include "LnxNet.h"

#include "LnNet.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)	{ if(p) { delete p; p = NULL; } }
#endif


CLcNet::CLcNet()
{
	m_scH		= 0;
	memset(&m_sdH, 0, sizeof m_sdH);
	memset(m_sIp, 0, sizeof m_sdH);
	memset(m_sPt, 0, sizeof m_sPt);

	m_pUsrData	= NULL;
	
	m_PtcType	= NETPRT_NONE;		// Protocol type
	m_HstType	= NETHST_NONE;		// Client Server?

	m_eNetSt	= NETST_NONE;

	m_pScktMsg	= NULL;
	m_pScktNew	= NULL;
	m_pScktCls	= NULL;
}

CLcNet::~CLcNet()
{
	Destroy();
}


void CLcNet::Destroy()
{
	SAFE_DELETE(	m_pUsrData	);
	SAFE_DELETE(	m_pScktMsg	);
	SAFE_DELETE(	m_pScktNew	);
	SAFE_DELETE(	m_pScktCls	);
}


	SOCKET			m_scH;				// Host socket
	SOCKADDR_IN		m_sdH;				// Host Address
	char			m_sIp[32];			// IP
	char			m_sPt[16];			// Port

	INT				m_PtcType;			// Protocol type
	INT				m_HstType;			// Client or Server?

	INT				m_eNetSt;			// Network State

INT CLcNet::Query(char* sCmd, void* pData)
{
	if(0==_stricmp("Get Host Socket", sCmd))
	{
		*((SOCKET*)pData) = m_scH;
		return 0;
	}

	else if(0==_stricmp("Get Host Address", sCmd))
	{
		memcpy(pData, &m_sdH, sizeof(SOCKADDR_IN));
		return 0;
	}

	else if(0==_stricmp("Get Host Ip", sCmd))
	{
		strcpy((char*)pData, m_sIp);
		return 0;
	}

	else if(0==_stricmp("Get Host Port", sCmd))
	{
		strcpy((char*)pData, m_sPt);
		return 0;
	}

	else if(0==_stricmp("Get Protocol type", sCmd))
	{
		*((SOCKET*)pData) = m_PtcType;
		return 0;
	}

	else if(0==_stricmp("Get Network State", sCmd))
	{
		*((SOCKET*)pData) = m_eNetSt;
		return 0;
	}
	
	return -1;
}

	
LRESULT	CLcNet::MsgProc(HWND, UINT, WPARAM, LPARAM)
{
	return -1;
}


INT CLcNet::NetState()
{
	return m_eNetSt;
}


DWORD WINAPI CLcNet::ThreadRecv(void* pParam)
{
	return ((CLcNet*)pParam)->ProcRecv(pParam);
}


DWORD WINAPI CLcNet::ThreadSend(void* pParam)
{
	return ((CLcNet*)pParam)->ProcSend(pParam);
}


DWORD WINAPI CLcNet::ThreadAccp(void* pParam)
{
	return ((CLcNet*)pParam)->ProcAccp(pParam);
}


DWORD WINAPI CLcNet::ThreadWork(void* pParam)
{
	return ((CLcNet*)pParam)->ProcWork(pParam);
}


DWORD CLcNet::ProcRecv(void* pParam)
{
	return 0;
}


DWORD CLcNet::ProcSend(void* pParam)
{
	return 0;
}

DWORD CLcNet::ProcAccp(void* pParam)
{
	return 0;
}


DWORD CLcNet::ProcWork(void* pParam)
{
	return 0;
}





void CLcNet::ScLstRecvPush(SOCKET s, int n)
{
	LcProc_CsLock();

	CLcNet::_TsckMsg msg(s, n);
	m_pScktMsg->PushBack(&msg);

	LcProc_CsUnlock();
}

INT CLcNet::ScLstRecvPop(SOCKET* s, int* n)
{
	INT hr = -1;

	if(m_pScktMsg->IsEmpty())
		return -1;

	LcProc_CsLock();

	// 메시지 버퍼에서 소켓을 꺼내온다.
	CLcNet::_TsckMsg msg;

	m_pScktMsg->PopFront(&msg);

	if(s)	*s	= msg.nScH;
	if(n)	*n	= msg.nIdx;

	hr  = m_pScktMsg->GetSize();

	LcProc_CsUnlock();

	return hr;
}

INT CLcNet::ScLstRecvCount()
{
	return m_pScktMsg->GetSize();
}



void CLcNet::ScLstNewPush(SOCKET s, int n)
{
	CLcNet::_TsckMsg msg(s, n);
	
	LcProc_CsLock();
	m_pScktNew->PushBack(&msg);
	LcProc_CsUnlock();
}

INT CLcNet::ScLstNewPop(SOCKET* s, int* n)
{
	INT hr = -1;

	if(m_pScktNew->IsEmpty())
		return -1;

	LcProc_CsLock();

	// New 소켓 버퍼에서 소켓을 꺼내온다.
	CLcNet::_TsckMsg msg;

	m_pScktNew->PopFront(&msg);
	
	if(s)	*s = msg.nScH;
	if(n)	*n = msg.nIdx;

	hr   = m_pScktNew->GetSize();

	LcProc_CsUnlock();

	return hr;
}

INT CLcNet::ScLstNewCount()
{
	return m_pScktNew->GetSize();
}



void CLcNet::ScLstClosePush(SOCKET s, int n)
{
	LcProc_CsLock();

	CLcNet::_TsckMsg msg(s, n);
	m_pScktCls->PushBack(&msg);

	LcProc_CsUnlock();
}

INT CLcNet::ScLstClosePop(SOCKET* s, int* n)
{
	INT hr = -1;

	if(m_pScktCls->IsEmpty())
		return -1;

	LcProc_CsLock();

	// Close 소 버퍼에서 소켓을 꺼내온다.
	CLcNet::_TsckMsg msg;

	m_pScktCls->PopFront(&msg);
	
	if(s)	*s = msg.nScH;
	if(n)	*n = msg.nIdx;

	hr   = m_pScktCls->GetSize();

	LcProc_CsUnlock();

	return hr;
}

INT CLcNet::ScLstCloseCount()
{
	return m_pScktCls->GetSize();
}

