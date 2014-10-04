// Interface for the ILcxNet class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcxNet_H_
#define _LcxNet_H_

#ifndef interface
#define interface struct
#endif


#ifndef LC_CLASS_DESTROYER
#define LC_CLASS_DESTROYER( CLASS_NAME )	\
virtual ~CLASS_NAME(){}
#endif

typedef unsigned int    u_int;
typedef u_int           SOCKET;


interface ILcxNet
{
	LC_CLASS_DESTROYER(	ILcxNet	);

	enum ELcNetSt				// Network State
	{
		NETST_WSAERROR	= -2,	// WSA�� Ȯ�� ������ ����
		NETST_ERROR		= -1,	// ��Ÿ ��� ERROR

		NETST_NONE		= 0,	// NONE
		NETST_OK		= 1,	// OK
		NETST_CONNECTING= 2,	// While Connecting
		NETST_CONNECTED	= 3,	// Connected
		NETST_CLOSE		= 4,	// CLOSE
	};

	struct _Tdata
	{
	};


	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;			// ������ �ʱ�ȭ�ϰ� ������ �����Ѵ�.
	virtual void	Destroy()=0;												// ������ �Ҹ�ñ�Ű�� ������ �����Ѵ�.
	virtual INT		FrameMove()=0;												// ��Ʈ��ũ���� �ʿ��� �κ��� �����͸� ������Ʈ ��Ų��.
	virtual INT		Query(char* sCmd, void* pData)=0;
	virtual LRESULT	MsgProc(HWND, UINT, WPARAM, LPARAM)=0;						// Async Select��

	virtual INT		Close()=0;													// ���ϸ� �Ҹ��Ų��.
	virtual INT		Listen()=0;													// �������� ���Ǵ� ����
	virtual INT		Connect(char* sIp=NULL, char* sPort=NULL)=0;				// Ŭ���̾�Ʈ���� ���. ������ �ݰ� �ٽ� ������ �� ����Ѵ�. �����ǳ� ��Ʈ�� NULL���ϸ� ���� �����ǿ� ��Ʈ ���

	virtual INT		Send(const char* pSrc, INT  iSnd, DWORD  dMsg
											, INT nIdx = 0xFFFFFFFF)=0;			// ��Ŷ�� ���� ��(�迭�� �ε���)
	virtual INT		Recv(      char* pDst, INT* iRcv, DWORD* dMsg
											, SOCKET* scH=NULL)=0;				// ��Ŷ�� ���� ��(������ ���� ����)

	virtual INT		NetState()=0;												// Network ����
	
	// for server...	
	virtual SOCKET	GetSocket(INT nIdx=0)=0;									// -1: Onself. 0~ Connected Socket
	virtual	INT		SetUserData(_Tdata* pData, INT nIdx=0)=0;					// -1: Onself. 0~ Connected Socket
	virtual	_Tdata* GetUserData(INT nIdx=0)=0;									//

	virtual	INT		ScLstRecvPop(SOCKET* s, int* n)=0;							// ScLst(Socket List)
	virtual	INT		ScLstRecvCount()=0;

	virtual	INT		ScLstNewPop(SOCKET* s, int* n)=0;
	virtual	INT		ScLstNewCount()=0;

	virtual	INT		ScLstClosePop(SOCKET* s, int* n)=0;
	virtual	INT		ScLstCloseCount()=0;
};


INT		LcNet_WinSockCreate();
void	LcNet_WinsockDestroy();

INT		LcNet_CreateTcpServer(char* sCmd
							 , ILcxNet** pData
							 , void* p1			// IP
							 , void* p2			// Port
							 );

INT		LcNet_CreateTcpClient(char* sCmd
							 , ILcxNet** pData
							 , void* p1			// IP
							 , void* p2			// Port
							 );


INT		LcNet_CreateTcpServerEx(char* sCmd
							 , ILcxNet** pData
							 , void* p1			// IP
							 , void* p2			// Port
							 , void* v1			// Ex Value1
							 , void* v2			// Ex Value2
							 );

INT		LcNet_CreateTcpClientEx(char* sCmd
							 , ILcxNet** pData
							 , void* p1			// IP
							 , void* p2			// Port
							 , void* v1			// Ex Value1
							 , void* v2			// Ex Value2
							 );






interface ILcxNetDB
{
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual	INT		Query(char* sCmd, void* pData)=0;


	virtual INT		Connect(void* pDataType, void* =0, void* =0, void* =0)=0;
	virtual void	Close()=0;

	virtual INT		SqlBind(char* sSQL, char*** sOutputBufc,INT** nDataBufc, INT nBufSize)=0;
	virtual INT		SqlExec()=0;
	virtual INT		SqlClose()=0;	
};



INT		LcDB_Create(char* sCmd
					, ILcxNetDB** pData
					,	void* p1 = 0	// No Use
					,	void* p2 = 0	// No Use
					,	void* p3 = 0	// No Use
					,	void* p4 = 0	// No Use
					);

