/***************************************************************************//**
 * @file mac-header.h
 * @brief Definitions and macros of PHY and MAC packet headers.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef MAC_HEADER_H
#define MAC_HEADER_H

#include "mac-types.h"
#include "sl_status.h"
#include "buffer_manager/buffer-management.h"

uint16_t emberFetchLowHighInt16u(const uint8_t *contents);
void emberStoreLowHighInt16u(uint8_t *contents, uint16_t value);

uint32_t emFetchInt32u(bool lowHigh, const uint8_t* contents);
void emStoreInt32u(bool lowHigh, uint8_t* contents, uint32_t value);

#define emberFetchLowHighInt32u(contents) \
  (emFetchInt32u(true, contents))
#define emberStoreLowHighInt32u(contents, value) \
  (emStoreInt32u(true, contents, value))

// -----------------------------------------------------------------------------
// Connect types stuff

/**
 * @brief 802.15.4 addressing mode.
 */
typedef uint8_t EmberMacAddressMode;
enum {
  EMBER_MAC_ADDRESS_MODE_NONE     = 0x00,
  EMBER_MAC_ADDRESS_MODE_RESERVED = 0x01,
  EMBER_MAC_ADDRESS_MODE_SHORT    = 0x02,
  EMBER_MAC_ADDRESS_MODE_LONG     = 0x03,
};

/**
 * @brief A structure that stores an 802.15.4 address.
 */
typedef struct {
  union {
    uint8_t longAddress[EUI64_SIZE];
    uint16_t shortAddress;
  } addr;

  EmberMacAddressMode mode;
} EmberMacAddress;

/**
 * @brief Message options.
 */
typedef uint8_t EmberMessageOptions;
enum {
  /** No options. */
  EMBER_OPTIONS_NONE                     = 0x00,
  /** The packet should be sent out encrypted. */
  EMBER_OPTIONS_SECURITY_ENABLED         = 0x01,
  /** An acknowledgment should be requested for the outgoing packet. */
  EMBER_OPTIONS_ACK_REQUESTED            = 0x02,
  /** The packet should be sent with high priority. */
  EMBER_OPTIONS_HIGH_PRIORITY            = 0x04,
  /** The packet should be sent via the indirect queue. This option only applies
     to nodes with ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE
     node type. */
  EMBER_OPTIONS_INDIRECT                 = 0x08,
};

typedef struct {
  /**
   * An ::EmberMacAddress structure indicating the source address of a MAC frame.
   */
  EmberMacAddress srcAddress;
  /**
   * An ::EmberMacAddress structure indicating the destination address of a MAC
   * frame.
   */
  EmberMacAddress dstAddress;
  /**
   * An ::EmberPanId struct indicating the source PAN ID of a MAC frame. This
   * field is meaningful only if srcPanIdSpecified is set to true.
   */
  EmberPanId srcPanId;
  /**
   * An ::EmberPanId struct indicating the destination PAN ID of a MAC frame.
   * This field is meaningful only if dstPanIdSpecified is set to true.
   */
  EmberPanId dstPanId;
  /**
   * True if the srcPanId field is set, false otherwise.
   */
  bool srcPanIdSpecified;
  /**
   * True if the dstPanId field is set, false otherwise.
   */
  bool dstPanIdSpecified;
} EmberMacFrame;

/**
 * @brief An instance of this structure is passed to
 * ::emberIncomingMacMessageHandler(). It describes the incoming MAC frame.
 */
typedef struct {
  /**
   * An ::EmberMessageOptions value indicating the options used for the incoming
   * packet.
   */
  EmberMessageOptions options;
  /**
   * An ::EmberMacFrame structure indicating the source and destination
   * addresses and source and destination PAN IDs.
   */
  EmberMacFrame macFrame;
  /**
   * The RSSI in dBm the packet was received with.
   */
  int8_t rssi;
  /**
   * The LQI the packet was received with.
   */
  int8_t lqi;
  /**
   * The security MAC frame counter (if any).
   */
  uint32_t frameCounter;
  /**
   * An ::EmberMessageLength value indicating the length in bytes of the
   * MAC payload of the incoming message.
   */
  EmberMessageLength length;
  /**
   * A pointer to the message MAC payload.
   */
  uint8_t *payload;
} EmberIncomingMacMessage;

