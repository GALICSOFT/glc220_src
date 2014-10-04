// Implementation of the CLcNetSlctA class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <WINSOCK2.H>
#include <windows.h>
#include <stdio.h>

#include <LnUtil.h>

#include "LnxNet.h"

#include "LnNet.h"
#include "LnNetSlctA.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)	{ if(p) { delete p; p = NULL; } }
#endif

CLcNetSlctA::_THost::_THost()
{
	scH = 0;
	memset(&sdH, 0, sizeof(sdH));
	pUsrData	= NULL;
}


CLcNetSlctA::CLcNetSlctA()
{
	m_hThSend	= NULL;
	m_dThSend	= 0;
	m_nThSend	= 0;

	m_iNsck	= 0;

	m_hWnd	= 0;
	m_dwMsg	= 0;
}


CLcNetSlctA::~CLcNetSlctA()
{
	Destroy();
}




INT CLcNetSlctA::Create(void* p1, void* p2, void* p3, void* p4)
{
	char* sIp  = (char*)p1;
	char* sPort= (char*)p2;
	char* sPtcl= (char*)p3;
	char* sSvr = (char*)p4;
	
	memset(m_sIp, 0, sizeof m_sIp);
	memset(m_sPt, 0, sizeof m_sPt);
	
	if(sIp  )	strcpy(m_sIp, sIp  );
	if(sPort)	strcpy(m_sPt, sPort);
	
	
	if(0==_stricmp("TCP", sPtcl))
		m_PtcType	= NETPRT_TCP;												// Protocol type
	
	else if(0==_stricmp("UDP", sPtcl))
		m_PtcType	= NETPRT_UDP;												// Protocol type
	
	
	if(0==_stricmp("Client", sSvr))
	{
		m_HstType	= NETHST_CLIENT;											// Client
		return Connect(NULL, NULL);
	}
	else if(0==_stricmp("Server", sSvr))
	{
		m_HstType	= NETHST_SERVER;											// Server
		return Listen();
	}
	
	return -1;
}

void CLcNetSlctA::Destroy()
{
	m_eNetSt = NETST_NONE;
	
	Close();

	if(m_hThSend)
		LcProc_ThreadClose(&m_hThSend	);

	SAFE_DELETE(	m_pScktMsg	);
	SAFE_DELETE(	m_pScktNew	);
	SAFE_DELETE(	m_pScktCls	);
}




INT CLcNetSlctA::FrameMove()
{
	if( FAILED(m_nThSend))
		return -1;

	LcProc_CsLock();

	INT hr = 0;

	if(NETHST_CLIENT == m_HstType)
	{
		hr = m_rbSnd.GetStore();
	}
	else if(NETHST_SERVER == m_HstType)
	{
		for(INT i=0;i<m_iNsck; ++i)
		{
			hr = m_rmH[i].rbSnd.GetStore();
			if(hr>0)
				break;
		}
	}

	if(hr>0)
		LcProc_ThreadResume(&m_hThSend);

	LcProc_CsUnlock();

	return 0;
}


INT CLcNetSlctA::Query(char* sCmd, void* pData)
{
	if(0==_stricmp("Get Client Number", sCmd))
	{
		*((INT*)pData) = m_iNsck-1;
		return 0;
	}

	else if(0==_stricmp("Set Window Handle", sCmd))
	{
		m_hWnd = *((HWND*)pData);
		return 0;
	}

	else if(0==_stricmp("Set Message Value", sCmd))
	{
		m_dwMsg = *((DWORD*)pData);
		return 0;
	}

	else if(0==_stricmp("Client Close", sCmd))
	{
		struct _T
		{
			INT		n;
			SOCKET	s;
		} *t = (_T*)pData;

		SOCKET scT = t->s;
		SOCKET scH = t->s;
		LcNet_SocketClose(&scT);
		int n = Fd_Clr(scH);

		printf("Close: %d Remain: %d\n", scH, (m_iNsck-1));
		// 삭제할 소켓리스트에 등록
		ScLstClosePush(scH, n-1);

		return 0;
	}

	else if(0==_stricmp("Window Procedure", sCmd))
	{
		struct _T
		{
			HWND	hWnd;
			UINT	uMsg;
			WPARAM	wPar;
			LPARAM  lPar;
		} *t = (_T*)pData;

		return MsgProc(t->hWnd, t->uMsg, t->wPar, t->lPar);
	}

	return CLcNet::Query(sCmd, pData);
}




