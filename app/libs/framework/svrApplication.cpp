//=============================================================================
// FILE: svrApplication.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "svrApplication.h"
#include "svrUtil.h"

namespace Svr
{



SvrApplication::SvrApplication()
{
    mAppContext.physicalWidth = 0;
    mAppContext.physicalHeight = 0;
    mAppContext.targetEyeWidth = 0;
    mAppContext.targetEyeHeight = 0;

    mAppContext.display = EGL_NO_DISPLAY;

    mAppContext.eyeRenderSurface = EGL_NO_SURFACE;
    mAppContext.eyeRenderContext = EGL_NO_CONTEXT;

    mAppContext.assetManager = NULL;
    mAppContext.nativeWindow = NULL;

    mAppContext.frameCount = 0;
}

SvrApplication::~SvrApplication()
{
}

void SvrApplication::Initialize()
{

}



void SvrApplication::Update()
{

}

void SvrApplication::Shutdown()
{

}

SvrApplicationContext& SvrApplication::GetApplicationContext()
{
    return mAppContext;
}



}
