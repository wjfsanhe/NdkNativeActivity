/******************************************************************************/
/*! \file  QVRServiceClient.h */
/*
* Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
* All Rights Reserved
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
******************************************************************************/
#ifndef QVRSERVICE_CLIENT_H
#define QVRSERVICE_CLIENT_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <sched.h>
#include <dlfcn.h>
#include <stdlib.h>

/**************************************************************************//**
* Error codes
******************************************************************************/
#define QVR_SUCCESS                    0
#define QVR_ERROR                      -1
#define QVR_CALLBACK_NOT_SUPPORTED     -2
#define QVR_API_NOT_SUPPORTED          -3
#define QVR_INVALID_PARAM              -4


/**************************************************************************//**
* QVRSERVICE_SERVICE_VERSION
* -----------------------------------------------------------------------------
* Description
*   Can be used with GetParam() to retrieve the VR Service's version.
* Access
*   Read only
* Notes
*   None
******************************************************************************/
#define QVRSERVICE_SERVICE_VERSION    "service-version"

/**************************************************************************//**
* QVRSERVICE_CLIENT_VERSION
* -----------------------------------------------------------------------------
* Description
*   Can be used with GetParam() to retrieve the VR Service client's version.
* Access
*   Read only
* Notes
*   None
******************************************************************************/
#define QVRSERVICE_CLIENT_VERSION     "client-version"

/**************************************************************************//**
* QVRSERVICE_TRACKER_ANDROID_OFFSET_NS
* -----------------------------------------------------------------------------
* Description
*   Can be used with GetParam() to retrieve the fixed offset between the
*   tracker time domain (QTimer) and the Android time domain in ns.
* Access
*   Read only
* Notes
*   None
******************************************************************************/
#define QVRSERVICE_TRACKER_ANDROID_OFFSET_NS "tracker-android-offset-ns"


/**************************************************************************//**
* QVRSERVICE_VRMODE_STATE
* -----------------------------------------------------------------------------
*   VRMODE_UNSUPPORTED
*      VR Mode is unsupported on this device.
*   VRMODE_STARTING
*      VR Mode is starting. The state will automatically transition to
*      VRMODE_STARTED once startup is complete.
*   VRMODE_STARTED
*      VR Mode is started/active. While in this state, only the client that
*      started VR Mode can modify the VR Mode state.
*   VRMODE_STOPPING
*      VR Mode is stopping. The state will automatically transition to
*      VRMODE_STOPPED once complete.
*   VRMODE_STOPPED
*      VR Mode is stopped/inactive. While in this state, any client can modify
*      the VR Mode state.
******************************************************************************/
typedef enum {
    VRMODE_UNSUPPORTED = 0,
    VRMODE_STARTING,
    VRMODE_STARTED,
    VRMODE_STOPPING,
    VRMODE_STOPPED,
} QVRSERVICE_VRMODE_STATE;

/**************************************************************************//**
* QVRSERVICE_TRACKING_MODE
* -----------------------------------------------------------------------------
*   TRACKING_MODE_NONE
*      Tracking is disabled. Calls to GetHeadTrackingData() will fail.
*   TRACKING_MODE_ROTATIONAL
*      Rotational mode provides tracking information using 3 degrees of freedom,
*      i.e. "what direction am I facing?". When this mode is used, the rotation
*      quaternion in the qvrservice_head_tracking_data_t structure will be
*      filled in when GetHeadTrackingData() is called.
*   TRACKING_MODE_POSITIONAL
*      Positional mode provides tracking information using 6 degrees of freedom,
*      i.e. "what direction am I facing, and where am I relative to my starting
*      point?" When this mode is used, both the rotation quaternion and
*      translation vector in the qvrservice_head_tracking_data_t structure
*      will be filled in when GetHeadTrackingData() is called.
*   TRACKING_MODE_ROTATIONAL_MAG
*      Rotational_Mag mode provides tracking information similar to
*      TRACKING_MODE_ROTATIONAL but without any drift over time. The drift
*      sensitive apps (i.e. movie theater) would use this tracking mode.
*      However, this mode may require spacial movement of the device to have
*      the mag sensor calibrated (i.e. figure 8 movement). Also, this mode
*      is very sensitive to magnetic fields. In case of uncalibrated mag
*      data, this mode automatically defaults to TRACKING_MODE_ROTATIONAL which
*      may exhibit slow drift over time.
*
******************************************************************************/
typedef enum {
    TRACKING_MODE_NONE = 0,
    TRACKING_MODE_ROTATIONAL = 0x1,
    TRACKING_MODE_POSITIONAL = 0x2,
    TRACKING_MODE_ROTATIONAL_MAG = 0x4,
} QVRSERVICE_TRACKING_MODE;

