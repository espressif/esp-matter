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

#if ((defined(USB_DEVICE_CONFIG_CCID)) && (USB_DEVICE_CONFIG_CCID > 0U))
#include "usb_device_ccid.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static usb_status_t USB_DeviceCcidAllocateHandle(usb_device_ccid_struct_t **handle);
static usb_status_t USB_DeviceCcidFreeHandle(usb_device_ccid_struct_t *handle);
static usb_status_t USB_DeviceCcidRemoveTransfer(usb_device_ccid_transfer_struct_t **transfer_queue,
                                                 usb_device_ccid_transfer_struct_t **transfer);
static usb_status_t USB_DeviceCcidAddTransfer(usb_device_ccid_transfer_struct_t **transfer_queue,
                                              usb_device_ccid_transfer_struct_t *transfer);
static usb_status_t USB_DeviceCcidInterruptIn(usb_device_handle deviceHandle,
                                              usb_device_endpoint_callback_message_struct_t *event,
                                              void *callbackParam);
static usb_status_t USB_DeviceCcidBulkIn(usb_device_handle deviceHandle,
                                         usb_device_endpoint_callback_message_struct_t *event,
                                         void *callbackParam);
static usb_status_t USB_DeviceCcidBulkOut(usb_device_handle deviceHandle,
                                          usb_device_endpoint_callback_message_struct_t *event,
                                          void *callbackParam);
static usb_status_t USB_DeviceCcidEndpointsInit(usb_device_ccid_struct_t *ccidHandle);
static usb_status_t USB_DeviceCcidEndpointsDeinit(usb_device_ccid_struct_t *ccidHandle);

/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static usb_device_ccid_struct_t
    s_UsbDeviceCcidHandle[USB_DEVICE_CONFIG_CCID];

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief Allocate a device ccid class handle.
 *
 * This function allocates a device ccid class handle.
 *
 * @param handle          It is out parameter, is used to return pointer of the device ccid class handle to the caller.
 *
 * @retval kStatus_USB_Success              Get a device ccid class handle successfully.
 * @retval kStatus_USB_Busy                 Cannot allocate a device ccid class handle.
 */
static usb_status_t USB_DeviceCcidAllocateHandle(usb_device_ccid_struct_t **handle)
{
    uint32_t count;
    for (count = 0U; count < USB_DEVICE_CONFIG_CCID; count++)
    {
        if (NULL == s_UsbDeviceCcidHandle[count].handle)
        {
            *handle = &s_UsbDeviceCcidHandle[count];
            return kStatus_USB_Success;
        }
    }

    return kStatus_USB_Busy;
}

/*!
 * @brief Free a device ccid class handle.
 *
 * This function frees a device ccid class handle.
 *
 * @param handle          The device ccid class handle.
 *
 * @retval kStatus_USB_Success              Free device ccid class handle successfully.
 */
static usb_status_t USB_DeviceCcidFreeHandle(usb_device_ccid_struct_t *handle)
{
    handle->handle        = NULL;
    handle->configStruct  = (usb_device_class_config_struct_t *)NULL;
    handle->configuration = 0U;
    handle->alternate     = 0U;
    return kStatus_USB_Success;
}

/*!
 * @brief Remove a transfer node from a queue.
 *
 * This function removes a transfer node from a queue.
 *
 * @param transfer_queue          A pointer points to a queue pointer.
 * @param transfer                It is an OUT parameter, return the transfer node pointer.
 *
 * @retval kStatus_USB_Success              Free device ccid class handle successfully.
 * @retval kStatus_USB_Busy                 Can not get transfer node due to the queue is empty.
 */
static usb_status_t USB_DeviceCcidRemoveTransfer(usb_device_ccid_transfer_struct_t **transfer_queue,
                                                 usb_device_ccid_transfer_struct_t **transfer)
{
    USB_OSA_SR_ALLOC();

    if ((NULL == transfer_queue) || (NULL == transfer))
    {
        return kStatus_USB_InvalidParameter;
    }

    USB_OSA_ENTER_CRITICAL();
    *transfer = *transfer_queue;
    if (*transfer_queue)
    {
        *transfer_queue = (*transfer_queue)->next;
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_Success;
    }
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Busy;
}

/*!
 * @brief Add a transfer node to a queue.
 *
 * This function adds a transfer node to a queue.
 *
 * @param transfer_queue          A pointer points to a queue pointer.
 * @param transfer                The transfer node pointer.
 *
 * @retval kStatus_USB_Success              Free device ccid class handle successfully.
 * @retval kStatus_USB_Error                The transfer node has been added.
 */
static usb_status_t USB_DeviceCcidAddTransfer(usb_device_ccid_transfer_struct_t **transfer_queue,
                                              usb_device_ccid_transfer_struct_t *transfer)
{
    usb_device_ccid_transfer_struct_t *p;
    usb_device_ccid_transfer_struct_t *q;
    USB_OSA_SR_ALLOC();

    if (NULL == transfer_queue)
    {
        return kStatus_USB_InvalidParameter;
    }
    p = *transfer_queue;
    q = *transfer_queue;

    USB_OSA_ENTER_CRITICAL();
    while (p)
    {
        q = p;
        if (p == transfer)
        {
            USB_OSA_EXIT_CRITICAL();
            return kStatus_USB_Error;
        }
        p = p->next;
    }
    transfer->next = NULL;
    if (q)
    {
        q->next = transfer;
    }
    else
    {
        *transfer_queue = transfer;
    }
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Success;
}

