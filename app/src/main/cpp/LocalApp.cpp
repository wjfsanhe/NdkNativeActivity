//=============================================================================
// FILE: app.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#include <unistd.h>

#include "svrApi.h"

#include "svrProfile.h"
#include "svrUtil.h"
#include "LocalApp.h"

//#include "svrConfig.h"

#define PROFILE_EYE_RENDERING   0

#define NUM_MULTIVIEW_SLICES    2


using namespace Svr;

// Functions that have to be queried from EGL
typedef void(*PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVR)(GLenum, GLenum, GLuint, GLint, GLint, GLsizei);
PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVR glFramebufferTextureMultiviewOVR = NULL;

#ifndef GL_TEXTURE_2D_MULTISAMPLE_ARRAY
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY        0x9102
#endif // GL_TEXTURE_2D_MULTISAMPLE_ARRAY

#ifndef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE       0x8D56
#endif // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE

// Functions that have to be queried from EGL
typedef void(*PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVR)(GLenum, GLenum, GLuint, GLint, GLsizei, GLint, GLsizei);
PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVR glFramebufferTextureMultisampleMultiviewOVR = NULL;

typedef void(*PFNGLTEXSTORAGE3DMULTISAMPLEOES)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
PFNGLTEXSTORAGE3DMULTISAMPLEOES glTexStorage3DMultisampleOES = NULL;

// These are from eglextQCOM.h
#ifndef EGL_NEW_IMAGE_QCOM
#define EGL_NEW_IMAGE_QCOM              0x3120
#endif // EGL_NEW_IMAGE_QCOM

#ifndef EGL_IMAGE_FORMAT_QCOM
#define EGL_IMAGE_FORMAT_QCOM           0x3121
#endif // EGL_IMAGE_FORMAT_QCOM

#ifndef EGL_FORMAT_RGBA_8888_QCOM
#define EGL_FORMAT_RGBA_8888_QCOM       0x3122
#endif // EGL_FORMAT_RGBA_8888_QCOM

enum eRenderMode
{
    kRenderModeSingle = 0,
    kRenderModeDouble,
    kRenderModeMultiview
};
eRenderMode gRenderMode = kRenderModeSingle;

// Configuration Variables


// For testing warping we need to sometimes turn off frame submit. These work with gTouchTogglesSubmitFrame
bool gSubmitFrame = true;
unsigned int gLastToggleTime = 0;

// Sometimes we need to test application running slower than TimeWarp.
// To force a desired FPS, set gForcedVSyncTime to desired frame time (milliseconds)
unsigned int gLastRenderTime = 0;

//TODO: Add this to config file and check in the arrow object
bool gUseArrows = false;

//-----------------------------------------------------------------------------
unsigned int GetTimeMS()
//-----------------------------------------------------------------------------
{
    struct timeval t;
    t.tv_sec = t.tv_usec = 0;

    if (gettimeofday(&t, NULL) == -1)
    {
        return 0;
    }

    return (unsigned int)(t.tv_sec * 1000LL + t.tv_usec / 1000LL);
}

LocalApp::LocalApp()
        : mMdlRotation(0.0f)
        , mFrameTimer(64)
{
}

//-----------------------------------------------------------------------------
void * LocalApp::GetFileBuffer(const char *pFileName, int *pBufferSize)
//-----------------------------------------------------------------------------
{
    char filePath[512];

    sprintf(filePath, "%s/%s", mAppContext.externalPath, pFileName);

    LOGI("Opening File: %s", filePath);
    FILE* fp = fopen(filePath, "rb");
    if (!fp)
    {
        LOGE("Unable to open file: %s", filePath);
        return NULL;
    }

    // Need the file size
    fseek(fp, 0, SEEK_END);
    *pBufferSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Now read the file into the new buffer
    char* pRetBuff = new char[*pBufferSize + 1];
    fread(pRetBuff, 1, *pBufferSize, fp);
    pRetBuff[*pBufferSize] = 0;

    fclose(fp);

    return pRetBuff;
}