/**************************************************************************//**
* qvrservice_sensor_data_raw_t
* -----------------------------------------------------------------------------
* This structure contains raw sensor values in the following format:
*   gts:        Timestamp of the gyroscope data
*   ats:        Timestamp of the accelerometer data
*   gx, gy, gz: Gyroscope X, Y, and Z
*   ax, ay, az: Accelerometer X, Y, and Z
*   reserved:   Reserved for future use
******************************************************************************/
typedef struct {
    uint64_t gts;
    uint64_t ats;
    float gx;
    float gy;
    float gz;
    float ax;
    float ay;
    float az;
    uint8_t reserved[24];
} qvrservice_sensor_data_raw_t;

/**************************************************************************//**
* qvrservice_head_tracking_data_t
* -----------------------------------------------------------------------------
* This structure contains a quaternion (x,y,z,w) representing rotational pose
* and position vector representing translational pose. ts is the timestamp of
* the corresponding sensor value originating from the sensor stack.
* The cofficients may be used for computing a forward-predicted pose.
* flags_3dof_mag_used bit will be set only when TRACKING_MODE_ROTATIONAL_MAG
* mode is used. flags_3dof_mag_calibrated will be set only when mag data
* is calibrated. If the flag is not set, user needs to be notified to move
* the device for calibrating mag sensor (i.e. figure 8 movement)
* pose_quality, sensor_quality and camera_quality fields are filled only
* for TRACKING_MODE_POSITIONAL mode. They are scaled from 0.0 (bad) to 1.0
* (good).
******************************************************************************/
typedef struct {
    float rotation[4];
    float translation[3];
    uint32_t reserved;
    uint64_t ts;
    uint64_t reserved1;
    float prediction_coff_s[3];
    uint32_t reserved2;
    float prediction_coff_b[3];
    uint32_t reserved3;
    float prediction_coff_bdt[3];
    uint32_t reserved4;
    float prediction_coff_bdt2[3];
    uint32_t reserved5;
    uint32_t flags_3dof_mag_used : 1;
    uint32_t flags_3dof_mag_calibrated : 1;
    float pose_quality;
    float sensor_quality;
    float camera_quality;
} qvrservice_head_tracking_data_t;



/**************************************************************************//**
* QVRSERVICE_RING_BUFFER_ID
* -----------------------------------------------------------------------------
*   RING_BUFFER_POSE
*      ID to indicate the ring buffer for the pose data in shared memory.
*      This ring buffer contains qvrservice_head_tracking_data_t elements.
******************************************************************************/
typedef enum {
    RING_BUFFER_POSE=1,
    RING_BUFFER_MAX
} QVRSERVICE_RING_BUFFER_ID;

/**************************************************************************//**
* qvrservice_ring_buffer_desc_t
* -----------------------------------------------------------------------------
* This structure describes the ring buffer attributes for the provided ring
* buffer ID (see QVRSERVICE_RING_BUFFER_ID).
*   fd:            File descriptor of shared memory block
*   size:          Size in bytes of shared memory block
*   index_offset:  Offset in bytes to the ring index. The index is a 4-byte
*                  integer.
*   ring_offset:   Offset in bytes to the first element of the ring buffer.
*   element_size:  Size in bytes of each element in the ring buffer. This value
*                  should match the structure size for a given ring buffer ID.
*   num_elements:  Total number of elements in the ring buffer
******************************************************************************/
typedef struct {
    int32_t fd;
    uint32_t size;
    uint32_t index_offset;
    uint32_t ring_offset;
    uint32_t element_size;
    uint32_t num_elements;
    uint32_t reserved[2];
} qvrservice_ring_buffer_desc_t;

