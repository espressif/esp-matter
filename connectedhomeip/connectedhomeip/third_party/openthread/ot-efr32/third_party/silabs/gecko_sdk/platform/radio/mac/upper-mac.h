/***************************************************************************//**
 * @file upper-mac.h
 * @brief
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

#ifndef UPPER_MAC_H
#define UPPER_MAC_H

#if (!defined(PHY_NULL) && !defined(ZIGBEE_STACK_ON_HOST)) && (defined(MAC_DUAL_PRESENT) || SINGLE_PHY_MULTIPAGE_SUPPORT || defined (UNIX) || defined (UNIX_SIMULATION))
#define UPPER_MAC_USING_BASE_PHY
#include "phy/phy.h"
#else
#include "mac-phy.h"
#include PLATFORM_HEADER
#endif

#include "buffer_manager/buffer-management.h"

#include "lower-mac.h"
#include "multi-mac.h"
#include "mac-multi-network.h"
#include "sl_status.h"

#define MAC_TX_QUEUE_SIZE 8
#if (defined(PHY_DUAL) || defined(PHY_SIMULATION_DUAL) || defined (MAC_DUAL_PRESENT) || defined(ALT_MAC))
#define MAX_MAC_INDEX 2
#else
#define MAX_MAC_INDEX 1
#endif
//#define MAX_MAC_INDEX 2
#define MAX_FLAT_PACKET_SIZE 127

// When this callback is called, the higher layer shall perform any final adjustments to the packet necessary just before TX (such as NWK encryption)
// It shall place the packet to send in the flat buffer, with the payload at mac_payload_offset.
// This callback is able to drop a packet by setting the length byte to 0 (flat_packet_buffer[0]) to zero
// If per device power control is used (such as for GB868, it shall fill in the transmit power to tx_power.
typedef uint8_t (*sl_mac_prepare_tx_buffer_t) (PacketHeader packet, uint8_t *flat_packet_buffer, uint8_t mac_payload_offset, uint8_t mac_index, uint8_t nwk_index, int8_t *tx_power);

typedef void (*sl_mac_receive_callback_t) (PacketHeader incoming_packet);
typedef void (*sl_mac_shutdown_callback_t)(uint8_t mac_index);
typedef void (*sl_mac_symbol_timer_callback_t)(void);
typedef bool (*sl_mac_is_passthrough_callback_t) (uint8_t* mac_header, uint8_t macPayloadLength);
typedef void (*sl_mac_poll_handler_t) (EmberNodeId node, bool transmitExpected);
typedef bool (*sl_mac_make_jit_callback_t)(void);
typedef bool (*sl_mac_packet_handoff_incoming_callback_t) (Buffer rawPacket, uint8_t index, void *data);
typedef bool (*sl_mac_process_network_header_callback_t) (Buffer macHeader, const uint8_t *networkHeader);
typedef void (*sl_mac_poll_received_callback_t) (uint8_t childIndex, uint8_t nwkIndex);

typedef enum {
  SL_MAC_TRANSMIT_PRIORITY_HIGH,
  SL_MAC_TRANSMIT_PRIORITY_NORMAL,
  TRANSMIT_PRIORITY_BYPASS_SHUTDOWN,
} sl_mac_transmit_priority_t;

typedef enum {
  SL_MAC_RADIO_OFF,
  SL_MAC_RX_OFF,
  SL_MAC_RX_ON_WHEN_IDLE,
  SL_MAC_RX_DUTY_CYCLING,
} sl_mac_rx_state_t;

typedef void (*sl_mac_transmit_complete_callback_t) (uint8_t mac_index, sl_status_t status, PacketHeader packet, uint8_t tag);

typedef struct {
  uint8_t channel;
  uint8_t mac_index;
  int8_t tx_power;
  EmberPanId pan_id;
  EmberNodeId local_node_id;
  EmberEUI64 local_eui;
  EmberNodeId parent_node_id;
  EmberEUI64 parent_eui;
  sl_mac_csma_parameters_t csmaParams;
  sl_mac_rx_state_t rx_state;
  sl_mac_receive_callback_t rx_callback;
  sl_mac_prepare_tx_buffer_t prepare_tx_callback;
  sl_mac_is_passthrough_callback_t passthrough_filter_callback;
  sl_mac_poll_handler_t poll_handler_callback;
  sl_mac_transmit_complete_callback_t indirect_tx_complete_callback;
  sl_mac_make_jit_callback_t make_jit_message_callback;
  sl_mac_packet_handoff_incoming_callback_t packet_handoff_incoming_callback;
  sl_mac_process_network_header_callback_t process_network_header_callback;
  sl_mac_transmit_complete_callback_t poll_tx_complete_callback;
  sl_mac_poll_received_callback_t poll_rx_callback;
} sl_mac_radio_parameters_t;

sl_status_t sl_mac_init(uint8_t mac_index);

void sl_mac_mark_upper_mac_buffers(void);

sl_status_t sl_mac_get_free_nwk_index(uint8_t mac_index, uint8_t *free_nwk_index);

// DUAL_PAN
// sli_upper mac_update_lower_mac_params will dispatch to appropriate version of
// sli_set_lower_mac_params based on whether or not we are operating on
// concurrent-rx-on networks
sl_status_t sli_upper_mac_update_lower_mac_params(uint8_t mac_index);
sl_status_t sli_set_lower_mac_params_to_current_network(uint8_t mac_index);
sl_status_t sli_set_mac_pan_id_node_id_to_current_network(uint8_t mac_index);

#ifndef EMBER_MULTI_NETWORK_STRIPPED
sl_status_t sli_set_lower_mac_params_to_concurrent_networks(uint8_t mac_index);
// when given pan id and radio channel it will return a bitmask indicating
// which network indices "match".  It will set the bit for the index of
// the first active nwk on that channel with that pan, or if the given
// pan id is a broadcast (0xFFFF) it will set a bit for all active nwks on the
// given channel
sl_status_t sl_mac_get_matching_nwk_index_bitmask(uint8_t mac_index,
                                                  uint16_t pan_id,
                                                  uint8_t radio_channel,
                                                  uint8_t *nwk_bit_mask);
// returns the first nwk index matching the given pan or 0xff
uint8_t sl_mac_get_nwk_index_matching_pan(uint8_t mac_index,
                                          uint8_t radio_channel,
                                          uint16_t pan_id);
#endif // EMBER_MULTI_NETWORK_STRIPPED

/**
 *@brief Configure network receive parameters.
 * If multi network is supported, and the rx Parameters are not possible,
 * an error will be returned.  For example, two rxOnWhenIdle networks on
 * different channels.
 * @param networkIndex The network index for multi network support
 * @param radioParameters The network parameters for receive filtering
 * @return EMBER_SUCCESS or EMBER_INVALID_CALL if index out of range or rx Parameters are invalid
 */
