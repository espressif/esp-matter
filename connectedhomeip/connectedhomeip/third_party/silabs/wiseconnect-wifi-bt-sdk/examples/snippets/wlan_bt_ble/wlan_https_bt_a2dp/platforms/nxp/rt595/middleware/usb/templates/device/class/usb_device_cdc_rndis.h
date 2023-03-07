/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _USB_DEVICE_CDC_RNDIS_H_
#define _USB_DEVICE_CDC_RNDIS_H_

/*!
 * @addtogroup cdc_rndis
 * @{
 */

/*******************************************************************************
* Definitions
******************************************************************************/
#define USB_DEVICE_CONFIG_CDC_RNDIS_MAX_INSTANCE (1U) /*!< The maximum number of USB CDC RNDIS device instance*/
                                                      /*!
                                                       * @name RNDIS Control Message Type
                                                       * See MSDN for details.
                                                       * @{
                                                       */
#define RNDIS_PACKET_MSG (0x00000001U)
#define RNDIS_INITIALIZE_MSG (0x00000002U)
#define RNDIS_HALT_MSG (0x00000003U)
#define RNDIS_QUERY_MSG (0x00000004U)
#define RNDIS_SET_MSG (0x00000005U)
#define RNDIS_RESET_MSG (0x00000006U)
#define RNDIS_INDICATE_STATUS_MSG (0x00000007U)
#define RNDIS_KEEPALIVE_MSG (0x00000008U)
#define RNDIS_INITIALIZE_CMPLT (0x80000002U)
#define RNDIS_QUERY_CMPLT (0x80000004U)
#define RNDIS_SET_CMPLT (0x80000005U)
#define RNDIS_RESET_CMPLT (0x80000006U)
#define RNDIS_KEEPALIVE_CMPLT (0x80000008U)
/* @} */

/*!
 * @name Object Identifiers used by NdisRequest Query/Set Information
 * See MSDN for details.
 * @{
 */
