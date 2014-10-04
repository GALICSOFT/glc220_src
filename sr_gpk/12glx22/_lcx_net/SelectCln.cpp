//
//
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#if defined(_MSC_VER)
  #include <winsock2.h>
  #include <process.h>

  typedef int				socklen_t;

#else

#if !defined(__BADA__)
  #include <netdb.h>
#endif

  #include <pthread.h>
  #include <unistd.h>
  #include <errno.h>
  #include <unistd.h>
  #include <fcntl.h>

  #include <arpa/inet.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <sys/socket.h>
  #include <sys/select.h>
  #include <sys/time.h>
  #include <sys/ioctl.h>
  #include <sys/epoll.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>

  // compatible to winsock
  typedef int				SOCKET;
  typedef sockaddr			SOCKADDR;
  typedef sockaddr_in		SOCKADDR_IN;
  typedef linger			LINGER;
  typedef hostent			HOSTENT;
  typedef fd_set			FD_SET;
  typedef timeval			TIMEVAL;
  typedef epoll_event		EP_EVENT;

  #define closesocket		close
  #define INVALID_SOCKET	(~0)
  #define SOCKET_ERROR		-1
  #define SD_BOTH			SHUT_RDWR
  #define EIO_PENDING		EINPROGRESS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcAux.h>

#include "../LcxNetwork.h"
#include "SelectCln.h"


CSelectCln::CSelectCln()
{
	m_bNetSt	= LCNET_OK;
	m_scHost	= 0;
	memset(&m_fdSet, 0, sizeof(FD_SET));

	m_uPt		= 0;
	memset(m_uIp, 0, sizeof m_uIp);

	m_nSnd		= 0;
	m_nRcv		= 0;
	memset(m_sSnd, 0, LC_PCK_SEND+4);
	memset(m_sRcv, 0, LC_PCK_RECV+4);
}


CSelectCln::~CSelectCln()
{
	Destroy();
}


void CSelectCln::Destroy()
{
	if(!m_scHost)
		return;

	m_smSnd.Close();
	m_scKey.Close();

	LcNet_SocketClose(&m_scHost);
	LcNet_Close();

	m_bNetSt = LCNET_EFAIL;
}


INT CSelectCln::Query(char* sCmd, LC_HANDLE v)
{
	if(     0 == LcStr_Stricmp("Update", sCmd))
	{
		return this->ProcRecv();
	}
	else if( 0 == LcStr_Stricmp("Connect", sCmd))
	{
		if(v)
		{
			TLC_SOCKET  *t = (TLC_SOCKET*)v;

			this->m_uPt = t->uPt;
			memcpy(this->m_uIp, t->uIp, LC_IP_LEN);
		}

		return this->Connect();
	}
	else if( 0 == LcStr_Stricmp("Disconnect", sCmd))
	{
		return this->DisConnect();
	}
	else
		return LCNET_EFAIL;

	return LCNET_OK;
}


INT CSelectCln::Release()
{
	return LCNET_OK;
}

INT CSelectCln::Send(LC_SOCKET  sc, TLC_PACKET* pPck)
{
	if(!m_scHost || NULL == pPck)
		return -1;


	if(!m_bNetSt)
		return 0;

	m_scKey.Lock();
		char* s= pPck->GetData();
		m_nSnd = pPck->GetLengthData();

		memcpy(m_sSnd, s, m_nSnd);
	m_scKey.Unlock();

	m_smSnd.SetSignal();

	return LCNET_OK;
}

INT CSelectCln::Recv(LC_SOCKET* sc, TLC_PACKET* pPck)
{
	return LCNET_OK;
}

INT CSelectCln::RecvCount()
{
	return LCNET_OK;
}

INT CSelectCln::SetAttrib(UINT dCmd, LC_HANDLE v)
{
	return LCNET_OK;
}

INT CSelectCln::GetAttrib(UINT dCmd, LC_HANDLE v)
{
	if(LCNET_STATE == dCmd)
	{
		*((INT*)v) = m_bNetSt;
	}
	else
		return LCNET_EFAIL;

	return LCNET_OK;
}


INT	CSelectCln::Create(void* p1, void* p2, void* p3, void* p4)
{
	TLC_SOCKET*	pSock = (TLC_SOCKET*)p1;

	if(NULL == pSock)
	{
		LcNet_Log((char*)"Err:: parameter for socket failed", __LINE__, (char*)__FILE__);
		return LCNET_EFAIL;
	}

	if(LC_FAILED(LcNet_Open()))
	{
		LcNet_Log((char*)"Err:: LcNet_Open() failed", __LINE__, (char*)__FILE__);
		return LCNET_EFAIL;
	}

	m_uPt = pSock->uPt;
	if(pSock->uIp && pSock->uIp[0])
		memcpy(m_uIp, pSock->uIp, LC_IP_LEN);


	// synchronization...
	m_scKey.Init();
	m_smSnd.Init();


	//create thread for sending
	LC_HANDLE hTh = NULL;
	hTh = LcNet_ThreadCreate((void*)CSelectCln::ThreadSend, this);
	LcNet_CloseHandle(&hTh);

	return 0;
}


