/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017, 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#if USB_DEVICE_CONFIG_CDC_RNDIS
#include "usb_device_cdc_rndis.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined(USB_STACK_BM)
#define USB_CDC_RNDIS_MUTEX_LOCK(_X_) \
    USB_OSA_SR_ALLOC();               \
    USB_OSA_ENTER_CRITICAL()
#define USB_CDC_RNDIS_MUTEX_UNLOCK(_X_) USB_OSA_EXIT_CRITICAL()
#else
#define USB_CDC_RNDIS_MUTEX_LOCK(_X_) USB_OsaMutexLock(_X_)
#define USB_CDC_RNDIS_MUTEX_UNLOCK(_X_) USB_OsaMutexUnlock(_X_)
#endif

#define NOTIF_PACKET_SIZE (0x08)
#define VENDOR_INFO_SIZE (16)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
usb_status_t USB_DeviceCdcRndisInitializeCommand(usb_device_cdc_rndis_struct_t *handle,
                                                 uint8_t **message,
                                                 uint32_t *len);
usb_status_t USB_DeviceCdcRndisKeepaliveCommand(usb_device_cdc_rndis_struct_t *handle,
                                                uint8_t **message,
                                                uint32_t *len);
usb_status_t USB_DeviceCdcRndisQueryCommand(usb_device_cdc_rndis_struct_t *handle, uint8_t **message, uint32_t *len);
usb_status_t USB_DeviceCdcRndisSetCommand(usb_device_cdc_rndis_struct_t *handle, uint8_t **message, uint32_t *len);
usb_status_t USB_DeviceCdcRndisIndicateStatusCommand(usb_device_cdc_rndis_struct_t *handle,
                                                     uint8_t **message,
                                                     uint32_t *len);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* The USB CDC RNDIS device instance. */
static usb_device_cdc_rndis_struct_t s_cdcRndisHandle[USB_DEVICE_CONFIG_CDC_RNDIS_MAX_INSTANCE];

/* The response data for RNDIS notification. */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_responseAvailableData[NOTIF_PACKET_SIZE] = {
    USB_DEVICE_CDC_NOTIF_RESPONSE_AVAIL, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/* The buffer to store RNDIS request. */
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_rndisCommand[RNDIS_MAX_EXPECTED_COMMAND_SIZE];
/* The 4-byte-aligned buffer to store RNDIS response. */
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint32_t s_responseData[(RNDIS_MAX_EXPECTED_RESPONSE_SIZE + 3) / sizeof(uint32_t)];

/* The buffer to store RNDIS device vendor information. */
static uint8_t s_vendorInfo[VENDOR_INFO_SIZE] = " FSL RNDIS PORT ";

/* The array of all the OIDs that are supported. */
static uint32_t s_listSuppOid[RNDIS_NUM_OIDS_SUPPORTED] = {NDIS_OID_GEN_SUPPORTED_LIST,
                                                           NDIS_OID_GEN_HARDWARE_STATUS,
                                                           NDIS_OID_GEN_MEDIA_SUPPORTED,
                                                           NDIS_OID_GEN_MEDIA_IN_USE,
                                                           NDIS_OID_GEN_MAXIMUM_FRAME_SIZE,
                                                           NDIS_OID_GEN_LINK_SPEED,
                                                           NDIS_OID_GEN_TRANSMIT_BLOCK_SIZE,
                                                           NDIS_OID_GEN_RECEIVE_BLOCK_SIZE,
                                                           NDIS_OID_GEN_VENDOR_ID,
                                                           NDIS_OID_GEN_VENDOR_DESCRIPTION,
                                                           NDIS_OID_GEN_CURRENT_PACKET_FILTER,
                                                           NDIS_OID_GEN_MAXIMUM_TOTAL_SIZE,
                                                           NDIS_OID_GEN_MEDIA_CONNECT_STATUS,
                                                           NDIS_OID_GEN_XMIT_OK,
                                                           NDIS_OID_GEN_RCV_OK,
                                                           NDIS_OID_GEN_XMIT_ERROR,
                                                           NDIS_OID_GEN_RCV_ERROR,
                                                           NDIS_OID_GEN_RCV_NO_BUFFER,
                                                           NDIS_OID_802_3_PERMANENT_ADDRESS,
                                                           NDIS_OID_802_3_CURRENT_ADDRESS,
                                                           NDIS_OID_802_3_MULTICAST_LIST,
                                                           NDIS_OID_802_3_MAXIMUM_LIST_SIZE,
                                                           NDIS_OID_802_3_RCV_ERROR_ALIGNMENT,
                                                           NDIS_OID_802_3_XMIT_ONE_COLLISION,
                                                           NDIS_OID_802_3_XMIT_MORE_COLLISIONS};

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Allocates a handle for USB CDC RNDIS device.
 *
 * This function allocates a handle for USB CDC RNDIS device.
 *
 * @param handle The device handle of the CDC RNDIS device.
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceCdcRndisAllocateHandle(usb_device_cdc_rndis_struct_t **handle)
{
    uint32_t count;
    for (count = 0; count < USB_DEVICE_CONFIG_CDC_RNDIS_MAX_INSTANCE; count++)
    {
        if (NULL == (void *)s_cdcRndisHandle[count].cdcAcmHandle)
        {
            *handle = &s_cdcRndisHandle[count];
            return kStatus_USB_Success;
        }
    }

    return kStatus_USB_Busy;
}

/*!
 * @brief Free the handle for USB CDC RNDIS device.
 *
 * This function frees the handle for USB CDC RNDIS device.
 *
 * @param handle The device handle of the CDC RNDIS device.
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceCdcRndisFreeHandle(usb_device_cdc_rndis_struct_t *handle)
{
    handle->cdcAcmHandle = (uint32_t)NULL;
    return kStatus_USB_Success;
}

/*!
 * @brief Initializes the USB CDC RNDIS device.
 *
 * This function initializes the USB CDC RNDIS device.
 *
 * @param classHandle The class handle of the CDC ACM class.
 * @param config The pointer to the configure parameter.
 * @param handle The pointer to pointer of the RNDIS device.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisInit(class_handle_t classHandle,
                                    usb_device_cdc_rndis_config_struct_t *config,
                                    usb_device_cdc_rndis_struct_t **handle)
{
    usb_device_cdc_rndis_struct_t *cdcRndisHandle;
    usb_status_t error = kStatus_USB_Error;

    error = USB_DeviceCdcRndisAllocateHandle(&cdcRndisHandle);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    /* Initially RNDIS is in Uninitialized state */
    cdcRndisHandle->cdcAcmHandle            = classHandle;
    cdcRndisHandle->rndisDeviceState        = RNDIS_UNINITIALIZED;
    cdcRndisHandle->rndisHwState            = NDIS_HARDWARE_STATUS_NOT_READY;
    cdcRndisHandle->rndisMediaConnectStatus = NDIS_MEDIA_STATE_UNKNOWN;
    cdcRndisHandle->rndisDevMaxTxSize       = config->devMaxTxSize;
    cdcRndisHandle->rndisCallback           = config->rndisCallback;
    cdcRndisHandle->rndisCommand            = &s_rndisCommand[0];
    cdcRndisHandle->responseData            = (uint8_t *)&s_responseData[0];
    if (kStatus_USB_OSA_Success != USB_OsaMutexCreate(&(cdcRndisHandle->statusMutex)))
    {
#ifdef DEBUG
        usb_echo("mutex create error!");
#endif
    }
    *handle = cdcRndisHandle;
    return error;
}

