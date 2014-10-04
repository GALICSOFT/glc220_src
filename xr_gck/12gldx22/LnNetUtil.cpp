// Implementation of the LcNetUtil functions.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LnUtil.h>

#include "LnxNet.h"



void LcNet_FormatMessage(char* sMsg, DWORD hr)
{
	LPVOID	pBuf=NULL;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		0,
		(LPTSTR)&pBuf,
		0,
		NULL 
	);

	if(pBuf)
	{
		strcpy(sMsg, (char*)pBuf);
		LocalFree( pBuf );
	}
}


void LcNet_FormatMessage(char* sMsg)
{
	LPVOID	pBuf=NULL;
	DWORD	hr= GetLastError();

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		0,
		(LPTSTR)&pBuf,
		0,
		NULL 
	);

	if(pBuf)
	{
		strcpy(sMsg, (char*)pBuf);
		LocalFree( pBuf );
	}
}



void LcNet_ErrorMessage(DWORD hr)
{
	char	sMsg[1024] ={0};

	LcNet_FormatMessage(sMsg);
	printf("Error: %s\n", sMsg);
}



INT LcNet_WSAGetError()
{
	INT		hr = 0;
	char	sMsg[512]={0};

	hr = WSAGetLastError();

	//에러가 아닌 경우다.
	hr = (WSA_IO_PENDING ==hr || WSAEWOULDBLOCK==hr || WSAEISCONN==hr) ? 1 : 0;

	if(hr)
		return 0;

	LcNet_FormatMessage(sMsg);
	printf("Error Line: %s\n", sMsg);

	return -1;
}



INT LcNet_WSAStartup()
{
	INT		hr;
	WORD	wVersion;
	WSADATA wsaData;

	DWORD	iBuf =0;
	INT		nTCP[2] = {IPPROTO_TCP, IPPROTO_UDP};
 
	wVersion = MAKEWORD( 2, 2 );
 
	if ( 0 != WSAStartup( wVersion, &wsaData ))
		return -1;
 
	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
	{
		WSACleanup();
		return -1; 
	}

	// 버퍼의 크기만 반환하고 에러를 발생해야 된다.
	hr = WSAEnumProtocols(0, 0, &iBuf);

	if( (SOCKET_ERROR != hr) && (WSAENOBUFS != WSAGetLastError()) )
	{
		WSACleanup();
		return -1;
	}

	LPWSAPROTOCOL_INFO	pProtoInfo = (LPWSAPROTOCOL_INFO)malloc(iBuf);

	hr = WSAEnumProtocols(nTCP, pProtoInfo, &iBuf);
	free(pProtoInfo);

	if(SOCKET_ERROR ==hr)
	{
		WSACleanup();
		return -1; 
	}

	return 0;
}


void LcNet_WSACleanup()
{
	WSACleanup();
}



INT LcNet_SocketErrorCheck(INT hr)
{
	if(SOCKET_ERROR == hr)
	{
		hr = WSAGetLastError();
		hr = (WSA_IO_PENDING ==hr || WSAEWOULDBLOCK==hr || WSAEISCONN==hr) ? 1 : 0;

		if(0 == hr)
			return -1;
	}

	return 0;
}




void LcNet_SocketAddr(SOCKADDR_IN* pOut, char* sIp, char* sPort)
{
	memset(pOut, 0, sizeof(SOCKADDR_IN));

	pOut->sin_family=AF_INET;
	pOut->sin_port=htons(atoi(sPort));

	if(sIp && strlen(sIp)>1)
		pOut->sin_addr.s_addr = inet_addr(sIp);

	else
		pOut->sin_addr.s_addr = htonl(INADDR_ANY);
}



void LcNet_SocketClose(SOCKET* scH)
{
	if(*scH<1)
		return;

	::shutdown(*scH, SD_BOTH);
	closesocket(*scH);
	*scH = 0;
}



