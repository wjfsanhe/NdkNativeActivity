//=============================================================================
// FILE: svrApiCore.cpp
//
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#include <errno.h>
#include <fcntl.h>
#include <jni.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <jni.h>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"


#include "svrApi.h"
#include "svrCpuTimer.h"
#include "svrProfile.h"
#include "svrUtil.h"

#include "svrConfig.h"

#include "private/svrApiCore.h"
#include "private/svrApiHelper.h"
#include "private/svrApiPredictiveSensor.h"
#include "private/svrApiSensor.h"

#define QVRSERVICE_SDK_CONFIG_FILE  "sdk-config-file"

using namespace Svr;

// Surface Properties
VAR(int, gEyeBufferWidth, 1024, kVariableNonpersistent);            //Value returned as recommended eye buffer width in svrDeviceInfo
VAR(int, gEyeBufferHeight, 1024, kVariableNonpersistent);           //Value returned as recommended eye buffer height in svrDeviceInfo
VAR(float, gEyeBufferFovX, 90.0f, kVariableNonpersistent);          //Value returned as recommended FOV X in svrDeviceInfo
VAR(float, gEyeBufferFovY, 90.0f, kVariableNonpersistent);          //Value returned as recommended FOV Y in svrDeviceInfo

// TimeWarp Properties
VAR(bool, gDisableReprojection, false, kVariableNonpersistent);     //Override to disable reprojection
VAR(bool, gDisablePredictedTime, false, kVariableNonpersistent);    //Forces svrGetPredictedDisplayTime to return 0.0
VAR(int, gRenderThreadCore, 3, kVariableNonpersistent);             //Core id to set render thread affinity for (-1 disables affinity)
VAR(bool, gEnableRenderThreadFifo, false, kVariableNonpersistent);  //Enable/disable setting SCHED_FIFO scheduling policy on the render thread thread
VAR(int, gForceMinVsync, 0, kVariableNonpersistent);                //Override for svrFrameParams minVsync option (0:override disabled, 1 or 2 forced value)
VAR(bool, gUseLinePtr, true, kVariableNonpersistent);               //Override for using the linePtr interrupt, if set to false Choreographer will be used instead


//Symphony power brackets for performance levels
VAR(int, gForceCpuLevel, -1, kVariableNonpersistent);                //Override to force CPU performance level (-1: app defined, 0:system defined/off, 1/2/3 for min,medium,max)
VAR(int, gCpuLvl1Min, 30, kVariableNonpersistent);                  //Lower CPU frequency (percentage) bound for min performance level
VAR(int, gCpuLvl1Max, 50, kVariableNonpersistent);                  //Upper CPU frequency (percentage) bound for min performance level
VAR(int, gCpuLvl2Min, 51, kVariableNonpersistent);                  //Lower CPU frequency (percentage) bound for medium performance level
VAR(int, gCpuLvl2Max, 80, kVariableNonpersistent);                  //Upper CPU frequency (percentage) bound for medium performance level
VAR(int, gCpuLvl3Min, 81, kVariableNonpersistent);                  //Lower CPU frequency (percentage) bound for max performance level
VAR(int, gCpuLvl3Max, 100, kVariableNonpersistent);                 //Upper CPU frequency (percentage) bound for max performance level  

VAR(int, gForceGpuLevel, -1, kVariableNonpersistent);                //Override to force GPU performance level (-1: app defined, 0:system defined/off, 1/2/3 for min,medium,max)
VAR(int, gGpuLvl1Min, 30, kVariableNonpersistent);                  //Lower GPU frequency (percentage) bound for min performance level
VAR(int, gGpuLvl1Max, 50, kVariableNonpersistent);                  //Upper GPU frequency (percentage) bound for min performance level
VAR(int, gGpuLvl2Min, 51, kVariableNonpersistent);                  //Lower GPU frequency (percentage) bound for medium performance level
VAR(int, gGpuLvl2Max, 80, kVariableNonpersistent);                  //Upper GPU frequency (percentage) bound for medium performance level
VAR(int, gGpuLvl3Min, 81, kVariableNonpersistent);                  //Lower GPU frequency (percentage) bound for max performance level
VAR(int, gGpuLvl3Max, 100, kVariableNonpersistent);                 //Upper GPU frequency (percentage) bound for max performance level  

//Tracking overrides
VAR(int, gForceTrackingMode, 0, kVariableNonpersistent);            //Force a specific tracking mode 1 = rotational 3 = rotational & positional

//Log options
VAR(float, gLogLinePtrDelay, 0.0, kVariableNonpersistent);          //Log line ptr delays longer greater than this value (0.0 = disabled)
VAR(bool, gLogSubmitFps, false, kVariableNonpersistent);            //Enables output of submit FPS to LogCat

//Debug Server options
VAR(bool, gEnableDebugServer, false, kVariableNonpersistent);       //Enables a very basic json-rpc server for interacting with vr while running

//Debug toggles
VAR(bool, gDisableFrameSubmit, false, kVariableNonpersistent);      //Debug flag that will prevent the eye buffer render thread from submitted frames to time warp


int gFifoPriorityRender = 96;
int gNormalPriorityRender = 0;      // Cause they want something :)

int gRecenterTransition = 1000;

namespace Svr
{
    SvrAppContext* gAppContext = NULL;
}

static const char* gSvrConfigFilePath = "/data/misc/vr/svrapi_config.txt";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

extern "C"
{
    void Java_com_qualcomm_svrapi_SvrApi_nativeVsync(JNIEnv *jni, jclass clazz, jlong frameTimeNanos)
    { 
        if (gAppContext == NULL){
            LOGE("======================");
            return;
        }
        const double periodNano =  1e9 /gAppContext->deviceInfo.displayRefreshRateHz;
        
        if (gAppContext != NULL && gAppContext->modeContext != NULL)
        {
            pthread_mutex_lock(&gAppContext->modeContext->vsyncMutex);

            uint64_t vsyncTimeStamp = frameTimeNanos;
           
            if (gAppContext->modeContext->vsyncTimeNano == 0)
            {
                //Don't count the first time through
                gAppContext->modeContext->vsyncTimeNano = vsyncTimeStamp;
                gAppContext->modeContext->vsyncCount = 1;
            }
            else
            {
                unsigned int nVsync = floor(0.5 + ((double)(vsyncTimeStamp - gAppContext->modeContext->vsyncTimeNano) / periodNano));
                gAppContext->modeContext->vsyncCount += nVsync;
                gAppContext->modeContext->vsyncTimeNano = vsyncTimeStamp;
            }

            pthread_mutex_unlock(&gAppContext->modeContext->vsyncMutex);

        }   // gAppContext != NULL
    }
}


