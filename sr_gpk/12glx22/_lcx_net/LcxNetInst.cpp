//
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#if defined(_MSC_VER)
  #include <winsock2.h>
  #include <process.h>

#else

#if !defined(__BADA__)
  #include <netdb.h>
#endif

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
#include "SelectSvr.h"


INT LcxEnt_CreateNet(char* sCmd
				   , PLCX_NET* pOut		// Output instance
				   , LC_HANDLE p1		// User Data(Option)
				   , LC_HANDLE p2		// No Use
				   , LC_HANDLE p3		// No Use
				   , LC_HANDLE p4		// No Use
					 )
{
	INT	hr  = LC_EFAIL;
	TLC_SOCKET* tsc  = (TLC_SOCKET*)p1;
	PLCX_NET	pObj = NULL;


	if(NULL == tsc || LCNET_TCP != tsc->prot)
		return LC_EFAIL;


	if(LCNET_CLIENT == tsc->type)
	{
		if     (LCNET_SELECT == tsc->io)
		{
			pObj = new CSelectCln;
			hr = ((CSelectCln*)pObj)->Create(p1, p2, p3, p4);
		}
		//else if(LCNET_ASYNC == tsc->io)
		//{
		//	pObj = new CAsyncCln;
		//	hr = ((CAsyncCln*)pObj)->Create(p1, p2, p3, p4);
		//}
		//else if(LCNET_EVENT == tsc->io)
		//{
		//	pObj = new CEventCln;
		//	hr = ((CEventCln*)pObj)->Create(p1, p2, p3, p4);
		//}
		//else if(LCNET_IOCP == tsc->io)
		//{
		//	pObj = new CIocpCln;
		//	hr = ((CIocpCln*)pObj)->Create(p1, p2, p3, p4);
		//}
		//else if(LCNET_EPOLL == tsc->io)
		//{
		//	pObj = new CEpollCln;
		//	hr = ((CEpollCln*)pObj)->Create(p1, p2, p3, p4);
		//}
	}

	else if(LCNET_SERVER == tsc->type)
	{
		if     (LCNET_SELECT == tsc->io)
		{
			pObj = new CSelectSvr;
			hr = ((CSelectSvr*)pObj)->Create(p1, p2, p3, p4);
		}
		//else if(LCNET_EVENT == tsc->io)
		//{
		//	pObj = new CEventSvr;
		//	hr = ((CEventSvr*)pObj)->Create(p1, p2, p3, p4);
		//}
		//else if(LCNET_IOCP == tsc->io)
		//{
		//	pObj = new CIocpSvr;
		//	hr = ((CIocpSvr*)pObj)->Create(p1, p2, p3, p4);
		//}
		//else if(LCNET_EPOLL == tsc->io)
		//{
		//	pObj = new CEpollSvr;
		//	hr = ((CEpollSvr*)pObj)->Create(p1, p2, p3, p4);
		//}
	}

	
	if(LC_FAILED(hr))
	{
		if(pObj)
			delete pObj;

		return hr;
	}


	*pOut = pObj;
	return LCNET_OK;
}

