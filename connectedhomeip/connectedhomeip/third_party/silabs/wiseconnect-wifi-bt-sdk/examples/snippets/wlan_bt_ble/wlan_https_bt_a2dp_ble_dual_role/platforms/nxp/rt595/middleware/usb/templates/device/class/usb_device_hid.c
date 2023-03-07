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

#if ((defined(USB_DEVICE_CONFIG_HID)) && (USB_DEVICE_CONFIG_HID > 0U))
#include "usb_device_hid.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static usb_status_t USB_DeviceHidAllocateHandle(usb_device_hid_struct_t **handle);
static usb_status_t USB_DeviceHidFreeHandle(usb_device_hid_struct_t *handle);
static usb_status_t USB_DeviceHidInterruptIn(usb_device_handle handle,
                                             usb_device_endpoint_callback_message_struct_t *message,
                                             void *callbackParam);
static usb_status_t USB_DeviceHidInterruptOut(usb_device_handle handle,
                                              usb_device_endpoint_callback_message_struct_t *message,
                                              void *callbackParam);
static usb_status_t USB_DeviceHidEndpointsInit(usb_device_hid_struct_t *hidHandle);
static usb_status_t USB_DeviceHidEndpointsDeinit(usb_device_hid_struct_t *hidHandle);

/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static usb_device_hid_struct_t
    s_UsbDeviceHidHandle[USB_DEVICE_CONFIG_HID];

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Allocate a device hid class handle.
 *
 * This function allocates a device hid class handle.
 *
 * @param handle          It is out parameter, is used to return pointer of the device hid class handle to the caller.
 *
 * @retval kStatus_USB_Success              Get a device hid class handle successfully.
 * @retval kStatus_USB_Busy                 Cannot allocate a device hid class handle.
 */
static usb_status_t USB_DeviceHidAllocateHandle(usb_device_hid_struct_t **handle)
{
    uint32_t count;
    for (count = 0U; count < USB_DEVICE_CONFIG_HID; count++)
    {
        if (NULL == s_UsbDeviceHidHandle[count].handle)
        {
            *handle = &s_UsbDeviceHidHandle[count];
            return kStatus_USB_Success;
        }
    }

    return kStatus_USB_Busy;
}

/*!
 * @brief Free a device hid class handle.
 *
 * This function frees a device hid class handle.
 *
 * @param handle          The device hid class handle.
 *
 * @retval kStatus_USB_Success              Free device hid class handle successfully.
 */