//-----------------------------------------------------------------------------
static void svrLinePtrCallback(void *ctx, uint64_t vsync_ts)
//-----------------------------------------------------------------------------
{
    const double periodNano = 1e9 / gAppContext->deviceInfo.displayRefreshRateHz;

    //Make sure we're getting these callbacks in time...
    uint64_t cbkTimeStamp = Svr::GetTimeNano();
    float msDelay = (((double)(cbkTimeStamp - vsync_ts)) * 1e-6);
    int delayed = 0;
    if (gLogLinePtrDelay > 0.0 && msDelay > gLogLinePtrDelay)
    {
        LOGE("svrLinePtrCallback: %3.3f delayed", msDelay);
        delayed = 1;
    }
    PROFILE_ENTER(0, 0, "svrLinePtrCb %d Delayed %d", (int)(gAppContext->modeContext->vsyncCount) % 8 + 1, delayed);

    uint64_t vsyncTimeStamp = vsync_ts;

    //LOGI("Vsync : %llu", gAppContext->modeContext->vsyncCount);

    pthread_mutex_lock(&gAppContext->modeContext->vsyncMutex);
    {
        if (gAppContext->modeContext->vsyncTimeNano == 0)
        {
            //Don't count the first time through
            gAppContext->modeContext->vsyncTimeNano = vsyncTimeStamp;
            gAppContext->modeContext->vsyncCount = 1;
        }
        else
        {
            unsigned int nVsync = floor(0.5 + ((double)(vsyncTimeStamp - gAppContext->modeContext->vsyncTimeNano) / periodNano));
            gAppContext->modeContext->vsyncCount += nVsync;
            gAppContext->modeContext->vsyncTimeNano = vsyncTimeStamp;
        }
    }
    pthread_mutex_unlock(&gAppContext->modeContext->vsyncMutex);
    PROFILE_FREE(0, 0);
}

#ifdef USE_QVR_SERVICE
//-----------------------------------------------------------------------------
void svrNotifyFailedQvrService()
//-----------------------------------------------------------------------------
{
    // Find the method ...	
	JNIEnv* pThreadJEnv;
	if (gAppContext->javaVm->AttachCurrentThread(&pThreadJEnv, NULL) != JNI_OK)
	{
		LOGE("svrInitialize AttachCurrentThread failed.");
    }
	
    jclass ParentClass = gAppContext->javaSvrApiClass;
    jmethodID MethodId = pThreadJEnv->GetStaticMethodID(ParentClass, "NotifyNoVr", "(Landroid/app/Activity;)V");
    if (MethodId == NULL)
    {
        LOGE("Unable to find Method: SvrNativeActivity::NotifyNoVr()");
        return;
    }

    // ... call the method
    LOGI("Method Called: SvrNativeActivity::NotifyNoVr()...");
    pThreadJEnv->CallStaticVoidMethod(ParentClass, MethodId, gAppContext->javaActivityObject);
}
#endif // USE_QVR_SERVICE

//-----------------------------------------------------------------------------
int svrGetAndroidOSVersion()
//-----------------------------------------------------------------------------
{
    char fileBuf[256];

    FILE* pFile = popen("getprop ro.build.version.sdk", "r");
    if (!pFile)
    {
        LOGE("Failed to getprop on 'ro.build.version.sdk'");
        return -1;
    }
    fgets(fileBuf, 256, pFile);

    int version;
    sscanf(fileBuf, "%d", &version);
    pclose(pFile);

    return version;
}

//-----------------------------------------------------------------------------
void L_SetThreadPriority(const char *pName, int policy, int priority)
//-----------------------------------------------------------------------------
{

    // What is the current thread policy?
    int oldPolicy = sched_getscheduler(gettid());
    switch (oldPolicy)
    {
    case SCHED_NORMAL:      // 0
        LOGI("Current %s (0x%x = %d) Scheduling policy: SCHED_NORMAL", pName, (int)gettid(), (int)gettid());
        break;

    case SCHED_FIFO:        // 1
        LOGI("Current %s (0x%x = %d) Scheduling policy: SCHED_FIFO", pName, (int)gettid(), (int)gettid());
        break;

    case SCHED_FIFO | SCHED_RESET_ON_FORK:        // 1
        LOGI("Current %s (0x%x = %d) Scheduling policy: SCHED_FIFO | SCHED_RESET_ON_FORK", pName, (int)gettid(), (int)gettid());
        break;

    case SCHED_RR:          // 2
        LOGI("Current %s (0x%x = %d) Scheduling policy: SCHED_RR", pName, (int)gettid(), (int)gettid());
        break;

    case SCHED_BATCH:       // 3
        LOGI("Current %s (0x%x = %d) Scheduling policy: SCHED_BATCH", pName, (int)gettid(), (int)gettid());
        break;

        // case SCHED_ISO:         // 4
        //     LOGI("Current %s (0x%x = %d) Scheduling policy: SCHED_ISO", pName, (int)gettid(), (int)gettid());
        //     break;

    case SCHED_IDLE:        // 5
        LOGI("Current %s (0x%x = %d) Scheduling policy: SCHED_IDLE", pName, (int)gettid(), (int)gettid());
        break;

    default:
        LOGI("Current %s (0x%x = %d) Scheduling policy: %d", pName, (int)gettid(), (int)gettid(), oldPolicy);
        break;
    }

    // Where is it going?
    switch (policy)
    {
    case SCHED_NORMAL:      // 0
        LOGI("    Setting => SCHED_NORMAL");
        break;

    case SCHED_FIFO:        // 1
        LOGI("    Setting => SCHED_FIFO");
        break;

    case SCHED_FIFO | SCHED_RESET_ON_FORK:        // 1
        LOGI("    Setting => SCHED_FIFO | SCHED_RESET_ON_FORK");
        break;
        
    case SCHED_RR:          // 2
        LOGI("    Setting => SCHED_RR");
        break;

    case SCHED_BATCH:       // 3
        LOGI("    Setting => SCHED_BATCH");
        break;

        // case SCHED_ISO:         // 4
        //     LOGI("    Setting => SCHED_ISO");
        //     break;

    case SCHED_IDLE:        // 5
        LOGI("    Setting => SCHED_IDLE");
        break;

    default:
        LOGI("    Setting => UNKNOWN! (%d)", policy);
        break;
    }
    
    if (gAppContext->qvrService->SetThreadPriority(gettid(), policy, priority) < 0)
    {
        LOGE("Error setting thread priority!");
    }

    // What was the result?
    int newPolicy = sched_getscheduler(gettid());
    switch (newPolicy)
    {
    case SCHED_NORMAL:      // 0
        LOGI("    Result => SCHED_NORMAL");
        break;

    case SCHED_FIFO:        // 1
        LOGI("    Result => SCHED_FIFO");
        break;

    case SCHED_FIFO | SCHED_RESET_ON_FORK:        // 1
        LOGI("    Result => SCHED_FIFO | SCHED_RESET_ON_FORK");
        break;
        
    case SCHED_RR:          // 2
        LOGI("    Result => SCHED_RR");
        break;

    case SCHED_BATCH:       // 3
        LOGI("    Result => SCHED_BATCH");
        break;

        // case SCHED_ISO:         // 4
        //     LOGI("    Result => SCHED_ISO");
        //     break;

    case SCHED_IDLE:        // 5
        LOGI("    Result => SCHED_IDLE");
        break;

    default:
        LOGI("    Result => UNKNOWN! (%d)", newPolicy);
        break;
    }
}