INT LcNet_SocketTcpCreate(SOCKET* pScH, BOOL bOverLapped)
{
	BOOL	bDontLinger = TRUE;
	BOOL	bReUse = TRUE;
	LINGER  stLinger={1,0};

	if(bOverLapped)
		*pScH = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	else
		*pScH = socket(AF_INET, SOCK_STREAM, 0);

	
	if(INVALID_SOCKET == *pScH)
		return -1;

	
	// set SO_LINGER to off
	// do not linger on close waiting for unsent data to be sent
	if(SOCKET_ERROR == setsockopt(*pScH, SOL_SOCKET, SO_DONTLINGER, (const char*)&bDontLinger, sizeof(BOOL)))
		return -1;

	
	if(SOCKET_ERROR == setsockopt(*pScH, SOL_SOCKET, SO_LINGER, (const char*)&stLinger, sizeof(LINGER)))
		return -1;

	// set to SO_REUSEADDR
	// allow socket to be bound to an address that is already in use 

	if(SOCKET_ERROR == setsockopt(*pScH, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReUse, sizeof(BOOL)))
		return -1;


	return 0;
}



INT LcNet_SocketUdpCreate(SOCKET* pScH, BOOL bOverLapped)
{
	return -1;
}





INT LcNet_SocketConnect(SOCKET scH, SOCKADDR_IN* psdH)
{
	INT	hr=-1;

	hr = connect(scH, (SOCKADDR*)psdH, sizeof(SOCKADDR_IN));
	
	// 소켓이 비동기일 경우 이것을 반드시 확인
	if(SOCKET_ERROR == hr)
	{
		hr = WSAGetLastError();
		hr = (WSA_IO_PENDING ==hr || WSAEWOULDBLOCK==hr || WSAEISCONN==hr) ? 1 : 0;

		if(0 == hr)
		{
			return -1;
		}
	}

	return 0;
}






INT LcNet_SocketBind(SOCKET scH, SOCKADDR_IN* pSdH)
{
	INT hr=-1;

	hr = bind(scH, (SOCKADDR*)pSdH, sizeof(SOCKADDR_IN));

	if(SOCKET_ERROR == hr)
	{
		hr = WSAGetLastError();
		hr = (WSA_IO_PENDING ==hr || WSAEWOULDBLOCK==hr || WSAEISCONN==hr) ? 1 : 0;

		if(0 == hr)
		{
			return -1;
		}
	}

	return 0;
}


INT LcNet_SocketListen(SOCKET scH)
{
	INT hr=-1;

	hr = listen(scH, SOMAXCONN);

	if(SOCKET_ERROR == hr)
	{
		hr = WSAGetLastError();
		hr = (WSA_IO_PENDING ==hr || WSAEWOULDBLOCK==hr || WSAEISCONN==hr) ? 1 : 0;

		if(0 == hr)
		{
			return -1;
		}
	}

	return 0;
}



INT LcNet_SocketAccept(SOCKET* pscOut, SOCKADDR_IN* psdOut, SOCKET scListen)
{
	INT		hr=-1;
	SOCKET	scCln=0;
	INT		iSizeAdd = sizeof(SOCKADDR_IN);

	scCln = accept(scListen, (SOCKADDR*)psdOut, &iSizeAdd);
//	scCln = WSAAccept(scH, (SOCKADDR*)&sdCln, &iSizeAdd, NULL, NULL);
	
	// 리슨소켓이 비동기의 경우
	if(INVALID_SOCKET == scCln)
	{
		hr = WSAGetLastError();
		hr = (WSA_IO_PENDING ==hr || WSAEWOULDBLOCK==hr || WSAEISCONN==hr) ? 1 : 0;

		if( 0 == hr)
			return -1;

		// 비동기모드로 계속 돌고 있다.
	//	*pscOut =0;
		return 0;
	}

	*pscOut = scCln;

	// 소켓을 만듬
	return 0;
}