/**************************************************************************//**
* QVRSERVICE_DISP_INTERRUPT_ID
* -----------------------------------------------------------------------------
*   DISP_INTERRUPT_VSYNC
*      This is the display VSYNC signal. It fires at the beginning of every
*      frame.
*      Interrupt config data: pointer to qvrservice_vsync_interrupt_config_t
*   DISP_INTERRUPT_LINEPTR
*      This interrupt can be configured to interrupt after a line of data has
*      been transmitted to the display.
*      Interrupt config data: pointer to qvrservice_lineptr_interrupt_config_t
******************************************************************************/
typedef enum {
    DISP_INTERRUPT_VSYNC = 0,
    DISP_INTERRUPT_LINEPTR,
    DISP_INTERRUPT_MAX
} QVRSERVICE_DISP_INTERRUPT_ID;

/**************************************************************************//**
* disp_interrupt_callback_fn
* -----------------------------------------------------------------------------
* Callback for handling a display interrupt
*    pCtx:    The context passed in to SetDisplayInterruptConfig()
*    ts:      The timestamp of the hardware interrupt
******************************************************************************/
typedef void (*disp_interrupt_callback_fn)(void *pCtx, uint64_t ts);

/**************************************************************************//**
* qvrservice_vsync_interrupt_config_t
* -----------------------------------------------------------------------------
* This structure is used to configure vsync interrupts
*    cb:      Callback to call when interrupt occurs. Set to NULL to disable
*             interrupts.
*    ctx:     Context passed to callback
******************************************************************************/
typedef struct {
    disp_interrupt_callback_fn cb;
    void *ctx;
} qvrservice_vsync_interrupt_config_t;

/**************************************************************************//**
* qvrservice_lineptr_interrupt_config_t
* -----------------------------------------------------------------------------
* This structure is used to configure lineptr interrupts
*    cb:      Callback to call when interrupt occurs. Set to NULL to disable
*             interrupts.
*    ctx:     Context passed to callback
*    line:    Line number to interrupt on. Valid values are 1 to N, where N is
*             the width of the display (in pixels)
******************************************************************************/
typedef struct {
    disp_interrupt_callback_fn cb;
    void *ctx;
    uint32_t line;
} qvrservice_lineptr_interrupt_config_t;

/**************************************************************************//**
* QVRSERVICE_CLIENT_STATUS
* -----------------------------------------------------------------------------
*   STATUS_DISCONNECTED
*      The client was unexpectedly disconnected from server. If this occurs,
*      the QVRServiceClient object must be deleted.
*   STATUS_STATE_CHANGED
*      The VR Mode state has changed. arg1 will contain the new state,
*      arg2 will contain the previous state.
*   STATUS_SENSOR_ERROR
*      The sensor stack has detected an error. The arg parameters will be set
*      to useful values to help identify the error (TBD).
******************************************************************************/
typedef enum {
    STATUS_DISCONNECTED = 0,
    STATUS_STATE_CHANGED,
    STATUS_SENSOR_ERROR,
    STATUS_MAX = 0xffffffff
} QVRSERVICE_CLIENT_STATUS;

/**************************************************************************//**
* client_status_callback
* -----------------------------------------------------------------------------
* Callback for handling a client status event
*    pCtx:    [in] The context passed in to SetClientStatusCallback()
*    status:  [in] Status value specifying the reason for the callback
*    arg1:    [in] Argument 1 (depends on status)
*    arg2:    [in] Argument 2 (depends on status)
******************************************************************************/
typedef void (*client_status_callback_fn)(void *pCtx,
    QVRSERVICE_CLIENT_STATUS status, uint32_t arg1, uint32_t arg2);

/**************************************************************************//**
* Client APIs to communicate with QVRService. Typical call flow is as follows:
*   1. Create QVRServiceClient object
*   2. Call GetVRMode() to verify VR mode is supported and in the STOPPED state
*   3. Call SetClientStatusCallback() to get notified of status events
*   4. Call GetTrackingMode()/SetTrackinMode() to configure tracking
*   5. Call SetDisplayInterruptConfig() to handle display interrupts
*   6. Call StartVRMode() to start VR mode
*       - Handle display interrupt events
*       - Call GetHeadTrackingData() to read latest tracking data
*   7. Call StopVRMode() when to end VR mode
*   8. Delete QVRServiceClient object
******************************************************************************/

