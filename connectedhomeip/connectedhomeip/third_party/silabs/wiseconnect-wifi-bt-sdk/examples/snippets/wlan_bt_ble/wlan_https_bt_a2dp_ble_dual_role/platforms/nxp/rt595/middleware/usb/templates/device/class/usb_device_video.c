/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"

#if ((defined(USB_DEVICE_CONFIG_VIDEO)) && (USB_DEVICE_CONFIG_VIDEO > 0U))
#include "usb_device_video.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static usb_status_t USB_DeviceVideoAllocateHandle(usb_device_video_struct_t **handle);
static usb_status_t USB_DeviceVideoFreeHandle(usb_device_video_struct_t *handle);
static usb_status_t USB_DeviceVideoControlIn(usb_device_handle handle,
                                             usb_device_endpoint_callback_message_struct_t *message,
                                             void *callbackParam);
static usb_status_t USB_DeviceVideoStreamIn(usb_device_handle handle,
                                            usb_device_endpoint_callback_message_struct_t *message,
                                            void *callbackParam);
static usb_status_t USB_DeviceVideoStreamOut(usb_device_handle handle,
                                             usb_device_endpoint_callback_message_struct_t *message,
                                             void *callbackParam);
static usb_status_t USB_DeviceVideoStreamEndpointsInit(usb_device_video_struct_t *videoHandle);
static usb_status_t USB_DeviceVideoStreamEndpointsDeinit(usb_device_video_struct_t *videoHandle);
static usb_status_t USB_DeviceVideoControlEndpointsInit(usb_device_video_struct_t *videoHandle);
static usb_status_t USB_DeviceVideoControlEndpointsDeinit(usb_device_video_struct_t *videoHandle);

static usb_status_t USB_DeviceVideoVcPowerModeControl(usb_device_video_struct_t *videoHandle,
                                                      usb_device_control_request_struct_t *controlRequest);
static usb_status_t USB_DeviceVideoVcInterfaceRequest(usb_device_video_struct_t *videoHandle,
                                                      usb_device_control_request_struct_t *controlRequest);
static usb_status_t USB_DeviceVideoVcCameraTerminalRequest(usb_device_video_struct_t *videoHandle,
                                                           usb_device_control_request_struct_t *controlRequest);
static usb_status_t USB_DeviceVideoVcProcessingUnitRequest(usb_device_video_struct_t *videoHandle,
                                                           usb_device_control_request_struct_t *controlRequest);
static usb_status_t USB_DeviceVideoVcRequest(usb_device_video_struct_t *videoHandle,
                                             usb_device_control_request_struct_t *controlRequest);
static usb_status_t USB_DeviceVideoVsProbeRequest(usb_device_video_struct_t *videoHandle,
                                                  usb_device_control_request_struct_t *controlRequest);
static usb_status_t USB_DeviceVideoVsCommitRequest(usb_device_video_struct_t *videoHandle,
                                                   usb_device_control_request_struct_t *controlRequest);
static usb_status_t USB_DeviceVideoVsStillProbeRequest(usb_device_video_struct_t *videoHandle,
                                                       usb_device_control_request_struct_t *controlRequest);
static usb_status_t USB_DeviceVideoVsStillCommitRequest(usb_device_video_struct_t *videoHandle,
                                                        usb_device_control_request_struct_t *controlRequest);
static usb_status_t USB_DeviceVideoVsStillImageTriggerRequest(usb_device_video_struct_t *videoHandle,
                                                              usb_device_control_request_struct_t *controlRequest);
static usb_status_t USB_DeviceVideoVsRequest(usb_device_video_struct_t *videoHandle,
                                             usb_device_control_request_struct_t *controlRequest);

/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static usb_device_video_struct_t
    s_UsbDeviceVideoHandle[USB_DEVICE_CONFIG_VIDEO];

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Allocate a device video class handle.
 *
 * This function allocates a device video class handle.
 *
 * @param handle          It is out parameter, is used to return pointer of the device video class handle to the caller.
 *
 * @retval kStatus_USB_Success              Get a device video class handle successfully.
 * @retval kStatus_USB_Busy                 Cannot allocate a device video class handle.
 */
static usb_status_t USB_DeviceVideoAllocateHandle(usb_device_video_struct_t **handle)
{
    uint32_t count;
    USB_OSA_SR_ALLOC();

    USB_OSA_ENTER_CRITICAL();
    for (count = 0U; count < USB_DEVICE_CONFIG_VIDEO; count++)
    {
        if (NULL == s_UsbDeviceVideoHandle[count].handle)
        {
            *handle = &s_UsbDeviceVideoHandle[count];
            USB_OSA_EXIT_CRITICAL();
            return kStatus_USB_Success;
        }
    }
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Busy;
}

/*!
 * @brief Free a device video class handle.
 *
 * This function frees a device video class handle.
 *
 * @param handle          The device video class handle.
 *
 * @retval kStatus_USB_Success              Free device video class handle successfully.
 */
static usb_status_t USB_DeviceVideoFreeHandle(usb_device_video_struct_t *handle)
{
    USB_OSA_SR_ALLOC();

    USB_OSA_ENTER_CRITICAL();
    handle->handle           = NULL;
    handle->configStruct     = (usb_device_class_config_struct_t *)NULL;
    handle->configuration    = 0U;
    handle->controlAlternate = 0U;
    handle->streamAlternate  = 0U;
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Success;
}

