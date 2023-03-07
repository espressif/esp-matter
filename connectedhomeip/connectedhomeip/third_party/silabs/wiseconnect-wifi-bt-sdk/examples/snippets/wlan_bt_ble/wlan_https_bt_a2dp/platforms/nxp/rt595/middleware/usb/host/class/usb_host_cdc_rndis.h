/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_HOST_CDC_RNDIS_H__
#define __USB_HOST_CDC_RNDIS_H__

/*!
 * @addtogroup usb_host_cdc_rnds_drv
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* general OIDs  */
#define OID_GEN_SUPPORTED_LIST            (0x00010101U)
#define OID_GEN_HARDWARE_STATUS           (0x00010102U)
#define OID_GEN_MEDIA_SUPPORTED           (0x00010103U)
#define OID_GEN_MEDIA_IN_USE              (0x00010104U)
#define OID_GEN_MAXIMUM_LOOKAHEAD         (0x00010105U)
#define OID_GEN_MAXIMUM_FRAME_SIZE        (0x00010106U)
#define OID_GEN_LINK_SPEED                (0x00010107U)
#define OID_GEN_TRANSMIT_BUFFER_SPACE     (0x00010108U)
#define OID_GEN_RECEIVE_BUFFER_SPACE      (0x00010109U)
#define OID_GEN_TRANSMIT_BLOCK_SIZE       (0x0001010AU)
#define OID_GEN_RECEIVE_BLOCK_SIZE        (0x0001010BU)
#define OID_GEN_VENDOR_ID                 (0x0001010CU)
#define OID_GEN_VENDOR_DESCRIPTION        (0x0001010DU)
#define OID_GEN_CURRENT_PACKET_FILTER     (0x0001010EU)
#define OID_GEN_CURRENT_LOOKAHEAD         (0x0001010FU)
#define OID_GEN_DRIVER_VERSION            (0x00010110U)
#define OID_GEN_MAXIMUM_TOTAL_SIZE        (0x00010111U)
#define OID_GEN_PROTOCOL_OPTIONS          (0x00010112U)
#define OID_GEN_MAC_OPTIONS               (0x00010113U)
#define OID_GEN_MEDIA_CONNECT_STATUS      (0x00010114U)
#define OID_GEN_MAXIMUM_SEND_PACKETS      (0x00010115U)
#define OID_GEN_VENDOR_DRIVER_VERSION     (0x00010116U)
#define OID_GEN_SUPPORTED_GUIDS           (0x00010117U)
#define OID_GEN_NETWORK_LAYER_ADDRESSES   (0x00010118U)
#define OID_GEN_TRANSPORT_HEADER_OFFSET   (0x00010119U)
#define OID_GEN_MACHINE_NAME              (0x0001021AU)
#define OID_GEN_RNDIS_CONFIG_PARAMETER    (0x0001021BU)
#define OID_GEN_VLAN_ID                   (0x0001021C
/*general statistic OIdS */
#define OID_GEN_XMIT_OK                  (0x00020101U)
#define OID_GEN_RCV_OK                      (0x00020102U)
#define OID_GEN_XMIT_ERROR               (0x00020103U)
#define OID_GEN_RCV_ERROR                (0x00020104U)
#define OID_GEN_RCV_NO_BUFFER            (0x00020105U)

/* 802.3 OIDs(Ethernet)  */
#define OID_802_3_PERMANENT_ADDRESS       (0x01010101)
#define OID_802_3_CURRENT_ADDRESS         (0x01010102)
#define OID_802_3_MULTICAST_LIST          (0x01010103)
#define OID_802_3_MAXIMUM_LIST_SIZE       (0x01010104)
#define OID_802_3_MAC_OPTIONS             (0x01010105)

#define OID_802_3_RCV_ERROR_ALIGNMENT     (0x01020101)
#define OID_802_3_XMIT_ONE_COLLISION      (0x01020102)
#define OID_802_3_XMIT_MORE_COLLISIONS    (0x01020103)

#define OID_802_3_XMIT_DEFERRED           (0x01020201)
#define OID_802_3_XMIT_MAX_COLLISIONS     (0x01020202)
#define OID_802_3_RCV_OVERRUN             (0x01020203)
#define OID_802_3_XMIT_UNDERRUN           (0x01020204)
#define OID_802_3_XMIT_HEARTBEAT_FAILURE  (0x01020205)
#define OID_802_3_XMIT_TIMES_CRS_LOST     (0x01020206)
#define OID_802_3_XMIT_LATE_COLLISIONS    (0x01020207)


