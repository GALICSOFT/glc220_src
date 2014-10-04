// Implementation of the ILCX_Network.
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


INT LcNet_Open()
{

#if defined(_MSC_VER)
	INT hr;
	WORD	wVersion;
	WSADATA wsaData;

	DWORD	iBuf =0;
	INT		nTCP[2] = {IPPROTO_TCP, IPPROTO_UDP};

	wVersion = MAKEWORD( 2, 2 );

	if ( 0 != WSAStartup( wVersion, &wsaData ))
		return LCNET_EFAIL;

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
	{
		WSACleanup();
		return LCNET_EFAIL;
	}

	// 버퍼의 크기만 반환하고 에러를 발생해야 된다.
	hr = WSAEnumProtocols(0, 0, &iBuf);

	if( (LCNET_EFAIL != hr) && (WSAENOBUFS != LcNet_LastGetErr()) )
	{
		WSACleanup();
		return LCNET_EFAIL;
	}

	LPWSAPROTOCOL_INFO	pProtoInfo = (LPWSAPROTOCOL_INFO)malloc(iBuf);

	hr = WSAEnumProtocols(nTCP, pProtoInfo, &iBuf);
	free(pProtoInfo);

	if(LCNET_EFAIL ==hr)
	{
		WSACleanup();
		return LCNET_EFAIL;
	}

#else
	// 1. USB에 연결 되어 있는지 확인
	// 2. IP 확인


#endif

	return LCNET_OK;
}

INT	LcNet_Close()
{
#if defined(_MSC_VER)
	WSACleanup();
#endif


	return LCNET_OK;
}



INT	LcNet_SocketCreate(LC_SOCKET* pScH, INT type)
{
	INT		hr  = 0;
	SOCKET	scH = 0;

	if(LCNET_TCP == type)
		scH = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	else if(LCNET_UDP == type)
		scH = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	else
		return LCNET_EFAIL;


	if(INVALID_SOCKET == scH)
		return LCNET_EFAIL;

	hr  = 0;
	hr |= LcNet_SocketOptionSet(scH, LCNET_SOCK_NAGLE_OFF, LC_TRUE);
	hr |= LcNet_SocketOptionSet(scH, LCNET_SOCK_REUSEADDR, LC_TRUE);
	hr |= LcNet_SocketOptionSet(scH, LCNET_SOCK_DONTLINGER, LC_TRUE);
	hr |= LcNet_SocketOptionSet(scH, LCNET_SOCK_LINGER, LC_TRUE);
	if(hr)
		LcNet_Log((char*)"Sock option failed", __LINE__, (char*)__FILE__);

	*pScH = scH;

	return LCNET_OK;
}

INT	LcNet_SocketClose(LC_SOCKET* scH)
{
	if(LCNET_INVALID_SOCKET == *scH || 0 == *scH)
		return LCNET_OK;

	shutdown(*scH, SD_BOTH);
	closesocket(*scH);

	*scH = 0;

	return LCNET_OK;
}


INT	LcNet_SocketConnect(LC_SOCKET scH, USHORT uPort, BYTE* uIp)
{
	INT	hr = LCNET_EFAIL;
	SOCKADDR_IN	sdH = {0};

	if(0 == uPort || NULL == uIp || 0 == uIp[0])
		return LCNET_EFAIL;

	memcpy(&sdH.sin_addr, uIp, 4);
	sdH.sin_family      = AF_INET;
	sdH.sin_port        = htons(uPort);

	hr = connect(scH, (SOCKADDR*)&sdH, sizeof(SOCKADDR_IN));
	if(SOCKET_ERROR == hr)
	{
		hr = LcNet_LastGetErr();
		if(LC_FAILED(LcNet_IsNotErr(hr) ))
		{
			LcNet_Log((char*)"connection error");
			return LCNET_EFAIL;
		}
	}

	return LCNET_OK;
}