INT LcNet_SocketSelect(FD_SET* pFdRead, FD_SET* pFdWrite, TIMEVAL* timeout, FD_SET* pFdExcept)
{
	INT hr=-1;

	hr = select(0, pFdRead, pFdWrite, pFdExcept, timeout);

	if(SOCKET_ERROR == hr)
	{
		hr = WSAGetLastError();
		hr = (WSA_IO_PENDING ==hr || WSAEWOULDBLOCK==hr || WSAEISCONN==hr) ? 1 : 0;

		if(0 == hr)
		{
			return -1;
		}
	}

	return hr;
}



INT LcNet_SocketNonBlocking(SOCKET scH, BOOL bOn)
{
	u_long on =bOn? 1: 0;
	return ioctlsocket(scH, FIONBIO, &on);
}

INT LcNet_SocketNaggleOff(SOCKET scH, BOOL bOff)
{
	BOOL bNagle = bOff? 1: 0; // Nagle 알고리즘 작동 중지
	return setsockopt(scH, IPPROTO_TCP, TCP_NODELAY, (char*)&bNagle, sizeof bNagle);
}


INT LcNet_SocketDirectBuffer(SOCKET scH)
{
	INT nZero=0;
	return setsockopt(scH, SOL_SOCKET, SO_SNDBUF, (const char*)&nZero, sizeof nZero);
}


// AsyncSelect I/O Model
INT LcNet_WSAAsyncSelect(	SOCKET		scH
						,	HWND		hWnd
						,	unsigned int wMsg								// Message ID
						,	long		lEvents)
{
	INT hr= WSAAsyncSelect(scH, hWnd, wMsg, lEvents);

	if(SOCKET_ERROR == hr)
		return hr;

	return 0;
}



// Event I/O Model
HANDLE LcNet_EventCreate(BOOL bManualReset, BOOL bIntitialState)
{
	return CreateEvent(NULL, bManualReset, bIntitialState, NULL);
}


void LcNet_EventResume(HANDLE hEvent)
{
	SetEvent(hEvent);
}


void LcNet_EventSuspend(HANDLE hEvent)
{
	ResetEvent(hEvent);
}

INT LcNet_EventWait(HANDLE hEvent, DWORD dWaitMillisecond)
{
	DWORD hr=0;
	
	hr = WaitForSingleObject(hEvent, dWaitMillisecond);

	if(WAIT_FAILED == hr)
		return -1;

	else if(WAIT_ABANDONED == hr ||WAIT_OBJECT_0 == hr || WAIT_TIMEOUT == hr)
	{
	}


	return 0;
}


void LcNet_EventClose(HANDLE* hEvent)
{
	if(hEvent && *hEvent)
	{
		CloseHandle(*hEvent);
		*hEvent=NULL;
	}
}


HANDLE	LcNet_WSAEventCreate()
{
	return WSACreateEvent();
}


void LcNet_WSAEventClose(HANDLE* pEvent)
{
	if(pEvent && *pEvent)
	{
		WSACloseEvent( *pEvent );
		*pEvent = 0;
	}
}

INT LcNet_WSAEventSelect(SOCKET scH, HANDLE evEvt, long lEvents)
{
	INT hr= WSAEventSelect(scH, evEvt, lEvents);

	if(SOCKET_ERROR == hr)
		return hr;

	return 0;
}


