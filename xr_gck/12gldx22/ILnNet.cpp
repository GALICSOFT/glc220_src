// Implementation of the ILcxNet class.
//
////////////////////////////////////////////////////////////////////////////////


#include <WINSOCK2.H>
#include <windows.h>
#include <stdio.h>

#include "LnxNet.h"

#include "LnNet.h"

#include "LnNetSlctA.h"


INT LcNet_WinSockCreate()
{
	return LcNet_WSAStartup();
}

void LcNet_WinsockDestroy()
{
	LcNet_WSACleanup();
}

INT LcNet_Create(char* sCmd
				 , ILcxNet** pData
				 , void* p1			// IP
				 , void* p2			// Port
				 , void* p3			// TCP: "TCP" , UDP: "UDP"
				 , void* p4			// Server: "Server", Client: "Client"
				 , void* v1 = 0		// Ex Value1
				 , void* v2 = 0		// Ex Value2
				 );




INT LcNet_CreateTcpServer(char* sCmd
				 , ILcxNet** pData
				 , void* p1			// IP
				 , void* p2			// Port
				 )
{
	return LcNet_Create(sCmd, pData, p1, p2, "TCP", "Server");
}

INT LcNet_CreateTcpClient(char* sCmd
				 , ILcxNet** pData
				 , void* p1			// IP
				 , void* p2			// Port
				 )
{
	return LcNet_Create(sCmd, pData, p1, p2, "TCP", "Client");
}



INT LcNet_CreateTcpServerEx(char* sCmd
				 , ILcxNet** pData
				 , void* p1			// IP
				 , void* p2			// Port
				 , void* v1			// Ex Value1
				 , void* v2			// Ex Value2
				 )
{
	return LcNet_Create(sCmd, pData, p1, p2, "TCP", "Server", v1, v2);
}

INT LcNet_CreateTcpClientEx(char* sCmd
				 , ILcxNet** pData
				 , void* p1			// IP
				 , void* p2			// Port
				 , void* v1			// Ex Value1
				 , void* v2			// Ex Value2
				 )
{
	return LcNet_Create(sCmd, pData, p1, p2, "TCP", "Client", v1, v2);
}





INT LcNet_Create(char* sCmd
				 , ILcxNet** pData
				 , void* p1			// IP
				 , void* p2			// Port
				 , void* p3			// TCP: "TCP" , UDP: "UDP"
				 , void* p4			// Server: "Server", Client: "Client"
				 , void* v1			// Ex Value1
				 , void* v2			// Ex Value2
				 )
{
	ILcxNet* pObj = NULL;

	(*pData) = NULL;

	if(0==_stricmp("Async Select", sCmd))
	{
		pObj = new CLcNetSlctA;
		pObj->Query("Set Window Handle", v1);
		pObj->Query("Set Message Value", v2);
	}
	else
	{
		return -1;
	}


	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;
	return 0;
}


