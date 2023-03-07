/*
 * Copyright (c) 2012-2016, GreenPeak Technologies
 * Copyright (c) 2017-2022, Qorvo Inc
 *
 * gpMacCore.h
 *   This file contains the definitions of the public functions and enumerations of the gpMacCore.(based on IEEE802.15.4-2006)
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _GP_MACCORE_H_
#define _GP_MACCORE_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpEncryption.h"
/**
 * @file gpMacCore.h
 *
 * The Core functionality of the Mac layer is implemented in these functions.
 *
 * @defgroup INIT Initialization
 * This module groups the primitives for the Initialization procedure.
 *
 * @defgroup MLME_DATA MLME-DATA primitives
 * This module groups the primitives for data transmission.
 *
 * To transmit a data package the origniator needs to:
 * - use gpPd_GetPd() to claim memory that will be used to write the data payload into the radio chip
 * - trigger gpMacCore_DataRequest() with the claimed memory handle
 * - wait until the gpMacCore_cbDataConfirm() callback has been called
 * - use gpPd_FreePd(pdHandle) to free the claimed memory
 *
 * The receiving device should handle an incomming packet as follows:
 * - gpMacCore_cbDataIndication() callback indicating the data was received
 * - after reading the data from the radio chip with the gpPd_ReadByteStream((pdHandle, nsduOffset) function,
 *   use gpPd_FreePd(pdHandle) to free the memory
 *
 * @defgroup MLME_SAP MLME-SAP primitives
 * This module groups the primitives for management commands.
 *
 * - gpMacCore_AssociateRequest() to initiate the association procedure
 * - gpMacCore_AssociateResponse() the response
 * - gpMacCore_cbBeaconNotifyIndication()
 * - gpMacCore_PollRequest()
 * - gpMacCore_Reset()
 * - gpMacCore_ScanRequest() to initiate a scan process
 *
*/


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifndef GP_DIVERSITY_NR_OF_STACKS
#define GP_DIVERSITY_NR_OF_STACKS    1
#endif //GP_DIVERSITY_NR_OF_STACKS

#define GP_MACCORE_PANID_BROADCAST              0xFFFF
#define GP_MACCORE_SHORT_ADDR_BROADCAST         0xFFFF
#define GP_MACCORE_SHORT_ADDR_UNALLOCATED       0xFFFE

#define GP_MACCORE_MULTICHANNEL_MAX_CHANNELS 3
#define GP_MACCORE_INVALID_CHANNEL 0xFF

// phy definitions
/** @brief The minimal value that the header overhead can be in a frame (2 bytes framecontrol + 1 byte sequence number + 2 bytes FCS), as specified in the IEEE802.15.4-2006 spec.*/
#define GP_MACCORE_MIN_FRAME_OVERHEAD        5  /* 2 framecontrol + 1 sequence number + 2 FCS */
/** @brief The maximal number of bytes the header overhead can be in a beacon, as specified in the IEEE802.15.4-2006 spec.*/
#define GP_MACCORE_MAX_BEACON_OVERHEAD       75
/** @brief The maximal PHY length of a packet (in bytes), as specified in the IEEE802.15.4-2006 spec.*/
#define GP_MACCORE_MAX_PHY_PACKET_SIZE   127
/** @brief The maximal PHY length of a packet (in bytes), as specified in the IEEE802.15.4-2006 spec without the CRC bytes.*/
#define GP_MACCORE_MAX_PHY_PACKET_SIZE_NO_FCS   125
/** @brief The maximal length of the MAC payload (in bytes), as specified in the IEEE802.15.4-2006 spec.*/
#define GP_MACCORE_MAX_MAC_FRAME_SIZE       (GP_MACCORE_MAX_PHY_PACKET_SIZE - GP_MACCORE_MIN_FRAME_OVERHEAD)
/** @brief The maximal length of the beacon payload (in bytes), as specified in the IEEE802.15.4-2006 spec*/
#define GP_MACCORE_MAX_BEACON_PAYLOAD_LENGTH GP_MACCORE_MAX_PHY_PACKET_SIZE - GP_MACCORE_MAX_BEACON_OVERHEAD
/** @brief The maximum number of octets added by the MAC sublayer to the PSDU without security, as specified in the IEEE 802.15.4 spec*/
#define GP_MACCORE_MAX_MPDU_UNSECURED_OVERHEAD 25   // 23 bytes MAC Header + 2 bytes MAC footer
/** @brief The maximum number of octets that can be transmitted in the MAC Payload field of an unsecured MAC frame that will be guaranteed not to exceed aMaxPHYPacketSize., as specified in the IEEE 802.15.4 spec*/
#define GP_MACCORE_MAX_MAC_SAVE_PAYLOAD_SIZE (GP_MACCORE_MAX_PHY_PACKET_SIZE - GP_MACCORE_MAX_MPDU_UNSECURED_OVERHEAD)

/** @brief The number of symbols forming a superframe slot when the superframe order is equal to 0. */
#define GP_MACCORE_BASE_SLOT_DURATION   60
/** @brief The number of slots contained in any superframe. */
#define GP_MACCORE_NUM_SUPERFRAME_SLOTS 16
/** @brief The number of symbols forming a superframe when the superframe order is equal to 0.
 *  As it's defined in symbols, 960 needs to be multipled by 16us to get time in us = 15360us */
#define GP_MACCORE_BASE_SUPERFRAME_DURATION GP_MACCORE_BASE_SLOT_DURATION*GP_MACCORE_NUM_SUPERFRAME_SLOTS
/** @brief Maximum scan duration that can be given to a San Request primitive.
 *  Actual scan window time is calculated with 'aBaseSuperframeDuration * (2^n + 1)' = 960 * 16 * (2^n + 1).
 *  A scanduration of 4 will give +- 260ms. */
#define GP_MACCORE_MAX_SCAN_DURATION    14

/** @brief Maximum header byte size without security fields (23 bytes) */
#define GP_MACCORE_MAX_MAC_HEADER_SIZE               23
/** @brief Maximum security header byte size, consisting of security control (1), frame contorl (4) and keyIdentifier (0/1/5/9). */
#define GP_MAC_MAX_MACCORE_SECURITY_PART_HEADER_SIZE 14
/** @brief Maximum header byte size including security fields (23+14 = 37 bytes) */
#define GP_MACCORE_MAX_MAC_SECURED_HEADER_SIZE (GP_MACCORE_MAX_MAC_HEADER_SIZE + GP_MAC_MAX_MACCORE_SECURITY_PART_HEADER_SIZE)

// MAC constants
/** @brief The symbol duration in microseconds.  Two symbols correspond to 1 transmitted byte. */
#define GP_MACCORE_SYMBOL_DURATION              16   //us/symbool

/** @brief RX-to-TX or TX-to-RX maximum turnaround time (in symbol periods).
 *         As specified in IEEE802.15.4-2006 paragraph 6.4.1.
 */
#define GP_MACCORE_TX_TO_RX_TURNAROUNDTIME      12 /* symbols */
#define GP_MACCORE_RX_TO_TX_TURNAROUNDTIME      12 /* symbols */
/** @brief The PHY header size in octets.
 *         As specified in IEEE802.15.4-2006 paragraph 6.3 PPDU format.
 */
#define GP_MACCORE_PHYHEADER_OCTETS             (4 + 1 + 1) /* actual preamble + sdf + len field */

/** @brief The number of bits per modulation symbol.
 *         As specified in IEEE802.15.4-2006 paragraph 6.5 2450 MHz PHY specifications.
 */
#define GP_MACCORE_PHY_BITS_PER_SYMBOL           4

/** @brief The size of an ACK frame PDU without an encryption header in octets.
 *         As specified in IEEE802.15.4-2006 paragraph 7.2.2.3 Acknowledgment frame format.
 */
#define GP_MACCORE_MAX_PHY_ACK_PACKET_SIZE       5

/** @brief The size of an Enhanced ACK frame PDU with an encryption header in octets.
 *         As specified in IEEE802.15.4-2015 paragraph 7.3.3 Acknowledgment frame format.
 *         The current max value is the value with:
 *         Short Source addressing, Source Dest addressing, PanID compression, Aux header,
 *         One Thread CSL IE, and HT2 header termination.
 */
#define GP_MACCORE_MAX_PHY_ENHACK_PACKET_SIZE    29

/** @brief Max duration in us for transmitting a frame and receiving the corresponding ACK, without the CSMA time
 */
#define GP_MACCORE_MAX_TXFRAME_PLUS_RXACK_TIME(ack_size)  ( (GP_MACCORE_RX_TO_TX_TURNAROUNDTIME * GP_MACCORE_SYMBOL_DURATION)     + \
                                                  ( ( GP_MACCORE_PHYHEADER_OCTETS + GP_MACCORE_MAX_PHY_PACKET_SIZE )              * \
                                                  8 / GP_MACCORE_PHY_BITS_PER_SYMBOL * GP_MACCORE_SYMBOL_DURATION)                + \
                                                  (GP_MACCORE_TX_TO_RX_TURNAROUNDTIME * GP_MACCORE_SYMBOL_DURATION)               + \
                                                  ( ( GP_MACCORE_PHYHEADER_OCTETS + ack_size )                                    * \
                                                  8 / GP_MACCORE_PHY_BITS_PER_SYMBOL * GP_MACCORE_SYMBOL_DURATION)                  \
                                                )

#define GP_MACCORE_MAX_TXFRAME_PLUS_RXNORMALACK_TIME  GP_MACCORE_MAX_TXFRAME_PLUS_RXACK_TIME(GP_MACCORE_MAX_PHY_ACK_PACKET_SIZE)
#define GP_MACCORE_MAX_TXFRAME_PLUS_RXENHACK_TIME     GP_MACCORE_MAX_TXFRAME_PLUS_RXACK_TIME(GP_MACCORE_MAX_PHY_ENHACK_PACKET_SIZE)


/** @brief The acknowledge duration time is used to define the time that the receiver stays
 *         enable waiting for an acknowledge frame after a transmitted packet that requires
 *         an acknowledge frame.
 */
#define GP_MACCORE_ACK_WAIT_DURATION            54
/** @brief The channel mask of the supported channels.  Bit 0 correspond to channel 0, bit 1 to channel 1,.... */
#define GP_MACCORE_CHANNELS_SUPPORTED           0x3FF800

/** Scan duration for ScanTypeEDWifiBeacon in microseconds (typical Wifi beacon interval is 100 ms) */
#define GP_MACCORE_WIFI_BEACON_SCAN_DURATION    MS_TO_US(103)

/** @brief The maximum number of octets that can be transmitted in the MAC Payload field.*/
#define GP_MACCORE_MAX_MAC_SAFE_PAYLOAD_SIZE        (GP_MACCORE_MAX_PHY_PACKET_SIZE - GP_MACCORE_MAX_MPDU_UNSECURED_OVERHEAD)

// Scan related defines
#ifndef GP_MACCORE_SCAN_RXOFFWINDOW_TIME_US
#define GP_MACCORE_SCAN_RXOFFWINDOW_TIME_US  0
#endif //GP_MACCORE_SCAN_RXOFFWINDOW_TIME_US

// Security related defines
#ifndef GP_MACCORE_NUMBER_OF_KEY_DEVICE_DESCRIPTORS
#define GP_MACCORE_NUMBER_OF_KEY_DEVICE_DESCRIPTORS 5
#endif //GP_MACCORE_NUMBER_OF_KEY_DEVICE_DESCRIPTORS

#ifndef GP_MACCORE_NUMBER_OF_DEVICE_DESCRIPTORS
#define GP_MACCORE_NUMBER_OF_DEVICE_DESCRIPTORS 5
#endif //GP_MACCORE_NUMBER_OF_DEVICE_DESCRIPTORS

#ifndef GP_MACCORE_NUMBER_OF_KEY_DESCRIPTOR_ENTRIES
#define GP_MACCORE_NUMBER_OF_KEY_DESCRIPTOR_ENTRIES 5
#endif //GP_MACCORE_NUMBER_OF_KEY_DESCRIPTOR_ENTRIES

#ifndef GP_MACCORE_NUMBER_OF_SECURITY_LEVEL_DESCRIPTORS
#define GP_MACCORE_NUMBER_OF_SECURITY_LEVEL_DESCRIPTORS 1
#endif //GP_MACCORE_NUMBER_OF_SECURITY_LEVEL_DESCRIPTORS

#ifndef GP_MACCORE_NUMBER_OF_KEY_ID_LOOKUP_DESCRIPTORS
#define GP_MACCORE_NUMBER_OF_KEY_ID_LOOKUP_DESCRIPTORS 3
#endif //GP_MACCORE_NUMBER_OF_KEY_ID_LOOKUP_DESCRIPTORS