bool LocalApp::LoadTextureCommon(GLuint *pTexture, const char *pFileName)
{
    GLenum TexTarget;

    return true;
}

bool LocalApp::CreateEglImage()
{
    // This overlay can any size.  Could make same size as eye buffer or
    // size of physical surface (for one eye).  Video textures will be larger
    // so final render will be sharper.
    mImageWidth = mAppContext.physicalWidth / 2;
    mImageHeight = mAppContext.physicalHeight;

    LOGI("**************************************************");
    LOGI("Creating %dx%d image buffer for overlay...", mImageWidth, mImageHeight);
    LOGI("**************************************************");

    // Create in RGBA format
    int totalBytes = mImageWidth * mImageHeight * 4;
    unsigned char *pData = (unsigned char *) new unsigned char[totalBytes];
    if (pData == NULL)
    {
        LOGE("Unable to allocate %d bytes for image buffer!", totalBytes);
        return false;
    }

    // Fill in default data for the texture
    // Routine taken from page 373 of OpenGL Redbook
    unsigned char onColor[4] = { 255, 255, 255, 255 };
    unsigned char offColor[4] = { 25, 25, 25, 255 };
    for (int row = 0; row < mImageHeight; row++)
    {
        for (int col = 0; col < mImageWidth; col++)
        {
            int whichPixel = (col * mImageHeight + row) * 4;

            // 0x08 parameters tells how many "checkers".
            // The lower the value, the more "checkers"
            // The number of "checkers" is uiWidth / 0x08
            unsigned int onTest = (row & 0x08) ^ (col & 0x08);

            if (onTest)
            {
                pData[whichPixel + 0] = onColor[0];
                pData[whichPixel + 1] = onColor[1];
                pData[whichPixel + 2] = onColor[2];
                pData[whichPixel + 3] = onColor[3];
            }
            else
            {
                pData[whichPixel + 0] = offColor[0];
                pData[whichPixel + 1] = offColor[1];
                pData[whichPixel + 2] = offColor[2];
                pData[whichPixel + 3] = offColor[3];
            }
        }   // Which Column
    }   // Which Row

    // Create the image object
    EGLint EglAttribs[] =
            {
                    EGL_WIDTH, mImageWidth,
                    EGL_HEIGHT, mImageHeight,
                    EGL_IMAGE_FORMAT_QCOM, EGL_FORMAT_RGBA_8888_QCOM,
                    EGL_NONE
            };

    // Can't get access to gralloc so have to let the driver create a new one for us.
    // mEGLImage = eglCreateImageKHR(mAppContext.display, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, (EGLClientBuffer)pData, EglAttribs);
    mEGLImage = eglCreateImageKHR(mAppContext.display, EGL_NO_CONTEXT, EGL_NEW_IMAGE_QCOM, (EGLClientBuffer)0, EglAttribs);
    if (mEGLImage == 0)
    {
        LOGE("Unable to create image buffer!");
        return false;
    }

    delete[] pData;

    // Create the GL texture object that goes with the image
    glGenTextures(1, &mImageTexture);

    glBindTexture(GL_TEXTURE_EXTERNAL_OES, mImageTexture);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Have the EGL image provide backing for the GL texture
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, mEGLImage);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);

    // Create the GL framebuffer object and attach the texture to it
    glGenFramebuffers(1, &mImageFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mImageFrameBuffer);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_EXTERNAL_OES, mImageTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void LocalApp::UpdateEglImage()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mImageFrameBuffer);
    GLenum nDrawBuffer[] = { GL_COLOR_ATTACHMENT0 };
    glViewport(0, 0, mImageWidth, mImageHeight);
    glScissor(0, 0, mImageWidth, mImageHeight);
    glDrawBuffers(1, nDrawBuffer);

    glClearColor(0.45f, 0.2f, 0.45f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Bind the blit shader...


    // Unbind the frame buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void LocalApp::CreateBlitAssets()
{

}

void LocalApp::InitializeModel(const char* path)
{

}



void LocalApp::InitializeFunctions()
{
    LOGI("**************************************************");
    LOGI("Initializing Function: glFramebufferTextureMultiviewOVR");
    LOGI("**************************************************");
    glFramebufferTextureMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVR)eglGetProcAddress("glFramebufferTextureMultiviewOVR");
    if (!glFramebufferTextureMultiviewOVR)
    {
        LOGE("Failed to  get proc address for glFramebufferTextureMultiviewOVR!!");
        exit(EXIT_FAILURE);
    }

    LOGI("**************************************************");
    LOGI("Initializing Function: glFramebufferTextureMultisampleMultiviewOVR");
    LOGI("**************************************************");
    glFramebufferTextureMultisampleMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVR)eglGetProcAddress("glFramebufferTextureMultisampleMultiviewOVR");
    if (!glFramebufferTextureMultisampleMultiviewOVR)
    {
        LOGE("Failed to  get proc address for glFramebufferTextureMultisampleMultiviewOVR!!");
        exit(EXIT_FAILURE);
    }

    LOGI("**************************************************");
    LOGI("Initializing Function: glTexStorage3DMultisampleOES");
    LOGI("**************************************************");
    glTexStorage3DMultisampleOES = (PFNGLTEXSTORAGE3DMULTISAMPLEOES)eglGetProcAddress("glTexStorage3DMultisampleOES");
    if (!glTexStorage3DMultisampleOES)
    {
        LOGE("Failed to  get proc address for glTexStorage3DMultisampleOES!!");
        exit(EXIT_FAILURE);
    }
}