/*!
 * @brief De-initializes the USB CDC RNDIS device.
 *
 * This function de-initializes the USB CDC RNDIS device.
 *
 * @param handle The pointer the RNDIS device.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisDeinit(usb_device_cdc_rndis_struct_t *handle)
{
    usb_device_cdc_rndis_struct_t *cdcRndisHandle;
    usb_status_t error = kStatus_USB_Error;

    cdcRndisHandle = handle;

    if (!cdcRndisHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    if (kStatus_USB_OSA_Success != USB_OsaMutexDestroy(cdcRndisHandle->statusMutex))
    {
#ifdef DEBUG
        usb_echo("mutex destroy error!");
#endif
    }
    error = USB_DeviceCdcRndisFreeHandle(cdcRndisHandle);
    return error;
}

/*!
 * @brief Send notification to host that a response is available.
 *
 * This function is called to send notification to host that a response is available.
 *
 * @param handle The pointer the RNDIS device.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisResponseAvailable(usb_device_cdc_rndis_struct_t *handle)
{
    usb_device_cdc_acm_struct_t *cdcAcmHandle;
    usb_status_t error = kStatus_USB_Error;
    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    cdcAcmHandle = (usb_device_cdc_acm_struct_t *)(handle->cdcAcmHandle);
    /* update array for current interface */
    s_responseAvailableData[4] = cdcAcmHandle->interfaceNumber;

    error = USB_DeviceCdcAcmSend((uint32_t)cdcAcmHandle, cdcAcmHandle->interruptIn.ep, s_responseAvailableData,
                                 NOTIF_PACKET_SIZE);
    return error;
}

/*!
 * @brief Response to kUSB_DeviceCdcEventSendEncapsulatedCommand.
 *
 * This function is called in response to kUSB_DeviceCdcEventSendEncapsulatedCommand.
 *
 * @param handle The pointer the RNDIS device.
 * @param message The pointer to the pointer of the message buffer.
 * @param len The pointer of the length of the buffer.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisMessageSet(usb_device_cdc_rndis_struct_t *handle, uint8_t **message, uint32_t *len)
{
    uint32_t messageType;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (*len > RNDIS_MAX_EXPECTED_COMMAND_SIZE)
    {
        return kStatus_USB_Error;
    }

    messageType = USB_LONG_TO_LITTLE_ENDIAN(*((uint32_t *)(*message)));

    if (messageType == RNDIS_HALT_MSG)
    {
        /* No response is send to host on receiving Halt Command */
        USB_DeviceCdcRndisHaltCommand(handle);
    }
    else
    {
        USB_DeviceCdcRndisResponseAvailable(handle);
    }

    if (messageType == RNDIS_INITIALIZE_MSG)
    {
        /* Update the NDIS HW status */
        handle->rndisHwState = NDIS_HARDWARE_STATUS_INITIALIZING;
    }

    return kStatus_USB_Success;
}