/*RNDIS message types*/
#define REMOTE_NDIS_PACKET_MSG            (0x00000001U)
#define REMOTE_NDIS_INITIALIZE_MSG        (0x00000002U)
#define REMOTE_NDIS_INITIALIZE_CMPLT      (0x80000002U)
#define REMOTE_NDIS_HALT_MSG                (0x00000003U)
#define REMOTE_NDIS_QUERY_MSG                (0x00000004U)
#define REMOTE_NDIS_QUERY_CMPLT            (0x80000004U)
#define REMOTE_NDIS_SET_MSG                (0x00000005U)
#define REMOTE_NDIS_SET_CMPLT                (0x80000005U)
#define REMOTE_NDIS_RESET_MSG                (0x00000006U)
#define REMOTE_NDIS_RESET_CMPLT            (0x80000006U)
#define REMOTE_NDIS_INDICATE_STATUS_MSG   (0x00000007U)
#define REMOTE_NDIS_KEEPALIVE_MSG         (0x00000008U)
#define REMOTE_NDIS_KEEPALIVE_CMPLT         (0x80000008U)

/*Status Values */
/*! Success.*/
#define RNDIS_STATUS_SUCCESS (0x00000000U)
/*! Unspecified error*/
#define RNDIS_STATUS_FAILURE (0xC0000001U)
/*! Invalid data error. */
#define RNDIS_STATUS_INVALID_DATA (0xC0010015U)
/*! Unsupported request error */
#define RNDIS_STATUS_NOT_SUPPORTED (0xC00000BBU)
/*! Device is connected to a network medium */
#define RNDIS_STATUS_MEDIA_CONNECT (0x4001000BU)
/*! Device is disconnected from the medium */
#define RNDIS_STATUS_MEDIA_DISCONNECT (0x4001000CU)

#define NDIS_PACKET_TYPE_DIRECTED (0x0001U)
/*! Multicast address packets sent to addresses in the multicast address list.
 * A protocol driver can receive Ethernet (802.3) multicast packets or
 * Token Ring (802.5) functional address packets by specifying the multicast or
 * functional address packet type. Setting the multicast address list or
 * functional address determines which multicast address
 * groups the NIC driver enables.
 */
#define NDIS_PACKET_TYPE_MULTICAST (0x0002U)
/*! All multicast address packets, not just the ones enumerated in the
 * multicast address list.
 */
#define NDIS_PACKET_TYPE_ALL_MULTICAST (0x0004U)
/*! Broadcast packets. */
#define NDIS_PACKET_TYPE_BROADCAST (0x0008U)
/*! All source routing packets. If the protocol driver sets this bit,
 * the NDIS library attempts to act as a source routing bridge.
 */
#define NDIS_PACKET_TYPE_SOURCE_ROUTING (0x0010U)
/*! Specifies all packets.*/
#define NDIS_PACKET_TYPE_PROMISCUOUS (0x0020U)
/*! SMT packets that an FDDI NIC receives. */
#define NDIS_PACKET_TYPE_SMT (0x0040U)




/*! Size of INITIALIZE_MSG. */
#define RNDIS_INITIALIZE_MSG_SIZE (24U)
/*! RSize of QUERY_MSG. */
#define RNDIS_QUERY_MSG_SIZE (28U)
/*! Size of SET_MSG. */
#define RNDIS_SET_MSG_SIZE (28U)
/*! Size of RESET_MSG. */
#define RNDIS_RESET_MSG_SIZE (12U)
/*! Size of KEEPALIVE_MSG. */
#define RNDIS_KEEPALIVE_MSG_SIZE (12U)
/*! Size of DATA_MSG_HEADER. */
#define RNDIS_DAT_MSG_HEADER_SIZE (44U)
   
/*! RNDIS Version */
#define RNDIS_MAJOR_VERSION  (0x5U)
/*! RNDIS Version */
#define RNDIS_MINOR_VERSION  (0x0U)
#define RNDIS_MAX_TRANSFER_PACKET_SIZE  (0x4000U)
#define RNDIS_FRAME_MAX_FRAMELEN (1518U) /*!< Default maximum Ethernet frame size. */

