/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"

#if ((defined(USB_DEVICE_CONFIG_PRINTER)) && (USB_DEVICE_CONFIG_PRINTER > 0U))
#include "usb_device_printer.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static usb_device_printer_struct_t
    s_PrinterHandle[USB_DEVICE_CONFIG_PRINTER];

/*******************************************************************************
 * Code
 ******************************************************************************/

static usb_status_t USB_DevicePrinterAllocateHandle(usb_device_printer_struct_t **printerHandle)
{
    uint8_t index;

    for (index = 0; index < USB_DEVICE_CONFIG_PRINTER; ++index)
    {
        if (s_PrinterHandle[index].deviceHandle == NULL)
        {
            *printerHandle = &(s_PrinterHandle[index]);
            return kStatus_USB_Success;
        }
    }

    return kStatus_USB_Busy;
}

static usb_status_t USB_DevicePrinterFreeHandle(usb_device_printer_struct_t *printerHandle)
{
    /* ensure that printerHandle is not NULL before calling this function */
    printerHandle->deviceHandle = NULL;
    printerHandle->classConfig = NULL;
    printerHandle->alternate = 0xFFu;
    printerHandle->configuration = 0;
    printerHandle->interfaceNumber = 0;

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
 *                        In the class, the value is the printer class handle.
 *
 * @retval kStatus_USB_Success          The transfer is successful.
 * @retval kStatus_USB_InvalidHandle    The device handle not be found.
 */
static usb_status_t USB_DevicePrinterBulkInCallback(usb_device_handle handle,
                                                    usb_device_endpoint_callback_message_struct_t *message,
                                                    void *callbackParam)
{
    usb_device_printer_struct_t *printerHandle = (usb_device_printer_struct_t *)callbackParam;
    usb_status_t status = kStatus_USB_Error;

    if (callbackParam == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    printerHandle->bulkInBusy = 0U;
    if ((NULL != printerHandle->classConfig) && (printerHandle->classConfig->classCallback))
    {
        /* Notify the application data received by calling the printer class callback.
        ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        status = printerHandle->classConfig->classCallback((class_handle_t)printerHandle,
                                                           kUSB_DevicePrinterEventSendResponse, message);
    }

    return status;
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
 *                        In the class, the value is the printer class handle.
 *
 * @retval kStatus_USB_Success          The transfer is successful.
 * @retval kStatus_USB_InvalidHandle    The device handle not be found.
 */
static usb_status_t USB_DevicePrinterBulkOutCallback(usb_device_handle handle,
                                                     usb_device_endpoint_callback_message_struct_t *message,
                                                     void *callbackParam)
{
    usb_device_printer_struct_t *printerHandle = (usb_device_printer_struct_t *)callbackParam;
    usb_status_t status = kStatus_USB_Error;

    if (printerHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }
    printerHandle->bulkOutBusy = 0U;
    if ((NULL != printerHandle->classConfig) && (printerHandle->classConfig->classCallback))
    {
        /* ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        status = printerHandle->classConfig->classCallback((class_handle_t)printerHandle,
                                                           kUSB_DevicePrinterEventRecvResponse, message);
    }

    return status;
}

/*!
 * @brief Initialize the endpoints of the printer class.
 *
 * This callback function is used to initialize the endpoints of the printer class.
 *
 * @param printerHandle          The device printer class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DevicePrinterEndpointsInit(usb_device_printer_struct_t *printerHandle)
{
    /* ensure that printerHandle is not NULL before calling this function */
    usb_device_interface_list_t *configInterfaceList;
    usb_device_interface_struct_t *interface = NULL;
    usb_status_t status = kStatus_USB_Error;
    uint8_t interfaceIndex;
    uint8_t index;

    /* return error when configuration is invalid (0 or more than the configuration number) */
    if ((printerHandle->configuration == 0U) ||
        (printerHandle->configuration > printerHandle->classConfig->classInfomation->configurations))
    {
        return status;
    }

    configInterfaceList =
        &(printerHandle->classConfig->classInfomation->interfaceList[printerHandle->configuration - 1]);
    for (interfaceIndex = 0; interfaceIndex < configInterfaceList->count; ++interfaceIndex)
    {
        if (USB_DEVICE_CONFIG_PRINTER_CLASS_CODE == configInterfaceList->interfaces[interfaceIndex].classCode)
        {
            /* index means the alternate interface's index here */
            for (index = 0; index < configInterfaceList->interfaces[interfaceIndex].count; ++index)
            {
                if (configInterfaceList->interfaces[interfaceIndex].interface[index].alternateSetting ==
                    printerHandle->alternate)
                {
                    interface = &(configInterfaceList->interfaces[interfaceIndex].interface[index]);
                    break;
                }
            }
            printerHandle->interfaceNumber = configInterfaceList->interfaces[interfaceIndex].interfaceNumber;
            break;
        }
    }
    if (interface == NULL)
    {
        return status;
    }

    /* Keep new interface handle. */
    printerHandle->interfaceHandle = interface;

    /* Initialize the endpoints of the new interface. */
    /* index means the endpoint's index here */
    for (index = 0; index < interface->endpointList.count; ++index)
    {
        usb_device_endpoint_init_struct_t epInitStruct;
        usb_device_endpoint_callback_struct_t epCallback;
        epInitStruct.zlt = 0U;
        epInitStruct.interval = interface->endpointList.endpoint[index].interval;
        epInitStruct.endpointAddress = interface->endpointList.endpoint[index].endpointAddress;
        epInitStruct.maxPacketSize = interface->endpointList.endpoint[index].maxPacketSize;
        epInitStruct.transferType = interface->endpointList.endpoint[index].transferType;

        if (((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT) == USB_IN)
        {
            epCallback.callbackFn = USB_DevicePrinterBulkInCallback;
            printerHandle->bulkInPipeDataBuffer = (uint8_t*)USB_UNINITIALIZED_VAL_32;
            printerHandle->bulkInPipeStall = 0U;
            printerHandle->bulkInPipeDataLen = 0U;
        }
        else
        {
            epCallback.callbackFn = USB_DevicePrinterBulkOutCallback;
            printerHandle->bulkOutPipeDataBuffer = (uint8_t*)USB_UNINITIALIZED_VAL_32;
            printerHandle->bulkOutPipeStall = 0U;
            printerHandle->bulkOutPipeDataLen = 0U;
        }
        epCallback.callbackParam = printerHandle;

        status = USB_DeviceInitEndpoint(printerHandle->deviceHandle, &epInitStruct, &epCallback);
    }

    return status;
}

/*!
 * @brief De-initialize the endpoints of the printer class.
 *
 * This callback function is used to de-initialize the endpoints of the printer class.
 *
 * @param printerHandle          The device printer class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DevicePrinterEndpointsDeinit(usb_device_printer_struct_t *printerHandle)
{
    /* ensure that printerHandle is not NULL before calling this function */
    usb_status_t status = kStatus_USB_Error;
    uint8_t index;

    /* return directly when the interfaceHandle is NULL, it means USB_DevicePrinterEndpointsInit is not called */
    if (printerHandle->interfaceHandle == NULL)
    {
        return kStatus_USB_Success;
    }

    for (index = 0; index < printerHandle->interfaceHandle->endpointList.count; ++index)
    {
        status = USB_DeviceDeinitEndpoint(printerHandle->deviceHandle,
                                          printerHandle->interfaceHandle->endpointList.endpoint[index].endpointAddress);
    }
    printerHandle->interfaceHandle = NULL;

    return status;
}

usb_status_t USB_DevicePrinterInit(uint8_t controllerId,
                                   usb_device_class_config_struct_t *config,
                                   class_handle_t *handle)

{
    usb_device_handle deviceHandle;
    usb_device_printer_struct_t *printerHandle;
    usb_status_t status = kStatus_USB_Error;
    USB_OSA_SR_ALLOC();

    /* get the controller's device handle */
    status = USB_DeviceClassGetDeviceHandle(controllerId, &deviceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }
    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    /* Allocate a printer class handle. */
    USB_OSA_ENTER_CRITICAL();
    status = USB_DevicePrinterAllocateHandle(&printerHandle);
    printerHandle->deviceHandle = deviceHandle; /* this printer instance is used */
    USB_OSA_EXIT_CRITICAL();
    if (status != kStatus_USB_Success)
    {
        return status;
    }

    printerHandle->classConfig = config;
    printerHandle->alternate = 0xFFu;
    printerHandle->configuration = 0;
    printerHandle->interfaceNumber = 0;
    printerHandle->interfaceHandle = NULL;

    *handle = (class_handle_t)printerHandle;

    return status;
}

usb_status_t USB_DevicePrinterDeinit(class_handle_t handle)
{
    usb_status_t status;
    usb_device_printer_struct_t *printerHandle = (usb_device_printer_struct_t *)handle;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }

    status = USB_DevicePrinterEndpointsDeinit(printerHandle);
    USB_DevicePrinterFreeHandle(printerHandle);

    return status;
}

usb_status_t USB_DevicePrinterEvent(void *handle, uint32_t event, void *param)
{
    usb_status_t status = kStatus_USB_Error;
    usb_device_printer_struct_t *printerHandle = (usb_device_printer_struct_t *)handle;
    uint16_t temp16;
    uint8_t temp8;

    if ((handle == NULL) || (param == NULL))
    {
        return kStatus_USB_InvalidHandle;
    }

    switch (event)
    {
        case kUSB_DeviceClassEventDeviceReset:
            /* de-initialize printer instance */
            printerHandle->interfaceHandle = NULL;
            printerHandle->alternate = 0xFFu;
            printerHandle->configuration = 0;
            printerHandle->interfaceNumber = 0;
            printerHandle->bulkInBusy = 0U;
            printerHandle->bulkOutBusy = 0U;
            status = kStatus_USB_Success;
            break;

        case kUSB_DeviceClassEventSetConfiguration:
            temp8 = *((uint8_t *)param);
            if (printerHandle->classConfig == NULL)
            {
                break;
            }
            if (temp8 == printerHandle->configuration)
            {
                status = kStatus_USB_Success;
                break;
            }

            /* De-initialize the endpoints when current configuration is none zero. */
            if (printerHandle->configuration != 0)
            {
                status = USB_DevicePrinterEndpointsDeinit(printerHandle);
            }
            /* Save new configuration. */
            printerHandle->configuration = temp8;
            printerHandle->alternate = 0U;
            printerHandle->bulkInBusy = 0U;
            printerHandle->bulkOutBusy = 0U;

            /* Initialize the endpoints of the new current configuration */
            status = USB_DevicePrinterEndpointsInit(printerHandle);
            break;

        case kUSB_DeviceClassEventSetInterface:
            if (printerHandle->classConfig == NULL)
            {
                break;
            }

            /* Get the new alternate setting of the interface */
            temp16 = *((uint16_t *)param);
            /* Get the alternate setting value */
            temp8 = (uint8_t)(temp16 & 0xFFU);
            /* Whether the interface belongs to the class. */
            if (printerHandle->interfaceNumber != ((uint8_t)(temp16 >> 8U)))
            {
                break;
            }
            /* Only handle new alternate setting. */
            if (temp8 == printerHandle->alternate)
            {
                break;
            }

            /* De-initialize old endpoints */
            status = USB_DevicePrinterEndpointsDeinit(printerHandle);
            printerHandle->alternate = temp8;
            printerHandle->bulkInBusy = 0U;
            printerHandle->bulkOutBusy = 0U;
            /* Initialize new endpoints */
            status = USB_DevicePrinterEndpointsInit(printerHandle);
            break;

        case kUSB_DeviceClassEventSetEndpointHalt:
            if ((printerHandle->classConfig == NULL) || (printerHandle->interfaceHandle == NULL))
            {
                break;
            }
            /* Get the endpoint address */
            temp8 = *((uint8_t *)param);
            for (temp16 = 0; temp16 < printerHandle->interfaceHandle->endpointList.count; ++temp16)
            {
                if (temp8 == printerHandle->interfaceHandle->endpointList.endpoint[temp16].endpointAddress)
                {
                    /* Only stall the endpoint belongs to the class */
                    if (USB_IN == ((printerHandle->interfaceHandle->endpointList.endpoint[temp16].endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                       USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
                    {
                       printerHandle->bulkInPipeStall = 1U;
                    }
                    else
                    {
                        printerHandle->bulkOutPipeStall = 1U;
                    }
                    status = USB_DeviceStallEndpoint(printerHandle->deviceHandle, temp8);
                    break;
                }
            }
            break;

        case kUSB_DeviceClassEventClearEndpointHalt:
            if ((printerHandle->classConfig == NULL) || (printerHandle->interfaceHandle == NULL))
            {
                break;
            }
            /* Get the endpoint address */
            temp8 = *((uint8_t *)param);
            for (temp16 = 0; temp16 < printerHandle->interfaceHandle->endpointList.count; ++temp16)
            {
                if (temp8 == printerHandle->interfaceHandle->endpointList.endpoint[temp16].endpointAddress)
                {
                    /* Only un-stall the endpoint belongs to the class */
                    status = USB_DeviceUnstallEndpoint(printerHandle->deviceHandle, temp8);
                    if (USB_IN == (((temp8) & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                    USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
                    {
                        if (printerHandle->bulkInPipeStall)
                        {
                            printerHandle->bulkInPipeStall = 0U;
                            if ((uint8_t*)USB_UNINITIALIZED_VAL_32 != printerHandle->bulkInPipeDataBuffer)
                            {
                                status = USB_DeviceSendRequest(printerHandle->deviceHandle, (printerHandle->interfaceHandle->endpointList.endpoint[temp16].endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK),
                                                              printerHandle->bulkInPipeDataBuffer, printerHandle->bulkInPipeDataLen);
                                if (kStatus_USB_Success != status)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer = printerHandle->bulkInPipeDataBuffer;
                                    endpointCallbackMessage.length = printerHandle->bulkInPipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    USB_DevicePrinterBulkInCallback(printerHandle->deviceHandle,(void*)&endpointCallbackMessage, handle);
                                }
                                printerHandle->bulkInPipeDataBuffer = (uint8_t*)USB_UNINITIALIZED_VAL_32;
                                printerHandle->bulkInPipeDataLen = 0U;
                            }
                        }
                    }
                    else
                    {
                        if (printerHandle->bulkOutPipeStall == 1U)
                        {
                            printerHandle->bulkOutPipeStall = 0U;
                            if ((uint8_t*)USB_UNINITIALIZED_VAL_32 != printerHandle->bulkOutPipeDataBuffer)
                            {
                                status = USB_DeviceRecvRequest(printerHandle->deviceHandle, (printerHandle->interfaceHandle->endpointList.endpoint[temp16].endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK),
                                                              printerHandle->bulkOutPipeDataBuffer, printerHandle->bulkOutPipeDataLen);
                                if (kStatus_USB_Success != status)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer = printerHandle->bulkOutPipeDataBuffer;
                                    endpointCallbackMessage.length = printerHandle->bulkOutPipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    USB_DevicePrinterBulkInCallback(printerHandle->deviceHandle, (void*)&endpointCallbackMessage, handle);
                                }
                                printerHandle->bulkOutPipeDataBuffer = (uint8_t*)USB_UNINITIALIZED_VAL_32;
                                printerHandle->bulkOutPipeDataLen = 0U;
                            }
                        }
                    }
                    break;
                }
            }
            break;

        case kUSB_DeviceClassEventClassRequest:
            if (param)
            {
                /* Handle the printer class specific request. */
                usb_device_control_request_struct_t *controlRequest = (usb_device_control_request_struct_t *)param;
                usb_device_printer_class_request_t classRequest;

                if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) !=
                    USB_REQUEST_TYPE_RECIPIENT_INTERFACE)
                {
                    break;
                }

                switch (controlRequest->setup->bRequest)
                {
                    case USB_DEVICE_PRINTER_GET_DEVICE_ID:
                        /* GET_DEVICE_ID */
                        classRequest.configIndex = (uint8_t)controlRequest->setup->wValue;
                        classRequest.interface = (uint8_t)(controlRequest->setup->wIndex >> 8);
                        classRequest.alternateSetting = (uint8_t)(controlRequest->setup->wIndex);
                        /* ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                        it is from the second parameter of classInit */
                        status = printerHandle->classConfig->classCallback(
                            (class_handle_t)printerHandle, kUSB_DevicePrinterEventGetDeviceId, &classRequest);
                        controlRequest->buffer = classRequest.buffer;
                        controlRequest->length = classRequest.length;
                        break;

                    case USB_DEVICE_PRINTER_GET_PORT_STATUS:
                        /* GET_PORT_STATUS */
                        classRequest.interface = (uint8_t)(controlRequest->setup->wIndex);
                        if (classRequest.interface != printerHandle->interfaceNumber)
                        {
                            controlRequest->buffer = NULL;
                            controlRequest->length = 0U;
                        }
                        else
                        {
                            /* ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            status = printerHandle->classConfig->classCallback(
                                (class_handle_t)printerHandle, kUSB_DevicePrinterEventGetPortStatus, &classRequest);
                            controlRequest->buffer = classRequest.buffer;
                            controlRequest->length = classRequest.length;
                        }
                        break;

                    case USB_DEVICE_PRINTER_SOFT_RESET:
                        classRequest.interface = (uint8_t)(controlRequest->setup->wIndex);
                        if (classRequest.interface != printerHandle->interfaceNumber)
                        {
                            controlRequest->buffer = NULL;
                            controlRequest->length = 0U;
                        }
                        else
                        {
                            /* reset BULK_IN/OUT endpoint and inform application */
                            USB_DevicePrinterEndpointsDeinit(printerHandle);
                            USB_DevicePrinterEndpointsInit(printerHandle);
                            /* ClassCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            status = printerHandle->classConfig->classCallback(
                                (class_handle_t)printerHandle, kUSB_DevicePrinterEventSoftReset, &classRequest);
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

    return status;
}

usb_status_t USB_DevicePrinterSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_printer_struct_t *printerHandle = (usb_device_printer_struct_t *)handle;
    usb_status_t status = kStatus_USB_Error;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (printerHandle->bulkInBusy)
    {
        return kStatus_USB_Busy;
    }
    printerHandle->bulkInBusy = 1U;

    if (printerHandle->bulkInPipeStall)
    {
        printerHandle->bulkInPipeDataBuffer = buffer;
        printerHandle->bulkInPipeDataLen = length;
        return kStatus_USB_Success;
    }

    status = USB_DeviceSendRequest(printerHandle->deviceHandle, ep, buffer, length);
    if (kStatus_USB_Success != status)
    {
        printerHandle->bulkInBusy = 0U;
    }
    return status;
}

usb_status_t USB_DevicePrinterRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_printer_struct_t *printerHandle = (usb_device_printer_struct_t *)handle;
    usb_status_t status = kStatus_USB_Error;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (printerHandle->bulkOutBusy)
    {
        return kStatus_USB_Busy;
    }
    printerHandle->bulkOutBusy = 1U;

    if (printerHandle->bulkOutPipeStall)
    {
        printerHandle->bulkOutPipeDataBuffer = buffer;
        printerHandle->bulkOutPipeDataLen = length;
        return kStatus_USB_Success;
    }
    status = USB_DeviceRecvRequest(printerHandle->deviceHandle, ep, buffer, length);
    if (kStatus_USB_Success != status)
    {
        printerHandle->bulkOutBusy = 0U;
    }

    return status;
}
#endif