INT CLcNetSlctA::Connect(char* sIp, char* sPort)
{
	if(sIp  ){	memset(m_sIp, 0, sizeof m_sIp);	strcpy(m_sIp, sIp  );	}
	if(sPort){	memset(m_sPt, 0, sizeof m_sPt);	strcpy(m_sPt, sPort);	}
	
	//2. 소켓 어드레스 세팅
	LcNet_SocketAddr(&m_sdH, m_sIp, m_sPt);
	
	
	//3. 소켓을 만든다.
	if(NETPRT_TCP==m_PtcType)
	{
		if(FAILED(LcNet_SocketTcpCreate(&m_scH)))								// TCP소켓을 만든다.
			return -1;
		
	}
	else if(NETPRT_UDP==m_PtcType)
	{
		if(FAILED(LcNet_SocketUdpCreate(&m_scH)))								// UDP소켓을 만든다.
			return -1;
	}
	

	// AsycSelect에 연결. 자동으로 Non-blocking전환
	INT hr = LcNet_WSAAsyncSelect(  m_scH
							, m_hWnd
							, m_dwMsg
							, (FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE));

	if(FAILED(LcNet_SocketErrorCheck(hr)))
	{
		ERROR_CHECK_MESSAGE(hr);

		return -1;
	}

	//4. 커넥션
	hr = LcNet_SocketConnect(m_scH, &m_sdH);

	if(FAILED(LcNet_SocketErrorCheck(hr)))
	{
		ERROR_CHECK_MESSAGE(hr);

		return -1;
	}


	m_eNetSt = NETST_CONNECTING;

	
	// Nagle 정지.
	LcNet_SocketNaggleOff(m_scH);

	Fd_Zero();
	Fd_Set(m_scH, &m_sdH);

	// Send 쓰레드
	m_hThSend = LcProc_ThreadCreate(ThreadSend, this, 0, &m_dThSend);

	return 0;
}




INT CLcNetSlctA::Close()
{
	// 소켓을 닫는다.
	LcNet_SocketClose(&m_scH);
	
	return 0;
}

INT CLcNetSlctA::Listen()
{
	//1. 소켓 어드레스 세팅
	LcNet_SocketAddr(&m_sdH, m_sIp, m_sPt);
	
	
	//2. 소켓을 만든다.
	if(NETPRT_TCP==m_PtcType)
	{
		if(FAILED(LcNet_SocketTcpCreate(&m_scH)))								// TCP소켓을 만든다.
			return -1;
	}
	else if(NETPRT_UDP==m_PtcType)
	{
		if(FAILED(LcNet_SocketUdpCreate(&m_scH)))								// UDP소켓을 만든다.
			return -1;
	}
	
	
	//3. 소켓 바인딩
	if(FAILED(LcNet_SocketBind(m_scH, &m_sdH)))
		return -1;


	// 입/출력 패킷에 관련된 큐 설정
	m_pScktMsg = new TqueCrc<CLcNet::_TsckMsg >(PCK_BUF_MESSAGE);
	m_pScktNew = new TqueCrc<CLcNet::_TsckMsg >(PCK_BUF_MESSAGE);
	m_pScktCls = new TqueCrc<CLcNet::_TsckMsg >(PCK_BUF_MESSAGE);

	
	//4. 비동기 연결
	// The WSAAsyncSelect function automatically sets socket' s to nonblocking mode
	INT hr = LcNet_WSAAsyncSelect(m_scH, m_hWnd, m_dwMsg, FD_ACCEPT|FD_READ|FD_WRITE|FD_CLOSE);

	if(FAILED(LcNet_SocketErrorCheck(hr)))
	{
		ERROR_CHECK_MESSAGE(hr);
		return -1;
	}

	//5. 소켓 리쓴
	if(FAILED(LcNet_SocketListen(m_scH)))
		return -1;

	Fd_Zero();
	Fd_Set(m_scH, &m_sdH);

	//6. Send용 쓰레드를 만든다.
	m_hThSend = LcProc_ThreadCreate(ThreadSend, this, 0, &m_dThSend);

	return 0;
}