INT		LcDB_CreateAndConnect(char* sCmd				// "ODBC" or "OLEDB"
							  , ILcxNetDB** pData		// In-out Data
							  , void* p1				// DataBase Type: "MDB", "excel", "DSN", ...
							  , void* p2=0				// DataBase Name
							  , void* p3=0				// User Id
							  , void* p4=0				// Password
							  , void* p5=0				// Host Computer
							  );












// ��� �Լ��� �����ϴ� ��� ���� ���� ��ȯ.

#define SAFE_CLOSE_HANDLE(p) { if(p){ CloseHandle( p );	(p) = NULL;	} }

#define ERROR_MESSAGE_LINE(pMsg)				\
{												\
	printf("Error Line:%3d %s\t%s\n"			\
		, __LINE__, __FILE__, (pMsg));			\
}


// Error Message
void	LcNet_FormatMessage(char* sMsg, DWORD hr);								// Network Message Formating
void	LcNet_FormatMessage(char* sMsg);										// Network Message Formating
void	LcNet_ErrorMessage(DWORD hr);											// Network Error Message Catching
INT		LcNet_WSAGetError();


#define ERROR_CHECK_MESSAGE(hr)					\
{												\
	char _sLcErrMsg[1024]={0};					\
	LcNet_FormatMessage(_sLcErrMsg, hr);		\
	printf("Error Line:%3d %s\t%s\n"			\
		, __LINE__, __FILE__, (_sLcErrMsg));	\
}


// Winsock DLL
INT		LcNet_WSAStartup();														// ���� ���̺귯�� �ʱ�ȭ
void	LcNet_WSACleanup();														// ���� ���̺귯�� ����


// Socket
INT		LcNet_SocketErrorCheck(INT hr);											// Socket Error Checking
void	LcNet_SocketAddr(SOCKADDR_IN* pOut, char* sIp, char* sPort);			// Setting Socket Address
void	LcNet_SocketClose(SOCKET* scH);											// Socket Close
INT		LcNet_SocketTcpCreate(SOCKET* pScH, BOOL bOverLapped=FALSE);			// Create TCP Socket
INT		LcNet_SocketUdpCreate(SOCKET* pScH, BOOL bOverLapped=FALSE);			// Create UDP Socket

INT		LcNet_SocketConnect(SOCKET scH, SOCKADDR_IN* psdH);						// Connection
INT		LcNet_SocketBind(SOCKET scH, SOCKADDR_IN* pSdH);						// Socket Binding
INT		LcNet_SocketListen(SOCKET scH);											// Socket Listen

INT		LcNet_SocketAccept(SOCKET* pscOut			// Output Socket
							, SOCKADDR_IN* psdOut	// Output Socket Address
							, SOCKET scListen		// Listen socket
							);						// Accept

INT		LcNet_SocketSelect(FD_SET* pFdRead, FD_SET* pFdWrite, TIMEVAL* timeout, FD_SET* pFdExcept=NULL);

INT		LcNet_SocketNonBlocking(SOCKET scH, BOOL bOn=TRUE);						// Set NonBlocking
INT		LcNet_SocketNaggleOff(SOCKET scH, BOOL bOff=TRUE);						// Off nagle Algorithm
INT		LcNet_SocketDirectBuffer(SOCKET scH);									// Disable send buffering on the socket


// AsyncSelect I/O Model
INT		LcNet_WSAAsyncSelect(	SOCKET		scH			// Socket
							,	HWND		hWnd		// Window Handle
							,	unsigned int wMsg		// Message Procedure ID
							,	long lEvents =(FD_ACCEPT|FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE)
							 );													// WSA Async Select


// Event I/O Model
HANDLE	LcNet_EventCreate(	BOOL bManualReset=FALSE		// FALSE�̸� �������� Reset(Ȱ��ȭ)�� ȣ�� �ؾ� �Ѵ�.
						,	BOOL bIntitialState=TRUE);	// �ʱ� ���°� Signal(Ȱ��ȭ)����

void	LcNet_EventResume(HANDLE hEvent);										// �̺�Ʈ Ȱ��ȭ
void	LcNet_EventSuspend(HANDLE hEvent);										// �̺�Ʈ ��Ȱ��ȭ

INT		LcNet_EventWait(HANDLE hEvent, DWORD dWaitMillisecond=INFINITE);		// Event Wait
void	LcNet_EventClose(HANDLE* hEvent);

HANDLE	LcNet_WSAEventCreate();													// WSA Event Create
void	LcNet_WSAEventClose(HANDLE* pEvent);									// WSA Event Close

INT		LcNet_WSAEventSelect(	SOCKET scH
							,	HANDLE evEvt
							,	long lEvents =(FD_ACCEPT|FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE)
							 );													// WSA Event Select

INT		LcNet_WSAEventWaits(	INT* pArr				// In: Event Array. �迭���� �̺�Ʈ ���� ��ŭ �̺�Ʈ �ε����� ��� �Ǿ� �ִ�.
							,	INT nSrc				// In: Event Array Number.
							,	HANDLE* pSrc			// In:
							);							// Return is Event ����. Failse is -1

INT		LcNet_WSAEventEnum(SOCKET s, HANDLE e);									// WSA Enum Network Event