/**
 * @brief An instance of this structure is passed to
 * emberMacMessageSentHandler(). It describes the outgoing MAC frame.
 */
typedef struct {
  /**
   * An ::EmberMessageOptions value indicating the options used for transmitting
   * the outgoing message.
   */
  EmberMessageOptions options;
  /**
   * An ::EmberMacFrame struct indicating the source and destination addresses
   * and source and destination PAN IDs of the outgoing MAC frame.
   */
  EmberMacFrame macFrame;
  /**
   * A tag value the application can use to match ::emberMacMessageSend() calls
   * to the corresponding ::emberMacMessageSentHandler() calls.
   */
  uint8_t tag;
  /**
   * The security frame counter of the outgoing MAC frame (if any).
   */
  uint32_t frameCounter;
  /**
   * An ::EmberMessageLength value indicating the length in bytes of the
   * incoming message.
   */
  EmberMessageLength length;
  /**
   * A pointer to the message payload.
   */
  uint8_t *payload;
  /**
   * The RSSI in dBm of the ACK corresponding to this message. This field is
   * meaningful only if ::EMBER_OPTIONS_ACK_REQUESTED flag is set in the options
   * field.
   */
  int8_t ackRssi;
} EmberOutgoingMacMessage;

bool sli_mac_filter_match_check(uint8_t mac_index, uint8_t nwk_index, uint8_t* mac_header);
sl_status_t sl_mac_set_filter_match_list(bool custom_filter,
                                         const EmberMacFilterMatchData* mac_filter_match_list);

#ifndef EMBER_MULTI_NETWORK_STRIPPED
bool sl_mac_is_broadcast_pan(uint8_t *rawMacHeader, bool hasPhyHeader);
sl_status_t sl_mac_get_pan_id(uint8_t *rawMacHeader, bool hasPhyHeader, uint16_t *pan_id);
#endif // EMBER_MULTI_NETWORK_STRIPPED

//------------------------------------------------------------------------
// 802.15.4 Frame Control Field definitions for Beacon, Ack, Data, Command

#define MAC_FRAME_TYPE_MASK                    ((uint16_t)0x0007U) // Bits 0..2
#define MAC_FRAME_TYPE_BEACON                  ((uint16_t)0x0000U) // Beacon
#define MAC_FRAME_TYPE_DATA                    ((uint16_t)0x0001U) // Data
#define MAC_FRAME_TYPE_ACK                     ((uint16_t)0x0002U) // ACK
#define MAC_FRAME_TYPE_COMMAND                 ((uint16_t)0x0003U) // Command
#define MAC_FRAME_TYPE_CONTROL                 MAC_FRAME_TYPE_COMMAND // (synonym)
#define MAC_FRAME_TYPE_RESERVED_MASK           ((uint16_t)0x0004U) // Versions 0/1
// 802.15.4E-2012 introduced MultiPurpose with different Frame Control Field
// layout described in the MultiPurpose section below.
#define MAC_FRAME_TYPE_MULTIPURPOSE            ((uint16_t)0x0005U) // MultiPurpose

