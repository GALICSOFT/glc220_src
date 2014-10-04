//	Copyright(C) GaLic Soft. All rights reserved.
//	GaLic Game Library Collection
//
// Interface for the ILCX_Network class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxNetwork_H_
#define _LcxNetwork_H_

#include <LcType.h>


enum ELC_NETWORK
{
	// general return
	LCNET_INVALID_SOCKET	= (~0),
	LCNET_EFAIL				= LC_EFAIL,
	LCNET_OK				= LC_OK,
	LCNET_TIMEOUT			= LC_OK,

	// protocol
	LCNET_TCP				= 6,								// IPPROTO_TCP
	LCNET_UDP				= 17,								// IPPROTO_UDP

	// io option
	LCNET_SOCK_NONBLOCKING	= 126,								// FIONBIO
	LCNET_SOCK_NAGLE_OFF	= 0x0001,							// TCP_NODELAY

	LCNET_SOCK_REUSEADDR	= 0X0004,							// SO_REUSEADDR
	LCNET_SOCK_LINGER		= 0x0080,							// SO_LINGER
	LCNET_SOCK_DONTLINGER	= (UINT)(~LCNET_SOCK_LINGER),

	LCNET_SOCK_SNDTIMEO		= 0x1005,							// SO_SNDTIMEO
	LCNET_SOCK_RCVTIMEO		= 0x1006,							// SO_RCVTIMEO
	LCNET_SOCK_SNDBUF		= 0x1001,							// SO_SNDBUF
	LCNET_SOCK_RCVBUF		= 0x1002,							// SO_RCVBUF

	// Host type
	LCNET_SERVER			= 0x00010001,						// Server
	LCNET_CLIENT			= 0x00010002,						// Clinet

	// io notify type
	LCNET_SELECT			= 0x00020001,						// select for linux and windows client or p2p
	LCNET_ASYNC				= 0x00020002,						// async select for windows client
	LCNET_EVENT				= 0x00020003,						// event select for windows client
	LCNET_IOCP				= 0x00020004,						// iocp for windows server
	LCNET_EPOLL				= 0x00020005,						// epoll for linux server

	// for packet
	LC_PCK_MAX				= 0x568,							// max segment size = 1384
	LC_PCK_HEAD				= 16,								// packet header size = 16
	LC_PCK_SEND				= 2*(LC_PCK_MAX+LC_PCK_HEAD),		// max send buffer size
	LC_PCK_RECV				= 4*(LC_PCK_MAX+LC_PCK_HEAD),		// max send buffer size


	// for query and attribute command
	LCNET_CONNECT			= 0x00030001,						// connection action
	LCNET_STATE				= 0x00030002,						// for getting the state

#if defined(_MSC_VER)
	LCNET_EVENT_MAX			= 64,
#else
	LCNET_EVENT_MAX			= 1024,
#endif

};


INT  LcNet_Open();																// Network Device Open
INT  LcNet_Close();																// Network Device Close

INT  LcNet_SocketCreate		(LC_SOCKET* pScH, INT type);						// Create Socket. nagle off, linger(1,0), don't linger, reuseaddr
INT  LcNet_SocketClose		(LC_SOCKET* scH);									// Socket Close

INT  LcNet_SocketConnect	(LC_SOCKET scH, USHORT uPort, BYTE* uIp);			// Connection
INT  LcNet_SocketBind		(LC_SOCKET scH, USHORT uPort, BYTE* uIp=NULL);		// Socket Binding
INT  LcNet_SocketListen		(LC_SOCKET scH);									// Socket Listen
INT  LcNet_SocketAccept		(LC_SOCKET* pScH, LC_SOCKET scListen);				// Accept

INT  LcNet_SocketSend		(LC_SOCKET scH, const char* Buf,INT len);			// Data(Buffer) Send (Write)
INT  LcNet_SocketRecv		(LC_SOCKET scH, char* Buf,INT len);					// Data(Buffer) Receive (Read)
INT  LcNet_SocketOptionSet	(LC_SOCKET scH, INT opt, INT v);					// set the socket option

INT  LcNet_IpAddress		(char* sIp, LC_SOCKET scH);							// get the ip address from socket
INT  LcNet_LocalIpAddress	(char** sIpList);									// Get Local Ip Addresses. Let's read 20 bytes each ip address. if failed return -1 else return the number of ip address
void LcNet_InetAddr32		(BYTE* uIp, char* sIp);								// 32-bit ip

INT  LcNet_LastGetErr	();
void LcNet_ErrFormat	(char* s, INT er);
INT  LcNet_IsNotErr		(INT er);

void LcNet_Log			(char* msg, INT line=0, char* file=NULL);
void LcNet_Log			(INT line, char* file, INT er=0, char* msg=NULL);

INT  LcNet_ProcessorCount();