#ifndef GP_MACCORE_NUMBER_OF_KEY_USAGE_DESCRIPTORS
#define GP_MACCORE_NUMBER_OF_KEY_USAGE_DESCRIPTORS 2
#endif //GP_MACCORE_NUMBER_OF_KEY_USAGE_DESCRIPTORS

/** @brief Return stack identifier when no stack is found */
#define GP_MACCORE_STACK_UNDEFINED 0xff

// Compile time definitions to optimize single stack code size
#if (GP_DIVERSITY_NR_OF_STACKS > 1) || (defined(GP_DIVERSITY_JUMPTABLES))
#define MACCORE_STACKID_REF               stackId
#define MACCORE_STACKID_ARG_1             gpMacCore_StackId_t MACCORE_STACKID_REF
#define MACCORE_STACKID_ARG_2             ,gpMacCore_StackId_t MACCORE_STACKID_REF
#define MACCORE_STACKID_MAP_1(stackId)    stackId
#define MACCORE_STACKID_MAP_2(a,stackId)  a,stackId
#define MACCORE_STACKID_MAP_4(a,b,c,stackId)  a,b,c,stackId
#else //(GP_DIVERSITY_NR_OF_STACKS > 1) || (defined(GP_DIVERSITY_JUMPTABLES))
#define MACCORE_STACKID_REF               0
#define MACCORE_STACKID_ARG_1             void
#define MACCORE_STACKID_ARG_2
#define MACCORE_STACKID_MAP_1(stackId)
#define MACCORE_STACKID_MAP_2(a,stackId)  a
#define MACCORE_STACKID_MAP_4(a,b,c,stackId)  a,b,c
#endif //(GP_DIVERSITY_NR_OF_STACKS > 1) || (defined(GP_DIVERSITY_JUMPTABLES))

// Different Tx option settings
#define GP_MACCORE_TX_OPT_NONE                  0x00
/** @brief Enable ACK request for this packet Data Request */
#define GP_MACCORE_TX_OPT_ACK_REQ               0x01
/** @brief Enable GTS transmission for this packet Data Request */
#define GP_MACCORE_TX_OPT_GTS                   0x02
/** @brief Send packet as an indirect transmission. Packet will be buffered untill polled successfully or expired. */
#define GP_MACCORE_TX_OPT_INDIRECT              0x04
#ifdef GP_MACCORE_DIVERSITY_RAW_FRAMES
/** @brief Keep the framecounter value as specified by the higher layers in the frame during the raw encryption. */
#define GP_MACCORE_TX_OPT_RAW_KEEP_FRAMECOUNTER 0x08
#endif //GP_MACCORE_DIVERSITY_RAW_FRAMES
//Following options are not defined in spec - added for DataRequest API purposes
/** @brief Enable Transmission via scheduled HW TX queue. */
#define GP_MACCORE_TX_OPT_TIMEDTX               0x10
#ifdef GP_MACCORE_DIVERSITY_RAW_FRAMES
/** @brief Use a raw transmission of the payload. No MAC header is added or security applied.
 *         To be used when above layer handles the full packet formatting.*/
#define GP_MACCORE_TX_OPT_RAW                   0x20
#endif //GP_MACCORE_DIVERSITY_RAW_FRAMES
/** @brief Perform a direct transmission, but as a response to a previous poll/datarequest. */
#define GP_MACCORE_TX_OPT_INJECT                0x40
/** @brief Set the data pending bit in the Packet header.
           This is handled automatically for indirect queued packets according to the queue contents. */
#define GP_MACCORE_TX_OPT_MORE_DATA_PENDING     0x80

#define GP_MACCORE_INDIRECT_TRANSMISSION_ENABLED(txOptions)             (txOptions & GP_MACCORE_TX_OPT_INDIRECT)
#define GP_MACCORE_INDIRECT_TRANSMISSION_CLEAR(txOptions)               (txOptions&=~GP_MACCORE_TX_OPT_INDIRECT)

#define GP_MACCORE_INJECT_ENABLED(txOptions)             (txOptions & GP_MACCORE_TX_OPT_INJECT)
#define GP_MACCORE_INJECT_CLEAR(txOptions)               (txOptions&=~GP_MACCORE_TX_OPT_INJECT)

#define GP_MACCORE_TIMEDTX_ENABLED(txOptions)           (txOptions & GP_MACCORE_TX_OPT_TIMEDTX)

#define GP_MACCORE_ADDRESSMODE_NONE(mode)               (mode == gpMacCore_AddressModeNoAddress)
#define GP_MACCORE_ADDRESSMODE_RESERVED(mode)           (mode == gpMacCore_AddressModeReserved)
#define GP_MACCORE_ADDRESSMODE_SHORT(mode)              (mode == gpMacCore_AddressModeShortAddress)
#define GP_MACCORE_ADDRESSMODE_EXTENDED(mode)           (mode == gpMacCore_AddressModeExtendedAddress)

#define GP_MACCORE_IS_SHORT_ADDR_BROADCAST(addr)        (addr == GP_MACCORE_SHORT_ADDR_BROADCAST)
#define GP_MACCORE_IS_SHORT_ADDR_UNALLOCATED(addr)      (addr == GP_MACCORE_SHORT_ADDR_UNALLOCATED)

#define GP_MACCORE_NO_SECURITY_SPECIFIED(pSecOptions)   (pSecOptions == NULL || pSecOptions->securityLevel == gpEncryption_SecLevelNothing)

#define GP_MACCORE_CHECK_CHANNEL_VALID(channel)         (((channel) >= 11) && ((channel) <= 26))

// shortcut to use checks in if statement (that can be omitted when assert level does not match)
#define GP_MACCORE_CHECK_IF(check)                      (GP_ASSERT_IF(GP_DIVERSITY_ASSERT_LEVEL_DEV_EXT, check))

// regular check macro's (to be used anywhere, cannot be ommitted during compilation)
#define GP_MACCORE_CHECK_ADDRESSMODE_NONE(mode)             (mode != gpMacCore_AddressModeNoAddress)
#define GP_MACCORE_CHECK_ADDRESSMODE_VALID(mode)            (mode != gpMacCore_AddressModeReserved && mode <= gpMacCore_AddressModeExtendedAddress)
#define GP_MACCORE_CHECK_ADDRESSINFO_VALID(info)            (info != NULL && GP_MACCORE_CHECK_ADDRESSMODE_VALID(info->addressMode))
#define GP_MACCORE_CHECK_ADDRESSMODE_PRESENT(mode)          (mode == gpMacCore_AddressModeShortAddress || mode == gpMacCore_AddressModeExtendedAddress)
#define GP_MACCORE_CHECK_SCANTYPE_VALID(type)               ((type != gpMacCore_ScanTypePassive && type <= gpMacCore_ScanTypeOrphan) || (type >= gpMacCore_ScanTypeRSSI && type <= gpMacCore_ScanTypeAIMasked) )
#define GP_MACCORE_CHECK_SCANDURATION_VALID(duration)       (duration <= GP_MACCORE_MAX_SCAN_DURATION || duration == 0xFF)
#define GP_MACCORE_CHECK_TXOPTIONS_VALID(options)           (!GP_MACCORE_INDIRECT_TRANSMISSION_ENABLED(options))

// special check-if macro's that return true when check is false (to be used inside if statement, can be ommited when assert level does not match)
#define GP_MACCORE_CHECK_IF_ADDRESSMODE_NOT_NONE(mode)      (GP_MACCORE_CHECK_IF(GP_MACCORE_CHECK_ADDRESSMODE_NONE(mode)))
#define GP_MACCORE_CHECK_IF_ADDRESSMODE_INVALID(mode)       (GP_MACCORE_CHECK_IF(GP_MACCORE_CHECK_ADDRESSMODE_VALID(mode)))
#define GP_MACCORE_CHECK_IF_ADDRESSINFO_INVALID(info)       (GP_MACCORE_CHECK_IF(GP_MACCORE_CHECK_ADDRESSINFO_VALID(info)))
#define GP_MACCORE_CHECK_IF_ADDRESSMODE_NOT_PRESENT(mode)   (GP_MACCORE_CHECK_IF(GP_MACCORE_CHECK_ADDRESSMODE_PRESENT(mode)))
#define GP_MACCORE_CHECK_IF_SCANTYPE_INVALID(type)          (GP_MACCORE_CHECK_IF(GP_MACCORE_CHECK_SCANTYPE_VALID(type)))
#define GP_MACCORE_CHECK_IF_SCANDURATION_INVALID(duration)  (GP_MACCORE_CHECK_IF(GP_MACCORE_CHECK_SCANDURATION_VALID(duration)))
#define GP_MACCORE_CHECK_IF_TXOPTIONS_INVALID(options)      (GP_MACCORE_CHECK_IF(GP_MACCORE_CHECK_TXOPTIONS_VALID(options)))

#define GP_MACCORE_DEFAULT_TRANSACTION_PERSISTENCE_TIME 0x01F4

#define GP_MACCORE_STATISTICS_ALL_DEVICES   0xFFFF

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt8 gpMacCore_StackId_t;

/** @name gpMacCore_Result_t */
//@{
//PHY results
/** @brief The requested operation was completed successfully. For instance if a transmission was requested, this value indicates a successful transmission.*/
#define gpMacCore_ResultSuccess                 0x0
//MAC results
/** @brief The frame counter purportedly applied by the originator of the received frame is invalid. */
#define gpMacCore_ResultCounterError            0xdb
/** @brief The key purportedly applied by the originator of the received frame is not allowed to be used with that frame type according to the key usage policy of the recipient.*/
#define gpMacCore_ResultImproperKeyType         0xdc
/** @brief The security level purportedly applied by the originator of the received frame does not meet the minimum security level required/expected by the recipient for that frame type.*/
#define gpMacCore_ResultImproperSecurityLevel   0xdd
/** @brief The received frame was purportedly secured using security based on IEEE Std 802.15.4-2003, and such security is not supported by this standard. */
#define gpMacCore_ResultUnsupportedLegacy       0xde
/** @brief The security purportedly applied by the originator of the received frame is not supported. */
#define gpMacCore_ResultUnsupportedSecurity     0xdf
/** @brief The beacon was lost following a synchronization request. */
#define gpMacCore_ResultBeaconLoss              0xe0
/** @brief A transmission could not take place due to activity on the channel, i.e., the CSMA-CA mechanism has failed. */
#define gpMacCore_ResultChannelAccessFailure    0xe1
/** @brief The GTS request has been denied by the PAN coordinator. */
#define gpMacCore_ResultDenied                  0xe2
/** @brief The attempt to disable the transceiver has failed. */
#define gpMacCore_ResultDisableTrxFailure       0xe3
/** @brief The received frame induces a failed security check according to the security suite. */
#define gpMacCore_ResultSecurityError           0xe4
/** @brief The frame resulting from secure processing has a length that is greater than aMACMaxFrameSize. */
#define gpMacCore_ResultFrameTooLong            0xe5
/** @brief The requested GTS transmission failed because the specified GTS either did not have a transmit GTS direction or was not defined. */
#define gpMacCore_ResultInvalidGTS              0xe6
/** @brief A request to purge an MSDU from the transaction queue wasmade using an MSDU handle that was not found in the transaction table. */
#define gpMacCore_ResultInvalidHandle           0xe7
/** @brief A parameter in the primitive is out of the valid range. */
#define gpMacCore_ResultInvalidParameter        0xe8
/** @brief No acknowledgment was received after aMaxFrameRetries. */
#define gpMacCore_ResultNoAck                   0xe9
/** @brief A scan operation failed to find any network beacons. */
#define gpMacCore_ResultNoBeacon                0xea
/** @brief No response data was available following a request. */
#define gpMacCore_ResultNoData                  0xeb
/** @brief The operation failed because a short address was not allocated. */
#define gpMacCore_ResultNoShortAddress          0xec
/** @brief A request to enable the receiver was unsuccessful because it could not be completed within the CAP. */
#define gpMacCore_ResultOutOfCAP                0xed
/** @brief A PAN identifier conflict has been detected and communicated to the PAN coordinator. */
#define gpMacCore_ResultPanIdConflict           0xee
/** @brief A coordinator realignment command has been received. */
#define gpMacCore_ResultRealignment             0xef
/** @brief The transaction has expired and its information is discarded. */
#define gpMacCore_ResultTransactionExpired      0xf0
/** @brief There is no capacity to store the transaction. */
#define gpMacCore_ResultTransactionOverflow     0xf1
/** @brief The transceiver was transmitting when the receiver was requested to be enabled. */
#define gpMacCore_ResultTxActive                0xf2
/** @brief The appropriate key is not available in the ACL. */
#define gpMacCore_ResultUnavailableKey          0xf3
/** @brief A SET/GET request was issued with the identifier of a PIBattribute that is not supported. */
#define gpMacCore_ResultUnsupportedAttribute    0xf4
/** @brief A request to send data was unsuccessful because neither the source address parameters nor the destination address parameters were present.*/
#define gpMacCore_ResultInvalidAddress          0xf5
/** @brief A receiver enable request was unsuccessful because it specified a number of symbols that was longer than the beacon interval. */
#define gpMacCore_ResultOnTimeTooLong           0xf6
/** @brief A receiver enable request was unsuccessful because it could not be completed within the current superframe and was not permitted to be deferred until the next superframe. */
#define gpMacCore_ResultPastTime                0xf7
/** @brief The device was instructed to start sending beacons based on the timing of the beacon transmissions of its coordinator, but the device is not currently tracking the beacon of its coordinator.*/
#define gpMacCore_ResultTrackingOff             0xf8
/** @brief An attempt to write to a MAC PIB attribute that is in a table failed because the specified table index was out of range.*/
#define gpMacCore_ResultInvalidIndex            0xf9
/** @brief A scan operation terminated prematurely because the number of PAN descriptors stored reached an implementation specified maximum.*/
#define gpMacCore_ResultLimitReached            0xfa
/** @brief A SET/GET request was issued with the identifier of an attribute that is read only. */
#define gpMacCore_ResultReadOnly                0xfb
/** @brief A request to perform a scan operation failed because the MLME was in the process of performing a previously initiated scan operation. */
#define gpMacCore_ResultScanInProgress          0xfc
/** @brief The device was instructed to start sending beacons based on the timing of the beacon transmissions of its coordinator, but the instructed start time overlapped the transmission time of the beacon of its coordinator. */
#define gpMacCore_ResultSuperframeOverlap       0xfd
/** @typedef gpMacCore_Result_t
 *  @brief The gpMacCore_Result_t type defines the result as defined by the IEEE802.15.4 MAC. Not all the  possible values are used by the non-beaconed MAC.
*/
typedef UInt8 gpMacCore_Result_t;
//@}