/**************************************************************************//**
* QVRSERVICECLIENT_API_VERSION
* -----------------------------------------------------------------------------
* Defines the API versions of this interface. The api_version member of the
******************************************************************************/
typedef enum {
    QVRSERVICECLIENT_API_VERSION_1 = 1,
    QVRSERVICECLIENT_API_VERSION_2 = 2,
} QVRSERVICECLIENT_API_VERSION;


typedef void* qvrservice_client_handle_t;

typedef struct qvrservice_client_ops {

    /**********************************************************************//**
    * Create()
    * -------------------------------------------------------------------------
    * \return
    *    Returns qvrservice_client_handle_t
    * API level
    *    1 or higher
    * Timing requirements
    *    This function needs to be called first before calling any other
    *    functions listed below. The client context received by calling
    *    this function needs to be passed to the other functions listed
    *    below.
    * Notes
    *    None
    **************************************************************************/
    qvrservice_client_handle_t (*Create)();

    /**********************************************************************//**
    * Destroy()
    * -------------------------------------------------------------------------
    * \param
    *    client:  [in] qvrservice_client_handle_t returned by Create.
    * \return
    *    None
    * API level
    *    1 or higher
    * Timing requirements
    *    This function needs to be called when app shuts down. This
    *    function will destroy the client context therefore the same client
    *    context can't be used in any other functions listed below.
    * Notes
    *    None
    **************************************************************************/
    void (*Destroy)(qvrservice_client_handle_t client);

    /**********************************************************************//**
    * SetClientStatusCallback()
    * -------------------------------------------------------------------------
    * \param
    *    client:   [in] qvrservice_client_handle_t returned by Create.
    *    cb:       [in] Callback function to be called to handle status events
    *    pCtx:     [in] Context to be passed to callback function
    * \return
    *    Returns 0 upon success, -1 otherwise
    * API level
    *    1 or higher
    * Timing requirements
    *    This function may be called at any time. The client will maintain only
    *    one callback, so subsequent calls to this function will overwrite any
    *    previous callbacks set. cb may be set to NULL to disable status
    *    callbacks.
    * Notes
    *    None
    **************************************************************************/
    int32_t (*SetClientStatusCallback)(qvrservice_client_handle_t client,
        client_status_callback_fn cb, void *pCtx);

    /**********************************************************************//**
    * GetVRMode()
    * -------------------------------------------------------------------------
    * \param
    *    client:  [in] qvrservice_client_handle_t returned by Create.
    * \return
    *    Current VR Mode state
    * API level
    *    1 or higher
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    See QVRSERVICE_VRMODE_STATE for more info
    **************************************************************************/
    QVRSERVICE_VRMODE_STATE (*GetVRMode)(qvrservice_client_handle_t client);

    /**********************************************************************//**
    * StartVRMode()
    * -------------------------------------------------------------------------
    * \param
    *    client:  [in] qvrservice_client_handle_t returned by Create.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * API level
    *    1 or higher
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
    int32_t (*StartVRMode)(qvrservice_client_handle_t client);

    /**********************************************************************//**
    * StopVRMode()
    * -------------------------------------------------------------------------
    * \param
    *    client:  [in] qvrservice_client_handle_t returned by Create.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * API level
    *    1 or higher
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    See QVRSERVICE_VRMODE_STATE for more info
    **************************************************************************/
    int32_t (*StopVRMode)(qvrservice_client_handle_t client);

    /***********************************************************************//**
    * GetTrackingMode()
    * --------------------------------------------------------------------------
    * \param
    *    client:       [in] qvrservice_client_handle_t returned by Create.
    *    pCurrentMode: [out] If non-NULL, will be set to the currently
    *                      configured tracking mode
    *    pSupportedModes:  [out] If non-NULL, will be set to a bitmask
    *                      representing which tracking modes are supported by
    *                      the device.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * API level
    *    1 or higher
    * Timing requirements
    *    This function may be called at any time.
    * Notes
    *    None
    ***************************************************************************/
    int32_t (*GetTrackingMode)(qvrservice_client_handle_t client,
        QVRSERVICE_TRACKING_MODE *pCurrentMode,
        uint32_t *pSupportedModes);

    /***********************************************************************//**
    * SetTrackingMode()
    * --------------------------------------------------------------------------
    * \param
    *    client:  [in] qvrservice_client_handle_t returned by Create.
    *    mode:    [in] Tracking mode to set
    * \return
    *    Returns 0 upon success, -1 otherwise
    * API level
    *    1 or higher
    * Timing requirements
    *    This function must be called prior to calling StartVRMode().
    * Notes
    *    None
    ***************************************************************************/
    int32_t (*SetTrackingMode)(qvrservice_client_handle_t client,
         QVRSERVICE_TRACKING_MODE mode);

    /***********************************************************************//**
    * SetDisplayInterruptConfig()
    * --------------------------------------------------------------------------
    * \param
    *    client:   [in] qvrservice_client_handle_t returned by Create.
    *    id:       [in] Display interrupt to use (see
    *                   QVRSERVICE_DISP_INTERRUPT_ID)
    *    pCfg:     [in] Configuration information for display interrupt
    *    cfgSize:  [in] Size of the data passed in the pCfg pointer
    * \return
    *    If the configuration information specified a callback but callbacks
    *    are not supported on the device, then QVR_CALLBACK_NOT_SUPPORTED is
    *    returned. Otherwise returns QVR_SUCCESS upon success or QVR_ERROR if
    *    another error occurred.
    * API level
    *    1 or higher
    * Timing requirements
    *    This function may be called at any time, but callbacks will only occur
    *    when VR Mode is in the VRMODE_STARTED state and the device supports
    *    callbacks.
    * Notes
    *    If VR Mode is in the VRMODE_STARTED state, this function will only
    *    work if called by the same client that started VR Mode.
    ***************************************************************************/
    int32_t (*SetDisplayInterruptConfig)(qvrservice_client_handle_t client,
        QVRSERVICE_DISP_INTERRUPT_ID id,
        void *pCfg, uint32_t cfgSize);

    /***********************************************************************//**
    * SetThreadPriority()
    * --------------------------------------------------------------------------
    * \param
    *    client:   [in] qvrservice_client_handle_t returned by Create.
    *    tid:      [in] Thread ID of the thread whose priority will be changed
    *    policy:   [in] Scheduling policy to set. Use SCHED_FIFO or SCHED_RR for
    *              real-time performance.
    *    priority: [in] Priority value. For real-time policies, the values can
    *              be in the range of 1-99 (higher the number, higher the
    *              priority).
    * \return
    *    Returns 0 upon success, -1 otherwise
    * API level
    *    1 or higher
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    See sched.h for more info.
    ***************************************************************************/
    int32_t (*SetThreadPriority)(qvrservice_client_handle_t client, int tid,
          int policy, int priority);

    /**********************************************************************//**
    * GetParam()
    * -------------------------------------------------------------------------
    * \param
    *    client:   [in] qvrservice_client_handle_t returned by Create.
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
    * API level
    *    1 or higher
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    The pValue buffer will be filled in up to *pLen bytes (including NUL),
    *    so this may result in truncation of the value if the required length
    *    is larger than the size passed in pLen.
    **************************************************************************/
    int32_t (*GetParam)(qvrservice_client_handle_t client, const char* pName,
          uint32_t* pLen, char* pValue);

    /**********************************************************************//**
    * SetParam()
    * -------------------------------------------------------------------------
    * \param
    *    client:   [in] qvrservice_client_handle_t returned by Create.
    *    pName:    [in] NUL-terminated name of parameter value to set. Must not
    *              be NULL.
    *    pValue:   [in] NUL-terminated value. Must not be NULL.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * API level
    *    1 or higher
    * Timing requirements
    *    Some parameters may only be able to be set when VR Mode is in the
    *    VRMODE_STOPPED state. Additionally, some parameters may only be able to
    *    be set when VR Mode is in the VRMODE_STARTED state if SetParam() is
    *    called from the same client that started VR Mode.
    * Notes
    *    None
    **************************************************************************/
    int32_t (*SetParam)(qvrservice_client_handle_t client, const char* pName,
        const char* pValue);

    /**********************************************************************//**
    * GetSensorRawData()
    * -------------------------------------------------------------------------
    * \param
    *    client:   [in] qvrservice_client_handle_t returned by Create.
    *    ppData:   [out] Address of pointer to qvrservice_sensor_data_raw_t
    *              structure that will contain the raw data values.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * API level
    *    1 or higher
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    None
    **************************************************************************/
    int32_t (*GetSensorRawData)(qvrservice_client_handle_t client,
        qvrservice_sensor_data_raw_t **ppData);

    /**********************************************************************//**
    * GetHeadTrackingData()
    * -------------------------------------------------------------------------
    * \param
    *    client:   [in] qvrservice_client_handle_t returned by Create.
    *    ppData:   [out] Address of pointer to qvrservice_head_tracking_data_t
    *              structure that will contain the tracking data values.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * API level
    *    1 or higher
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    None
    **************************************************************************/
    int32_t (*GetHeadTrackingData)(qvrservice_client_handle_t client,
        qvrservice_head_tracking_data_t **ppData);

    /**********************************************************************//**
    * GetRingBufferDescriptor()
    * -------------------------------------------------------------------------
    * \param
    *    client:  [in] qvrservice_client_handle_t returned by Create.
    *    id   :   [in] ID of the ring buffer descriptor to retrieve
    *    pDesc:   [out] Ring buffer descriptor
    * \return
    *    Returns 0 upon success, -1 otherwise
    * API level
    *    2 or higher
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    Client needs to read the index value from ring buffer's index offset,
    *    then should access the element from the buffer's ring offset
    *    for retrieving the latest updated element in the buffer
    **************************************************************************/
    int32_t (*GetRingBufferDescriptor)(qvrservice_client_handle_t client,
        QVRSERVICE_RING_BUFFER_ID id,
        qvrservice_ring_buffer_desc_t *pDesc);

    /**********************************************************************//**
    * GetHistoricalHeadTrackingData()
    * -------------------------------------------------------------------------
    * \param
    *    client:       [in] qvrservice_client_handle_t returned by Create.
    *    ppData:       [out] Address of pointer to
    *                  qvrservice_head_tracking_data_t structure that will
    *                  contain the tracking data values.
    *    timestampNs:  [in] The time in nanoseconds for the pose requested
    *                  (which must be a time in the past). This value must be
    *                  in the Tracker (Qtimer) time domain.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    Use QVRSERVICE_TRACKER_ANDROID_OFFSET_NS for converting between
    *    Tracker and Android time domains.
    **************************************************************************/
    int32_t (*GetHistoricalHeadTrackingData)(
        qvrservice_client_handle_t client,
        qvrservice_head_tracking_data_t **ppData, int64_t timestampNs);

    //Reserved for future use
    void* reserved[64 - 2];

}qvrservice_client_ops_t;