/*!
 * @brief Response to kUSB_DeviceCdcEventGetEncapsulatedResponse.
 *
 * This function is called in response to kUSB_DeviceCdcEventGetEncapsulatedResponse.
 *
 * @param handle The pointer the RNDIS device.
 * @param message The pointer to the pointer of the message buffer.
 * @param len The pointer of the length of the buffer.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisMessageGet(usb_device_cdc_rndis_struct_t *handle, uint8_t **message, uint32_t *len)
{
    uint32_t messageType;
    uint32_t controlRequestLen;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    controlRequestLen = *len;
    *len              = 0;

    /* we can avoid one swap operation by using messageType in
       PSTN_Rndis_Message_Set instead of messageType, but this gives
       cleaner implementation as all manipulations and parsing on command
       are done in this function */
    messageType = USB_LONG_TO_LITTLE_ENDIAN(*(uint32_t *)(handle->rndisCommand));

    switch (messageType)
    {
        case RNDIS_INITIALIZE_MSG:
            /* Preparing for response to RNDIS_INITIALIZE_MSG command
               i.e. RNDIS_INITIALIZE_CMPLT data */
            USB_DeviceCdcRndisInitializeCommand(handle, message, len);
            break;
        case RNDIS_QUERY_MSG:
            /* Preparing for response to RNDIS_QUERY_MSG command
               i.e. RNDIS_QUERY_CMPLT data */
            /* correct the endianness of OID */
            ((rndis_query_msg_struct_t *)(handle->rndisCommand))->oid =
                USB_LONG_TO_LITTLE_ENDIAN(((rndis_query_msg_struct_t *)(handle->rndisCommand))->oid);
            USB_DeviceCdcRndisQueryCommand(handle, message, len);
            break;
        case RNDIS_SET_MSG:
            /* Preparing for response to RNDIS_SET_MSG command
               i.e. RNDIS_SET_CMPLT data */
            /* Correct the endianness of OID and InformationBufferLength
               and InformationBufferOffset*/
            ((rndis_set_msg_struct_t *)(handle->rndisCommand))->oid =
                USB_LONG_TO_LITTLE_ENDIAN(((rndis_set_msg_struct_t *)(handle->rndisCommand))->oid);
            ((rndis_set_msg_struct_t *)(handle->rndisCommand))->informationBufferLength =
                USB_LONG_TO_LITTLE_ENDIAN(((rndis_set_msg_struct_t *)(handle->rndisCommand))->informationBufferLength);
            ((rndis_set_msg_struct_t *)(handle->rndisCommand))->informationBufferOffset =
                USB_LONG_TO_LITTLE_ENDIAN(((rndis_set_msg_struct_t *)(handle->rndisCommand))->informationBufferOffset);
            USB_DeviceCdcRndisSetCommand(handle, message, len);
            break;
        case RNDIS_RESET_MSG:
            /* Preparing for response to RNDIS_RESET_MSG command
               i.e. RNDIS_RESET_CMPLT data */
            USB_DeviceCdcRndisResetCommand(handle, message, len);
            break;
        case RNDIS_INDICATE_STATUS_MSG:
            /* Preparing for response to RNDIS_INDICATE_STATUS_MSG
               command */
            USB_DeviceCdcRndisIndicateStatusCommand(handle, message, len);
            break;
        case RNDIS_KEEPALIVE_MSG:
            /* Preparing for response to RNDIS_KEEPALIVE_MSG command
               i.e. RNDIS_KEEPALIVE_CMPLT data */
            USB_DeviceCdcRndisKeepaliveCommand(handle, message, len);
            break;
        default:
#ifdef DEBUG
            usb_echo("UNSUPPORTED RNDIS Command : 0x%x\n", messageType);
#endif
            return kStatus_USB_InvalidRequest;
    }

    if (controlRequestLen < *len)
    {
        return kStatus_USB_InvalidRequest;
    }
    return kStatus_USB_Success;
}