/** @name gpMacCore_Attribute_t */
//@{
// phy attr
/** @brief The IEEE802.15.4-2006 channel to use for all following transmissions and receptions.*/
#define gpMacCore_AttributeCurrentChannel               0x0 /* The RF channel to be used for all following transmissions and receptions */
/** @brief The 5 most significant bits (MSBs) (b27,... ,b31) of ChannelsSupported shall be reserved and set to 0, and the 27 LSBs (b0,b1, ... b26) shall indicate the status(1=available, 0=unavailable) for each of the 27 valid channels.  */
#define gpMacCore_AttributeChannelsSupported            0x1 /* Supported channel mask from bit 0 to bit 27 */
/** @brief The 2 MSBs represent the tolerance on the transmit power: 00 = +- 1 dB 01 = +- 3 dB 10 = +- 6 dB The 6 LSBs represent a signed integer in twos-complement format, corresponding to the nominal transmit power of the device in decibels relative to 1 mW. The lowest value of phyTransmitPower shall be interpreted as less than or equal to +-32 dBm. */
#define gpMacCore_AttributeTransmitPower                0x2 /* Transmit pozer in mdB */
/** @brief The CCA mode to use. Three CCA modes (1-3) are defined as specified in the IEEE802.15.4-2006 standard. */
#define gpMacCore_AttributeCCAMode                      0x3 /* CCA mode */
// mac attr
/** @brief The maximum number of symbols to wait for an acknowledgment frame to arrive following a transmitted data frame. */
#define gpMacCore_AttributeAckWaitDuration              0x40
//#define gpMacCore_AttributeMacBattLifeExt             0x43
//#define gpMacCore_AttributeBattLifeExtPeriods         0x44
/** @brief The extended 64 bit IEEE802.15.4-2006 MAC address of the coordinator of the network the device has joined. */
#define gpMacCore_AttributeCoordExtendedAddress         0x4A
/** @brief The 16 bit short address assigned to the coordinator with which the device is associated. A value of 0xfffe indicates that the coordinator is only using its 64 bit extended address. A value of 0xffff indicates that this value is unknown. */
#define gpMacCore_AttributeCoordShortAddress            0x4B
/** @brief The sequence number added to the data or MAC command frames to be transmitted. */
#define gpMacCore_AttributeDSN                          0x4C
/** @brief The maximum number of backoffs the CSMA-CA algorithm will attempt before declaring a channel access failure. */
#define gpMacCore_AttributeMaxCsmaBackoffs              0x4E
/** @brief The minimum value of the backoff exponent in the CSMA-CA algorithm. Note that if this value is set to 0, collision avoidance is disabled during the first iteration of the algorithm. Also note that for the slotted version of the CSMA-CA algorithm with batterylife extension enabled, the minimum value of the backoff exponent will be the lesser of 2 and the value of macMinBE. */
#define gpMacCore_AttributeMinBE                        0x4F
/** @brief The 16 bit identifier of the PAN (Personal Area Network) on which the device is operating. If this value is 0xffff, the device is not associated to a network. */
#define gpMacCore_AttributePANId                        0x50
/** @brief This attribute indicates whether the MAC sublayer is to enable its receiver during idle periods. */
#define gpMacCore_AttributeRxOnWhenIdle                 0x52
/** @brief The 16 bit address that the device uses to communicate in the PAN. If the device is a PAN coordinator, this value shall be chosen before a PAN is started. Otherwise, the address is allocated by a coordinator during association. A value of 0xfffe indicates that the device has associated but has not been allocated a short address. A value of 0xffff indicates that the device is not associated to a network. */
#define gpMacCore_AttributeShortAddress                 0x53
//#define gpMacCore_AttributeACLEntryDescriptorSet      0x70
//#define gpMacCore_AttributeACLEntryDescriptorSetSize  0x71
/** @brief Indication of whether a coordinator is currently allowing association. A value of TRUE indicates that association is permitted. */
#define gpMacCore_AttributeAssociationPermit            0x41
/** @brief The contents of the beacon payload. The length of the beacon payload is specified through the 'beacon payload length' attribute.*/
#define gpMacCore_AttributeBeaconPayload                0x45
/** @brief The length, in octets, of the beacon payload. */
#define gpMacCore_AttributeBeaconPayloadLength          0x46
/** @brief Specification of how often the coordinator transmits a beacon. The macBeaconOrder, BO, and the beacon interval, BI, are related as follows: for 0 <= BO <= 14, BI = aBaseSuperframeDuration * 2BO symbols. If BO = 15, the coordinator will not transmit a beacon. */
#define gpMacCore_AttributeBeaconOrder                  0x47
//#define gpMacCore_AttributeBeaconTxTime               0x48
//#define gpMacCore_AttributeBSN                        0x49
/** @brief TRUE if the PAN coordinator is to accept GTS requests. FALSE otherwise. */
#define gpMacCore_AttributeGTSPermit                    0x4D
/** @brief This indicates whether the MAC sublayer is in a promiscuous (receive all) mode. A value of TRUE indicates that the MAC sublayer accepts all frames received from the PHY, regardless of the source/destination address fields. */
#define gpMacCore_AttributePromiscuousMode              0x51
/** @brief This specifies the length of the active portion of the superframe, including the beacon frame. The superframe order, SO, and the superframe duration, SD, are related as follows: for 0 <= SO <= BO <= 14, SD = aBaseSuperframeDuration*2SO symbols. If SO = 15, the superframe will not be active after the beacon.*/
#define gpMacCore_AttributeSuperFrameOrder              0x54
/** @brief The maximum time (in superframe periods) that a transaction is stored by a coordinator and it is indicated in the beacon that data is pending. */
#define gpMacCore_AttributeTransactionPersistenceTime   0x55
/** @brief  Indication of whether the device is associated to the PAN through the PAN coordinator.
* A value of TRUE indicates the device has associated through the PAN coordinator. Otherwise, the value is set to FALSE. */
#define gpMacCore_AttributeAssociatedPANCoord           0x56
/** @brief The maximum value of the backoff exponent, BE, in the CSMA-CA algorithm */
#define gpMacCore_AttributeMaxBE                        0x57
/** @brief The maximum time to wait either for a frame intended as a response to a data
* request frame or for a broadcast frame following a beacon with the Frame Pending
* field set to one. */
#define gpMacCore_AttributeMaxFrameTotalWaitTime        0x58
/** @brief The maximum number of retries allowed after a transmission failure. */
#define gpMacCore_AttributeNumberOfRetries              0x59
/** @brief The maximum time, in multiples of aBaseSuperframeDuration, a device shall wait
* for a response command frame to be available following a request command frame. */
#define gpMacCore_AttributeResponseWaitTime             0x5a
/** @brief Indication of whether the MAC sublayer has security enabled. */
#define gpMacCore_AttributeSecurityEnabled              0x5d
/** @brief Defines the Tx antenna, only used with antenna diversity. */
#define gpMacCore_AttributeTxAntenna                    0x5e

/** @brief The extended 64 bit address of the device. */
#define gpMacCore_AttributeExtendedAddress              0xFF

// Attributes for security
/** @brief KeyDescriptor entries, each containing keys and security related information. */
#define gpMacCore_AttributeKeyTable                     0x71
/** @brief Number of key tables */
#define gpMacCore_AttributeKeyTableEntries              0x72
/** @brief DeviceDescriptors for each remote device with which this device securely communicates. */
#define gpMacCore_AttributeDeviceTable                  0x73
/** @brief Number of device tables */
#define gpMacCore_AttributeDeviceTableEntries           0x74
/** @brief Provides information about the security level required for each MAC frame type and subtype. */
#define gpMacCore_AttributeSecurityLevelTable           0x75
/** @brief Number of security level tables. */
#define gpMacCore_AttributeSecurityLevelTableEntries    0x76
/** @brief The outgoing frame counter for this device. */
#define gpMacCore_AttributeFrameCounter                 0x77
/** @brief The security level used for automatic data requests. */
#define gpMacCore_AttributeAutoRequestSecurityLevel     0x78
/** @brief The key identifier mode used for automatic data requests. This attribute is invalid
* if the macAutoRequestSecurityLevel attribute is set to 0x00.*/
#define gpMacCore_AttributeAutoRequestKeyIdMode         0x79
/** @brief The originator of the key used for automatic data requests. This attribute is invalid
* if the macAutoRequestKeyIdMode element is invalid or set to 0x00. */
#define gpMacCore_AttributeAutoRequestKeySource         0x7A
/** @brief The index of the key used for automatic data requests. This attribute is invalid
* if the macAutoRequestKeyIdMode attribute is invalid or set to 0x00 */
#define gpMacCore_AttributeAutoRequestKeyIndex          0x7B
/** @brief The originator of the default key used for key identifier mode 0x01. */
#define gpMacCore_AttributeDefaultKeySource             0x7C
/** @brief  The extended address of the PAN coordinator. */
#define gpMacCore_AttributePANCoordExtendedAddress      0x7D
/** @brief The short address assigned to the PAN coordinator.
* A value of 0xfffe indicates that the PAN coordinator is only using its extended address.
* A value of 0xffff indicates that this value is unknown. */
#define gpMacCore_AttributePANCoordShortAddress         0x7E
/** @typedef gpMacCore_Attribute_t
 *  @brief The gpMacCore_Attribute_t type defines the attribute type. The value could be an IEEE802.15.4 MAC attribute or a non-standard attributes that was added for flexibility. Only the PIB attributes that are used in this mac could be used, the non-used PIB attributes are not implemented to save memory.
*/
typedef UInt8 gpMacCore_Attribute_t;

/** @enum gpMacCore_ScanType_t */
//@{
/** @brief The energy detect scan, can be used to detect the amount of energy in the air. It is only used on FFD's (Full Functional Devices).*/
#define gpMacCore_ScanTypeED                 0
/** @brief The Active Scan will send out a beacon request. FFDs in the POS (Personal Operating Space) of the device will respond by sending out a beacon.*/
#define gpMacCore_ScanTypeActive             1
/** @brief The passive scan is not implemented. */
#define gpMacCore_ScanTypePassive            2
/** @brief The orphan scan is used when a node lost his coordinator (i.e. when it has changed panId or channel). Using the orphan scan the device can locate his coordinator. */
#define gpMacCore_ScanTypeOrphan             3
/** @brief (Wifi Aware) RSSI as a measure of the energy. */
#define gpMacCore_ScanTypeRSSI               0x80
/** @brief (Wifi Aware) RSSI as a measure of the energy masked with Min Interference Level. */
#define gpMacCore_ScanTypeRSSIMasked         0x81
/** @brief (Wifi Aware) Absolute Interference as a measure of the energy (0 = -95dBm, 254 = -45dBm, 255 = RDS Blocked Channel). */
#define gpMacCore_ScanTypeAI                 0x82
/** @brief (Wifi Aware)  Absolute Interference Masked as a measure of the energy masked with Min Interference Level (0 = -95dBm, 254 = -45dBm, 255 = RDS Blocked Channel). */
#define gpMacCore_ScanTypeAIMasked           0x83
/** @brief Stop an ongoing scan. The confirm for the scan in progress will be triggered with the results up to that point. */
#define gpMacCore_ScanTypeStop               0xFF
/** @typedef gpMacCore_ScanType_t
 *  @brief The gpMacCore_ScanType_t type defines the scanType as described in the IEEE802.15.4 MAC. Since this is a non-beaconed MAC, the passive scan is not a valid value.
*/
typedef UInt8 gpMacCore_ScanType_t;
//@}