typedef struct qvrservice_client{
    int api_version;
    qvrservice_client_ops_t* ops;
} qvrservice_client_t;

qvrservice_client_t* getQvrServiceClientInstance(void);


//helper
#define QVRSERVICE_CLIENT_LIB "libqvrservice_client.so"

typedef struct {
    void* libHandle;
    qvrservice_client_t* client;
    qvrservice_client_handle_t clientHandle;

} qvrservice_client_helper_t;


static inline qvrservice_client_helper_t* QVRServiceClient_Create()
{
    qvrservice_client_helper_t* me = (qvrservice_client_helper_t*)
                                    malloc(sizeof(qvrservice_client_helper_t));
    if(!me) return NULL;

    me->libHandle = dlopen( QVRSERVICE_CLIENT_LIB, RTLD_NOW);
    if (!me->libHandle) {
        free(me);
        return NULL;
    }

    typedef qvrservice_client_t* (*qvrservice_client_wrapper_fn)(void);
    qvrservice_client_wrapper_fn qvrServiceClient;

    qvrServiceClient = (qvrservice_client_wrapper_fn)dlsym(me->libHandle,
                                                "getQvrServiceClientInstance");
    if (!qvrServiceClient) {
        dlclose(me->libHandle);
        free(me);
        return NULL;
    }
    me->client = qvrServiceClient();
    me->clientHandle = me->client->ops->Create();
    if(!me->clientHandle){
        dlclose(me->libHandle);
        free(me);
        return NULL;
    }

    return me;
}