INT LcNet_WSAEventWaits(INT* pArr/*In,Out*/, INT nSrc/*In*/, HANDLE* pSrc/*In*/)
{
	INT iN=0;
	INT nIdx=-1;

	nIdx = WSAWaitForMultipleEvents(nSrc, pSrc, FALSE, WSA_INFINITE, FALSE);
	nIdx -= WSA_WAIT_EVENT_0;

	if(nIdx<0 || nIdx>=WSA_MAXIMUM_WAIT_EVENTS)
		return -1;		// 에러임.

	if(1== nSrc)		//한개만 접속해 있다면 바로 리턴해준다.
		return nSrc;	// 갯수도 1개
		
	for(INT i=nIdx; i<nSrc; ++i)
	{
		nIdx=WSAWaitForMultipleEvents(1, &pSrc[i], TRUE, 0, FALSE);
		
		if((nIdx==WSA_WAIT_FAILED || nIdx==WSA_WAIT_TIMEOUT))
			continue;

		nIdx=i;
		pArr[iN] = i;
		++iN;
	}

	if(iN<0)
		return -1;	// 아예 음수 값을 준다.

	return iN;		// 갯수를 돌려준다.
}



INT LcNet_WSAEventEnum(SOCKET s, HANDLE e)
{
	WSANETWORKEVENTS	pE;
	
	if(SOCKET_ERROR == WSAEnumNetworkEvents(s, e, &pE))
	{
		printf("Enum Events Error\n");
		return -1;
	}

	if( FD_ACCEPT & pE.lNetworkEvents)				// case Accept
	{
		if( pE.iErrorCode[FD_ACCEPT_BIT])
		{
			if(FAILED(LcNet_WSAGetError()))
			{
				printf("Accept Error\n");
				return -1;
			}
		}
		
		return FD_ACCEPT;
	}
	
	else if( FD_CONNECT & pE.lNetworkEvents)		// case Connect
	{
		if( pE.iErrorCode[FD_CONNECT_BIT])
		{
			if(FAILED(LcNet_WSAGetError()))
			{
				printf("Connect Error\n");
				return -1;
			}
		}
		
		return FD_CONNECT;
	}
	
	else if( FD_CLOSE & pE.lNetworkEvents)			// case Close
	{
		if( pE.iErrorCode[FD_CLOSE_BIT])
		{
			if(FAILED(LcNet_WSAGetError()))
			{
//				printf("Close Event Error\n");
//				return -1;
			}
		}
		
		return FD_CLOSE;
	}
	
	else if( FD_READ & pE.lNetworkEvents)			// case Read
	{
		if( pE.iErrorCode[FD_READ_BIT])
		{
			if(FAILED(LcNet_WSAGetError()))
			{
				printf("Read Error\n");
				return -1;
			}
		}
		
		return FD_READ;
	}

	else if( FD_WRITE & pE.lNetworkEvents)			// case Write
	{
		if( pE.iErrorCode[FD_WRITE_BIT])
		{
			if(FAILED(LcNet_WSAGetError()))
			{
				printf("Write Error\n");
				return -1;
			}
		}
		
		return FD_WRITE;
	}
	
	// 다른 메시지 이면
//	printf("Other Message.%d\n", pE.lNetworkEvents);
	
	return 0;
}




INT	LcNet_GetSystemProcessNumber()
{
	SYSTEM_INFO SystemInfo;

	GetSystemInfo(&SystemInfo);
	
	return INT(SystemInfo.dwNumberOfProcessors);
}


HANDLE LcNet_IocpPortCreate(SOCKET scH, HANDLE hIocp, void* pAddress)
{
	return CreateIoCompletionPort((HANDLE)(scH), hIocp, (DWORD)pAddress, 0);
}


INT LcNet_LocalIpAddress(char* sIp/*IP List*/, INT* piN/*IP Count*/, INT* iWidth)
{
	SOCKADDR_IN sdH;
	HOSTENT*	pHost=0;
	char		sHost[512];
	INT			i=0;

	INT nIp=0;
	INT	nWs=16;

	memset(sHost, 0, sizeof sHost);

	if(::gethostname(sHost, sizeof sHost))
		return -1;

	pHost = ::gethostbyname(sHost);

	if(!pHost)
		return -1;

	//설마 네트워크 아이피를 32개 이상 가진 녀석은 없을 것!!!!!
	for(i=0;pHost->h_addr_list[i] && i<32; ++i)
	{
		memcpy(&sdH.sin_addr, pHost->h_addr_list[i], pHost->h_length);
		strcpy(sIp + i* (nWs), inet_ntoa(sdH.sin_addr));
		++nIp;
	}

	if(piN)
		*piN = nIp;

	if(iWidth)
		*iWidth = nWs;


	return 0;
}