INT	LcNet_SocketBind(LC_SOCKET scH, USHORT uPort, BYTE* uIp)
{
	INT	hr = LCNET_EFAIL;
	SOCKADDR_IN	sdH = {0};

	if(0 == uPort)
		return LCNET_EFAIL;

	if(uIp && uIp[0])
		memcpy(&sdH.sin_addr, uIp, 4);

	sdH.sin_family      = AF_INET;
	sdH.sin_port        = htons(uPort);

	hr = bind(scH, (SOCKADDR*)&sdH, sizeof(SOCKADDR_IN));
	if(SOCKET_ERROR == hr)
	{
		hr = LcNet_LastGetErr();
		if(LC_FAILED(LcNet_IsNotErr(hr) ))
		{
			LcNet_Log((char*)"address binding error");
			return LCNET_EFAIL;
		}
	}

	return LCNET_OK;
}

INT	LcNet_SocketListen(LC_SOCKET scH)
{
	INT hr= LCNET_EFAIL;

	hr = listen(scH, SOMAXCONN);
	if(SOCKET_ERROR == hr)
	{
		hr = LcNet_LastGetErr();
		if(LC_FAILED(LcNet_IsNotErr(hr) ))
		{
			LcNet_Log((char*)"listen error");
			return LCNET_EFAIL;
		}
	}

	return LCNET_OK;
}



INT	LcNet_SocketAccept(LC_SOCKET* pScH, LC_SOCKET scListen)
{
	INT		hr = LCNET_EFAIL;
	SOCKET	scCln = 0;

	scCln = accept((LC_SOCKET)scListen, NULL, NULL);
	if(INVALID_SOCKET ==scCln)
	{
		hr = LcNet_LastGetErr();
		if(LC_FAILED(LcNet_IsNotErr(hr) ))
		{
			LcNet_Log((char*)"listen error");
			return LCNET_EFAIL;
		}

		*pScH = 0;
		return LCNET_OK;
	}

	*pScH = scCln;
	return scCln;
}



INT LcNet_SocketSend(LC_SOCKET s, const char* b, INT l)
{
	INT hr= LCNET_OK;

	hr = send(s, b, l, 0);
	if(SOCKET_ERROR == hr)
	{
		hr = LcNet_LastGetErr();
		if(LC_FAILED(LcNet_IsNotErr(hr) ))
		{
			LcNet_Log((char*)"send error");
			return LCNET_EFAIL;
		}

		return LCNET_OK;
	}
	else if( 0 == hr)					// send 0 byte
		return LCNET_EFAIL;

	return hr;
}

INT	LcNet_SocketRecv(LC_SOCKET s, char* b, INT l)
{
	INT hr= LCNET_EFAIL;

	hr = recv(s, b, l, 0);
	if(SOCKET_ERROR == hr)
	{
		hr = LcNet_LastGetErr();
		if(LC_FAILED(LcNet_IsNotErr(hr) ))
		{
			LcNet_Log((char*)"recv error");
			return LCNET_EFAIL;
		}

		return LCNET_OK;
	}
	else if( 0 == hr)					// recv zero byte
	{
		LcNet_Log((char*)"Gracefully Disconnected");
		return LCNET_EFAIL;
	}

	return hr;
}