/** @enum gpMacCore_AddressMode_t */
//@{
/** @brief Values of the address mode field */
#define gpMacCore_AddressModeNoAddress                  0
#define gpMacCore_AddressModeReserved                   1
#define gpMacCore_AddressModeShortAddress               2
#define gpMacCore_AddressModeExtendedAddress            3
typedef UInt8 gpMacCore_AddressMode_t;
//@}

/** @enum gpMacCore_FrameType_t */
//@{
/** @brief Values of the frame type field */
#define gpMacCore_FrameTypeBeacon       0
#define gpMacCore_FrameTypeData         1
#define gpMacCore_FrameTypeAcknowledge  2
#define gpMacCore_FrameTypeCommand      3
typedef UInt8 gpMacCore_FrameType_t;
//@}

/** @enum gpMacCore_Command_t */
//@{
/** @brief Values of the command type field */
#define gpMacCore_CommandAssociationRequest             0x1
#define gpMacCore_CommandAssociationResponse            0x2
#define gpMacCore_CommandDataRequest                    0x4
#define gpMacCore_CommandOrphanNotification             0x6
#define gpMacCore_CommandBeaconRequest                  0x7
#define gpMacCore_CommandCoordinatorRealignment         0x8
typedef UInt8 gpMacCore_Command_t;
//@}
//@{
/** @brief Values of the mac version field */
#define gpMacCore_MacVersion2003      0
#define gpMacCore_MacVersion2006      1
#define gpMacCore_MacVersion2015      2
typedef UInt8 gpMacCore_MacVersion_t;
//@}

/** @typedef gpMacCore_PanId_t
 *  @brief The gpMacCore_PanId_t type specifies a panId (Personal Area Network Identifier). The panId is a unique identifier of the network. The length of the panId is 2 bytes.
*/
typedef UInt16 gpMacCore_PanId_t;

/** @typedef gpMacCore_SuperFrameSpec_t
 *  @brief The gpMacCore_SuperFrameSpec_t type definition based on the structure gpMacCore_SuperFrameSpec.
*/
typedef UInt16 gpMacCore_SuperFrameSpec_t;

typedef UInt8 gpMacCore_KeyIdLookupListEntries_t;
typedef UInt8 gpMacCore_KeyDeviceListEntries_t;
typedef UInt8 gpMacCore_KeyUsageListEntries_t;
typedef UInt8 gpMacCore_KeyTablesEntries_t;
typedef UInt8 gpMacCore_DeviceTablesEntries_t;
typedef UInt8 gpMacCore_Index_t;
typedef UInt8 gpMacCore_SecurityLevelTableEntries_t;

/** @enum gpMacCore_KeyIdMode_t */
//@{
/** @brief Values of the KeyId Mode field */
#define gpMacCore_KeyIdModeImplicit         0
#define gpMacCore_KeyIdModeExplicit1Octet   1
#define gpMacCore_KeyIdModeExplicit4Octet   2
#define gpMacCore_KeyIdModeExplicit8Octet   3
typedef UInt8 gpMacCore_KeyIdMode_t;
//@}

typedef UInt8 gpMacCore_KeyIndex_t;

/** @union gpMacCore_Address_t
 *  @brief The gpMacCore_Address_t union either contains a short or an extended address.
*/
typedef union {
/** The short address (2 bytes).*/
    UInt16  Short;
/** The extended address(MAC address), 8 bytes.*/
    MACAddress_t Extended;
} gpMacCore_Address_t;

typedef union{
    UInt8 pKeySource4[4];
    UInt8 pKeySource8[8];
}gpMacCore_KeySource_t;

typedef struct {
    UInt8 payloadStart;
    UInt8 headerStart;
    UInt8 payloadLength;
    UInt8 headerLength;
} gpMacCore_EncryptionLengthInfo_t;

/** @struct gpMacCore_Security_t
 *  @brief The gpMacCore_Security_t struct contains security related parameters.
 *  @param securityLevel  The security level to be used (see IEEE802.15.4-2006).
 *  @param keyIdMode      The mode used to identify the key to be used (see IEEE802.15.4-2006).
 *  @param pKeySource     A pointer to the originator of the key to be used (see IEEE802.15.4-2006).
 *  @param keyIndex       The index of the key to be used (see IEEE802.15.4-2006).
*/
typedef struct gpMacCore_Security_s{
    gpEncryption_SecLevel_t        securityLevel;
    gpMacCore_KeyIdMode_t   keyIdMode;
    gpMacCore_KeySource_t   pKeySource;
    gpMacCore_KeyIndex_t    keyIndex;
} gpMacCore_Security_t;

/** @struct gpMacCore_AddressInfo_t
 *  @brief The gpMacCore_AddressInfo_t struct contains all address related information. (addressmode, address and panId)
*/
typedef struct {
    gpMacCore_Address_t address;
    gpMacCore_PanId_t panId;
    gpMacCore_AddressMode_t addressMode;
} gpMacCore_AddressInfo_t;

/** @struct gpMacCore_PanDescriptor
 *  @brief The gpMacCore_PanDescriptor structure specifies the parameters of the PanDescriptor as is defined in the IEEE802.15.4-2006 spec.
 *  @typedef gpMacCore_PanDescriptor_t
 *  @brief The gpMacCore_PanDescriptor_t type definition based on the structure gpMacCore_PanDescriptor.
*/
typedef struct gpMacCore_PanDescriptor {
/** A timestamp indicating at which time the beacon frame was received.*/
    UInt32 timeStamp;  // Attention!!! Timestamp from beginning of subinterval (within beacon or IWU slot)
/** The superframe specification as specified in the IEEE802.15.4-2006 specification. */
    gpMacCore_SuperFrameSpec_t superFrame;
/** The channel the network is currently operating on. */
    UInt8 logicalChannel;
/** The quality of the link. This is an unsigned integer, ranging from 0 to 255. Lower values represent lower link quality.*/
    UInt8 linkQuality;
/** The address information of the pan coordinator (addressmode, address and panId) */
    gpMacCore_AddressInfo_t coordAddressInfo;
} gpMacCore_PanDescriptor_t;

typedef struct gpMacCore_KeyIdLookupDescriptor{
    UInt8 lookupData[9];
    UInt8 lookupDataSize;
}gpMacCore_KeyIdLookupDescriptor_t;

typedef struct gpMacCore_DeviceDescriptor{
    gpMacCore_PanId_t panId;
    UInt16 shortAddr;
    MACAddress_t extAddress;
    UInt32 frameCounter;
    Bool exempt;
}gpMacCore_DeviceDescriptor_t;

typedef struct gpMacCore_KeyDeviceDescriptor{
    gpMacCore_DeviceDescriptor_t deviceDescriptor;
    Bool uniqueDevice;
    Bool blackListed;
}gpMacCore_KeyDeviceDescriptor_t;

typedef struct gpMacCore_KeyUsageDescriptor{
    gpMacCore_FrameType_t frameType;
    gpMacCore_Command_t commandFrameIdentifier;
}gpMacCore_KeyUsageDescriptor_t;

typedef struct gpMacCore_KeyDescriptor{
    gpMacCore_KeyIdLookupDescriptor_t keyIdLookupDescriptors[ GP_MACCORE_NUMBER_OF_KEY_ID_LOOKUP_DESCRIPTORS ];
    gpMacCore_KeyIdLookupListEntries_t keyIdLookupListEntries;
    gpMacCore_KeyDeviceDescriptor_t keyDeviceDescriptorList[ GP_MACCORE_NUMBER_OF_KEY_DEVICE_DESCRIPTORS ];
    gpMacCore_KeyDeviceListEntries_t keyDeviceListEntries;
    gpMacCore_KeyUsageDescriptor_t keyUsageDescriptorList[ GP_MACCORE_NUMBER_OF_KEY_USAGE_DESCRIPTORS ];
    gpMacCore_KeyUsageListEntries_t keyUsageListEntries;
    UInt8 key[16];
}gpMacCore_KeyDescriptor_t;

typedef struct gpMacCore_SecurityLevelDescriptor{
    gpMacCore_FrameType_t frameType;
    gpMacCore_Command_t commandFrameIdentifier;
    gpEncryption_SecLevel_t securityMinimum;
    Bool deviceOverrideSecurityMinimum;
}gpMacCore_SecurityLevelDescriptor_t;

typedef struct gpMacCore_ShortAndPanID{
    gpMacCore_PanId_t panId;
    UInt16            shortAddress;
}gpMacCore_ShortAndPanID_t;

typedef struct gpMacCore_DeviceLookupDescriptor{
    union{
        gpMacCore_ShortAndPanID_t shortAndPanId;
        MACAddress_t extendedAddress;
    }DeviceLookup;
    UInt8 lookupSize;
}gpMacCore_DeviceLookupDescriptor_t;

typedef struct {
    gpPd_Lqi_t  lqi;
    gpPd_Rssi_t rssi;
} gpMacCore_DiagRxCntr_t;

typedef struct {
    UInt32      avgRetryCntr;
    UInt32      txCntr;
} gpMacCore_DiagTxCntr_t;

/** @typedef  gpMacCore_AssocStatus_t
 *  @brief See table 83 - Valid values of the Association Status field -  IEEE 802.14.5
 *    0x0: Association successful.
 *    0x1: PAN at capacity.
 *    0x2: PAN access denied.
*/
#define gpMacCore_AssocSuccess  0
#define gpMacCore_AssocFullPan  1
#define gpMacCore_AssocDenied   2
typedef UInt8 gpMacCore_AssocStatus_t;

typedef struct {
    UInt8 channel[GP_MACCORE_MULTICHANNEL_MAX_CHANNELS];
} gpMacCore_MultiChannelOptions_t;
#define GP_MACCORE_SET_DEFAULT_MULTICHANNELOPTIONS(multiChannelOptions) \
multiChannelOptions.channel[0] = GP_MACCORE_INVALID_CHANNEL; \
multiChannelOptions.channel[1] = GP_MACCORE_INVALID_CHANNEL; \
multiChannelOptions.channel[2] = GP_MACCORE_INVALID_CHANNEL;

/** @struct gpMacCore_TxTimingOptions_t
 *  @brief Options to specify transmission via timed TX queue.
 */
typedef struct {
    /** Absolute timestamp for scheduled TX (based on chip time in microseconds as returned by gpHal_GetTime()). */
    UInt32  txTimestamp;
} gpMacCore_TxTimingOptions_t;

typedef void (*gpMacCore_TransmissionCallback_t)(void);

/** @enum gpMacCore_DataPendingMode_t */
//@{
/** @brief Values of the DataPendingMode field */
#define gpMacCore_DataPendingModeMac802154               0 /* normal mode */
#define gpMacCore_DataPendingModeForNonNeighbourDevices  1 /* for use with indirect transmit to unknown devices e.g. needed by Zigbee 3.0 */
typedef UInt8 gpMacCore_DataPendingMode_t;
//@}

/** @name gpMacCore_TxPower_t*/
//@{
#define gpMacCore_TxPowerMin         0x7d
#define gpMacCore_TxPowerMax         0x7e
/** @brief Default transmit powers are configured per channel through gpMacCore_SetDefaultTransmitPowers */
#define gpMacCore_TxPowerDefault     0x7f
/** @typedef gpMacCore_TxPower_t
 *  @brief The gpMacCore_TxPower_t type defines the transmit power in dBm or the default, min or max define.
*/
typedef Int8 gpMacCore_TxPower_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_DataTable.h"
#include "gpMacCore_CodeJumpTableFlash_Defs.h"
#include "gpMacCore_CodeJumpTableRom_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)


/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/** @ingroup INIT
 *
 * This function initializes the MacCore layer. It should be called before calling any other request function.
 *
*/
GP_API void gpMacCore_Init(void);
GP_API void gpMacCore_DeInit(void);