/*!
 * @brief Interrupt IN endpoint callback function.
 *
 * This callback function is used to notify uplayer the transfser result of a transfer.
 * This callback pointer is passed when the interrupt IN pipe initialized.
 *
 * @param handle          The device handle. It equals the value returned from USB_DeviceInit.
 * @param message         The result of the interrupt IN pipe transfer.
 * @param callbackParam  The parameter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam. In the class, the value is the video class handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoControlIn(usb_device_handle handle,
                                             usb_device_endpoint_callback_message_struct_t *message,
                                             void *callbackParam)
{
    usb_device_video_struct_t *videoHandle;
    usb_status_t error = kStatus_USB_Error;

    /* Get the video class handle */
    videoHandle = (usb_device_video_struct_t *)callbackParam;

    if (!videoHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if ((NULL != videoHandle->configStruct) && (videoHandle->configStruct->classCallback))
    {
        /* Notify the application control data sent by calling the video class callback.
        ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = videoHandle->configStruct->classCallback((class_handle_t)videoHandle,
                                                         kUSB_DeviceVideoEventControlSendResponse, message);
    }

    return error;
}

/*!
 * @brief ISO IN endpoint callback function.
 *
 * This callback function is used to notify uplayer the transfser result of a transfer.
 * This callback pointer is passed when the ISO IN pipe initialized.
 *
 * @param handle          The device handle. It equals the value returned from USB_DeviceInit.
 * @param message         The result of the ISO IN pipe transfer.
 * @param callbackParam  The parameter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam. In the class, the value is the video class handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoStreamIn(usb_device_handle handle,
                                            usb_device_endpoint_callback_message_struct_t *message,
                                            void *callbackParam)
{
    usb_device_video_struct_t *videoHandle;
    usb_status_t error = kStatus_USB_Error;

    /* Get the video class handle */
    videoHandle = (usb_device_video_struct_t *)callbackParam;

    if (!videoHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    videoHandle->streamInPipeBusy = 0U;
    if ((NULL != videoHandle->configStruct) && (videoHandle->configStruct->classCallback))
    {
        /* Notify the application stream data sent by calling the video class callback.
        ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = videoHandle->configStruct->classCallback((class_handle_t)videoHandle,
                                                         kUSB_DeviceVideoEventStreamSendResponse, message);
    }

    return error;
}

/*!
 * @brief ISO OUT endpoint callback function.
 *
 * This callback function is used to notify uplayer the transfser result of a transfer.
 * This callback pointer is passed when the ISO OUT pipe initialized.
 *
 * @param handle          The device handle. It equals the value returned from USB_DeviceInit.
 * @param message         The result of the ISO OUT pipe transfer.
 * @param callbackParam  The parameter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam. In the class, the value is the video class handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoStreamOut(usb_device_handle handle,
                                             usb_device_endpoint_callback_message_struct_t *message,
                                             void *callbackParam)
{
    usb_device_video_struct_t *videoHandle;
    usb_status_t error = kStatus_USB_Error;

    /* Get the video class handle */
    videoHandle = (usb_device_video_struct_t *)callbackParam;

    if (!videoHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    videoHandle->streamOutPipeBusy = 0U;
    if ((NULL != videoHandle->configStruct) && (videoHandle->configStruct->classCallback))
    {
        /* Notify the application stream data sent by calling the video class callback.
        ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = videoHandle->configStruct->classCallback((class_handle_t)videoHandle,
                                                         kUSB_DeviceVideoEventStreamRecvResponse, message);
    }

    return error;
}

/*!
 * @brief Initialize the stream endpoints of the video class.
 *
 * This callback function is used to initialize the stream endpoints of the video class.
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoStreamEndpointsInit(usb_device_video_struct_t *videoHandle)
{
    usb_device_interface_list_t *interfaceList;
    usb_device_interface_struct_t *interface = (usb_device_interface_struct_t *)NULL;
    usb_status_t error                       = kStatus_USB_Error;

    /* Check the configuration is valid or not. */
    if (!videoHandle->configuration)
    {
        return error;
    }

    if (videoHandle->configuration > videoHandle->configStruct->classInfomation->configurations)
    {
        return error;
    }

    /* Get the interface list of the new configuration. */
    if (NULL == videoHandle->configStruct->classInfomation->interfaceList)
    {
        return error;
    }
    interfaceList = &videoHandle->configStruct->classInfomation->interfaceList[videoHandle->configuration - 1U];

    /* Find stream interface by using the alternate setting of the interface. */
    for (int count = 0U; count < interfaceList->count; count++)
    {
        if ((USB_DEVICE_VIDEO_CC_VIDEO == interfaceList->interfaces[count].classCode) &&
            (USB_DEVICE_VIDEO_SC_VIDEOSTREAMING == interfaceList->interfaces[count].subclassCode))
        {
            for (int index = 0U; index < interfaceList->interfaces[count].count; index++)
            {
                if (interfaceList->interfaces[count].interface[index].alternateSetting == videoHandle->streamAlternate)
                {
                    interface = &interfaceList->interfaces[count].interface[index];
                    break;
                }
            }
            videoHandle->streamInterfaceNumber = interfaceList->interfaces[count].interfaceNumber;
            break;
        }
    }
    if (!interface)
    {
        /* Return error if the stream interface is not found. */
        return error;
    }

    /* Keep new stream interface handle. */
    videoHandle->streamInterfaceHandle = interface;

    /* Initialize the endpoints of the new interface. */
    for (int count = 0U; count < interface->endpointList.count; count++)
    {
        usb_device_endpoint_init_struct_t epInitStruct;
        usb_device_endpoint_callback_struct_t epCallback;
        epInitStruct.zlt             = 0U;
        epInitStruct.interval        = interface->endpointList.endpoint[count].interval;
        epInitStruct.endpointAddress = interface->endpointList.endpoint[count].endpointAddress;
        epInitStruct.maxPacketSize   = interface->endpointList.endpoint[count].maxPacketSize;
        epInitStruct.transferType    = interface->endpointList.endpoint[count].transferType;

        if ((USB_ENDPOINT_ISOCHRONOUS == (epInitStruct.transferType & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK)) &&
            (USB_IN == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)))
        {
            epCallback.callbackFn = USB_DeviceVideoStreamIn;
        }
        else if ((USB_ENDPOINT_ISOCHRONOUS ==
                  (epInitStruct.transferType & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK)) &&
                 (USB_OUT == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                              USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)))
        {
            epCallback.callbackFn = USB_DeviceVideoStreamOut;
        }
        else
        {
            continue;
        }
        epCallback.callbackParam = videoHandle;

        error = USB_DeviceInitEndpoint(videoHandle->handle, &epInitStruct, &epCallback);
    }
    return error;
}

/*!
 * @brief De-initialize the stream endpoints of the video class.
 *
 * This callback function is used to de-initialize the stream endpoints of the video class.
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoStreamEndpointsDeinit(usb_device_video_struct_t *videoHandle)
{
    usb_status_t error = kStatus_USB_Error;

    if (!videoHandle->streamInterfaceHandle)
    {
        return error;
    }
    /* De-initialize all stream endpoints of the interface */
    for (int count = 0U; count < videoHandle->streamInterfaceHandle->endpointList.count; count++)
    {
        error = USB_DeviceDeinitEndpoint(
            videoHandle->handle, videoHandle->streamInterfaceHandle->endpointList.endpoint[count].endpointAddress);
    }
    videoHandle->streamInterfaceHandle = NULL;
    return error;
}

/*!
 * @brief Initialize the control endpoints of the video class.
 *
 * This callback function is used to initialize the control endpoints of the video class.
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoControlEndpointsInit(usb_device_video_struct_t *videoHandle)
{
    usb_device_interface_list_t *interfaceList;
    usb_device_interface_struct_t *interface = (usb_device_interface_struct_t *)NULL;
    usb_status_t error                       = kStatus_USB_Error;

    /* Check the configuration is valid or not. */
    if (!videoHandle->configuration)
    {
        return error;
    }

    if (videoHandle->configuration > videoHandle->configStruct->classInfomation->configurations)
    {
        return error;
    }

    /* Get the control interface list of the new configuration. */
    if (NULL == videoHandle->configStruct->classInfomation->interfaceList)
    {
        return error;
    }
    interfaceList = &videoHandle->configStruct->classInfomation->interfaceList[videoHandle->configuration - 1U];

    /* Find control interface by using the alternate setting of the interface. */
    for (int count = 0U; count < interfaceList->count; count++)
    {
        if ((USB_DEVICE_VIDEO_CC_VIDEO == interfaceList->interfaces[count].classCode) &&
            (USB_DEVICE_VIDEO_SC_VIDEOCONTROL == interfaceList->interfaces[count].subclassCode))
        {
            for (int index = 0U; index < interfaceList->interfaces[count].count; index++)
            {
                if (interfaceList->interfaces[count].interface[index].alternateSetting == videoHandle->controlAlternate)
                {
                    interface = &interfaceList->interfaces[count].interface[index];
                    break;
                }
            }
            videoHandle->controlInterfaceNumber = interfaceList->interfaces[count].interfaceNumber;
            break;
        }
    }
    if (!interface)
    {
        /* Return error if the control interface is not found. */
        return error;
    }
    /* Keep new control interface handle. */
    videoHandle->controlInterfaceHandle = interface;

    /* Initialize the control endpoints of the new interface. */
    for (int count = 0U; count < interface->endpointList.count; count++)
    {
        usb_device_endpoint_init_struct_t epInitStruct;
        usb_device_endpoint_callback_struct_t epCallback;
        epInitStruct.zlt             = 0U;
        epInitStruct.endpointAddress = interface->endpointList.endpoint[count].endpointAddress;
        epInitStruct.maxPacketSize   = interface->endpointList.endpoint[count].maxPacketSize;
        epInitStruct.transferType    = interface->endpointList.endpoint[count].transferType;

        if ((USB_ENDPOINT_INTERRUPT == (epInitStruct.transferType & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK)) &&
            (USB_IN == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)))
        {
            epCallback.callbackFn = USB_DeviceVideoControlIn;
        }
        else
        {
            continue;
        }
        epCallback.callbackParam = videoHandle;

        error = USB_DeviceInitEndpoint(videoHandle->handle, &epInitStruct, &epCallback);
    }
    return error;
}