/*! @brief CDC class-specific code, Base Class E0h (Wireless Controller) */
#define USB_HOST_CDC_RNDIS_CLASS_CODE 0xE0U
/*! @brief RF Controller*/
#define USB_HOST_CDC_RNDIS_SUBCLASS_CODE 0x01U
/*Remote NDIS.  Information can be found at: http://www.microsoft.com/windowsmobile/mobileoperators/default.mspx*/
#define USB_HOST_CDC_RNDIS_PROTOCOL_CODE 0x03U

/*this is the minimum byte length of the buffer posted by host*/


/*******************************************************************************
 * RNDS class public structure, enumeration, macro, function
 ******************************************************************************/
/*! @brief Define message structure for REMOTE_NDIS_INITIALIZE_MSG. */
typedef struct _rndis_init_msg_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t requestID;
    uint32_t majorVersion;
    uint32_t minorVersion;
    uint32_t maxTransferSize;
} rndis_init_msg_struct_t;
/*! @brief Define message structure for REMOTE_NDIS_HALT_MSG. */
typedef struct _rndis_halt_msg_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t requestID;
} rndis_halt_msg_struct_t;
/*! @brief Define message structure for REMOTE_NDIS_QUERY_MSG. */
typedef struct _rndis_query_msg_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t requestID;
    uint32_t oid;
    uint32_t informationBufferLength;
    uint32_t informationBufferOffset;
    uint32_t deviceVcHandle;
} rndis_query_msg_struct_t;
/*! @brief Define message structure for REMOTE_NDIS_SET_MSG. */
typedef struct _rndis_set_msg_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t requestID;
    uint32_t oid;
    uint32_t informationBufferLength;
    uint32_t informationBufferOffset;
    uint32_t deviceVcHandle;
} rndis_set_msg_struct_t;
/*! @brief Define message structure for REMOTE_NDIS_RESET_MSG. */
typedef struct _rndis_reset_msg_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t Reserved;
} rndis_reset_msg_struct_t;
/*! @brief Define message structure for REMOTE_NDIS_INDICATE_STATUS_MSG. */
typedef struct _rndis_indicate_status_msg_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t status;
    uint32_t statusBufferLength;
    uint32_t statusBufferOffset;
} rndis_indicate_status_msg_struct_t;
/*! @brief Define message structure for REMOTE_NDIS_DIAGNOSTIC_MSG. */
typedef struct _rndis_diagnostic_msg_struct
{
    uint32_t DiagStatus;
    uint32_t ErrorOffset;
} rndis_diagnostic_msg_struct_t;
/*! @brief Define message structure for REMOTE_NDIS_KEEPALIVE_MSG. */
typedef struct _rndis_keepalive_msg_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t requestID;
} rndis_keepalive_msg_struct_t;
/*! @brief Define message structure for REMOTE_NDIS_INITIALIZE_CMPLT. */
typedef struct _rndis_init_cmplt_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t requestID;
    uint32_t status;
    uint32_t majorVersion;
    uint32_t minorVersion;
    uint32_t deviceFlags;
    uint32_t medium;
    uint32_t maxPacketsPerTransfer;
    uint32_t maxTransferSize;
    uint32_t packetAlignmentFactor;
    uint32_t afListOffset;
    uint32_t afListSize;
} rndis_init_cmplt_struct_t;
/*! @brief Define message structure for REMOTE_NDIS_QUERY_CMPLT. */
typedef struct _rndis_query_cmplt_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t requestID;
    uint32_t status;
    uint32_t informationBufferLength;
    uint32_t informationBufferOffset;
} rndis_query_cmplt_struct_t;
/*! @brief Define message structure for REMOTE_NDIS_SET_CMPLT. */
typedef struct _rndis_set_cmplt_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t requestID;
    uint32_t status;
} rndis_set_cmplt_struct_t;
/*! @brief Define message structure for REMOTE_NDIS_RESET_CMPLT. */
typedef struct _rndis_reset_cmplt_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t status;
    uint32_t addressingReset;
} rndis_reset_cmplt_struct_t;


