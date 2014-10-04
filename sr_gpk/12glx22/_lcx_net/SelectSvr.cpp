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

#include "../LcxNetwork.h"
#include "SelectSvr.h"


CSelectSvr::RemoteHost::RemoteHost()
{
	scH		= 0;
	nSnd	= 0;
	memset(sSnd, 0, LC_PCK_SEND+4);
}

CSelectSvr::RemoteHost::RemoteHost(LC_SOCKET s)
{
	scH		= s;
	nSnd	= 0;
	memset(sSnd, 0, LC_PCK_SEND+4);
}

CSelectSvr::RemoteHost::~RemoteHost()
{
	Close();
}


void CSelectSvr::RemoteHost::Close()
{
	if(0 == scH)
		return;

	LcNet_SocketClose(&scH);

	nSnd	= 0;
	memset(sSnd, 0, LC_PCK_SEND+4);
}



void CSelectSvr::RemoteHost::SetupBuf(char* s, INT len)
{
	nSnd = len;
	memcpy(sSnd, s, nSnd);
	memset(sSnd+nSnd, 0, LC_PCK_SEND-len);
}



void* CSelectSvr::FindHost(LC_SOCKET scH)
{
	if(0 == scH)
		return NULL;

	INT iSize = (INT)m_vHost.size();

	for(INT i=0; i<iSize; ++i)
	{
		if(scH == m_vHost[i]->scH)
			return m_vHost[i];
	}

	return NULL;
}

void CSelectSvr::DeleteHost(LC_SOCKET scH)
{
	INT iSize = (INT)m_vHost.size();

	for(INT i=0; i<iSize; ++i)
	{
		if(scH == m_vHost[i]->scH)
		{
			delete m_vHost[i];
			m_vHost.erase( m_vHost.begin() + i);
			return;
		}
	}

}

void CSelectSvr::DeleteNotUseHost()
{
	m_scKey.Lock();
	std::vector<RemoteHost* >::iterator _F = m_vHost.begin();

	for( ; _F != m_vHost.end(); )
	{
		RemoteHost* p = (*_F);

		if(p && 0 >= p->scH)
		{
			delete p;
			_F = m_vHost.erase(_F);
			continue;
		}

		++_F;
	}

	m_scKey.Unlock();
}


void CSelectSvr::DeleteAllHost()
{
	INT iSize = (INT)m_vHost.size();

	for(INT i=0; i<iSize; ++i)
		delete m_vHost[i];

	m_vHost.clear();
}




////////////////////////////////////////////////////////////////////////////////

CSelectSvr::CSelectSvr()
{
	m_bNetSt	= LCNET_OK;
	m_scHost	= 0;
	memset(&m_fdSet, 0, sizeof(FD_SET));

	m_uPt		= 0;
	memset(m_uIp, 0, sizeof m_uIp);
}


CSelectSvr::~CSelectSvr()
{
	Destroy();
}


void CSelectSvr::Destroy()
{
	DeleteAllHost();

	m_smSnd.Close();
	m_scKey.Close();

	LcNet_SocketClose(&m_scHost);
	LcNet_Close();
}


INT CSelectSvr::Query(char* sCmd, LC_HANDLE)
{
	if(     0 == LcStr_Stricmp("Update", sCmd))
	{
		return this->ProcRecv();
	}
	else
		return LCNET_EFAIL;


	return LCNET_OK;
}

INT CSelectSvr::Release()
{
	return LCNET_OK;
}

INT CSelectSvr::SetAttrib(UINT dCmd, LC_HANDLE v)
{
	return LCNET_OK;
}

INT CSelectSvr::GetAttrib(UINT dCmd, LC_HANDLE v)
{
	if(LCNET_STATE == dCmd)
	{
		*((INT*)v) = m_bNetSt;
	}
	else
		return LCNET_EFAIL;

	return LCNET_OK;
}

INT CSelectSvr::Send(LC_SOCKET  sc, TLC_PACKET*)
{
	return LCNET_OK;
}