/*!
 * @brief De-initialize the control endpoints of the video class.
 *
 * This callback function is used to de-initialize the control endpoints of the video class.
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoControlEndpointsDeinit(usb_device_video_struct_t *videoHandle)
{
    usb_status_t error = kStatus_USB_Error;

    if (!videoHandle->controlInterfaceHandle)
    {
        return error;
    }
    /* De-initialize all control endpoints of the interface */
    for (int count = 0U; count < videoHandle->controlInterfaceHandle->endpointList.count; count++)
    {
        error = USB_DeviceDeinitEndpoint(
            videoHandle->handle, videoHandle->controlInterfaceHandle->endpointList.endpoint[count].endpointAddress);
    }
    videoHandle->controlInterfaceHandle = NULL;
    return error;
}

/*!
 * @brief Handle the video device vc power modex control request.
 *
 * This callback function is used to handle the video device vc power modex control request.
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 * @param controlRequest       The pointer of the control request structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoVcPowerModeControl(usb_device_video_struct_t *videoHandle,
                                                      usb_device_control_request_struct_t *controlRequest)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint32_t command   = 0U;

    switch (controlRequest->setup->bRequest)
    {
        case USB_DEVICE_VIDEO_REQUEST_CODE_SET_CUR:
            command = USB_DEVICE_VIDEO_SET_CUR_VC_POWER_MODE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_CUR:
            command = USB_DEVICE_VIDEO_GET_CUR_VC_POWER_MODE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_INFO:
            command = USB_DEVICE_VIDEO_GET_INFO_VC_POWER_MODE_CONTROL;
            break;
        default:
            break;
    }
    if ((command) && (NULL != videoHandle->configStruct) && (videoHandle->configStruct->classCallback))
    {
        /* ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = videoHandle->configStruct->classCallback((class_handle_t)videoHandle, command, controlRequest);
    }
    return error;
}

/*!
 * @brief Handle the video device control interface class-specific request.
 *
 * This callback function is used to handle the video device control pipe class-specific request.
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 * @param controlRequest       The pointer of the control request structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoVcInterfaceRequest(usb_device_video_struct_t *videoHandle,
                                                      usb_device_control_request_struct_t *controlRequest)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint8_t cs         = (controlRequest->setup->wValue >> 0x08U) & 0xFFU;

    if (USB_DEVICE_VIDEO_VC_VIDEO_POWER_MODE_CONTROL == cs)
    {
        /* handle the video device vc power modex control request */
        error = USB_DeviceVideoVcPowerModeControl(videoHandle, controlRequest);
    }
    else if (USB_DEVICE_VIDEO_VC_REQUEST_ERROR_CODE_CONTROL == cs)
    {
    }
    else
    {
    }
    return error;
}

