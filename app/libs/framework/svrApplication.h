//=============================================================================
// FILE: svrApplication.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#pragma once

#include <android/sensor.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <pthread.h>

#include "svrApi.h"

#include "svrInput.h"
//#include "svrRenderTarget.h"

#define SVR_NUM_EYE_BUFFERS     3
#define SVR_NUM_EYES			2

namespace Svr
{
    enum SvrEyeId
    {
        kLeft = 0,
        kRight = 1
    };

   /* struct SvrEyeBuffer
    {
        //SvrRenderTarget eyeTarget[SVR_NUM_EYES];
		//SvrRenderTarget singleSampledBufferForTimeWarp[SVR_NUM_EYES];
    };*/

    struct SvrApplicationContext
    {
        int             physicalWidth;
        int             physicalHeight;
        int             targetEyeWidth;
        int             targetEyeHeight;
        float           targetEyeFovXDeg;
        float           targetEyeFovYDeg;

        //SvrEyeBuffer    eyeBuffers[SVR_NUM_EYE_BUFFERS];
        int             eyeBufferIndex;
    
        EGLDisplay      display;
        EGLSurface      eyeRenderSurface;
        EGLContext      eyeRenderContext;
       
        ANativeActivity*    activity;
        AAssetManager*  assetManager;
        ANativeWindow*  nativeWindow;

        svrPerfLevel    cpuPerfLevel;
        svrPerfLevel    gpuPerfLevel;
        unsigned int    trackingMode;

    	char*          internalPath;
        char*          externalPath;

        int            frameCount;
    };

    class SvrApplication
    {
    public:
        SvrApplication();
        virtual ~SvrApplication();

        virtual void Initialize();
        virtual void Shutdown();

        virtual void Update();
        virtual void Render() = 0;


        SvrInput& GetInput();
        SvrApplicationContext& GetApplicationContext();
      
    protected:


    protected:
        SvrApplicationContext   mAppContext;
    };

    extern SvrApplication* CreateApplication();
}