INT	LcNet_SocketOptionSet(LC_SOCKET scH, INT opt, INT v)
{
	INT hr = LCNET_OK;

	if     (LCNET_SOCK_NONBLOCKING == opt)
	{
#if defined(_MSC_VER)
		hr = ioctlsocket(scH, FIONBIO, (u_long*)&v);
#else
		INT f;
		if((f = fcntl(scH, F_GETFL,0)) == -1)
		{
			perror("fnctl error");
			f = 0;
		}

		fcntl(scH, F_SETFL, f | O_NONBLOCK);
#endif
	}

	else if(LCNET_SOCK_NAGLE_OFF == opt)
		hr = setsockopt(scH, IPPROTO_TCP, TCP_NODELAY, (const char*)&v, sizeof v);

	else if(LCNET_SOCK_REUSEADDR  == opt)
		hr = setsockopt(scH, SOL_SOCKET, SO_REUSEADDR, (const char*)&v, sizeof v);

	else if(LCNET_SOCK_LINGER     == opt)
	{
		LINGER t_v= {v,0};
		hr = setsockopt(scH, SOL_SOCKET, SO_LINGER, (const char*)&t_v, sizeof t_v);
	}

	else if(LCNET_SOCK_DONTLINGER == (UINT)opt)
	{
#if defined(_MSC_VER)
		hr = setsockopt(scH, SOL_SOCKET, SO_DONTLINGER, (const char*)&v, sizeof v);
#else
		hr = 0;
#endif
	}

	else if(LCNET_SOCK_SNDTIMEO == opt)
	{
		timeval t_v={ 0, v*1000};
		hr = setsockopt(scH, SOL_SOCKET, SO_SNDTIMEO, (const char*)&t_v, sizeof t_v);
	}

	else if(LCNET_SOCK_RCVTIMEO == opt)
	{
		timeval t_v={ 0, v*1000};
		hr = setsockopt(scH, SOL_SOCKET, SO_RCVTIMEO, (const char*)&t_v, sizeof t_v);
	}

	else if(LCNET_SOCK_SNDBUF == opt)
		hr = setsockopt(scH, SOL_SOCKET, SO_SNDBUF, (const char*)&v, sizeof v);

	else if(LCNET_SOCK_RCVBUF == opt)
		hr = setsockopt(scH, SOL_SOCKET, SO_RCVBUF, (const char*)&v, sizeof v);

	else
		hr = LCNET_EFAIL;


	if(SOCKET_ERROR == hr || LCNET_EFAIL == hr)
		return LCNET_EFAIL;


	return LCNET_OK;
}


INT LcNet_IpAddress(char* s, LC_SOCKET scH)
{
	SOCKADDR_IN sdH ={0};
	INT size = sizeof(SOCKADDR_IN);

	getpeername(scH, (SOCKADDR *)&sdH, (socklen_t*)&size);
	strcpy(s, inet_ntoa(sdH.sin_addr) );

	return LCNET_OK;
}


INT LcNet_LocalIpAddress(char** sIpList)
{
	in_addr		in_add={0};
	HOSTENT*	pHost=0;
	char		sHost[260]={0};

	char*		sIp = NULL;

	INT			i = 0, nIp = 0;
	INT			w = 20;

	if(gethostname(sHost, 260))
		return LCNET_EFAIL;

	pHost = gethostbyname(sHost);
	if(!pHost)
		return LCNET_EFAIL;

	for(i=0; pHost->h_addr_list[i]; ++i)
		++nIp;

	if(0 == nIp)
		return LCNET_EFAIL;


	sIp = (char*)malloc(nIp* w);
	memset(sIp, 0, nIp* w);

	for(i=0; pHost->h_addr_list[i]; ++i)
	{
		memcpy(&in_add, pHost->h_addr_list[i], pHost->h_length);
		strcpy(sIp + i * w, inet_ntoa(in_add));
	}

	*sIpList = sIp;
	return nIp;
}



void LcNet_InetAddr32(BYTE* uIp, char* sIp)
{
	unsigned long ip = (unsigned long)inet_addr(sIp);
	memcpy(uIp, &ip, sizeof(unsigned long));
}



INT	LcNet_LastGetErr()
{
	INT er =0;
#if defined(_MSC_VER)
	er = WSAGetLastError();
#else
	er  = errno	;
#endif

	return er;
}


void LcNet_ErrFormat(char* s, INT er)
{
#if defined(_MSC_VER)
	char* buf;
	FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
				| FORMAT_MESSAGE_IGNORE_INSERTS
				, NULL, er, 0, (LPSTR)&buf, 0, NULL );

	sprintf(s, buf);
	LocalFree( buf );
#else
	sprintf(s, strerror(er));
#endif
}


#if defined(_MSC_VER)
  #define EWOULDBLOCK		WSAEWOULDBLOCK
  #define EIO_PENDING		WSA_IO_PENDING
  #define EISCONN			WSAEISCONN
  #define EINPROGRESS		WSAEINPROGRESS
#else
  #define EIO_PENDING		EINPROGRESS
#endif


INT	LcNet_IsNotErr(INT er)
{
	if(EWOULDBLOCK == er || EIO_PENDING ==er || EISCONN ==er || EINPROGRESS ==er)
		return LCNET_OK;

	return LCNET_EFAIL;
}