// Implementation of the TRingBuf class.
//
////////////////////////////////////////////////////////////////////////////////	

TRingBuf::TRingBuf(): F(0), L(0), W(0), S(0)
{
#if	PCK_USE_BUF_POINTER
	B	= NULL;
	W	= 0;
#else
	W	= PCK_BUF_MAX_QUEUE;
	memset(B, 0, W);
#endif
}


TRingBuf::TRingBuf(INT iSize): F(0), L(0), W(iSize), S(0)
{
#if	PCK_USE_BUF_POINTER
	B	= new BYTE[W];
#endif
	memset(B, 0, W);
}


TRingBuf::~TRingBuf()
{
	W	= 0;
	F	= 0;
	L	= 0;

#if	PCK_USE_BUF_POINTER
	if(B)
	{
		delete [] B;
		B= NULL;
	}
#endif
}

void TRingBuf::SetSize(INT iSize)
{
	F	= 0;
	L	= 0;

#if	PCK_USE_BUF_POINTER
	W	= iSize;
	
	if(B)
	{
		delete [] B;
		B= NULL;
	}
	
	B	= new BYTE[W];
#else
	W	= PCK_BUF_MAX_QUEUE;
#endif
	
	memset(B, 0, W);
}



INT TRingBuf::End()		{	return L;	}
INT TRingBuf::Front()	{	return F;	}
INT TRingBuf::GetStore(){	return S;	}

void TRingBuf::Clear()
{
	F	= 0;
	L	= 0;
	S	= 0;	
	memset(B, 0, W);
}

INT TRingBuf::PushBack(BYTE* /*In*/pSrc, INT iSize/*Length*/)
{
	// 쓸 공간의 부족...
	if( W<(iSize+S))
		return -1;

	INT	iLen = iSize;

	while(iLen--)
	{
		*(B+L) = *(pSrc++);
		++L;
		L %= W;
	}

	S +=iSize;

	return 0;
}


INT TRingBuf::PopFront(BYTE* /*Out*/pDst, WORD* iSize/*Length*/)
{
	BYTE	sSize[PCK_BUF_HEAD]={0};
	INT		T= F;
	WORD	iLen=0;

	// 프론트를 2바이트 이동시켜 본다.
	*(sSize+0) = *(B+T);
	++T;
	T%=W;

	*(sSize+1) = *(B+T);
	++T;
	T%=W;


	*iSize = 0;

	// 패킷 헤더에서 저장되어 있는 패킷의 길이를 가져온다.
	iLen = *((WORD*)sSize);

	// 아직 완선됭 패킷이 없음.
	if(0==iLen)
		return -1;

	// 사이즈가 저장되어 있는 것보다 크다.
	if(iLen>S)
		return -1;

	//목적 버퍼에 패킷을 복사한다.
	WORD	iL= iLen;
	
	while (iLen--)
	{
		*(pDst++) = *(B + F);
		*(B + F) =0;

		++F;
		F %=W;
	}

	// 전체 쌓여 있는 패킷을 양을 조절한다.
	S -= iL;
	*iSize = iL;

	return 0;
}