/*!
 * @brief Interrupt IN endpoint callback function.
 *
 * This callback function is used to notify uplayer the transfser result of a transfer.
 * This callback pointer is passed when the interrupt IN pipe initialized.
 *
 * @param deviceHandle          The device handle. It equals the value returned from USB_DeviceInit.
 * @param event                  The result of the interrupt IN pipe transfer.
 * @param callbackParam         The parameter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam. In the class, the value is the ccid class handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceCcidInterruptIn(usb_device_handle deviceHandle,
                                              usb_device_endpoint_callback_message_struct_t *event,
                                              void *callbackParam)
{
    usb_device_ccid_struct_t *ccidHandle = (usb_device_ccid_struct_t *)callbackParam;
    usb_device_ccid_notification_struct_t notification;
    usb_status_t error = kStatus_USB_Error;
    USB_OSA_SR_ALLOC();

    if (((NULL == deviceHandle) || (NULL == callbackParam) || (NULL == event)) || (NULL == event->buffer))
    {
        return kStatus_USB_Error;
    }

    /* Save the transed buffer address and length */
    notification.buffer = event->buffer;
    notification.length = event->length;
    usb_device_ccid_notify_slot_chnage_notification_t *ccidNotify =
        (usb_device_ccid_notify_slot_chnage_notification_t *)event->buffer;

    if (USB_DEVICE_CCID_RDR_TO_PC_NOTIFYSLOTCHANGE == ccidNotify->bMessageType)
    {
        if (ccidHandle->configStruct->classCallback)
        {
            /* Notify the up layer, the slot change notification sent.
            classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
            it is from the second parameter of classInit */
            ccidHandle->configStruct->classCallback((class_handle_t)ccidHandle, kUSB_DeviceCcidEventSlotChangeSent,
                                                    &notification);
        }
    }
    else if (USB_DEVICE_CCID_RDR_TO_PC_HARDWAREERROR == ccidNotify->bMessageType)
    {
        if (ccidHandle->configStruct->classCallback)
        {
            /* Notify the up layer, the hardware error notification sent.
            classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
            it is from the second parameter of classInit */
            ccidHandle->configStruct->classCallback((class_handle_t)ccidHandle, kUSB_DeviceCcidEventHardwareErrorSent,
                                                    &notification);
        }
    }
    else
    {
    }
    USB_OSA_ENTER_CRITICAL();
    if ((ccidHandle->configuration) && (ccidHandle->endpointInterruptIn))
    {
        /* If there is a blocking slot changed notification, send it to the host */
        if (ccidHandle->slotsChanged)
        {
            ccidHandle->slotsSendingChangeBuffer[0] = ccidHandle->slotsChangeBuffer[0];
            for (uint8_t i = 1U; i < sizeof(ccidHandle->slotsChangeBuffer); i++)
            {
                ccidHandle->slotsSendingChangeBuffer[i] = ccidHandle->slotsChangeBuffer[i];
                ccidHandle->slotsChangeBuffer[i] &= ~0xAAU;
            }
            error = USB_DeviceSendRequest(ccidHandle->handle, ccidHandle->endpointInterruptIn,
                                          ccidHandle->slotsSendingChangeBuffer,
                                          sizeof(ccidHandle->slotsSendingChangeBuffer));
            if (kStatus_USB_Success == error)
            {
                ccidHandle->slotsChanged    = 0U;
                ccidHandle->interruptInBusy = 1U;
            }
            else
            {
                ccidHandle->interruptInBusy = 0U;
            }
        }
        else
        {
            ccidHandle->interruptInBusy = 0U;
        }
    }
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_Success;
}

/*!
 * @brief Bulk IN endpoint callback function.
 *
 * This callback function is used to notify uplayer the transfser result of a transfer.
 * This callback pointer is passed when the Bulk IN pipe initialized.
 *
 * @param deviceHandle   The device handle. It equals the value returned from USB_DeviceInit.
 * @param event           The result of the Bulk IN pipe transfer.
 * @param callbackParam  The parameter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam. In the class, the value is the ccid class handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceCcidBulkIn(usb_device_handle deviceHandle,
                                         usb_device_endpoint_callback_message_struct_t *event,
                                         void *callbackParam)
{
    usb_device_ccid_struct_t *ccidHandle = (usb_device_ccid_struct_t *)callbackParam;
    usb_device_ccid_transfer_struct_t *transfer;
    usb_status_t error = kStatus_USB_Error;
    if (((NULL == deviceHandle) || (NULL == callbackParam) || (NULL == event)) || (NULL == event->buffer) ||
        (USB_UNINITIALIZED_VAL_32 == event->length))
    {
        return kStatus_USB_Error;
    }

    /* Remove the transed transfer from the busy queue. */
    if (kStatus_USB_Success == USB_DeviceCcidRemoveTransfer(&ccidHandle->transferHead, &transfer))
    {
        if (ccidHandle->configStruct->classCallback)
        {
            /* Notify the up layer, the response sent.
            classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
            it is from the second parameter of classInit */
            ccidHandle->configStruct->classCallback((class_handle_t)ccidHandle, kUSB_DeviceCcidEventResponseSent,
                                                    transfer->buffer);
        }
        /* Add the transfer node to the idle queue */
        USB_DeviceCcidAddTransfer(&ccidHandle->transferFree, transfer);
    }

    /* Clear the bulk IN pipe busy flag. */
    ccidHandle->bulkInBusy = 0U;
    /* If there is a blocking transfer, send it to the host */
    if (ccidHandle->transferHead)
    {
        error = USB_DeviceSendRequest(ccidHandle->handle, ccidHandle->endpointBulkIn, ccidHandle->transferHead->buffer,
                                      ccidHandle->transferHead->length);
        if (kStatus_USB_Success == error)
        {
            ccidHandle->bulkInBusy = 1U;
        }
    }
    return error;
}