void LcNet_Log(char* msg, INT line, char* file)
{
	if(msg)		printf("%s ::", msg);
	if(line)	printf("%4d ::", line);
	if(file)	printf("%s", file);

	printf("\n");
}

void LcNet_Log(INT line, char* file, INT er, char* msg)
{
	if(msg)
		printf("%s ::", msg);

	if(er)
	{
		char str_err[1024]={0};
		LcNet_ErrFormat(str_err, er);
		printf("%s ::", str_err);
	}

	if(line)	printf("%4d ::", line);
	if(file)	printf("%s", file);

	printf("\n");
}


INT	LcNet_ProcessorCount()
{
	INT hr = 1;
#if defined(_MSC_VER)
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	hr = (INT)SystemInfo.dwNumberOfProcessors;
#endif
	return hr;
}


INT	LcNet_Select(INT nFdCnt, void* pfdR, void* pfdW, void* pfdE, INT wait)
{
	INT hr= LC_OK;

	FD_SET* fR = (FD_SET*)pfdR;
	FD_SET* fW = (FD_SET*)pfdW;
	FD_SET* fE = (FD_SET*)pfdE;

	timeval  wTime={0, wait*1000};
	timeval* pwTime = NULL;

	if(0 <= wait)
		pwTime = &wTime;


	hr = select(nFdCnt, fR, fW, fE, pwTime);
	if(SOCKET_ERROR == hr)
	{
		hr = LcNet_LastGetErr();
		if(LC_FAILED(LcNet_IsNotErr(hr) ))
		{
			LcNet_Log((char*)"select error");
			return LCNET_EFAIL;
		}

		hr = LCNET_OK;
	}

	return hr;
}



LC_HANDLE LcNet_ThreadCreate(void* pFunc, void* pParam
							, INT bSuspend, void* pId)
{
#if defined(_MSC_VER)
	unsigned (LC_WINAPI* pThread) (void *);
	unsigned _InitFlag = bSuspend? CREATE_SUSPENDED:0;

	pThread = (unsigned (__stdcall* ) (void *))pFunc;

	return (LC_HANDLE)_beginthreadex(NULL, 0, pThread, pParam
									, _InitFlag, (unsigned int*)pId);
#else
	INT hr =0;
	pthread_t hTh=0;
	hr = pthread_create(&hTh, NULL, (void* (*)(void*))pFunc, pParam);
	if(0 != hr)
		return NULL;

	return (LC_HANDLE)hTh;
#endif

	return NULL;
}


void LcNet_ThreadEnd(UINT v)
{
#if defined(_MSC_VER)
	_endthreadex(v);
#else

#endif
}


INT LcNet_ThreadClose(LC_HANDLE* v)
{
	INT hr = 0;

	if(NULL == v || NULL == *v)
		return hr;

#if defined(_MSC_VER)
	::CloseHandle(*v);
#else
	pthread_t hTh=(pthread_t)(*v);
	hr = pthread_detach(hTh);
#endif

	*v= NULL;
	return hr;
}

void LcNet_CloseHandle(LC_HANDLE* v)
{
	if(NULL == v || NULL == *v)
		return;

#if defined(_MSC_VER)
	::CloseHandle(*v);
#endif

	*v=NULL;
}


void LcNet_Sleep(INT millisecond)
{
#if defined(_MSC_VER)
	Sleep(10);
#else
	usleep(10 * 1000);
#endif
}


////////////////////////////////////////////////////////////////////////////////


TLC_PACKET::TLC_PACKET()
{
	Reset();
}

TLC_PACKET::TLC_PACKET(const char* src)
{
	INT _l = 0;
	memcpy(pck, src, LC_PCK_HEAD);

	_l = this->len - LC_PCK_HEAD;
	memcpy(sgm, src, _l);
}


void TLC_PACKET::Reset()
{
	bsp = 0xCCCC;
	pvj	= 0x12;
	pvn = 0x04;
	crp = 0;
	len = 0;
	pro = 0;
	sqc = 0;

	len = LC_PCK_HEAD;
}



