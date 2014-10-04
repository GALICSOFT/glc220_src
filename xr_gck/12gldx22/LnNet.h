// Interface for the CLcNet class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcNet_H_
#define _LcNet_H_

class CLcNet : public ILcxNet
{
public:
	enum ELcNetPrtcl
	{
		NETPRT_NONE	= 0,				// NONE
		NETPRT_TCP	= 1,				// TCP
		NETPRT_UDP	= 2,				// UDP
	};

	enum ELcNetHstType
	{
		NETHST_NONE		=0,				// NONE
		NETHST_CLIENT	=1,				// Client
		NETHST_SERVER	=2,				// Server
	};

	enum ELcNetEventType
	{
		NETEVT_NONE		=0,				// NONE
		NETEVT_SEND		=1,				// Send Event
		NETEVT_RECV		=2,				// Receive
		NETEVT_ACCP		=4,				// Accept
		NETEVT_CONN		=8,				// Connect
		NETEVT_CLOSE	=16,			// Close
	};

	struct _TsckMsg
	{
		SOCKET	nScH;
		INT		nIdx;
		
		_TsckMsg():nScH(0), nIdx(-1)	{}
		_TsckMsg(SOCKET s, INT n):nScH(s), nIdx(n){}
	};

protected:
	SOCKET			m_scH;				// Host socket
	SOCKADDR_IN		m_sdH;				// Host Address
	char			m_sIp[32];			// IP
	char			m_sPt[16];			// Port

	_Tdata*			m_pUsrData;			// User Data

	INT				m_PtcType;			// Protocol type
	INT				m_HstType;			// Client or Server?

	INT				m_eNetSt;			// Network State

public:
	CLcNet();
	virtual ~CLcNet();
	
	virtual void	Destroy();
	virtual INT		Query(char* sCmd, void* pData);
	virtual LRESULT	MsgProc(HWND, UINT, WPARAM, LPARAM);
	virtual INT		NetState();

public:
	static DWORD WINAPI ThreadRecv(void* pParam);		// Receive 쓰레드
	static DWORD WINAPI ThreadSend(void* pParam);		// Send 쓰레드
	static DWORD WINAPI ThreadAccp(void* pParam);		// Accept 쓰레드
	static DWORD WINAPI ThreadWork(void* pParam);		// Work 쓰레드

	virtual DWORD	ProcRecv(void* pParam);
	virtual DWORD	ProcSend(void* pParam);
	virtual DWORD	ProcAccp(void* pParam);
	virtual DWORD	ProcWork(void* pParam);

// for Server..
protected:
	TqueCrc<_TsckMsg >*	m_pScktMsg;												// 메시지 받는 소켓 리스트
	TqueCrc<_TsckMsg >*	m_pScktNew;												// 생성한 소켓 리스트
	TqueCrc<_TsckMsg >*	m_pScktCls;												// 닫은 소켓 리스트

public:
	virtual	void	ScLstRecvPush(SOCKET s, int n);
	virtual	INT		ScLstRecvPop(SOCKET* s, int* n);
	virtual	INT		ScLstRecvCount();

	virtual	void	ScLstNewPush(SOCKET s, int n);
	virtual	INT		ScLstNewPop(SOCKET* s, int* n);
	virtual	INT		ScLstNewCount();

	virtual	void	ScLstClosePush(SOCKET s, int n);
	virtual	INT		ScLstClosePop(SOCKET* s, int* n);
	virtual	INT		ScLstCloseCount();
};

#endif
