//
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _SelectSvr_H_
#define _SelectSvr_H_

#include <vector>
#include <LcType.h>

class CSelectSvr : public ILCX_Net
{
protected:
	struct RemoteHost
	{
		LC_SOCKET	scH;								// socket
		INT			nSnd;								// recorded byte
		char		sSnd[LC_PCK_SEND+4];				// Send buffer

		RemoteHost();
		RemoteHost(LC_SOCKET s);
		~RemoteHost();

		void Close();
		void SetupBuf(char* s, INT len);
	};

protected:
	INT				m_bNetSt;							// state
	LC_SOCKET		m_scHost;							// Listen socket
	fd_set			m_fdSet;							// fd set
	INT				m_fdMax;							// Max descriptor

	USHORT			m_uPt;
	BYTE			m_uIp[LC_IP_LEN];

	TLC_SYNC_MUTEX	m_smSnd;							// for sending mutex
	TLC_SYNC_CS		m_scKey;							// for critical section

	std::vector<RemoteHost* > m_vHost;					// host list


public:
	CSelectSvr();
	virtual ~CSelectSvr();

	virtual INT	Query(char* sCmd, LC_HANDLE);			// execution
	virtual INT	Release();								// release this instnace

	virtual INT	SetAttrib(UINT dCmd, LC_HANDLE);		// write attribute value
	virtual INT	GetAttrib(UINT dCmd, LC_HANDLE);		// read attribute value

	virtual INT	Send(LC_SOCKET  sc, TLC_PACKET*);		// add to sending ring buffer
	virtual INT	Recv(LC_SOCKET* sc, TLC_PACKET*);		// dequeue from receiving buffer
	virtual INT	RecvCount();							// get the number of receive messages

	INT		Create(void* =0, void* =0, void* =0, void* =0);

protected:
	void	Destroy();
	void	EchoMsg(char* s, INT len);
	void	SendBuf();

	INT		ProcRecv();									// receive process
	INT		ProcSend();									// send process

	static DWORD LC_WINAPI ThreadSend(void*);

	void*	FindHost(LC_SOCKET);
	void	DeleteHost(LC_SOCKET);
	void	DeleteNotUseHost();
	void	DeleteAllHost();
};


#endif


