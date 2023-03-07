/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"

#if ((defined(USB_DEVICE_CONFIG_PHDC)) && (USB_DEVICE_CONFIG_PHDC > 0U))
#include "usb_device_phdc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*! @brief local function prototypes */
static usb_status_t USB_DevicePhdcAllocateHandle(usb_device_phdc_struct_t **handle);
static usb_status_t USB_DevicePhdcFreeHandle(usb_device_phdc_struct_t *handle);
static usb_status_t USB_DevicePhdcBulkInCallback(usb_device_handle handle,
                                                 usb_device_endpoint_callback_message_struct_t *message,
                                                 void *callbackParam);
static usb_status_t USB_DevicePhdcBulkOutCallback(usb_device_handle handle,
                                                  usb_device_endpoint_callback_message_struct_t *message,
                                                  void *callbackParam);
static usb_status_t USB_DevicePhdcInterruptInCallback(usb_device_handle handle,
                                                      usb_device_endpoint_callback_message_struct_t *message,
                                                      void *callbackParam);
static usb_status_t USB_DevicePhdcEndpointsInit(usb_device_phdc_struct_t *phdcHandle);
static usb_status_t USB_DevicePhdcEndpointsDeinit(usb_device_phdc_struct_t *phdcHandle);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief the PHDC device instance */
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) usb_device_phdc_struct_t g_phdcHandle[USB_DEVICE_CONFIG_PHDC];

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Allocate a device PHDC class handle.
 *
 * This function allocates a device PHDC class handle.
 *
 * @param handle          It is out parameter, is used to return pointer of the device PHDC class handle to the caller.
 *
 * @retval kStatus_USB_Success              Get a device PHDC class handle successfully.
 * @retval kStatus_USB_Busy                 Cannot allocate a device PHDC class handle.
 */
static usb_status_t USB_DevicePhdcAllocateHandle(usb_device_phdc_struct_t **handle)
{
    uint32_t count;
    for (count = 0; count < USB_DEVICE_CONFIG_PHDC; count++)
    {
        if (NULL == g_phdcHandle[count].handle)
        {
            *handle = &g_phdcHandle[count];
            return kStatus_USB_Success;
        }
    }
    return kStatus_USB_Busy;
}

/*!
 * @brief Free a device PHDC class handle.
 *
 * This function frees a device PHDC class handle.
 *
 * @param handle          The device PHDC class handle.
 *
 * @retval kStatus_USB_Success              Free device PHDC class handle successfully.
 */
static usb_status_t USB_DevicePhdcFreeHandle(usb_device_phdc_struct_t *handle)
{
    handle->handle        = NULL;
    handle->configStruct  = (usb_device_class_config_struct_t *)NULL;
    handle->configuration = 0U;
    handle->alternate     = 0U;
    return kStatus_USB_Success;
}

/*!
 * @brief bulk IN endpoint callback function.
 *
 * This callback function is used to notify upper layer the transfer result of a transfer.
 * This callback pointer is passed when the bulk IN pipe initialized.
 *
 * @param handle          The device handle. It equals the value returned from USB_DeviceInit.
 * @param message         The result of the bulk IN pipe transfer.
 * @param callbackParam   The parameter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam.
 *                        In the class, the value is the PHDC class handle.
 *
 * @retval kStatus_USB_Success          The transfer is successful.
 * @retval kStatus_USB_InvalidHandle    The device handle not be found.
 */
static usb_status_t USB_DevicePhdcBulkInCallback(usb_device_handle handle,
                                                 usb_device_endpoint_callback_message_struct_t *message,
                                                 void *callbackParam)
{
    usb_device_phdc_struct_t *phdcHandle;
    usb_status_t error = kStatus_USB_Error;

    phdcHandle = (usb_device_phdc_struct_t *)callbackParam;

    if (!phdcHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    phdcHandle->bulkIn.isBusy = 0U;
    if ((NULL != phdcHandle->configStruct) && (phdcHandle->configStruct->classCallback))
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = phdcHandle->configStruct->classCallback((class_handle_t)phdcHandle,
                                                        kUSB_DevicePhdcEventBulkInSendComplete, message);
    }

    return error;
}