INT CLcNetSlctA::Send(const char* pSrc, INT  iSnd, DWORD  dMsg, INT nIdx)
{
	BYTE	sBuf[PCK_BUF_MAX_MSG]={0};

	INT		iSize	= LcNet_PacketEncode(sBuf, (BYTE*)pSrc, iSnd, dMsg);
	INT		hr		= 0;

	if(0xFFFFFFFF==nIdx)
	{
		LcProc_CsLock();
		hr		= m_rbSnd.PushBack(sBuf, iSize);
		LcProc_CsUnlock();
		
		if(FAILED(hr))
			return -1;
	}

	else
	{
		nIdx +=1;

		if(nIdx<1 || nIdx >= m_iNsck)
			return -1;

		LcProc_CsLock();
		hr		= m_rmH[nIdx].rbSnd.PushBack(sBuf, iSize);
		LcProc_CsUnlock();

		if(FAILED(hr))
			return -1;
	}
	
	return 0;
}

INT CLcNetSlctA::Recv(char* pDst, INT* iRcv, DWORD* dMsg, SOCKET* scH)
{
	BYTE	sBuf[PCK_BUF_MAX_MSG]={0};
	WORD	iSize = 0;
	INT		hr	= 0;

	*iRcv = 0;

	if(NULL==scH)
	{
		LcProc_CsLock();
		hr = m_rbRcv.PopFront(sBuf, &iSize);
		LcProc_CsUnlock();
		
		if(FAILED(hr))
			return -1;

		if(0==iSize)
			return -1;

		*iRcv = LcNet_PacketDecode((BYTE*)pDst, dMsg, sBuf, iSize);
	}

	else
	{
		INT	nIdx = *scH+1;
		
		if(nIdx<1 || nIdx>= m_iNsck)
			return -1;

		LcProc_CsLock();
		hr = m_rmH[nIdx].rbRcv.PopFront(sBuf, &iSize);
		LcProc_CsUnlock();

		if(FAILED(hr) || 0==iSize)
			return -1;
		
		*iRcv = LcNet_PacketDecode((BYTE*)pDst, dMsg, sBuf, iSize);
		*scH = m_rmH[nIdx].scH;
	}

	return 0;
}


SOCKET CLcNetSlctA::GetSocket(INT nIdx)
{
	if(NETHST_CLIENT == m_HstType)
		return m_scH;

	nIdx +=1;

	if( nIdx<1 || nIdx >= m_iNsck)
		return 0xFFFFFFFF;

	return m_rmH[nIdx].scH;
}


INT CLcNetSlctA::SetUserData(ILcxNet::_Tdata* pData, INT nIdx)
{
	if(NETHST_CLIENT == m_HstType)
	{
		m_pUsrData = pData;
		return 0;
	}

	nIdx +=1;

	if( nIdx<1 || nIdx >= m_iNsck)
		return -1;

	m_rmH[nIdx].pUsrData = pData;

	return 0;
}

ILcxNet::_Tdata* CLcNetSlctA::GetUserData(INT nIdx)
{
	if(NETHST_CLIENT == m_HstType)
		return m_pUsrData;

	nIdx +=1;

	if( nIdx<1 || nIdx >= m_iNsck)
		return NULL;

	return m_rmH[nIdx].pUsrData;
}


//ILcxNet::_Tdata* CLcNetSlctA::GetUserData(SOCKET _scH)
//{
//	if(NETHST_CLIENT == m_HstType)
//		return m_pUsrData;
//
//	_Tdata*	 pUsrData = NULL;													// User Data
//
//	for(int i=0; i<m_iNsck; ++i)
//	{
//		if(m_rmH[i].scH == _scH)
//			return m_rmH[i].pUsrData;
//	}
//
//	return NULL;
//}