void LocalApp::InitializeMultiView(int whichBuffer, int width, int height, int samples)
{

}

void LocalApp::Initialize()
{


}

void LocalApp::DestroyBlitAssets()
{

}


void L_CreateLayout(float centerX, float centerY, float radiusX, float radiusY, svrOverlayLayout *pLayout)
{

}


void LocalApp::Update()
{
    PROFILE_ENTER(GROUP_WORLDRENDER, 0, "Update");

    mFrameTimer.Start();

    SvrApplication::Update();


    // Update the time
    unsigned int TimeNow = GetTimeMS();

    mFrameCount++;




    PROFILE_EXIT(GROUP_WORLDRENDER);
}
void LocalApp::Shutdown() {}

void LocalApp::Render()
{

    float predictedTimeMs = svrGetPredictedDisplayTime();
    svrHeadPoseState poseState = svrGetPredictedHeadPose(6);

    if (true)
    {
        LOGE("Position: (%0.2f, %0.2f, %0.2f); Orientation: (%0.2f, %0.2f, %0.2f, %0.2f)", poseState.pose.position.x,
             poseState.pose.position.y,
             poseState.pose.position.z,
             poseState.pose.rotation.x,
             poseState.pose.rotation.y,
             poseState.pose.rotation.z,
             poseState.pose.rotation.w);
    }

    if (poseState.poseStatus & kTrackingPosition)
    {
        //Apply a scale to adjust for the scale of the content
        poseState.pose.position.x *= 2.0f;
        poseState.pose.position.y *= 2.0f;
        poseState.pose.position.z *= 2.0f;
    }
    else
    {
        poseState.pose.position.x = mHeadPosition.x;
        poseState.pose.position.y = mHeadPosition.y;
        poseState.pose.position.z = mHeadPosition.z;
    }

    float currentIpd = DEFAULT_IPD;
    float headHeight = DEFAULT_HEAD_HEIGHT;
    float headDepth = DEFAULT_HEAD_DEPTH;
    SvrGetEyeViewMatrices(poseState, false,
                          currentIpd, headHeight, headDepth,
                          mViewMtx[kLeft], mViewMtx[kRight]);




    mFrameTimer.Stop();

}

namespace Svr
{
    Svr::SvrApplication* CreateApplication()
    {
        return new LocalApp();
    }
}