/*! @brief Define message structure for REMOTE_NDIS_KEEPALIVE_CMPLT. */
typedef struct _rndis_keepalive_cmplt_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t requestID;
    uint32_t status;
} rndis_keepalive_cmplt_struct_t;
/*! @brief Define message structure for RNDIS_PACKET_MSG. */
typedef struct _rndis_packet_msg_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t dataOffset;
    uint32_t dataLength;
    uint32_t oobDataOffset;
    uint32_t oobDataLength;
    uint32_t numOOBDataElements;
    uint32_t perPacketInfoOffset;
    uint32_t perPacketInfoLength;
    uint32_t vcHandle;
    uint32_t reserved;
    uint8_t  dataBuffer[RNDIS_FRAME_MAX_FRAMELEN];
} rndis_packet_msg_struct_t;
/*! @brief Define RNDIS state. See MSDN for details. */
typedef enum _rndis_state_enum
{
    /*! The host operating system is running, the device is connected to the host via the chosen 
    *bus transport, and the host is not yet configured to exchange any RNDIS messages.
    */
    RNDIS_UNINITIALIZED = 0,
    /*! The host has successfully mapped and initialized the RNDIS protocol data and control channels
    *on the bus transport
    */
    RNDIS_BUS_UNINITIALIZED,
    /*!The host is configured to send and receive any of the RNDIS control messages for suitably 
    *configuring or querying the device, to receive status indications from the device, to reset
    *the device, or to tear down the data and control channels.
    */
    RNDIS_INITIALIZED,
    /*! This state is entered after the host has received REMOTE_NDIS_SET_CMPLT messages from the
    *device in response to the REMOTE_NDIS_SET_MSG.
    */
    RNDIS_DATA_INITIALIZED,
} rndis_state_enum_t;
typedef struct _usb_host_rndis
{
    usb_host_handle hostHandle;                /*!< This instance's related host handle.*/
    usb_device_handle deviceHandle;            /*!< This instance's related device handle.*/
    uint32_t effective_version;                   /*!< The highest matching MajorVersion and MinorVersion values of the RNDIS protocol.*/ 
    uint32_t max_transfer_size;                   /*!< The maximum size of the RNDIS message that can be exchanged between the host and the device.*/
    uint32_t max_packet_per_transaction;       /*!< The maximum number of network packets per bus transaction.*/
    uint32_t byte_alignmnet;                   /*!< The byte alignment requirements in multi-packet transfers.*/
}usb_host_rndis_struct;
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @name USB CDC host class driver
 * @{
 */
extern usb_status_t USB_HostRndisInitMsg(usb_host_class_handle classHandle,
                               uint8_t *messageBuffer,
                               uint32_t messageBufferLength,
                               transfer_callback_t callbackFn,
                               void *callbackParam);


extern usb_status_t USB_HostRndisQueryMsg(usb_host_class_handle classHandle,    
                               uint32_t Oid,
                               uint8_t *messageBuffer,
                               uint32_t messageBufferLength,
                               uint32_t informationOffset,
                               uint32_t informationLength,
                                                           uint8_t* OIDInputBuffer,
                               transfer_callback_t callbackFn,
                               void *callbackParam);
extern usb_status_t USB_HostRndisSetMsg(usb_host_class_handle classHandle,    
                               uint32_t Oid,
                               uint8_t *messageBuffer,
                               uint32_t messageBufferLength,
                               uint32_t informationOffset,
                               uint32_t informationLength,
                                                           uint32_t* OIDInputBuffer,
                               transfer_callback_t callbackFn,
                               void *callbackParam);

extern usb_status_t USB_HostRndisHaltMsg(usb_host_class_handle classHandle,    
                               uint8_t *messageBuffer,
                               uint32_t messageBufferLength,
                               transfer_callback_t callbackFn,
                               void *callbackParam);

extern usb_status_t USB_HostRndisResetMsg(usb_host_class_handle classHandle,    
                               uint8_t *messageBuffer,
                               uint32_t messageBufferLength,
                               transfer_callback_t callbackFn,
                               void *callbackParam);
extern usb_status_t USB_HostRndisSendDataMsg(usb_host_class_handle classHandle,    
                               uint8_t *messageBuffer,
                               uint32_t messageBufferLength,
                               uint32_t oobDataOffset,
                               uint32_t oobDataLength,
                               uint32_t numOOBDataElements,
                               uint32_t perPacketInfoOffset,
                               uint32_t perPacketInfoLength,
                               uint8_t * dataBuffer,
                               uint32_t dataLength,
                               transfer_callback_t callbackFn,
                               void *callbackParam);
extern usb_status_t USB_HostRndisRecvDataMsg(usb_host_class_handle classHandle,    
                               uint8_t *buffer,
                               uint32_t bufferLength,
                               transfer_callback_t callbackFn,
                               void *callbackParam);
/*@}*/

#ifdef __cplusplus
}
#endif
/*@}*/

#endif /*__USB_HOST_CDC_H__*/
