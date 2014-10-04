


#if defined(_MSC_VER)
  #pragma comment(linker, "/subsystem:console")
  #pragma comment(lib, "ws2_32.lib")
  #pragma comment(lib, "lgpwn_b22_.lib")
  #pragma comment(lib, "lgpwn_c22_.lib")
  #pragma comment(lib, "lgpwn_x22_.lib")
  #pragma comment(lib, "lgpwn_t22_.lib")

  #pragma comment(lib, "ftf243m_.lib")
  #pragma comment(lib, "ImgJpg_.lib")
  #pragma comment(lib, "png144_.lib")
  #pragma comment(lib, "zlib125_.lib")
  #pragma comment(lib, "al11s_.lib")

  #pragma comment(lib, "opengl32.lib")
  #pragma comment(lib, "winmm.lib")
  #pragma comment(lib, "dxguid.lib")
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcAux.h>

#include <LcxNetwork.h>


USHORT		uPt	= 20000;
BYTE		uIp[16]	= {192, 168, 0, 30,0};
PLCX_NET	g_pCln = NULL;


INT TestSendData();


int main(int argc, char** argv)
{
	INT hr = 0;

	TLC_SOCKET tsc={0};

	tsc.prot  = LCNET_TCP;
	tsc.type  = LCNET_CLIENT;
	tsc.io    = LCNET_SELECT;

	hr = LcxEnt_CreateNet(NULL, &g_pCln, &tsc);
	if(LC_FAILED(hr))
		return 0;


	tsc.uPt   = uPt;
	tsc.uIp   = uIp;
	g_pCln->Query("Connect", &tsc);


	while(g_pCln)
	{
		TestSendData();
		hr=g_pCln->Query("Update", NULL);

		if(0>hr)
			break;
	}


	delete g_pCln;
	g_pCln = NULL;

	return 0;
}



INT TestSendData()
{
	if(NULL == g_pCln)
		return 0;

	INT bConnected = 0;
	g_pCln->GetAttrib(LCNET_STATE, &bConnected);

	if(0>=bConnected)
		return bConnected;


	int		hr;
	char	sTmp[1024]={0};
	int		iLen;
	static int c=0;
	++c;

	static int sqc=0;

	if(c<10000)
		return 0;


	c=0;
	++sqc;
	sprintf(sTmp, "Client Send: %4d", sqc);
	iLen = strlen(sTmp);

	// 데이터 송신
	TLC_PACKET	pck;
	iLen = (iLen >>1 ) <<1;	iLen += 2;

	pck.WriteBegin(0, sqc);
		pck.AddData(sTmp, iLen);
	pck.WriteEnd();

	hr = g_pCln->Send(0, &pck);
	if(LC_FAILED(hr))
		return -1;

	return 0;
}