/*!
 * @brief Bulk OUT endpoint callback function.
 *
 * This callback function is used to notify uplayer the transfser result of a transfer.
 * This callback pointer is passed when the Bulk OUT pipe initialized.
 *
 * @param deviceHandle   The device handle. It equals the value returned from USB_DeviceInit.
 * @param event           The result of the Bulk OUT pipe transfer.
 * @param callbackParam  The parameter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam. In the class, the value is the ccid class handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceCcidBulkOut(usb_device_handle deviceHandle,
                                          usb_device_endpoint_callback_message_struct_t *event,
                                          void *callbackParam)
{
    usb_device_ccid_struct_t *ccidHandle;
    usb_device_ccid_common_command_t *commonRequest;
    usb_device_ccid_transfer_struct_t *transfer = NULL;
    usb_status_t usbError                       = kStatus_USB_InvalidRequest;
    uint8_t response_error                      = USB_DEVICE_CCID_SLOT_ERROR_COMMAND_NOT_SUPPORTED;

    if (((NULL == deviceHandle) || (NULL == callbackParam) || (NULL == event)) || (NULL == event->buffer) ||
        (USB_UNINITIALIZED_VAL_32 == event->length))
    {
        return kStatus_USB_Error;
    }
    ccidHandle    = (usb_device_ccid_struct_t *)callbackParam;
    commonRequest = (usb_device_ccid_common_command_t *)event->buffer;

    /* Check the slot is valid or not */
    if ((ccidHandle->slots <= commonRequest->bSlot) ||
        (event->length <
         (USB_LONG_FROM_LITTLE_ENDIAN_DATA(commonRequest->dwLength) + USB_DEVICE_CCID_COMMAND_HEADER_LENGTH)))
    {
        if (ccidHandle->slots <= commonRequest->bSlot)
        {
            response_error = USB_DEVICE_CCID_SLOT_ERROR_SLOT_NOT_EXIST;
        }
        else
        {
            response_error = USB_DEVICE_CCID_SLOT_ERROR_BAD_LENGTH;
        }
    }
    else
    {
        /* If the slot is valid, handle the host's request */
        ccidHandle->slotsSequenceNumber[commonRequest->bSlot] = commonRequest->bSeq;
        switch (commonRequest->bMessageType)
        {
            case USB_DEVICE_CCID_PC_TO_RDR_ICCPOWERON:
            case USB_DEVICE_CCID_PC_TO_RDR_ICCPOWEROFF:
            case USB_DEVICE_CCID_PC_TO_RDR_GETSLOTSTATUS:
            case USB_DEVICE_CCID_PC_TO_RDR_XFRBLOCK:
            case USB_DEVICE_CCID_PC_TO_RDR_GETPARAMETERS:
            case USB_DEVICE_CCID_PC_TO_RDR_RESETPARAMETERS:
            case USB_DEVICE_CCID_PC_TO_RDR_SETPARAMETERS:
            case USB_DEVICE_CCID_PC_TO_RDR_ESCAPE:
            case USB_DEVICE_CCID_PC_TO_RDR_ICCCLOCK:
            case USB_DEVICE_CCID_PC_TO_RDR_T0APDU:
            case USB_DEVICE_CCID_PC_TO_RDR_SECURE:
            case USB_DEVICE_CCID_PC_TO_RDR_MECHANICAL:
            case USB_DEVICE_CCID_PC_TO_RDR_ABORT:
            case USB_DEVICE_CCID_PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY:
                if (ccidHandle->configStruct->classCallback)
                {
                    usb_device_ccid_command_struct_t command;
                    command.commandBuffer = event->buffer;
                    command.commandLength = event->length;
                    /* Notify the up layer, the command received, and then the application need to handle the command.
                    classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                    it is from the second parameter of classInit */
                    usbError = ccidHandle->configStruct->classCallback((class_handle_t)ccidHandle,
                                                                       kUSB_DeviceCcidEventCommandReceived, &command);
                    if (kStatus_USB_Success == usbError)
                    {
                        /* Get a new transfer node from the idle queue */
                        if (kStatus_USB_Success == USB_DeviceCcidRemoveTransfer(&ccidHandle->transferFree, &transfer))
                        {
                            /* Save the response buffer and data length to the transfer node */
                            transfer->buffer = command.responseBuffer;
                            transfer->length = command.responseLength;
                            /* Add the transfer node to the busy queue */
                            USB_DeviceCcidAddTransfer(&ccidHandle->transferHead, transfer);
                            if (!ccidHandle->bulkInBusy)
                            {
                                /* If the bulk IN pipe is idle, send the response data to the host */
                                if (kStatus_USB_Success == USB_DeviceSendRequest(ccidHandle->handle,
                                                                                 ccidHandle->endpointBulkIn,
                                                                                 transfer->buffer, transfer->length))
                                {
                                    ccidHandle->bulkInBusy = 1U;
                                }
                            }
                        }
                    }
                }
                break;
            default:
                break;
        }
    }

    /* Response the host when the command failed. */
    if (kStatus_USB_Success != usbError)
    {
        if (kStatus_USB_Success == USB_DeviceCcidRemoveTransfer(&ccidHandle->transferFree, &transfer))
        {
            usb_device_ccid_slot_status_struct_t slotStatus;
            transfer->buffer                = (uint8_t *)&transfer->response;
            transfer->length                = sizeof(transfer->response);
            transfer->response.bMessageType = USB_DEVICE_CCID_RDR_TO_PC_SLOTSTATUS;
            USB_LONG_TO_LITTLE_ENDIAN_DATA(0U, transfer->response.dwLength);
            transfer->response.bSlot = commonRequest->bSlot;
            transfer->response.bSeq  = commonRequest->bSeq;
            slotStatus.clockStatus   = USB_DEVICE_CCID_CLCOK_STATUS_CLOCK_STOPPED_UNKNOWN;
            slotStatus.slot          = commonRequest->bSlot;
            slotStatus.present       = USB_DEVICE_CCID_SLOT_STATUS_ICC_NOT_PRESENT;
            if (ccidHandle->configStruct->classCallback)
            {
                /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                it is from the second parameter of classInit*/
                ccidHandle->configStruct->classCallback((class_handle_t)ccidHandle, kUSB_DeviceCcidEventGetSlotStatus,
                                                        &slotStatus);
            }
            transfer->response.bStatus      = slotStatus.present | USB_DEVICE_CCID_SLOT_STATUS_COMMAND_STATUS_FAILED;
            transfer->response.bError       = response_error;
            transfer->response.bClockStatus = slotStatus.clockStatus;
            USB_DeviceCcidAddTransfer(&ccidHandle->transferHead, transfer);
            if (!ccidHandle->bulkInBusy)
            {
                usbError = USB_DeviceSendRequest(ccidHandle->handle, ccidHandle->endpointBulkIn, transfer->buffer,
                                                 transfer->length);
                if (kStatus_USB_Success == usbError)
                {
                    ccidHandle->bulkInBusy = 1U;
                }
            }
        }
    }
    /* Prime next transfer to receive a command from the host */
    if (ccidHandle->endpointBulkOut)
    {
        USB_DeviceRecvRequest(ccidHandle->handle, ccidHandle->endpointBulkOut, ccidHandle->commandBuffer,
                              sizeof(ccidHandle->commandBuffer));
    }
    return usbError;
}

