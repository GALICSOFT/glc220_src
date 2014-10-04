// Implementation of the GLC Input.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#if defined(_WIN32)
  #include <windows.h>
#endif

// for X-window base
#if defined(_PC_LINUX_)
  #include <X11/keysym.h>
  #include <GL/glx.h>
  #include <unistd.h>
  #include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <LcType.h>

#include "LcBase.h"
#include "LcStdev.h"
#include "LcAux.h"


namespace LCSYS
{
	#if defined(_PC_LINUX_)
	LC_HANDLE	polling_input(LC_HANDLE p);			// Input Thread Function
	pthread_t	g_InputTh;							// Input Thread
	INT			g_InputThID;						// Thread ID
	#endif

	BYTE		g_InputKey[LC_MAX_KEY]={0};

	TLC_INPUT_TS	g_InputMts[LC_MAX_TSB]={{0,},};
	//{
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//	{ -1, -1, LCIK_KEYNONE,} ,
	//};
}

//
////////////////////////////////////////////////////////////////////////////////



INT LcStv_InputOpen()
{
#if defined(_PC_LINUX_)
	LC_HWND			pWnd = (LC_HWND)LcVar_ReadP(LC_VAR_WINDOW);
	Window			hWnd = (Window)pWnd->w;

	LCSYS::g_InputThID = pthread_create(&LCSYS::g_InputTh, NULL, polling_input, NULL);
	pthread_detach(LCSYS::g_InputTh);

	if(LCSYS::g_InputThID < 0)
	{
		perror("polling input thread create error\n");
		return -1;
	}

#endif

	return LC_OK;
}



INT LcStv_InputClose()
{
#if defined(_PC_LINUX_)
	LCSYS::g_InputDspX = NULL;
#endif

	return LC_OK;
}