LRESULT CLcNetSlctA::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	SOCKET	scHost	= 0;
	DWORD	dError	= 0;
	DWORD	dEvent	= 0;

	if(uMsg != m_dwMsg || FAILED(m_nThSend))
		return 0;
	
	scHost	= (SOCKET)wParam;
	dError	= WSAGETSELECTERROR(lParam);
	dEvent	= WSAGETSELECTEVENT(lParam);

	if(dError)
	{
		// 클라이언트의 경우 비정상적인 에러
		printf("Network Error\n");

		if(NETHST_CLIENT == m_HstType || m_scH == scHost)
		{
			m_nThSend =-1;
			m_eNetSt = NETST_WSAERROR;
			LcProc_ThreadResume(&m_hThSend);
//			Sleep(10);
			return (0XFF000000 | dError);
		}
		else if(NETHST_SERVER == m_HstType)
		{
			SOCKET sT =scHost;
			LcNet_SocketClose(&sT);
			int n = Fd_Clr(scHost);

			printf("Close: %d Remain: %d\n", scHost, (m_iNsck-1));
			// 삭제할 소켓리스트에 등록
			ScLstClosePush(scHost, n-1);

			return 0;
		}		
	}


	if(FD_ACCEPT == dEvent)						// Accept
	{
		SOCKET			scH;
		SOCKADDR_IN		sdH;

		if(SUCCEEDED(LcNet_SocketAccept(&scH, &sdH, m_scH)))
		{
			if(scH)
			{
				// 비동기 셀렉트에 연결한다.
				LcNet_WSAAsyncSelect(scH, hWnd, m_dwMsg, (FD_READ|FD_WRITE|FD_CLOSE));

				int n = Fd_Set(scH, &sdH);	// 증가...

				// 생성 소켓리스트에 등록
				ScLstNewPush(scH, n-1);

				printf("New Client: %d \n", scH);
			}
		}

		return 0;
	}

	else if(FD_CONNECT == dEvent)
	{
		m_eNetSt = NETST_CONNECTED;
		return 0;
	}

	else if(FD_READ == dEvent)					// Receive
	{
		BYTE	sBuf[PCK_BUF_MAX_MSG]={0};
		INT		iSize = 0;

		if(NETHST_CLIENT == m_HstType && scHost == m_scH)
		{	
			iSize=recv(m_scH, (char*)sBuf, sizeof sBuf, 0);

			if(iSize>0)
			{
				sBuf[iSize]=0;

				LcProc_CsLock();
				m_rbRcv.PushBack(sBuf, iSize);
				LcProc_CsUnlock();
			}
		}
		
		else if(NETHST_SERVER == m_HstType)
		{
			for(INT i=1; i<m_iNsck; ++i)
			{
				SOCKET	s = m_rmH[i].scH;

				if(s == scHost)
				{
					iSize=recv(s, (char*)sBuf, sizeof sBuf, 0);

					if(iSize>0)
					{
						sBuf[iSize]=0;

						LcProc_CsLock();
						m_rmH[i].rbRcv.PushBack(sBuf, iSize);
						LcProc_CsUnlock();

						ScLstRecvPush(s, i-1);
					}
				}
			}
		}

		return 0;
	}

	else if(FD_CLOSE == dEvent)
	{
		if(NETHST_CLIENT == m_HstType)
		{
			m_nThSend =-1;
			m_eNetSt = NETST_CLOSE;
			LcProc_ThreadResume(&m_hThSend);
			Sleep(10);
			return -1;
		}
		else if(NETHST_SERVER == m_HstType)
		{
			SOCKET scT =scHost;
			LcNet_SocketClose(&scT);
			int n = Fd_Clr(scHost);

			printf("Close: %d Remain: %d\n", scHost, (m_iNsck-1));
			// 삭제할 소켓리스트에 등록
			ScLstClosePush(scHost, n-1);
		}

		return 0;
	}

	else if(FD_WRITE == dEvent)					// Sending
	{
//		printf("Message Sending...\n");
		return 0;
	}
	
	return 0;
}