/*!
 * @brief Initialize the endpoints of the ccid class.
 *
 * This callback function is used to initialize the endpoints of the ccide class.
 *
 * @param ccidHandle          The device ccid class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceCcidEndpointsInit(usb_device_ccid_struct_t *ccidHandle)
{
    usb_device_interface_list_t *interfaceList;
    usb_device_interface_struct_t *interface = (usb_device_interface_struct_t *)NULL;
    usb_status_t error                       = kStatus_USB_Error;

    /* Check the configuration is valid or not. */
    if (!ccidHandle->configuration)
    {
        return error;
    }

    if (ccidHandle->configuration > ccidHandle->configStruct->classInfomation->configurations)
    {
        return error;
    }

    /* Get the interface list of the new configuration. */
    if (NULL == ccidHandle->configStruct->classInfomation->interfaceList)
    {
        return error;
    }
    interfaceList = &ccidHandle->configStruct->classInfomation->interfaceList[ccidHandle->configuration - 1U];

    /* Find stream interface by using the alternate setting of the interface. */
    for (int count = 0U; count < interfaceList->count; count++)
    {
        if ((USB_DEVICE_CCID_CLASS_CODE == interfaceList->interfaces[count].classCode) &&
            (USB_DEVICE_CCID_SUBCLASS_CODE == interfaceList->interfaces[count].subclassCode) &&
            (USB_DEVICE_CCID_PROTOCOL_CODE == interfaceList->interfaces[count].protocolCode))
        {
            for (int index = 0U; index < interfaceList->interfaces[count].count; index++)
            {
                if (interfaceList->interfaces[count].interface[index].alternateSetting == ccidHandle->alternate)
                {
                    interface = &interfaceList->interfaces[count].interface[index];
                    break;
                }
            }
            ccidHandle->interfaceNumber = interfaceList->interfaces[count].interfaceNumber;
            break;
        }
    }
    if (!interface)
    {
        /* Return error if the stream interface is not found. */
        return error;
    }
    /* Keep new interface handle. */
    ccidHandle->interfaceHandle = interface;
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

        if ((USB_ENDPOINT_BULK == (epInitStruct.transferType & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK)) &&
            (USB_IN == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)))
        {
            epCallback.callbackFn      = USB_DeviceCcidBulkIn;
            epInitStruct.zlt           = 1U;
            ccidHandle->endpointBulkIn = epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK;
        }
        else if ((USB_ENDPOINT_BULK == (epInitStruct.transferType & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK)) &&
                 (USB_OUT == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                              USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)))
        {
            epCallback.callbackFn       = USB_DeviceCcidBulkOut;
            ccidHandle->endpointBulkOut = epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK;
        }
        else
        {
            epCallback.callbackFn = USB_DeviceCcidInterruptIn;
            ccidHandle->endpointInterruptIn =
                epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK;
        }
        epCallback.callbackParam = ccidHandle;

        error = USB_DeviceInitEndpoint(ccidHandle->handle, &epInitStruct, &epCallback);
    }
    return error;
}