/*!
 * @brief Handle the video device camera terminal control request.
 *
 * This callback function is used to handle the video device camera terminal control request.
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 * @param controlRequest       The pointer of the control request structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoVcCameraTerminalRequest(usb_device_video_struct_t *videoHandle,
                                                           usb_device_control_request_struct_t *controlRequest)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint8_t cs         = (controlRequest->setup->wValue >> 0x08U) & 0xFFU;

    switch (cs)
    {
        case USB_DEVICE_VIDEO_CT_SCANNING_MODE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_AE_MODE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_AE_PRIORITY_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_EXPOSURE_TIME_RELATIVE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_FOCUS_ABSOLUTE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_FOCUS_RELATIVE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_FOCUS_AUTO_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_IRIS_ABSOLUTE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_IRIS_RELATIVE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_ZOOM_ABSOLUTE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_ZOOM_RELATIVE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_PANTILT_ABSOLUTE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_PANTILT_RELATIVE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_ROLL_ABSOLUTE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_ROLL_RELATIVE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_PRIVACY_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_FOCUS_SIMPLE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_WINDOW_CONTROL:
            break;
        case USB_DEVICE_VIDEO_CT_REGION_OF_INTEREST_CONTROL:
            break;
        default:
            break;
    }
    return error;
}

/*!
 * @brief Handle the video device processing uint control request.
 *
 * This callback function is used to handle the video device processing uint control request.
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 * @param controlRequest       The pointer of the control request structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoVcProcessingUnitRequest(usb_device_video_struct_t *videoHandle,
                                                           usb_device_control_request_struct_t *controlRequest)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint8_t cs         = (controlRequest->setup->wValue >> 0x08U) & 0xFFU;

    switch (cs)
    {
        case USB_DEVICE_VIDEO_PU_BACKLIGHT_COMPENSATION_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_BRIGHTNESS_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_CONTRAST_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_GAIN_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_POWER_LINE_FREQUENCY_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_HUE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_SATURATION_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_SHARPNESS_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_GAMMA_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_WHITE_BALANCE_COMPONENT_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_DIGITAL_MULTIPLIER_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_HUE_AUTO_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_ANALOG_VIDEO_STANDARD_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_ANALOG_LOCK_STATUS_CONTROL:
            break;
        case USB_DEVICE_VIDEO_PU_CONTRAST_AUTO_CONTROL:
            break;
        default:
            break;
    }

    return error;
}

/*!
 * @brief Handle the video device control request.
 *
 * This callback function is used to handle the video device control request,
 * including the control interface, uint or terminal control.
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 * @param controlRequest       The pointer of the control request structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoVcRequest(usb_device_video_struct_t *videoHandle,
                                             usb_device_control_request_struct_t *controlRequest)
{
    usb_device_video_entities_struct_t *entity_list;
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint8_t entityId   = (uint8_t)(controlRequest->setup->wIndex >> 0x08U);

    if (!videoHandle->controlInterfaceHandle)
    {
        return error;
    }

    /* Get the video class-specific information. */
    entity_list = (usb_device_video_entities_struct_t *)videoHandle->controlInterfaceHandle->classSpecific;

    if (0U == entityId)
    {
        /* Handle the video control interface request. */
        error = USB_DeviceVideoVcInterfaceRequest(videoHandle, controlRequest);
    }
    else
    {
        for (int i = 0U; i < entity_list->count; i++)
        {
            if (entityId == entity_list->entity[i].entityId)
            {
                switch (entity_list->entity[i].entityType)
                {
                    case USB_DESCRIPTOR_SUBTYPE_VIDEO_VC_INPUT_TERMINAL:
                        if (USB_DEVICE_VIDEO_ITT_CAMERA == entity_list->entity[i].terminalType)
                        {
                            /* Handle the video camera terminal control request. */
                            error = USB_DeviceVideoVcCameraTerminalRequest(videoHandle, controlRequest);
                        }
                        break;
                    case USB_DESCRIPTOR_SUBTYPE_VIDEO_VC_OUTPUT_TERMINAL:
                        break;
                    case USB_DESCRIPTOR_SUBTYPE_VIDEO_VC_SELECTOR_UNIT:
                        break;
                    case USB_DESCRIPTOR_SUBTYPE_VIDEO_VC_PROCESSING_UNIT:
                        /* Handle the video processing uint control request. */
                        error = USB_DeviceVideoVcProcessingUnitRequest(videoHandle, controlRequest);
                        break;
                    case USB_DESCRIPTOR_SUBTYPE_VIDEO_VC_EXTENSION_UNIT:
                        break;
                    case USB_DESCRIPTOR_SUBTYPE_VIDEO_VC_ENCODING_UNIT:
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return error;
}

/*!
 * @brief Handle the video device probe control request.
 *
 * This callback function is used to handle the video device probe control request,
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 * @param controlRequest       The pointer of the control request structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoVsProbeRequest(usb_device_video_struct_t *videoHandle,
                                                  usb_device_control_request_struct_t *controlRequest)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint32_t command   = 0U;

    switch (controlRequest->setup->bRequest)
    {
        case USB_DEVICE_VIDEO_REQUEST_CODE_SET_CUR:
            command = USB_DEVICE_VIDEO_SET_CUR_VS_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_CUR:
            command = USB_DEVICE_VIDEO_GET_CUR_VS_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_MIN:
            command = USB_DEVICE_VIDEO_GET_MIN_VS_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_MAX:
            command = USB_DEVICE_VIDEO_GET_MAX_VS_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_RES:
            command = USB_DEVICE_VIDEO_GET_RES_VS_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_LEN:
            command = USB_DEVICE_VIDEO_GET_LEN_VS_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_INFO:
            command = USB_DEVICE_VIDEO_GET_INFO_VS_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_DEF:
            command = USB_DEVICE_VIDEO_GET_DEF_VS_PROBE_CONTROL;
            break;
        default:
            break;
    }
    if ((command) && (NULL != videoHandle->configStruct) && (videoHandle->configStruct->classCallback))
    {
        /*ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
  it is from the second parameter of classInit*/
        error = videoHandle->configStruct->classCallback((class_handle_t)videoHandle, command, controlRequest);
    }
    return error;
}

/*!
 * @brief Handle the video device commit control request.
 *
 * This callback function is used to handle the video device commit control request,
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 * @param controlRequest       The pointer of the control request structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoVsCommitRequest(usb_device_video_struct_t *videoHandle,
                                                   usb_device_control_request_struct_t *controlRequest)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint32_t command   = 0U;

    switch (controlRequest->setup->bRequest)
    {
        case USB_DEVICE_VIDEO_REQUEST_CODE_SET_CUR:
            command = USB_DEVICE_VIDEO_SET_CUR_VS_COMMIT_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_CUR:
            command = USB_DEVICE_VIDEO_GET_CUR_VS_COMMIT_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_LEN:
            command = USB_DEVICE_VIDEO_GET_LEN_VS_COMMIT_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_INFO:
            command = USB_DEVICE_VIDEO_GET_INFO_VS_COMMIT_CONTROL;
            break;
        default:
            break;
    }
    if ((command) && (NULL != videoHandle->configStruct) && (videoHandle->configStruct->classCallback))
    {
        /* ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = videoHandle->configStruct->classCallback((class_handle_t)videoHandle, command, controlRequest);
    }
    return error;
}

/*!
 * @brief Handle the video device still probe control request.
 *
 * This callback function is used to handle the video device still probe control request,
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 * @param controlRequest       The pointer of the control request structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoVsStillProbeRequest(usb_device_video_struct_t *videoHandle,
                                                       usb_device_control_request_struct_t *controlRequest)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint32_t command   = 0U;

    switch (controlRequest->setup->bRequest)
    {
        case USB_DEVICE_VIDEO_REQUEST_CODE_SET_CUR:
            command = USB_DEVICE_VIDEO_SET_CUR_VS_STILL_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_CUR:
            command = USB_DEVICE_VIDEO_GET_CUR_VS_STILL_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_MIN:
            command = USB_DEVICE_VIDEO_GET_MIN_VS_STILL_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_MAX:
            command = USB_DEVICE_VIDEO_GET_MAX_VS_STILL_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_RES:
            command = USB_DEVICE_VIDEO_GET_RES_VS_STILL_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_LEN:
            command = USB_DEVICE_VIDEO_GET_LEN_VS_STILL_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_INFO:
            command = USB_DEVICE_VIDEO_GET_INFO_VS_STILL_PROBE_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_DEF:
            command = USB_DEVICE_VIDEO_GET_DEF_VS_STILL_PROBE_CONTROL;
            break;
        default:
            break;
    }
    if ((command) && (NULL != videoHandle->configStruct) && (videoHandle->configStruct->classCallback))
    {
        /*ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
  it is from the second parameter of classInit*/
        error = videoHandle->configStruct->classCallback((class_handle_t)videoHandle, command, controlRequest);
    }
    return error;
}