sl_status_t sl_mac_set_nwk_radio_parameters(uint8_t mac_index, uint8_t nwk_index, sl_mac_radio_parameters_t *radio_parameters);

sl_status_t sl_mac_get_nwk_radio_parameters(uint8_t mac_index, uint8_t nwk_index, sl_mac_radio_parameters_t *return_params);

//Request graceful shutdown of mac
//mac continues transmitting until queue is empty, then suspends operation and calls the callback
//any packets received during the suspension are passed to the provided incoming packet callback.
sl_status_t sl_mac_suspend_operation(uint8_t mac_index, sl_mac_shutdown_callback_t shutdown_callback, sl_mac_receive_callback_t incoming_packet_callback);

// Resume MAC operation after concluding shutdown
sl_status_t sl_mac_resume_operation(uint8_t mac_index);

void sl_mac_kickstart(uint8_t mac_index);

void sli_mac_empty_transmit_queue(uint8_t mac_index);

// Submit a packet to the mac queue
sl_status_t sl_mac_submit(uint8_t mac_index,
                          uint8_t nwk_index,
                          PacketHeader packet,
                          sl_mac_transmit_priority_t priority,
                          sl_mac_transmit_complete_callback_t callback,
                          uint8_t tag);

// Triggers the Mac to send a data request when available
// The prior Connect API simply sent the poll immediately.
// However, in Zigbee/Thread, the transmit state machine attempts to
// balance polls with other transmissions, so this API supports that
sl_status_t sl_mac_request_poll(uint8_t mac_index, uint8_t nwk_index);

/** @brief cancel requested data poll
 */
void sl_mac_cancel_poll(uint8_t mac_index, uint8_t nwk_index);
void sl_mac_notify_mac_is_idle(uint8_t mac_index);

sl_status_t sl_mac_schedule_symbol_timer(uint8_t mac_index,
                                         sl_mac_symbol_timer_callback_t callback,
                                         uint16_t duration);

uint8_t sl_mac_prepare_transmit(uint8_t nwk_index, Buffer packet, uint8_t *outgoing_flat_packet);

sl_status_t sl_mac_send_raw_message(uint8_t mac_index,
                                    Buffer message,
                                    sl_mac_transmit_priority_t priority,
                                    bool useCca,
                                    sl_mac_transmit_complete_callback_t callback);

typedef struct {
  Buffer packet;
  uint8_t nwk_index;
  uint8_t tag;
  sl_mac_transmit_complete_callback_t tx_complete_callback;
} sl_mac_upper_mac_tx_queue_entry_t;

typedef struct {
  uint8_t outgoing_flat_packet[MAX_FLAT_PACKET_SIZE];
  uint8_t pending_tasks_bitmask;

  sl_mac_upper_mac_tx_queue_entry_t tx_queue[MAC_TX_QUEUE_SIZE];
  uint8_t unsent_message_count;
  sl_mac_shutdown_callback_t mac_shutdown_callback;
  sl_mac_receive_callback_t incoming_packet_callback;
  bool mac_is_shutdown;

  sl_mac_symbol_timer_callback_t symbol_timer_callback;

  sl_mac_radio_parameters_t nwk_radio_parameters[SL_MAC_MAX_SUPPORTED_NETWORKS];
  sl_mac_upper_mac_tx_queue_entry_t current_outgoing_frame;
  uint8_t current_nwk;
} sl_mac_upper_mac_state_t;

extern sl_mac_upper_mac_state_t sl_mac_upper_mac_state[MAX_MAC_INDEX];

#define MAC_PENDING_TASK_POLL_BIT                  0x01
#define MAC_PENDING_TASK_POLL_BIT_AFTER_SEND       0x02

bool sl_mac_long_id_data_pending(EmberEUI64 address);
RadioPowerMode sl_mac_get_radio_idle_mode(uint8_t mac_index);

typedef enum {
  // We have three modes for now: normal with csma,
  // waiting for grant without csma,
  // complete mac hold off
  MAC_TX_MODE_NORMAL,
  MAC_TX_MODE_GRANT,
  MAC_TX_MODE_MAC_HO,   // complete mac hold off
  // Add more options here.
} EmMacTxMode;

uint8_t sl_mac_get_current_radio_network_index(uint8_t mac_index);
bool sl_mac_upper_mac_is_empty(uint8_t mac_index);
bool sl_mac_network_is_active(uint8_t mac_index, uint8_t nwk_index);

uint8_t sli_mac_get_next_sequence(void);

#endif