/* General Objects */
#define NDIS_OID_GEN_SUPPORTED_LIST (0x00010101U)
#define NDIS_OID_GEN_HARDWARE_STATUS (0x00010102U)
#define NDIS_OID_GEN_MEDIA_SUPPORTED (0x00010103U)
#define NDIS_OID_GEN_MEDIA_IN_USE (0x00010104U)
#define NDIS_OID_GEN_MAXIMUM_LOOKAHEAD (0x00010105U)
#define NDIS_OID_GEN_MAXIMUM_FRAME_SIZE (0x00010106U)
#define NDIS_OID_GEN_LINK_SPEED (0x00010107U)
#define NDIS_OID_GEN_TRANSMIT_BUFFER_SPACE (0x00010108U)
#define NDIS_OID_GEN_RECEIVE_BUFFER_SPACE (0x00010109U)
#define NDIS_OID_GEN_TRANSMIT_BLOCK_SIZE (0x0001010AU)
#define NDIS_OID_GEN_RECEIVE_BLOCK_SIZE (0x0001010BU)
#define NDIS_OID_GEN_VENDOR_ID (0x0001010CU)
#define NDIS_OID_GEN_VENDOR_DESCRIPTION (0x0001010DU)
#define NDIS_OID_GEN_CURRENT_PACKET_FILTER (0x0001010EU)
#define NDIS_OID_GEN_CURRENT_LOOKAHEAD (0x0001010FU)
#define NDIS_OID_GEN_DRIVER_VERSION (0x00010110U)
#define NDIS_OID_GEN_MAXIMUM_TOTAL_SIZE (0x00010111U)
#define NDIS_OID_GEN_PROTOCOL_OPTIONS (0x00010112U)
#define NDIS_OID_GEN_MAC_OPTIONS (0x00010113U)
#define NDIS_OID_GEN_MEDIA_CONNECT_STATUS (0x00010114U)
#define NDIS_OID_GEN_MAXIMUM_SEND_PACKETS (0x00010115U)
#define NDIS_OID_GEN_XMIT_OK (0x00020101U)
#define NDIS_OID_GEN_RCV_OK (0x00020102U)
#define NDIS_OID_GEN_XMIT_ERROR (0x00020103U)
#define NDIS_OID_GEN_RCV_ERROR (0x00020104U)
#define NDIS_OID_GEN_RCV_NO_BUFFER (0x00020105U)
#define NDIS_OID_GEN_DIRECTED_BYTES_XMIT (0x00020201U)
#define NDIS_OID_GEN_DIRECTED_FRAMES_XMIT (0x00020202U)
#define NDIS_OID_GEN_MULTICAST_BYTES_XMIT (0x00020203U)
#define NDIS_OID_GEN_MULTICAST_FRAMES_XMIT (0x00020204U)
#define NDIS_OID_GEN_BROADCAST_BYTES_XMIT (0x00020205U)
#define NDIS_OID_GEN_BROADCAST_FRAMES_XMIT (0x00020206U)
#define NDIS_OID_GEN_DIRECTED_BYTES_RCV (0x00020207U)
#define NDIS_OID_GEN_DIRECTED_FRAMES_RCV (0x00020208U)
#define NDIS_OID_GEN_MULTICAST_BYTES_RCV (0x00020209U)
#define NDIS_OID_GEN_MULTICAST_FRAMES_RCV (0x0002020AU)
#define NDIS_OID_GEN_BROADCAST_BYTES_RCV (0x0002020BU)
#define NDIS_OID_GEN_BROADCAST_FRAMES_RCV (0x0002020CU)
#define NDIS_OID_GEN_RCV_CRC_ERROR (0x0002020DU)
#define NDIS_OID_GEN_TRANSMIT_QUEUE_LENGTH (0x0002020EU)
#define NDIS_OID_GEN_GET_TIME_CAPS (0x0002020FU)
#define NDIS_OID_GEN_GET_NETCARD_TIME (0x00020210U)
/* 802.3 Objects (Ethernet) */
#define NDIS_OID_802_3_PERMANENT_ADDRESS (0x01010101U)
#define NDIS_OID_802_3_CURRENT_ADDRESS (0x01010102U)
#define NDIS_OID_802_3_MULTICAST_LIST (0x01010103U)
#define NDIS_OID_802_3_MAXIMUM_LIST_SIZE (0x01010104U)
#define NDIS_OID_802_3_MAC_OPTIONS (0x01010105U)

#define NDIS_802_3_MAC_OPTION_PRIORITY (0x00000001U)
#define NDIS_OID_802_3_RCV_ERROR_ALIGNMENT (0x01020101U)
#define NDIS_OID_802_3_XMIT_ONE_COLLISION (0x01020102U)
#define NDIS_OID_802_3_XMIT_MORE_COLLISIONS (0x01020103U)
#define NDIS_OID_802_3_XMIT_DEFERRED (0x01020201U)
#define NDIS_OID_802_3_XMIT_MAX_COLLISIONS (0x01020202U)
#define NDIS_OID_802_3_RCV_OVERRUN (0x01020203U)
#define NDIS_OID_802_3_XMIT_UNDERRUN (0x01020204U)
#define NDIS_OID_802_3_XMIT_HEARTBEAT_FAILURE (0x01020205U)
#define NDIS_OID_802_3_XMIT_TIMES_CRS_LOST (0x01020206U)
#define NDIS_OID_802_3_XMIT_LATE_COLLISIONS (0x01020207U)

/* Optional OIDs */
#define NDIS_OID_GEN_VENDOR_DRIVER_VERSION (0x00010116U)
#define NDIS_OID_GEN_SUPPORTED_GUIDS (0x00010117U)
#define NDIS_OID_GEN_NETWORK_LAYER_ADDRESSES (0x00010118U) /* Set only */
#define NDIS_OID_GEN_TRANSPORT_HEADER_OFFSET (0x00010119U) /* Set only */
#define NDIS_OID_GEN_MACHINE_NAME (0x0001021AU)
#define NDIS_OID_GEN_RNDIS_CONFIG_PARAMETER (0x0001021BU) /* Set only */
#define NDIS_OID_GEN_VLAN_ID (0x0001021CU)
#define NDIS_OID_GEN_MEDIA_CAPABILITIES (0x00010201U)
#define NDIS_OID_GEN_PHYSICAL_MEDIUM (0x00010202U)
/* @} */