/*!
 * @brief Handle the video device still commit control request.
 *
 * This callback function is used to handle the video device still commit control request,
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 * @param controlRequest       The pointer of the control request structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoVsStillCommitRequest(usb_device_video_struct_t *videoHandle,
                                                        usb_device_control_request_struct_t *controlRequest)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint32_t command   = 0U;

    switch (controlRequest->setup->bRequest)
    {
        case USB_DEVICE_VIDEO_REQUEST_CODE_SET_CUR:
            command = USB_DEVICE_VIDEO_SET_CUR_VS_STILL_COMMIT_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_CUR:
            command = USB_DEVICE_VIDEO_SET_CUR_VS_STILL_COMMIT_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_LEN:
            command = USB_DEVICE_VIDEO_SET_CUR_VS_STILL_COMMIT_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_INFO:
            command = USB_DEVICE_VIDEO_SET_CUR_VS_STILL_COMMIT_CONTROL;
            break;
        default:
            break;
    }
    if ((command) && (NULL != videoHandle->configStruct) && (videoHandle->configStruct->classCallback))
    {
        /* ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = videoHandle->configStruct->classCallback((class_handle_t)videoHandle, command, controlRequest);
    }
    return error;
}

/*!
 * @brief Handle the video device still image trigger control request.
 *
 * This callback function is used to handle the video device still image trigger control request,
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 * @param controlRequest       The pointer of the control request structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoVsStillImageTriggerRequest(usb_device_video_struct_t *videoHandle,
                                                              usb_device_control_request_struct_t *controlRequest)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint32_t command   = 0U;

    switch (controlRequest->setup->bRequest)
    {
        case USB_DEVICE_VIDEO_REQUEST_CODE_SET_CUR:
            command = USB_DEVICE_VIDEO_SET_CUR_VS_STILL_IMAGE_TRIGGER_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_CUR:
            command = USB_DEVICE_VIDEO_GET_CUR_VS_STILL_IMAGE_TRIGGER_CONTROL;
            break;
        case USB_DEVICE_VIDEO_REQUEST_CODE_GET_INFO:
            command = USB_DEVICE_VIDEO_GET_INFO_VS_STILL_IMAGE_TRIGGER_CONTROL;
            break;
        default:
            break;
    }
    if ((command) && (NULL != videoHandle->configStruct) && (videoHandle->configStruct->classCallback))
    {
        /* ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = videoHandle->configStruct->classCallback((class_handle_t)videoHandle, command, controlRequest);
    }
    return error;
}

/*!
 * @brief Handle the video device stream interface control request.
 *
 * This callback function is used to handle the video devicestream interface controll request,
 *
 * @param videoHandle          The device video class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 * @param controlRequest       The pointer of the control request structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceVideoVsRequest(usb_device_video_struct_t *videoHandle,
                                             usb_device_control_request_struct_t *controlRequest)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint8_t cs         = (controlRequest->setup->wValue >> 0x08U) & 0xFFU;

    switch (cs)
    {
        case USB_DEVICE_VIDEO_VS_PROBE_CONTROL:
            /* Handle device video probe control request. */
            error = USB_DeviceVideoVsProbeRequest(videoHandle, controlRequest);
            break;
        case USB_DEVICE_VIDEO_VS_COMMIT_CONTROL:
            /* Handle device video commit control request. */
            error = USB_DeviceVideoVsCommitRequest(videoHandle, controlRequest);
            break;
        case USB_DEVICE_VIDEO_VS_STILL_PROBE_CONTROL:
            /* Handle device video still probe control request. */
            error = USB_DeviceVideoVsStillProbeRequest(videoHandle, controlRequest);
            break;
        case USB_DEVICE_VIDEO_VS_STILL_COMMIT_CONTROL:
            /* Handle device video still commit control request. */
            error = USB_DeviceVideoVsStillCommitRequest(videoHandle, controlRequest);
            break;
        case USB_DEVICE_VIDEO_VS_STILL_IMAGE_TRIGGER_CONTROL:
            /* Handle device video still image trigger control request. */
            error = USB_DeviceVideoVsStillImageTriggerRequest(videoHandle, controlRequest);
            break;
        case USB_DEVICE_VIDEO_VS_STREAM_ERROR_CODE_CONTROL:
            break;
        case USB_DEVICE_VIDEO_VS_GENERATE_KEY_FRAME_CONTROL:
            break;
        case USB_DEVICE_VIDEO_VS_UPDATE_FRAME_SEGMENT_CONTROL:
            break;
        case USB_DEVICE_VIDEO_VS_SYNCH_DELAY_CONTROL:
            break;
        default:
            break;
    }

    return error;
}