#define MAC_FRAME_FLAG_SECURITY_ENABLED        ((uint16_t)0x0008U) // Bit 3
#define MAC_FRAME_FLAG_FRAME_PENDING           ((uint16_t)0x0010U) // Bit 4
#define MAC_FRAME_FLAG_ACK_REQUIRED            ((uint16_t)0x0020U) // Bit 5
#define MAC_FRAME_FLAG_INTRA_PAN               ((uint16_t)0x0040U) // Bit 6
// 802.15.4-2006 renamed the Intra-Pan flag PanId-Compression
#define MAC_FRAME_FLAG_PANID_COMPRESSION       MAC_FRAME_FLAG_INTRA_PAN
#define MAC_FRAME_FLAG_RESERVED                ((uint16_t)0x0080U) // Bit 7 reserved
// Use the reserved flag internally for CCA inhibit
#define MAC_INHIBIT_CCA                        MAC_FRAME_FLAG_RESERVED
// 802.15.4E-2012 introduced these flags for Frame Version 2 frames
// which are reserved bit positions in earlier Frame Version frames:
#define MAC_FRAME_FLAG_SEQ_SUPPRESSION         ((uint16_t)0x0100U) // Bit 8
#define MAC_FRAME_FLAG_IE_LIST_PRESENT         ((uint16_t)0x0200U) // Bit 9

#define MAC_FRAME_DESTINATION_MODE_MASK        ((uint16_t)0x0C00U) // Bits 10..11
#define MAC_FRAME_DESTINATION_MODE_NONE        ((uint16_t)0x0000U) // Mode 0
#define MAC_FRAME_DESTINATION_MODE_RESERVED    ((uint16_t)0x0400U) // Mode 1
#define MAC_FRAME_DESTINATION_MODE_SHORT       ((uint16_t)0x0800U) // Mode 2
#define MAC_FRAME_DESTINATION_MODE_LONG        ((uint16_t)0x0C00U) // Mode 3
// 802.15.4e-2012 only (not adopted into 802.15.4-2015)
#define MAC_FRAME_DESTINATION_MODE_BYTE        MAC_FRAME_DESTINATION_MODE_RESERVED

#define MAC_FRAME_VERSION_MASK                 ((uint16_t)0x3000U) // Bits 12..13
#define MAC_FRAME_VERSION_2003                 ((uint16_t)0x0000U) // Version 0
#define MAC_FRAME_VERSION_2006                 ((uint16_t)0x1000U) // Version 1
// In 802.15.4-2015, Version 2 is just called "IEEE STD 802.15.4"
// which can be rather confusing. It was introduced in 802.15.4E-2012.
#define MAC_FRAME_VERSION_2012                 ((uint16_t)0x2000U) // Version 2
#define MAC_FRAME_VERSION_RESERVED             ((uint16_t)0x3000U) // Version 3

#define MAC_FRAME_SOURCE_MODE_MASK             ((uint16_t)0xC000U) // Bits 14..15
#define MAC_FRAME_SOURCE_MODE_NONE             ((uint16_t)0x0000U) // Mode 0
#define MAC_FRAME_SOURCE_MODE_RESERVED         ((uint16_t)0x4000U) // Mode 1
#define MAC_FRAME_SOURCE_MODE_SHORT            ((uint16_t)0x8000U) // Mode 2
#define MAC_FRAME_SOURCE_MODE_LONG             ((uint16_t)0xC000U) // Mode 3
// 802.15.4e-2012 only (not adopted into 802.15.4-2015)
#define MAC_FRAME_SOURCE_MODE_BYTE             MAC_FRAME_SOURCE_MODE_RESERVED

//------------------------------------------------------------------------
// 802.15.4E-2012 Frame Control Field definitions for MultiPurpose

#define MAC_MP_FRAME_TYPE_MASK                 MAC_FRAME_TYPE_MASK // Bits 0..2
#define MAC_MP_FRAME_TYPE_MULTIPURPOSE         MAC_FRAME_TYPE_MULTIPURPOSE

#define MAC_MP_FRAME_FLAG_LONG_FCF             ((uint16_t)0x0008U) // Bit 3

#define MAC_MP_FRAME_DESTINATION_MODE_MASK     ((uint16_t)0x0030U) // Bits 4..5
#define MAC_MP_FRAME_DESTINATION_MODE_NONE     ((uint16_t)0x0000U) // Mode 0
#define MAC_MP_FRAME_DESTINATION_MODE_RESERVED ((uint16_t)0x0010U) // Mode 1
#define MAC_MP_FRAME_DESTINATION_MODE_SHORT    ((uint16_t)0x0020U) // Mode 2
#define MAC_MP_FRAME_DESTINATION_MODE_LONG     ((uint16_t)0x0030U) // Mode 3
// 802.15.4e-2012 only (not adopted into 802.15.4-2015)
#define MAC_MP_FRAME_DESTINATION_MODE_BYTE     MAC_MP_FRAME_DESTINATION_MODE_RESERVED