//-----------------------------------------------------------------------------
bool svrInitialize(const svrInitParams* pInitParams)
//-----------------------------------------------------------------------------
{
    LOGI("svrApi Version : %s", svrGetVersion());

    gAppContext = new SvrAppContext();
    gAppContext->qvrService = NULL;

    gAppContext->inVrMode = false;

    gAppContext->looper = ALooper_forThread();
    gAppContext->javaVm = pInitParams->javaVm;
    gAppContext->javaEnv = pInitParams->javaEnv;
    gAppContext->javaActivityObject = pInitParams->javaActivityObject;

/*    //Initialize render extensions
    *//*if (!InitializeRenderExtensions())
    {
        LOGE("Failed to initialize required EGL/GL extensions");
        return false;
    }
    else
    {
        LOGI("EGL/GL Extensions Initialized");
    }*//*

    //Load the SvrApi Java class and cache necessary method references
    //Since we are utilizing a native activity we need to go through the activities class loader to find the SvrApi class
        
	JNIEnv* pThreadJEnv;
	if (gAppContext->javaVm->AttachCurrentThread(&pThreadJEnv, NULL) != JNI_OK)
	{
		LOGE("svrInitialize AttachCurrentThread failed.");
        return false;
    }

    jclass tmpActivityClass = pThreadJEnv->FindClass("android/app/NativeActivity");
    jclass activityClass = (jclass)pThreadJEnv->NewGlobalRef(tmpActivityClass);

    jmethodID getClassLoaderMethodId = pThreadJEnv->GetMethodID(activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject classLoaderObj = pThreadJEnv->CallObjectMethod(gAppContext->javaActivityObject, getClassLoaderMethodId);

    jclass tmpClassLoaderClass = pThreadJEnv->FindClass("java/lang/ClassLoader");
    jclass classLoader = (jclass)pThreadJEnv->NewGlobalRef(tmpClassLoaderClass);

    jmethodID findClassMethodId = pThreadJEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    //Get a reference to the SvrApi class
    jstring strClassName = pThreadJEnv->NewStringUTF("com/qualcomm/svrapi/SvrApi");
    jclass tmpjavaSvrApiClass = (jclass)pThreadJEnv->CallObjectMethod(classLoaderObj, findClassMethodId, strClassName);
    gAppContext->javaSvrApiClass = (jclass)pThreadJEnv->NewGlobalRef(tmpjavaSvrApiClass);

    if (gAppContext->javaSvrApiClass == NULL)
    {
        LOGE("Failed to initialzie SvrApi Java class");
        return false;
    }

    //Register our native methods
    struct
    {
        jclass          clazz;
        JNINativeMethod nm;
    } nativeMethods[] =
    {
        { gAppContext->javaSvrApiClass, { "nativeVsync", "(J)V", (void*)Java_com_qualcomm_svrapi_SvrApi_nativeVsync } }
    };

    const int count = sizeof(nativeMethods) / sizeof(nativeMethods[0]);

    for (int i = 0; i < count; i++)
    {
        if (pThreadJEnv->RegisterNatives(nativeMethods[i].clazz, &nativeMethods[i].nm, 1) != JNI_OK)
        {
            LOGE("Failed to register %s", nativeMethods[i].nm.name);
            return false;
        }
    }

    //Cache method ids for the start/stop vsync callback methods
    gAppContext->javaSvrApiStartVsyncMethodId = pThreadJEnv->GetStaticMethodID(gAppContext->javaSvrApiClass,
        "startVsync", "(Landroid/app/Activity;)V");
    if (gAppContext->javaSvrApiStartVsyncMethodId == NULL)
    {
        LOGE("Failed to locate startVsync method");
        return false;
    }

    gAppContext->javaSvrApiStopVsyncMethodId = pThreadJEnv->GetStaticMethodID(gAppContext->javaSvrApiClass,
        "stopVsync", "(Landroid/app/Activity;)V");
    if (gAppContext->javaSvrApiStopVsyncMethodId == NULL)
    {
        LOGE("Failed to locate stopVsync method");
        return false;
    }

    //Gather information about the device
    memset(&gAppContext->deviceInfo, 0, sizeof(svrDeviceInfo));

    jmethodID refreshRateId = pThreadJEnv->GetStaticMethodID(gAppContext->javaSvrApiClass,
        "getRefreshRate", "(Landroid/app/Activity;)F");
    if (refreshRateId == NULL)
    {
        LOGE("svrGetDeviceInfo: Failed to locate getRefreshRate method");
        return false;
    }
    else
    {
        jfloat refreshRate = pThreadJEnv->CallStaticFloatMethod(gAppContext->javaSvrApiClass, refreshRateId, gAppContext->javaActivityObject);
        gAppContext->deviceInfo.displayRefreshRateHz = refreshRate;
        LOGI("Display Refresh : %f", refreshRate);
    }

    jmethodID getDisplayWidthId = pThreadJEnv->GetStaticMethodID(gAppContext->javaSvrApiClass,
        "getDisplayWidth", "(Landroid/app/Activity;)I");
    if (getDisplayWidthId == NULL)
    {
        LOGE("Failed to locate getDisplayWidth method");
        return false;
    }
    else
    {
        jint displayWidth = pThreadJEnv->CallStaticIntMethod(gAppContext->javaSvrApiClass, getDisplayWidthId, gAppContext->javaActivityObject);
        gAppContext->deviceInfo.displayWidthPixels = displayWidth;
        LOGI("Display Width : %d", displayWidth);
    }

    jmethodID getDisplayHeightId = pThreadJEnv->GetStaticMethodID(gAppContext->javaSvrApiClass,
        "getDisplayHeight", "(Landroid/app/Activity;)I");
    if (getDisplayHeightId == NULL)
    {
        LOGE("Failed to locate getDisplayHeight method");
        return false;
    }
    else
    {
        jint displayHeight = pThreadJEnv->CallStaticIntMethod(gAppContext->javaSvrApiClass, getDisplayHeightId, gAppContext->javaActivityObject);
        gAppContext->deviceInfo.displayHeightPixels = displayHeight;
        LOGI("Display Height : %d", displayHeight);
    }

   
    jmethodID getDisplayOrientationId = pThreadJEnv->GetStaticMethodID(gAppContext->javaSvrApiClass,
        "getDisplayOrientation", "(Landroid/app/Activity;)I");
    if (getDisplayOrientationId == NULL)
    {
        LOGE("Failed to locate getDisplayOrientation method");
        return false;
    }
    else
    {
        jint displayOrientation = 270;
        displayOrientation = pThreadJEnv->CallStaticIntMethod(gAppContext->javaSvrApiClass, getDisplayOrientationId, gAppContext->javaActivityObject);
        gAppContext->deviceInfo.displayOrientation = displayOrientation;
        LOGI("Display Orienation : %d", displayOrientation);
    }*/

    //Get the current OS version
    int osVersion = svrGetAndroidOSVersion();
    LOGI("Device OS Version = %d", osVersion);

    gAppContext->currentTrackingMode = 0;

#ifdef USE_QVR_SERVICE
    //Connect to the QVR Service
    LOGI("Connecting to QVR Service...");
    gAppContext->qvrService = new QVRServiceClient();
    if (gAppContext->qvrService == NULL || gAppContext->qvrService->GetVRMode() == VRMODE_UNSUPPORTED)
    {
        LOGE("    QVR Serivce reported VR not supported");
        svrNotifyFailedQvrService();
        return false;
    }

    //Get the supported tracking modes
    unsigned int supportedTrackingModes = svrGetSupportedTrackingModes();
    if (supportedTrackingModes & kTrackingRotation)
    {
        LOGI("  QVR Service supports rotational tracking");
    }
    if (supportedTrackingModes & kTrackingPosition)
    {
        LOGI("  QVR Service supports positional tracking");
    }

    //Load SVR configuration options
    if (osVersion >= 24)
    {
        //If we are on Android-N or greater we need to get the configuration from
        //the QVR service rather than loading it from the sdcard
        unsigned int len = 0;
        int r = gAppContext->qvrService->GetParam(QVRSERVICE_SDK_CONFIG_FILE, &len, NULL);
        if (r == 0)
        {
            LOGI("Loading variables from QVR Service [len=%d]", len);
            if (len > 0)
            {
                char *p = new char[len];
                r = gAppContext->qvrService->GetParam(QVRSERVICE_SDK_CONFIG_FILE, &len, p);
                //LoadVariableBuffer(p);
                delete[] p;
            }
        }
        else
        {
            LOGE("QVR Service GetParam 'QVRSERVICE_SDK_CONFIG_FILE' Failed");
        }
    }
    else
    {
//        LoadVariableFile(gSvrConfigFilePath);
    }
#else
    LoadVariableFile(gSvrConfigFilePath);
#endif

    //On Android N we can't use line lineptr interrupt from the QVR service for tracking
    //Vsync, instead force the use of Choreographer
    if (osVersion >= 24)
    {
        LOGI("Overriding config file and forcing Choreographer for Android N or greater");
        gUseLinePtr = false;
    }

    //Set the default tracking mode to rotational only
    // Need to be AFTER reading the config file so overrides will work
    svrSetTrackingMode(kTrackingRotation);

    //Set other device info
    /*gAppContext->deviceInfo.deviceOSVersion = osVersion;
    gAppContext->deviceInfo.targetEyeWidthPixels = gEyeBufferWidth;
    gAppContext->deviceInfo.targetEyeHeightPixels = gEyeBufferHeight;
    gAppContext->deviceInfo.targetFovXRad = gEyeBufferFovX * DEG_TO_RAD;
    gAppContext->deviceInfo.targetFovYRad = gEyeBufferFovY * DEG_TO_RAD;*/

/*    //Log out some useful information
    jmethodID vsyncOffetId = pThreadJEnv->GetStaticMethodID(gAppContext->javaSvrApiClass,
        "getVsyncOffsetNanos", "(Landroid/app/Activity;)J");
    if (vsyncOffetId == NULL)
    {
        LOGE("Failed to locate getVsyncOffsetNanos method");
    }
    else
    {
        jlong result = pThreadJEnv->CallStaticLongMethod(gAppContext->javaSvrApiClass, vsyncOffetId, gAppContext->javaActivityObject);
        LOGI("Vsync Offset : %d", (int)result);
    }*/

/*    if (gDisableReprojection)
    {
        LOGI("Timewarp disabled from configuration file");
    }

    if (gForceMinVsync > 0)
    {
        LOGI("Forcing minVsync = %d", gForceMinVsync);
    }*/



    return true;
}