/*!
 * @brief Handle the event passed to the video class.
 *
 * This function handles the event passed to the video class.
 *
 * @param handle          The video class handle, got from the usb_device_class_config_struct_t::classHandle.
 * @param event           The event codes. Please refer to the enumeration usb_device_class_event_t.
 * @param param           The param type is determined by the event code.
 *
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success              Free device handle successfully.
 * @retval kStatus_USB_InvalidParameter     The device handle not be found.
 * @retval kStatus_USB_InvalidRequest       The request is invalid, and the control pipe will be stalled by the caller.
 */
usb_status_t USB_DeviceVideoEvent(void *handle, uint32_t event, void *param)
{
    usb_device_video_struct_t *videoHandle;
    usb_status_t error = kStatus_USB_Error;
    uint16_t interfaceAlternate;
    uint8_t *temp8;
    uint8_t alternate;

    if ((!param) || (!handle))
    {
        return kStatus_USB_InvalidHandle;
    }

    /* Get the video class handle. */
    videoHandle = (usb_device_video_struct_t *)handle;

    switch (event)
    {
        case kUSB_DeviceClassEventDeviceReset:
            /* Bus reset, clear the configuration. */
            videoHandle->configuration     = 0U;
            videoHandle->streamOutPipeBusy = 0U;
            videoHandle->streamInPipeBusy  = 0U;
            break;
        case kUSB_DeviceClassEventSetConfiguration:
            /* Get the new configuration. */
            temp8 = ((uint8_t *)param);
            if (!videoHandle->configStruct)
            {
                break;
            }
            if (*temp8 == videoHandle->configuration)
            {
                break;
            }
            /* De-initialize the endpoints when current configuration is none zero. */
            if (videoHandle->configuration)
            {
                error = USB_DeviceVideoControlEndpointsDeinit(videoHandle);
                error = USB_DeviceVideoStreamEndpointsDeinit(videoHandle);
            }
            /* Save new configuration. */
            videoHandle->configuration = *temp8;
            /* Clear the alternate setting value. */
            videoHandle->controlAlternate = 0U;
            /* Clear the alternate setting value. */
            videoHandle->streamAlternate        = 0U;
            videoHandle->controlInterfaceHandle = NULL;
            videoHandle->streamInterfaceHandle  = NULL;
            /* Initialize the control endpoints of the new current configuration by using the alternate setting 0. */
            error = USB_DeviceVideoControlEndpointsInit(videoHandle);
            /* Initialize the stream endpoints of the new current configuration by using the alternate setting 0. */
            error = USB_DeviceVideoStreamEndpointsInit(videoHandle);
            break;
        case kUSB_DeviceClassEventSetInterface:
            if (!videoHandle->configStruct)
            {
                break;
            }
            /* Get the new alternate setting of the interface */
            interfaceAlternate = *((uint16_t *)param);
            /* Get the alternate setting value */
            alternate = (uint8_t)(interfaceAlternate & 0xFFU);

            /* Whether the interface belongs to the class. */
            if (videoHandle->controlInterfaceNumber == ((uint8_t)(interfaceAlternate >> 8U)))
            {
                /* When the interface is control interface. */
                /* Only handle new alternate setting. */
                if (alternate == videoHandle->controlAlternate)
                {
                    break;
                }
                /* De-initialize old endpoints */
                error                         = USB_DeviceVideoControlEndpointsDeinit(videoHandle);
                videoHandle->controlAlternate = alternate;
                /* Initialize new endpoints */
                error = USB_DeviceVideoControlEndpointsInit(videoHandle);
            }
            else if (videoHandle->streamInterfaceNumber == ((uint8_t)(interfaceAlternate >> 8U)))
            {
                /* When the interface is stream interface. */
                /* Only handle new alternate setting. */
                if (alternate == videoHandle->streamAlternate)
                {
                    break;
                }
                /* De-initialize old endpoints */
                error                        = USB_DeviceVideoStreamEndpointsDeinit(videoHandle);
                videoHandle->streamAlternate = alternate;
                /* Initialize new endpoints */
                error = USB_DeviceVideoStreamEndpointsInit(videoHandle);
            }
            else
            {
            }
            break;
        case kUSB_DeviceClassEventSetEndpointHalt:
            if (!videoHandle->configStruct)
            {
                break;
            }
            /* Get the endpoint address */
            temp8 = ((uint8_t *)param);
            if (videoHandle->controlInterfaceHandle)
            {
                for (int count = 0U; count < videoHandle->controlInterfaceHandle->endpointList.count; count++)
                {
                    if (*temp8 == videoHandle->controlInterfaceHandle->endpointList.endpoint[count].endpointAddress)
                    {
                        /* Only stall the endpoint belongs to control interface of the class */
                        error = USB_DeviceStallEndpoint(videoHandle->handle, *temp8);
                    }
                }
            }
            if (videoHandle->streamInterfaceHandle)
            {
                for (int count = 0U; count < videoHandle->streamInterfaceHandle->endpointList.count; count++)
                {
                    if (*temp8 == videoHandle->streamInterfaceHandle->endpointList.endpoint[count].endpointAddress)
                    {
                        /* Only stall the endpoint belongs to stream interface of the class */
                        error = USB_DeviceStallEndpoint(videoHandle->handle, *temp8);
                    }
                }
            }
            break;
        case kUSB_DeviceClassEventClearEndpointHalt:
            if (!videoHandle->configStruct)
            {
                break;
            }
            /* Get the endpoint address */
            temp8 = ((uint8_t *)param);
            if (videoHandle->controlInterfaceHandle)
            {
                for (int count = 0U; count < videoHandle->controlInterfaceHandle->endpointList.count; count++)
                {
                    if (*temp8 == videoHandle->controlInterfaceHandle->endpointList.endpoint[count].endpointAddress)
                    {
                        /* Only un-stall the endpoint belongs to control interface of the class */
                        error = USB_DeviceUnstallEndpoint(videoHandle->handle, *temp8);
                    }
                }
            }
            if (videoHandle->streamInterfaceHandle)
            {
                for (int count = 0U; count < videoHandle->streamInterfaceHandle->endpointList.count; count++)
                {
                    if (*temp8 == videoHandle->streamInterfaceHandle->endpointList.endpoint[count].endpointAddress)
                    {
                        /* Only un-stall the endpoint belongs to stream interface of the class */
                        error = USB_DeviceUnstallEndpoint(videoHandle->handle, *temp8);
                    }
                }
            }
            break;
        case kUSB_DeviceClassEventClassRequest:
            if (param)
            {
                /* Handle the video class specific request. */
                usb_device_control_request_struct_t *controlRequest = (usb_device_control_request_struct_t *)param;
                uint8_t interface_index                             = (uint8_t)controlRequest->setup->wIndex;

                if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) !=
                    USB_REQUEST_TYPE_RECIPIENT_INTERFACE)
                {
                    break;
                }

                switch (controlRequest->setup->bmRequestType)
                {
                    case USB_DEVICE_VIDEO_SET_REQUEST_INTERFACE:
                        if (controlRequest->isSetup)
                        {
                            /* Get the buffer to receive the data sent from the host. */
                            if ((NULL != videoHandle->configStruct) && (videoHandle->configStruct->classCallback))
                            {
                                /*ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                                                  it is from the second parameter of classInit*/
                                error = videoHandle->configStruct->classCallback(
                                    (class_handle_t)videoHandle, kUSB_DeviceVideoEventClassRequestBuffer,
                                    controlRequest);
                            }
                            break;
                        }
                    case USB_DEVICE_VIDEO_GET_REQUEST_INTERFACE:
                        if (videoHandle->controlInterfaceNumber == interface_index)
                        {
                            error = USB_DeviceVideoVcRequest(videoHandle, controlRequest);
                        }
                        else if (videoHandle->streamInterfaceNumber == interface_index)
                        {
                            error = USB_DeviceVideoVsRequest(videoHandle, controlRequest);
                        }
                        else
                        {
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        default:
            break;
    }
    return error;
}

/*!
 * @brief Initialize the video class.
 *
 * This function is used to initialize the video class.
 *
 * @param controllerId   The controller id of the USB IP. Please refer to the enumeration usb_controller_index_t.
 * @param config          The class configuration information.
 * @param handle          It is out parameter, is used to return pointer of the video class handle to the caller.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceVideoInit(uint8_t controllerId, usb_device_class_config_struct_t *config, class_handle_t *handle)
{
    usb_device_video_struct_t *videoHandle;
    usb_status_t error = kStatus_USB_Error;

    /* Allocate a video class handle. */
    error = USB_DeviceVideoAllocateHandle(&videoHandle);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    /* Get the device handle according to the controller id. */
    error = USB_DeviceClassGetDeviceHandle(controllerId, &videoHandle->handle);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    if (!videoHandle->handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    /* Save the configuration of the class. */
    videoHandle->configStruct = config;
    /* Clear the configuration value. */
    videoHandle->configuration    = 0U;
    videoHandle->streamAlternate  = 0xffU;
    videoHandle->controlAlternate = 0xffU;

    *handle = (class_handle_t)videoHandle;
    return error;
}

/*!
 * @brief De-initialize the device video class.
 *
 * The function de-initializes the device video class.
 *
 * @param handle The ccid class handle got from usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceVideoDeinit(class_handle_t handle)
{
    usb_device_video_struct_t *videoHandle;
    usb_status_t error = kStatus_USB_Error;

    videoHandle = (usb_device_video_struct_t *)handle;

    if (!videoHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    /* De-initialzie the stream endpoints. */
    error = USB_DeviceVideoStreamEndpointsDeinit(videoHandle);
    /* De-initialzie the control endpoints. */
    error = USB_DeviceVideoControlEndpointsDeinit(videoHandle);
    /* Free the video class handle. */
    USB_DeviceVideoFreeHandle(videoHandle);
    return error;
}