INT	CSelectCln::Connect()
{
	INT hr;

	if(0 == m_uPt || NULL == m_uIp || 0 == m_uIp[0])
		return -1;

	DisConnect();


	if(LC_FAILED(LcNet_SocketCreate(&m_scHost, LCNET_TCP)))
		return LCNET_EFAIL;


	FD_ZERO(&m_fdSet);
	FD_SET(m_scHost, &m_fdSet);


	hr = LcNet_SocketOptionSet(m_scHost, LCNET_SOCK_NONBLOCKING, LC_TRUE);

	FD_ZERO(&m_fdSet);
	FD_SET(m_scHost, &m_fdSet);

	hr = LcNet_SocketConnect(m_scHost, m_uPt, m_uIp);
	if(LC_FAILED(hr))
		return LCNET_EFAIL;

	return 0;
}

INT CSelectCln::DisConnect()
{
	if(!m_scHost)
		return 0;

	memset(&m_fdSet, 0, sizeof(FD_SET));
	LcNet_SocketClose(&m_scHost);

	return 0;
}


void CSelectCln::SendBuf()
{
	m_scKey.Lock();

	INT	iLen = m_nSnd;
	INT iSnd = 0;
	INT	iTot = 0;
	INT hr   = 0;

	if(0 < iLen && 0 < m_scHost)
	{
		while(m_scHost && iTot<iLen)
		{
			char* p = m_sSnd + iTot;

			//iSnd = send(m_scHost, p, iLen-iTot, 0);
			iSnd = LcNet_SocketSend(m_scHost, p, iLen-iTot);
			if(LC_FAILED(iSnd))
			{
				hr = LcNet_LastGetErr();
				LcNet_Log((char*)"Err::CEventCln::SendBuf::", __LINE__, (char*)__FILE__);
				break;
			}

			if(0== iSnd)
			{
				LcSys_Sleep(10);
				continue;
			}

			iTot += iSnd;
		}

		m_nSnd = 0;

		if(LC_FAILED(hr))
			DisConnect();
	}

	m_scKey.Unlock();
}


INT CSelectCln::ProcSend()
{
	INT hr = 0;

	while(m_smSnd.sync_m)
	{
		hr = m_smSnd.Begin(1000);

#if defined(_MSC_VER)
		if(WAIT_TIMEOUT == hr || WAIT_OBJECT_0 != hr || 0 == m_scHost)
		{
			hr = m_smSnd.End();
			continue;
		}
#endif

		this->SendBuf();

		m_smSnd.End();
	}

	LcNet_ThreadEnd(0);
	return 0;
}



INT CSelectCln::ProcRecv()
{
	if(0 > m_bNetSt)
		return -1;

	INT		hr = 0;

	FD_SET	fdsR = m_fdSet;
	FD_SET	fdsW = m_fdSet;
	FD_SET	fdsE = m_fdSet;

	if     (0 == m_bNetSt)
		hr = LcNet_Select(m_scHost+1, &fdsR, &fdsW, &fdsE, 0);
	else if(1 == m_bNetSt)
		hr = LcNet_Select(m_scHost+1, &fdsR, 0, 0, 0);
	else
		return -1;

	if(LC_FAILED(hr))
	{
		printf("select error\n");
		return -1;
	}

	if(0 == hr)		// time out
		return 0;


	if     (0 == m_bNetSt && FD_ISSET(m_scHost, &fdsE))
	{
		printf("FDE::connection failed\n");
		return -1;
	}
	else if(0 == m_bNetSt && FD_ISSET(m_scHost, &fdsW))
	{
		m_bNetSt = 1;
		printf("FDW::connected\n");
		return 0;
	}

	if(FD_ISSET(m_scHost, &fdsR))
	{
		if(0 == m_bNetSt)
			m_bNetSt = 1;

		INT		iRcv=0;
		char	sRcv[LC_PCK_RECV+4]={0};

		iRcv = recv(m_scHost, sRcv, LC_PCK_RECV, 0);


		if(0> iRcv)
		{
			char smsg[512]={0};
			INT er =LcNet_LastGetErr();

			if(0>LcNet_IsNotErr(er))
			{
				LcNet_ErrFormat(smsg, er);
				printf("FDR::connection Error\n");
				return -1;
			}

			return 0;
		}
		else if(0==iRcv)
		{
			printf("Gracefully Disconnected\n");
			return 0;
		}

		// print the received message
		printf("Recv: %s\n", sRcv);
	}

	return 0;
}


DWORD LC_WINAPI CSelectCln::ThreadSend(void *pParam)
{
	CSelectCln* pNet = (CSelectCln*)pParam;
	return pNet->ProcSend();
}

