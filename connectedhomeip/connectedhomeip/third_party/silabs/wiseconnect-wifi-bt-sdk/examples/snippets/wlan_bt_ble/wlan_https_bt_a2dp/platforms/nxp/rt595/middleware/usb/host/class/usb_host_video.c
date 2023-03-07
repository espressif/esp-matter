/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_host_config.h"
#if ((defined USB_HOST_CONFIG_VIDEO) && (USB_HOST_CONFIG_VIDEO))
#include "usb_host.h"
#include "usb_host_video.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief open video control interface.
 *
 * @param videoInstance     video instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostVideoControlOpenInterface(usb_host_video_instance_struct_t *videoInstance);

/*!
 * @brief open video stream interface.
 *
 * @param videoInstance     video instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostVideoStreamOpenInterface(usb_host_video_instance_struct_t *videoInstance);

/*!
 * @brief video control pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVideoSetControlInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief video set interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVideoSetStreamInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief video control command transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVideoControlCommandCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief video stream iso in pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVideoStreamIsoInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief video send control transfer common code.
 *
 * @param classHandle     the class handle.
 * @param typeRequest     setup packet request type.
 * @param request         setup packet request value.
 * @param wvalue          setup packet wvalue value.
 * @param windex          setup packet index value.
 * @param wlength         setup packet wlength value.
 * @param data            data buffer pointer will be transfer.
 * @param callbackFn      this callback is called after this function completes.
 * @param callbackParam   the first parameter in the callback function.
 *
 * @return An error code or kStatus_USB_Success.
 */
