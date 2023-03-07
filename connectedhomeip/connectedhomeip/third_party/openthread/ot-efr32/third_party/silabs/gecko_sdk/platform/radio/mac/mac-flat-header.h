/***************************************************************************//**
 * @file mac-flat-header.h
 * @brief Definitions and macros of PHY and MAC headers, represented as flat
 * (over-the-air) packets.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef MAC_FLAT_HEADER_H
#define MAC_FLAT_HEADER_H

// -----------------------------------------------------------------------------
// Internal APIs

enum {
  EMBER_PH_FIELD_PHY_HEADER                    = 0, // the first (and only)
  EMBER_PH_FIELD_PHY_HEADER_PAYLOAD_LENGTH     = 0, // field of the PHY header
                                                    // is the phy payload length
  EMBER_PH_FIELD_PHY_PAYLOAD                   = 1, // the MAC frame control is
  EMBER_PH_FIELD_MAC_HEADER                    = 1, // the first field of the
  EMBER_PH_FIELD_MAC_HEADER_FRAME_CONTROL      = 1, // MAC header, which is the
                                                    // the beginning of the PHY
                                                    // payload
  EMBER_PH_FIELD_MAC_HEADER_SEQUENCE_NUMBER    = 2,
  EMBER_PH_FIELD_MAC_HEADER_DEST_PAN_ID        = 3,
  EMBER_PH_FIELD_MAC_HEADER_DEST_ADDR          = 4,
  EMBER_PH_FIELD_MAC_HEADER_SRC_PAN_ID         = 5,
  EMBER_PH_FIELD_MAC_HEADER_SRC_ADDR           = 6,
  EMBER_PH_FIELD_MAC_AUXILIARY_HEADER          = 7,
  EMBER_PH_FIELD_MAC_AUX_SECURITY_CONTROL      = 7,
  EMBER_PH_FIELD_MAC_AUX_FRAME_COUNTER         = 8,
  EMBER_PH_FIELD_MAC_PAYLOAD                   = 9,
  EMBER_PH_FIELD_MAC_MIC                       = 13,
};

// -----------------------------------------------------------------------------
// PHY Header definitions

#define EMBER_PHY_HEADER_OFFSET             0
#define EMBER_PHY_PAYLOAD_LENGTH_OFFSET     (EMBER_PHY_HEADER_OFFSET)
#define EMBER_PHY_HEADER_LENGTH             1
#define EMBER_PHY_PAYLOAD_OFFSET            (EMBER_PHY_HEADER_LENGTH)
#define EMBER_PHY_PREAMBLE_LENGTH           4
#define EMBER_PHY_SFD_LENGTH                2
#define EMBER_PHY_CRC_LENGTH                2
#define EMBER_PHY_MAX_PACKET_LENGTH         128
#define EMBER_PHY_MAX_PAYLOAD_LENGTH        (EMBER_PHY_MAX_PACKET_LENGTH \
                                             - EMBER_PHY_HEADER_LENGTH   \
                                             - EMBER_PHY_CRC_LENGTH)

// -----------------------------------------------------------------------------
// MAC Header definitions

#define EMBER_MAC_HEADER_OFFSET                 (EMBER_PHY_HEADER_LENGTH)
#define EMBER_MAC_HEADER_FRAME_CONTROL_OFFSET   (EMBER_MAC_HEADER_OFFSET)
#define EMBER_MAC_HEADER_SEQUENCE_NUMBER_OFFSET (EMBER_MAC_HEADER_OFFSET + 2)

// Frame control defs
#define EMBER_MAC_HEADER_FC_FRAME_TYPE_MASK                 0x0007
#define EMBER_MAC_HEADER_FC_FRAME_TYPE_OFFSET               0
#define EMBER_MAC_HEADER_FC_SECURITY_ENABLED_BIT            0x0008
#define EMBER_MAC_HEADER_FC_FRAME_PENDING_BIT               0x0010
#define EMBER_MAC_HEADER_FC_ACK_REQUEST_BIT                 0x0020
#define EMBER_MAC_HEADER_FC_PAN_ID_COMPRESS_BIT             0x0040
#define EMBER_MAC_HEADER_FC_RESERVED_BITS_MASK              0x0380
#define EMBER_MAC_HEADER_FC_RESERVED_BITS_OFFSET            7

// 802.15.4E-2012 introduced these flags for Frame Version 2 frames
// which are reserved bit positions in earlier Frame Version frames:
#define EMBER_MAC_HEADER_FC_SEQ_SUPPRESSION_BIT             0x0100
#define EMBER_MAC_HEADER_FC_IE_LIST_PRESENT_BIT             0x0200

// Address modes
#define EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_MASK             0x0C00
#define EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_NONE             0x0000
#define EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_RESERVED         0x0400
#define EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_SHORT            0x0800
#define EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_LONG             0x0C00
#define EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_OFFSET           10

#define EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_MASK              0xC000
#define EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_NONE              0x0000
#define EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_RESERVED          0x4000
#define EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_SHORT             0x8000
#define EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_LONG              0xC000
#define EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_OFFSET            14

// Version types
#define EMBER_MAC_HEADER_FC_FRAME_VERSION_MASK              0x3000
#define EMBER_MAC_HEADER_FC_FRAME_VERSION_2003              0x0000
#define EMBER_MAC_HEADER_FC_FRAME_VERSION_2006              0x1000
#define EMBER_MAC_HEADER_FC_FRAME_VERSION_2012              0x2000
#define EMBER_MAC_HEADER_FC_FRAME_VERSION_OFFSET            12

// Frame control frame type
#define EMBER_MAC_HEADER_FC_FRAME_TYPE_BEACON               0x00
#define EMBER_MAC_HEADER_FC_FRAME_TYPE_DATA                 0x01
#define EMBER_MAC_HEADER_FC_FRAME_TYPE_ACK                  0x02
#define EMBER_MAC_HEADER_FC_FRAME_TYPE_COMMAND              0x03
#define EMBER_MAC_HEADER_FC_FRAME_TYPE_CONTROL              EMBER_MAC_HEADER_FC_FRAME_TYPE_COMMAND // (synonym)
#define EMBER_MAC_HEADER_FC_FRAME_TYPE_MULTIPURPOSE         0x05

// Frame version
#define EMBER_MAC_HEADER_FC_FRAME_VERSION_15_4_2003         0x00
#define EMBER_MAC_HEADER_FC_FRAME_VERSION_15_4              0x01

// IEEE's definition of multipurpose frames differ from the standard MAC frame

#define EMBER_MULTIPURPOSE_MAC_HEADER_SEQUENCE_NUMBER_OFFSET        (EMBER_MAC_HEADER_SEQUENCE_NUMBER_OFFSET)
#define EMBER_MULTIPURPOSE_MAC_HEADER_IE_OFFSET                     (10)

// Frame control defs
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_LONG_FRAME_CONTROL_BIT     0x0008
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_SECURITY_ENABLED_BIT       0x0200
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_PAN_ID_PRESENT_BIT         0x0100
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_FRAME_PENDING_BIT          0x0800
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_ACK_REQUEST_BIT            0x4000
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_IE_PRESENT_BIT             0x8000

#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_DEST_ADDR_MODE_MASK        0x0030
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_DEST_ADDR_MODE_NONE        0x0000
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_DEST_ADDR_MODE_RESERVED    0x0010
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_DEST_ADDR_MODE_SHORT       0x0020
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_DEST_ADDR_MODE_LONG        0x0030
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_DEST_ADDR_MODE_OFFSET      4

#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_SRC_ADDR_MODE_MASK         0x00C0
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_SRC_ADDR_MODE_NONE         0x0000
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_SRC_ADDR_MODE_RESERVED     0x0040
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_SRC_ADDR_MODE_SHORT        0x0080
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_SRC_ADDR_MODE_LONG         0x00C0
#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_SRC_ADDR_MODE_OFFSET       6

#define EMBER_MULTIPURPOSE_MAC_HEADER_FC_FRAME_VERSION              0

#define EMBER_802_15_4_IE_HEADER_LENGTH_RENDEZVOUS_TIME             0x04
#define EMBER_802_15_4_IE_HEADER_LENGTH_RENDEZVOUS_TIME_OFFSET      0
#define EMBER_802_15_4_IE_HEADER_ELEMENT_ID_RENDEZVOUS_TIME         0x1D
#define EMBER_802_15_4_IE_HEADER_ELEMENT_ID_RENDEZVOUS_TIME_OFFSET  7
// -----------------------------------------------------------------------------
// MAC Auxiliary Security Header definitions

#define EMBER_MAC_AUX_HEADER_SECURITY_CONTROL_FIELD_OFFSET  0

// Security control field defs
#define EMBER_MAC_AUX_HEADER_SECURITY_LEVEL_MASK            0x07
#define EMBER_MAC_AUX_HEADER_SECURITY_LEVEL_OFFSET          0

// 15.4-2011 - Key identifier mode
// 00 - Key is determined implicitly from the originator and recipient(s) of the
//      frame, as indicated in the frame header.
// 01 - Key is determined from the Key Index field in conjunction with
//      macDefaultKeySource.
// 10 - Key is determined explicitly from the 4-octet Key Source field and the
//      Key Index field.
// 11 - Key is determined explicitly from the 8-octet Key Source field and the
//      Key Index field.
#define EMBER_MAC_AUX_HEADER_KEY_ID_MODE_MASK               0x18
#define EMBER_MAC_AUX_HEADER_KEY_ID_MODE_OFFSET             3

// Some security suites we support (namely XXTEA), need to have the payload
// padded. We allow up to 3 bytes padding. We use the 2 of the 3 reserved bits
// in the security control field to inform the destination about the extra bytes
// included in the MAC payload because of the security padding requirement.
#define EMBER_MAC_AUX_HEADER_PADDING_SIZE_MASK              0x60
#define EMBER_MAC_AUX_HEADER_PADDING_SIZE_OFFSET            5

// We use the most significant reserved bit to distinguish between the two
// security suites we support (AES or XXTEA).
#define EMBER_MAC_AUX_HEADER_XXTEA_SECURITY_BIT             0x80
#define EMBER_MAC_AUX_HEADER_XXTEA_SECURITY_OFFSET          7

// Frame counter
#define EMBER_MAC_AUX_HEADER_FRAME_COUNTER_SIZE             4
#define EMBER_MAC_AUX_HEADER_FRAME_COUNTER_OFFSET           1

// 15.4-2011 - Key identifier
// The Key Identifier field has a variable length and identifies the key that is
// used for cryptographic protection of outgoing frames, either explicitly or in
// conjunction with implicitly defined side information. The Key Identifier
// field shall be present only if the Key Identifier Mode field, as defined in
// 7.4.1.2, is set to a value different from 0x00.
// -------------
// We always use key identifier mode 00, which means that the key identifier
// field is not present.
#define EMBER_MAC_AUX_HEADER_KEY_IDENTIFIER_OFFSET          5

// Security control (1 byte) + frame counter (4 bytes)
#define EMBER_MAC_AUX_HEADER_SIZE                           5

// -----------------------------------------------------------------------------
// Frame Control definitions

#define EMBER_MAC_DATA_FRAME_CONTROL         \
  (EMBER_MAC_HEADER_FC_FRAME_TYPE_DATA       \
   | EMBER_MAC_HEADER_FC_PAN_ID_COMPRESS_BIT \
   | EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_SHORT \
   | EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_SHORT)

#define EMBER_MAC_BEACON_SHORT_FRAME_CONTROL \
  (EMBER_MAC_HEADER_FC_FRAME_TYPE_BEACON     \
   | EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_SHORT \
   | EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_NONE)

#define EMBER_MAC_BEACON_LONG_FRAME_CONTROL \
  (EMBER_MAC_HEADER_FC_FRAME_TYPE_BEACON    \
   | EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_LONG \
   | EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_NONE)

#define EMBER_MAC_BEACON_ENHANCED_FRAME_CONTROL \
  (EMBER_MAC_HEADER_FC_FRAME_TYPE_BEACON        \
   | EMBER_MAC_HEADER_FC_FRAME_VERSION_2012     \
   | EMBER_MAC_HEADER_FC_IE_LIST_PRESENT_BIT    \
   | EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_LONG     \
   | EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_NONE)

// -----------------------------------------------------------------------------
// Phy header macros

#define sl_mac_flat_phy_header(phy_packet) \
  (phy_packet + EMBER_PHY_HEADER_OFFSET)

#define sl_mac_flat_phy_payload_length(phy_packet) \
  ((phy_packet)[EMBER_PHY_HEADER_OFFSET])

#define sl_mac_flat_phy_payload(phy_packet) \
  ((phy_packet) + EMBER_PHY_PAYLOAD_LENGTH_OFFSET)

#define sl_mac_flat_phy_packet_length(phy_packet) \
  (EMBER_PHY_HEADER_LENGTH + sl_mac_flat_phy_payload_length(phy_packet))

// -----------------------------------------------------------------------------
// MAC header macros

#define sl_mac_flat_mac_header_offset(packet, hasPhyHeader) \
  (sl_mac_flat_field_offset((packet), (hasPhyHeader), EMBER_PH_FIELD_MAC_HEADER))

// Returns the size of the MAC header (included the auxiliary MAC header if
// present).
#define sl_mac_flat_mac_header_length(packet, hasPhyHeader)                       \
  (sl_mac_flat_field_offset((packet), (hasPhyHeader), EMBER_PH_FIELD_MAC_PAYLOAD) \
   - ((hasPhyHeader) ? EMBER_PHY_HEADER_LENGTH : 0))

#define sl_mac_flat_mac_header(packet, hasPhyHeader) \
  ((packet) + sl_mac_flat_mac_header_offset((packet), (hasPhyHeader)))

// Frame control
#define sl_mac_flat_mac_frame_control(packet, hasPhyHeader)        \
  (emberFetchLowHighInt16u((packet)                                \
                           + EMBER_MAC_HEADER_FRAME_CONTROL_OFFSET \
                           - ((hasPhyHeader) ? 0 : 1)))

#define sl_mac_flat_set_mac_frame_control(packet, hasPhyHeader, frameControl) \
  (emberStoreLowHighInt16u((packet)                                           \
                           + EMBER_MAC_HEADER_FRAME_CONTROL_OFFSET            \
                           - ((hasPhyHeader) ? 0 : 1),                        \
                           (frameControl)))

#define sl_mac_flat_frame_type(packet, hasPhyHeader) \
  (sl_mac_flat_mac_frame_control((packet), (hasPhyHeader)) & EMBER_MAC_HEADER_FC_FRAME_TYPE_MASK)

#define sl_mac_flat_security_enabled(packet, hasPhyHeader)  \
  ((sl_mac_flat_mac_frame_control((packet), (hasPhyHeader)) \
    & EMBER_MAC_HEADER_FC_SECURITY_ENABLED_BIT) > 0)

#define sl_mac_flat_frame_pending(packet, hasPhyHeader)     \
  ((sl_mac_flat_mac_frame_control((packet), (hasPhyHeader)) \
    & EMBER_MAC_HEADER_FC_FRAME_PENDING_BIT) > 0)

#define sl_mac_flat_ack_requested(packet, hasPhyHeader)     \
  ((sl_mac_flat_mac_frame_control((packet), (hasPhyHeader)) \
    & EMBER_MAC_HEADER_FC_ACK_REQUEST_BIT) > 0)

#define sl_mac_flat_panid_compressed(packet, hasPhyHeader)  \
  ((sl_mac_flat_mac_frame_control((packet), (hasPhyHeader)) \
    & EMBER_MAC_HEADER_FC_PAN_ID_COMPRESS_BIT) > 0)

#define sl_mac_flat_dest_addr_mode(packet, hasPhyHeader)   \
  (sl_mac_flat_mac_frame_control((packet), (hasPhyHeader)) \
   & EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_MASK)

#define sl_mac_flat_frame_version(packet, hasPhyHeader)    \
  (sl_mac_flat_mac_frame_control((packet), (hasPhyHeader)) \
   & EMBER_MAC_HEADER_FC_FRAME_VERSION_MASK)

#define sl_mac_flat_src_addr_mode(packet, hasPhyHeader)    \
  (sl_mac_flat_mac_frame_control((packet), (hasPhyHeader)) \
   & EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_MASK)

// Sequence number
#define sl_mac_flat_sequence_number(packet, hasPhyHeader) \
  ((packet)[EMBER_MAC_HEADER_SEQUENCE_NUMBER_OFFSET - ((hasPhyHeader) ? 0 : 1)])

// Destination PAN ID
#define sl_mac_flat_dest_panid_offset(packet, hasPhyHeader) \
  (sl_mac_flat_field_offset((packet), (hasPhyHeader), EMBER_PH_FIELD_MAC_HEADER_DEST_PAN_ID))

// 15.4-2011 - "This field shall be included in the MAC frame only if the
// Destination Addressing Mode field is nonzero".
#define sl_mac_flat_dest_panid_present(packet, hasPhyHeader) \
  (sl_mac_flat_dest_addr_mode((packet), (hasPhyHeader))      \
   != EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_NONE)

#define sl_mac_flat_dest_panid(packet, hasPhyHeader) \
  (emberFetchLowHighInt16u((packet) + sl_mac_flat_dest_panid_offset((packet), (hasPhyHeader))))

// Destination address
#define sl_mac_flat_dest_addr_offset(packet, hasPhyHeader) \
  (sl_mac_flat_field_offset((packet), (hasPhyHeader), EMBER_PH_FIELD_MAC_HEADER_DEST_ADDR))

// 15.4-2011 - "This field shall be included in the MAC frame only if the
// Destination Addressing Mode field is nonzero".
#define sl_mac_flat_dest_addr_present(packet, hasPhyHeader) \
  (sl_mac_flat_dest_addr_mode((packet), (hasPhyHeader))     \
   != EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_NONE)

#define sl_mac_flat_dest_addr_is_short(packet, hasPhyHeader) \
  (sl_mac_flat_dest_addr_mode((packet), (hasPhyHeader))      \
   == EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_SHORT)

#define sl_mac_flat_dest_addr_is_long(packet, hasPhyHeader) \
  (sl_mac_flat_dest_addr_mode((packet), (hasPhyHeader))     \
   == EMBER_MAC_HEADER_FC_DEST_ADDR_MODE_LONG)

#define sl_mac_flat_dest_short_address(packet, hasPhyHeader) \
  (emberFetchLowHighInt16u((packet) + sl_mac_flat_dest_addr_offset((packet), (hasPhyHeader))))

#define sl_mac_flat_dest_long_address(packet, hasPhyHeader) \
  ((packet) + sl_mac_flat_dest_addr_offset((packet), (hasPhyHeader)))

// Source PAN ID
#define sl_mac_flat_src_panid_offset(packet, hasPhyHeader) \
  (sl_mac_flat_field_offset((packet), (hasPhyHeader), EMBER_PH_FIELD_MAC_HEADER_SRC_PAN_ID))

// 15.4-2011 - "This field shall be included in the MAC frame only if the Source
// Addressing Mode field is nonzero and the PAN ID Compression field is equal to
// zero".
#define sl_mac_flat_src_panid_present(packet, hasPhyHeader) \
  ((sl_mac_flat_src_addr_mode((packet), (hasPhyHeader))     \
    != EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_NONE)              \
   && !sl_mac_flat_panid_compressed((packet), (hasPhyHeader)))

#define sl_mac_flat_src_panid(packet, hasPhyHeader) \
  (emberFetchLowHighInt16u((packet) + sl_mac_flat_src_panid_offset((packet), (hasPhyHeader))))

// Source address
#define sl_mac_flat_src_addr_offset(packet, hasPhyHeader) \
  (sl_mac_flat_field_offset((packet), (hasPhyHeader), EMBER_PH_FIELD_MAC_HEADER_SRC_ADDR))

// 15.4-2011 - "This field shall be included in the MAC frame only if the Source
// Addressing Mode field is nonzero".
#define sl_mac_flat_src_addr_present(packet, hasPhyHeader) \
  (sl_mac_flat_src_addr_mode((packet), (hasPhyHeader))     \
   != EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_NONE)

#define sl_mac_flat_src_addr_is_short(packet, hasPhyHeader) \
  (sl_mac_flat_src_addr_mode((packet), (hasPhyHeader))      \
   == EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_SHORT)

#define sl_mac_flat_src_addr_is_long(packet, hasPhyHeader) \
  (sl_mac_flat_src_addr_mode((packet), (hasPhyHeader))     \
   == EMBER_MAC_HEADER_FC_SRC_ADDR_MODE_LONG)

#define sl_mac_flat_src_short_address(packet, hasPhyHeader) \
  (emberFetchLowHighInt16u((packet) + sl_mac_flat_src_addr_offset((packet), (hasPhyHeader))))

#define sl_mac_flat_src_long_address(packet, hasPhyHeader) \
  ((packet) + sl_mac_flat_src_addr_offset((packet), (hasPhyHeader)))

// -----------------------------------------------------------------------------
// MAC auxiliary security header macros

#define sl_mac_flat_aux_security_header_offset(packet, hasPhyHeader) \
  (sl_mac_flat_field_offset((packet), (hasPhyHeader), EMBER_PH_FIELD_MAC_AUXILIARY_HEADER))

#define sl_mac_flat_aux_security_header_present(packet, hasPhyHeader) \
  (sl_mac_flat_security_enabled((packet), (hasPhyHeader)))

#define sl_mac_flat_aux_security_header(packet, hasPhyHeader) \
  ((packet) + sl_mac_flat_aux_security_header_offset((packet), (hasPhyHeader)))

#define sl_mac_flat_aux_security_control_offset(packet, hasPhyHeader) \
  (sl_mac_flat_field_offset((packet), (hasPhyHeader), EMBER_PH_FIELD_MAC_AUX_SECURITY_CONTROL))

#define sl_mac_flat_aux_security_control(packet, hasPhyHeader) \
  (*((packet) + sl_mac_flat_aux_security_control_offset((packet), (hasPhyHeader))))

#define sl_mac_flat_aux_security_frame_counter_offset(packet, hasPhyHeader) \
  (sl_mac_flat_field_offset((packet), (hasPhyHeader), EMBER_PH_FIELD_MAC_AUX_FRAME_COUNTER))

#define sl_mac_flat_aux_security_frame_counter(packet, hasPhyHeader) \
  (emberFetchLowHighInt32u(((packet), (hasPhyHeader)) + sl_mac_flat_aux_security_frame_counter_offset((packet), (hasPhyHeader))))

#define sl_mac_flat_aux_security_level(packet, hasPhyHeader) \
  (sl_mac_flat_aux_security_control((packet), (hasPhyHeader)) & EMBER_MAC_AUX_HEADER_SECURITY_LEVEL_MASK)

#define sl_mac_flat_aux_security_key_id_mode(packet, hasPhyHeader)                                      \
  ((sl_mac_flat_aux_security_control((packet), (hasPhyHeader)) & EMBER_MAC_AUX_HEADER_KEY_ID_MODE_MASK) \
   >> EMBER_MAC_AUX_HEADER_KEY_ID_MODE_OFFSET)

#define sl_mac_flat_aux_security_padding_size(packet, hasPhyHeader)                                      \
  ((sl_mac_flat_aux_security_control((packet), (hasPhyHeader)) & EMBER_MAC_AUX_HEADER_PADDING_SIZE_MASK) \
   >> EMBER_MAC_AUX_HEADER_PADDING_SIZE_OFFSET)

#define sl_mac_flat_aux_security_is_xxtea(packet, hasPhyHeader) \
  ((sl_mac_flat_aux_security_control((packet), (hasPhyHeader)) & EMBER_MAC_AUX_HEADER_XXTEA_SECURITY_BIT) > 0)

#define sl_mac_flat_aux_security_is_aes(packet, hasPhyHeader) \
  (!sl_mac_flat_aux_security_is_xxtea((packet), (hasPhyHeader)))

// -----------------------------------------------------------------------------
// MAC payload macros and functions

uint8_t sl_mac_flat_field_offset(uint8_t *packet, bool hasPhyHeader, uint8_t field);

#define sl_mac_flat_payload_offset(packet, hasPhyHeader) \
  (sl_mac_flat_field_offset((packet), (hasPhyHeader), EMBER_PH_FIELD_MAC_PAYLOAD))

// TODO: Calculate payload length for a flat packet starting at the MAC header.
uint8_t sl_mac_flat_payload_length(uint8_t *phy_packet);

#define sl_mac_flat_payload(packet, hasPhyHeader) \
  ((packet) + sl_mac_flat_payload_offset((packet), (hasPhyHeader)))

#define EMBER_MAC_PAYLOAD_COMMAND_ID_OFFSET   0

// -----------------------------------------------------------------------------
// MAC MIC macros

#define sl_mac_flat_mic_offset(packet, hasPhyHeader) \
  (sl_mac_flat_field_offset((packet), EMBER_PH_FIELD_MAC_MIC))

#define sl_mac_flat_mic(packet, hasPhyHeader) \
  (emberFetchLowHighInt32u(((packet), (hasPhyHeader)) + sl_mac_flat_mic_offset((packet), (hasPhyHeader))))

#define EMBER_MAC_SECURITY_MIC_SIZE           4
#define EMBER_MAC_SECURITY_2006_ENC_MIC_32    5

#endif //MAC_FLAT_HEADER_H