//-----------------------------------------------------------------------------
void svrShutdown()
//-----------------------------------------------------------------------------
{
    if (gAppContext != NULL)
    {
#ifdef USE_QVR_SERVICE
        delete gAppContext->qvrService;
#endif // USE_QVR_SERVICE

        delete gAppContext;
        gAppContext = NULL;
    }
}

//-----------------------------------------------------------------------------
svrDeviceInfo svrGetDeviceInfo()
//-----------------------------------------------------------------------------
{
    if (gAppContext != NULL)
    {
        return gAppContext->deviceInfo;
    }
    else
    {
        LOGE("Called svrGetDeviceInfo without initialzing svr");
        svrDeviceInfo tmp;
        memset(&tmp, 0, sizeof(svrDeviceInfo));
        return tmp;
    }
}

//-----------------------------------------------------------------------------
void svrSetCpuPerfLevel(svrPerfLevel level)
//-----------------------------------------------------------------------------
{
    int cpuMin = gCpuLvl2Min;
    int cpuMax = gCpuLvl2Max;

    if (level == kPerfSystem)
    {
        LOGI("Setting System Defined CPU Power Mode");

    }
    else
    {
        if (level == kPerfMinimum)
        {
            cpuMin = gCpuLvl1Min;
            cpuMax = gCpuLvl1Max;
        }
        else if (level == kPerfMedium)
        {
            cpuMin = gCpuLvl2Min;
            cpuMax = gCpuLvl2Max;
        }
        else if (level == kPerfMaximum)
        {
            cpuMin = gCpuLvl3Min;
            cpuMax = gCpuLvl3Max;
        }

        LOGI("Setting Explicit CPU Power Mode [%d. %d]", cpuMin, cpuMax);
    }
}

//-----------------------------------------------------------------------------
void svrSetGpuPerfLevel(svrPerfLevel level)
//-----------------------------------------------------------------------------
{
    int gpuMin = gGpuLvl2Min;
    int gpuMax = gGpuLvl2Max;

    if (level == kPerfSystem)
    {
        LOGI("Setting System Defined GPU Power Mode");
    }
    else
    {
        if (level == kPerfMinimum)
        {
            gpuMin = gGpuLvl1Min;
            gpuMax = gGpuLvl1Max;
        }
        else if (level == kPerfMedium)
        {
            gpuMin = gGpuLvl2Min;
            gpuMax = gGpuLvl2Max;
        }
        else if (level == kPerfMaximum)
        {
            gpuMin = gGpuLvl3Min;
            gpuMax = gGpuLvl3Max;
        }


    }
}