INT LcNet_PacketEncode(BYTE* pOut		// Output Packet
					  , BYTE* sMsg		// Packet contents
					  , INT iSndM		// Pakcet contents length
					  , DWORD nMsg		// Send Message Kind
					  , INT iPstL		// Packet Structure Length Byte = 2 Byte
					  ,	INT	iPstC		// Packet Cryptography	Length	= 4 Byte
					  , INT iPstM		// Packet Structure Message Byte= 4 Byte
					  , INT iPstT		// Packet Structure Tail Byte	= 4 Byte
						)
{
	// 데이터 전송
	//	+-----------+-------------+-----------+---------+------------+
	//	|길이(2Byte)|Crypto(4Byte)|종류(4Byte)| 메시지  | tail(4Byte)|
	//	+-----------+-------------+-----------+---------+------------+
	INT	eEnc= 0xDEADBEEF;		// Cryptography
	INT	iEnd= 0x0;				// End Mark


	////////////////////////////////////////////////////////////////////////////
	//
	// Encoding Process... 생략..
	//
	////////////////////////////////////////////////////////////////////////////

	// Packet을 만들기위한 위치 계산
	INT  iCr = iPstL;									// 암호화 방식 위치
	INT  iMs = iPstL + iPstC;							// 메시지 종류 위치
	INT  iSn = iPstL + iPstC + iPstM;					// 보낼 Message 위치
	WORD iEn = iPstL + iPstC + iPstM + iSndM;			// End Mark 위치
	WORD iTot= iPstL + iPstC + iPstM + iPstT + iSndM;	// 암호화 길이 포함 전체 패킷 길이


				memcpy(pOut + 0	 , &iTot, iPstL);		// 패킷 전체길이 복사
	if(iPstC)	memcpy(pOut + iCr, &eEnc, iPstC);		// 암호 방식 복사
	if(iPstM)	memcpy(pOut + iMs, &nMsg, iPstM);		// 메시지 종류 복사
				memcpy(pOut + iSn,  sMsg, iSndM);		// 메시지 복사
	if(iPstT)	memcpy(pOut + iEn, &iEnd, iPstT);		// End Mark 복사

	// 패킷 전체 길이 리턴
	return iTot;
}



INT LcNet_PacketDecode(BYTE* sMsg						// Output Message
					  , DWORD* nMsg						// Receive Message Kind
					  , BYTE* pIn						// Receive Packets
					  , INT iRcvM						// Receive Packet Length
					  , INT iPstL						// Packet Structure Length Byte = 2 Byte
					  ,	INT	iPstC						// Packet Cryptography			= 4 Byte
					  , INT iPstM						// Packet Structure Message Byte= 4 Byte
					  , INT iPstT						// Packet Structure Tail Byte	= 4 Byte
					  )
{
	// 패킷을 분해.
	INT		eEnc = 0;									// 암호화 방식
	WORD	iTot = 0;									// 전체 패킷의 길이
	INT		iSnd = 0;									// 전송된 메시지의 길이					
	INT		iEnd= 0x0;									// End Mark


	memcpy(&iTot, pIn + 0, iPstL);						// 전체 길이를 가져온다.

	// 전체 길이와 입력 받은 패킷의 길이를 비교한다.
	if( iTot < iRcvM)
		return -1;

	iSnd = iTot - ( iPstL + iPstC + iPstM + iPstT);		// 메시지의 길이를 설정

	INT  iCr = iPstL;									// 암호화 방식 위치
	INT  iMs = iPstL + iPstC;							// 메시지 종류 위치
	INT  iSn = iPstL + iPstC + iPstM;					// 보낼 Message 위치
	WORD iEn = iTot  - iPstT;							// End Mark 위치
	
	if(iPstC)	memcpy(&eEnc, pIn + iCr , iPstC);		// 암호화 종류를 가져온다.
	if(iPstM)	memcpy( nMsg, pIn + iMs , iPstM);		// 메시지의 종류를 가저온다.
	if(iPstT)	memcpy( sMsg, pIn + iSn , iSnd );		// 메시지를 가져온다.

	////////////////////////////////////////////////////////////////////////////
	//
	// Decoding Process... 생략..
	//
	////////////////////////////////////////////////////////////////////////////

	// 메시지 길이 리턴
	return iSnd;
}