INT  LcNet_Select	(INT nFdCnt, LC_HANDLE R, LC_HANDLE W, LC_HANDLE E
						, INT wait=LC_INFINITE);								// select read. wait time = millisecond


LC_HANDLE LcNet_ThreadCreate(void* pFunc, void* pParam = NULL
							, INT bSuspend = LC_FALSE, void* pId=NULL);
void LcNet_ThreadEnd	(UINT =0);
INT  LcNet_ThreadClose	(LC_HANDLE* v);

void LcNet_CloseHandle	(LC_HANDLE* v);
void LcNet_Sleep		(INT millisecond);


////////////////////////////////////////////////////////////////////////////////


struct TLC_SOCKET
{
	LC_SOCKET	sc;
	INT			prot;									// protocol: LCNET_TCP, LCNET_UDP
	INT			type;									// host type: LCNET_SERVER, LCNET_CLIENT
	INT			io;										// io type: LCNET_SELECT, LCNET_ASYNC, LCNET_EVENT, LCNET_IOCP, LCNET_EPOLL
	USHORT		uPt;									// port
	BYTE*		uIp;									// ip
	LC_HANDLE	tx[4];									// extended data
};


#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif

struct TLC_PACKET
{
	union
	{
		struct
		{
			unsigned short	bsp;						// 0xcccc
			unsigned char	pvj;						// packet major version
			unsigned char	pvn;						// packet minor version
			unsigned short	crp;						// crypto level or type 0,1,2,4 etc
			unsigned short	len;						// packet length
			unsigned int	pro;						// packet type
			unsigned int	sqc;						// packet sequence
			char	sgm[LC_PCK_MAX];					// segment data 1384
		};
		char pck[LC_PCK_MAX+LC_PCK_HEAD];				// 1400
	};

	TLC_PACKET();
	TLC_PACKET(const char* src);
	void  Reset();

	INT   GetType();
	INT   GetLength();
	INT   GetLengthData();
	char* GetPack();
	char* GetData();

	void  WriteBegin	(INT type, INT sqc);
	void  WriteEnd		(INT crypto = 0);
	void  AddData		(void* data, INT len);
	void  ReadAll		(void* dst);
	INT   ReadData		(void* dst, INT size, INT offset);
}

#if !defined(_MSC_VER)
	__attribute__((packed))
#endif
;

#if defined(_MSC_VER)
#pragma pack(pop)
#endif


DCL_INTERFACE ILCX_Net
{
	DCL_CLASS_DESTROYER(	ILCX_Net	);

	virtual INT	Query(char* sCmd, LC_HANDLE)=0;									// execution
	virtual INT	Release()=0;													// release this instnace

	virtual INT	SetAttrib(UINT dCmd, LC_HANDLE)=0;								// write attribute value
	virtual INT	GetAttrib(UINT dCmd, LC_HANDLE)=0;								// read attribute value

	virtual INT	Send(LC_SOCKET  , TLC_PACKET*)=0;								// add to sending ring buffer
	virtual INT	Recv(LC_SOCKET* , TLC_PACKET*)=0;								// dequeue from receiving buffer
	virtual INT	RecvCount()=0;													// get the number of receive messages
};

typedef ILCX_Net*	PLCX_NET;

// Create the network instance.
INT LcxEnt_CreateNet(char* sCmd
				   , PLCX_NET* pOut												// Output instance
				   , LC_HANDLE p1 = NULL										// User Data(Option)
				   , LC_HANDLE p2 = NULL										// No Use
				   , LC_HANDLE p3 = NULL										// No Use
				   , LC_HANDLE p4 = NULL										// No Use
					 );



// for linux epolling system
struct TLC_EPOLL
{
	INT			eh;		// epoll handle for accept
	LC_HANDLE	ev;		// epoll event
	INT			en;		// watch count

	TLC_EPOLL();
	~TLC_EPOLL();

	INT		Create(INT cnt);
	void	Close();
	INT		Ctrl(INT fd, INT opt, INT io_t);
	INT		Wait(DWORD waitTime = LC_INFINITE);
	INT		GetFd(INT n);	// get the fd to be binded event
};



// Synchronization for CRITICAL SECTION
struct TLC_SYNC_CS
{
	LC_HANDLE	sync_m;

	TLC_SYNC_CS();
	~TLC_SYNC_CS();

	INT		Init();
	INT		Close();
	INT		Lock();
	INT		Unlock();
};


// sync for mutex
struct TLC_SYNC_MUTEX
{
	LC_HANDLE	sync_m;
	LC_HANDLE	sync_d;

	TLC_SYNC_MUTEX();
	~TLC_SYNC_MUTEX();

	INT		Init();
	INT		Close();
	INT		Begin(int wait=-1);	// begin wait
	INT		End();				// release mutex
	INT		SetSignal();
};


#endif