#define MAC_MP_FRAME_SOURCE_MODE_MASK          ((uint16_t)0x00C0U) // Bits 6..7
#define MAC_MP_FRAME_SOURCE_MODE_NONE          ((uint16_t)0x0000U) // Mode 0
#define MAC_MP_FRAME_SOURCE_MODE_RESERVED      ((uint16_t)0x0040U) // Mode 1
#define MAC_MP_FRAME_SOURCE_MODE_SHORT         ((uint16_t)0x0080U) // Mode 2
#define MAC_MP_FRAME_SOURCE_MODE_LONG          ((uint16_t)0x00C0U) // Mode 3
// 802.15.4e-2012 only (not adopted into 802.15.4-2015)
#define MAC_MP_FRAME_SOURCE_MODE_BYTE          MAC_MP_FRAME_SOURCE_MODE_RESERVED

#define MAC_MP_FRAME_FLAG_PANID_PRESENT        ((uint16_t)0x0100U) // Bit 8
#define MAC_MP_FRAME_FLAG_SECURITY_ENABLED     ((uint16_t)0x0200U) // Bit 9
#define MAC_MP_FRAME_FLAG_SEQ_SUPPRESSION      ((uint16_t)0x0400U) // Bit 10
#define MAC_MP_FRAME_FLAG_FRAME_PENDING        ((uint16_t)0x0800U) // Bit 11

#define MAC_MP_FRAME_VERSION_MASK              MAC_FRAME_VERSION_MASK // Bits 12..13
#define MAC_MP_FRAME_VERSION_2012              ((uint16_t)0x0000U) // Version 0
// All other MultiPurpose Frame Versions are reserved

#define MAC_MP_FRAME_FLAG_ACK_REQUIRED         ((uint16_t)0x4000U) // Bit 14
#define MAC_MP_FRAME_FLAG_IE_LIST_PRESENT      ((uint16_t)0x8000U) // Bit 15

//------------------------------------------------------------------------
// Information Elements fields

// There are Header IEs and Payload IEs.  Header IEs are authenticated
// if MAC Security is enabled.  Payload IEs are both authenticated and
// encrypted if MAC security is enabled.

// Header and Payload IEs have slightly different formats and different
// contents based on the 802.15.4 spec.

// Both are actually a list of IEs that continues until a termination
// IE is seen.

#define MAC_FRAME_HEADER_INFO_ELEMENT_LENGTH_MASK 0x007F // bits 0-6
#define MAC_FRAME_HEADER_INFO_ELEMENT_ID_MASK     0x7F80 // bits 7-14
#define MAC_FRAME_HEADER_INFO_ELEMENT_TYPE_MASK   0x8000 // bit  15

#define MAC_FRAME_HEADER_INFO_ELEMENT_ID_SHIFT 7

#define MAC_FRAME_PAYLOAD_INFO_ELEMENT_LENGTH_MASK   0x07FF  // bits 0 -10
#define MAC_FRAME_PAYLOAD_INFO_ELEMENT_GROUP_ID_MASK 0x7800  // bits 11-14
#define MAC_FRAME_PAYLOAD_INFO_ELEMENT_TYPE_MASK     0x8000  // bit  15

#define MAC_FRAME_PAYLOAD_INFO_ELEMENT_ID_SHIFT      11

// This "type" field indicates header vs. payload IE.  However there is
// also a Header IE List terminator which would imply the IE list
// that follows is only payload IEs.
#define MAC_FRAME_INFO_ELEMENT_TYPE_MASK             0x8000

