/***************************************************************************//**
 * @file mac-packet-header.h
 * @brief Definitions and macros of MAC headers, represented using in-memory
 * format.
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

#ifndef MAC_PACKET_HEADER_H
#define MAC_PACKET_HEADER_H

//------------------------------------------------------------------------------
// MAC Info definitions
#include PLATFORM_HEADER

// These are from 802.15.4 except that we shift them up by two bits to fit
// where we put them in MAC info byte.
enum {
  EMBER_MAC_INFO_TYPE_BEACON      = 0,
  EMBER_MAC_INFO_TYPE_DATA        = BIT(2),
  EMBER_MAC_INFO_TYPE_PASSTHROUGH = BIT(3),
  EMBER_MAC_INFO_TYPE_MAC_COMMAND = (BIT(2) | BIT(3))
};

// Bitmasks for flags in the mac info field
#define EMBER_MAC_INFO_FRAME_TYPE_MASK           ((uint16_t)(BIT(2) | BIT(3)))  // 0x000C

// Outgoing packets with this set are created by the stack itself and
// are not passed to emberMessageSentHandler().
//
// TODO: Invert the meaning of this bit so headers are private by default and
// then set the bit in any header sent by the app.
// - Ed (Feb 2011)
#define EMBER_MAC_INFO_STACK_PRIVATE_MASK        ((uint16_t)(BIT(4)))              // 0x0010

#define EMBER_MAC_INFO_MAC_SECURITY_MASK         ((uint16_t)(BIT(5)))              // 0x0020
#define EMBER_MAC_INFO_FRAME_PENDING_MASK        ((uint16_t)(BIT(7)))              // 0x0080

// If set on an outgoing unicast to a sleepy child, this causes the
// message to be sent directly instead of waiting for a poll.
#define EMBER_MAC_INFO_POLL_RESPONSE_MASK        ((uint16_t)(BIT(10)))             // 0x0400

// Marks incoming data packets that were sent to the broadcast node ID.
#define EMBER_MAC_INFO_BROADCAST_NODE_ID         ((uint16_t)(BIT(11)))             // 0x0800

// For incoming packets this is a copy of the frame pending bit on the
// ACK that was sent in response.
#define EMBER_MAC_INFO_OUTGOING_FRAME_PENDING    ((uint16_t)(BIT(12)))             // 0x1000

// When we need to send MAC datagrams from our long source we use
// this to indicate that the short node ID should NOT be used.
#define EMBER_MAC_INFO_LONG_SOURCE_DATAGRAM_MASK ((uint16_t)(BIT(13)))             // 0x2000

// For messages which are being exchanged via a secure medium like for
// example BLE (Zigbee Direct) there is no NWK encryption needed
// this bit indicates whether the corresponding packet is to be treated
// as if it had NWK security
#define EMBER_MAC_INFO_TREAT_AS_SECURE_PACKET ((uint16_t)(BIT(15)))                // 0x8000

void sl_mac_header_set_mac_info(PacketHeader header, uint16_t info);

void sl_mac_header_set_mac_info_bit(PacketHeader header, uint16_t bit, bool set);

uint16_t sl_mac_header_mac_info(PacketHeader header);
uint8_t  sli_mac_header_mac_index(PacketHeader header);
uint8_t sl_mac_header_mac_info_frame_type(PacketHeader header);

//------------------------------------------------------------------------------
// in-memory representation

//-----------------------------
// Common in-memory stored info
typedef struct {
  sl_mac_node_id_t destination;
} sl_mac_in_memory_tx_data_info_t;

typedef struct {
  sl_mac_node_id_t source;
} sl_mac_in_memory_rx_data_info_t;

typedef struct {
  // No extra members in raw packets
  sl_mac_node_id_t dummy; //IAR doesn't like empty structs.  This shouldn't cost any ram since it's part of a union anyway
} sl_mac_in_memory_raw_info_t;

typedef struct {
  uint16_t mac_info_flags;
  uint8_t nwk_index;
  uint8_t mac_index;
  union {
    sl_mac_in_memory_tx_data_info_t tx_data;
    sl_mac_in_memory_rx_data_info_t rx_data;
    sl_mac_in_memory_raw_info_t raw_info;
  } pkt_info;
} sl_mac_in_memory_info_t;

//--------------------------------------
// Common in-memory stored appended info

// When on emMacToNetworkQueue
typedef struct {
  // The link quality indicator of the received packet
  uint8_t lqi;
  // The energy detected over the last 8 symbols of the packet in units of dBm
  uint8_t rssi;
  // The channel index where this packet was received
  uint8_t channel;
  // The value of the MAC timer when the SFD was received for this packet
  uint32_t timestamp;
} sl_mac_in_memory_queue_info_t;

// When in shortIndirectPool or longIndirectPool (in indirect-queue.c).
typedef struct {
  uint16_t indirect_timeout;
  uint8_t indirect_tries;
  uint8_t submit_flag;
} sl_mac_in_memory_indirect_queue_info_t;

typedef union {
  sl_mac_in_memory_queue_info_t queue_info;
  sl_mac_in_memory_indirect_queue_info_t indirect_queue_info;
} sl_mac_in_memory_appended_info_t;

//--------------------------------------

typedef struct {
  sl_mac_in_memory_info_t info;
  sl_mac_in_memory_appended_info_t appended_info;
  uint8_t payload[0];
} sl_mac_in_memory_overhead_t;
#define EMBER_MAC_IN_MEMORY_OVERHEAD sizeof(sl_mac_in_memory_overhead_t)

//--------------------------------------
// in-memory APIs

bool sl_mac_is_data_packet(PacketHeader header);
bool sl_mac_is_raw_packet(PacketHeader header);

uint16_t sl_mac_destination_mode(PacketHeader header);
sl_mac_node_id_t sl_mac_destination(PacketHeader header);
uint8_t *sl_mac_destination_pointer(PacketHeader header);

void sl_mac_set_destination(PacketHeader header, sl_mac_node_id_t id); //sToDo: this probably needs to be replaced with exact implementations

uint16_t sl_mac_source_mode(PacketHeader header);
sl_mac_node_id_t sl_mac_source(PacketHeader header);
uint8_t *sl_mac_source_pointer(PacketHeader header);

uint8_t sl_mac_payload_length(PacketHeader header);
uint8_t *sl_mac_payload_pointer(PacketHeader header);

#ifdef EMBER_MULTI_NETWORK_STRIPPED
#define sl_mac_nwk_index(header) 0
#else
uint8_t sl_mac_nwk_index(PacketHeader header);
#endif
uint8_t sl_mac_lqi(PacketHeader header);
int8_t sl_mac_rssi(PacketHeader header);
uint8_t sl_mac_channel(PacketHeader header);
uint32_t sl_mac_timestamp(PacketHeader header);

void sl_mac_set_lqi(PacketHeader header, uint8_t lqi);
void sl_mac_set_rssi(PacketHeader header, int8_t rssi);
void sl_mac_set_channel(PacketHeader header, uint8_t channel);
void sl_mac_set_timestamp(PacketHeader header, uint32_t timestamp);

PacketHeader sl_mac_make_data_message(sl_mac_node_id_t destination,
                                      uint8_t payloadLength,
                                      const uint8_t *payload,
                                      uint16_t macInfoFlags,
                                      uint8_t nwk_index);

PacketHeader sl_mac_make_raw_message(Buffer payload,
                                     uint16_t macInfoFlags,
                                     uint8_t nwk_index);

sl_status_t sl_mac_radio_receive_mac_header_callback(uint8_t mac_index,
                                                     uint8_t nwk_index,
                                                     uint8_t *rawMacHeader,
                                                     uint8_t macPayloadLength,
                                                     PacketHeader *rxPayload);

#endif //MAC_PACKET_HEADER_H