static usb_status_t USB_DeviceHidFreeHandle(usb_device_hid_struct_t *handle)
{
    handle->handle        = NULL;
    handle->configStruct  = (usb_device_class_config_struct_t *)NULL;
    handle->configuration = 0U;
    handle->alternate     = 0U;
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
 * usb_device_endpoint_callback_struct_t::callbackParam. In the class, the value is the HID class handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceHidInterruptIn(usb_device_handle handle,
                                             usb_device_endpoint_callback_message_struct_t *message,
                                             void *callbackParam)
{
    usb_device_hid_struct_t *hidHandle;
    usb_status_t error = kStatus_USB_Error;

    /* Get the HID class handle */
    hidHandle = (usb_device_hid_struct_t *)callbackParam;

    if (!hidHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    hidHandle->interruptInPipeBusy = 0U;
    if ((NULL != hidHandle->configStruct) && (hidHandle->configStruct->classCallback))
    {
        /* Notify the application data sent by calling the hid class callback. classCallback is initialized
           in classInit of s_UsbDeviceClassInterfaceMap,it is from the second parameter of classInit */
        error =
            hidHandle->configStruct->classCallback((class_handle_t)hidHandle, kUSB_DeviceHidEventSendResponse, message);
    }

    return error;
}

/*!
 * @brief Interrupt OUT endpoint callback function.
 *
 * This callback function is used to notify uplayer the transfser result of a transfer.
 * This callback pointer is passed when the interrupt OUT pipe initialized.
 *
 * @param handle          The device handle. It equals the value returned from USB_DeviceInit.
 * @param message         The result of the interrupt OUT pipe transfer.
 * @param callbackParam  The parameter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam. In the class, the value is the HID class handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceHidInterruptOut(usb_device_handle handle,
                                              usb_device_endpoint_callback_message_struct_t *message,
                                              void *callbackParam)
{
    usb_device_hid_struct_t *hidHandle;
    usb_status_t error = kStatus_USB_Error;

    /* Get the HID class handle */
    hidHandle = (usb_device_hid_struct_t *)callbackParam;

    if (!hidHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    hidHandle->interruptOutPipeBusy = 0U;
    if ((NULL != hidHandle->configStruct) && (hidHandle->configStruct->classCallback))
    {
        /* Notify the application data sent by calling the hid class callback. classCallback is initialized
           in classInit of s_UsbDeviceClassInterfaceMap,it is from the second parameter of classInit */
        error =
            hidHandle->configStruct->classCallback((class_handle_t)hidHandle, kUSB_DeviceHidEventRecvResponse, message);
    }

    return error;
}

/*!
 * @brief Initialize the endpoints of the hid class.
 *
 * This callback function is used to initialize the endpoints of the hid class.
 *
 * @param hidHandle          The device hid class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceHidEndpointsInit(usb_device_hid_struct_t *hidHandle)
{
    usb_device_interface_list_t *interfaceList;
    usb_device_interface_struct_t *interface = (usb_device_interface_struct_t *)NULL;
    usb_status_t error                       = kStatus_USB_Error;

    /* Check the configuration is valid or not. */
    if (!hidHandle->configuration)
    {
        return error;
    }

    if (hidHandle->configuration > hidHandle->configStruct->classInfomation->configurations)
    {
        return error;
    }

    /* Get the interface list of the new configuration. */
    if (NULL == hidHandle->configStruct->classInfomation->interfaceList)
    {
        return error;
    }
    interfaceList = &hidHandle->configStruct->classInfomation->interfaceList[hidHandle->configuration - 1U];

    /* Find interface by using the alternate setting of the interface. */
    for (int count = 0U; count < interfaceList->count; count++)
    {
        if (USB_DEVICE_CONFIG_HID_CLASS_CODE == interfaceList->interfaces[count].classCode)
        {
            for (int index = 0U; index < interfaceList->interfaces[count].count; index++)
            {
                if (interfaceList->interfaces[count].interface[index].alternateSetting == hidHandle->alternate)
                {
                    interface = &interfaceList->interfaces[count].interface[index];
                    break;
                }
            }
            hidHandle->interfaceNumber = interfaceList->interfaces[count].interfaceNumber;
            break;
        }
    }
    if (!interface)
    {
        /* Return error if the interface is not found. */
        return error;
    }

    /* Keep new interface handle. */
    hidHandle->interfaceHandle = interface;

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

        if (USB_IN == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                       USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
        {
            epCallback.callbackFn                = USB_DeviceHidInterruptIn;
            hidHandle->interruptInPipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
            hidHandle->interruptInPipeStall      = 0U;
            hidHandle->interruptInPipeDataLen    = 0U;
        }
        else
        {
            epCallback.callbackFn                 = USB_DeviceHidInterruptOut;
            hidHandle->interruptOutPipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
            hidHandle->interruptOutPipeStall      = 0U;
            hidHandle->interruptOutPipeDataLen    = 0U;
        }
        epCallback.callbackParam = hidHandle;

        error = USB_DeviceInitEndpoint(hidHandle->handle, &epInitStruct, &epCallback);
    }
    return error;
}

/*!
 * @brief De-initialize the endpoints of the hid class.
 *
 * This callback function is used to de-initialize the endpoints of the hid class.
 *
 * @param hidHandle          The device hid class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceHidEndpointsDeinit(usb_device_hid_struct_t *hidHandle)
{
    usb_status_t error = kStatus_USB_Error;

    if (!hidHandle->interfaceHandle)
    {
        return error;
    }
    /* De-initialize all endpoints of the interface */
    for (int count = 0U; count < hidHandle->interfaceHandle->endpointList.count; count++)
    {
        error = USB_DeviceDeinitEndpoint(hidHandle->handle,
                                         hidHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress);
    }
    hidHandle->interfaceHandle = NULL;
    return error;
}