/*!
 * @brief bulk OUT endpoint callback function.
 *
 * This callback function is used to notify upper layer the transfer result of a transfer.
 * This callback pointer is passed when the bulk OUT pipe initialized.
 *
 * @param handle          The device handle. It equals the value returned from USB_DeviceInit.
 * @param message         The result of the bulk OUT pipe transfer.
 * @param callbackParam   The parameter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam.
 *                        In the class, the value is the PHDC class handle.
 *
 * @retval kStatus_USB_Success          The transfer is successful.
 * @retval kStatus_USB_InvalidHandle    The device handle not be found.
 */
static usb_status_t USB_DevicePhdcBulkOutCallback(usb_device_handle handle,
                                                  usb_device_endpoint_callback_message_struct_t *message,
                                                  void *callbackParam)
{
    usb_device_phdc_struct_t *phdcHandle;
    usb_status_t error = kStatus_USB_Error;

    phdcHandle = (usb_device_phdc_struct_t *)callbackParam;

    if (!phdcHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    phdcHandle->bulkOut.isBusy = 0U;
    if ((NULL != phdcHandle->configStruct) && (phdcHandle->configStruct->classCallback))
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = phdcHandle->configStruct->classCallback((class_handle_t)phdcHandle, kUSB_DevicePhdcEventDataReceived,
                                                        message);
    }

    return error;
}

/*!
 * @brief Interrupt IN endpoint callback function.
 *
 * This callback function is used to notify upper layer the transfer result of a transfer.
 * This callback pointer is passed when the interrupt IN pipe initialized.
 *
 * @param handle          The device handle. It equals the value returned from USB_DeviceInit.
 * @param message         The result of the interrupt IN pipe transfer.
 * @param callbackParam  The parameter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam.
 *                        In the class, the value is the PHDC class handle.
 *
 * @retval kStatus_USB_Success          The transfer is successful.
 * @retval kStatus_USB_InvalidHandle    The device handle not be found.
 */
static usb_status_t USB_DevicePhdcInterruptInCallback(usb_device_handle handle,
                                                      usb_device_endpoint_callback_message_struct_t *message,
                                                      void *callbackParam)
{
    usb_device_phdc_struct_t *phdcHandle;
    usb_status_t error = kStatus_USB_Error;

    phdcHandle = (usb_device_phdc_struct_t *)callbackParam;

    if (!phdcHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    phdcHandle->interruptIn.isBusy = 0U;
    if ((NULL != phdcHandle->configStruct) && (phdcHandle->configStruct->classCallback))
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = phdcHandle->configStruct->classCallback((class_handle_t)phdcHandle,
                                                        kUSB_DevicePhdcEventInterruptInSendComplete, message);
    }
    return error;
}

/*!
 * @brief Initialize the endpoints of the PHDC class.
 *
 * This callback function is used to initialize the endpoints of the PHDC class.
 *
 * @param phdcHandle             The device PHDC class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @retval kStatus_USB_Success   The PHDC endpoint is initialized successful.
 * @retval kStatus_USB_Error     The interfaces not be found.
 */