/*!
 * @name NDIS Hardware status codes for OID_GEN_HARDWARE_STATUS
 * See MSDN for details.
 * @{
 */
/*! Available and capable of sending and receiving data over the wire */
#define NDIS_HARDWARE_STATUS_READY (0x00000000U)
/*! Initializing */
#define NDIS_HARDWARE_STATUS_INITIALIZING (0x00000001U)
/*! Resetting */
#define NDIS_HARDWARE_STATUS_RESET (0x00000002U)
/*! Closing */
#define NDIS_HARDWARE_STATUS_CLOSING (0x00000003U)
/*! Not ready */
#define NDIS_HARDWARE_STATUS_NOT_READY (0x00000004U)
/* @} */

/*!
 * @name NDIS media types that the NIC can support
 * See MSDN for details.
 * @{
 */
/*! Ethernet (802.3) is not supported for NDIS 6.0 drivers.
 * Note  NDIS 5.x Miniport drivers that conform to the IEEE<sup>®</sup> 802.11 interface must
 * use this media type. For more information about the 802.11 interface,
 * see 802.11 Wireless LAN Miniport Drivers.
 */
#define NDIS_MEDIUM802_3 (0x00000000U)
/*! Token Ring (802.5) is not supported for NDIS 6.0 drivers.*/
#define NDIS_MEDIUM802_5 (0x00000001U)
/*! FDDI is not supported on Windows<sup>®</sup> Vista.*/
#define NDIS_MEDIUM_FDDI (0x00000002U)
/*! WAN*/
#define NDIS_MEDIUM_WAN (0x00000003U)
/*! LocalTalk*/
#define NDIS_MEDIUM_LOCAL_TALK (0x00000004U)
/*! DEC/Intel/Xerox (DIX) Ethernet*/
#define NDIS_MEDIUM_DIX (0x00000005U)
/*! ARCNET (raw) is not supported on Windows Vista.*/
#define NDIS_MEDIUM_ARCNET_RAW (0x00000006U)
/*! ARCNET (878.2) is not supported on Windows Vista.*/
#define NDIS_MEDIUM_ARCNET878_2 (0x00000007U)
/*! ATM is not supported for NDIS 6.0 drivers.*/
#define NDIS_MEDIUM_ATM (0x00000008U)
/*! Native 802.11. This media type is used by Miniport drivers that conform to
 * the Native 802.11 interface. For more information about this interface, see
 * Native 802.11 Wireless LAN Miniport Drivers.
 * Note: Native 802.11 interface is supported in NDIS 6.0 and later versions
 */
#define NDIS_MEDIUM_NATIVE802_11 (0x00000009U)
/*! Various types of NdisWirelessXxx media
  * Note  This media type is not available for use beginning with Windows Vista.
  */
#define NDIS_MEDIUM_WIRELESS_WAN (0x0000000AU)
/*! Infrared (IrDA)*/
#define NDIS_MEDIUM_IRDA (0x0000000BU)
/*! Connection-oriented WAN */
#define NDIS_MEDIUM_COWAN (0x0000000CU)
/*! IEEE 1394 (firewire) bus */
#define NDIS_MEDIUM1394 (0x0000000DU)
/*! Broadcast PC network.*/
#define NDIS_MEDIUM_BPC (0x0000000EU)
/*! InfiniBand network.*/
#define NDIS_MEDIUM_INFINI_BAND (0x0000000FU)
/*! Tunnel network.*/
#define NDIS_MEDIUM_TUNNEL (0x00000010U)
/*! NDIS loopback network. */
#define NDIS_MEDIUM_LOOPBACK (0x00000011U)
/* @} */

