
// Copyright (C) 2010 Kajaani University of Applied Sciences (Kajaani AMK). See license.txt for license terms.
//
// Updated 21.2.2011


#include <framework/kajak3DFramework.h>
#include <framework/framework_functions.h>
#include <HAL/Input.h>
#include <HAL/Platform.h>

#include <jni.h>
#include <android/log.h>

#define MAX_PATH_LENGTH	256
#define TAG ANDROID_LOG_INFO,	"Kajak3DLib"
#define Logd __android_log_print

static jclass gameActivityCls = 0;
static JavaVM* jVM = 0;
static jobject gameActivityObj = 0;

static char apkFilePath[MAX_PATH_LENGTH];
static char storageDirectoryPath[MAX_PATH_LENGTH];

static core::Kajak3DConfig* myKajak3DConfig = 0;
static framework::Kajak3DFramework* myGameApp = 0;

extern "C" 
{

void shutdown( JNIEnv* env, jclass c, jobject thiz );
void playsound();

int JNI_OnLoad(JavaVM* vm, void* reserved)
{
	jVM = vm;
	JNIEnv* env;
	if ((vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK))
	{
		Logd(TAG, "Failed to get Java environment using GetEnv()");
		return -1;
	}

	return JNI_VERSION_1_4;
}

/**
* com.kajak3d.BaseGameActivity
*/
JNIEXPORT void JNICALL Java_com_kajak3d_BaseGameActivity_nativeCreateKajak3DConfig(JNIEnv* env, jclass thiz, jobject obj, jstring apkPath, jstring storageDir, jint width, jint height, jint bpp)
{
	if (!myKajak3DConfig)
	{
		jboolean isCopy;
		const char* str = env->GetStringUTFChars(apkPath, &isCopy);
		strncpy(apkFilePath, str, sizeof(apkFilePath)-1);
		env->ReleaseStringUTFChars(apkPath, str);

		str = env->GetStringUTFChars(storageDir, &isCopy);
		strncpy(storageDirectoryPath, str, sizeof(storageDirectoryPath)-1);
		env->ReleaseStringUTFChars(storageDir, str);
		
		Logd(TAG, "apk path: %s screen info: %d %d %d", apkFilePath, width, height, bpp);
		Logd(TAG, "storage directory: %s", storageDirectoryPath);

		myKajak3DConfig	= new core::Kajak3DConfig();
		myKajak3DConfig->HALConfig.displayWidth = width;
		myKajak3DConfig->HALConfig.displayHeight = height;
		myKajak3DConfig->HALConfig.displayColor = bpp;	
		myKajak3DConfig->HALConfig.apkFilePath = apkFilePath;
		myKajak3DConfig->HALConfig.appInternalStorageDirectory = storageDirectoryPath;

		// set width and height to displayconfig too
                myKajak3DConfig->displayConfig.width = width;
                myKajak3DConfig->displayConfig.height = height;


		// store global reference to game activity
		jVM->GetEnv((void**)&env, JNI_VERSION_1_4);
		gameActivityObj = (jobject) env->NewGlobalRef(obj);
		gameActivityCls = env->GetObjectClass(gameActivityObj);

		// store references to HAL too
		myKajak3DConfig->HALConfig.javaVMPtr = jVM;
		myKajak3DConfig->HALConfig.activityObjectPtr = gameActivityObj;
		myKajak3DConfig->HALConfig.activityClassPtr = gameActivityCls;
	}
	else
	{
		Logd(TAG, "Kajak3DConfig already exists.");
	}
}

/**
* com.kajak3d.AudioJNI
*
* store global references to soundmanager object
*/
JNIEXPORT void JNICALL Java_com_kajak3d_AudioJNI_setAudio( JNIEnv* env, jclass cls, jobject obj )
{
	jVM->GetEnv((void**)&env, JNI_VERSION_1_4);
	myKajak3DConfig->HALConfig.audioObjectPtr = env->NewGlobalRef(obj);
	myKajak3DConfig->HALConfig.audioClassPtr = env->GetObjectClass((jobject) myKajak3DConfig->HALConfig.audioObjectPtr);
	Logd(TAG, "setAudio() done. objectPtr %p classPtr %p", myKajak3DConfig->HALConfig.audioObjectPtr, myKajak3DConfig->HALConfig.audioClassPtr);
}

/**
* com.kajak3d.RendererJNI
*/
JNIEXPORT void JNICALL Java_com_kajak3d_RendererJNI_nativeRender( JNIEnv* env, jclass cls, jobject obj )
{
	if (myGameApp) 
	{
		framework::FrameworkUpdateReturnValue retval = myGameApp->update();
		if (retval == framework::FRAMEWORK_END)
		{
			shutdown(env, cls, obj);
		} 
		else
		{
			myGameApp->render();
		}
	}
}

JNIEXPORT void JNICALL Java_com_kajak3d_RendererJNI_nativeResize( JNIEnv* env, jobject thiz, jint w, jint h )
{
	Logd(TAG, "resize %d %d", w, h);
	HAL::Display::handleEvent(0, HAL::OS_WINDOW_SIZE_CHANGED, w, h);
}

JNIEXPORT void JNICALL Java_com_kajak3d_RendererJNI_nativeCreateKajak3DGameApp( JNIEnv* env, jobject cls )
{
	if (!myGameApp)
	{
		Logd(TAG, "initializing kajak3d");
		myGameApp = kajak3DMain(myKajak3DConfig);
		Logd(TAG, "kajak3d initialize done %p", myGameApp);			
	}
	else
	{
		Logd(TAG, "kajak was already initialized!");		
	}
}


JNIEXPORT void JNICALL Java_com_kajak3d_RendererJNI_nativePaused( JNIEnv* env, jobject cls )
{
	Logd(TAG, "nativePaused");
	if (myGameApp)
	{
		HAL::Display::handleEvent(0, HAL::OS_LOST_FOCUS,0,0);
		HAL::Display::handleEvent(0, HAL::OS_LOST_DEVICE,0,0);
		Logd(TAG, "focus and device lost.");
	}
}

JNIEXPORT void JNICALL Java_com_kajak3d_RendererJNI_nativeResumed( JNIEnv* env, jobject cls )
{
	Logd(TAG, "nativeResumed");
	if (myGameApp)
	{
		HAL::Display::handleEvent(0, HAL::OS_RESET_DEVICE,0,0);
		HAL::Display::handleEvent(0, HAL::OS_GET_FOCUS,0,0);
	}
}

/**
* com.kajak3d.TouchListenerJNI
*/
JNIEXPORT void JNICALL Java_com_kajak3d_TouchListenerJNI_nativeHandleTouchEvent(JNIEnv* env, jobject cls, jint event, jint pointerId, jint x, jint y)
{
	if (myGameApp)
	{
		HAL::androidTouchEventHandler(event, pointerId, x, y);
	}
}

JNIEXPORT void JNICALL Java_com_kajak3d_KeyEventJNI_nativeHandleKeyEvent(JNIEnv* env, jobject cls, jint keyCode, jint keyEvent)
{
	if (myGameApp)
	{
		Logd(TAG, "nativeHandleKeyEvent");
		HAL::androidButtonHandler(keyCode, keyEvent);
	}
}

/**
* com.kajak3d.AccelerometerEventListener
*/
JNIEXPORT void JNICALL Java_com_kajak3d_AccelerometerEventListener_nativeSensorEvent(JNIEnv* env, jobject cls, jint sensorId, jfloat x, jfloat y, jfloat z)
{
	if (myGameApp)
	{
		HAL::androidAccelerometerEventHandler(sensorId, x, y, z);
//	Logd(TAG, "accelerometer id %d xyz %f %f %f", sensorId, x, y, z);
	}
}

void shutdown( JNIEnv* env, jclass c, jobject thiz )
{
	jVM->GetEnv((void**)&env, JNI_VERSION_1_4);
	jmethodID mid = env->GetMethodID(gameActivityCls, "killApplication", "()V");
	if (!mid)
	{
		Logd(TAG, "error, killApplication method not found");
		return;
	}

	Logd(TAG, "freeing memory...");
	delete myGameApp;
	delete myKajak3DConfig;
	myGameApp = 0;
	Logd(TAG, "shutting down...");
	env->CallVoidMethod(gameActivityObj, mid);
}

}