INT LcStv_InputWriteMts(INT idx, TLC_INPUT_TS* pSrc)
{
	INT	nOrient = 0;

	INT sysScn[2];
	INT sysW, sysH;
	INT scnW, scnH;
	TLC_INPUT_TS*	pDst = NULL;

	if(0 > idx || idx >=LC_MAX_TSB)
		return LC_EFAIL;


	// set to default state
	if(NULL == pSrc || LCIK_KEYNONE == pSrc->t)
	{
		// select the dest
		pDst = &LCSYS::g_InputMts[idx];

		pDst->x = -1;
		pDst->y = -1;
		pDst->t = LCIK_KEYNONE;

		return LC_OK;
	}


#if defined(_MSC_VER)

	// copy to dest
	pDst = &LCSYS::g_InputMts[idx];
	memcpy(pDst, pSrc, sizeof(TLC_INPUT_TS));

	return LC_OK;
#endif


	LcSys_GetAttrib(LCSYS_ORIENT, &nOrient);
	LcSys_GetAttrib(LCSYS_CONST_WINDOW_SIZE, &sysScn[0]);
	LcVar_WindowSize(&scnW, &scnH);

	sysW = sysScn[0];
	sysH = sysScn[1];
	//LOGW("window: system(%d %d), game window( %d %d ) ------------------------\n", sysW, sysH, scnW, scnH);


#if defined(__APPLE__) || defined(__IPHONE__)

	// select the dest
	pDst = &LCSYS::g_InputMts[idx];

	if(LCSYS_ORIENT_LLANDSCAPE == nOrient)
	{
		pDst->x =        pSrc->y;
		pDst->y = scnW - pSrc->x;
	}
	else if(LCSYS_ORIENT_RLANDSCAPE == nOrient)
	{
		pDst->x = scnH - pSrc->y;
		pDst->y =        pSrc->x;
	}

#elif defined(__ANDROID__) || defined(__BADA__)

	// find index for up
	if(LCIK_KEYUP == pSrc->t)
	{
		int i=0, n=-1;

		for(i=0; i<LC_MAX_TSB; ++i)
		{
			int st = LCSYS::g_InputMts[i].t;
			//LOGI("WriteMts::Up[%d, %d] ------------------------\n", i, st);

			if(LCIK_KEYNONE >= st)
				continue;

			++n;

			if(n == idx)
			{
				idx = i;
				break;
			}
		}

		if(LC_MAX_TSB <= i)
		{
			int nCnt = LcStv_InputCountMts();

			LOGE("WriteMts:Find Failed [%d/%d] ------------------------\n", idx, nCnt);
			return LC_EFAIL;
		}

		//LOGI("WriteMts::Up[%d/%d] ------------------------\n", idx, LcStv_InputCountMts());

		// select the dest
		pDst = &LCSYS::g_InputMts[idx];
		memcpy(pDst, pSrc, sizeof(TLC_INPUT_TS));
		pDst->t = LCIK_NONE;
	}
	else
	{
		// select the dest
		pDst = &LCSYS::g_InputMts[idx];
		memcpy(pDst, pSrc, sizeof(TLC_INPUT_TS));

		//LOGI("WriteMts::Down[%d/%d] ------------------------\n", idx, LcStv_InputCountMts());

		// debuging...
		//if(LCIK_KEYDOWN == pSrc->t)
		//{
		//	for(int i=0; i<LC_MAX_TSB; ++i)
		//	{
		//		int st = LCSYS::g_InputMts[i].t;
		//		if(LCIK_KEYNONE >= st)
		//			continue;

		//		LOGI("WriteMts::Down[%d, %d] ------------------------\n", i, st);
		//	}
		//}
	}


	if(LCSYS_ORIENT_PORTRAIT == nOrient)
	{
		pDst->x = pSrc->x * scnW/ sysW;
		pDst->y = pSrc->y * scnH/ sysH;
		//LOGW("LCSYS_ORIENT_PORTRAIT input: %d %d ------------------------\n", x, y);
	}

	else if(LCSYS_ORIENT_LLANDSCAPE == nOrient)
	{
		pDst->x = (pSrc->y       ) * scnH/ sysH;
		pDst->y = (sysW - pSrc->x) * scnW/ sysW;
		//LOGW("LCSYS_ORIENT_LLANDSCAPE input: %d %d ------------------------\n", x, y);
	}

	else if(LCSYS_ORIENT_RLANDSCAPE == nOrient)
	{
		pDst->x = (sysH - pSrc->y) * scnH/sysH;
		pDst->y = (       pSrc->x) * scnW/sysW;
		//LOGW("LCSYS_ORIENT_RLANDSCAPE input: %d %d ------------------------\n", x, y);
	}
#endif

	return LC_OK;
}



INT LcStv_InputReadMts(INT idx, TLC_INPUT_TS* pDst)
{
	TLC_INPUT_TS* pSrc = NULL;

	if(0 > idx || idx >=LC_MAX_TSB)
		return LC_EFAIL;


	pSrc = &LCSYS::g_InputMts[idx];

#if defined(_WIN32)
	POINT	pt={0};

	HWND	hWnd = (HWND)LcVar_ReadP(LC_VAR_WINDOW);

	if(NULL == hWnd || hWnd != GetActiveWindow() )
	{
		pDst->x = -1;
		pDst->y = -1;
		pDst->t = LCIK_KEYNONE;
		return LC_CANNT_ACQUIRE_DEVICE;
	}

	// left(0), right(1) button
	if(1 >= idx)
	{
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);

		LCSYS::g_InputMts[idx].x = pt.x;
		LCSYS::g_InputMts[idx].y = pt.y;
	}

	if     (0 == idx) 	LCSYS::g_InputMts[0].t = (GetAsyncKeyState(WNVK_LBUTTON) & 0x8000) ? 1: 0;	// LButton
	else if(1 == idx)	LCSYS::g_InputMts[1].t = (GetAsyncKeyState(WNVK_RBUTTON) & 0x8000) ? 1: 0;	// RButton
	else if(2 == idx)	LCSYS::g_InputMts[2].t = (GetAsyncKeyState(WNVK_MBUTTON) & 0x8000) ? 1: 0;	// MButton