/*!
 * @name NDIS Packet Filter Bits for OID_GEN_CURRENT_PACKET_FILTER.
 * See MSDN for details.
 * @{
 */
/*! Directed packets. Directed packets contain a destination address
 * equal to the station address of the NIC.
 */
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
/*! All packets sent by installed protocols and all packets indicated by the NIC
 * that is identified by a given NdisBindingHandle.
 */
#define NDIS_PACKET_TYPE_ALL_LOCAL (0x0080U)
/*! NIC driver frames that a Token Ring NIC receives. */
#define NDIS_PACKET_TYPE_MAC_FRAME (0x8000U)
/*! Functional address packets sent to addresses included in the current
 * functional address.
 */
#define NDIS_PACKET_TYPE_FUNCTIONAL (0x4000U)
/*! All functional address packets, not just the ones in the
 * current functional address.
 */
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL (0x2000U)
/*! Packets sent to the current group address. */
#define NDIS_PACKET_TYPE_GROUP (0x1000U)
/* @} */

/*!
 * @name RNDIS status values
 * See MSDN for details.
 * @{
 */
/*! The requested operation completed successfully.*/
#define RNDIS_STATUS_SUCCESS (0x00000000U)
/*! The underlying driver does not support the requested operation.*/
#define RNDIS_STATUS_NOT_RECOGNIZED (0x00010001U)
/*! Unsupported request error (equivalent to STATUS_NOT_SUPPORTED). */
#define RNDIS_STATUS_NOT_SUPPORTED (0xC00000BBU)
/*! The underlying driver attempted the requested operation, usually a set,
 * on its NIC but it was aborted by the Netcard. For example, an attempt to set
 * too many multicast addresses might cause the return of this value.
 */
#define RNDIS_STATUS_NOT_ACCEPTED (0x00010003U)
/*! This value usually is a non specific default, returned when none of the more
 * specific NDIS_STATUS_XXX causes the underlying driver to fail the request.
 */
#define RNDIS_STATUS_FAILURE (0xC0000001U)
/*! The request can't be satisfied due to a resource shortage.
 * Usually, this return indicates that an attempt to allocate memory was
 * unsuccessful, but it does not necessarily indicate that the same request,
 * submitted later, it is aborted for the same reason.
 */
#define RNDIS_STATUS_RESOURCES (0xC000009AU)
/*! The underlying driver failed the requested operation because a close is in progress.*/
#define RNDIS_STATUS_CLOSING (0xC0010002U)
/*! The underlying driver failed the requested operation because indicating a close is in progress.*/
#define RNDIS_STATUS_CLOSING_INDICATING (0xC001000EU)
/*! The underlying NIC driver cannot satisfy the request at this time because it
 * is currently resetting the Netcard.
 */
#define RNDIS_STATUS_RESET_IN_PROGRESS (0xC001000DU)
/*! The value specified in the InformationBufferLength member of the
 * NDIS_REQUEST-structured buffer at NdisRequest does not match the
 * requirements for the given OID_XXX code. If the information buffer
 * is too small, the BytesNeeded member contains the correct value for
 * InformationBufferLength on return from NdisRequest.
 */
#define RNDIS_STATUS_INVALID_LENGTH (0xC0010014U)
/*! The information buffer is too small. */
#define RNDIS_STATUS_BUFFER_TOO_SHORT (0xC0010016U)
/*! The data supplied at InformationBuffer in the given NDIS_REQUEST structure
 * is invalid for the given OID_XXX code.
 */
#define RNDIS_STATUS_INVALID_DATA (0xC0010015U)
/*! The OID_XXX code specified in the OID member of the NDIS_REQUEST-structured
 * buffer at NdisRequest is invalid or unsupported by the underlying driver.
 */