/** @ingroup INIT
 * @brief This function resets the MAC layer. If 'setDefaultPib' is true, the PIB attributes will be set to their default value.
 *
 *  This function resets the MAC layer. If 'setDefaultPib' is set to true, it also resets the PIB variables, including the IEEE802.15.4-2006 standard values.
 *  The tranceiver will be switched off.
 *  All the buffered packets will be removed.
 *  The callback functions (specified using the gpMacCore_RegisterCallbackFunctions function) are registered.
 *  The interrupts are enabled.
 *  A list to find catching train messages is created (a method to filter out duplicate packets).
 *
 *  @param setDefaultPib    If true the PIB values are reset to their default value (as specified in the IEEE802.15.4-2006 specification).
 *  @param stackId          The identifier of the stack
*/
GP_API gpMacCore_Result_t gpMacCore_Reset(Bool setDefaultPib, gpMacCore_StackId_t stackId);

/** @brief This function is used to send a data packet to another device.
 *
 *  This function is used to send a data packet conform the IEEE802.15.4-2006 MAC specification.
 *
 *
 *  @param srcAddrMode    The address modes to be used for source address.      This parameter is used as in the Frame Control Field of an IEEE802.15.4-2006 packet. We refer to the define GP_IEEEMAC_SRC_ADDR_(EXT/SHORT).
 *  @param pDstAddrInfo   All the information about the destination (address mode, address and panId).
 *  @param txOptions      Tx Options byte (see IEEE802.15.4-2006).
 *  @param pSecOptions    The security options - maybe NULL to be used (see IEEE802.15.4-2006).
 *  @param pdLoh          The PD Length/Offset/Handle of the data payload .
 *  @param stackId        The identifier of the stack doing this request
*/
GP_API void gpMacCore_DataRequest(gpMacCore_AddressMode_t srcAddrMode, gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 txOptions, gpMacCore_Security_t *pSecOptions, gpMacCore_MultiChannelOptions_t multiChannelOptions, gpPd_Loh_t pdLoh, gpMacCore_StackId_t stackId);

/** @brief Schedule a packet for timed transmission.
 *
 *  To send a packet via timed transmission, perform two steps:
 *  - Call gpMacCore_DataRequest() with option GP_MACCORE_TX_OPT_TIMEDTX
 *    to queue the packet;
    - Call gpMacCore_ScheduleTimedTx() to schedule transmission of the queued packet.
 *
 *  This function is only available on hardware that supports a timed transmission queue.
 *
 *  @param pdHandle       Handle of TX packet to schedule for TX; must have been previously submitted via gpMacCore_DataRequest().
 *  @param timingOptions  Specification of scheduled transmit time.
 *  @param stackId        The identifier of the stack doing this request
 */
GP_API gpMacCore_Result_t gpMacCore_ScheduleTimedTx(gpPd_Handle_t pdHandle, gpMacCore_TxTimingOptions_t timingOptions, gpMacCore_StackId_t stackId);

/** @brief This function is used to purge a packet from the indirect transmission list
 *
 *  @param handle        The handle to be purged
 *  @param stackId       The identifier of the stack doing this request
*/
GP_API void gpMacCore_PurgeRequest(gpPd_Handle_t pdHandle, gpMacCore_StackId_t stackId);

/** @ingroup MLME_SAP
 *
 *  @brief This function is used to start a scan.
 *
 *  This function starts a scan conform the IEEE802.15.4-2006 spec. A scan can be issued to detect a channel with the least amount of interference, to find networks, etc.
 *
 *  @param scanType        This parameter defines which of the 3 implemented scans needs to be performed: ED, active or orphan scan.
 *  @param scanChannels    This parameter defines which channels need to be scanned. It is a bitmask where bit 0 must be '1' to select channel 0, bit 26 '1' to select channel 26, etc. Since the chip works in the 2.4GHz band, only channels 11 till 26 can be selected. If other channels are selected, they are ignored and only the selected channels between channel 11 and channel 26 are scanned.
 *  @param scanDuration    This parameter specifies how long a channel will be scanned. The scantime equals [GP_IEEEMAC_BASE_SUPERFRAME_DURATION * (2^scanduration + 1)]symbols, where 1 symbol is 16us.
 *  @param resultListSize  The length of the resultList (in bytes). If an ED scan is done, this should equal the amount of channels.
 *                         For an active scan, resultListSize is 0. Received Beacons are returned through gpMacCore_cbBeaconNotifyIndication() calls during the scan.
 *                         If it is an orphan scan, resultListSize is 0.
 *  @param pResultList     This is a pointer to an array where the result of the scan can be saved. If an ED scan is done, the size is 'resultListSize' bytes.
                           Note the list is not used by the active scan for memory optimizations. The results are conveyed through separate BeaconNotify callbacks.
 *  @param stackId         The identifier of the stack doing this request
*/
GP_API void gpMacCore_ScanRequest(gpMacCore_ScanType_t scanType, UInt32 scanChannels, UInt8 scanDuration , UInt8 resultListSize, UInt8* pEdScanResultList, gpMacCore_StackId_t stackId);

#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR)
/** @ingroup MLME_SAP
 *
 *  @brief This function is issued by a device who wishes to join a network.
 *
 *  This function starts the associate procedure. It is used to join a network. When called, the AssociateRequest function will construct an associate request command packet, and send it to the coordinator.
 *
 *  @param logicalChannel           The channel that will be used to do the association attempt
 *  @param coordAddrMode            The address mode to be used for the coordinator address. Possible values are: 2 = 16 bit short address, 3 = 64 bit extended address.
 *  @param coordPanId               The panId of the coordinator.
 *  @param pCoordAddress            A pointer to the address of the coordinator.
 *  @param capabilityInformation    Specifies the operational capabilities of the associating device.
*/
GP_API void gpMacCore_AssociateRequest(UInt8 logicalChannel, gpMacCore_AddressInfo_t* pCoordAddrInfo, UInt8 capabilityInformation, gpMacCore_StackId_t stackId);
#endif // defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR)

#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)
/** @ingroup MLME_SAP
 *
 *  @brief  This function is used to send an associate response command packet to a device that is trying to join. The associate response command packet contains the results of the join procedure.
 *
 *  This function is used to respond to a previously received associate request command packet. Upon reception of an associate request command packet, the AssociateIndication function is called to inform the higher layer. The next higher layer then decides to allow the joining device in the network or not. Its decision is sent back to the joining device through the associate response packet.
 *
 *  @param pDeviceAddress           A pointer to the address of the device that is trying to join.
 *  @param associateShortAddress    If the joining device is allowed (successful association), this parameter contains the short address that was allocated for the device. If the association was unsuccessful, this parameter is set to 0xffff.
 *  @param status                   This parameter contains the result of the association attempt. Possible values are:
 *                                  0x0: Association successful.
 *                                  0x1: PAN at capacity.
 *                                  0x2: PAN access denied.
 *  @param stackId                  The identifier of the stack doing this response
*/
GP_API void gpMacCore_AssociateResponse(MACAddress_t *pDeviceAddress, UInt16 associateShortAddress, gpMacCore_Result_t status, gpMacCore_StackId_t stackId);
#endif // defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)

#if defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR)
/** @ingroup MLME_SAP
 *
 * @brief This function is used to poll a coordinator for data by sending a data request command. If the coordinator has data pending for the device that issued the poll request, it will forward the data.
 *
 *  This function starts a poll. This function is used to request packets that are pending on his coordinator (by sending a data request command to the coordinator).
 *  If the coordinator has data pending for the device that issued the poll request, it will forward the data.
 *  If a short address is allocated to the device, and it was written into the gpMacCore_AttributeShortAddress PIB attribute,
 *  the short address will be used as source address of the data request command.
 *  If the gpMacCore_AttributeShortAddress PIB attribute is set to 0xffff or 0xfffe, the device will use its extended address as source address in the data request command packet.
 *
 *  @param coordAddrInfo        All the details of the coordinator address
 *  @param stackId              The identifier of the stack doing this request
*/
GP_API void gpMacCore_PollRequest(gpMacCore_AddressInfo_t* coordAddrInfo, gpMacCore_Security_t *pSecOptions, gpMacCore_StackId_t stackId);
#endif /* defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR) */

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */
/** @brief Calls the PollConfirm function. The PollConfirm inform the next higher layer about a previously issued PollRequest.
 *
 *  This function returns the status of a previously issued PollRequest.
 *
 *  @param status       The status of the pollrequest. If success, the received data will be returned using the DataIndication function.
 *  @param coordAddrInfo        All the details of the coordinator address
*/
void gpMacCore_cbPollConfirm( gpMacCore_Result_t status, gpMacCore_AddressInfo_t* coordAddrInfo, gpPd_TimeStamp_t txTime );

/** @brief This function is used to start a new superframe specification. As this is a non-beacon enabled MAC, it is mostly used when starting a coordinator.
 *
 *  This function is used to start a new superframe specification. As this is a non-beacon enabled MAC, it is mostly used when starting a coordinator. It will set the values for the superframe.
 *
 *  @param panId                             Sets the panId that will be used.
 *  @param logicalChannel                    Sets the logicalChannel (the channel the network will operate on).
 *  @param panCoordinator                    Sets panCoordinator TRUE or FALSE. If the device is a PAN coordinator, this parameter must be set to TRUE.
 *  @param stackId                           The identifier of the stack doing this request
*/
GP_API gpMacCore_Result_t gpMacCore_Start(gpMacCore_PanId_t panId, UInt8 logicalChannel, Bool panCoordinator, gpMacCore_StackId_t stackId);

/** @brief This function set the current channel.
 *  @param channel  The channel to set.
 *  @param stackId  The identifier of the stack doing this
*/
#define gpMacCore_SetCurrentChannel(channel, stackId) gpMacCore_SetCurrentChannel_STACKID(MACCORE_STACKID_MAP_2(channel,stackId))
GP_API void gpMacCore_SetCurrentChannel_STACKID(UInt8 channel MACCORE_STACKID_ARG_2);

/** @brief This function set the 3 Rx channels.
 *  @param pChannel  The pointer of channel list.
*/
#define gpMacCore_SetMultipleChannelTable(pChannel, stackId) gpMacCore_SetMultipleChannelTable_STACKID(MACCORE_STACKID_MAP_2(pChannel,stackId))
GP_API void gpMacCore_SetMultipleChannelTable_STACKID(UInt8* pChannel MACCORE_STACKID_ARG_2);

/** @brief This function returns the current channel.
*/
#define gpMacCore_GetCurrentChannel(stackId) gpMacCore_GetCurrentChannel_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetCurrentChannel_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This functions set the default transmit power for each channel
 *
 *  @param Pointer to 16 byte array with default transmit power (in dBm) for each IEEE channel (11..26).
 *
*/
GP_API void gpMacCore_SetDefaultTransmitPowers(Int8* pDefaultTransmitPowerTable);

/** @brief This function set the transmission power.
 *  @param transmitPower The transmission power to set in dBm, or one of gpMacCore_TxPowerMin, TxPowerMax, TxPowerDefault.
 *  @param stackId       The identifier of the stack doing this
*/
#define gpMacCore_SetTransmitPower(transmitPower, stackId) gpMacCore_SetTransmitPower_STACKID(MACCORE_STACKID_MAP_2(transmitPower,stackId))
GP_API void gpMacCore_SetTransmitPower_STACKID(gpMacCore_TxPower_t transmitPower MACCORE_STACKID_ARG_2);

/** @brief This function returns the transmission power.
*/
#define gpMacCore_GetTransmitPower(stackId) gpMacCore_GetTransmitPower_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API Int8 gpMacCore_GetTransmitPower_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function set the CCA mode.
 *  @param cCAMode The CCA mode to set.
 *  @param stackId The identifier of the stack doing this request
*/
#define gpMacCore_SetCCAMode(cCAMode, stackId) gpMacCore_SetCCAMode_STACKID(MACCORE_STACKID_MAP_2(cCAMode,stackId))
GP_API void gpMacCore_SetCCAMode_STACKID(UInt8 cCAMode MACCORE_STACKID_ARG_2);