#endif

	memcpy(pDst, pSrc, sizeof(TLC_INPUT_TS));

	return LC_OK;
}



INT LcStv_InputCountMts()
{
	int nCnt = 0;

	for(int i=0; i<LC_MAX_TSB; ++i)
	{
		if(LCIK_KEYNONE < LCSYS::g_InputMts[i].t)
			++nCnt;
	}

	return nCnt;
}



////////////////////////////////////////////////////////////////////////////////

INT LcStv_InputReadKey(PKEYMAP pOut)
{
	INT		n =0;

#if defined(_WIN32)
	HWND	hWnd = (HWND)LcVar_ReadP(LC_VAR_WINDOW);
	INT		Key = 0;

	if(NULL == pOut || NULL == hWnd)
		return LC_CANNT_ACQUIRE_DEVICE;

	if(hWnd != GetActiveWindow() )
		return LC_CANNT_ACQUIRE_DEVICE;

	for(n=0; n<LC_MAX_KEY; ++n)
	{
		Key = GetAsyncKeyState(n) & 0x8000;
		LCSYS::g_InputKey[n] = (Key)? 1: 0;
	}

#elif defined(_PC_LINUX_)


#endif

	for(n=0; n<LC_MAX_KEY; ++n)
		pOut[n] = LCSYS::g_InputKey[n];

	return LC_OK;
}


INT LcStv_InputWriteKey(INT key, INT v)
{
	LCSYS::g_InputKey[key] = v;

	return LC_OK;
}





#if defined(_PC_LINUX_)