#define RNDIS_STATUS_INVALID_OID (0xC0010017U)
/*! Device is connected to network medium. */
#define RNDIS_STATUS_MEDIA_CONNECT (0x4001000BU)
/*! Device is disconnected from network medium. */
#define RNDIS_STATUS_MEDIA_DISCONNECT (0x4001000CU)
/* @} */

/*!
 * @name RNDIS Response sizes
 * Definitions of the size of response of various message types.
 * @{
 */
/*! Response size of INITIALIZE_MSG. */
#define RNDIS_RESPONSE_INITIALIZE_MSG_SIZE (52U)
/*! Response size of QUERY_MSG. */
#define RNDIS_RESPONSE_QUERY_MSG_SIZE (24U)
/*! Response size of SET_MSG. */
#define RNDIS_RESPONSE_SET_MSG_SIZE (16U)
/*! Response size of RESET_MSG. */
#define RNDIS_RESPONSE_RESET_MSG_SIZE (16U)
/*! Response size of KEEPALIVE_MSG. */
#define RNDIS_RESPONSE_KEEPALIVE_MSG_SIZE (16U)
/* @} */

/*! The Miniport driver type is connectionless. */
#define RNDIS_DF_CONNECTIONLESS (0x00000001U)
/*! The Miniport driver type is connection-oriented. */
#define RNDIS_DF_CONNECTION_ORIENTED (0x00000002U)
/*! The number of RNDIS data messages that the device can handle in a single transfer. */
#define RNDIS_SINGLE_PACKET_TRANSFER (0x00000001U)
/*! The byte alignment that the device expects for each RNDIS message that is part of a multimessage transfer. */
#define RNDIS_PACKET_ALIGNMENT_FACTOR (0x00000003U)
/*! The number of OIDs the RNDIS device supported. */
#define RNDIS_NUM_OIDS_SUPPORTED (25U)
/*! The vendor ID of the RNDIS device. Vendors without an IEEE-registered code should use the value 0xFFFFFF. */
#define RNDIS_VENDOR_ID (0xFFFFFFU)
/*! A single byte that the vendor assigns to identify a particular NIC*/
#define RNDIS_NIC_IDENTIFIER_VENDOR (0x01U)

/*!
 * @name RNDIS device connection status
 * Definitions of the status value of NIC connection.
 * @{
 */
/*! The network connection has been lost. */
#define NDIS_MEDIA_STATE_CONNECTED (0x00000000U)
/*! The network connection has been restored. */
#define NDIS_MEDIA_STATE_DISCONNECTED (0x00000001U)
/*! The initial value of the connection status. */
#define NDIS_MEDIA_STATE_UNKNOWN (0xFFFFFFFFU)
/* @} */

/*! DataLength : Data length of communication feature. */
#define RNDIS_MAX_EXPECTED_COMMAND_SIZE (76U)

/*! This is the maximum observed command size we get on control endpoint --
 * Memory for commands is allocated at initialization, instead of
 * being dynamically allocated  when command is received to avoid memory
 * fragmentation.
 */
#define RNDIS_MAX_EXPECTED_RESPONSE_SIZE (RNDIS_RESPONSE_QUERY_MSG_SIZE + (RNDIS_NUM_OIDS_SUPPORTED << 2U))

/*!
 * @name Reserved for connection oriented devices. Set value to zero.
 * @{
 */
#define RNDIS_AF_LIST_OFFSET (0x00000000U)
#define RNDIS_AF_LIST_SIZE (0x00000000U)
/* @} */

/*! Size of Ethernet address. */
#define RNDIS_ETHER_ADDR_SIZE (6U)

/*! Size of USB header for RNDIS packet. */
#define RNDIS_USB_HEADER_SIZE (44U)
/*! Maximum size of multicast address list. */
#define RNDIS_MULTICAST_LIST_SIZE (0U)