//-----------------------------------------------------------------------------
void svrBeginVr(const svrBeginParams* pBeginParams)
//-----------------------------------------------------------------------------
{
    LOGI("svrBeginVr");

#if defined (USE_QVR_SERVICE)
    if(gAppContext->qvrService == NULL)
    {
        LOGE("svrBeginVr Failed: SnapdragonVR not initialized!");
        return;
    }
#endif // defined (USE_QVR_SERVICE)

    PROFILE_INITIALIZE();

    PROFILE_THREAD_NAME(gTelemetryData.context, 0, "Eye Render Thread");
/*
    if (gEnableRenderThreadFifo)
    {
        L_SetThreadPriority("Render Thread", SCHED_FIFO | SCHED_RESET_ON_FORK, gFifoPriorityRender);
    }

    if (gRenderThreadCore >= 0)
    {
        LOGI("Setting Eye Render Affinity to %d", gRenderThreadCore);
        svrSetThreadAffinity(gRenderThreadCore);
    }
  
    LOGI("Setting CPU/GPU Performance Levels...");
    if (gForceCpuLevel < 0)
    {
        svrSetCpuPerfLevel(pBeginParams->cpuPerfLevel);
    }
    else
    {
        svrSetCpuPerfLevel((svrPerfLevel)gForceCpuLevel);
    }

    if (gForceGpuLevel < 0)
    {
        svrSetGpuPerfLevel(pBeginParams->gpuPerfLevel);
    }
    else
    {
        svrSetGpuPerfLevel((svrPerfLevel)gForceGpuLevel);
    }*/
	
	//Set currently selected tracking mode. This is needed when the application resumes from suspension
    LOGI("Set tracking mode context...");
    svrSetTrackingMode(gAppContext->currentTrackingMode);

    LOGI("Creating mode context...");
    gAppContext->modeContext = new SvrModeContext();
    
    gAppContext->modeContext->nativeWindow = pBeginParams->nativeWindow;

    gAppContext->modeContext->vsyncCount = 0;
    gAppContext->modeContext->vsyncTimeNano = 0;
    pthread_mutex_init(&gAppContext->modeContext->vsyncMutex, NULL);

    pthread_cond_init(&gAppContext->modeContext->warpThreadContextCv, NULL);
    pthread_mutex_init(&gAppContext->modeContext->warpThreadContextMutex, NULL);
    gAppContext->modeContext->warpContextCreated = false;

    gAppContext->modeContext->warpThreadExit = false; 
    gAppContext->modeContext->vsyncThreadExit = false;
  
    pthread_cond_init(&gAppContext->modeContext->warpBufferConsumedCv, NULL);
    pthread_mutex_init(&gAppContext->modeContext->warpBufferConsumedMutex, NULL);
  
    gAppContext->modeContext->eyeRenderWarpSurface = EGL_NO_SURFACE;
    gAppContext->modeContext->eyeRenderOrigSurface = EGL_NO_SURFACE;
    gAppContext->modeContext->eyeRenderOrigConfigId = -1;
    gAppContext->modeContext->eyeRenderContext = EGL_NO_CONTEXT;
    gAppContext->modeContext->warpRenderContext = EGL_NO_CONTEXT;
    gAppContext->modeContext->warpRenderSurface = EGL_NO_SURFACE;
    gAppContext->modeContext->warpRenderSurfaceWidth = 0;
    gAppContext->modeContext->warpRenderSurfaceHeight = 0;

    //Initialize the warp frame param structures
    memset(&gAppContext->modeContext->frameParams[0], 0, sizeof(svrFrameParamsInternal) * NUM_SWAP_FRAMES);
    gAppContext->modeContext->submitFrameCount = 0;
    gAppContext->modeContext->warpFrameCount = 0;
    gAppContext->modeContext->prevSubmitVsyncCount = 0;
   
    // Recenter rotation
    gAppContext->modeContext->recenterRot = glm::fquat();
    gAppContext->modeContext->recenterPos = glm::vec3(0.0f, 0.0f, 0.0f);

    //Start Vsync monitoring
    LOGI("Starting VSync Monitoring...");

#if defined (USE_QVR_SERVICE)
    if (gUseLinePtr)
    {
        LOGI("Configuring Line Pointer Interrupt...");
        //Utilize the VR Service Vsync callback for tracking HW Vsync
        qvrservice_lineptr_interrupt_config_t linePtr0Config;
        memset(&linePtr0Config, 0, sizeof(qvrservice_lineptr_interrupt_config_t));
        linePtr0Config.cb = svrLinePtrCallback;
        linePtr0Config.ctx = gAppContext;
        linePtr0Config.line = 1;
        int res = gAppContext->qvrService->SetDisplayInterruptConfig(DISP_INTERRUPT_LINEPTR, &linePtr0Config, sizeof(qvrservice_lineptr_interrupt_config_t));
        LOGI("   QVR Service Line Pointer Configured [%d]", res);
    }
    else
    {
       /* //Utilize Choreographer for tracking HW Vsync
        JNIEnv* pThreadJEnv;
        if (gAppContext->javaVm->AttachCurrentThread(&pThreadJEnv, NULL) != JNI_OK)
        {
            LOGE("    svrBeginVr AttachCurrentThread failed.");
        }

        LOGI("   Using Choreographer VSync Monitoring");
        pThreadJEnv->CallStaticVoidMethod(gAppContext->javaSvrApiClass, gAppContext->javaSvrApiStartVsyncMethodId, gAppContext->javaActivityObject);*/
    }
#else
    //Utilize Choreographer for tracking HW Vsync
    JNIEnv* pThreadJEnv;
    if (gAppContext->javaVm->AttachCurrentThread(&pThreadJEnv, NULL) != JNI_OK)
    {
        LOGE("    svrBeginVr AttachCurrentThread failed.");
    }
        
    LOGI("   Using Choreographer VSync Monitoring");
    pThreadJEnv->CallStaticVoidMethod(gAppContext->javaSvrApiClass, gAppContext->javaSvrApiStartVsyncMethodId, gAppContext->javaActivityObject);
#endif

#ifdef USE_QVR_SERVICE
    int ret;
    QVRSERVICE_VRMODE_STATE state;

    //Tell the VR Service to put the system into VR Mode
    //Note : This must be called after the line pointer interrupt has been configured
    ret = gAppContext->qvrService->StartVRMode();
    if (ret < 0)
    {
        LOGE("svrBeginVr : QVRService error starting VR mode => %d", ret);
        return;
    }

    state = gAppContext->qvrService->GetVRMode();
    if (state != VRMODE_STARTED)
    {
        LOGE("svrBeginVr : QVRService VR not started, current State = %d", (int)state);
        return;
    }

    LOGI("QVRService: Service Initialized");
#endif

    LOGI("Starting Timewarp...");
    //svrBeginTimeWarp();

#if !defined(USE_QVR_SERVICE)
    LOGI("Starting Sensors...");
    svrStartSensors(false);
#endif // !defined(USE_QVR_SERVICE)

    if (gEnableDebugServer)
    {
//        svrStartDebugServer();
    }

    // Only now are we truly in VR mode
    gAppContext->inVrMode = true;
}