INT CSelectSvr::Recv(LC_SOCKET* sc, TLC_PACKET*)
{
	return LCNET_OK;
}

INT CSelectSvr::RecvCount()
{
	return LCNET_OK;
}


INT CSelectSvr::Create(void* p1, void* p2, void* p3, void* p4)
{
	INT			hr = LCNET_EFAIL;
	TLC_SOCKET*	pSock = (TLC_SOCKET*)p1;

	printf("Starting Server.\nPort: %d\n", (INT)pSock->uPt);

	if(LC_FAILED(LcNet_Open()))
		return LCNET_EFAIL;


	m_uPt = pSock->uPt;
	if(pSock->uIp && pSock->uIp[0])
		memcpy(m_uIp, pSock->uIp, LC_IP_LEN);


	if(LC_FAILED(LcNet_SocketCreate(&m_scHost, LCNET_TCP)))
		return LCNET_EFAIL;



	hr = LcNet_SocketBind(m_scHost, m_uPt, m_uIp);
	if(LC_FAILED(hr))
		return LCNET_EFAIL;


	// synchronization...
	m_scKey.Init();
	m_smSnd.Init();


	FD_ZERO(&m_fdSet);
	// increase m_fdSet.fd_count
	FD_SET(m_scHost, &m_fdSet);
	m_fdMax = m_scHost + 1;

	// listen
	hr = LcNet_SocketListen(m_scHost);
	if(LC_FAILED(hr))
		return LCNET_EFAIL;


	//create thread for sending
	LC_HANDLE hTh = NULL;
	hTh = LcNet_ThreadCreate((void*)CSelectSvr::ThreadSend, this);
	LcNet_CloseHandle(&hTh);

	return LCNET_OK;
}



void CSelectSvr::EchoMsg(char* s, INT iLen)
{
	m_scKey.Lock();

		INT iSize = (INT)m_vHost.size();

		for(INT i=0; i<iSize; ++i)
		{
			RemoteHost* pCln = m_vHost[i];
			if(0 >= pCln->scH)
				continue;

			pCln->SetupBuf(s, iLen);
		}

	m_scKey.Unlock();

	m_smSnd.SetSignal();
}


void CSelectSvr::SendBuf()
{
	m_scKey.Lock();

	INT iSize = (INT)m_vHost.size();

	for(INT i=0; i<iSize; ++i)
	{
		RemoteHost* pCln = m_vHost[i];
		if(0 >= pCln->scH)
			continue;


		LC_SOCKET scH = pCln->scH;
		INT	iLen = pCln->nSnd;
		INT iSnd = 0;
		INT	iTot = 0;

		if(0 == iLen)
			continue;


		while(iTot<iLen)
		{
			char* p = pCln->sSnd + iTot;

			//iSnd = send(pCln->scH, p, iLen-iTot, 0);
			iSnd = LcNet_SocketSend(scH, p, iLen-iTot);
			if(LC_FAILED(iSnd))
			{
				// Àü¼Û error.
				LcNet_Log((char*)"Err::CSelectSvr::ProcSend::", __LINE__, (char*)__FILE__);
				break;
			}

			if(0== iSnd)
			{
				LcSys_Sleep(1);
				continue;
			}

			iTot += iSnd;
		}

		pCln->nSnd = 0;
	}

	// remove send error socketlist
	DeleteNotUseHost();

	m_scKey.Unlock();
}




INT CSelectSvr::ProcSend()
{
	INT hr = 0;

	while(m_smSnd.sync_m)
	{
		if(m_vHost.empty())
			continue;

		hr = m_smSnd.Begin(1);

#if defined(_MSC_VER)
		if(WAIT_TIMEOUT == hr || WAIT_OBJECT_0 != hr)
		{
			hr = m_smSnd.End();
			continue;
		}
#endif

		this->SendBuf();

		m_smSnd.End();
	}

	LcNet_ThreadEnd();
	return LCNET_OK;
}