INT TLC_PACKET::GetType()		{	return this->pro;	}
INT TLC_PACKET::GetLength()		{	return (this->len);	}
INT TLC_PACKET::GetLengthData()	{	return (this->len - LC_PCK_HEAD);	}
char* TLC_PACKET::GetPack()		{	return pck;	}
char* TLC_PACKET::GetData()		{	return sgm;	}



void TLC_PACKET::WriteBegin(INT type, INT sqc)
{
	Reset();
	this->pro = type;
	this->sqc = sqc;
}

void TLC_PACKET::WriteEnd(INT crypto)
{
	// need encryption
}

void TLC_PACKET::AddData(void* data, INT len)
{
	memcpy(pck + this->len, data, len);
	this->len += len;
}


void TLC_PACKET::ReadAll(void* dst)
{
	INT _l = this->len - LC_PCK_HEAD;
	memcpy(dst, sgm, _l);
}

INT TLC_PACKET::ReadData(void* dst, INT size, INT offset)
{
	memcpy(dst, sgm+offset, size);
	return offset + size;
}


////////////////////////////////////////////////////////////////////////////////

TLC_EPOLL::TLC_EPOLL()
{
	eh = 0;
	ev = 0;
	en = 0;
}

TLC_EPOLL::~TLC_EPOLL()
{
	Close();
}

INT TLC_EPOLL::Create(INT cnt)
{
#if defined(_MSC_VER)
	return -1;

#else
	en = cnt;
	eh = epoll_create(en);
	if(0>= eh)
		return -1;

	EP_EVENT* _ep = new EP_EVENT[en];
	ev = (LC_HANDLE)_ep;
#endif

	return 0;
}

void TLC_EPOLL::Close()
{
#if defined(_MSC_VER)
	return;
#else
	if(0 == en)
		return;

	en = 0;

	if(ev)
	{
		EP_EVENT* _ep = (EP_EVENT*)ev;
		delete [] _ep;
	}

	ev	= NULL;

	close(eh);
#endif
}


INT TLC_EPOLL::Ctrl(INT fd, INT opt, INT io_t)
{
	INT hr = LC_EFAIL;
#if defined(_MSC_VER)
	return hr;
#else
	if(EPOLL_CTL_ADD == opt)
	{
		EP_EVENT t_ep ={0};
		t_ep.events  = EPOLLIN;
		t_ep.data.fd = fd;
		hr = epoll_ctl(eh, opt, fd, &t_ep);
	}
	else if(EPOLL_CTL_DEL == opt)
	{
		hr = epoll_ctl(eh, opt, fd, NULL);
	}
#endif
	return hr;
}

INT TLC_EPOLL::Wait(DWORD waitTime)
{
	INT hr = LC_EFAIL;
#if !defined(_MSC_VER)
	hr = epoll_wait(eh, (EP_EVENT*)ev, en, waitTime);
#endif
	return hr;
}

INT TLC_EPOLL::GetFd(INT n)
{
	INT fd = -1;
	if(0>n || n>=en)
		return fd;
#if !defined(_MSC_VER)
	EP_EVENT* _ep = (EP_EVENT*)ev;
	fd = _ep[n].data.fd;
#endif
	return fd;
}


////////////////////////////////////////////////////////////////////////////////


// Synchronization for CRITICAL SECTION
TLC_SYNC_CS::TLC_SYNC_CS()
{
	sync_m = NULL;
}


TLC_SYNC_CS::~TLC_SYNC_CS()
{
	Close();
}


INT TLC_SYNC_CS::Init()
{
#if defined(_MSC_VER)
	if(sync_m)
		return 0;

	sync_m = (LC_HANDLE)(new CRITICAL_SECTION);
	InitializeCriticalSection((CRITICAL_SECTION*)sync_m);
#else

	sync_m = (LC_HANDLE)(new pthread_mutex_t);
	pthread_mutex_init((pthread_mutex_t*)sync_m, NULL);
#endif

	return 0;
}

INT TLC_SYNC_CS::Close()
{
	INT hr = 0;
	if(NULL == sync_m)
		return 0;

#if defined(_MSC_VER)
	CRITICAL_SECTION* _sync_m = (CRITICAL_SECTION*)sync_m;
	DeleteCriticalSection(_sync_m);
	delete _sync_m;
#else
	hr = pthread_mutex_destroy((pthread_mutex_t*)sync_m);

	if(sync_m)
	{
		pthread_mutex_t* _sync_m = (pthread_mutex_t*)sync_m;
		delete _sync_m;
	}
#endif

	sync_m = NULL;
	return hr;
}