//-----------------------------------------------------------------------------
void svrEndVr()
//-----------------------------------------------------------------------------
{
    LOGI("svrEndVr");

    if (gAppContext == NULL)
    {
        LOGE("Unable to end VR! Application context has been released!");
        return;
    }

    if (!gAppContext->inVrMode)
    {
        LOGE("Unable to EndVR,  already ended");
        return;
    }
        
    // No longer in VR mode
    gAppContext->inVrMode = false;

#if defined (USE_QVR_SERVICE)
    if (!gUseLinePtr)
    {
        LOGI("Stopping Choreographer VSync Monitoring...");
    /*    JNIEnv* pThreadJEnv;
        if (gAppContext->javaVm->AttachCurrentThread(&pThreadJEnv, NULL) != JNI_OK)
        {
            LOGE("    svrBeginVr AttachCurrentThread failed.");
        }
        pThreadJEnv->CallStaticVoidMethod(gAppContext->javaSvrApiClass, gAppContext->javaSvrApiStopVsyncMethodId, gAppContext->javaActivityObject);*/
    }

    if (gAppContext->qvrService == NULL)
    {
        LOGE("svrEndVr Failed: SnapdragonVR not initialized!");
        return;
    }
#endif // defined (USE_QVR_SERVICE)

#if !defined(USE_QVR_SERVICE)
    LOGI("Stopping Choreographer VSync Monitoring...");
    JNIEnv* pThreadJEnv;
    if (gAppContext->javaVm->AttachCurrentThread(&pThreadJEnv, NULL) != JNI_OK)
    {
        LOGE("    svrBeginVr AttachCurrentThread failed.");
    }
    pThreadJEnv->CallStaticVoidMethod(gAppContext->javaSvrApiClass, gAppContext->javaSvrApiStopVsyncMethodId, gAppContext->javaActivityObject);
#endif

    LOGI("Stopping Timewarp...");
    //svrEndTimeWarp();

#if !defined(USE_QVR_SERVICE)
    LOGI("Stopping Sensors...");
    svrStopSensors();
#endif // !defined(USE_QVR_SERVICE)

    if (gRenderThreadCore >= 0)
    {
        LOGI("Clearing Eye Render Affinity");
        svrClearThreadAffinity();
    }

    if (gEnableRenderThreadFifo)
    {
        L_SetThreadPriority("Render Thread", SCHED_NORMAL, gNormalPriorityRender);
    }

    LOGI("Resetting CPU/GPU Performance Levels...");
    svrSetCpuPerfLevel(kPerfSystem);
    svrSetGpuPerfLevel(kPerfSystem);

	LOGI("Resetting tracking pose...");
	// We must save the mode since svrSetTackingMode() modifies it internally
	// We need it when the app resumes from suspension
	int currentTrackingMode = gAppContext->currentTrackingMode;
    svrSetTrackingMode(kTrackingRotation);
	gAppContext->currentTrackingMode = currentTrackingMode;	
	
#ifdef USE_QVR_SERVICE
    LOGI("Disconnecting from QVR Service...");
   
    QVRSERVICE_VRMODE_STATE state;
    int ret = gAppContext->qvrService->StopVRMode();
    if (ret < 0)
    {
        LOGE("Error stopping VR mode => %d", ret);
    }

    state = gAppContext->qvrService->GetVRMode();
    if (state != VRMODE_STOPPED)
    {
        LOGE("VR not stopped: Current State = %d", (int)state);
    }
#endif // USE_QVR_SERVICE

    //Delete the mode context
    //We can end up here with gAppContext->modeContext set to NULL
    if (gAppContext->modeContext != NULL)
    {
        //Clean up our synchronization primitives
        LOGI("Cleaning up thread synchronization primitives...");
        pthread_mutex_destroy(&gAppContext->modeContext->warpThreadContextMutex);
        pthread_cond_destroy(&gAppContext->modeContext->warpThreadContextCv);
        pthread_mutex_destroy(&gAppContext->modeContext->vsyncMutex);
        
        //Clean up any GPU fences still hanging around
        LOGI("Cleaning up frame fences...");
        for (int i = 0; i < NUM_SWAP_FRAMES; i++)
        {
            svrFrameParamsInternal& fp = gAppContext->modeContext->frameParams[i];
            if (fp.frameSync != 0)
            {
                glDeleteSync(fp.frameSync);
            }
        }

        LOGI("Deleting mode context...");
        delete gAppContext->modeContext;
        gAppContext->modeContext = NULL;
    }
  
    if (gEnableDebugServer)
    {
//        svrStopDebugServer();
    }

    PROFILE_SHUTDOWN();

    LOGI("VR mode ended");
}

//-----------------------------------------------------------------------------
void svrSubmitFrame(const svrFrameParams* pFrameParams)
//-----------------------------------------------------------------------------
{
    static unsigned int frameCounter = 0;
    static unsigned int prevTimeMs = 0;

    if (gAppContext == NULL || gAppContext->inVrMode == false)
    {
        LOGE("svrSubmitFrame Failed: Called when not in VR mode!");
        return;
    }

    if (gAppContext->modeContext == NULL)
    {
        LOGE("svrSubmitFrame Failed: Called when not in VR mode!");
        return;
    }

    while (gDisableFrameSubmit)
    {
        sleep(1);
        continue;
    }

    if (gLogSubmitFps)
    {
        unsigned int currentTimeMs = GetTimeNano() * 1e-6;
        frameCounter++;
        if (currentTimeMs - prevTimeMs > 1000)
        {
            float elapsedSec = (float)(currentTimeMs - prevTimeMs) / 1000.0f;
            float currentFPS = (float)frameCounter / elapsedSec;
            LOGI("FPS: %0.2f", currentFPS);

            frameCounter = 0;
            prevTimeMs = currentTimeMs;
        }
    }

    if (gAppContext == NULL || gAppContext->modeContext == NULL)
    {
        return;
    }


    unsigned int lastFrameCount = gAppContext->modeContext->submitFrameCount;

    unsigned int nextFrameCount = gAppContext->modeContext->submitFrameCount + 1;


    svrFrameParamsInternal& fp = gAppContext->modeContext->frameParams[nextFrameCount % NUM_SWAP_FRAMES];
    fp.frameParams = *pFrameParams;

    if (gForceMinVsync > 0)
    {
        fp.frameParams.minVsyncs = gForceMinVsync;
    }

    fp.frameSubmitTimeStamp = 0;
    fp.warpFrameLeftTimeStamp = 0;
    fp.warpFrameRightTimeStamp = 0;
    fp.minVSyncCount = gAppContext->modeContext->prevSubmitVsyncCount + fp.frameParams.minVsyncs;

    if (gDisableReprojection)
    {
        fp.frameParams.frameOptions |= kDisableReprojection;
    }

    fp.frameSubmitTimeStamp = Svr::GetTimeNano();

    if (fp.frameSync != 0)
    {
        glDeleteSync(fp.frameSync);
    }

    fp.frameSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    PROFILE_ENTER(GROUP_WORLDRENDER, 0, "glFlush");
    glFlush();
    PROFILE_EXIT(GROUP_WORLDRENDER);

   
    gAppContext->modeContext->submitFrameCount = nextFrameCount;
    LOGV("Submitting Frame : %d [minV=%llu curV=%llu]", gAppContext->modeContext->submitFrameCount, fp.minVSyncCount, gAppContext->modeContext->vsyncCount);

    PROFILE_ENTER(GROUP_WORLDRENDER, 0, "Submit Frame : %d", gAppContext->modeContext->submitFrameCount);

    //Wait until the eyebuffer has been consumed
    while (true)
    {
        pthread_mutex_lock(&gAppContext->modeContext->warpBufferConsumedMutex);
        pthread_cond_wait(&gAppContext->modeContext->warpBufferConsumedCv, &gAppContext->modeContext->warpBufferConsumedMutex);
        pthread_mutex_unlock(&gAppContext->modeContext->warpBufferConsumedMutex);

        if (gAppContext->modeContext->warpFrameCount >= lastFrameCount )
        {
            //Make sure we maintain the minSync interval
            gAppContext->modeContext->prevSubmitVsyncCount = glm::max(gAppContext->modeContext->vsyncCount, gAppContext->modeContext->prevSubmitVsyncCount + fp.frameParams.minVsyncs);

            LOGV("Finished : %d [%llu]", gAppContext->modeContext->submitFrameCount, gAppContext->modeContext->vsyncCount);
            break;
        }
    }
 
    PROFILE_EXIT(GROUP_WORLDRENDER);
   
    PROFILE_TICK();
}