// Read Key and Mouse Event
void* polling_input(void*)
{
	XEvent	EventX={0};
	char	buf[32]={0};
	KeySym	k;
	INT		hr=0;
	INT		c=0;

	sched_param param;

	param.sched_priority = sched_get_priority_min(SCHED_RR);
	hr = pthread_setschedparam(pthread_self(), SCHED_RR, &param);

	if(0 != hr)
	{
		printf("Err: priority RR\n");
		param.sched_priority = sched_get_priority_min(SCHED_FIFO);
		hr = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

		if(0 != hr)
		{
			printf("Err: priority FIFO\n");
			param.sched_priority = 0;
			hr = pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);

			if(0 != hr)
				printf("Err: priority OTHER\n");
		}
	}


	while(NULL != LCSYS::g_InputDspX)
	{
		//printf("%d\n", c);
		//hr = XPending(LCSYS::g_InputDspX);
		//if(0 >= hr )
		//{
		//	++c;
		//	usleep(10 * 1000);
		//	continue;
		//}

		hr = XNextEvent(LCSYS::g_InputDspX, &EventX);		// Blocking until Event occurrence, so does not need XPending()

		//if(0>=hr)
		//{
		//	printf("No Event\n");
		//	continue;
		//}
		//
		//printf("Event: %d\n", hr);
		//if(XFilterEvent(&EventX, LCSYS::g_InputWinX))
		//	continue;

		if(ButtonPress == EventX.type)
		{
			//printf("button press: %d\n", EventX.xkey.keycode);
			//printf("button press: %d %d %d\n", EventX.xbutton.button, EventX.xbutton.x, EventX.xbutton.y);
			if(1 == EventX.xbutton.button)
			{
				LCSYS::g_InputMtsX = EventX.xbutton.x;
				LCSYS::g_InputMtsY = EventX.xbutton.y;
				LCSYS::g_InputMtsT = 1;
			}
		}
		else if(ButtonRelease == EventX.type)
		{
			//printf("button Up: %d %d %d\n", EventX.xbutton.button, EventX.xbutton.x, EventX.xbutton.y);
			if(1 == EventX.xbutton.button)
			{
				LCSYS::g_InputMtsX = EventX.xbutton.x;
				LCSYS::g_InputMtsY = EventX.xbutton.y;
				LCSYS::g_InputMtsT = 1;
			}
		}

		else if(ClientMessage == EventX.type)
		{
			//	if (*XGetAtomName(LCSYS::g_InputDspX, EventX.xclient.message_type) == *"WM_DELETE_WINDOW")
			//	{
			//		hr = -1;
			//		return hr;
			//	}
			//
		}

		// Resize screen// Resize screen
		else if(ConfigureNotify == EventX.type)
		{
			//	if ((EventX.xconfigure.width != m_ScnW) || (EventX.xconfigure.height != m_ScnH))
			//	{
			//	}
		}

		else if(KeyPress == EventX.type)
		{
			memset(buf, 0, sizeof buf);
			hr = XLookupString( (XKeyEvent*)&EventX, buf, 16, &k, NULL);

			//if(0<hr)
			{
				if(XK_w      == k)	{ LCSYS::g_InputKey[LCKEY_UP        ] =1; }
				if(XK_q      == k)	{ LCSYS::g_InputKey[LCKEY_UP_LEFT   ] =1; }
				if(XK_a      == k)	{ LCSYS::g_InputKey[LCKEY_LEFT      ] =1; }
				if(XK_z      == k)	{ LCSYS::g_InputKey[LCKEY_DOWN_LEFT ] =1; }
				if(XK_s      == k)	{ LCSYS::g_InputKey[LCKEY_DOWN      ] =1; }
				if(XK_x      == k)	{ LCSYS::g_InputKey[LCKEY_DOWN_RIGHT] =1; }
				if(XK_d      == k)	{ LCSYS::g_InputKey[LCKEY_RIGHT     ] =1; }
				if(XK_e      == k)	{ LCSYS::g_InputKey[LCKEY_UP_RIGHT  ] =1; }
				if(XK_Shift_L== k)	{ LCSYS::g_InputKey[LCKEY_MENU      ] =1; }
				if(XK_Shift_R== k)	{ LCSYS::g_InputKey[LCKEY_SELECT    ] =1; }

				if(XK_Control_L==k)	{ LCSYS::g_InputKey[LCKEY_FL        ] =1; }
				if(XK_Control_R==k)	{ LCSYS::g_InputKey[LCKEY_FR        ] =1; }
				if(XK_Left   == k)	{ LCSYS::g_InputKey[LCKEY_FA        ] =1; }
				if(XK_Right  == k)	{ LCSYS::g_InputKey[LCKEY_FB        ] =1; }
				if(XK_Down   == k)	{ LCSYS::g_InputKey[LCKEY_FX        ] =1; }
				if(XK_Up     == k)	{ LCSYS::g_InputKey[LCKEY_FY        ] =1; }
				if(XK_Prior  == k)	{ LCSYS::g_InputKey[LCKEY_VOL_UP    ] =1; }
				if(XK_Next   == k)	{ LCSYS::g_InputKey[LCKEY_VOL_DOWN  ] =1; }
				if(XK_Tab    == k)	{ LCSYS::g_InputKey[LCKEY_TAT       ] =1; }
				if(XK_Home   == k)	{ LCSYS::g_InputKey[LCKEY_HOME      ] =1; }
				if(XK_End    == k)	{ LCSYS::g_InputKey[LCKEY_HOLD      ] =1; }
			}
			//printf("key press: x%x(x%x)\n", k, EventX.xkey.keycode);
		}

		else if(KeyRelease == EventX.type)
		{
			memset(buf, 0, sizeof buf);
			hr = XLookupString( (XKeyEvent*)&EventX, buf, 16, &k, NULL);

			//if(0<hr)
			{
				if(XK_w      == k)	{ LCSYS::g_InputKey[LCKEY_UP        ] =0; }
				if(XK_q      == k)	{ LCSYS::g_InputKey[LCKEY_UP_LEFT   ] =0; }
				if(XK_a      == k)	{ LCSYS::g_InputKey[LCKEY_LEFT      ] =0; }
				if(XK_z      == k)	{ LCSYS::g_InputKey[LCKEY_DOWN_LEFT ] =0; }
				if(XK_s      == k)	{ LCSYS::g_InputKey[LCKEY_DOWN      ] =0; }
				if(XK_x      == k)	{ LCSYS::g_InputKey[LCKEY_DOWN_RIGHT] =0; }
				if(XK_d      == k)	{ LCSYS::g_InputKey[LCKEY_RIGHT     ] =0; }
				if(XK_e      == k)	{ LCSYS::g_InputKey[LCKEY_UP_RIGHT  ] =0; }
				if(XK_Shift_L== k)	{ LCSYS::g_InputKey[LCKEY_MENU      ] =0; }
				if(XK_Shift_R== k)	{ LCSYS::g_InputKey[LCKEY_SELECT    ] =0; }

				if(XK_Control_L==k)	{ LCSYS::g_InputKey[LCKEY_FL        ] =0; }
				if(XK_Control_R==k)	{ LCSYS::g_InputKey[LCKEY_FR        ] =0; }
				if(XK_Left   == k)	{ LCSYS::g_InputKey[LCKEY_FA        ] =0; }
				if(XK_Right  == k)	{ LCSYS::g_InputKey[LCKEY_FB        ] =0; }
				if(XK_Down   == k)	{ LCSYS::g_InputKey[LCKEY_FX        ] =0; }
				if(XK_Up     == k)	{ LCSYS::g_InputKey[LCKEY_FY        ] =0; }
				if(XK_Prior  == k)	{ LCSYS::g_InputKey[LCKEY_VOL_UP    ] =0; }
				if(XK_Next   == k)	{ LCSYS::g_InputKey[LCKEY_VOL_DOWN  ] =0; }
				if(XK_Tab    == k)	{ LCSYS::g_InputKey[LCKEY_TAT       ] =0; }
				if(XK_Home   == k)	{ LCSYS::g_InputKey[LCKEY_HOME      ] =0; }
				if(XK_End    == k)	{ LCSYS::g_InputKey[LCKEY_HOLD      ] =0; }
			}
		}

	}

}//polling_input()


