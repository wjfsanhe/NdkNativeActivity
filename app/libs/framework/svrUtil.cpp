//=============================================================================
// FILE: svrUtil.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//============================================================================

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"

#include "svrUtil.h"

namespace Svr
{



    void SvrGetEyeViewMatrices(const svrHeadPoseState& poseState, bool bUseHeadModel,
                               float ipd, float headHeight, float headDepth, glm::mat4& outLeftEyeMatrix, glm::mat4& outRightEyeMatrix)
    {
        glm::fquat poseQuat = glm::fquat(poseState.pose.rotation.w, poseState.pose.rotation.x, poseState.pose.rotation.y, poseState.pose.rotation.z);
        glm::mat4 poseMat = glm::mat4_cast(poseQuat);

        glm::vec4 headOffset;

        if (bUseHeadModel && 
            ((poseState.poseStatus | kTrackingPosition) == 0 ))
        {
            //Only use a head model if the uses has chosen to enable it and positional tracking is not enabled
            headOffset = glm::vec4(0.0f, headHeight, -headDepth, 1.0f);
        }
        else
        {
            if ((poseState.poseStatus | kTrackingPosition) != 0)
            {
                //Positional tracking is enabled so use that positional data from the pose 
                headOffset.x = poseState.pose.position.x;
                headOffset.y = poseState.pose.position.y;
                headOffset.z = poseState.pose.position.z;
            }
            else
            {
                //No head model, no positional tracking data
                headOffset = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

                // But the user may have set the desired position in the pose state
                headOffset.x = poseState.pose.position.x;
                headOffset.y = poseState.pose.position.y;
                headOffset.z = poseState.pose.position.z;
            }
        }

        glm::mat4 leftEyeOffsetMat = glm::translate(glm::mat4(1.0f), glm::vec3((-0.5f * ipd) + headOffset.x, headOffset.y, headOffset.z));
        glm::mat4 rightEyeOffsetMat = glm::translate(glm::mat4(1.0f), glm::vec3((0.5f * ipd) + headOffset.x, headOffset.y, headOffset.z));

        outLeftEyeMatrix = poseMat * leftEyeOffsetMat;
        outRightEyeMatrix = poseMat * rightEyeOffsetMat;
    }

}//End namespace Svr
