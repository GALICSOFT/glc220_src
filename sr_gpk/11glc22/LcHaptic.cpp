// Implementation of the Haptic moter.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#if defined(__ANDROID__)
  #include <assert.h>
  #include <jni.h>
#else
  typedef int		jint		;
  typedef void*		jclass		;
  typedef void*		jmethodID	;
  typedef void*		JavaVM		;
  typedef void*		JNIEnv		;

#endif

#include <LcType.h>

#include "LcBase.h"
#include "LcAux.h"
#include "LcStdev.h"


namespace LCSYS
{
	extern JavaVM*	g_pJavaVM	;
	extern jclass	g_JniClzz	;
}


////////////////////////////////////////////////////////////////////////////////


jmethodID Jni_HapticEnable	= {0};
jmethodID Jni_HapticPlay	= {0};
jmethodID Jni_HapticPattern	= {0};
jmethodID Jni_HapticStop	= {0};


INT LcStv_HapticOpen()
{
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

#if defined(_MSC_VER)

	//LOGE("Not Implemented in this Device\n");
	return LC_INVALID_DEVICE;

#elif defined(__ANDROID__)
	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("HapticOpen::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	Jni_HapticEnable   = pEnv->GetStaticMethodID(clzz, "JniCB_HapticEnable", "()I");
	if(0 == Jni_HapticEnable)
	{
		LOGE("JniCB_HapticEnable GetStaticMethodID Err!!! -------------------\n");
		return LC_EFAIL;
	}

	Jni_HapticPlay   = pEnv->GetStaticMethodID(clzz, "JniCB_HapticPlay", "(I)V");
	if(0 == Jni_HapticPlay)
	{
		LOGE("JniCB_HapticPlay GetStaticMethodID Err!!! -------------------\n");
		return LC_EFAIL;
	}

	Jni_HapticPattern   = pEnv->GetStaticMethodID(clzz, "JniCB_HapticPlayPattern", "([JI)V");
	if(0 == Jni_HapticPattern)
	{
		LOGE("JniCB_HapticPattern GetStaticMethodID Err!!! -------------------\n");
		return LC_EFAIL;
	}

	Jni_HapticStop   = pEnv->GetStaticMethodID(clzz, "JniCB_HapticStop", "()V" );
	if(0 == Jni_HapticStop)
	{
		LOGE("JniCB_HapticStop GetStaticMethodID Err!!!---------------------------\n");
		return LC_EFAIL;
	}

	LOGI("Haptic Environemnt Success");
	return LC_OK;
#endif

	return LC_EFAIL;
}


INT LcStv_HapticClose()
{
	return LC_OK;
}


INT LcStv_HapticEnable()
{
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

#if defined(_MSC_VER)

	//LOGE("Not Implemented in this Device\n");
	return LC_INVALID_DEVICE;

#elif defined(__ANDROID__)
	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("HapticOpen::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	pEnv->CallStaticVoidMethod(clzz, Jni_HapticEnable);

	return LC_OK;
#endif

	return LC_EFAIL;
}


INT LcStv_HapticPlay(INT millisec)
{
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

#if defined(_MSC_VER)

	//LOGE("Not Implemented in this Device\n");
	return LC_INVALID_DEVICE;

#elif defined(__ANDROID__)
	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("HapticOpen::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	pEnv->CallStaticVoidMethod(clzz, Jni_HapticPlay, (jint)millisec);
	return LC_OK ;
#endif
	return LC_EFAIL;
}


INT LcStv_HapticPattern(INT p[], INT n, INT r)
{
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

#if defined(_MSC_VER)

	//LOGE("Not Implemented in this Device\n");
	return LC_INVALID_DEVICE;

#elif defined(__ANDROID__)
	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("HapticOpen::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	INT i=0;

	if(LC_INFINITE == r)
		r = 0;		// infinite
	else
		r= -1;		// once


	jlongArray long_arr = pEnv->NewLongArray(n*2);

	for(i=0; i<n*2; ++i)
	{
		jlong t = p[i];
		pEnv->SetLongArrayRegion(long_arr, i, 1, &t);
	}	

	pEnv->CallStaticVoidMethod(clzz, Jni_HapticPattern, long_arr, r);

	for(i=0; i<n*2; ++i)
	{
		jlong t = p[i];
		pEnv->ReleaseLongArrayElements(long_arr, &t, 0);
	}

	return LC_OK;
#endif
	return LC_EFAIL;
}


INT LcStv_HapticStop()
{
	JNIEnv*	pEnv	= NULL;
	JavaVM*	pJvm	= LCSYS::g_pJavaVM;
	jclass	clzz	= LCSYS::g_JniClzz;

#if defined(_MSC_VER)

	//LOGE("Not Implemented in this Device\n");
	return LC_INVALID_DEVICE;

#elif defined(__ANDROID__)
	if(NULL == pJvm || 0 > pJvm->AttachCurrentThread(&pEnv, NULL))
	{
		LOGE("HapticOpen::Err::AttachCurrentThread::\n");
		return LC_EFAIL;
	}

	pEnv->CallStaticVoidMethod(clzz, Jni_HapticStop);
	return LC_OK;
#endif
	return LC_EFAIL;
}


/*

JNIEnv*		pEnv = pEnv;
jclass		clzz = LCSYS::g_JniCls;

jfieldID	act = (jfieldID)pEnv->GetStaticFieldID(clzz, "m_act"	, "Ljava/lang/Object;")

jclass		clzz = pEnv->GetObjectClass(act);

jmethodID	FuncGetSrv	= (jmethodID)pEnv->GetMethodID(clzz, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
jobject		HaticMng	= pEnv->CallObjectMethod(context, FuncGetSrv, pEnv->NewStringUTF("vibrator"));

jclass		HapticCls	= (jclass   )pEnv->FindClass("android/os/Vibrator");
jmethodID	FuncPlay	= (jmethodID)pEnv->GetMethodID(HapticCls, "vibrate", "(J)V");
jmethodID	FuncPattern	= (jmethodID)pEnv->GetMethodID(HapticCls, "vibrate", "([JI)V");
jmethodID	FuncStop	= (jmethodID)pEnv->GetMethodID(HapticCls, "cancel", "()V");


///////////////////////////////////////////////////////////////////////////////////////////////////
// play
jlong millsec = 1000;
pEnv->CallVoidMethod(HaticMng, FuncPlay, millsec);



///////////////////////////////////////////////////////////////////////////////////////////////////
// pattern play
jlongArray ptrn = pEnv->NewLongArray(4);


// set Vibrate Patterns
jlong pattern1 = 0;
jlong pattern2 = 300;
jlong pattern3 = 700;
jlong pattern4 = 1000;

pEnv->SetLongArrayRegion(ptrn, 0, 1, &pattern1);
pEnv->SetLongArrayRegion(ptrn, 1, 1, &pattern2);
pEnv->SetLongArrayRegion(ptrn, 2, 1, &pattern3);
pEnv->SetLongArrayRegion(ptrn, 2, 1, &pattern4);

pEnv->CallVoidMethod(HaticMng, FuncPattern, ptrn, 0);


// Release Elements
pEnv->ReleaseLongArrayElements(ptrn, &pattern1, 0);
pEnv->ReleaseLongArrayElements(ptrn, &pattern2, 0);
pEnv->ReleaseLongArrayElements(ptrn, &pattern3, 0);
pEnv->ReleaseLongArrayElements(ptrn, &pattern4, 0);


///////////////////////////////////////////////////////////////////////////////////////////////////
// stop
pEnv->CallVoidMethod(HaticMng, FuncStop);

*/
