//
//
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _SelectCln_H_
#define _SelectCln_H_

#include <LcType.h>


class CSelectCln : public ILCX_Net
{
protected:
	INT						m_bNetSt;					// net state
	LC_SOCKET				m_scHost;					// host socket
	fd_set					m_fdSet;					// socket fd set

	USHORT					m_uPt;
	BYTE					m_uIp[LC_IP_LEN];

	TLC_SYNC_MUTEX			m_smSnd;					// for sending mutex
	TLC_SYNC_CS				m_scKey;					// for critical section

	INT						m_nSnd;						// recorded 
	char					m_sSnd[LC_PCK_SEND+4];		// send buffer
	INT						m_nRcv;						// received
	char					m_sRcv[LC_PCK_RECV+4];		// recv buffer

public:
	CSelectCln();
	virtual ~CSelectCln();

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
	INT		Connect();
	INT		DisConnect();
	void	SendBuf();

	INT		ProcSend();									// send process
	INT		ProcRecv();									// receive process

	static DWORD LC_WINAPI ThreadSend(void*);			// thread for sending
};

#endif