// ��Ÿ
INT		LcNet_GetSystemProcessNumber();											// Number of CPU
HANDLE	LcNet_IocpPortCreate(SOCKET scH, HANDLE hIocp, void* pAddress);			// IOCP ��Ʈ ����

INT		LcNet_LocalIpAddress(	char* sIp				// List
							,	INT* piN=NULL			// Count
							,	INT* iWidth=NULL);		// Get Local Ip Addresses




// Interface for the TRingBuf class.
//
////////////////////////////////////////////////////////////////////////////////

#define PCK_USE_BUF_POINTER	0
#define PCK_BUF_MAX_MSG		1024
#define PCK_BUF_MAX_QUEUE	4096
#define PCK_BUF_HEAD		2
#define PCK_BUF_CRYPT		4
#define PCK_BUF_KIND		4
#define PCK_BUF_TAIL		4

//#define PCK_BUF_MESSAGE	65536
#define PCK_BUF_MESSAGE		0x100

class TRingBuf
{
public:
	INT		F;						// Header
	INT		L;						// Rear(Last)
	INT		W;						// Width
	INT		S;						// Stored

#if	PCK_USE_BUF_POINTER
	BYTE*	B;						// Buffer
#else
	BYTE	B[PCK_BUF_MAX_QUEUE];
#endif

public:
	TRingBuf();
	TRingBuf(INT iSize);
	virtual ~TRingBuf();

	void	SetSize(INT iSize);
	void	Clear()	;

	INT		End()	;
	INT		Front()	;
	INT		GetStore();

	INT		PushBack(BYTE* /* In*/pSrc, INT   iSize/*Length:  In*/);
	INT		PopFront(BYTE* /*Out*/pDst, WORD* iSize/*Length: Out*/);
};


// Circular Queue
template<class T>
class TqueCrc
{
protected:
	INT		F;						// Header
	INT		L;						// Rear(Last)
	INT		N;						// Queue Count
	INT		W;						// Width
	
	INT		S;						// Space (Unused Memory)

	T*		B;						// Buffer

public:
	TqueCrc(INT iSize): F(0), L(0), N(0), W(0), S(0), B(0)
	{
		W	= iSize;
		B	= new T[W];

		memset(B, 0, W* sizeof(T));
	}
	
	~TqueCrc()
	{
		if(B)	{	delete [] B; B= NULL;	}
	}
	
	void SetSize(INT iSize)
	{
		F	= 0;
		L	= 0;
		N	= 0;
		W	= iSize;
		S	= W;		
		
		if(B)	{	delete [] B; B= NULL;	}
		
		B	= new T[W];
		memset(B, 0, W* sizeof(T));
	}
	
	
	INT PushBack(T* t)
	{
		// *(B+L) = *t;
		memcpy((B+L), t, sizeof T);

		--S;
		++L;
		L %= W;
		++N;
		
		return 0;
	}
	
	void PopFront(T* t)
	{
		if(N<=0)
			return;
		
		// *t = *(B+L);
		memcpy(t, (B+F), sizeof T);

		++S;
		++F;
		F %= W;
		--N;
	}


	INT		GetSize()	{	return N;		}

	INT		End()		{	return L;		}
	INT		Front()		{	return F;		}
	BOOL	IsEmpty()	{	return !N;		}
};




template<class T> inline
T LcNet_Find_Socket(T _F, T _L, SOCKET scH)
{
	for (; _F != _L; ++_F)
	{
		if (scH == (*_F)->scH)
			break;
	}
	
	return _F;
}



template<class T> inline
INT LcNet_Find_Idx(T _F, T _L, void* p)
{
	for (INT i=0; _F != _L; ++_F, ++i)
	{
		if (p == (void*)(*_F))
			return i;
	}
	
	return -1;
}


INT	 LcNet_PacketEncode(BYTE* pOut						// Output Packet
					  , BYTE* sMsg						// Packet contents
					  , INT iSndM						// Pakcet contents length
					  , DWORD nMsg						// Send Message Kind
					  , INT iPstL = (PCK_BUF_HEAD)		// Packet Structure Length	= 2 Byte
					  ,	INT	iPstC = (PCK_BUF_CRYPT)		// Packet Crypto Length		= 4 Byte
					  , INT iPstM = (PCK_BUF_KIND)		// Packet Structure Message = 4 Byte
					  , INT iPstT = (PCK_BUF_TAIL)		// Packet End Mark			= 4 Byte
					  );


INT	 LcNet_PacketDecode(BYTE* sMsg						// Output Message
					  , DWORD* nMsg						// Receive Message Kind
					  , BYTE* pIn						// Receive Packets
					  , INT iRcvM						// Receive Packet Length
					  , INT iPstL = (PCK_BUF_HEAD)		// Packet Structure Length	= 2 Byte
					  ,	INT	iPstC = (PCK_BUF_CRYPT)		// Packet Crypto Length		= 4 Byte
					  , INT iPstM = (PCK_BUF_KIND)		// Packet Structure Message = 4 Byte
					  , INT iPstT = (PCK_BUF_TAIL)		// Packet End Mark			= 4 Byte
					  );








#endif