/** @brief This function returns the CCA mode.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetCCAMode(stackId) gpMacCore_GetCCAMode_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetCCAMode_STACKID(MACCORE_STACKID_ARG_1);
/** @brief This function sets the coordinator's extended MAC address.
 *  @param pCoordExtendedAddress A pointer to the address to set.
 *  @param stackId         The identifier of the stack doing this request
*/
#define gpMacCore_SetCoordExtendedAddress(pCoordExtendedAddress, stackId) gpMacCore_SetCoordExtendedAddress_STACKID(MACCORE_STACKID_MAP_2(pCoordExtendedAddress ,stackId))
GP_API void gpMacCore_SetCoordExtendedAddress_STACKID( MACAddress_t* pCoordExtendedAddress  MACCORE_STACKID_ARG_2);
/** @brief This function returns the coordinator's extended MAC address.
 *  @param pCoordExtendedAddress A pointer to the address to read.
 *  @param stackId         The identifier of the stack doing this request
*/
#define gpMacCore_GetCoordExtendedAddress(pCoordExtendedAddress , stackId) gpMacCore_GetCoordExtendedAddress_STACKID(MACCORE_STACKID_MAP_2(pCoordExtendedAddress ,stackId))
GP_API void gpMacCore_GetCoordExtendedAddress_STACKID( MACAddress_t* pCoordExtendedAddress  MACCORE_STACKID_ARG_2);

/** @brief This function sets the coordinator's short MAC address.
 *  @param coordShortAddress    A pointer to the address to set.
 *  @param stackId              The identifier of the stack doing this
*/
#define gpMacCore_SetCoordShortAddress(coordShortAddress, stackId) gpMacCore_SetCoordShortAddress_STACKID(MACCORE_STACKID_MAP_2(coordShortAddress,stackId))
GP_API void gpMacCore_SetCoordShortAddress_STACKID(UInt16 coordShortAddress MACCORE_STACKID_ARG_2);

/** @brief This function returns the coordinator's short MAC address.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetCoordShortAddress(stackId) gpMacCore_GetCoordShortAddress_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt16 gpMacCore_GetCoordShortAddress_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the panCoordinator property for a stack.
 *  @param panCoordinator   True or false, if the stack is pan coordinator or not.
 *  @param stackId          The identifier of the stack doing this
*/
#define gpMacCore_SetPanCoordinator(panCoordinator, stackId) gpMacCore_SetPanCoordinator_STACKID(MACCORE_STACKID_MAP_2(panCoordinator,stackId))
GP_API void gpMacCore_SetPanCoordinator_STACKID(Bool panCoordinator MACCORE_STACKID_ARG_2);
/** @brief This function returns the panCoordinator property of a stack.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetPanCoordinator(stackId) gpMacCore_GetPanCoordinator_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API Bool gpMacCore_GetPanCoordinator_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the MAC DSN.
 *  @param DSN          The DSN walue to set.
 *  @param stackId      The identifier of the stack doing this
*/
#define gpMacCore_SetDsn(dsn, stackId) gpMacCore_SetDsn_STACKID(MACCORE_STACKID_MAP_2(dsn,stackId))
GP_API void gpMacCore_SetDsn_STACKID(UInt8 dsn MACCORE_STACKID_ARG_2);
/** @brief This function returns the MAC DSN.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetDsn(stackId) gpMacCore_GetDsn_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetDsn_STACKID(MACCORE_STACKID_ARG_1);
/** @brief This function sets the maximum CSMA backoffs.
 *  @param maxCsmaBackoffs The maximum CSMA backoffs to set.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetMaxCsmaBackoffs(maxCsmaBackoffs, stackId) gpMacCore_SetMaxCsmaBackoffs_STACKID(MACCORE_STACKID_MAP_2(maxCsmaBackoffs,stackId))
GP_API void gpMacCore_SetMaxCsmaBackoffs_STACKID(UInt8 maxCsmaBackoffs MACCORE_STACKID_ARG_2);

/** @brief This function returns the maximum CSMA backoffs.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetMaxCsmaBackoffs(stackId) gpMacCore_GetMaxCsmaBackoffs_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetMaxCsmaBackoffs_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the minimun BE.
 *  @param minBE    The minimum BE to set.
 *  @param stackId  The identifier of the stack doing this
*/
#define gpMacCore_SetMinBE(minBE, stackId) gpMacCore_SetMinBE_STACKID(MACCORE_STACKID_MAP_2(minBE,stackId))
GP_API void gpMacCore_SetMinBE_STACKID(UInt8 minBE MACCORE_STACKID_ARG_2);

/** @brief This function returns the minimun BE.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetMinBE(stackId) gpMacCore_GetMinBE_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetMinBE_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the minimun BE.
 *  @param minBE    The minimum BE to set.
 *  @param stackId  The identifier of the stack doing this
*/
#define gpMacCore_SetMaxBE(maxBE, stackId) gpMacCore_SetMaxBE_STACKID(MACCORE_STACKID_MAP_2(maxBE,stackId))
GP_API void gpMacCore_SetMaxBE_STACKID(UInt8 maxBE MACCORE_STACKID_ARG_2);

/** @brief This function returns the minimun BE.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetMaxBE(stackId) gpMacCore_GetMaxBE_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetMaxBE_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the CSMA mode used when transmitting
 *  @param csmaMode    The CSMA mode to set.
 *  @param stackId     The identifier of the stack doing this
*/
#define gpMacCore_SetCsmaMode(csmaMode, stackId) gpMacCore_SetCsmaMode_STACKID(MACCORE_STACKID_MAP_2(csmaMode,stackId))
GP_API void gpMacCore_SetCsmaMode_STACKID(UInt8 csmaMode MACCORE_STACKID_ARG_2);

/** @brief This function returns the CSMA mode.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetCsmaMode(stackId) gpMacCore_GetCsmaMode_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetCsmaMode_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the PANID.
 *  @param panId The PANID to set.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetPanId(panId, stackId) gpMacCore_SetPanId_STACKID(MACCORE_STACKID_MAP_2(panId,stackId))
GP_API void gpMacCore_SetPanId_STACKID(UInt16 panId MACCORE_STACKID_ARG_2);

/** @brief This function returns the PANID.
*/
#define gpMacCore_GetPanId(stackId) gpMacCore_GetPanId_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt16 gpMacCore_GetPanId_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the RxOnWhenIdle attribute.
 *  @param rxOnWhenIdle The RxOnWhenIdle attribute to set.
 *  @param stackId         The identifier of the stack doing this
*/

#define gpMacCore_SetRxOnWhenIdle(rxOnWhenIdle, stackId) gpMacCore_SetRxOnWhenIdle_STACKID(MACCORE_STACKID_MAP_2(rxOnWhenIdle,stackId))
GP_API void gpMacCore_SetRxOnWhenIdle_STACKID(Bool rxOnWhenIdle MACCORE_STACKID_ARG_2);

/** @brief This function returns the RxOnWhenIdle attribute.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetRxOnWhenIdle(stackId) gpMacCore_GetRxOnWhenIdle_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API Bool gpMacCore_GetRxOnWhenIdle_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the short MAC address of the device.
 *  @param shortAddress    The address to set.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetShortAddress(shortAddress, stackId) gpMacCore_SetShortAddress_STACKID(MACCORE_STACKID_MAP_2(shortAddress,stackId))
GP_API void gpMacCore_SetShortAddress_STACKID(UInt16 shortAddress MACCORE_STACKID_ARG_2);

/** @brief This function returns the short MAC address of the device.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetShortAddress(stackId) gpMacCore_GetShortAddress_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt16 gpMacCore_GetShortAddress_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the association permit attribute.
 *  @param associationPermit The association permit value to set.
*/
#define gpMacCore_SetAssociationPermit(associationPermit, stackId) gpMacCore_SetAssociationPermit_STACKID(MACCORE_STACKID_MAP_2(associationPermit,stackId))
GP_API void gpMacCore_SetAssociationPermit_STACKID(Bool associationPermit MACCORE_STACKID_ARG_2);

/** @brief This function returns the association permit attribute.
*/
#define gpMacCore_GetAssociationPermit(stackId) gpMacCore_GetAssociationPermit_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API Bool gpMacCore_GetAssociationPermit_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the beacon payload pointer.
 *  @param pBeaconPayload The beacon payload pointer to set.
*/
#define gpMacCore_SetBeaconPayload( pBeaconPayload, stackId) gpMacCore_SetBeaconPayload_STACKID(MACCORE_STACKID_MAP_2(pBeaconPayload,stackId))
GP_API void gpMacCore_SetBeaconPayload_STACKID(UInt8* pBeaconPayload MACCORE_STACKID_ARG_2);

/** @brief This function returns the beacon payload pointer.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetBeaconPayload(pBeaconPayload, stackId)  gpMacCore_GetBeaconPayload_STACKID(MACCORE_STACKID_MAP_2(pBeaconPayload,stackId))
GP_API void gpMacCore_GetBeaconPayload_STACKID(UInt8* pBeaconPayload MACCORE_STACKID_ARG_2);

/** @brief This function sets the beacon payload length.
 *  @param beaconPayloadLength The beacon payload length to set.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetBeaconPayloadLength(beaconPayloadLength, stackId) gpMacCore_SetBeaconPayloadLength_STACKID(MACCORE_STACKID_MAP_2(beaconPayloadLength,stackId))
GP_API void gpMacCore_SetBeaconPayloadLength_STACKID(UInt8 beaconPayloadLength MACCORE_STACKID_ARG_2);

/** @brief This function returns the beacon payload length.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetBeaconPayloadLength(stackId) gpMacCore_GetBeaconPayloadLength_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetBeaconPayloadLength_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the promiscuous mode.
 *  @param promiscuousMode The promiscuous mode value to set.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetPromiscuousMode(promiscuousMode, stackId) gpMacCore_SetPromiscuousMode_STACKID(MACCORE_STACKID_MAP_2(promiscuousMode,stackId))
GP_API void gpMacCore_SetPromiscuousMode_STACKID(UInt8 promiscuousMode MACCORE_STACKID_ARG_2);

/** @brief This function returns the promiscuous mode.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetPromiscuousMode(stackId) gpMacCore_GetPromiscuousMode_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetPromiscuousMode_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the transaction persistence time.
 *  @param transactionPersistentTime The transaction persistence time to set.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetTransactionPersistenceTime(transactionPersistentTime, stackId) gpMacCore_SetTransactionPersistenceTime_STACKID(MACCORE_STACKID_MAP_2(transactionPersistentTime,stackId))
GP_API void gpMacCore_SetTransactionPersistenceTime_STACKID(UInt16 transactionPersistentTime MACCORE_STACKID_ARG_2);

/** @brief This function returns the transaction persistence time.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetTransactionPersistenceTime(stackId) gpMacCore_GetTransactionPersistenceTime_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt16 gpMacCore_GetTransactionPersistenceTime_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function sets the extended MAC address of the device.
 *  @param pExtendedAddress The pointer to the address to set.
*/
#define gpMacCore_SetExtendedAddress(pExtendedAddress, stackId) gpMacCore_SetExtendedAddress_STACKID(MACCORE_STACKID_MAP_2(pExtendedAddress,stackId))
GP_API void gpMacCore_SetExtendedAddress_STACKID(MACAddress_t* pExtendedAddress MACCORE_STACKID_ARG_2);

/** @brief This function returns the extended MAC address of the device.
 *  @param pExtendedAddress The pointer to the address to read.
*/
#define gpMacCore_GetExtendedAddress(pExtendedAddress, stackId) gpMacCore_GetExtendedAddress_STACKID(MACCORE_STACKID_MAP_2(pExtendedAddress,stackId))
GP_API void gpMacCore_GetExtendedAddress_STACKID(MACAddress_t* pExtendedAddress MACCORE_STACKID_ARG_2);