/*!
 * @brief initialize the RNDIS device and prepare data to be sent to host as RNDIS_INITIALIZE_CMPLT.
 *
 * This function is called to initialize the RNDIS device and prepare
 * data to be sent to host as RNDIS_INITIALIZE_CMPLT.
 *
 * @param handle The pointer the RNDIS device.
 * @param message The pointer to the pointer of the message buffer.
 * @param len The pointer of the length of the buffer.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisInitializeCommand(usb_device_cdc_rndis_struct_t *handle,
                                                 uint8_t **message,
                                                 uint32_t *len)
{
    rndis_init_msg_struct_t *rndisInitMsg;
    rndis_init_cmplt_struct_t *rndisInitCmplt;
    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    rndisInitMsg   = (rndis_init_msg_struct_t *)handle->rndisCommand;
    rndisInitCmplt = (rndis_init_cmplt_struct_t *)handle->responseData;

    /* preparing for Byte 0-3 : MessageType*/
    rndisInitCmplt->messageType = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_INITIALIZE_CMPLT);

    /* preparing for Byte 4-7 : MessageLength*/
    rndisInitCmplt->messageLength = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_RESPONSE_INITIALIZE_MSG_SIZE);

    /* preparing for Byte 8-11 : RequestID*/
    rndisInitCmplt->requestID = rndisInitMsg->requestID;

    /* preparing for Byte 12-15 : Status*/
    if (handle->rndisDeviceState == RNDIS_UNINITIALIZED)
    {
        rndisInitCmplt->status = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_STATUS_SUCCESS);
    }
    else
    {
        rndisInitCmplt->status = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_STATUS_FAILURE);
    }

    /* preparing for Byte 16-19 ; MajorVersion*/
    /* We are currently returning the same driver version to host in
       response to initialization command as reported by host driver */
    rndisInitCmplt->majorVersion = rndisInitMsg->majorVersion;

    /* preparing for Byte 20-23 : MinorVersion*/
    rndisInitCmplt->minorVersion = rndisInitMsg->minorVersion;

    /* preparing for Byte 24-27 : DeviceFlags*/
    rndisInitCmplt->deviceFlags = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_DF_CONNECTIONLESS);

    /* preparing for Byte 28-31 : Medium*/
    rndisInitCmplt->medium = USB_LONG_TO_LITTLE_ENDIAN(NDIS_MEDIUM802_3);

    /* preparing for Byte 32-35 : MaxPacketsPerTransfer*/
    /* We are not implementing multiple packet transfer support in our RNDIS */
    rndisInitCmplt->maxPacketsPerTransfer = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_SINGLE_PACKET_TRANSFER);

    /* preparing for Byte 36-39 : MaxTransferSize*/
    /* We are currently returning the same max transfer size to host
       as it send to device in its corresponding filed in
       initialization command */
    handle->rndisHostMaxTxSize      = USB_LONG_TO_LITTLE_ENDIAN(rndisInitMsg->maxTransferSize);
    rndisInitCmplt->maxTransferSize = USB_LONG_TO_LITTLE_ENDIAN(handle->rndisDevMaxTxSize);

    /* preparing for Byte 40-43 : PacketAlignmentFactor*/
    rndisInitCmplt->packetAlignmentFactor = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_PACKET_ALIGNMENT_FACTOR);

    /* preparing for Byte 44-47 : AFListOffset*/
    rndisInitCmplt->afListOffset = RNDIS_AF_LIST_OFFSET;

    /* preparing for Byte 48-51 : AFListSize*/
    rndisInitCmplt->afListSize = RNDIS_AF_LIST_SIZE;

    *message = handle->responseData;
    *len     = RNDIS_RESPONSE_INITIALIZE_MSG_SIZE;

    /* initializing RNDIS variables */
    handle->rndisDeviceState            = RNDIS_INITIALIZED;
    handle->rndisHwState                = NDIS_HARDWARE_STATUS_READY;
    handle->rndisMediaConnectStatus     = NDIS_MEDIA_STATE_DISCONNECTED;
    handle->numFramesTxOk               = 0;
    handle->numFramesRxOk               = 0;
    handle->numFramesTxError            = 0;
    handle->numFramesRxError            = 0;
    handle->numRecvFramesMissed         = 0;
    handle->numRecvFramesAlignmentError = 0;
    handle->numFramesTxOneCollision     = 0;
    handle->numFramesTxManyCollision    = 0;
    return kStatus_USB_Success;
}

/*!
 * @brief check the health of RNDIS device and prepare data to be sent to host as RNDIS_KEEPALIVE_CMPLT.
 *
 * This function is called to check the health of RNDIS device and prepare
 * data to be sent to host as RNDIS_KEEPALIVE_CMPLT.
 *
 * @param handle The pointer the RNDIS device.
 * @param message The pointer to the pointer of the message buffer.
 * @param len The pointer of the length of the buffer.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisKeepaliveCommand(usb_device_cdc_rndis_struct_t *handle, uint8_t **message, uint32_t *len)

{
    rndis_keepalive_msg_struct_t *rndisKeepaliveMsg;
    rndis_keepalive_cmplt_struct_t *rndisKeepaliveCmplt;
    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    rndisKeepaliveMsg   = (rndis_keepalive_msg_struct_t *)handle->rndisCommand;
    rndisKeepaliveCmplt = (rndis_keepalive_cmplt_struct_t *)handle->responseData;

    /* preparing for Byte 0-3 : MessageType*/
    rndisKeepaliveCmplt->messageType = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_KEEPALIVE_CMPLT);

    /* preparing for Byte 4-7 : MessageLength*/
    rndisKeepaliveCmplt->messageLength = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_RESPONSE_KEEPALIVE_MSG_SIZE);

    /* preparing for Byte 8-11 : RequestID*/
    rndisKeepaliveCmplt->requestID = rndisKeepaliveMsg->requestID;

    /* preparing for Byte 12-15 : Status*/
    USB_CDC_RNDIS_MUTEX_LOCK(handle->statusMutex);
    if (handle->rndisDeviceState == RNDIS_DATA_INITIALIZED)
    {
        rndisKeepaliveCmplt->status = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_STATUS_SUCCESS);
    }
    else
    {
        rndisKeepaliveCmplt->status = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_STATUS_FAILURE);
    }
    USB_CDC_RNDIS_MUTEX_UNLOCK(handle->statusMutex);

    *message = (uint8_t *)rndisKeepaliveCmplt;
    *len     = RNDIS_RESPONSE_KEEPALIVE_MSG_SIZE;
    return kStatus_USB_Success;
}