static inline void QVRServiceClient_Destroy(qvrservice_client_helper_t* me)
{
    if(!me) return;

    if(me->client->ops->Destroy){
        me->client->ops->Destroy( me->clientHandle);
    }

    if(me->libHandle ){
        dlclose(me->libHandle);
    }
    free(me);
    me = NULL;
}

static inline int32_t QVRServiceClient_SetClientStatusCallback(
   qvrservice_client_helper_t* helper, client_status_callback_fn cb, void *pCtx)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->SetClientStatusCallback) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->SetClientStatusCallback(
        helper->clientHandle, cb, pCtx);
}

static inline QVRSERVICE_VRMODE_STATE QVRServiceClient_GetVRMode(
    qvrservice_client_helper_t* helper)
{
    if(helper && helper->client->ops->GetVRMode){
        return helper->client->ops->GetVRMode(helper->clientHandle);
    }
    return VRMODE_UNSUPPORTED;
}

static inline int32_t QVRServiceClient_StartVRMode(
    qvrservice_client_helper_t* helper)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->StartVRMode) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->StartVRMode(helper->clientHandle);
}

static inline int32_t QVRServiceClient_StopVRMode(
    qvrservice_client_helper_t* helper)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->StopVRMode) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->StopVRMode(helper->clientHandle);
}