INT TLC_SYNC_CS::Lock()
{
#if defined(_MSC_VER)
	EnterCriticalSection((CRITICAL_SECTION*)sync_m);
#else
	pthread_mutex_lock((pthread_mutex_t*)sync_m);
#endif
	return 0;
}

INT TLC_SYNC_CS::Unlock()
{
#if defined(_MSC_VER)
	LeaveCriticalSection((CRITICAL_SECTION*)sync_m);
#else
	pthread_mutex_unlock((pthread_mutex_t*)sync_m);
#endif
	return 0;
}


////////////////////////////////////////////////////////////////////////////////

TLC_SYNC_MUTEX::TLC_SYNC_MUTEX()
{
	sync_m = NULL;
	sync_d = NULL;
}

TLC_SYNC_MUTEX::~TLC_SYNC_MUTEX()
{
	Close();
}


INT TLC_SYNC_MUTEX::Init()
{
	INT hr = 0;
#if defined(_MSC_VER)
	if(sync_m)
		return 0;

	sync_m = CreateMutex(NULL, FALSE, NULL);
#else
	sync_m = (LC_HANDLE)(new pthread_mutex_t);
	sync_d = (LC_HANDLE)(new pthread_cond_t);

	hr = pthread_mutex_init((pthread_mutex_t*)sync_m, NULL);
	hr = pthread_cond_init((pthread_cond_t*)sync_d, NULL);
#endif
	return hr;
}


INT TLC_SYNC_MUTEX::Close()
{
	INT hr = 0;
	if(NULL == sync_m)
		return 0;

#if defined(_MSC_VER)
	hr = CloseHandle(sync_m);
#else
	hr = pthread_mutex_destroy((pthread_mutex_t*)sync_m);
	hr = pthread_cond_destroy((pthread_cond_t*)sync_d);

	if(sync_m)
	{
		pthread_mutex_t* _sync_m = (pthread_mutex_t*)sync_m;
		delete _sync_m;
	}

	if(sync_d)
	{
		pthread_cond_t* _sysnc_d = (pthread_cond_t*)sync_d;
		delete _sysnc_d;
	}
#endif

	sync_m = NULL;
	sync_d = NULL;
	return hr;
}


INT TLC_SYNC_MUTEX::Begin(INT wait)
{
	INT hr=-1;

	if(0 > wait)
		wait = -1;

#if defined(_MSC_VER)
	hr = WaitForSingleObject(sync_m, wait);
#else

	hr = pthread_mutex_lock((pthread_mutex_t*)sync_m);

	if(0 > wait)
		hr = pthread_cond_wait((pthread_cond_t*)sync_d, (pthread_mutex_t*)sync_m);
	else
	{
		timespec abstime;
		timeval now;
		gettimeofday(&now, NULL);

		// setup the microsecond = current micro time + wait * 1000;
		unsigned long usec = (now.tv_usec + wait*1000);

		// setup the second = current second + micro/1,000,000
		abstime.tv_sec  = now.tv_sec + (unsigned long)(usec/1000000);

		// setup the micro second = micro time % 1,000,000
		abstime.tv_nsec = (usec%1000000)*1000;

		hr = pthread_cond_timedwait((pthread_cond_t*)sync_d, (pthread_mutex_t*)sync_m, &abstime);
	}
#endif

	return hr;
}


INT TLC_SYNC_MUTEX::End()
{
	INT hr = -1;
#if defined(_MSC_VER)
	hr = ::ReleaseMutex(sync_m);
#else
	hr = pthread_mutex_unlock((pthread_mutex_t*)sync_m);
#endif

	return hr;
}


INT TLC_SYNC_MUTEX::SetSignal()
{
	INT hr = -1;
#if defined(_MSC_VER)
	hr = ::ReleaseMutex(sync_m);
#else
	hr = pthread_cond_signal((pthread_cond_t*)sync_d);
#endif

	return hr;
}