/*!
 * @brief Handle the event passed to the hid class.
 *
 * This function handles the event passed to the hid class.
 *
 * @param handle          The hid class handle, got from the usb_device_class_config_struct_t::classHandle.
 * @param event           The event codes. Please refer to the enumeration usb_device_class_event_t.
 * @param param           The param type is determined by the event code.
 *
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success              Free device handle successfully.
 * @retval kStatus_USB_InvalidParameter     The device handle not be found.
 * @retval kStatus_USB_InvalidRequest       The request is invalid, and the control pipe will be stalled by the caller.
 */
usb_status_t USB_DeviceHidEvent(void *handle, uint32_t event, void *param)
{
    usb_device_hid_struct_t *hidHandle;
    usb_device_hid_report_struct_t report;
    usb_status_t error = kStatus_USB_Error;
    uint16_t interfaceAlternate;
    uint8_t *temp8;
    uint8_t alternate;

    if ((!param) || (!handle))
    {
        return kStatus_USB_InvalidHandle;
    }

    /* Get the hid class handle. */
    hidHandle = (usb_device_hid_struct_t *)handle;

    switch (event)
    {
        case kUSB_DeviceClassEventDeviceReset:
            /* Bus reset, clear the configuration. */
            hidHandle->configuration        = 0U;
            hidHandle->interruptInPipeBusy  = 0U;
            hidHandle->interruptOutPipeBusy = 0U;
            hidHandle->interfaceHandle      = NULL;
            break;
        case kUSB_DeviceClassEventSetConfiguration:
            /* Get the new configuration. */
            temp8 = ((uint8_t *)param);
            if (!hidHandle->configStruct)
            {
                break;
            }
            if (*temp8 == hidHandle->configuration)
            {
                break;
            }

            /* De-initialize the endpoints when current configuration is none zero. */
            if (hidHandle->configuration)
            {
                error = USB_DeviceHidEndpointsDeinit(hidHandle);
            }
            /* Save new configuration. */
            hidHandle->configuration = *temp8;
            /* Clear the alternate setting value. */
            hidHandle->alternate = 0U;

            /* Initialize the endpoints of the new current configuration by using the alternate setting 0. */
            error = USB_DeviceHidEndpointsInit(hidHandle);
            break;
        case kUSB_DeviceClassEventSetInterface:
            if (!hidHandle->configStruct)
            {
                break;
            }
            /* Get the new alternate setting of the interface */
            interfaceAlternate = *((uint16_t *)param);
            /* Get the alternate setting value */
            alternate = (uint8_t)(interfaceAlternate & 0xFFU);

            /* Whether the interface belongs to the class. */
            if (hidHandle->interfaceNumber != ((uint8_t)(interfaceAlternate >> 8U)))
            {
                break;
            }
            /* Only handle new alternate setting. */
            if (alternate == hidHandle->alternate)
            {
                break;
            }
            /* De-initialize old endpoints */
            error                = USB_DeviceHidEndpointsDeinit(hidHandle);
            hidHandle->alternate = alternate;
            /* Initialize new endpoints */
            error = USB_DeviceHidEndpointsInit(hidHandle);
            break;
        case kUSB_DeviceClassEventSetEndpointHalt:
            if ((!hidHandle->configStruct) || (!hidHandle->interfaceHandle))
            {
                break;
            }
            /* Get the endpoint address */
            temp8 = ((uint8_t *)param);
            for (int count = 0U; count < hidHandle->interfaceHandle->endpointList.count; count++)
            {
                if (*temp8 == hidHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress)
                {
                    /* Only stall the endpoint belongs to the class */
                    if (USB_IN == ((hidHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress &
                                    USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                                   USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
                    {
                        hidHandle->interruptInPipeStall = 1U;
                    }
                    else
                    {
                        hidHandle->interruptOutPipeStall = 1U;
                    }
                    error = USB_DeviceStallEndpoint(hidHandle->handle, *temp8);
                }
            }
            break;
        case kUSB_DeviceClassEventClearEndpointHalt:
            if ((!hidHandle->configStruct) || (!hidHandle->interfaceHandle))
            {
                break;
            }
            /* Get the endpoint address */
            temp8 = ((uint8_t *)param);
            for (int count = 0U; count < hidHandle->interfaceHandle->endpointList.count; count++)
            {
                if (*temp8 == hidHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress)
                {
                    /* Only un-stall the endpoint belongs to the class */
                    error = USB_DeviceUnstallEndpoint(hidHandle->handle, *temp8);
                    if (USB_IN == (((*temp8) & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                                   USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
                    {
                        if (hidHandle->interruptInPipeStall)
                        {
                            hidHandle->interruptInPipeStall = 0U;
                            if ((uint8_t *)USB_UNINITIALIZED_VAL_32 != hidHandle->interruptInPipeDataBuffer)
                            {
                                error = USB_DeviceSendRequest(
                                    hidHandle->handle,
                                    (hidHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress &
                                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK),
                                    hidHandle->interruptInPipeDataBuffer, hidHandle->interruptInPipeDataLen);
                                if (kStatus_USB_Success != error)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer  = hidHandle->interruptInPipeDataBuffer;
                                    endpointCallbackMessage.length  = hidHandle->interruptInPipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    USB_DeviceHidInterruptIn(hidHandle->handle, (void *)&endpointCallbackMessage,
                                                             handle);
                                }
                                hidHandle->interruptInPipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
                                hidHandle->interruptInPipeDataLen    = 0U;
                            }
                        }
                    }
                    else
                    {
                        if (hidHandle->interruptOutPipeStall)
                        {
                            hidHandle->interruptOutPipeStall = 0U;
                            if ((uint8_t *)USB_UNINITIALIZED_VAL_32 != hidHandle->interruptOutPipeDataBuffer)
                            {
                                error = USB_DeviceRecvRequest(
                                    hidHandle->handle,
                                    (hidHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress &
                                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK),
                                    hidHandle->interruptOutPipeDataBuffer, hidHandle->interruptOutPipeDataLen);
                                if (kStatus_USB_Success != error)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer  = hidHandle->interruptOutPipeDataBuffer;
                                    endpointCallbackMessage.length  = hidHandle->interruptOutPipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    USB_DeviceHidInterruptOut(hidHandle->handle, (void *)&endpointCallbackMessage,
                                                              handle);
                                }
                                hidHandle->interruptOutPipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
                                ;
                                hidHandle->interruptOutPipeDataLen = 0U;
                            }
                        }
                    }
                }
            }
            break;
        case kUSB_DeviceClassEventClassRequest:
            if (param)
            {
                /* Handle the hid class specific request. */
                usb_device_control_request_struct_t *controlRequest = (usb_device_control_request_struct_t *)param;

                if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) !=
                    USB_REQUEST_TYPE_RECIPIENT_INTERFACE)
                {
                    break;
                }

                if ((controlRequest->setup->wIndex & 0xFFU) != hidHandle->interfaceNumber)
                {
                    break;
                }

                switch (controlRequest->setup->bRequest)
                {
                    case USB_DEVICE_HID_REQUEST_GET_REPORT:
                        /* Get report request */
                        report.reportType = (controlRequest->setup->wValue & 0xFF00U) >> 0x08U;
                        report.reportId   = (controlRequest->setup->wValue & 0x00FFU);
                        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                           it is from the second parameter of classInit */
                        error                  = hidHandle->configStruct->classCallback((class_handle_t)hidHandle,
                                                                       kUSB_DeviceHidEventGetReport, &report);
                        controlRequest->buffer = report.reportBuffer;
                        controlRequest->length = report.reportLength;
                        break;
                    case USB_DEVICE_HID_REQUEST_GET_IDLE:
                        /* Get idle request, classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                           it is from the second parameter of classInit */
                        error = hidHandle->configStruct->classCallback(
                            (class_handle_t)hidHandle, kUSB_DeviceHidEventGetIdle, &hidHandle->idleRate);
                        controlRequest->buffer = &hidHandle->idleRate;
                        break;
                    case USB_DEVICE_HID_REQUEST_GET_PROTOCOL:
                        /* Get protocol request, classCallback is initialized in classInit of
                           s_UsbDeviceClassInterfaceMap,
                           it is from the second parameter of classInit */
                        error = hidHandle->configStruct->classCallback(
                            (class_handle_t)hidHandle, kUSB_DeviceHidEventGetIdle, &hidHandle->protocol);
                        controlRequest->buffer = &hidHandle->protocol;
                        break;
                    case USB_DEVICE_HID_REQUEST_SET_REPORT:
                        /* Set report request */
                        report.reportType = (controlRequest->setup->wValue & 0xFF00U) >> 0x08U;
                        report.reportId   = (controlRequest->setup->wValue & 0x00FFU);
                        if (controlRequest->isSetup)
                        {
                            report.reportLength = controlRequest->length;
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                               it is from the second parameter of classInit */
                            error = hidHandle->configStruct->classCallback(
                                (class_handle_t)hidHandle, kUSB_DeviceHidEventRequestReportBuffer, &report);
                            controlRequest->buffer = report.reportBuffer;
                            controlRequest->length = report.reportLength;
                        }
                        else
                        {
                            report.reportBuffer = controlRequest->buffer;
                            report.reportLength = controlRequest->length;
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                               it is from the second parameter of classInit */
                            error = hidHandle->configStruct->classCallback((class_handle_t)hidHandle,
                                                                           kUSB_DeviceHidEventSetReport, &report);
                        }
                        break;
                    case USB_DEVICE_HID_REQUEST_SET_IDLE:
                        /* Set idle request */
                        {
                            hidHandle->idleRate = (controlRequest->setup->wValue & 0xFF00U) >> 0x08U;
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                               it is from the second parameter of classInit */
                            error = hidHandle->configStruct->classCallback(
                                (class_handle_t)hidHandle, kUSB_DeviceHidEventSetIdle, &hidHandle->idleRate);
                        }
                        break;
                    case USB_DEVICE_HID_REQUEST_SET_PROTOCOL:
                        /* Set protocol request */
                        {
                            hidHandle->protocol = (controlRequest->setup->wValue & 0x00FFU);
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                               it is from the second parameter of classInit */
                            error = hidHandle->configStruct->classCallback(
                                (class_handle_t)hidHandle, kUSB_DeviceHidEventSetProtocol, &hidHandle->protocol);
                        }
                        break;
                    default:
                        error = kStatus_USB_InvalidRequest;
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
 * @brief Initialize the hid class.
 *
 * This function is used to initialize the hid class.
 *
 * @param controllerId   The controller id of the USB IP. Please refer to the enumeration usb_controller_index_t.
 * @param config          The class configuration information.
 * @param handle          It is out parameter, is used to return pointer of the hid class handle to the caller.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceHidInit(uint8_t controllerId, usb_device_class_config_struct_t *config, class_handle_t *handle)
{
    usb_device_hid_struct_t *hidHandle;
    usb_status_t error = kStatus_USB_Error;

    /* Allocate a hid class handle. */
    error = USB_DeviceHidAllocateHandle(&hidHandle);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    /* Get the device handle according to the controller id. */
    error = USB_DeviceClassGetDeviceHandle(controllerId, &hidHandle->handle);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    if (!hidHandle->handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    /* Save the configuration of the class. */
    hidHandle->configStruct = config;
    /* Clear the configuration value. */
    hidHandle->configuration = 0U;
    hidHandle->alternate     = 0xffU;

    *handle = (class_handle_t)hidHandle;
    return error;
}

/*!
 * @brief De-initialize the device hid class.
 *
 * The function de-initializes the device hid class.
 *
 * @param handle The hid class handle got from usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceHidDeinit(class_handle_t handle)
{
    usb_device_hid_struct_t *hidHandle;
    usb_status_t error = kStatus_USB_Error;

    hidHandle = (usb_device_hid_struct_t *)handle;

    if (!hidHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    /* De-initialzie the endpoints. */
    error = USB_DeviceHidEndpointsDeinit(hidHandle);
    /* Free the hid class handle. */
    USB_DeviceHidFreeHandle(hidHandle);
    return error;
}

/*!
 * @brief Send data through a specified endpoint.
 *
 * The function is used to send data through a specified endpoint.
 * The function calls USB_DeviceSendRequest internally.
 *
 * @param handle The hid class handle got from usb_device_class_config_struct_t::classHandle.
 * @param ep     Endpoint index.
 * @param buffer The memory address to hold the data need to be sent.
 * @param length The data length need to be sent.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The return value just means if the sending request is successful or not; the transfer done is notified by
 * USB_DeviceHidInterruptIn.
 * Currently, only one transfer request can be supported for one specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for one specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer could begin only when the previous transfer is done (get notification through the endpoint
 * callback).
 */
usb_status_t USB_DeviceHidSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_hid_struct_t *hidHandle;
    usb_status_t error = kStatus_USB_Error;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    hidHandle = (usb_device_hid_struct_t *)handle;

    if (hidHandle->interruptInPipeBusy)
    {
        return kStatus_USB_Busy;
    }
    hidHandle->interruptInPipeBusy = 1U;

    if (hidHandle->interruptInPipeStall)
    {
        hidHandle->interruptInPipeDataBuffer = buffer;
        hidHandle->interruptInPipeDataLen    = length;
        return kStatus_USB_Success;
    }
    error = USB_DeviceSendRequest(hidHandle->handle, ep, buffer, length);
    if (kStatus_USB_Success != error)
    {
        hidHandle->interruptInPipeBusy = 0U;
    }
    return error;
}

/*!
 * @brief Receive data through a specified endpoint.
 *
 * The function is used to receive data through a specified endpoint.
 * The function calls USB_DeviceRecvRequest internally.
 *
 * @param handle The hid class handle got from usb_device_class_config_struct_t::classHandle.
 * @param ep     Endpoint index.
 * @param buffer The memory address to save the received data.
 * @param length The data length want to be received.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The return value just means if the receiving request is successful or not; the transfer done is notified by
 * USB_DeviceHidInterruptOut.
 * Currently, only one transfer request can be supported for one specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for one specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer could begin only when the previous transfer is done (get notification through the endpoint
 * callback).
 */
usb_status_t USB_DeviceHidRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_hid_struct_t *hidHandle;
    usb_status_t error = kStatus_USB_Error;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    hidHandle = (usb_device_hid_struct_t *)handle;

    if (hidHandle->interruptOutPipeBusy)
    {
        return kStatus_USB_Busy;
    }
    hidHandle->interruptOutPipeBusy = 1U;

    if (hidHandle->interruptOutPipeStall)
    {
        hidHandle->interruptOutPipeDataBuffer = buffer;
        hidHandle->interruptOutPipeDataLen    = length;
        return kStatus_USB_Success;
    }
    error = USB_DeviceRecvRequest(hidHandle->handle, ep, buffer, length);
    if (kStatus_USB_Success != error)
    {
        hidHandle->interruptOutPipeBusy = 0U;
    }
    return error;
}

#endif