static inline int32_t QVRServiceClient_GetTrackingMode(
    qvrservice_client_helper_t* helper,
    QVRSERVICE_TRACKING_MODE *pCurrentMode, uint32_t *pSupportedModes)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->GetTrackingMode) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->GetTrackingMode(
        helper->clientHandle, pCurrentMode, pSupportedModes);
}

static inline int32_t QVRServiceClient_SetTrackingMode(
    qvrservice_client_helper_t* helper, QVRSERVICE_TRACKING_MODE mode)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->SetTrackingMode) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->SetTrackingMode(helper->clientHandle, mode);
}

static inline int32_t QVRServiceClient_SetDisplayInterruptConfig(
    qvrservice_client_helper_t* helper, QVRSERVICE_DISP_INTERRUPT_ID id,
    void *pCfg, uint32_t cfgSize)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->SetDisplayInterruptConfig) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->SetDisplayInterruptConfig(
        helper->clientHandle, id, pCfg, cfgSize);
}

static inline int32_t QVRServiceClient_SetThreadPriority(
    qvrservice_client_helper_t* helper, int tid, int policy, int priority)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->SetThreadPriority) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->SetThreadPriority(
        helper->clientHandle, tid, policy, priority);
}

static inline int32_t QVRServiceClient_GetParam(
    qvrservice_client_helper_t* helper, const char* pName, uint32_t* pLen,
    char* pValue)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->GetParam) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->GetParam(
        helper->clientHandle, pName, pLen, pValue);
}

static inline int32_t QVRServiceClient_SetParam(
    qvrservice_client_helper_t* helper, const char* pName, const char* pValue)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->SetParam) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->SetParam(
        helper->clientHandle, pName, pValue);
}

static inline int32_t QVRServiceClient_GetSensorRawData(
    qvrservice_client_helper_t* helper, qvrservice_sensor_data_raw_t **ppData)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->GetSensorRawData) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->GetSensorRawData(
        helper->clientHandle, ppData);
}

static inline int32_t QVRServiceClient_GetHeadTrackingData(
    qvrservice_client_helper_t* helper,
    qvrservice_head_tracking_data_t **ppData)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->GetHeadTrackingData) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->GetHeadTrackingData(
        helper->clientHandle, ppData);
}

static inline int32_t QVRServiceClient_GetRingBufferDescriptor(
    qvrservice_client_helper_t* helper, QVRSERVICE_RING_BUFFER_ID id,
    qvrservice_ring_buffer_desc_t *pDesc)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->GetRingBufferDescriptor) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->GetRingBufferDescriptor(
        helper->clientHandle, id, pDesc);
}

static inline int32_t QVRServiceClient_GetHistoricalHeadTrackingData(
    qvrservice_client_helper_t* helper,
    qvrservice_head_tracking_data_t **ppData, int64_t timestampNs)
{
    if(!helper) return QVR_INVALID_PARAM;
    if(!helper->client->ops->GetHistoricalHeadTrackingData) return QVR_API_NOT_SUPPORTED;

    return helper->client->ops->GetHistoricalHeadTrackingData(
        helper->clientHandle, ppData, timestampNs);
}


#ifdef __cplusplus
}
#endif

#endif /* QVRSERVICE_CLIENT_H */
