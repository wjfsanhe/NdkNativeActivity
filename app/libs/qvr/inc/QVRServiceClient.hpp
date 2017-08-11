/******************************************************************************/
/*! \file  QVRServiceClient.hpp */
/*
* Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
* All Rights Reserved
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
******************************************************************************/

/*
* This is deprecated! Use QVRServiceClient.h instead
*/

#ifndef QVRSERVICE_CLIENT_HPP
#define QVRSERVICE_CLIENT_HPP

#include <stdint.h>
#include <sched.h>

#include "QVRServiceClient.h"

/**************************************************************************//**
* qvrservice_sensor_tracking_data_t
* -----------------------------------------------------------------------------
* This structure contains a quaternion (x,y,z,w) representing rotational pose
* and position vector representing translational pose. ts is the timestamp of
* the corresponding sensor value originating from the sensor stack.
* The cofficients may be used for computing a forward-predicted pose.
******************************************************************************/
typedef struct {
    float rotation[4];
    float translation[3];
    uint32_t reserved;
    uint64_t ts;
    float prediction_coff_s[3];
    float prediction_coff_b[3];
    float prediction_coff_bdt[3];
    float prediction_coff_bdt2[3];
    uint8_t reserved2[40];
} qvrservice_sensor_tracking_data_t;

class QVRServiceClient {

public:
    QVRServiceClient();
    ~QVRServiceClient();

    /**********************************************************************//**
    * SetClientStatusCallback()
    * -------------------------------------------------------------------------
    * \param
    *    cb:       [in] Callback function to be called to handle status events
    *    pCtx:     [in] Context to be passed to callback function
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    This function may be called at any time. The client will maintain only
    *    one callback, so subsequent calls to this function will overwrite any
    *    previous callbacks set. cb may be set to NULL to disable status
    *    callbacks.
    * Notes
    *    None
    **************************************************************************/
    int32_t SetClientStatusCallback(client_status_callback_fn cb, void *pCtx);

    /**********************************************************************//**
    * GetVRMode()
    * -------------------------------------------------------------------------
    * \param
    *    None
    * \return
    *    Current VR Mode state
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    See QVRSERVICE_VRMODE_STATE for more info
    **************************************************************************/
    QVRSERVICE_VRMODE_STATE GetVRMode();

    /**********************************************************************//**
    * StartVRMode()
    * -------------------------------------------------------------------------
    * \param
    *    None
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    This function may be called at any time if the VR Mode is in the
    *    VRMODE_STOPPED state. Calling this function while the VR Mode is in
    *    any other state will return an error.
    * Notes
    *    The caller should not assume that VR Mode configuration (e.g. calls
    *    to SetParam(), SetDisplayInterruptConfig()) will persist through
    *    start/stop cycles. Therefore it is recommended to always reconfigure
    *    VR Mode to suit the caller's use case prior to or after calling
    *    StartVRMode(). See QVRSERVICE_VRMODE_STATE for more info.
    **************************************************************************/
    int32_t StartVRMode();

    /**********************************************************************//**
    * StopVRMode()
    * -------------------------------------------------------------------------
    * \param
    *    None
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    See QVRSERVICE_VRMODE_STATE for more info
    **************************************************************************/
    int32_t StopVRMode();

    /***********************************************************************//**
    * GetTrackingMode()
    * --------------------------------------------------------------------------
    * \param
    *    pCurrentMode:     [out] If non-NULL, will be set to the currently
    *                      configured tracking mode
    *    pSupportedModes:  [out] If non-NULL, will be set to a bitmask
    *                      representing which tracking modes are supported by
    *                      the device.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    This function may be called at any time.
    * Notes
    *    None
    ***************************************************************************/
    int32_t GetTrackingMode(QVRSERVICE_TRACKING_MODE *pCurrentMode,
        uint32_t *pSupportedModes);

    /***********************************************************************//**
    * SetTrackingMode()
    * --------------------------------------------------------------------------
    * \param
    *    mode:     [in] Tracking mode to set
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    This function must be called prior to calling StartVRMode().
    * Notes
    *    None
    ***************************************************************************/
    int32_t SetTrackingMode(QVRSERVICE_TRACKING_MODE mode);