/*!
 * @brief Query the RNDIS device for its characteristics or statistics information .
 *
 * This function is called to query the RNDIS device for its
 * characteristics or statistics information or status and prepare
 * data to be sent to host as RNDIS_Query_CMPLT.
 *
 * @param handle The pointer the RNDIS device.
 * @param message The pointer to the pointer of the message buffer.
 * @param len The pointer of the length of the buffer.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisQueryCommand(usb_device_cdc_rndis_struct_t *handle, uint8_t **message, uint32_t *len)

{
    rndis_query_msg_struct_t *rndisQueryMsg;
    rndis_query_cmplt_struct_t *rndisQueryCmplt;
    uint32_t infoBufLen = 0;
    uint8_t *infoBuf    = NULL;
    usb_device_cdc_rndis_request_param_struct_t reqParam;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    rndisQueryMsg   = (rndis_query_msg_struct_t *)handle->rndisCommand;
    rndisQueryCmplt = (rndis_query_cmplt_struct_t *)handle->responseData;

    /* preparing for Byte 0-3 : MessageType*/
    rndisQueryCmplt->messageType = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_QUERY_CMPLT);

    /* preparing for Byte 8-11 : RequestID*/
    rndisQueryCmplt->requestID = rndisQueryMsg->requestID;

    /* preparing for Byte 12-15 : Status*/
    rndisQueryCmplt->status = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_STATUS_SUCCESS);

    /* preparing for Byte 20-23 : InformationBufferOffset*/
    rndisQueryCmplt->informationBufferOffset = USB_LONG_TO_LITTLE_ENDIAN(0x00000010);

    infoBuf = (uint8_t *)(&(rndisQueryCmplt->requestID)) + rndisQueryCmplt->informationBufferOffset;

    switch (rndisQueryMsg->oid)
    {
        case NDIS_OID_GEN_SUPPORTED_LIST:
        {
            uint32_t i;
            /* List of supported OIDs - Query Mandatory - General Operational Characteristic */
            for (i = 0; i < RNDIS_NUM_OIDS_SUPPORTED; i++)
            {
                /* change the endianness of data before sending on USB Bus */
                s_listSuppOid[i] = USB_LONG_TO_LITTLE_ENDIAN(s_listSuppOid[i]);
            }
            infoBufLen = sizeof(s_listSuppOid);
            memcpy(infoBuf, s_listSuppOid, infoBufLen);
        }
        break;
        case NDIS_OID_GEN_HARDWARE_STATUS:
            /* Hardware status  - Query Mandatory - General Operational Characteristic*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->rndisHwState);
            break;
        case NDIS_OID_GEN_MEDIA_SUPPORTED:
            /* Media types supported (encoded) - Query Mandatory - General Operational Characteristic*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(NDIS_MEDIUM802_3);
            break;
        case NDIS_OID_GEN_MEDIA_IN_USE:
            /* Media types in use (encoded) - Query Mandatory - General Operational Characteristic*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(NDIS_MEDIUM802_3);
            break;
        case NDIS_OID_GEN_MAXIMUM_FRAME_SIZE:
            /* Maximum in bytes, frame size - Query Mandatory - General Operational Characteristic*/
            infoBufLen      = sizeof(uint32_t);
            reqParam.buffer = infoBuf;
            reqParam.length = infoBufLen;
            if (handle->rndisCallback)
            {
                /* The rndisCallback is initialized in APPInit and is from the second parameter of
                   USB_DeviceCdcRndisInit */
                handle->rndisCallback(handle->cdcAcmHandle, kUSB_DeviceCdcEventAppGetMaxFrameSize, &reqParam);
            }

            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(*((uint32_t *)infoBuf));
            break;
        case NDIS_OID_GEN_LINK_SPEED:
            /* Link speed in units of 100 bps - Query Mandatory - General Operational Characteristic*/
            infoBufLen      = sizeof(uint32_t);
            reqParam.buffer = infoBuf;
            reqParam.length = infoBufLen;
            if (handle->rndisCallback)
            {
                /* The rndisCallback is initialized in APPInit and is from the second parameter of
                   USB_DeviceCdcRndisInit */
                handle->rndisCallback(handle->cdcAcmHandle, kUSB_DeviceCdcEventAppGetLinkSpeed, &reqParam);
            }

            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(*((uint32_t *)infoBuf));
            break;
        case NDIS_OID_GEN_TRANSMIT_BLOCK_SIZE:
            /* Minimum amount of storage, in bytes, that a single packet
               occupies in the transmit buffer space of the NIC -
               Query Mandatory - General Operational Characteristic*/
            infoBufLen      = sizeof(uint32_t);
            reqParam.buffer = infoBuf;
            reqParam.length = infoBufLen;
            if (handle->rndisCallback)
            {
                /* The rndisCallback is initialized in APPInit and is from the second parameter of
                   USB_DeviceCdcRndisInit */
                handle->rndisCallback(handle->cdcAcmHandle, kUSB_DeviceCdcEventAppGetSendPacketSize, &reqParam);
            }

            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(*((uint32_t *)infoBuf));
            break;
        case NDIS_OID_GEN_RECEIVE_BLOCK_SIZE:
            /* Amount of storage, in bytes, that a single packet occupies in
               the receive buffer space of the NIC - Query Mandatory - General Operational Characteristic*/
            infoBufLen      = sizeof(uint32_t);
            reqParam.buffer = infoBuf;
            reqParam.length = infoBufLen;
            if (handle->rndisCallback)
            {
                /* The rndisCallback is initialized in APPInit and is from the second parameter of
                   USB_DeviceCdcRndisInit */
                handle->rndisCallback(handle->cdcAcmHandle, kUSB_DeviceCdcEventAppGetRecvPacketSize, &reqParam);
            }
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(*((uint32_t *)infoBuf));
            break;
        case NDIS_OID_GEN_VENDOR_ID:
            /* Vendor NIC code - Query Mandatory - General Operational Characteristic*/
            /* This object specifies a three-byte IEEE-registered vendor code,
               followed by a single byte that the vendor assigns to identify
               a particular NIC. The IEEE code uniquely identifies the vendor
               and is the same as the three bytes appearing at the beginning
               of the NIC hardware address.Vendors without an IEEE-registered
               code should use the value 0xFFFFFF. */
            infoBufLen = sizeof(uint32_t);

            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(
                (uint32_t)(((uint32_t)RNDIS_VENDOR_ID << 8U) | (uint32_t)RNDIS_NIC_IDENTIFIER_VENDOR));
            break;
        case NDIS_OID_GEN_VENDOR_DESCRIPTION:
            /* Vendor network card description - Query Mandatory - General Operational Characteristic*/
            infoBufLen = VENDOR_INFO_SIZE;
            memcpy(infoBuf, s_vendorInfo, infoBufLen);
            break;
        case NDIS_OID_GEN_CURRENT_PACKET_FILTER:
            /* Current packet filter (encoded) - Query and Set Mandatory - General Operational Characteristic*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->rndisPacketFilter);
            break;
        case NDIS_OID_GEN_MAXIMUM_TOTAL_SIZE:
            /* Maximum total packet length in bytes - Query Mandatory - General Operational Characteristic*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->rndisDevMaxTxSize);
            break;
        case NDIS_OID_GEN_MEDIA_CONNECT_STATUS:
            /* Whether the NIC is connected to the network - Query Mandatory - General Operational Characteristic*/
            {
                uint32_t mediaConnected;
                /* Whether the NIC is connected to the network - Query Mandatory - General Operational Characteristic*/
                infoBufLen      = sizeof(uint32_t);
                reqParam.buffer = (uint8_t *)&mediaConnected;
                reqParam.length = infoBufLen;
                if (handle->rndisCallback)
                {
                    /* The rndisCallback is initialized in APPInit and is from the second parameter of
                       USB_DeviceCdcRndisInit */
                    handle->rndisCallback(handle->cdcAcmHandle, kUSB_DeviceCdcEventAppGetLinkSpeed, &reqParam);
                }

                if (mediaConnected)
                {
                    handle->rndisMediaConnectStatus = NDIS_MEDIA_STATE_CONNECTED;
                }
                else
                {
                    handle->rndisMediaConnectStatus = NDIS_MEDIA_STATE_DISCONNECTED;
                }
                *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->rndisMediaConnectStatus);
            }
            break;
        case NDIS_OID_GEN_XMIT_OK:
            /* Frames transmitted without errors - Query Mandatory - General Statistics*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->numFramesTxOk);
            break;
        case NDIS_OID_GEN_RCV_OK:
            /* Frames received without errors - Query Mandatory - General Statistics*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->numFramesRxOk);
            break;
        case NDIS_OID_GEN_XMIT_ERROR:
            /* Frames not transmitted or transmitted with errors - Query Mandatory - General Statistics*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->numFramesTxError);
            break;
        case NDIS_OID_GEN_RCV_ERROR:
            /* Frames received with errors - Query Mandatory - General Statistics*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->numFramesRxError);
            break;
        case NDIS_OID_GEN_RCV_NO_BUFFER:
            /* Frame missed, no buffers - Query Mandatory - General Statistics*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->numRecvFramesMissed);
            break;
        case NDIS_OID_802_3_PERMANENT_ADDRESS:
            /* Permanent station address - Query Mandatory - Ethernet Operational Characteristic*/
            infoBufLen      = RNDIS_ETHER_ADDR_SIZE;
            reqParam.buffer = infoBuf;
            reqParam.length = infoBufLen;
            if (handle->rndisCallback)
            {
                /* The rndisCallback is initialized in APPInit and is from the second parameter of
                   USB_DeviceCdcRndisInit */
                handle->rndisCallback(handle->cdcAcmHandle, kUSB_DeviceCdcEventAppGetMacAddress, &reqParam);
            }

            break;
        case NDIS_OID_802_3_CURRENT_ADDRESS:
        {
            /* Current station address - Query Mandatory - Ethernet Operational Characteristic*/
            infoBufLen      = RNDIS_ETHER_ADDR_SIZE;
            reqParam.buffer = infoBuf;
            reqParam.length = infoBufLen;
            if (handle->rndisCallback)
            {
                /* The rndisCallback is initialized in APPInit and is from the second parameter of
                   USB_DeviceCdcRndisInit */
                handle->rndisCallback(handle->cdcAcmHandle, kUSB_DeviceCdcEventAppGetMacAddress, &reqParam);
            }
        }
        break;
        case NDIS_OID_802_3_MULTICAST_LIST:
            /* Current multicast address list - Query and Set Mandatory - Ethernet Operational Characteristic*/
            infoBufLen = RNDIS_ETHER_ADDR_SIZE;
            /* Currently Our RNDIS driver does not support multicast addressing */
            *((uint32_t *)infoBuf)     = USB_LONG_TO_LITTLE_ENDIAN(0x00000000);
            *((uint32_t *)infoBuf + 1) = USB_LONG_TO_LITTLE_ENDIAN(0x00000000);
            break;
        case NDIS_OID_802_3_MAXIMUM_LIST_SIZE:
            /* Maximum size of multicast address list - Query Mandatory - Ethernet Operational Characteristic*/
            infoBufLen = sizeof(uint32_t);
            /* Currently Our RNDIS driver does not support multicast addressing */
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_MULTICAST_LIST_SIZE);
            break;
        case NDIS_OID_802_3_RCV_ERROR_ALIGNMENT:
            /* Frames received with alignment error - Query Mandatory - Ethernet Statistics*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->numRecvFramesAlignmentError);
            break;
        case NDIS_OID_802_3_XMIT_ONE_COLLISION:
            /* Frames transmitted with one collision - Query Mandatory - Ethernet Statistics*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->numFramesTxOneCollision);
            break;
        case NDIS_OID_802_3_XMIT_MORE_COLLISIONS:
            /* Frames transmitted with more than one collision - Query Mandatory - Ethernet Statistics*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(handle->numFramesTxManyCollision);
            break;
        case NDIS_OID_GEN_PHYSICAL_MEDIUM:
            /* Physical media supported by the miniport (encoded)*/
            infoBufLen             = sizeof(uint32_t);
            *((uint32_t *)infoBuf) = USB_LONG_TO_LITTLE_ENDIAN(NDIS_PHYSICAL_MEDIUM_POWER_LINE);
            break;
        default:
#ifdef DEBUG
            usb_echo("OID 0x%x NOT SUPPORTED(QUERY)\n", rndisQueryMsg->oid);
#endif
            rndisQueryCmplt->status = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_STATUS_NOT_SUPPORTED);
            break;
    }

    *len = RNDIS_RESPONSE_QUERY_MSG_SIZE + infoBufLen;
    /* preparing for Byte 4-7 : MessageLength*/
    rndisQueryCmplt->messageLength = USB_LONG_TO_LITTLE_ENDIAN(*len);
    /* preparing for Byte 16-19 : InformationBufferLength*/
    rndisQueryCmplt->informationBufferLength = USB_LONG_TO_LITTLE_ENDIAN(infoBufLen);
    *message                                 = (uint8_t *)&(rndisQueryCmplt->messageType);
    return kStatus_USB_Success;
}

/*!
 * @brief Configure the operational parameters of RNDIS device.
 *
 * This function is called to Configure the operational parameters of
 * RNDIS device.
 *
 * @param handle The pointer the RNDIS device.
 * @param message The pointer to the pointer of the message buffer.
 * @param len The pointer of the length of the buffer.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisSetCommand(usb_device_cdc_rndis_struct_t *handle, uint8_t **message, uint32_t *len)

{
    rndis_set_msg_struct_t *rndisSetMsg;
    rndis_set_cmplt_struct_t *rndisSetCmplt;
    usb_device_cdc_rndis_request_param_struct_t reqParam;
    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    rndisSetMsg   = (rndis_set_msg_struct_t *)handle->rndisCommand;
    rndisSetCmplt = (rndis_set_cmplt_struct_t *)handle->responseData;

    /* preparing for Byte 0-3 : MessageType*/
    rndisSetCmplt->messageType = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_SET_CMPLT);

    *len = RNDIS_RESPONSE_SET_MSG_SIZE;
    /* preparing for Byte 4-7 : MessageLength*/
    rndisSetCmplt->messageLength = USB_LONG_TO_LITTLE_ENDIAN(*len);

    /* preparing for Byte 8-11 : RequestID*/
    rndisSetCmplt->requestID = rndisSetMsg->requestID;

    switch (rndisSetMsg->oid)
    {
        case NDIS_OID_GEN_CURRENT_PACKET_FILTER:
            /* Current packet filter (encoded) - Query and Set Mandatory - General Operational Characteristic*/
            if (rndisSetMsg->informationBufferLength != 4)
            {
                rndisSetCmplt->status = RNDIS_STATUS_INVALID_DATA;
            }
            else
            {
                uint32_t mediaConnected = 0;

                handle->rndisPacketFilter = USB_LONG_TO_LITTLE_ENDIAN(
                    *((uint32_t *)((uint8_t *)(&rndisSetMsg->requestID) + rndisSetMsg->informationBufferOffset)));
                reqParam.buffer = (uint8_t *)&mediaConnected;
                reqParam.length = sizeof(mediaConnected);
                if (handle->rndisCallback)
                {
                    /* The rndisCallback is initialized in APPInit and is from the second parameter of
                       USB_DeviceCdcRndisInit */
                    handle->rndisCallback(handle->cdcAcmHandle, kUSB_DeviceCdcEventAppGetLinkStatus, &reqParam);
                }

                USB_CDC_RNDIS_MUTEX_LOCK(handle->statusMutex);
                if ((handle->rndisPacketFilter) && (mediaConnected == 1))
                {
                    handle->rndisDeviceState        = RNDIS_DATA_INITIALIZED;
                    handle->rndisMediaConnectStatus = NDIS_MEDIA_STATE_CONNECTED;
                }
                else
                {
                    handle->rndisDeviceState        = RNDIS_INITIALIZED;
                    handle->rndisMediaConnectStatus = NDIS_MEDIA_STATE_DISCONNECTED;
                }
                USB_CDC_RNDIS_MUTEX_UNLOCK(handle->statusMutex);
            }
            break;
        case NDIS_OID_802_3_MULTICAST_LIST:
            /* Current multicast address list - Query and Set Mandatory - Ethernet Operational Characteristic*/
            if (rndisSetMsg->informationBufferLength != RNDIS_ETHER_ADDR_SIZE)
            {
                rndisSetCmplt->status = RNDIS_STATUS_INVALID_DATA;
            }
            else
            {
                uint64_t multiCastList;
                multiCastList =
                    *((uint64_t *)((uint8_t *)(&rndisSetMsg->requestID) + rndisSetMsg->informationBufferOffset));
                if (multiCastList)
                {
                    /* Currently Our RNDIS driver does not support multicast addressing */
                    rndisSetCmplt->status = RNDIS_STATUS_NOT_SUPPORTED;
                }
            }

            break;
        default:
#ifdef DEBUG
            usb_echo("OID 0x%x NOT SUPPORTED(SET)\n", rndisSetMsg->oid);
#endif
            rndisSetCmplt->status = RNDIS_STATUS_NOT_SUPPORTED;
            break;
    }
    /* preparing for Byte 12-15 : Status*/
    rndisSetCmplt->status = USB_LONG_TO_LITTLE_ENDIAN(rndisSetCmplt->status);
    *message              = (uint8_t *)&(rndisSetCmplt->messageType);
    return kStatus_USB_Success;
}

