

#if defined(_MSC_VER) && _MSC_VER > 1000
  #pragma warning( disable : 4786)
  #pragma warning( disable : 4996)
#endif


#if defined(_MSC_VER)
  #pragma comment(linker, "/subsystem:console")

  #pragma comment(lib, "lgpwn_b22_.lib")
  #pragma comment(lib, "lgpwn_c22_.lib")
  #pragma comment(lib, "lgpwn_x22_.lib")
  #pragma comment(lib, "lgpwn_t22_.lib")
  #pragma comment(lib, "lgpwn_ogg_.lib")
  #pragma comment(lib, "lgpwn_vorbis_.lib")

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

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>


struct TimeEvent : public ILC_MsgPrc
{
	virtual INT MsgPrc(LC_HANDLE p1=0, LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0)
	{
		PLC_TIMER pTimer = (PLC_TIMER)p1;

		UINT nCnt	= pTimer->GetCount();
		UINT nRemain= pTimer->GetRemainCount();
		char* sMsg	= (char*)pTimer->GetUserData();

		char sbuf[128]={0};
		int r = 0;

		r = sprintf(&sbuf[r], "key: 0x%p", p1);

		if(sMsg )
			r += sprintf(&sbuf[r], " Msg: %s", sMsg);

		r += sprintf(&sbuf[r], " count:%3d   remain:%3d", nCnt, nRemain);

		if(0 == nRemain)
			r += sprintf(&sbuf[r], "   --- stop");
		
		r += sprintf(&sbuf[r], "\n");

		printf(sbuf);

		return 0;
	}
};


int main(int arg, char *argv[])
{
	INT hr;
	PLC_TIMER	m_pTimer1 = 0;
	PLC_TIMER	m_pTimer2 = 0;
	PLC_TIMER	m_pTimer3 = 0;

	char msg1[] = "message1";
	char msg2[] = "message2";
	char msg3[] = "message3";




	// set 1 sec interval timer.
	hr = LcSys_TimerCreate(NULL, &m_pTimer1, 0, 700, 40, new TimeEvent, msg1);

	if(0>hr)
	{
		printf("LcTimer_Set() fail\n");
		return -1;
	}

	// set 2 sec interval timer.
	hr = LcSys_TimerCreate(NULL, &m_pTimer2, 0, 800, 50, new TimeEvent, msg2);

	if(0>hr)
	{
		printf("LcTimer_Set() fail\n");
		return -1;
	}

	// set 3 sec interval timer.
	hr = LcSys_TimerCreate(NULL, &m_pTimer3, 0, 900, 60, new TimeEvent, msg3);

	if(0>hr)
	{
		printf("LcTimer_Set() fail\n");
		return -1;
	}

	m_pTimer1->Start();
	m_pTimer2->Start();
	m_pTimer3->Start();

	int key = 0;
	char cmd[32] = {0,};

	while(1)
	{
		LcSys_Sleep(1000);

		if(	0 == m_pTimer1->GetRemainCount() &&
			0 == m_pTimer2->GetRemainCount() &&
			0 == m_pTimer3->GetRemainCount() )
			break;
	}

	m_pTimer1->Release();
	m_pTimer2->Release();
	m_pTimer3->Release();

	return 0;
}

