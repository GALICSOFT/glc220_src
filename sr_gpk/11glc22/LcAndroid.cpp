// Implementation Android System.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER > 1000
  #pragma warning( disable : 4786)
  #pragma warning( disable : 4996)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include "LcBase.h"
#include "LcAux.h"
#include "LcStdev.h"


///////////////////////////////////////////////////////////////////////////////
// JNIs for Android system

#if defined(__ANDROID__)
#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////

// setup the java environment
jint Lc_JniGlueSetupSys(JNIEnv* env, jobject thiz
								, jstring _pck, jstring _cls)
{
	INT   hr  = LC_OK;
	char* apk = (char*)env->GetStringUTFChars(_pck, NULL);
	char* cls = (char*)env->GetStringUTFChars(_cls, NULL);

	if(NULL ==apk || NULL == cls)
	{
		LOGE("setup the java environment failed\n");
		return LC_EFAIL;
	}


	hr = LcSys_Init(apk, cls, env);
	LOGW("setup the java environment::%d, %s, %s,  0x%p \n", hr, apk, cls, env);

	env->ReleaseStringUTFChars(_pck, (const char*)apk);
	env->ReleaseStringUTFChars(_pck, (const char*)cls);

	return hr;
}

// touch event process
jint Lc_JniGlueSetupTouch(JNIEnv* env, jobject thiz
								, jint idx, jint t, jint x, jint y)
{
	TLC_INPUT_TS v={x, y, t};
	//LOGI("JniGlue::Write Touch: %d %d %d %d ---------------", idx, x, y, t);
	LcStv_InputWriteMts(idx, &v);
	return LC_OK;
}


enum
{
	KEYCODE_HOME				= 3,
	KEYCODE_BACK				= 4,
	KEYCODE_VOLUME_UP			= 24,
	KEYCODE_VOLUME_DOWN			= 25,
	KEYCODE_MENU				= 82,
	KEYCODE_SEARCH				= 84,
};

// keypad event
jint Lc_JniGlueSetupKeypad(JNIEnv* env, jobject thiz
								, jint idx, jint t)
{
	int key = idx;
	switch(idx)
	{
		case KEYCODE_HOME		:	key = LCIK_HOME;	break;
		case KEYCODE_BACK		:	key = LCIK_BACK;	break;
		case KEYCODE_VOLUME_UP	:	key = LCIK_PAGEUP;	break;
		case KEYCODE_VOLUME_DOWN:	key = LCIK_PAGEDOWN;break;
		case KEYCODE_MENU		:	key = LCIK_F12;		break;
		case KEYCODE_SEARCH		:	key = LCIK_INSERT;	break;
	}

	//LOGI("JniGlue::WriteKey:%d, %d ---------------", key, t);
	LcStv_InputWriteKey(key, t);
	return LC_OK;
}


// G-sensor process
jint Lc_JniGlueSetupSensor(JNIEnv* env, jobject thiz
								, jint idx, jfloat x, jfloat y, jfloat z)
{
	TLC_SENSOR v={x, y, z, idx};
	//LOGI("JniGlue::Write Sensor: %d %f %f %f ---------------", idx, x, y, z);
	LcStv_SensorWrite(idx, &v);
	return LC_OK;
}



////////////////////////////////////////////////////////////////////////////////

// add the steam id to list
jint Lc_JniGlueAddStream(JNIEnv* env, jobject thiz
								, jstring rsc, jint rsId, jint streamId)
{
	char* str = (char*)env->GetStringUTFChars(rsc, NULL);
	char* name = NULL;

	if(!str)
		return LC_EFAIL;

	name = strrchr(str, '/'); ++name;

	JNI_OnAddStream(name, rsId, streamId, NULL);
	//LOGW("JniGlueAddStream: %s %2d %2d---\n",name,(int)rsId,(int)streamId);

	env->ReleaseStringUTFChars(rsc, (const char*)str);
	return LC_OK;
}

// find stream id with resource id
jint Lc_JniGlueFindStreamRscId(JNIEnv* env, jobject thiz
										, jint rsId)
{
	INT streamId = JNI_OnFindStreamRscId(rsId);
	return streamId;
}

#ifdef __cplusplus
}
#endif

#endif //__ANDROID__