/*! @brief Physical Medium Type definitions. Used with OID_GEN_PHYSICAL_MEDIUM. */
typedef enum _ndis_physical_medium_enum
{
    NDIS_PHYSICAL_MEDIUM_UNSPECIFIED,
    NDIS_PHYSICAL_MEDIUM_WIRELESS_LAN,
    NDIS_PHYSICAL_MEDIUM_CABLE_MODEM,
    NDIS_PHYSICAL_MEDIUM_PHONE_LINE,
    NDIS_PHYSICAL_MEDIUM_POWER_LINE,
    NDIS_PHYSICAL_MEDIUMDSL, /* includes ADSL and UADSL (G.Lite) */
    NDIS_PHYSICAL_MEDIUM_FIBRE_CHANNEL,
    NDIS_PHYSICAL_MEDIUM1394,
    NDIS_PHYSICAL_MEDIUM_WIRELESS_WAN,
    NDIS_PHYSICAL_MEDIUM_NATIVE802_11,
    NDIS_PHYSICAL_MEDIUM_BLUETOOTH,
    NDIS_PHYSICAL_MEDIUM_MAX /* Not a real physical type, defined as an upper-bound. */
} ndis_physical_medium_enum_t;

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

/*! @brief Define message structure for REMOTE_NDIS_SET_CMPLT. */
typedef struct _rndis_set_cmplt_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t requestID;
    uint32_t status;
} rndis_set_cmplt_struct_t;

/*! @brief Define message structure for REMOTE_NDIS_RESET_MSG. */
typedef struct _rndis_reset_msg_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t reserved;
} rndis_reset_msg_struct_t;

/*! @brief Define message structure for REMOTE_NDIS_RESET_CMPLT. */
typedef struct _rndis_reset_cmplt_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t status;
    uint32_t addressingReset;
} rndis_reset_cmplt_struct_t;

/*! @brief Define message structure for REMOTE_NDIS_INDICATE_STATUS_MSG. */
typedef struct _rndis_indicate_status_msg_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t status;
    uint32_t statusBufferLength;
    uint32_t statusBufferOffset;
} rndis_indicate_status_msg_struct_t;

/*! @brief Define message structure for REMOTE_NDIS_KEEPALIVE_MSG. */
typedef struct _rndis_keepalive_msg_struct
{
    uint32_t messageType;
    uint32_t messageLength;
    uint32_t requestID;
} rndis_keepalive_msg_struct_t;

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
} rndis_packet_msg_struct_t;

/*! @brief Define RNDIS device state. See MSDN for details. */
typedef enum _rndis_state_enum
{
    /*! Following bus-level initialization, the device is said to be in the RNDIS-uninitialized state.
    * If the device receives a REMOTE_NDIS_HALT_MSG, a bus-level disconnects, or a hard-reset
    * at any time, it forces the device to the RNDIS-uninitialized state.
    */
    RNDIS_UNINITIALIZED = 0,
    /*! After the device receives a REMOTE_NDIS_INITIALIZE_MSG and responds with
    * a REMOTE_NDIS_INITIALIZE_CMPLT with a status of RNDIS_STATUS_SUCCESS, the
    * device enters the RNDIS-initialized state.
    * If the device is in the RNDIS-data-initialized state when it receives a REMOTE_NDIS_SET_MSG
    * specifying a zero filter value for OID_GEN_CURRENT_PACKET_FILTER, this event forces
    * the device back to the RNDIS-initialized state.
    */
    RNDIS_INITIALIZED,
    /*! If the device receives a REMOTE_NDIS_SET_MSG that specifies a non-zero filter value
    * for OID_GEN_CURRENT_PACKET_FILTER, the device enters the RNDIS-data-initialized state.
    */
    RNDIS_DATA_INITIALIZED,
} rndis_state_enum_t;