/*!
 * @brief De-initialize the endpoints of the ccid class.
 *
 * This callback function is used to de-initialize the stream endpoints of the ccid class.
 *
 * @param ccidHandle          The device ccid class handle. It equals the value returned from
 * usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceCcidEndpointsDeinit(usb_device_ccid_struct_t *ccidHandle)
{
    usb_status_t error = kStatus_USB_Error;

    if (!ccidHandle->interfaceHandle)
    {
        return error;
    }
    /* De-initialize all endpoints of the interface */
    for (int count = 0U; count < ccidHandle->interfaceHandle->endpointList.count; count++)
    {
        error = USB_DeviceDeinitEndpoint(ccidHandle->handle,
                                         ccidHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress);
    }
    ccidHandle->endpointBulkIn      = 0U;
    ccidHandle->endpointBulkOut     = 0U;
    ccidHandle->endpointInterruptIn = 0U;
    ccidHandle->interfaceHandle     = NULL;
    return error;
}

/*!
 * @brief Handle the event passed to the ccid class.
 *
 * This function handles the event passed to the ccid class.
 *
 * @param handle          The ccid class handle, got from the usb_device_class_config_struct_t::classHandle.
 * @param event           The event codes. Please refer to the enumeration usb_device_class_event_t.
 * @param param           The param type is determined by the event code.
 *
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success              Free device handle successfully.
 * @retval kStatus_USB_InvalidParameter     The device handle not be found.
 * @retval kStatus_USB_InvalidRequest       The request is invalid, and the control pipe will be stalled by the caller.
 */