bool svrIsVRModeStopped()
{
 #if defined (USE_QVR_SERVICE)
    if (gAppContext == NULL || gAppContext->qvrService == NULL)
    {
        LOGE("svrIsVRModeStopped failed : SnapdragonVR not initialized!");
        return false;
    }

    return (gAppContext->qvrService->GetVRMode() == VRMODE_STOPPED);
#else
    return false;
#endif
}

//-----------------------------------------------------------------------------
void svrSetPerformanceLevels(svrPerfLevel cpuPerfLevel, svrPerfLevel gpuPerfLevel)
//-----------------------------------------------------------------------------
{
    // If the config flag is set to override the performance levels allow this.
    // Otherwise do nothing here
    if (gForceCpuLevel < 0)
    {
        svrSetCpuPerfLevel(cpuPerfLevel);
    }
    else
    {
        LOGI("Application CPU specified performance levels not being used.  Config file forcing to %d", gForceCpuLevel);
    }
		
    if (gForceGpuLevel < 0)
    {
        svrSetGpuPerfLevel(gpuPerfLevel);
    }
    else
    {
        LOGI("Application GPU specified performance levels not being used.  Config file forcing to %d", gForceGpuLevel);
    }
}

//-----------------------------------------------------------------------------
float svrGetPredictedDisplayTime()
//-----------------------------------------------------------------------------
{
    if (gAppContext == NULL || gAppContext->inVrMode == false)
    {
        LOGE("svrGetPredictedDisplayTime Failed: Called when not in VR mode!");
        return 0.0f;
    }

    if (gAppContext->modeContext == NULL)
    {
        LOGE("svrGetPredictedDisplayTime Failed: Called when not in VR mode!");
        return 0.0f;
    }

#if defined (USE_QVR_SERVICE)
    if (gAppContext->qvrService == NULL)
    {
        LOGE("svrGetPredictedDisplayTime Failed: SnapdragonVR not initialized!");
        return 0.0f;
    }
#endif // defined (USE_QVR_SERVICE)

    if (gDisablePredictedTime)
    {
        return 0.0f;
    }

    double framePeriodNano = 1e9 / gAppContext->deviceInfo.displayRefreshRateHz;

    pthread_mutex_lock(&gAppContext->modeContext->vsyncMutex);
    uint64_t timestamp = Svr::GetTimeNano();
    double framePct = (double)gAppContext->modeContext->vsyncCount + ((double)(timestamp - gAppContext->modeContext->vsyncTimeNano) / (framePeriodNano));
    pthread_mutex_unlock(&gAppContext->modeContext->vsyncMutex);

    double fractFrame = framePct - ((long)framePct);

    float predictedTime = (framePeriodNano - (fractFrame * framePeriodNano)) + (0.5 * framePeriodNano);

    return predictedTime * 1e-6;
}

//-----------------------------------------------------------------------------
svrHeadPoseState svrGetPredictedHeadPose(float predictedTimeMs)
//-----------------------------------------------------------------------------
{
    svrHeadPoseState poseState;

    glm::fquat poseRot;
    glm::vec3  posePos;

    poseState.poseStatus = 0;
	//poseState.pose.rotation = poseRot;
	poseState.pose.rotation.x = poseRot.x;
	poseState.pose.rotation.y = poseRot.y;
	poseState.pose.rotation.z = poseRot.z;
	poseState.pose.rotation.w = poseRot.w;
	//poseState.pose.position = posePos;
	poseState.pose.position.x = posePos.x;
	poseState.pose.position.y = posePos.y;
	poseState.pose.position.z = posePos.z;

    if (gAppContext == NULL)
    {
        LOGE("svrGetPredictedHeadPose Failed: SnapdragonVR not initialized!");
		return poseState;
    }

    if (gAppContext == NULL || gAppContext->inVrMode == false)
    {
        LOGE("svrGetPredictedHeadPose Failed: Called when not in VR mode!");
        return poseState;
    }

    if (gAppContext->modeContext == NULL)
    {
        LOGE("svrGetPredictedHeadPose Failed: Called when not in VR mode!");
        return poseState;
    }

#if defined (USE_QVR_SERVICE)
    if (gAppContext->qvrService == NULL)
    {
        LOGE("svrGetPredictedHeadPose Failed: QVR service not initialized!");
        return poseState;
    }

    uint64_t sampleTimeStamp;

    if (gAppContext->qvrService != NULL)
    {
        svrGetPredictiveHeadPoseAsQuat(predictedTimeMs, &sampleTimeStamp, poseRot, posePos);
        poseState.poseStatus = gAppContext->currentTrackingMode;
        LOGE("svrGetPredictedHeadPose(): Position: (%0.2f, %0.2f, %0.2f); Rotation: (%0.2f, %0.2f, %0.2f, %0.2f)", posePos.x, posePos.y, posePos.z, poseRot.x, poseRot.y, poseRot.z, poseRot.w);
    }
#else

    poseRot = svrGetHeadPoseAsQuat(predictedTimeMs);
    poseState.poseStatus = kTrackingRotation;

#endif //USE_QVR_SERVICE

    // Adjust by the recenter value
    poseRot = poseRot * gAppContext->modeContext->recenterRot;

    if (gAppContext->currentTrackingMode & kTrackingPosition)
    {
        // If no actual 6DOF camera the positions come back as NaN
        posePos = posePos - gAppContext->modeContext->recenterPos;

        // Need to adjust this new position by the rotation correction
        posePos = posePos * gAppContext->modeContext->recenterRot;
    }

     //LOGE("svrGetPredictedHeadPose(): Position: (%0.2f, %0.2f, %0.2f); Rotation: (%0.2f, %0.2f, %0.2f, %0.2f)", posePos.x, posePos.y, posePos.z, poseRot.x, poseRot.y, poseRot.z, poseRot.w);

    // LOGE("svrGetPredictedHeadPose(): Position: (%0.2f, %0.2f, %0.2f); Offset: (%0.2f, %0.2f, %0.2f)", posePos.x, posePos.y, posePos.z, gAppContext->modeContext->recenterPos.x, gAppContext->modeContext->recenterPos.y, gAppContext->modeContext->recenterPos.z);
    // LOGE("                           Rotation: (%0.2f, %0.2f, %0.2f, %0.2f); Offset: (%0.2f, %0.2f, %0.2f, %0.2f)", poseRot.x, poseRot.y, poseRot.z, poseRot.w, gAppContext->modeContext->recenterRot.x, gAppContext->modeContext->recenterRot.y, gAppContext->modeContext->recenterRot.z, gAppContext->modeContext->recenterRot.w);

    poseState.pose.rotation.x = poseRot.x;
    poseState.pose.rotation.y = poseRot.y;
    poseState.pose.rotation.z = poseRot.z;
    poseState.pose.rotation.w = poseRot.w;

    if (gAppContext->currentTrackingMode & kTrackingPosition)
    {
        poseState.pose.position.x = posePos.x;
        poseState.pose.position.y = posePos.y;
        poseState.pose.position.z = posePos.z;
    }
    else
    {
        //If we aren't tracking position then just zero out the postion in the pose
        //state
        poseState.pose.position.x = 0.0f;
        poseState.pose.position.y = 0.0f;
        poseState.pose.position.z = 0.0f;
    }

    poseState.poseTimeStampNs = Svr::GetTimeNano();

    poseState.angularVelocity.x = poseState.angularVelocity.y = poseState.angularVelocity.z = 0.0f;
    poseState.linearVelocity.x = poseState.linearVelocity.y = poseState.linearVelocity.z = 0.0f;
    poseState.angularAcceleration.x = poseState.angularAcceleration.y = poseState.angularAcceleration.z = 0.0f;
    poseState.linearAccelearation.x = poseState.linearAccelearation.y = poseState.linearAccelearation.z = 0.0f;

    poseState.predictedTimeMs = predictedTimeMs;

    return poseState;
}