/*! @brief Define structure for CDC RNDIS device. */
typedef struct _usb_device_cdc_rndis_struct
{
    class_handle_t cdcAcmHandle;          /*!< USB CDC ACM class handle. */
    uint8_t *rndisCommand;                /*!< The pointer to the buffer of the RNDIS request. */
    uint8_t *responseData;                /*!< The pointer to the buffer of the RNDIS response. */
    uint32_t rndisHostMaxTxSize;          /*!< The maximum transmit size in byte of the host. */
    uint32_t rndisDevMaxTxSize;           /*!< The maximum transmit size in byte of the device. */
    uint32_t rndisHwState;                /*!< The hardware state of the RNDIS device. */
    uint32_t rndisPacketFilter;           /*!< The packet filter of the RNDIS device. */
    uint32_t rndisMediaConnectStatus;     /*!< The media connection status of the RNDIS device. */
    uint32_t numFramesTxOk;               /*!< The number of the frames sent successfully. */
    uint32_t numFramesRxOk;               /*!< The number of the frames received successfully. */
    uint32_t numFramesTxError;            /*!< The number of the frames sent failed. */
    uint32_t numFramesRxError;            /*!< The number of the frames received failed. */
    uint32_t numRecvFramesMissed;         /*!< The number of the frames missed to receive. */
    uint32_t numRecvFramesAlignmentError; /*!< The number of the frames received that has alignment error. */
    uint32_t numFramesTxOneCollision;     /*!< The number of the frames sent that has one collision. */
    uint32_t numFramesTxManyCollision;    /*!< The number of the frames sent that has many collision. */
    uint8_t rndisDeviceState;             /*!< The RNDIS device state. */
    usb_osa_mutex_handle statusMutex;     /*!< The mutex to guarantee the consistent access to the device state. */
    /*! The callback function provided by application for the RNDIS request. */
    usb_status_t (*rndisCallback)(class_handle_t handle, uint32_t event, void *param);
} usb_device_cdc_rndis_struct_t;

/*! @brief Define structure for CDC RNDIS device. */
typedef struct _usb_device_cdc_rndis_config_struct
{
    /*! The maximum transmit size in byte of the device. This value is configured by application. */
    uint32_t devMaxTxSize;
    /*! The callback function provided by application for the RNDIS request. */
    usb_status_t (*rndisCallback)(class_handle_t handle, uint32_t event, void *param);
} usb_device_cdc_rndis_config_struct_t;

/*! @brief Define parameters for CDC RNDIS request. */
typedef struct _usb_device_cdc_rndis_request_param_struct
{
    uint8_t *buffer; /*!< The pointer to the buffer for RNDIS request. */
    uint32_t length; /*!< The length of the buffer for RNDIS request. */
} usb_device_cdc_rndis_request_param_struct_t;

/*! @brief Define RNDIS event. */
typedef enum _rndis_event_enum
{
    kUSB_DeviceCdcEventAppGetLinkSpeed,      /*!< This event indicates to get the link speed of the Ethernet. */
    kUSB_DeviceCdcEventAppGetSendPacketSize, /*!< This event indicates to get the USB send packet size. */
    kUSB_DeviceCdcEventAppGetRecvPacketSize, /*!< This event indicates to get the USB receive packet size. */
    kUSB_DeviceCdcEventAppGetMacAddress,     /*!< This event indicates to get the mac address of the device. */
    kUSB_DeviceCdcEventAppGetLinkStatus,     /*!< This event indicates to get the link status of the Ethernet. */
    kUSB_DeviceCdcEventAppGetMaxFrameSize    /*!< This event indicates to get the Ethernet maximum frame size. */
} rndis_event_enum_t;

/*******************************************************************************
* API
******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name USB CDC ACM Class Driver
 * @{
 */
/*!
 * @brief Initializes the USB CDC RNDIS device.
 *
 * This function sets the initial value for RNDIS device state, hardware state and media connection status, configures
 * the maximum transmit size and the RNDIS request callback according to the user configuration structure. It also
 * creates
 * the mutex for accessing the device state.
 *
 * @param classHandle The class handle of the CDC ACM class.
 * @param config The configure structure of the RNDIS device.
 * @param handle This is a out parameter. It points to the address of the USB CDC RNDIS device handle.
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success Initialize the RNDIS device successfully.
 * @retval kStatus_USB_Error Fails to allocate for the RNDIS device handle.
 */