static usb_status_t USB_HostVideoControl(usb_host_class_handle classHandle,
                                         uint8_t typeRequest,
                                         uint8_t request,
                                         uint16_t wvalue,
                                         uint16_t windex,
                                         uint16_t wlength,
                                         uint8_t *data,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief open video control interface.
 *
 * @param videoInstance     video instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostVideoControlOpenInterface(usb_host_video_instance_struct_t *videoInstance)
{
    usb_status_t status;
    uint8_t ep_index = 0U;
    usb_host_pipe_init_t pipe_init;
    usb_descriptor_endpoint_t *ep_desc = NULL;
    usb_host_interface_t *interface_ptr;

    if (videoInstance->interruptPipe != NULL)
    {
        status = USB_HostClosePipe(videoInstance->hostHandle, videoInstance->interruptPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        videoInstance->interruptPipe = NULL;
    }

    /* open interface pipes */
    interface_ptr = (usb_host_interface_t *)videoInstance->controlIntfHandle;
    for (ep_index = 0U; ep_index < interface_ptr->epCount; ++ep_index)
    {
        ep_desc = interface_ptr->epList[ep_index].epDesc;
        if (((ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((ep_desc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_INTERRUPT))
        {
            pipe_init.devInstance = videoInstance->deviceHandle;
            pipe_init.pipeType = USB_ENDPOINT_INTERRUPT;
            pipe_init.direction = USB_IN;
            pipe_init.endpointAddress = (ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipe_init.interval = ep_desc->bInterval;
            pipe_init.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                 USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipe_init.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                         USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipe_init.nakCount = USB_HOST_CONFIG_MAX_NAK;

            videoInstance->interruptInPacketSize = pipe_init.maxPacketSize;
            videoInstance->interruptInEpNum = pipe_init.endpointAddress;
            status = USB_HostOpenPipe(videoInstance->hostHandle, &videoInstance->interruptPipe, &pipe_init);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("usb_host_video_control_set_interface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
        else
        {
        }
    }

    return kStatus_USB_Success;
}

/*!
 * @brief open video stream interface.
 *
 * @param videoInstance     video instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostVideoStreamOpenInterface(usb_host_video_instance_struct_t *videoInstance)
{
    usb_status_t status;
    uint8_t ep_index = 0U;
    usb_host_pipe_init_t pipe_init;
    usb_descriptor_endpoint_t *ep_desc = NULL;
    usb_host_interface_t *interface_ptr;

    if (videoInstance->streamIsoInPipe != NULL)
    {
        status = USB_HostClosePipe(videoInstance->hostHandle, videoInstance->streamIsoInPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        videoInstance->streamIsoInPipe = NULL;
    }

    /* open interface pipes */
    interface_ptr = (usb_host_interface_t *)videoInstance->streamIntfHandle;
    for (ep_index = 0U; ep_index < interface_ptr->epCount; ++ep_index)
    {
        ep_desc = interface_ptr->epList[ep_index].epDesc;
        if (((ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((ep_desc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_ISOCHRONOUS))
        {
            pipe_init.devInstance = videoInstance->deviceHandle;
            pipe_init.pipeType = USB_ENDPOINT_ISOCHRONOUS;
            pipe_init.direction = USB_IN;
            pipe_init.endpointAddress = (ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipe_init.interval = ep_desc->bInterval;
            pipe_init.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                 USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipe_init.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                         USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK) >>
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_SHFIT;
            pipe_init.nakCount = USB_HOST_CONFIG_MAX_NAK;

            videoInstance->isoInPacketSize = pipe_init.maxPacketSize;
            videoInstance->isoInEpNum = pipe_init.endpointAddress;
            status = USB_HostOpenPipe(videoInstance->hostHandle, &videoInstance->streamIsoInPipe, &pipe_init);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("usb_host_video_stream_set_interface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
        else
        {
        }
    }

    return kStatus_USB_Success;
}

/*!
 * @brief video control pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVideoSetControlInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)param;

    videoInstance->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        status = USB_HostVideoControlOpenInterface(videoInstance);
    }

    if (videoInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the _USB_HostAudioControl,
        USB_HostVideoStreamSetInterface
        or USB_HostVideoControlSetInterface, but is the same function */
        videoInstance->controlCallbackFn(videoInstance->controlCallbackParam, transfer->transferBuffer,
                                         transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(videoInstance->hostHandle, transfer);
}

/*!
 * @brief video set interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVideoSetStreamInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)param;

    videoInstance->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        status = USB_HostVideoStreamOpenInterface(videoInstance);
    }

    if (videoInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the _USB_HostAudioControl,
        USB_HostAudioStreamSetInterface
        or USB_HostAudioControlSetInterface, but is the same function */
        videoInstance->controlCallbackFn(videoInstance->controlCallbackParam, NULL, 0U, status);
    }
    USB_HostFreeTransfer(videoInstance->hostHandle, transfer);
}

/*!
 * @brief video control command transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVideoControlCommandCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)param;

    if (videoInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostCdcControl,
        USB_HostCdcSetControlInterface
        or USB_HostCdcSetDataInterface, but is the same function */
        videoInstance->controlCallbackFn(videoInstance->controlCallbackParam, transfer->transferBuffer,
                                         transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(videoInstance->hostHandle, transfer);
}

/*!
 * @brief video stream iso in pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVideoStreamIsoInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)param;

    if (videoInstance->streamIsoInCallbackFn != NULL)
    {
        /* callback function is initialized in USB_HosVideoStreamRecv */
        videoInstance->streamIsoInCallbackFn(videoInstance->streamIsoInCallbackParam, transfer->transferBuffer,
                                             transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(videoInstance->hostHandle, transfer);
}

/*!
 * @brief video send control transfer common code.
 *
 * @param classHandle     the class handle.
 * @param typeRequest     setup packet request type.
 * @param request         setup packet request value.
 * @param wvalue          setup packet wvalue value.
 * @param windex          setup packet index value.
 * @param wlength         setup packet wlength value.
 * @param data            data buffer pointer will be transfer.
 * @param callbackFn      this callback is called after this function completes.
 * @param callbackParam   the first parameter in the callback function.
 *
 * @return An error code or kStatus_USB_Success.
 */
static usb_status_t USB_HostVideoControl(usb_host_class_handle classHandle,
                                         uint8_t typeRequest,
                                         uint8_t request,
                                         uint16_t wvalue,
                                         uint16_t windex,
                                         uint16_t wlength,
                                         uint8_t *data,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam)
{
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (USB_HostMallocTransfer(videoInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    videoInstance->controlCallbackFn = callbackFn;
    videoInstance->controlCallbackParam = callbackParam;

    transfer->transferBuffer = data;
    transfer->transferLength = wlength;
    transfer->callbackFn = USB_HostVideoControlCommandCallback;
    transfer->callbackParam = videoInstance;
    transfer->setupPacket->bmRequestType = typeRequest;
    transfer->setupPacket->bRequest = request;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(wvalue);
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(windex);
    transfer->setupPacket->wLength = USB_SHORT_TO_LITTLE_ENDIAN(wlength);

    if (USB_HostSendSetup(videoInstance->hostHandle, videoInstance->controlPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failed for USB_HostSendSetup\r\n");
#endif
        USB_HostFreeTransfer(videoInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    videoInstance->controlTransfer = transfer;

    return kStatus_USB_Success;
}

/*!
 * @brief set video class stream interface.
 *
 * This function bind the interface with the video instance.
 *
 * @param classHandle       The class handle.
 * @param interfaceHandle   The interface handle.
 * @param alternateSetting  The alternate setting value.
 * @param callbackFn        This callback is called after this function completes.
 * @param callbackParam     The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 * @retval kStatus_USB_Busy           callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  callback return status, the transfer is stall by device.
 * @retval kStatus_USB_Error          callback return status, open pipe fail, please reference to USB_HostOpenPipe.
 */
usb_status_t USB_HostVideoStreamSetInterface(usb_host_class_handle classHandle,
                                             usb_host_interface_handle interfaceHandle,
                                             uint8_t alternateSetting,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam)
{
    usb_status_t status;
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)classHandle;
    usb_host_interface_t *interface_ptr;
    usb_host_transfer_t *transfer;
    usb_host_video_descriptor_union_t descUnion;
    uint32_t length, ep = 0U;
    uint32_t descLength = 0;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }

    videoInstance->streamIntfHandle = interfaceHandle;

    status = USB_HostOpenDeviceInterface(videoInstance->deviceHandle, interfaceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }

    if (videoInstance->streamIsoInPipe != NULL)
    {
        status = USB_HostCancelTransfer(videoInstance->hostHandle, videoInstance->streamIsoInPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }

    /* open interface pipes */
    interface_ptr = (usb_host_interface_t *)interfaceHandle;

    if (0 == alternateSetting)
    {
        descUnion.bufr = interface_ptr->interfaceExtension;
        length = 0U;
        while (length < interface_ptr->interfaceExtensionLength)
        {
            if (descUnion.common->bDescriptorType == USB_HOST_DESC_CS_INTERFACE)
            {
                if (descUnion.common->bData[0] == USB_HOST_DESC_SUBTYPE_VS_INPUT_HEADER)
                {
                    videoInstance->vsInputHeaderDesc = (usb_host_video_stream_input_header_desc_t *)descUnion.bufr;
                    break;
                }
            }
            length += descUnion.common->bLength;
            descUnion.bufr += descUnion.common->bLength;
        }
    }
    else
    {
        descUnion.bufr = interface_ptr->interfaceExtension;
        length = 0U;
        while (length < interface_ptr->interfaceExtensionLength)
        {
            if ((descUnion.common->bDescriptorType == USB_DESCRIPTOR_TYPE_INTERFACE) &&
                (descUnion.interface->bAlternateSetting == alternateSetting))
            {
                interface_ptr->epCount = descUnion.interface->bNumEndpoints;
                break;
            }
            length += descUnion.common->bLength;
            descUnion.bufr += descUnion.common->bLength;
        }

        while (ep < interface_ptr->epCount)
        {
            if (descUnion.common->bDescriptorType == USB_DESCRIPTOR_TYPE_ENDPOINT)
            {
                interface_ptr->epList[ep].epDesc = (usb_descriptor_endpoint_t *)descUnion.bufr;
                descLength = descUnion.common->bLength;
                descUnion.bufr += descUnion.common->bLength;

                if (USB_HOST_DESC_CS_ENDPOINT == descUnion.common->bDescriptorType)
                {
                    interface_ptr->epList[ep].epExtension = descUnion.bufr;
                    interface_ptr->epList[ep].epExtensionLength = descUnion.common->bLength;
                }
                else
                {
                    descUnion.bufr -= descLength;
                }
                ep++;
            }
            descUnion.bufr += descUnion.common->bLength;
        }
    }

    if (alternateSetting == 0U)
    {
        if (callbackFn != NULL)
        {
            status = USB_HostVideoStreamOpenInterface(videoInstance);
            callbackFn(callbackParam, NULL, 0U, kStatus_USB_Success);
        }
    }
    else
    {
        if (USB_HostMallocTransfer(videoInstance->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }
        videoInstance->controlCallbackFn = callbackFn;
        videoInstance->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn = USB_HostVideoSetStreamInterfaceCallback;
        transfer->callbackParam = videoInstance;
        transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)videoInstance->streamIntfHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer = NULL;
        transfer->transferLength = 0;
        status = USB_HostSendSetup(videoInstance->hostHandle, videoInstance->controlPipe, transfer);

        if (status == kStatus_USB_Success)
        {
            videoInstance->controlTransfer = transfer;
        }
        else
        {
            USB_HostFreeTransfer(videoInstance->hostHandle, transfer);
        }
    }

    return status;
}

/*!
 * @brief set control interface.
 *
 * This function bind the control interface with the video instance.
 *
 * @param classHandle      the class handle.
 * @param interfaceHandle  the control interface handle.
 * @param alternateSetting the alternate setting value.
 * @param callbackFn       this callback is called after this function completes.
 * @param callbackParam    the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 * @retval kStatus_USB_Busy           callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  callback return status, the transfer is stall by device.
 * @retval kStatus_USB_Error          callback return status, open pipe fail, please reference to USB_HostOpenPipe.
 */
usb_status_t USB_HostVideoControlSetInterface(usb_host_class_handle classHandle,
                                              usb_host_interface_handle interfaceHandle,
                                              uint8_t alternateSetting,
                                              transfer_callback_t callbackFn,
                                              void *callbackParam)
{
    usb_status_t status;
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)classHandle;
    usb_host_interface_t *interface_ptr;
    usb_host_transfer_t *transfer;
    usb_host_video_descriptor_union_t desc;
    uint32_t length = 0U;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }
    videoInstance->controlIntfHandle = interfaceHandle;
    interface_ptr = (usb_host_interface_t *)interfaceHandle;

    status = USB_HostOpenDeviceInterface(videoInstance->deviceHandle, interfaceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }
    desc.bufr = interface_ptr->interfaceExtension;

    length = 0U;

    while (length < interface_ptr->interfaceExtensionLength)
    {
        if (((interface_ptr->interfaceDesc->bDescriptorType == USB_DESCRIPTOR_TYPE_INTERFACE) &&
             (interface_ptr->interfaceDesc->bAlternateSetting == alternateSetting)) ||
            ((desc.common->bDescriptorType == USB_DESCRIPTOR_TYPE_INTERFACE) &&
             (desc.interface->bAlternateSetting == alternateSetting)))
        {
            break;
        }
        length += desc.common->bLength;
        desc.bufr += desc.common->bLength;
    }
    while (length < interface_ptr->interfaceExtensionLength)
    {
        if (desc.common->bDescriptorType == USB_HOST_DESC_CS_INTERFACE)
        {
            if (desc.common->bData[0] == USB_HOST_DESC_SUBTYPE_VC_HEADER)
            {
                videoInstance->vcHeaderDesc = (usb_host_video_ctrl_header_desc_t *)desc.bufr;
            }
            else if (desc.common->bData[0] == USB_HOST_DESC_SUBTYPE_VC_INPUT_TERMINAL)
            {
                videoInstance->vcInputTerminalDesc = (usb_host_video_ctrl_it_desc_t *)desc.bufr;
            }
            else if (desc.common->bData[0] == USB_HOST_DESC_SUBTYPE_VC_OUTPUT_TERMINAL)
            {
                videoInstance->vcOutputTerminalDesc = (usb_host_video_ctrl_ot_desc_t *)desc.bufr;
            }
            else if (desc.common->bData[0] == USB_HOST_DESC_SUBTYPE_VC_PROCESSING_UNIT)
            {
                videoInstance->vcProcessingUnitDesc = (usb_host_video_ctrl_pu_desc_t *)desc.bufr;
            }
            else
            {
            }
        }
        length += desc.common->bLength;
        desc.bufr += desc.common->bLength;
    }

    if (alternateSetting == 0U)
    {
        if (callbackFn != NULL)
        {
            status = USB_HostVideoControlOpenInterface(videoInstance);
            callbackFn(callbackParam, NULL, 0U, kStatus_USB_Success);
        }
    }
    else
    {
        if (USB_HostMallocTransfer(videoInstance->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }
        videoInstance->controlCallbackFn = callbackFn;
        videoInstance->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn = USB_HostVideoSetControlInterfaceCallback;
        transfer->callbackParam = videoInstance;
        transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)videoInstance->controlIntfHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer = NULL;
        transfer->transferLength = 0;
        status = USB_HostSendSetup(videoInstance->hostHandle, videoInstance->controlPipe, transfer);

        if (status == kStatus_USB_Success)
        {
            videoInstance->controlTransfer = transfer;
        }
        else
        {
            USB_HostFreeTransfer(videoInstance->hostHandle, transfer);
        }
    }

    return status;
}

/*!
 * @brief video stream receive data.
 *
 * This function implements video receiving data.
 *
 * @param classHandle    The class handle.
 * @param buffer         The buffer pointer.
 * @param bufferLen      The buffer length.
 * @param callbackFn     This callback is called after this function completes.
 * @param callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostRecv.
 */
usb_status_t USB_HosVideoStreamRecv(usb_host_class_handle classHandle,
                                    uint8_t *buffer,
                                    uint32_t bufferLen,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam)
{
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (videoInstance->streamIsoInPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    if (USB_HostMallocTransfer(videoInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    videoInstance->streamIsoInCallbackFn = callbackFn;
    videoInstance->streamIsoInCallbackParam = callbackParam;
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLen;
    transfer->callbackFn = USB_HostVideoStreamIsoInPipeCallback;
    transfer->callbackParam = videoInstance;

    if (USB_HostRecv(videoInstance->hostHandle, videoInstance->streamIsoInPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failed to USB_HostRecv\r\n");
#endif
        USB_HostFreeTransfer(videoInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }

    return kStatus_USB_Success;
}

/*!
 * @brief initialize the video instance.
 *
 * This function allocate the resource for video instance.
 *
 * @param deviceHandle   the device handle.
 * @param classHandle    return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
usb_status_t USB_HostVideoInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle)
{
    usb_host_video_instance_struct_t *videoInstance =
        (usb_host_video_instance_struct_t *)USB_OsaMemoryAllocate(sizeof(usb_host_video_instance_struct_t));
    uint32_t info_value;

    if (videoInstance == NULL)
    {
        return kStatus_USB_AllocFail;
    }

    videoInstance->deviceHandle = deviceHandle;
    videoInstance->controlIntfHandle = NULL;
    videoInstance->streamIntfHandle = NULL;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetHostHandle, &info_value);
    videoInstance->hostHandle = (usb_host_handle)info_value;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceControlPipe, &info_value);
    videoInstance->controlPipe = (usb_host_pipe_handle)info_value;

    *classHandle = videoInstance;
    return kStatus_USB_Success;
}

/*!
 * @brief de-initialize the video instance.
 *
 * This function release the resource for video instance.
 *
 * @param deviceHandle   the device handle.
 * @param classHandle    the class handle.
 *
 * @retval kStatus_USB_Success    The device is de-initialized successfully.
 */
usb_status_t USB_HostVideoDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle)
{
    usb_status_t status;
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)classHandle;

    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (classHandle != NULL)
    {
        if (videoInstance->streamIsoInPipe != NULL)
        {
            status = USB_HostCancelTransfer(videoInstance->hostHandle, videoInstance->streamIsoInPipe, NULL);
            status = USB_HostClosePipe(videoInstance->hostHandle, videoInstance->streamIsoInPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            videoInstance->streamIsoInPipe = NULL;
        }

        USB_HostCloseDeviceInterface(deviceHandle, videoInstance->streamIntfHandle);

        if (videoInstance->interruptPipe != NULL)
        {
            status = USB_HostCancelTransfer(videoInstance->hostHandle, videoInstance->interruptPipe, NULL);
            status = USB_HostClosePipe(videoInstance->hostHandle, videoInstance->interruptPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            videoInstance->interruptPipe = NULL;
        }
        if ((videoInstance->controlPipe != NULL) && (videoInstance->controlTransfer != NULL))
        {
            status = USB_HostCancelTransfer(videoInstance->hostHandle, videoInstance->controlPipe,
                                            videoInstance->controlTransfer);
        }
        USB_HostCloseDeviceInterface(deviceHandle, videoInstance->controlIntfHandle);
        USB_OsaMemoryFree(videoInstance);
    }
    else
    {
        USB_HostCloseDeviceInterface(deviceHandle, NULL);
    }

    return kStatus_USB_Success;
}

/*!
 * @brief get video stream format descriptor.
 *
 * This function implements get video stream format descriptor.
 *
 * @param classHandle   The class handle.
 * @param subType       The descriptor subtype.
 * @param descriptor    The pointer of specific format descriptor.
 *
 * @retval kStatus_USB_Success            Get video stream format descriptor request successfully.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The descriptor is NULL pointer.
 *
 */
usb_status_t USB_HostVideoStreamGetFormatDescriptor(usb_host_class_handle classHandle,
                                                    uint8_t subType,
                                                    void **descriptor)
{
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)classHandle;
    usb_host_interface_t *interface_ptr;
    usb_host_video_descriptor_union_t descUnion;
    uint32_t length = 0U;

    if (NULL == classHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    interface_ptr = (usb_host_interface_t *)videoInstance->streamIntfHandle;

    descUnion.bufr = interface_ptr->interfaceExtension;
    length = 0U;
    while (length < interface_ptr->interfaceExtensionLength)
    {
        if (descUnion.common->bDescriptorType == USB_HOST_DESC_CS_INTERFACE)
        {
            if (descUnion.common->bData[0] == subType)
            {
                *descriptor = descUnion.bufr;
                return kStatus_USB_Success;
                ;
            }
        }
        length += descUnion.common->bLength;
        descUnion.bufr += descUnion.common->bLength;
    }

    return kStatus_USB_Error;
}

/*!
 * @brief get specific video stream frame descriptor.
 *
 * This function implements get specific video stream frame descriptor.
 *
 * @param classHandle        The class handle.
 * @param formatDescriptor   The frame descriptor pointer.
 * @param index              The specific frame descriptor id
 * @param descriptor         The pointer of specific frame descriptor.
 *
 * @retval kStatus_USB_Success           Get video stream frame descriptor request successfully.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The descriptor is NULL pointer.
 *
 */
usb_status_t USB_HostVideoStreamGetFrameDescriptor(
    usb_host_class_handle classHandle, void *formatDescriptor, uint8_t subType, uint8_t frameIndex, void **descriptor)
{
    usb_host_video_stream_payload_format_common_desc_t *formatDesc =
        (usb_host_video_stream_payload_format_common_desc_t *)formatDescriptor;
    usb_host_video_descriptor_union_t desc;
    uint32_t i = 0;
    uint8_t frameCount = 0;

    if (NULL == classHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if ((formatDesc == NULL) || (formatDesc->bDescriptorType != USB_HOST_DESC_CS_INTERFACE))
    {
        return kStatus_USB_InvalidParameter;
    }

    frameCount = formatDesc->bNumFrameDescriptors;
    desc.bufr = (void *)formatDesc;
    desc.bufr += desc.common->bLength;

    while (i <= frameCount)
    {
        if ((desc.video_frame_common->bDescriptorType == USB_HOST_DESC_CS_INTERFACE) &&
            (desc.video_frame_common->bDescriptorSubtype == subType))
        {
            if (desc.video_frame_common->bFrameIndex == frameIndex)
            {
                *descriptor = (void *)desc.bufr;
                return kStatus_USB_Success;
            }
        }
        i++;
        desc.bufr += desc.common->bLength;
    }

    return kStatus_USB_Error;
}

/*!
 * @brief video set probe.
 *
 * This function implements the Video class-specific request (set probe).
 *
 * @param classHandle    the class handle.
 * @param request        setup packet request value.
 * @param probe          video probe data
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success           Request successful.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The interface descriptor is NULL pointer.
 */
usb_status_t USB_HostVideoSetProbe(usb_host_class_handle classHandle,
                                   uint8_t request,
                                   uint8_t *probe,
                                   transfer_callback_t callbackFn,
                                   void *callbackParam)
{
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)classHandle;
    usb_host_interface_t *streamInterface;
    usb_status_t status;
    streamInterface = (usb_host_interface_t *)videoInstance->streamIntfHandle;

    if (NULL == streamInterface)
    {
        return kStatus_USB_InvalidHandle;
    }
    if (NULL == streamInterface->interfaceDesc)
    {
        return kStatus_USB_InvalidParameter;
    }
    status = USB_HostVideoControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        request, USB_HOST_VS_PROBE_CONTROL << 8, streamInterface->interfaceDesc->bInterfaceNumber, 26, probe,
        callbackFn, callbackParam);
    return status;
}

/*!
 * @brief video get probe.
 *
 * This function implements the Video class-specific request (get probe).
 *
 * @param classHandle   the class handle.
 * @param request       setup packet request value.
 * @param probe         video probe data
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success           Request successful.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The interface descriptor is NULL pointer.
 */
usb_status_t USB_HostVideoGetProbe(usb_host_class_handle classHandle,
                                   uint8_t request,
                                   uint8_t *probe,
                                   transfer_callback_t callbackFn,
                                   void *callbackParam)
{
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)classHandle;
    usb_host_interface_t *streamInterface;
    usb_status_t status;
    streamInterface = (usb_host_interface_t *)videoInstance->streamIntfHandle;
    if (NULL == streamInterface)
    {
        return kStatus_USB_InvalidHandle;
    }
    if (NULL == streamInterface->interfaceDesc)
    {
        return kStatus_USB_InvalidParameter;
    }
    status = USB_HostVideoControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        request, USB_HOST_VS_PROBE_CONTROL << 8, streamInterface->interfaceDesc->bInterfaceNumber, 26, probe,
        callbackFn, callbackParam);
    return status;
}

/*!
 * @brief video get commit.
 *
 * This function implements the Video class-specific request (get commit).
 *
 * @param classHandle    the class handle.
 * @param request        setup packet request value.
 * @param probe          video probe data
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success           Request successful.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The interface descriptor is NULL pointer.
 * @retval kStatus_USB_InvalidRequest    The request is invaild.
 */
usb_status_t USB_HostVideoGetCommit(usb_host_class_handle classHandle,
                                    uint8_t brequest,
                                    uint8_t *probe,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam)
{
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)classHandle;
    usb_host_interface_t *streamInterface;
    usb_status_t status;
    streamInterface = (usb_host_interface_t *)videoInstance->streamIntfHandle;
    if (NULL == streamInterface)
    {
        return kStatus_USB_InvalidHandle;
    }
    if (NULL == streamInterface->interfaceDesc)
    {
        return kStatus_USB_InvalidParameter;
    }
    if ((brequest == USB_HOST_VIDEO_GET_DEF) && (brequest == USB_HOST_VIDEO_GET_RES) &&
        (brequest == USB_HOST_VIDEO_GET_MIN) && (brequest == USB_HOST_VIDEO_GET_MAX))
    {
        return kStatus_USB_InvalidRequest;
    }
    status = USB_HostVideoControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        brequest, USB_HOST_VS_COMMIT_CONTROL << 8, streamInterface->interfaceDesc->bInterfaceNumber, 26, probe,
        callbackFn, callbackParam);
    return status;
}

/*!
 * @brief video set commit.
 *
 * This function implements the Video class-specific request (set commit).
 *
 * @param classHandle    the class handle.
 * @param request        setup packet request value.
 * @param probe          video probe data
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success           Request successful.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The interface descriptor is NULL pointer.
 */
usb_status_t USB_HostVideoSetCommit(usb_host_class_handle classHandle,
                                    uint8_t brequest,
                                    uint8_t *probe,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam)
{
    usb_host_video_instance_struct_t *videoInstance = (usb_host_video_instance_struct_t *)classHandle;
    usb_host_interface_t *streamInterface;
    usb_status_t status;
    streamInterface = (usb_host_interface_t *)videoInstance->streamIntfHandle;
    if (NULL == streamInterface)
    {
        return kStatus_USB_InvalidHandle;
    }
    if (NULL == streamInterface->interfaceDesc)
    {
        return kStatus_USB_InvalidParameter;
    }
    status = USB_HostVideoControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        brequest, USB_HOST_VS_COMMIT_CONTROL << 8, streamInterface->interfaceDesc->bInterfaceNumber, 26, probe,
        callbackFn, callbackParam);
    return status;
}

#endif