// Header Termination ID 1 is used when there are Payload IEs that follow.
// Header Termination ID 2 is used when there are no Payload IEs and the
//   next field is the MAC payload.
#define MAC_FRAME_HEADER_TERMINATION_ID_1 0x7E
#define MAC_FRAME_HEADER_TERMINATION_ID_2 0x7F
#define MAC_FRAME_PAYLOAD_TERMINATION_ID 0x0F

//------------------------------------------------------------------------------
// These are from phy/phy-appended-info.h

// Appended Info
// Stored as Big endian fields in a byte array.
#define NUM_APPENDED_INFO_BYTES 8
#define APPENDED_INFO_FRAME_STATUS_BYTE_1_INDEX 0 // MSByte
#define APPENDED_INFO_FRAME_STATUS_BYTE_0_INDEX 1 // LSByte
#define APPENDED_INFO_RSSI_BYTE_INDEX           2
#define APPENDED_INFO_LQI_BYTE_INDEX            3
#define APPENDED_INFO_GAIN_BYTE_INDEX           4
#define APPENDED_INFO_MAC_TIMER_BYTE_2_INDEX    5 // MSByte
#define APPENDED_INFO_MAC_TIMER_BYTE_1_INDEX    6
#define APPENDED_INFO_MAC_TIMER_BYTE_0_INDEX    7 // LSByte
// After the frame status word has been examined, the RX ISR partially
// overwrites it with the channel that the packet was received on.
// This stack uses the channel during active scans.
#define APPENDED_INFO_CHANNEL_OVERWRITE_BYTE_INDEX 0
// Multi-network: we overwrite the second byte of the frame status with the
// network index. This will make the stack aware at higher levels of the network
// on which the packet was received.
#define APPENDED_INFO_NETWORK_OVERWRITE_BYTE_INDEX 1
#define APPENDED_INFO_NETWORK_OVERWRITE_NET_ID_MASK 0x0F
#define APPENDED_INFO_NETWORK_OVERWRITE_NET_ID_SHIFT 0
#define APPENDED_INFO_NETWORK_OVERWRITE_PHY_ID_MASK 0xF0
#define APPENDED_INFO_NETWORK_OVERWRITE_PHY_ID_SHIFT 4

#define NUM_APPENDED_INFO_WORDS (NUM_APPENDED_INFO_BYTES >> 1)
#define APPENDED_INFO_FRAME_STATUS_WORD_INDEX    0
#define APPENDED_INFO_LQI_RSSI_WORD_INDEX        1
#define APPENDED_INFO_GAIN_UPPER_TIME_WORD_INDEX 2
#define APPENDED_INFO_LOWER_TIME_WORD_INDEX      3

// Appended Info Frame Status bits
#define FRAME_STATUS_RX_ACK          BIT(15)
#define FRAME_STATUS_CRC_PASS        BIT(14)
#define FRAME_STATUS_FILTER_COMPLETE BIT(13)
#define FRAME_STATUS_ERROR_RX_CORR   BIT(12)
#define FRAME_STATUS_BB_LENGTH_ERROR BIT(11)
#define FRAME_STATUS_TX_COLL_RX      BIT(10)
#define FRAME_STATUS_ILLEGAL_FORMAT  BIT(9)
#define FRAME_STATUS_ILLEGAL_LENGTH  BIT(8)
#define FRAME_STATUS_RX_MIN_LENGTH   BIT(7)
#define FRAME_STATUS_WRONG_FORMAT    BIT(6)
#define FRAME_STATUS_UNEXPECTED_ACK  BIT(5)
#define FRAME_STATUS_DST_PAN_ID      BIT(4)
#define FRAME_STATUS_DST_SHORT_ADDR  BIT(3)
#define FRAME_STATUS_DST_EXT_ADDR    BIT(2)
#define FRAME_STATUS_SRC_SHORT_ADDR  BIT(1)
#define FRAME_STATUS_WRONG_ADDR      BIT(0)

#include "mac-flat-header.h"
#include "mac-packet-header.h"

#endif //MAC_HEADER_H
