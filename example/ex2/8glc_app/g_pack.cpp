// JNIs for Android system
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcStdev.h>


#if defined(__ANDROID__)
#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif

extern jint Lc_JniGlueSetupSys		 (JNIEnv* env, jobject thiz, jstring _pck, jstring _cls);
extern jint Lc_JniGlueSetupTouch	 (JNIEnv* env, jobject thiz, jint idx, jint t, jint x, jint y);
extern jint Lc_JniGlueSetupKeypad	 (JNIEnv* env, jobject thiz, jint idx, jint t);
extern jint Lc_JniGlueSetupSensor	 (JNIEnv* env, jobject thiz, jint idx, jfloat x, jfloat y, jfloat z);
extern jint Lc_JniGlueAddStream		 (JNIEnv* env, jobject thiz, jstring rsc, jint rsId, jint streamId);
extern jint Lc_JniGlueFindStreamRscId(JNIEnv* env, jobject thiz, jint rsId);


// setup the java environment
jint Java_gpack_JniGlue_SetupSys(JNIEnv* env, jobject thiz, jstring _pck, jstring _cls)
{
	return Lc_JniGlueSetupSys(env, thiz, _pck, _cls);
}

// touch event process
jint Java_gpack_JniGlue_SetupTouch(JNIEnv* env, jobject thiz, jint idx, jint t, jint x, jint y)
{
	return Lc_JniGlueSetupTouch(env, thiz, idx, t, x, y);
}

// keypad event
jint Java_gpack_JniGlue_SetupKeypad(JNIEnv* env, jobject thiz, jint idx, jint t)
{
	return Lc_JniGlueSetupKeypad(env, thiz, idx, t);
}

// sensor process
jint Java_gpack_JniGlue_SetupSensor(JNIEnv* env, jobject thiz, jint idx, jfloat x, jfloat y, jfloat z)
{
	return Lc_JniGlueSetupSensor(env, thiz, idx, x, y, z);
}


// add the steam id to list
jint Java_gpack_JniGlue_AddStream(JNIEnv* env, jobject thiz, jstring rsc, jint rsId, jint streamId)
{
	return Lc_JniGlueAddStream(env, thiz, rsc, rsId, streamId);
}

// find stream id with resource id
jint Java_gpack_JniGlue_FindStreamRscId(JNIEnv* env, jobject thiz, jint rsId)
{
	return Lc_JniGlueFindStreamRscId(env, thiz, rsId);
}

#ifdef __cplusplus
}
#endif

#endif