usb_status_t USB_DeviceCcidEvent(void *handle, uint32_t event, void *param)
{
    usb_device_ccid_struct_t *ccidHandle;
    usb_device_ccid_notify_slot_chnage_notification_t *ccidNotify;
    uint8_t *temp8;
    usb_status_t error = kStatus_USB_Error;
    uint16_t interfaceAlternate;
    uint8_t alternate;
    USB_OSA_SR_ALLOC();

    if ((!param) || (!handle))
    {
        return kStatus_USB_InvalidHandle;
    }

    /* Get the ccid class handle. */
    ccidHandle = (usb_device_ccid_struct_t *)handle;

    switch (event)
    {
        case kUSB_DeviceClassEventDeviceReset:
            /* Bus reset, clear the configuration. */
            ccidHandle->configuration = 0U;
            break;
        case kUSB_DeviceClassEventSetConfiguration:
            /* Get the new configuration. */
            temp8 = ((uint8_t *)param);
            if (!ccidHandle->configStruct)
            {
                break;
            }
            if (*temp8 == ccidHandle->configuration)
            {
                break;
            }

            /* De-initialize the endpoints when current configuration is none zero. */
            if (ccidHandle->configuration)
            {
                error = USB_DeviceCcidEndpointsDeinit(ccidHandle);
            }
            USB_OSA_ENTER_CRITICAL();
            /* Save new configuration. */
            ccidHandle->configuration = *temp8;
            /* Clear the alternate setting value. */
            ccidHandle->alternate = 0U;

            /* Clear BULK IN pipe busy status */
            ccidHandle->bulkInBusy = 0U;
            /* Clear Interrupt IN pipe busy status */
            ccidHandle->interruptInBusy = 0U;
            /* Clear slot changed status */
            ccidHandle->slotsChanged = 0U;

            /* Reset the idle queue and busy queue */
            ccidHandle->transferFree = &ccidHandle->transfers[0];
            ccidHandle->transferHead = &ccidHandle->transfers[0];
            for (uint8_t i = 1U; i < USB_DEVICE_CONFIG_CCID_TRANSFER_COUNT; i++)
            {
                ccidHandle->transferHead->next = &ccidHandle->transfers[i];
                ccidHandle->transferHead       = &ccidHandle->transfers[i];
            }
            ccidHandle->transferHead->next = NULL;
            ccidHandle->transferHead       = NULL;

            /* Initialize the endpoints of the new current configuration by using the alternate setting 0. */
            error = USB_DeviceCcidEndpointsInit(ccidHandle);
            if (ccidHandle->endpointBulkOut)
            {
                /* Prime a transfer to receive a command from the host */
                USB_DeviceRecvRequest(ccidHandle->handle, ccidHandle->endpointBulkOut, ccidHandle->commandBuffer,
                                      sizeof(ccidHandle->commandBuffer));
            }

            /* Notify the host the slot changed when the interrut IN pipe is valid and there are some ICC present. */
            if (ccidHandle->endpointInterruptIn)
            {
                uint8_t send_slot_change = 0U;
                ccidNotify = (usb_device_ccid_notify_slot_chnage_notification_t *)&ccidHandle->slotsChangeBuffer[0];

                ccidNotify->bMessageType = USB_DEVICE_CCID_RDR_TO_PC_NOTIFYSLOTCHANGE;
                for (uint8_t i = 0U; i < USB_DEVICE_CONFIG_CCID_SLOT_MAX; i++)
                {
                    if (ccidNotify->bmSlotICCState[i >> 2] & (0x01U << ((i % 4U) << 1U)))
                    {
                        ccidNotify->bmSlotICCState[i >> 2] |= (0x02U << ((i % 4U) << 1U));
                        send_slot_change = 1U;
                    }
                }
                if (send_slot_change)
                {
                    ccidHandle->slotsSendingChangeBuffer[0] = ccidHandle->slotsChangeBuffer[0];
                    for (uint8_t i = 1U; i < sizeof(ccidHandle->slotsChangeBuffer); i++)
                    {
                        ccidHandle->slotsSendingChangeBuffer[i] = ccidHandle->slotsChangeBuffer[i];
                        ccidHandle->slotsChangeBuffer[i] &= ~0xAAU;
                    }
                    error = USB_DeviceSendRequest(ccidHandle->handle, ccidHandle->endpointInterruptIn,
                                                  ccidHandle->slotsSendingChangeBuffer,
                                                  sizeof(ccidHandle->slotsSendingChangeBuffer));
                    if (kStatus_USB_Success == error)
                    {
                        ccidHandle->interruptInBusy = 1U;
                    }
                }
            }
            USB_OSA_EXIT_CRITICAL();
            break;
        case kUSB_DeviceClassEventSetInterface:
            if (!ccidHandle->configStruct)
            {
                break;
            }

            /* Get the new alternate setting of the interface */
            interfaceAlternate = *((uint16_t *)param);
            /* Get the alternate setting value */
            alternate = (uint8_t)(interfaceAlternate & 0xFFU);

            /* Whether the interface belongs to the class. */
            if (ccidHandle->interfaceNumber != ((uint8_t)(interfaceAlternate >> 8U)))
            {
                break;
            }
            if (alternate == ccidHandle->alternate)
            {
                break;
            }
            /* De-initialize old endpoints */
            error                 = USB_DeviceCcidEndpointsDeinit(ccidHandle);
            ccidHandle->alternate = alternate;
            /* Initialize new endpoints */
            error = USB_DeviceCcidEndpointsInit(ccidHandle);
            if (ccidHandle->endpointBulkOut)
            {
                /* Prime a transfer to receive a command from the host */
                USB_DeviceRecvRequest(ccidHandle->handle, ccidHandle->endpointBulkOut, ccidHandle->commandBuffer,
                                      sizeof(ccidHandle->commandBuffer));
            }
            break;
        case kUSB_DeviceClassEventSetEndpointHalt:
            if ((!ccidHandle->configStruct) || (!ccidHandle->interfaceHandle))
            {
                break;
            }
            /* Get the endpoint address */
            temp8 = ((uint8_t *)param);
            for (int count = 0U; count < ccidHandle->interfaceHandle->endpointList.count; count++)
            {
                if (*temp8 == ccidHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress)
                {
                    /* Only stall the endpoint belongs to the interface of the class */
                    error = USB_DeviceStallEndpoint(ccidHandle->handle, *temp8);
                }
            }
            break;
        case kUSB_DeviceClassEventClearEndpointHalt:
            if ((!ccidHandle->configStruct) || (!ccidHandle->interfaceHandle))
            {
                break;
            }
            /* Get the endpoint address */
            temp8 = ((uint8_t *)param);
            for (int count = 0U; count < ccidHandle->interfaceHandle->endpointList.count; count++)
            {
                if (*temp8 == ccidHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress)
                {
                    /* Only un-stall the endpoint belongs to the interface of the class */
                    error = USB_DeviceUnstallEndpoint(ccidHandle->handle, *temp8);
                }
            }
            break;
        case kUSB_DeviceClassEventClassRequest:
            if (param)
            {
                /* Handle the ccid class specific request. */
                usb_device_control_request_struct_t *controlRequest = (usb_device_control_request_struct_t *)param;

                if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) !=
                    USB_REQUEST_TYPE_RECIPIENT_INTERFACE)
                {
                    break;
                }

                if ((controlRequest->setup->wIndex & 0xFFU) != ccidHandle->interfaceNumber)
                {
                    break;
                }

                switch (controlRequest->setup->bRequest)
                {
                    case USB_DEVICE_CCID_ABORT:
                        /* Abort a command */
                        if (ccidHandle->configStruct->classCallback)
                        {
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = ccidHandle->configStruct->classCallback((class_handle_t)ccidHandle,
                                                                            kUSB_DeviceCcidEventCommandAbort,
                                                                            &controlRequest->setup->wValue);
                        }
                        break;
                    case USB_DEVICE_CCID_GET_CLOCK_FREQUENCIES:
                        /* Get the clock frequencies */
                        if (ccidHandle->configStruct->classCallback)
                        {
                            usb_device_ccid_control_request_struct_t ccid_request;
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = ccidHandle->configStruct->classCallback(
                                (class_handle_t)ccidHandle, kUSB_DeviceCcidEventGetClockFrequencies, &ccid_request);
                            if (kStatus_USB_Success == error)
                            {
                                controlRequest->buffer = ccid_request.buffer;
                                controlRequest->length = ccid_request.length;
                            }
                        }
                        break;
                    case USB_DEVICE_CCID_GET_DATA_RATES:
                        /* Get the data rates */
                        if (ccidHandle->configStruct->classCallback)
                        {
                            usb_device_ccid_control_request_struct_t ccid_request;
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = ccidHandle->configStruct->classCallback(
                                (class_handle_t)ccidHandle, kUSB_DeviceCcidEventGetDataRate, &ccid_request);
                            if (kStatus_USB_Success == error)
                            {
                                controlRequest->buffer = ccid_request.buffer;
                                controlRequest->length = ccid_request.length;
                            }
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
 * @brief Initialize the ccid class.
 *
 * This function is used to initialize the ccid class.
 *
 * @param controllerId   The controller id of the USB IP. Please refer to the enumeration usb_controller_index_t.
 * @param config          The class configuration information.
 * @param handle          It is out parameter, is used to return pointer of the video class handle to the caller.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCcidInit(uint8_t controllerId, usb_device_class_config_struct_t *config, class_handle_t *handle)
{
    usb_device_ccid_struct_t *ccidHandle;
    usb_status_t error = kStatus_USB_Error;

    /* Allocate a ccid class handle. */
    error = USB_DeviceCcidAllocateHandle(&ccidHandle);

    if (kStatus_USB_Success != error)
    {
        return error;
    }
    /* Get the device handle according to the controller id. */
    error = USB_DeviceClassGetDeviceHandle(controllerId, &ccidHandle->handle);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    if (!ccidHandle->handle)
    {
        USB_DeviceCcidFreeHandle(ccidHandle);
        return kStatus_USB_InvalidHandle;
    }
    /* Save the configuration of the class. */
    ccidHandle->configStruct = config;
    /* Clear the configuration value. */
    ccidHandle->configuration       = 0U;
    ccidHandle->alternate           = 0xffU;
    ccidHandle->bulkInBusy          = 0U;
    ccidHandle->endpointBulkIn      = 0U;
    ccidHandle->endpointBulkOut     = 0U;
    ccidHandle->endpointInterruptIn = 0U;
    ccidHandle->slots               = 0U;
    ccidHandle->interruptInBusy     = 0U;
    ccidHandle->slotsChanged        = 0U;

    if (ccidHandle->configStruct->classCallback)
    {
        /* Get the max slot count of the application.
        classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        ccidHandle->configStruct->classCallback((class_handle_t)ccidHandle, kUSB_DeviceCcidEventGetSlotCount,
                                                &ccidHandle->slots);
    }

    if (ccidHandle->slots > USB_DEVICE_CONFIG_CCID_SLOT_MAX)
    {
        USB_DeviceCcidFreeHandle(ccidHandle);
        return kStatus_USB_Error;
    }

    ccidHandle->transferFree = &ccidHandle->transfers[0];
    ccidHandle->transferHead = &ccidHandle->transfers[0];
    for (uint8_t i = 1U; i < USB_DEVICE_CONFIG_CCID_TRANSFER_COUNT; i++)
    {
        ccidHandle->transferHead->next = &ccidHandle->transfers[i];
        ccidHandle->transferHead       = &ccidHandle->transfers[i];
    }
    ccidHandle->transferHead->next = NULL;
    ccidHandle->transferHead       = NULL;

    *handle = (class_handle_t)ccidHandle;
    return error;
}

/*!
 * @brief De-initialize the device ccid class.
 *
 * The function de-initializes the device ccid class.
 *
 * @param handle The ccid class handle got from usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCcidDeinit(class_handle_t handle)
{
    usb_device_ccid_struct_t *ccidHandle;
    usb_status_t error = kStatus_USB_Error;

    ccidHandle = (usb_device_ccid_struct_t *)handle;

    if (!ccidHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    /* De-initialzie the endpoints. */
    error = USB_DeviceCcidEndpointsDeinit(ccidHandle);
    /* Free the ccid class handle. */
    USB_DeviceCcidFreeHandle(ccidHandle);
    return error;
}

/*!
 * @brief Notify the slot status changed.
 *
 * The function is used to notify the slot status changed. This is a un-blocking function, the event
 * kUSB_DeviceCcidEventSlotChangeSent
 * will be asserted when the transfer completed.
 *
 * The slot status may not be sent to the host if the interrupt IN pipe is busy. And the status will be save internally,
 * and will be sent to the host when the interrupt IN pipe callback called. So, the event
 * kUSB_DeviceCcidEventSlotChangeSent
 * happened times will not equal to the function call times of this function.
 *
 * @param handle The ccid class handle got from usb_device_class_config_struct_t::classHandle.
 * @param slot   The changed slot number.
 * @param state  The changed slot status.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCcidNotifySlotChange(class_handle_t handle, uint8_t slot, usb_device_ccid_slot_state_t state)
{
    usb_device_ccid_struct_t *ccidHandle;
    usb_device_ccid_notify_slot_chnage_notification_t *ccidNotify;
    usb_status_t error = kStatus_USB_Error;
    USB_OSA_SR_ALLOC();

    ccidHandle = (usb_device_ccid_struct_t *)handle;

    if (!ccidHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (slot >= USB_DEVICE_CONFIG_CCID_SLOT_MAX)
    {
        return kStatus_USB_InvalidParameter;
    }

    USB_OSA_ENTER_CRITICAL();
    ccidNotify = (usb_device_ccid_notify_slot_chnage_notification_t *)&ccidHandle->slotsChangeBuffer[0];

    ccidNotify->bMessageType = USB_DEVICE_CCID_RDR_TO_PC_NOTIFYSLOTCHANGE;
    ccidNotify->bmSlotICCState[slot >> 2] &= ~(0x03U << ((slot % 4U) << 1U));
    ccidNotify->bmSlotICCState[slot >> 2] |=
        ((0x02U | ((kUSB_DeviceCcidSlotStateNoPresent == state) ? 0x00U : 0x01U)) << ((slot % 4U) << 1U));

    if ((ccidHandle->configuration) && (ccidHandle->endpointInterruptIn))
    {
        if (!ccidHandle->interruptInBusy)
        {
            ccidHandle->slotsSendingChangeBuffer[0] = ccidHandle->slotsChangeBuffer[0];
            for (uint8_t i = 1U; i < sizeof(ccidHandle->slotsChangeBuffer); i++)
            {
                ccidHandle->slotsSendingChangeBuffer[i] = ccidHandle->slotsChangeBuffer[i];
                ccidHandle->slotsChangeBuffer[i] &= ~0xAAU;
            }
            error = USB_DeviceSendRequest(ccidHandle->handle, ccidHandle->endpointInterruptIn,
                                          ccidHandle->slotsSendingChangeBuffer,
                                          sizeof(ccidHandle->slotsSendingChangeBuffer));
            if (kStatus_USB_Success == error)
            {
                ccidHandle->interruptInBusy = 1U;
            }
        }
        else
        {
            ccidHandle->slotsChanged = 1U;
        }
    }
    USB_OSA_EXIT_CRITICAL();
    return error;
}

/*!
 * @brief Notify the slot status changed.
 *
 * The function is used to notify the hardware error. This is a un-blocking function, the event
 * kUSB_DeviceCcidEventHardwareErrorSent
 * will be asserted when the transfer completed.
 *
 * If the interrupt IN pipe is busy, the function will return an error kStatus_USB_Error.
 *
 * @param handle      The ccid class handle got from usb_device_class_config_struct_t::classHandle.
 * @param slot        The changed slot number.
 * @param errorCode  The hardware error code.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCcidNotifyHardwareError(class_handle_t handle,
                                               uint8_t slot,
                                               usb_device_ccid_hardware_error_t errorCode)
{
    usb_device_ccid_struct_t *ccidHandle;
    usb_status_t error = kStatus_USB_Error;
    USB_OSA_SR_ALLOC();

    ccidHandle = (usb_device_ccid_struct_t *)handle;

    if (!ccidHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (slot >= USB_DEVICE_CONFIG_CCID_SLOT_MAX)
    {
        return kStatus_USB_InvalidParameter;
    }

    ccidHandle->hardwareError.bMessageType       = USB_DEVICE_CCID_RDR_TO_PC_HARDWAREERROR;
    ccidHandle->hardwareError.bHardwareErrorCode = errorCode;
    ccidHandle->hardwareError.bSlot              = slot;
    ccidHandle->hardwareError.bSeq               = ccidHandle->slotsSequenceNumber[slot];

    USB_OSA_ENTER_CRITICAL();
    if ((ccidHandle->configuration) && (ccidHandle->endpointInterruptIn))
    {
        if (!ccidHandle->interruptInBusy)
        {
            error = USB_DeviceSendRequest(ccidHandle->handle, ccidHandle->endpointInterruptIn,
                                          (uint8_t *)&ccidHandle->hardwareError, sizeof(ccidHandle->hardwareError));
            if (kStatus_USB_Success == error)
            {
                ccidHandle->interruptInBusy = 1U;
            }
        }
    }
    USB_OSA_EXIT_CRITICAL();
    return error;
}
#endif