/** @brief This function sets the number of transmission retries.
 *  @param numberOfRetries The number of transmission retries to set.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetNumberOfRetries(numberOfRetries, stackId) gpMacCore_SetNumberOfRetries_STACKID(MACCORE_STACKID_MAP_2(numberOfRetries,stackId))
GP_API void gpMacCore_SetNumberOfRetries_STACKID(UInt8 numberOfRetries MACCORE_STACKID_ARG_2);

/** @brief This function sets the number of transmission retries.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetNumberOfRetries(stackId) gpMacCore_GetNumberOfRetries_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetNumberOfRetries_STACKID(MACCORE_STACKID_ARG_1);

// THDP ?? add to the setget struct
#define gpMacCore_SetSecurityEnabled(securityEnabled, stackId) gpMacCore_SetSecurityEnabled_STACKID(MACCORE_STACKID_MAP_2(securityEnabled,stackId))
GP_API void gpMacCore_SetSecurityEnabled_STACKID(Bool securityEnabled MACCORE_STACKID_ARG_2);

#define gpMacCore_GetSecurityEnabled(stackId) gpMacCore_GetSecurityEnabled_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API Bool gpMacCore_GetSecurityEnabled_STACKID(MACCORE_STACKID_ARG_1);


/** @brief This function sets the BeaconStarted attribute.
 *  @param BeaconStarted   The BeaconStarted attribute value to set.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetBeaconStarted(BeaconStarted, stackId) gpMacCore_SetBeaconStarted_STACKID(MACCORE_STACKID_MAP_2(BeaconStarted, stackId))
void gpMacCore_SetBeaconStarted_STACKID(Bool BeaconStarted MACCORE_STACKID_ARG_2);
/** @brief This function returns the BeaconStarted attribute.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetBeaconStarted(stackId) gpMacCore_GetBeaconStarted_STACKID(MACCORE_STACKID_MAP_1(stackId))
Bool gpMacCore_GetBeaconStarted_STACKID(MACCORE_STACKID_ARG_1);
/** @brief This function sets the tx antenna.
 *  @param txAntenna       The tx antenna to be used.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetTxAntenna(txAntenna, stackId) gpMacCore_SetTxAntenna_STACKID(MACCORE_STACKID_MAP_2(txAntenna,stackId))
GP_API void gpMacCore_SetTxAntenna_STACKID(UInt8 txAntenna MACCORE_STACKID_ARG_2);
/** @brief This function gets the tx antenna.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetTxAntenna(stackId) gpMacCore_GetTxAntenna_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetTxAntenna_STACKID(MACCORE_STACKID_ARG_1);
/** @brief This function sets the mac version.
 *  @param MacVersion      The mac version of the stack.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetMacVersion(macVersion, stackId) gpMacCore_SetMacVersion_STACKID(MACCORE_STACKID_MAP_2(macVersion,stackId))
GP_API void gpMacCore_SetMacVersion_STACKID(gpMacCore_MacVersion_t macVersion MACCORE_STACKID_ARG_2);
/** @brief This function gets the mac version.
 *  @param stackId         The identifier of the stack doing this.
*/
#define gpMacCore_GetMacVersion(stackId) gpMacCore_GetMacVersion_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API gpMacCore_MacVersion_t gpMacCore_GetMacVersion_STACKID(MACCORE_STACKID_ARG_1);

/** @brief gpMacCore_SetIndicateBeaconNotifications.
 *
 *  This function manipulates the indication of beacons by the MAC
 *
 *  @param enable
 *  @note When enable is true, beacons will be indicated by the MAC to the specific stack
 *  @param stackId         The identifier of the stack doing this.
*/
#define gpMacCore_SetIndicateBeaconNotifications(enable, stackId) gpMacCore_SetIndicateBeaconNotifications_STACKID(MACCORE_STACKID_MAP_2(enable,stackId))
GP_API void gpMacCore_SetIndicateBeaconNotifications_STACKID(Bool enable MACCORE_STACKID_ARG_2);

/** @brief gpMacCore_GetIndicateBeaconNotifications.
 *
 *  This function returns IndicateBeaconNotifications
 *  @param stackId         The identifier of the stack doing this.
*/
#define gpMacCore_GetIndicateBeaconNotifications(stackId) gpMacCore_GetIndicateBeaconNotifications_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API Bool gpMacCore_GetIndicateBeaconNotifications_STACKID(MACCORE_STACKID_ARG_1);

/** @brief gpMacCore_SetForwardPollIndication.
 *
 *  This function manipulates the poll indication by the MAC
 *
 *  @param enable
 *  @note When enable is true, poll indication will be forwarded by the MAC to the specific stack
 *  @param stackId         The identifier of the stack doing this.
*/
#define gpMacCore_SetForwardPollIndications(enable, stackId) gpMacCore_SetForwardPollIndications_STACKID(MACCORE_STACKID_MAP_2(enable,stackId))
GP_API void gpMacCore_SetForwardPollIndications_STACKID(Bool enable MACCORE_STACKID_ARG_2);

/** @brief gpMacCore_GetForwardPollIndications.
 *
 *  This function returns the parameter ForwardPollIndications
 *  @param stackId         The identifier of the stack doing this.
*/
#define gpMacCore_GetForwardPollIndications(stackId) gpMacCore_GetForwardPollIndications_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API Bool gpMacCore_GetForwardPollIndications_STACKID(MACCORE_STACKID_ARG_1);



#define gpMacCore_SetLqiThresholdForTest(lqiThreshold, stackId) gpMacCore_SetLqiThresholdForTest_STACKID(MACCORE_STACKID_MAP_2(lqiThreshold, stackId))
GP_API void gpMacCore_SetLqiThresholdForTest_STACKID(UInt8 lqiThreshold MACCORE_STACKID_ARG_2);

#define gpMacCore_GetLqiThresholdForTest(stackId) gpMacCore_GetLqiThresholdForTest_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetLqiThresholdForTest_STACKID(MACCORE_STACKID_ARG_1);

#define gpMacCore_SetAddressModeOverrideForBeacons(addressMode, stackId) gpMacCore_SetAddressModeOverrideForBeacons_STACKID(MACCORE_STACKID_MAP_2(addressMode, stackId))
GP_API void gpMacCore_SetAddressModeOverrideForBeacons_STACKID(UInt8 addressMode MACCORE_STACKID_ARG_2);

#define gpMacCore_GetAddressModeOverrideForBeacons(stackId) gpMacCore_GetAddressModeOverrideForBeacons_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API UInt8 gpMacCore_GetAddressModeOverrideForBeacons_STACKID(MACCORE_STACKID_ARG_1);

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */
/** @brief Calls the DataConfirm callback function. It is used to indicate the result of a data transmission to the next higher layer.
 *
 *  This function is the Confirm of a data Request. It gives info of the success or failure of the dataRequest.
 *
 *  @param status       Status of the DataTransmission.
 *  @param pdHandle     The msduHandle that was given with the dataRequest is returned with the dataConfirm. This way, the next higher layer knows which packet generated the confirm.
*/
void gpMacCore_cbDataConfirm(gpMacCore_Result_t status, gpPd_Handle_t pdHandle);

/** @brief Calls the DataIndication callback function. It is used to inform the next higher layer a data packet is received.
 *
 *  This function indicates (to the next higher layer) that a data packet is received.
 *
 *  It has more or less the same parameters than the type gpMacCore_cbCPSDataIndication_t,
 *  except that is has one additional parameter handle and that the parameter pMsdu contains the address
 *  in the PBM memory where the data payload is located.
 *
 *  This function is only used, when the PBM version of the MAC is used.
 *
 *  @param pSrcAddrInfo     All the information about the source (address mode, address and panId).
 *  @param pDstAddrInfo     All the information about the destination (address mode, address and panId).
 *  @param DSN              The data sequence number of the received data frame.
 *  @param *pSecOptions     The security options purportedly used by the received frame (see IEEE802.15.4-2006).
 *  @param p_PdLoh          The PD Length/Offset/Handle of the mpdu.
 *  @param stackId          The stack Id this indication is ment for
 *
*/
void gpMacCore_cbDataIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 dsn, gpMacCore_Security_t *pSecOptions, gpPd_Loh_t pdLoh, gpMacCore_StackId_t stackId );


 /** @brief This function allows the MAC sublayer to notify the next higher layer of the
 * success of its request to purge a packet from the transaction queue.
 *
 *  @param status       Status of the PurgeRequest.
 *  @param pdHandle     The msduHandle that was given with the dataRequest.
*/
void gpMacCore_cbPurgeConfirm(gpMacCore_Result_t status, gpPd_Handle_t pdHandle);

/** @ingroup MLME_SAP
 *  @brief Calls the AssociateIndication function to indicate a node is trying to join.
 *
 *  This function informs the next higher layer that an associate request command was received, indicating a device wishes to associate.
 *
 *  @param pDeviceAddress          A pointer to the address of the device that requested the association. This will always be an extended (64 bit) address.
 *  @param capabilityInformation   The operational capabilities of the device requesting association as specified in the IEEE802.15.4-2006 specification.
 *  @param stackId                 The stack Id this indication is ment for
 *  @param rxTime                  reception time of this message
*/
void gpMacCore_cbAssociateIndication(gpMacCore_Address_t* pDeviceAddress, UInt8 capabilityInformation, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t rxTime);

/** @ingroup MLME_SAP
 *  @brief Calls the AssociateConfirm function, indicating the result of a previous issued AssociateRequest.
 *
 *  This function is used to inform a device of its attempt to join a network. It indicates the results (joined or not) of a previous issued AssociateRequest.
 *
 *  @param assocShortAddress       The short address the device may use if the association was successful.
 *  @param status                  The status of the association attempt. If this indicates a success, the assocShortAddress field contains the short address the device may use.
 *  @param txTime                  transmission time of this message
*/
void gpMacCore_cbAssociateConfirm(UInt16 assocShortAddress, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);

/** @ingroup MLME_SAP
 *  @brief Indicates a associate response communication status to the next higher layer.
 *
 *  This function indication primitive allows the MLME to indicate a communications status.
 *
 *  @param pSrcAddrInfo            The src address information of the device.
 *  @param pDstAddrInfo            The intended recipient of the message
 *  @param status                  The communications status.
 *  @param stackId                 The stack Id this indication is ment for
 *  @param rxTime                  reception time of this message
*/
void gpMacCore_cbAssociateCommStatusIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t txTime);

/** @ingroup MLME_SAP
 *  @brief Indicates a communications status to the next higher layer.
 *
 *  This function indication primitive allows the MLME to indicate a communications status.
 *
 *  @param pSrcAddrInfo            The src address information of the device.
 *  @param pDstAddrInfo            The intended recipient of the message
 *  @param status                  The communications status.
 *  @param stackId                 The stack Id this indication is ment for
 *  @param rxTime                  reception time of this message
*/
void gpMacCore_cbOrphanCommStatusIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t txTime);

/** @ingroup MLME_SAP
 *  @brief Indicates a security failure communication status to the next higher layer.
 *
 *  This function indication primitive allows the MLME to indicate a communications status.
 *
 *  @param pSrcAddrInfo            The src address information of the device.
 *  @param pDstAddrInfo            The intended recipient of the message
 *  @param status                  The communications status.
 *  @param stackId                 The stack Id this indication is ment for
 *  @param [t|r]xTime              transmission/reception time of this message
*/
void gpMacCore_cbSecurityFailureCommStatusIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t txTime);


/** @ingroup MLME_SAP
 *  @brief Calls the BeaconNotifyIndication function. Informs the next higher layer a beacon packet containing a payload has been received.
 *
 *  This function indicates a beacon with payload is received.
 *
 *  @param bsn                  The sequence number of the beacon.
 *  @param pPanDescriptor       A pointer to the PANDescriptor of the beacon, the values of this pointer should not be changed.
 *  @param stackId              The stack Id this indication is ment for
 *  @param beaconPayloadLength  The length of the beacon payload
 *  @param pBeaconPayload       A pointer to the beacon payload, the values of this pointer should not be changed.
*/
void gpMacCore_cbBeaconNotifyIndication( UInt8 bsn, gpMacCore_PanDescriptor_t* pPanDescriptor, gpMacCore_StackId_t stackId , UInt8 beaconPayloadLength , UInt8 *pBeaconPayload );

/** @brief Calls the ScanConfirm function, indicating the result of a previously issued ScanRequest to the next higher layer.
 *
 *  This function indicates the result of a previously issued scan to the next higher layer.
 *
 *  @param status               The result of the scan.
 *  @param scanType             The scanType that was performed.
 *  @param unscannedChannels    A bitmask of the channels that were not scanned.
 *  @param resultListSize       The size of the result list (For additional info see the ScanRequest function).
 *  @param resultList           A pointer to the result List.
*/
void gpMacCore_cbScanConfirm(gpMacCore_Result_t status, gpMacCore_ScanType_t scanType, UInt32 unscannedChannels, UInt8 resultListSize, UInt8* pEdScanResultList);

/** @brief Calls the OrphanIndication function. It is issued on a coordinator on receipt of an orphan notification command.
 *
 *  This function indicates a device is orphanned. It is issued on a coordinator on receipt of an orphan notification command.
 *
 *  @param pOrphanAddress   A pointer to the extended address of the orphaned device.
 *  @param stackId          The stack Id this indication is ment for
 *  @param rxTime           reception time of this message
*/
void gpMacCore_cbOrphanIndication(MACAddress_t* pOrphanAddress, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t rxTime);

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/** @brief Calls the OrphanResponse function.
 *
*/
void gpMacCore_OrphanResponse(MACAddress_t* pOrphanAddress, UInt16 shortAddress, Bool associatedMember, gpMacCore_StackId_t stackId);

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

/** @brief Calls the PollIndication function. It is only invoked when the gpMacCore_SetForwardPollIndications() is enabled.
 *
 *  This callback is only used when the upper layer implemnents its own data pending mechanism.
 *  All handling to send out the packet must be done by the higher layer upon receiving this callback.
 *
 *  @param addressMode   The address info used by the originator.
 *  @param stackId       The stack Id this indication is ment for
 *  @param rxTime        reception time of this message
*/
void gpMacCore_cbPollIndication(gpMacCore_AddressInfo_t *addressInfo, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t rxTime);