#endif




/*
enum ANDROID_KEY
{
	KEYCODE_SOFT_LEFT			= 1,
	KEYCODE_SOFT_RIGHT			= 2,
	KEYCODE_HOME				= 3,
	KEYCODE_BACK				= 4,
	KEYCODE_CALL				= 5,
	KEYCODE_ENDCALL				= 6,
	KEYCODE_0					= 7,
	KEYCODE_1					= 8,
	KEYCODE_2					= 9,
	KEYCODE_3					= 10,
	KEYCODE_4					= 11,
	KEYCODE_5					= 12,
	KEYCODE_6					= 13,
	KEYCODE_7					= 14,
	KEYCODE_8					= 15,
	KEYCODE_9					= 16,
	KEYCODE_STAR				= 17,
	KEYCODE_POUND				= 18,
	KEYCODE_DPAD_UP				= 19,
	KEYCODE_DPAD_DOWN			= 20,
	KEYCODE_DPAD_LEFT			= 21,
	KEYCODE_DPAD_RIGHT			= 22,
	KEYCODE_DPAD_CENTER			= 23,
	KEYCODE_VOLUME_UP			= 24,
	KEYCODE_VOLUME_DOWN			= 25,
	KEYCODE_POWER				= 26,
	KEYCODE_CAMERA				= 27,
	KEYCODE_CLEAR				= 28,
	KEYCODE_A					= 29,
	KEYCODE_B					= 30,
	KEYCODE_C					= 31,
	KEYCODE_D					= 32,
	KEYCODE_E					= 33,
	KEYCODE_F					= 34,
	KEYCODE_G					= 35,
	KEYCODE_H					= 36,
	KEYCODE_I					= 37,
	KEYCODE_J					= 38,
	KEYCODE_K					= 39,
	KEYCODE_L					= 40,
	KEYCODE_M					= 41,
	KEYCODE_N					= 42,
	KEYCODE_O					= 43,
	KEYCODE_P					= 44,
	KEYCODE_Q					= 45,
	KEYCODE_R					= 46,
	KEYCODE_S					= 47,
	KEYCODE_T					= 48,
	KEYCODE_U					= 49,
	KEYCODE_V					= 50,
	KEYCODE_W					= 51,
	KEYCODE_X					= 52,
	KEYCODE_Y					= 53,
	KEYCODE_Z					= 54,
	KEYCODE_COMMA				= 55,
	KEYCODE_PERIOD				= 56,
	KEYCODE_ALT_LEFT			= 57,
	KEYCODE_ALT_RIGHT			= 58,
	KEYCODE_SHIFT_LEFT			= 59,
	KEYCODE_SHIFT_RIGHT			= 60,
	KEYCODE_TAB					= 61,
	KEYCODE_SPACE				= 62,
	KEYCODE_SYM					= 63,
	KEYCODE_EXPLORER			= 64,
	KEYCODE_ENVELOPE			= 65,
	KEYCODE_ENTER				= 66,
	KEYCODE_DEL					= 67,
	KEYCODE_GRAVE				= 68,
	KEYCODE_MINUS				= 69,
	KEYCODE_EQUALS				= 70,
	KEYCODE_LEFT_BRACKET		= 71,
	KEYCODE_RIGHT_BRACKET		= 72,
	KEYCODE_BACKSLASH			= 73,
	KEYCODE_SEMICOLON			= 74,
	KEYCODE_APOSTROPHE			= 75,
	KEYCODE_SLASH				= 76,
	KEYCODE_AT					= 77,
	KEYCODE_NUM					= 78,
	KEYCODE_HEADSETHOOK			= 79,
	KEYCODE_FOCUS				= 80,
	KEYCODE_PLUS				= 81,
	KEYCODE_MENU				= 82,
	KEYCODE_NOTIFICATION		= 83,
	KEYCODE_SEARCH				= 84,
	KEYCODE_MEDIA_PLAY_PAUSE	= 85,
	KEYCODE_MEDIA_STOP			= 86,
	KEYCODE_MEDIA_NEXT			= 87,
	KEYCODE_MEDIA_PREVIOUS		= 88,
	KEYCODE_MEDIA_REWIND		= 89,
	KEYCODE_MEDIA_FAST_FORWARD	= 90,
	KEYCODE_MUTE				= 91,
	KEYCODE_PAGE_UP				= 92,
	KEYCODE_PAGE_DOWN			= 93,
	KEYCODE_PICTSYMBOLS			= 94,
	KEYCODE_SWITCH_CHARSET		= 95,
	KEYCODE_BUTTON_A			= 96,
	KEYCODE_BUTTON_B			= 97,
	KEYCODE_BUTTON_C			= 98,
	KEYCODE_BUTTON_X			= 99,
	KEYCODE_BUTTON_Y			= 100,
	KEYCODE_BUTTON_Z			= 101,
	KEYCODE_BUTTON_L1			= 102,
	KEYCODE_BUTTON_R1			= 103,
	KEYCODE_BUTTON_L2			= 104,
	KEYCODE_BUTTON_R2			= 105,
	KEYCODE_BUTTON_THUMBL		= 106,
	KEYCODE_BUTTON_THUMBR		= 107,
	KEYCODE_BUTTON_START		= 108,
	KEYCODE_BUTTON_SELECT		= 109,
	KEYCODE_BUTTON_MODE			= 110,
};

*/