/*!
 * @brief Soft reset the RNDIS device.
 *
 * This function is called to soft reset the RNDIS device.
 *
 * @param handle The pointer the RNDIS device.
 * @param message The pointer to the pointer of the message buffer.
 * @param len The pointer of the length of the buffer.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisResetCommand(usb_device_cdc_rndis_struct_t *handle, uint8_t **message, uint32_t *len)

{
    rndis_reset_cmplt_struct_t *rndisResetCmplt;
    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    rndisResetCmplt = (rndis_reset_cmplt_struct_t *)handle->responseData;

    /* preparing for Byte 0-3 : MessageType*/
    rndisResetCmplt->messageType = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_RESET_CMPLT);

    *len = RNDIS_RESPONSE_RESET_MSG_SIZE;
    /* preparing for Byte 4-7 : MessageLength*/
    rndisResetCmplt->messageLength = USB_LONG_TO_LITTLE_ENDIAN(*len);

    /* preparing for Byte 8-11 : Status*/
    rndisResetCmplt->status = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_STATUS_SUCCESS);

    /* preparing for Byte 12-15 : AddressingReset*/
    /* No need for host to resend addressing information */
    rndisResetCmplt->addressingReset = USB_LONG_TO_LITTLE_ENDIAN(0x00000000);

    *message = (uint8_t *)&(rndisResetCmplt->messageType);
    USB_CDC_RNDIS_MUTEX_LOCK(handle->statusMutex);
    handle->rndisHwState            = NDIS_HARDWARE_STATUS_RESET;
    handle->rndisDeviceState        = RNDIS_UNINITIALIZED;
    handle->rndisMediaConnectStatus = NDIS_MEDIA_STATE_UNKNOWN;
    USB_CDC_RNDIS_MUTEX_UNLOCK(handle->statusMutex);
    return kStatus_USB_Success;
}