extern usb_status_t USB_DeviceCdcRndisInit(class_handle_t classHandle,
                                           usb_device_cdc_rndis_config_struct_t *config,
                                           usb_device_cdc_rndis_struct_t **handle);

/*!
 * @brief Deinitializes the USB CDC RNDIS device.
 *
 * This function destroys the mutex of the device state and frees the RNDIS device handle.
 *
 * @param handle This is a pointer to the USB CDC RNDIS device handle.
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success De-Initialize the RNDIS device successfully.
 * @retval kStatus_USB_Error Fails to free the RNDIS device handle.
 * @retval kStatus_USB_InvalidHandle The RNDIS device handle is invalid.
 */
extern usb_status_t USB_DeviceCdcRndisDeinit(usb_device_cdc_rndis_struct_t *handle);

/*!
 * @brief Responds to kUSB_DeviceCdcEventSendEncapsulatedCommand.
 *
 * This function checks the message length to see if it exceeds the maximum of the RNDIS request size and
 * sets the device state or prepares notification for various message type accordingly.
 *
 * @param handle This is a pointer to the USB CDC RNDIS device handle.
 * @param message This is a pointer to the address of the RNDIS request buffer.
 * @param len This is a pointer to the variable of data size for the RNDIS request.
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success Responds to the host successfully.
 * @retval kStatus_USB_Error The message length exceeds the maximum of the RNDIS request.
 * @retval kStatus_USB_InvalidHandle The RNDIS device handle is invalid.
 */
extern usb_status_t USB_DeviceCdcRndisMessageSet(usb_device_cdc_rndis_struct_t *handle,
                                                 uint8_t **message,
                                                 uint32_t *len);

/*!
 * @brief Responds to kUSB_DeviceCdcEventGetEncapsulatedResponse.
 *
 * This function prepares the response for various message type which is stored in SendEncapsulatedCommand.
 *
 * @param handle This is a pointer to the USB CDC RNDIS device handle.
 * @param message This is an out parameter. It is a pointer to the address of the RNDIS response buffer.
 * @param len This is an out parameter. It is a pointer to the variable of data size for the RNDIS response.
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success Prepares for the response to the host successfully.
 * @retval kStatus_USB_InvalidRequest The message type is not supported.
 * @retval kStatus_USB_InvalidHandle The RNDIS device handle is invalid.
 */
extern usb_status_t USB_DeviceCdcRndisMessageGet(usb_device_cdc_rndis_struct_t *handle,
                                                 uint8_t **message,
                                                 uint32_t *len);

/*!
* @brief Soft reset the RNDIS device.
*
* This function is called to soft reset the RNDIS device.
*
* @param handle This is a pointer to the USB CDC RNDIS device handle.
* @param message This is an out parameter. It is a pointer to the address of the RNDIS response buffer.
* @param len This is an out parameter. It is a pointer to the variable of data size for the RNDIS response.
* @return A USB error code or kStatus_USB_Success.
* @retval kStatus_USB_Success Prepares for the response to the host successfully.
* @retval kStatus_USB_InvalidHandle The RNDIS device handle is invalid.
*/
extern usb_status_t USB_DeviceCdcRndisResetCommand(usb_device_cdc_rndis_struct_t *handle,
                                                   uint8_t **message,
                                                   uint32_t *len);

/*!
* @brief Halts the RNDIS device.
*
* This function is called to halt the RNDIS device.
*
* @param handle This is a pointer to the USB CDC RNDIS device handle.
* @return A USB error code or kStatus_USB_Success.
* @retval kStatus_USB_Success Halt the RNDIS device successfully.
* @retval kStatus_USB_InvalidHandle The RNDIS device handle is invalid.
*/
extern usb_status_t USB_DeviceCdcRndisHaltCommand(usb_device_cdc_rndis_struct_t *handle);
/*@}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _USB_DEVICE_CDC_RNDIS_H_ */