INT CSelectSvr::ProcRecv()
{
	RemoteHost*	pHost = NULL;
	INT			hr = 0;
	INT			i  = 0;

	FD_SET		fdsR = m_fdSet;

	LC_SOCKET	scHost = 0;
	LC_SOCKET	scCln  = 0;

	if(!m_scHost)
		return -1;

#if defined(_MSC_VER)
	hr = LcNet_Select(m_scHost+1, &fdsR, 0, 0, 0);
#else
	hr = LcNet_Select(m_fdMax+1, &fdsR, 0, 0, 0);
#endif

	if(LC_FAILED(hr))
	{
		printf("select error\n");
		return -1;
	}

	if(0 == hr)		// time out
		return 0;


#if defined(_MSC_VER)
	for(UINT i=0; i<m_fdSet.fd_count; ++i)
#else
	for(INT i = m_scHost; i<=m_fdMax; ++i)
#endif
	{


#if defined(_MSC_VER)
		scHost = m_fdSet.fd_array[i];
#else
		scHost = i;
#endif

		// find the event on fdset
		if(!FD_ISSET(scHost, &fdsR))
			continue;


		// from listen socket
		if(m_scHost == scHost)
		{
			char		sIp[64]= {0};

			hr = LcNet_SocketAccept(&scCln, m_scHost);
			if(LC_FAILED(hr))
				break;

			if(0 == scCln)
				continue;


			LcNet_IpAddress(sIp, scCln);
			printf("New Client: %5d %s\n", (INT)scCln, sIp);

			hr  = 0;
			hr  = LcNet_SocketOptionSet(scCln, LCNET_SOCK_NAGLE_OFF, LC_TRUE);
			hr |= LcNet_SocketOptionSet(scCln, LCNET_SOCK_REUSEADDR, LC_TRUE);
			hr |= LcNet_SocketOptionSet(scCln, LCNET_SOCK_DONTLINGER, LC_TRUE);
			hr |= LcNet_SocketOptionSet(scCln, LCNET_SOCK_LINGER, LC_TRUE);
			if(hr)
				LcNet_Log((char*)"Sock option failed", __LINE__, (char*)__FILE__);


			m_vHost.push_back(new RemoteHost(scCln));

			// increase the m_fdSet.fd_count
			FD_SET(scCln, &m_fdSet);
			printf("New Client: %d, Number: %d\n", scCln, (int)(m_vHost.size()) );

			continue;
		}


		pHost = (RemoteHost*)FindHost(scHost);
		if(NULL == pHost)
			continue;


		// other socket
		INT		iRcv = 0;
		char	sRcv[LC_PCK_RECV+4]={0};

		iRcv = LcNet_SocketRecv(scHost, sRcv, LC_PCK_RECV);
		if(LC_FAILED(iRcv))
		{
			printf("Disconnect Client: %d\n", (INT)scHost);
			pHost->Close();

			// decrease the m_fdSet.fd_count
			FD_CLR(scHost, &m_fdSet);
			printf("Client Number: %d \n", (int)m_vHost.size() -1);
			continue;
		}



		// Receive data
		printf("Recv from Client : %d %s\n", scHost, sRcv);

		// test message
		char	sSndBuf[LC_PCK_SEND]={0};
		sprintf(sSndBuf, "%5d : %s", scHost, sRcv);

		// send the message to all client
		EchoMsg(sSndBuf, strlen(sSndBuf) );
	}

	DeleteNotUseHost();


	// Make the max fd
	int iSize = (int)m_vHost.size();

	m_fdMax = m_scHost;

	for(i = 0; i<iSize; ++i)
	{
		scCln = m_vHost[i]->scH;
		if(scCln  > (LC_SOCKET)m_fdMax)
			m_fdMax = scCln;
	}

	return LCNET_OK;
}



DWORD LC_WINAPI CSelectSvr::ThreadSend(void *pParam)
{
	CSelectSvr* pNet = (CSelectSvr*)pParam;
	return pNet->ProcSend();
}