    /***********************************************************************//**
    * SetDisplayInterruptConfig()
    * --------------------------------------------------------------------------
    * \param
    *    id:       [in] Display interrupt to use (see
    *              QVRSERVICE_DISP_INTERRUPT_ID)
    *    pCfg:     [in] Configuration information for display interrupt
    *    cfgSize:  [in] Size of the data passed in the pCfg pointer
    * \return
    *    If the configuration information specified a callback but callbacks
    *    are not supported on the device, then QVR_CALLBACK_NOT_SUPPORTED is
    *    returned. Otherwise returns QVR_SUCCESS upon success or QVR_ERROR if
    *    another error occurred.
    * Timing requirements
    *    This function may be called at any time, but callbacks will only occur
    *    when VR Mode is in the VRMODE_STARTED state and the device supports
    *    callbacks.
    * Notes
    *    If VR Mode is in the VRMODE_STARTED state, this function will only
    *    work if called by the same client that started VR Mode.
    ***************************************************************************/
    int32_t SetDisplayInterruptConfig(QVRSERVICE_DISP_INTERRUPT_ID id,
        void *pCfg, uint32_t cfgSize);

    /***********************************************************************//**
    * SetThreadPriority()
    * --------------------------------------------------------------------------
    * \param
    *    tid:      [in] Thread ID of the thread whose priority will be changed
    *    policy:   [in] Scheduling policy to set. Use SCHED_FIFO or SCHED_RR for
    *              real-time performance.
    *    priority: [in] Priority value. For real-time policies, the values can
    *              be in the range of 1-99 (higher the number, higher the
    *              priority).
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    See sched.h for more info.
    ***************************************************************************/
    int32_t SetThreadPriority(int tid, int policy, int priority);

    /**********************************************************************//**
    * GetParam()
    * -------------------------------------------------------------------------
    * \param
    *    pName:    [in] NUL-terminated name of the parameter length/value to
    *              retrieve. Must not be NULL.
    *    pLen:     [inout] If pValue is NULL, pLen will be filled in with the
    *              number of bytes (including the NUL terminator) required to
    *              hold the value of the parameter specified by pName. If pValue
    *              is non-NULL, pLen must point to an integer that represents
    *              the length of the buffer pointed to by pValue. pLen must not
    *              be NULL.
    *    pValue:   [in] Buffer to receive value.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    The pValue buffer will be filled in up to *pLen bytes (including NUL),
    *    so this may result in truncation of the value if the required length
    *    is larger than the size passed in pLen.
    **************************************************************************/
    int32_t GetParam(const char* pName, uint32_t* pLen, char* pValue);

    /**********************************************************************//**
    * SetParam()
    * -------------------------------------------------------------------------
    * \param
    *    pName:    [in] NUL-terminated name of parameter value to set. Must not
    *              be NULL.
    *    pValue:   [in] NUL-terminated value. Must not be NULL.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    Some parameters may only be able to be set when VR Mode is in the
    *    VRMODE_STOPPED state. Additionally, some parameters may only be able to
    *    be set when VR Mode is in the VRMODE_STARTED state if SetParam() is
    *    called from the same client that started VR Mode.
    * Notes
    *    None
    **************************************************************************/
    int32_t SetParam(const char* pName, const char* pValue);

    /**********************************************************************//**
    * GetSensorRawData()
    * -------------------------------------------------------------------------
    * \param
    *    ppData:   [out] Address of pointer to qvrservice_sensor_data_raw_t
    *              structure that will contain the raw data values.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    None
    **************************************************************************/
    int32_t GetSensorRawData(qvrservice_sensor_data_raw_t **ppData);

    /**********************************************************************//**
    * GetSensorTrackingData()
    * -------------------------------------------------------------------------
    * \param
    *    ppData:   [out] Address of pointer to qvrservice_sensor_tracking_data_t
    *              structure that will contain the tracking data values.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    None
    **************************************************************************/
    int32_t GetSensorTrackingData(qvrservice_sensor_tracking_data_t **ppData);

private:
    void* mReserved;
};

#endif /* QVRSERVICE_CLIENT_HPP */