DWORD CLcNetSlctA::ProcSend(void* pParam)
{
	// 보내기 링버퍼에 있는 내용을 꺼내와서 패킷을 보낸다.
	BYTE	sBuf[PCK_BUF_MAX_MSG]={0};
	WORD	iSize = 0;
	INT		hr = -1;

	while(SUCCEEDED(m_nThSend))
	{
		hr = 0;

		// 데이터 전송
		if(NETHST_CLIENT == m_HstType)
		{
			LcProc_CsLock();
			hr = m_rbSnd.PopFront(sBuf, &iSize);
			LcProc_CsUnlock();

			if(FAILED(hr))
			{
				LcProc_ThreadSuspend(&m_hThSend);
				continue;
			}

			hr = send(m_scH, (char*)sBuf, iSize, 0);

			if(FAILED(LcNet_SocketErrorCheck(hr)))
			{
				ERROR_CHECK_MESSAGE(hr);
				break;
			}

			else if( hr != iSize)
			{
				printf("Send(hr != iSize): %d\n", hr);
			}
		}

		else if(NETHST_SERVER == m_HstType)
		{
			for(INT i=0;i<m_iNsck; ++i)
			{
				if(m_scH != m_rmH[i].scH)
				{
					LcProc_CsLock();
					hr = m_rmH[i].rbSnd.PopFront(sBuf, &iSize);
					LcProc_CsUnlock();

					if(SUCCEEDED(hr))
					{
						hr = send(m_rmH[i].scH, (char*)sBuf, iSize, 0);

						if(FAILED(LcNet_SocketErrorCheck(hr)))
						{
							ERROR_CHECK_MESSAGE(hr);
							continue;
						}
					}
				}
			}
		}

		LcProc_ThreadSuspend(&m_hThSend);
	}

	m_nThSend = -1;
	return hr;
}


INT CLcNetSlctA::SendAllData()
{
	INT		hr=-1;
	
	// 보내기 링버퍼에 있는 내용을 꺼내와서 패킷을 보낸다.
	BYTE	sBuf[PCK_BUF_MAX_MSG]={0};
	WORD	iSize = 0;
	
	while(SUCCEEDED(m_nThSend))
	{
		hr = 0;

		if(NETHST_CLIENT == m_HstType)
		{
			LcProc_CsLock();
			hr = m_rbSnd.PopFront(sBuf, &iSize);
			LcProc_CsUnlock();

			// 전송할 메시가 없으면 빠져 나간다.
			if(FAILED(hr))
				return 0;
			
			hr = send(m_scH, (char*)sBuf, iSize, 0);
		}
		
		else if(NETHST_SERVER == m_HstType)
		{
			for(INT j=0;j<m_iNsck; ++j)
			{
				if(m_scH != m_rmH[j].scH)
				{
					LcProc_CsLock();
					hr = m_rmH[j].rbSnd.PopFront(sBuf, &iSize);
					LcProc_CsUnlock();

					if(SUCCEEDED(hr))
					{
						hr = send(m_rmH[j].scH, (char*)sBuf, iSize, 0);
					}
				}
			}
			
			return 0;			
		}


		if(FAILED(LcNet_SocketErrorCheck(iSize)))			// Error Check
		{
			ERROR_CHECK_MESSAGE(hr);
			return -1;
		}
	}

	return 0;
}


int CLcNetSlctA::Fd_Set(SOCKET scH, SOCKADDR_IN* sdH)
{
	int i;
	int n =-1;
	
	for (i=0; i<m_iNsck; ++i)
	{
		if (m_rmH[i].scH == scH)
		{
			n = i;
			break;
		}
	}

	if (i == m_iNsck)
	{
		n = i;

		if (m_iNsck < WSA_MAXIMUM_WAIT_EVENTS)
		{
			m_rmH[i].scH = scH;
			++m_iNsck;
		}
	}

	return n;
}



int CLcNetSlctA::Fd_Clr(SOCKET scH)
{
	int i;
	int	n = -1;

	// n 찾기
	for (i=0; i<m_iNsck; ++i)
	{
		if (m_rmH[i].scH == scH)
		{
			n = i;
			break;
		}
	}

	for (i=0; i<m_iNsck; ++i)
	{
		if (m_rmH[i].scH == scH)
		{
			if(m_rmH[i].pUsrData)
			{
				delete m_rmH[i].pUsrData;
				m_rmH[i].pUsrData = NULL;
			}

			while (i < m_iNsck-1)
			{
				memcpy(&m_rmH[i], &m_rmH[i+1], sizeof(CLcNetSlctA::_THost));
				
				++i;
			}
			
			INT nCnt = m_iNsck-1;

			m_rmH[nCnt].pUsrData = NULL;
			m_rmH[nCnt].rbRcv.Clear();
			m_rmH[nCnt].rbSnd.Clear();

			--m_iNsck;
			break;
		}
	}

	return n;
}


void CLcNetSlctA::Fd_Zero()
{
	m_iNsck =0;
}