/** @brief Calls the PollNotify function. It is invoked when a data poll was received.
 *
 *  All data Tx is handled automatically by the MAC stack using the queued packets.
 *  This notification serves as bookkeepping information for the higher layer.
 *
 *  @param addressInfo   The address info used by the originator
 *  @param rxTime        Time at which the packet was received (in us)
 *  @param pdHandle      Handle of the queued packet that will be transmitted in response to this response. If no data was pending, 0xFF is returned here.
 *  @param fromNeighbour True if the poll came from a device specified in the Neighbour table list - see the gpMacCore_AddNeighbour() API calls.
 *  @param stackId       The stack Id this indication is ment for
*/
void gpMacCore_cbPollNotify(gpMacCore_AddressInfo_t *addressInfo, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t rxTime, gpPd_Handle_t pdHandle, Bool fromNeighbour);

/** @brief Calls the SecurityFrameCounterIndication function. It is invoked when the security framecounter window exceeds the previous value with GP_MACCORE_FRAMECOUNTER_WINDOW.
 *
 *  @param frameCounter  New frame counter value
 *  @param stackId       The stack Id this indication is ment for
*/
void gpMacCore_cbSecurityFrameCounterIndication(UInt32 frameCounter, gpMacCore_StackId_t stackId);


/** @brief Calls the stackIdValid callback function.
 *
 *  @param stackId      The stackId to check for validity
 */

GP_API Bool gpMacCore_cbValidStack(gpMacCore_StackId_t stackId);

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
/** @brief Indicates stack adding or removal to the gpMacCore component
 *
 *  @param stackId      The stackId that was added or removed
 */
/*  JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */
GP_API void gpMacCore_StackAdded(gpMacCore_StackId_t stackId);
GP_API void gpMacCore_StackRemoved(gpMacCore_StackId_t stackId);
/*  JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */


/** @brief This function Adds element to DataPending Queue
 *
 *  @param element
*/
GP_API gpMacCore_Result_t gpMacCore_DataPending_QueueAdd( gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId);


/** @brief This function Removes element to DataPending Queue
 *
 *  @param element
*/
GP_API gpMacCore_Result_t gpMacCore_DataPending_QueueRemove( gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId);

/** @brief This function Removes all elements from the DataPending Queue
 *
 *  @param stackId      The stackId for which the dataPending queue needs to be cleared.
*/
GP_API gpMacCore_Result_t gpMacCore_DataPending_QueueClear(gpMacCore_StackId_t stackId);

/** @brief This function returns if the Data pending bit will be set in the next ACK
 *
 *  @return dataPending
*/
GP_API Bool gpMacCore_DataPendingGetAckDataPending(void);

/** @brief This function sets interference level masks to allow operation alongside WiFi networks
 *
 *  @return void
*/
gpMacCore_Result_t gpMacCore_SetMinInterferenceLevels(Int8* pInterferenceLevels);

/** @brief This function adds an address to the neighbour list (determines how to handle a poll request).
 *
 *  @return Bool indicating success or fail
*/
GP_API Bool gpMacCore_AddNeighbour   (gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId);

/** @brief This function removes an address to the neighbour list (determines how to handle a poll request).
 *
 *  @return Bool indicating success or fail
*/
GP_API Bool gpMacCore_RemoveNeighbour(gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId);

/** @brief This function sets the mode to be used for datapending.
 *
 * @param dataPendingMode   mode from gpMacCore_DataPendingMode_t enumeration to set.
 *
*/
GP_API gpMacCore_Result_t gpMacCore_SetDataPendingMode(gpMacCore_DataPendingMode_t dataPendingMode, gpMacCore_StackId_t stackId);

/** @brief This function removes all neighbours from neighbour list.
 *
*/
GP_API void gpMacCore_ClearNeighbours(gpMacCore_StackId_t stackId);

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */



#ifdef GP_MACCORE_DIVERSITY_RAW_FRAMES
/** @brief This function enables the raw mode of the stack.
 *
 * This mode allows the upper stack to create the MAC header while transmitting a packets,
 * and to stop the MacCore from processing MAC headers, and instead passing them to the upper stack.
 *
 *  @param rawModeEnabled   Enables or disables the raw mode for a specific stack.
*/
#define gpMacCore_SetStackInRawMode(rawModeEnabled, stackId) gpMacCore_SetStackInRawMode_STACKID(MACCORE_STACKID_MAP_2(rawModeEnabled, stackId))
GP_API void gpMacCore_SetStackInRawMode_STACKID(Bool rawModeEnabled MACCORE_STACKID_ARG_2);

/** @brief This function returns if the raw mode is enabled for a specific stack.
 *
 *  @return rawModeEnabled   Indicates if raw mode is enabled or disabled for a specific stack.
*/
#define gpMacCore_GetStackInRawMode(stackId) gpMacCore_GetStackInRawMode_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API Bool gpMacCore_GetStackInRawMode_STACKID(MACCORE_STACKID_ARG_1);

#define gpMacCore_SetRawModeEncryptionKeys(encryptionKeyIdMode, encryptionKeyId, pCurrKey, stackId) gpMacCore_SetRawModeEncryptionKeys_STACKID(MACCORE_STACKID_MAP_4(encryptionKeyIdMode, encryptionKeyId, pCurrKey, stackId))
GP_API void gpMacCore_SetRawModeEncryptionKeys_STACKID(gpMacCore_KeyIdMode_t encryptionKeyIdMode, gpMacCore_KeyIndex_t encryptionKeyId, UInt8* pCurrKey MACCORE_STACKID_ARG_2);

#define gpMacCore_SetRawModeNonceFields(frameCounter, pExtendedAddress, seclevel, stackId) gpMacCore_SetRawModeNonceFields_STACKID(MACCORE_STACKID_MAP_4(frameCounter, pExtendedAddress, seclevel, stackId))
GP_API void gpMacCore_SetRawModeNonceFields_STACKID(UInt32 frameCounter, MACAddress_t* pExtendedAddress, UInt8 seclevel MACCORE_STACKID_ARG_2);

/** @brief This function configures the insertion of a VS IE header in the Enh Ack frames containing probing data.
 *
 *  @param linkMetrics      Bitmask of link metrics which should be reported.
 *  @param pExtendedAddress The extended address of the probing initiator.
 *  @param shortAddress     The short address of the probing initiator.
 *  @param stackId          The stack id.
*/
#define gpMacCore_ConfigureEnhAckProbing(linkMetrics, pExtendedAddress, shortAddress, stackId) gpMacCore_ConfigureEnhAckProbing_STACKID(MACCORE_STACKID_MAP_4(linkMetrics, pExtendedAddress, shortAddress, stackId))
GP_API gpMacCore_Result_t gpMacCore_ConfigureEnhAckProbing_STACKID(UInt8 linkMetrics, MACAddress_t* pExtendedAddress, UInt16 shortAddress MACCORE_STACKID_ARG_2);

#endif //GP_MACCORE_DIVERSITY_RAW_FRAMES

/** @brief This function enables the Enhanced FramePending behavior for a specific stack.
 *
 *  @param enableEnhancedFramePending   Enables or disables the Enhanced FramePending behavior for a specific stack.
*/
#define gpMacCore_EnableEnhancedFramePending(enableEnhancedFramePending, stackId) gpMacCore_EnableEnhancedFramePending_STACKID(MACCORE_STACKID_MAP_2(enableEnhancedFramePending, stackId))
GP_API void gpMacCore_EnableEnhancedFramePending(Bool enableEnhancedFramePending, gpMacCore_StackId_t stackId);



/** @brief This function enables or disables the Retransmits on CCA failures.
 *
 * This function enables or disables the Retransmits on CCA failures. The actual amount of retransmits
 * is set through gpMacDispatcher_SetNumberOfRetries. As such this function only changes the
 * default MAC 802.15.4-2015 behavior to also retransmit on a CCA fail.
 *
 *  @param enable          Enables or disables the Retransmits.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetRetransmitOnCcaFail(enable, stackId) gpMacCore_SetRetransmitOnCcaFail_STACKID(MACCORE_STACKID_MAP_2(enable,stackId))
GP_API void gpMacCore_SetRetransmitOnCcaFail_STACKID(Bool enable MACCORE_STACKID_ARG_2);
/** @brief This function returns if the feature "Retransmits on CCA fail" is enabled for a stackId.
 *
 * This function returns if the feature "Retransmits on CCA fail" is enabled for a stackId.
 *
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetRetransmitOnCcaFail(stackId) gpMacCore_GetRetransmitOnCcaFail_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API Bool gpMacCore_GetRetransmitOnCcaFail_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function enables or disables the random delays between subsequent retransmits.
 *
 * This function enables or disables the random delays between subsequent retransmits.
 * The actual amount of retransmits is set through gpMacDispatcher_SetNumberOfRetries. As such this
 * function only changes the default MAC 802.15.4-2015 behavior to add random delays between retransmits.
 *
 *  @param enable          Enables or disables the random delays between subsequent retransmits.
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_SetRetransmitRandomBackoff(enable, stackId) gpMacCore_SetRetransmitRandomBackoff_STACKID(MACCORE_STACKID_MAP_2(enable,stackId))
GP_API void gpMacCore_SetRetransmitRandomBackoff_STACKID(Bool enable MACCORE_STACKID_ARG_2);
/** @brief This function returns if the feature "random delays between subsequent retransmits" is enabled for a stackId.
 *
 * This function returns if the feature "random delays between subsequent retransmits" is enabled for a stackId.
 *
 *  @param stackId         The identifier of the stack doing this
*/
#define gpMacCore_GetRetransmitRandomBackoff(stackId) gpMacCore_GetRetransmitRandomBackoff_STACKID(MACCORE_STACKID_MAP_1(stackId))
GP_API Bool gpMacCore_GetRetransmitRandomBackoff_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function configures the minimum backoff exponent for random delays between retransmits.
 *
 * This function configures the minimum backoff exponent for random delays between retransmits due to NoAck failures.
 * See Thread v1.2.1 paragraph 3.2.7.4: Mac layer retries.
 * Not to be confused with the backoff exponent for random delays between CCA failures!
 *
 *  @param minBERetransmit The minimum backoff exponent.
 *  @param stackId         The identifier of the stack
*/
#define gpMacCore_SetMinBeRetransmit(minBERetransmit, stackId) gpMacCore_SetMinBeRetransmit_STACKID(MACCORE_STACKID_MAP_2(minBERetransmit,stackId))
void gpMacCore_SetMinBeRetransmit_STACKID(UInt8 minBERetransmit MACCORE_STACKID_ARG_2);

/** @brief This function returns the minimum backoff exponent for random delays between retransmits.
 *
 * This function returns the minimum backoff exponent for random delays between retransmits.
 *
 *  @param stackId         The identifier of the stack
*/
#define gpMacCore_GetMinBeRetransmit(stackId) gpMacCore_GetMinBeRetransmit_STACKID(MACCORE_STACKID_MAP_1(stackId))
UInt8 gpMacCore_GetMinBeRetransmit_STACKID(MACCORE_STACKID_ARG_1);

/** @brief This function configures the maximum backoff exponent for random delays between retransmits.
 *
 * This function configures the maximum backoff exponent for random delays between retransmits due to NoAck failures.
 * See Thread v1.2.1 paragraph 3.2.7.4: Mac layer retries.
 * Not to be confused with the backoff exponent for random delays between CCA failures!
 *
 *  @param maxBERetransmit The maximum backoff exponent.
 *  @param stackId         The identifier of the stack
*/
#define gpMacCore_SetMaxBeRetransmit(maxBERetransmit, stackId) gpMacCore_SetMaxBeRetransmit_STACKID(MACCORE_STACKID_MAP_2(maxBERetransmit,stackId))
void gpMacCore_SetMaxBeRetransmit_STACKID(UInt8 maxBERetransmit MACCORE_STACKID_ARG_2);

/** @brief This function returns the maximum backoff exponent for random delays between retransmits.
 *
 * This function returns the maximum backoff exponent for random delays between retransmits.
 *
 *  @param stackId         The identifier of the stack
*/
#define gpMacCore_GetMaxBeRetransmit(stackId) gpMacCore_GetMaxBeRetransmit_STACKID(MACCORE_STACKID_MAP_1(stackId))
UInt8 gpMacCore_GetMaxBeRetransmit_STACKID(MACCORE_STACKID_ARG_1);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif

#endif // _GP_MACCORE_H_