static usb_status_t USB_DevicePhdcEndpointsInit(usb_device_phdc_struct_t *phdcHandle)
{
    usb_device_interface_list_t *interfaceList;
    usb_device_interface_struct_t *interface = (usb_device_interface_struct_t *)NULL;
    usb_status_t error                       = kStatus_USB_Error;
    if (!phdcHandle)
    {
        return error;
    }
    if (phdcHandle->configuration > phdcHandle->configStruct->classInfomation->configurations)
    {
        return error;
    }
    /* gets the interface list */
    interfaceList = &phdcHandle->configStruct->classInfomation->interfaceList[phdcHandle->configuration - 1];
    for (int count = 0; count < interfaceList->count; count++)
    {
        if (USB_DEVICE_CONFIG_PHDC_CLASS_CODE == interfaceList->interfaces[count].classCode)
        {
            for (int index = 0; index < interfaceList->interfaces[count].count; index++)
            {
                if (interfaceList->interfaces[count].interface[index].alternateSetting == phdcHandle->alternate)
                {
                    interface = &interfaceList->interfaces[count].interface[index];
                    break;
                }
            }
            phdcHandle->interfaceNumber = interfaceList->interfaces[count].interfaceNumber;
            break;
        }
    }
    if (!interface)
    {
        return error;
    }
    phdcHandle->interfaceHandle = interface;
    for (int count = 0; count < interface->endpointList.count; count++)
    {
        usb_device_endpoint_init_struct_t epInitStruct;
        usb_device_endpoint_callback_struct_t epCallback;
        epInitStruct.zlt             = 0U;
        epInitStruct.interval        = interface->endpointList.endpoint[count].interval;
        epInitStruct.endpointAddress = interface->endpointList.endpoint[count].endpointAddress;
        epInitStruct.maxPacketSize   = interface->endpointList.endpoint[count].maxPacketSize;
        epInitStruct.transferType    = interface->endpointList.endpoint[count].transferType;
        if (USB_ENDPOINT_INTERRUPT == epInitStruct.transferType)
        {
            epCallback.callbackFn      = USB_DevicePhdcInterruptInCallback;
            phdcHandle->interruptIn.ep = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            phdcHandle->interruptIn.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
            phdcHandle->interruptIn.pipeStall      = 0U;
            phdcHandle->interruptIn.pipeDataLen    = 0U;
        }
        else if (USB_IN == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                            USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
        {
            epCallback.callbackFn = USB_DevicePhdcBulkInCallback;
            phdcHandle->bulkIn.ep = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            phdcHandle->bulkIn.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
            phdcHandle->bulkIn.pipeStall      = 0U;
            phdcHandle->bulkIn.pipeDataLen    = 0U;
        }
        else
        {
            epCallback.callbackFn  = USB_DevicePhdcBulkOutCallback;
            phdcHandle->bulkOut.ep = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            phdcHandle->bulkOut.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
            phdcHandle->bulkOut.pipeStall      = 0U;
            phdcHandle->bulkOut.pipeDataLen    = 0U;
        }
        epCallback.callbackParam = phdcHandle;

        error = USB_DeviceInitEndpoint(phdcHandle->handle, &epInitStruct, &epCallback);
    }
    return error;
}

/*!
 * @brief De-initialize the endpoints of the PHDC class.
 *
 * This callback function is used to de-initialize the endpoints of the PHC class.
 *
 * @param phdcHandle             The device PHDC class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @retval kStatus_USB_Success   The PHDC endpoint is de-initialized successful.
 * @retval kStatus_USB_Error     The interface handle not be found.
 */
static usb_status_t USB_DevicePhdcEndpointsDeinit(usb_device_phdc_struct_t *phdcHandle)
{
    usb_status_t error = kStatus_USB_Error;

    if (!phdcHandle->interfaceHandle)
    {
        return error;
    }
    for (int count = 0; count < phdcHandle->interfaceHandle->endpointList.count; count++)
    {
        error = USB_DeviceDeinitEndpoint(phdcHandle->handle,
                                         phdcHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress);
    }
    return error;
}

/*!
 * @brief Handle the event passed to the PHDC class.
 *
 * This function handles the event passed to the PHDC class.
 *
 * @param[in] handle          The PHDC class handle, got from the usb_device_class_config_struct_t::classHandle.
 * @param[in] event           The event codes. Please refer to the enumeration usb_device_class_event_t.
 * @param[in,out] param       The param type is determined by the event code.
 *
 * @retval kStatus_USB_Success              Free device handle successfully.
 * @retval kStatus_USB_InvalidParameter     The device handle not be found.
 * @retval kStatus_USB_InvalidRequest       The request is invalid, and the control pipe will be stalled by the caller.
 */
usb_status_t USB_DevicePhdcEvent(void *handle, uint32_t event, void *param)
{
    usb_device_phdc_struct_t *phdcHandle;
    usb_status_t error = kStatus_USB_Error;
    uint16_t interfaceAlternate;
    uint8_t *temp8;
    uint8_t alternate;

    if ((!param) || (!handle))
    {
        return kStatus_USB_InvalidHandle;
    }

    phdcHandle = (usb_device_phdc_struct_t *)handle;

    switch (event)
    {
        case kUSB_DeviceClassEventDeviceReset:
            phdcHandle->configuration      = 0U;
            phdcHandle->bulkIn.isBusy      = 0U;
            phdcHandle->bulkOut.isBusy     = 0U;
            phdcHandle->interruptIn.isBusy = 0U;
            break;
        case kUSB_DeviceClassEventSetConfiguration:
            temp8 = ((uint8_t *)param);
            if (!phdcHandle->configStruct)
            {
                break;
            }
            if (*temp8 == phdcHandle->configuration)
            {
                break;
            }

            if (phdcHandle->configuration)
            {
                error = USB_DevicePhdcEndpointsDeinit(phdcHandle);
            }
            phdcHandle->configuration = *temp8;
            phdcHandle->alternate     = 0U;
            error                     = USB_DevicePhdcEndpointsInit(phdcHandle);
            break;
        case kUSB_DeviceClassEventSetInterface:
            if (!phdcHandle->configStruct)
            {
                break;
            }

            interfaceAlternate = *((uint16_t *)param);
            alternate          = (uint8_t)(interfaceAlternate & 0xFF);

            if (phdcHandle->interfaceNumber != ((uint8_t)(interfaceAlternate >> 8)))
            {
                break;
            }
            if (alternate == phdcHandle->alternate)
            {
                break;
            }
            error                 = USB_DevicePhdcEndpointsDeinit(phdcHandle);
            phdcHandle->alternate = alternate;
            error                 = USB_DevicePhdcEndpointsInit(phdcHandle);
            break;
        case kUSB_DeviceClassEventSetEndpointHalt:
            if ((!phdcHandle->configStruct) || (!phdcHandle->interfaceHandle))
            {
                break;
            }
            temp8 = ((uint8_t *)param);
            for (int count = 0; count < phdcHandle->interfaceHandle->endpointList.count; count++)
            {
                if (*temp8 == phdcHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress)
                {
                    /* Only stall the endpoint belongs to the class */
                    if (USB_ENDPOINT_INTERRUPT ==
                        phdcHandle->interfaceHandle->endpointList.endpoint[count].transferType)
                    {
                        phdcHandle->interruptIn.pipeStall = 1U;
                    }
                    else if (USB_IN == ((phdcHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress &
                                         USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
                    {
                        phdcHandle->bulkIn.pipeStall = 1U;
                    }
                    else
                    {
                        phdcHandle->bulkOut.pipeStall = 1U;
                    }
                    error = USB_DeviceStallEndpoint(phdcHandle->handle, *temp8);
                }
            }
            break;
        case kUSB_DeviceClassEventClearEndpointHalt:
            if ((!phdcHandle->configStruct) || (!phdcHandle->interfaceHandle))
            {
                break;
            }
            temp8 = ((uint8_t *)param);
            for (int count = 0; count < phdcHandle->interfaceHandle->endpointList.count; count++)
            {
                if (*temp8 == phdcHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress)
                {
                    error = USB_DeviceUnstallEndpoint(phdcHandle->handle, *temp8);
                    if (USB_ENDPOINT_INTERRUPT ==
                        phdcHandle->interfaceHandle->endpointList.endpoint[count].transferType)
                    {
                        if (phdcHandle->interruptIn.pipeStall)
                        {
                            phdcHandle->interruptIn.pipeStall = 0U;
                            if ((uint8_t *)USB_UNINITIALIZED_VAL_32 != phdcHandle->interruptIn.pipeDataBuffer)
                            {
                                error = USB_DeviceSendRequest(phdcHandle->handle, (phdcHandle->interruptIn.ep),
                                                              phdcHandle->interruptIn.pipeDataBuffer,
                                                              phdcHandle->interruptIn.pipeDataLen);
                                if (kStatus_USB_Success != error)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer  = phdcHandle->interruptIn.pipeDataBuffer;
                                    endpointCallbackMessage.length  = phdcHandle->interruptIn.pipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    USB_DevicePhdcInterruptInCallback(phdcHandle->handle,
                                                                      (void *)&endpointCallbackMessage, handle);
                                }
                                phdcHandle->interruptIn.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
                                phdcHandle->interruptIn.pipeDataLen    = 0U;
                            }
                        }
                    }
                    else if (USB_IN == ((phdcHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress &
                                         USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
                    {
                        if (phdcHandle->bulkIn.pipeStall)
                        {
                            phdcHandle->bulkIn.pipeStall = 0U;
                            if ((uint8_t *)USB_UNINITIALIZED_VAL_32 != phdcHandle->bulkIn.pipeDataBuffer)
                            {
                                error = USB_DeviceSendRequest(phdcHandle->handle, (phdcHandle->bulkIn.ep),
                                                              phdcHandle->bulkIn.pipeDataBuffer,
                                                              phdcHandle->bulkIn.pipeDataLen);
                                if (kStatus_USB_Success != error)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer  = phdcHandle->bulkIn.pipeDataBuffer;
                                    endpointCallbackMessage.length  = phdcHandle->bulkIn.pipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    USB_DevicePhdcBulkInCallback(phdcHandle->handle, (void *)&endpointCallbackMessage,
                                                                 handle);
                                }
                                phdcHandle->bulkIn.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
                                phdcHandle->bulkIn.pipeDataLen    = 0U;
                            }
                        }
                    }
                    else
                    {
                        if (phdcHandle->bulkOut.pipeStall)
                        {
                            phdcHandle->bulkOut.pipeStall = 0U;
                            if ((uint8_t *)USB_UNINITIALIZED_VAL_32 != phdcHandle->bulkOut.pipeDataBuffer)
                            {
                                error = USB_DeviceRecvRequest(phdcHandle->handle, (phdcHandle->bulkOut.ep),
                                                              phdcHandle->bulkOut.pipeDataBuffer,
                                                              phdcHandle->bulkOut.pipeDataLen);
                                if (kStatus_USB_Success != error)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer  = phdcHandle->bulkOut.pipeDataBuffer;
                                    endpointCallbackMessage.length  = phdcHandle->bulkOut.pipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    USB_DevicePhdcBulkOutCallback(phdcHandle->handle, (void *)&endpointCallbackMessage,
                                                                  handle);
                                }
                                phdcHandle->bulkOut.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
                                phdcHandle->bulkOut.pipeDataLen    = 0U;
                            }
                        }
                    }
                }
            }
            break;
        case kUSB_DeviceClassEventClassRequest:
            if (param)
            {
                usb_device_control_request_struct_t *controlRequest = (usb_device_control_request_struct_t *)param;

                if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) !=
                    USB_REQUEST_TYPE_RECIPIENT_INTERFACE)
                {
                    break;
                }

                if ((controlRequest->setup->wIndex & 0xFF) != phdcHandle->interfaceNumber)
                {
                    break;
                }

                switch (controlRequest->setup->bRequest)
                {
                    case USB_DEVICE_PHDC_REQUEST_SET_FEATURE:
                    {
                        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                        it is from the second parameter of classInit */
                        error = phdcHandle->configStruct->classCallback(
                            (class_handle_t)phdcHandle, kUSB_DevicePhdcEventSetFeature, &controlRequest->setup->wValue);
                    }
                    break;
                    case USB_DEVICE_PHDC_REQUEST_CLEAR_FEATURE:
                    {
                        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                        it is from the second parameter of classInit */
                        error = phdcHandle->configStruct->classCallback((class_handle_t)phdcHandle,
                                                                        kUSB_DevicePhdcEventClearFeature,
                                                                        &controlRequest->setup->wValue);
                    }
                    break;
                    case USB_DEVICE_PHDC_REQUEST_GET_STATUS:
                    {
                        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                        it is from the second parameter of classInit */
                        error = phdcHandle->configStruct->classCallback((class_handle_t)phdcHandle,
                                                                        kUSB_DevicePhdcEventGetStatus, controlRequest);
                    }
                    break;
                    default:
                        error = kStatus_USB_InvalidRequest;
                        break;
                }
            }
            break;
        default:
            error = kStatus_USB_InvalidRequest;
            break;
    }
    return error;
}

/*!
 * @brief Initialize the PHDC class.
 *
 * This function is used to initialize the PHDC class.
 *
 * @param controllerId   The controller id of the USB IP. Please refer to the enumeration usb_controller_index_t.
 * @param config          The class configuration information.
 * @param handle          It is output parameter, is used to return pointer of the PHDC class handle to the caller.
 *
 * @retval kStatus_USB_Success          The PHDC class is initialized successfully.
 * @retval kStatus_USB_Busy             No PHDC device handle available for allocation.
 * @retval kStatus_USB_InvalidHandle    The PHDC device handle allocation failure.
 * @retval kStatus_USB_InvalidParameter The USB device handle allocation failure.
 */
usb_status_t USB_DevicePhdcInit(uint8_t controllerId, usb_device_class_config_struct_t *config, class_handle_t *handle)
{
    usb_device_phdc_struct_t *phdcHandle;
    usb_status_t error = kStatus_USB_Error;

    error = USB_DevicePhdcAllocateHandle(&phdcHandle);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    error = USB_DeviceClassGetDeviceHandle(controllerId, &phdcHandle->handle);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    if (!phdcHandle->handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    phdcHandle->configStruct  = config;
    phdcHandle->configuration = 0U;
    phdcHandle->alternate     = 0xff;

    *handle = (class_handle_t)phdcHandle;
    return error;
}

/*!
 * @brief De-initialize the device PHDC class.
 *
 * The function de-initializes the device PHDC class.
 *
 * @param handle The PHDC class handle got from usb_device_class_config_struct_t::classHandle.
 *
 * @retval kStatus_USB_InvalidHandle        The device handle not be found.
 * @retva; kStatus_USB_Success              The PHDC class is de-initialized successful.
 */
usb_status_t USB_DevicePhdcDeinit(class_handle_t handle)
{
    usb_device_phdc_struct_t *phdcHandle;
    usb_status_t error = kStatus_USB_Error;

    phdcHandle = (usb_device_phdc_struct_t *)handle;

    if (!phdcHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    error = USB_DevicePhdcEndpointsDeinit(phdcHandle);
    USB_DevicePhdcFreeHandle(phdcHandle);
    return error;
}

/*!
 * @brief Send data through a specified endpoint.
 *
 * The function is used to send data through a specified endpoint.
 * The function calls #USB_DeviceSendRequest internally.
 *
 * @param[in] handle The PHDC class handle got from usb_device_class_config_struct_t::classHandle.
 * @param[in] ep     Endpoint index.
 * @param[in] buffer The memory address to hold the data need to be sent.
 * @param[in] length The data length need to be sent.
 *
 * @retval kStatus_USB_InvalidHandle        The device handle not be found.
 * @retval kStatus_USB_Busy                 The previous transfer is pending.
 * @retva; kStatus_USB_Success              The sending is successful.
 */
usb_status_t USB_DevicePhdcSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_phdc_struct_t *phdcHandle;
    usb_device_phdc_pipe_t *pipe;
    usb_status_t error;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    phdcHandle = (usb_device_phdc_struct_t *)handle;
    if (phdcHandle->bulkIn.ep == ep)
    {
        pipe = &(phdcHandle->bulkIn);
    }
    else if (phdcHandle->interruptIn.ep == ep)
    {
        pipe = &(phdcHandle->interruptIn);
    }
    else
    {
        return kStatus_USB_InvalidParameter;
    }

    if (1U == pipe->isBusy)
    {
        return kStatus_USB_Busy;
    }
    pipe->isBusy = 1U;

    if (pipe->pipeStall)
    {
        pipe->pipeDataBuffer = buffer;
        pipe->pipeDataLen    = length;
        return kStatus_USB_Success;
    }
    error = USB_DeviceSendRequest(phdcHandle->handle, ep, buffer, length);
    if (kStatus_USB_Success != error)
    {
        pipe->isBusy = 0U;
    }
    return error;
}

/*!
 * @brief Receive data through a specified endpoint.
 *
 * The function is used to receive data through a specified endpoint.
 * The function calls #USB_DeviceRecvRequest internally.
 *
 * @param[in] handle The PHDC class handle got from usb_device_class_config_struct_t::classHandle.
 * @param[in] ep     Endpoint index.
 * @param[in] buffer The memory address to save the received data.
 * @param[in] length The data length want to be received.
 *
 * @retval kStatus_USB_InvalidHandle        The device handle not be found.
 * @retval kStatus_USB_Busy                 The previous transfer is pending.
 * @retva; kStatus_USB_Success              The receiving is successful.
 */
usb_status_t USB_DevicePhdcRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_phdc_struct_t *phdcHandle;
    usb_status_t error;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    phdcHandle = (usb_device_phdc_struct_t *)handle;
    if (1U == phdcHandle->bulkOut.isBusy)
    {
        return kStatus_USB_Busy;
    }
    phdcHandle->bulkOut.isBusy = 1U;

    if (phdcHandle->bulkOut.pipeStall)
    {
        phdcHandle->bulkOut.pipeDataBuffer = buffer;
        phdcHandle->bulkOut.pipeDataLen    = length;
        return kStatus_USB_Success;
    }
    error = USB_DeviceRecvRequest(phdcHandle->handle, ep, buffer, length);
    if (kStatus_USB_Success != error)
    {
        phdcHandle->bulkOut.isBusy = 0U;
    }
    return error;
}

#endif