/*!
 * @brief Indicate change in status of device.
 *
 * This function is called to indicate change in status of device.
 *
 * @param handle The pointer the RNDIS device.
 * @param message The pointer to the pointer of the message buffer.
 * @param len The pointer of the length of the buffer.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisIndicateStatusCommand(usb_device_cdc_rndis_struct_t *handle,
                                                     uint8_t **message,
                                                     uint32_t *len)

{
    return kStatus_USB_Success;
}

/*!
 * @brief Halt the RNDIS device.
 *
 * This function is called to halt the RNDIS device.
 * i.e. to terminate the network connection.
 *
 * @param handle The pointer the RNDIS device.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisHaltCommand(usb_device_cdc_rndis_struct_t *handle)
{
    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }

    USB_CDC_RNDIS_MUTEX_LOCK(handle->statusMutex);
    handle->rndisDeviceState        = RNDIS_UNINITIALIZED;
    handle->rndisMediaConnectStatus = NDIS_MEDIA_STATE_DISCONNECTED;
    handle->rndisHwState            = NDIS_HARDWARE_STATUS_NOT_READY;
    USB_CDC_RNDIS_MUTEX_UNLOCK(handle->statusMutex);
    return kStatus_USB_Success;
}
#endif /* USB_DEVICE_CONFIG_CDC_RNDIS */