//-----------------------------------------------------------------------------
void svrRecenterPose()
//-----------------------------------------------------------------------------
{
    // We want to disable reprojection for a few frames after recenter has been called
    gRecenterTransition = 0;

    float predictedTimeMs = 0.0f;

    glm::fquat poseRot;
    glm::vec3  posePos;

    if (gAppContext == NULL || gAppContext->inVrMode == false)
    {
        LOGE("svrRecenterPose Failed: Called when not in VR mode!");
        return;
    }

    if (gAppContext->modeContext == NULL)
    {
        LOGE("svrRecenterPose Failed: Called when not in VR mode!");
        return;
    }

#if defined (USE_QVR_SERVICE)
    if (gAppContext->qvrService == NULL)
    {
        LOGE("svrRecenterPose Failed: SnapdragonVR not initialized!");
        return;
    }

    LOGI("Recentering Pose");

    uint64_t sampleTimeStamp;
    svrGetPredictiveHeadPoseAsQuat(predictedTimeMs, &sampleTimeStamp, poseRot, posePos);
#else
    poseRot = svrGetHeadPoseAsQuat(predictedTimeMs);
#endif //USE_QVR_SERVICE

    gAppContext->modeContext->recenterPos = posePos;

    // Recenter is whatever it takes to get back to center
    // gAppContext->modeContext->recenterRot = inverse(poseRot);

    // But we really want to only adjust for Yaw
    glm::vec3 Forward = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::fquat CurrentQuat;
    CurrentQuat.x = poseRot.x;
    CurrentQuat.y = poseRot.y;
    CurrentQuat.z = poseRot.z;
    CurrentQuat.w = poseRot.w;

    glm::vec3 LookDir = CurrentQuat * Forward;

    float RotateRads = atan2(LookDir.x, LookDir.z);

    glm::fquat IdentityQuat;
    gAppContext->modeContext->recenterRot = glm::rotate(IdentityQuat, -RotateRads, glm::vec3(0.0f, 1.0f, 0.0f));

}

//-----------------------------------------------------------------------------
SVRP_EXPORT unsigned int svrGetSupportedTrackingModes()
//-----------------------------------------------------------------------------
{
    unsigned int result = 0;

#if defined (USE_QVR_SERVICE)
    if (gAppContext == NULL || gAppContext->qvrService == NULL)
    {
        LOGE("svrGetSupportedTrackingModes failed: SnapdragonVR not initialized!");
        return 0;
    }

    //Get the supported tracking modes
    unsigned int supportedTrackingModes;
    gAppContext->qvrService->GetTrackingMode(NULL, &supportedTrackingModes);
    
    if (supportedTrackingModes & TRACKING_MODE_ROTATIONAL)
    {
        result |= kTrackingRotation;
    }
    
    if (supportedTrackingModes & TRACKING_MODE_POSITIONAL)
    {
        //Note at this time the QVR service will report that it is capable of positional tracking
        //due to the presence of a second camera even though that second camera may not be a 6DOF module
        result |= kTrackingPosition;
    }

#endif

    return result;
}

//-----------------------------------------------------------------------------
SVRP_EXPORT void svrSetTrackingMode(unsigned int trackingModes)
//-----------------------------------------------------------------------------
{
#if defined (USE_QVR_SERVICE)  
    if (gAppContext == NULL || gAppContext->qvrService == NULL)
    {
        LOGE("svrSetTrackingMode failed: SnapdragonVR not initialized!");
        return;
    }

    QVRSERVICE_VRMODE_STATE qvrState = gAppContext->qvrService->GetVRMode();
    if (qvrState != VRMODE_STOPPED)
    {
        LOGE("svrSetTrackingMode failed: tracking mode can only be changed when VR mode is in a stopped state");
    }

    //Check for a forced tracking mode from the config file
    unsigned int actualTrackingMode = trackingModes;
    if( gForceTrackingMode != 0)
    {
        LOGI("svrSetTrackingMode : Forcing to %d from config file", gForceTrackingMode);
        actualTrackingMode = gForceTrackingMode;
    }
    else
    {
        LOGI("svrSetTrackingMode : Config file did NOT force tracking mode!");
    }

    //QVR Service accepts only a direct positional or rotational value, not a bitmask
    QVRSERVICE_TRACKING_MODE qvrTrackingMode = TRACKING_MODE_NONE;
    if ( (actualTrackingMode & kTrackingPosition) != 0)
    {
        LOGI("svrSetTrackingMode : Setting tracking mode to positional");
        qvrTrackingMode = TRACKING_MODE_POSITIONAL;
    }
    else if ( (actualTrackingMode & kTrackingRotation) != 0)
    {
        LOGI("svrSetTrackingMode : Setting tracking mode to rotational");
        qvrTrackingMode = TRACKING_MODE_ROTATIONAL;
    }

    gAppContext->qvrService->SetTrackingMode( qvrTrackingMode );

    gAppContext->currentTrackingMode = actualTrackingMode;
#endif
}