/*!
 * @brief Send data through a specified endpoint.
 *
 * The function is used to send data through a specified endpoint.
 * The function calls USB_DeviceSendRequest internally.
 *
 * @param handle The video class handle got from usb_device_class_config_struct_t::classHandle.
 * @param endpointAddress Endpoint index.
 * @param buffer The memory address to hold the data need to be sent.
 * @param length The data length need to be sent.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The return value just means if the sending request is successful or not; the transfer done is notified by
 * USB_DeviceVideoStreamIn or USB_DeviceVideoControlIn.
 * Currently, only one transfer request can be supported for one specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for one specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer could begin only when the previous transfer is done (get notification through the endpoint
 * callback).
 */
usb_status_t USB_DeviceVideoSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_video_struct_t *videoHandle;
    usb_status_t error = kStatus_USB_Error;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    videoHandle = (usb_device_video_struct_t *)handle;

    if (videoHandle->streamInPipeBusy)
    {
        return kStatus_USB_Busy;
    }
    videoHandle->streamInPipeBusy = 1U;

    error = USB_DeviceSendRequest(videoHandle->handle, ep, buffer, length);
    if (kStatus_USB_Success != error)
    {
        videoHandle->streamInPipeBusy = 0U;
    }
    return error;
}

/*!
 * @brief Receive data through a specified endpoint.
 *
 * The function is used to receive data through a specified endpoint.
 * The function calls USB_DeviceRecvRequest internally.
 *
 * @param handle The video class handle got from usb_device_class_config_struct_t::classHandle.
 * @param endpointAddress Endpoint index.
 * @param buffer The memory address to save the received data.
 * @param length The data length want to be received.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The return value just means if the receiving request is successful or not; the transfer done is notified by
 * USB_DeviceVideoStreamOut.
 * Currently, only one transfer request can be supported for one specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for one specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer could begin only when the previous transfer is done (get notification through the endpoint
 * callback).
 */
usb_status_t USB_DeviceVideoRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_video_struct_t *videoHandle;
    usb_status_t error = kStatus_USB_Error;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    videoHandle = (usb_device_video_struct_t *)handle;

    if (videoHandle->streamOutPipeBusy)
    {
        return kStatus_USB_Busy;
    }
    videoHandle->streamOutPipeBusy = 1U;

    error = USB_DeviceRecvRequest(videoHandle->handle, ep, buffer, length);
    if (kStatus_USB_Success != error)
    {
        videoHandle->streamOutPipeBusy = 0U;
    }
    return error;
}

#endif
