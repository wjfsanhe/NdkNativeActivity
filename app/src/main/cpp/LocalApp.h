//
// Created by wangjf on 8/10/17.
//

#ifndef LOCALACTIVITY_LOCALAPP_H
#define LOCALACTIVITY_LOCALAPP_H


#include <GLES/gl.h>

#define GL_GLEXT_PROTOTYPES
#include <GLES/glext.h>

#include <EGL/egl.h>

#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>

#include <math.h>


#include "svrApplication.h"
#include "svrCpuTimer.h"
//#include "svrGeometry.h"
//#include "svrGpuTimer.h"
//#include "svrKtxLoader.h"
//#include "svrRenderTarget.h"
//#include "svrShader.h"
#include "svrUtil.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"

struct MultiViewBuffer
{
    unsigned int    mColorId;
    unsigned int    mDepthId;
    unsigned int    mFboId;
};

enum OverlayImages
{
    kMonoOverlay = 0,
    kLeftOverlay,
    kRightOverlay,
    kQuadOverlay,
    kNumOverlayImages
};

class LocalApp : public Svr::SvrApplication
{
public:
    LocalApp();

    void Initialize();
    void Shutdown();
    void Update();
    void Render();


private:
    void *  GetFileBuffer(const char *pFileName, int *bufferSize);
    bool    LoadTextures();
    bool    LoadTextureCommon(GLuint *pTexture, const char *pFileName);

    bool    CreateEglImage();
    void    UpdateEglImage();

    void    InitializeFunctions();
    void    InitializeMultiView(int whichBuffer, int width, int height, int samples);

    void    CreateBlitAssets();
    void    DestroyBlitAssets();

    void    InitializeModel(const char* path);

    //void    InitializeShader(Svr::SvrShader &whichShader, const char* vertexPath, const char* fragmentPath, const char* vertexName, const char* fragmentName);


private:
    int                         mFrameCount;
    unsigned int                mLastFPSTime;
    float                       mCurrentFPS;

    glm::mat4                   mViewMtx[2];
    glm::mat4                   mProjMtx;
    glm::vec3                   mHeadPosition;

    glm::mat4                   mMdlMtx[6];
    glm::vec3                   mMdlColor[6];

    float                       mMdlRotation;



    Svr::SvrBufferedCpuTimer    mFrameTimer;


    // Testing overlay
    GLuint                      mOverlayTextures[kNumOverlayImages];
    GLuint                      mImageFrameBuffer;
    GLuint                      mImageTexture;
    EGLImageKHR                 mEGLImage;
    int                         mImageWidth;
    int                         mImageHeight;


};
#endif //LOCALACTIVITY_LOCALAPP_H
