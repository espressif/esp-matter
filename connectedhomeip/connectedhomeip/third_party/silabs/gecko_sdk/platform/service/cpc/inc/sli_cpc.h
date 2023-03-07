/***************************************************************************/ /**
 * @file
 * @brief CPC
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SLI_CPC_H
#define SLI_CPC_H

#include <stddef.h>
#include <stdarg.h>

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include "sl_status.h"
#include "sl_enum.h"
#include "sl_slist.h"
#include "sli_mem_pool.h"
#include "sl_sleeptimer.h"
#include "sl_cpc.h"
#include "sl_cpc_config.h"
#include "sli_cpc_system_common.h"
#if defined(SL_CATALOG_CPC_SECURITY_PRESENT)
#include "sl_cpc_security.h"
#include "sl_cpc_security_config.h"
#include "sli_cpc_security.h"
#endif

#define SLI_CPC_PROTOCOL_VERSION 2

#define SLI_CPC_ENDPOINT_SYSTEM             (1)

#if (!defined(SL_CATALOG_CPC_SECURITY_PRESENT))
#define SL_CPC_ENDPOINT_SECURITY_ENABLED    (0)
#else
#if (SL_CPC_SECURITY_ENABLED >= 1)
#define SL_CPC_ENDPOINT_SECURITY_ENABLED    (1)
#else
#define SL_CPC_ENDPOINT_SECURITY_ENABLED    (0)
#endif
#endif

#if defined(SL_CATALOG_BLUETOOTH_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_BLUETOOTH_ENABLED  (1)
#else
#define SLI_CPC_ENDPOINT_BLUETOOTH_ENABLED  (0)
#endif

#if defined(SL_CATALOG_RAIL_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_RAIL_ENABLED (2)
#else
#define SLI_CPC_ENDPOINT_RAIL_ENABLED (0)
#endif

#if defined(SL_CATALOG_ZIGBEE_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_ZIGBEE_ENABLED (1)
#else
#define SLI_CPC_ENDPOINT_ZIGBEE_ENABLED (0)
#endif

#if defined(SL_CATALOG_ZWAVE_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_ZWAVE_ENABLED  (1)
#else
#define SLI_CPC_ENDPOINT_ZWAVE_ENABLED  (0)
#endif

#if defined(SL_CATALOG_CONNECT_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_CONNECT_ENABLED  (1)
#else
#define SLI_CPC_ENDPOINT_CONNECT_ENABLED  (0)
#endif

#if defined(SL_CATALOG_GPIO_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_GPIO_ENABLED  (1)
#else
#define SLI_CPC_ENDPOINT_GPIO_ENABLED  (0)
#endif

#if defined(SL_CATALOG_OPENTHREAD_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_OPENTHREAD_ENABLED (1)
#else
#define SLI_CPC_ENDPOINT_OPENTHREAD_ENABLED (0)
#endif

#if defined(SL_CATALOG_WISUN_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_WISUN_ENABLED  (1)
#else
#define SLI_CPC_ENDPOINT_WISUN_ENABLED  (0)
#endif

#if defined(SL_CATALOG_WIFI_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_WIFI_ENABLED  (1)
#else
#define SLI_CPC_ENDPOINT_WIFI_ENABLED  (0)
#endif

#if defined(SL_CATALOG_15_4_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_15_4_ENABLED  (1)
#else
#define SLI_CPC_ENDPOINT_15_4_ENABLED  (0)
#endif

#if defined(SL_CATALOG_CLI_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_CLI_ENABLED  (1)
#else
#define SLI_CPC_ENDPOINT_CLI_ENABLED  (0)
#endif

#if defined(SL_CATALOG_CLI_NCP_PRESENT)
#define SLI_CPC_ENDPOINT_CLI_ENABLED  (1)
#else
#define SLI_CPC_ENDPOINT_CLI_ENABLED  (0)
#endif

#if defined(SL_CATALOG_BLUETOOTH_RCP_PRESENT)
#define SL_CPC_ENDPOINT_BLUETOOTH_RCP_ENABLED  (1)
#else
#define SL_CPC_ENDPOINT_BLUETOOTH_RCP_ENABLED  (0)
#endif

#if defined(SL_CATALOG_ACP_PRESENT)
#define SL_CPC_ENDPOINT_ACP_ENABLED  (1)
#else
#define SL_CPC_ENDPOINT_ACP_ENABLED  (0)
#endif

// Frame Flags
#define SL_CPC_OPEN_ENDPOINT_FLAG_IFRAME_DISABLE    0x01 << 0   // I-frame is enabled by default; This flag MUST be set to disable the i-frame support by the endpoint
#define SL_CPC_OPEN_ENDPOINT_FLAG_UFRAME_ENABLE     0x01 << 1   // U-frame is disabled by default; This flag MUST be set to enable u-frame support by the endpoint
#define SL_CPC_OPEN_ENDPOINT_FLAG_UFRAME_INFORMATION_DISABLE  0x01 << 2

// SL_CPC_FLAG_NO_BLOCK = 0x01 << 0
#define SL_CPC_FLAG_UNNUMBERED_INFORMATION      0x01 << 1
#define SL_CPC_FLAG_UNNUMBERED_POLL             0x01 << 2
#define SL_CPC_FLAG_UNNUMBERED_ACKNOWLEDGE      0x01 << 3
#define SL_CPC_FLAG_INFORMATION_FINAL           0x01 << 4

/// @brief Enumeration representing the possible id
SL_ENUM(sl_cpc_service_endpoint_id_t){
  SL_CPC_ENDPOINT_SYSTEM = 0,          ///< System control
#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  SL_CPC_ENDPOINT_SECURITY = 1,        ///< Security - related functionality
#endif
#if (SLI_CPC_ENDPOINT_BLUETOOTH_ENABLED >= 1)
  SL_CPC_ENDPOINT_BLUETOOTH = 2,       ///< Bluetooth endpoint
#endif
#if (SLI_CPC_ENDPOINT_RAIL_ENABLED >= 1)
  SL_CPC_SLI_CPC_ENDPOINT_RAIL_DOWNSTREAM = 3, ///< RAIL downstream endpoint
  SL_CPC_SLI_CPC_ENDPOINT_RAIL_UPSTREAM = 4,   ///< RAIL upstream endpoint
#endif
#if (SLI_CPC_ENDPOINT_ZIGBEE_ENABLED >= 1)
  SL_CPC_ENDPOINT_ZIGBEE = 5,          ///< ZigBee EZSP endpoint
#endif
#if (SLI_CPC_ENDPOINT_ZWAVE_ENABLED >= 1)
  SL_CPC_ENDPOINT_ZWAVE = 6,           ///< Z-Wave endpoint
#endif
#if (SLI_CPC_ENDPOINT_CONNECT_ENABLED >= 1)
  SL_CPC_ENDPOINT_CONNECT = 7,         ///< Connect endpoint
  #endif
#if (SLI_CPC_ENDPOINT_GPIO_ENABLED >= 1)
  SL_CPC_ENDPOINT_GPIO = 8,            ///< GPIO endpoint for controlling GPIOs on SECONDARYs
#endif
#if (SLI_CPC_ENDPOINT_OPENTHREAD_ENABLED >= 1)
  SL_CPC_ENDPOINT_OPENTHREAD = 9,      ///< Openthread Spinel endpoint
#endif
#if (SLI_CPC_ENDPOINT_WISUN_ENABLED >= 1)
  SL_CPC_ENDPOINT_WISUN = 10,           ///< WiSun endpoint
#endif
#if (SLI_CPC_ENDPOINT_WIFI_ENABLED >= 1)
  SL_CPC_ENDPOINT_WIFI = 11,           ///< WiFi endpoint(main control)
#endif
#if (SLI_CPC_ENDPOINT_15_4_ENABLED >= 1)
  SL_CPC_ENDPOINT_15_4 = 12,           ///< 802.15.4 endpoint
#endif
#if (SLI_CPC_ENDPOINT_CLI_ENABLED >= 1)
  SL_CPC_ENDPOINT_CLI = 13,            ///< Ascii based CLI for stacks / applications
#endif
#if (SL_CPC_ENDPOINT_BLUETOOTH_RCP_ENABLED >= 1)
  SL_CPC_ENDPOINT_BLUETOOTH_RCP = 14,  ///< Bluetooth RCP endpoint
#endif
#if (SL_CPC_ENDPOINT_ACP_ENABLED >= 1)
  SL_CPC_ENDPOINT_ACP = 15,            ///< ACP endpoint
#endif
  SL_CPC_ENDPOINT_LAST_ID_MARKER,      // DO NOT USE THIS ENDPOINT ID
};

#if defined(SL_CATALOG_CPC_PRIMARY_PRESENT)
#define SL_CPC_ON_FINAL_PRESENT
#endif

#if defined(SL_CATALOG_CPC_SECONDARY_PRESENT)
#define SL_CPC_ON_POLL_PRESENT
#endif

#if (!defined(SL_CATALOG_CPC_PRIMARY_PRESENT) && !defined(SL_CATALOG_CPC_SECONDARY_PRESENT))
// This is required for unit testing (assumed that we are unit testing on a secondary)
#define SL_CPC_ON_POLL_PRESENT
#endif

typedef void (*sl_cpc_on_poll_t)(uint8_t endpoint_id, void *arg,
                                 void *poll_data, uint32_t poll_data_length,    // Rx buffer is freed once this on_poll function return
                                 void **reply_data, uint32_t *reply_data_lenght,
                                 void **on_write_complete_arg);

typedef void (*sl_cpc_on_final_t)(uint8_t endpoint_id, void *arg, void *answer, uint32_t answer_lenght);

typedef struct {
  void *on_fnct_arg;
#ifdef SL_CPC_ON_FINAL_PRESENT
  sl_cpc_on_final_t on_final;
#endif
#ifdef SL_CPC_ON_POLL_PRESENT
  sl_cpc_on_poll_t on_poll;
  void *data;   // Anwser
  uint32_t data_length;
#endif
} sl_cpc_poll_final_t;

#define SLI_CPC_SERVICE_ENDPOINT_ID_START     ((uint8_t)SL_CPC_ENDPOINT_SYSTEM)
#define SLI_CPC_SERVICE_ENDPOINT_ID_END       ((uint8_t)SL_CPC_ENDPOINT_LAST_ID_MARKER - 1)
#define SLI_CPC_SERVICE_ENDPOINT_MAX_COUNT    (SLI_CPC_SERVICE_ENDPOINT_ID_END - SLI_CPC_SERVICE_ENDPOINT_ID_START + 1)

/***************************************************************************//**
 * The maximum size of a system endpoint command buffer.
 *
 * @note
 *   For the moment, this value must be manually set.
 *
 * @note : it is set to the size of the payload of a
 * CMD_PROPERTY_GET::PROP_ENDPOINT_STATES.
 *
 ******************************************************************************/
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define SL_CPC_ENDPOINT_MAX_COUNT  256

#define SLI_CPC_SYSTEM_COMMAND_BUFFER_SIZE \
  (sizeof(sli_cpc_system_cmd_t)            \
   + sizeof(sli_cpc_system_property_cmd_t) \
   + SL_CPC_ENDPOINT_MAX_COUNT * sizeof(sl_cpc_endpoint_state_t) / 2)

#ifndef SLI_CPC_SYSTEM_COMMAND_BUFFER_COUNT
#define SLI_CPC_SYSTEM_COMMAND_BUFFER_COUNT 5
#endif

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
#define SLI_CPC_RX_DATA_MAX_LENGTH          (SL_CPC_RX_PAYLOAD_MAX_LENGTH + 2 + SLI_SECURITY_TAG_LENGTH_BYTES)
#else
#define SLI_CPC_RX_DATA_MAX_LENGTH             (SL_CPC_RX_PAYLOAD_MAX_LENGTH + 2)
#endif
#define SLI_CPC_HDLC_REJECT_MAX_COUNT          ((SL_CPC_RX_BUFFER_MAX_COUNT / 2) + 1)
#define SLI_CPC_RX_QUEUE_ITEM_MAX_COUNT        (1 + ((SL_CPC_RX_BUFFER_MAX_COUNT / 4) * 3))
#define SLI_CPC_TX_QUEUE_ITEM_SFRAME_MAX_COUNT (SLI_CPC_RX_QUEUE_ITEM_MAX_COUNT)
#define SLI_CPC_BUFFER_HANDLE_MAX_COUNT        (SL_CPC_TX_QUEUE_ITEM_MAX_COUNT + SL_CPC_RX_BUFFER_MAX_COUNT + SLI_CPC_TX_QUEUE_ITEM_SFRAME_MAX_COUNT)
#define SLI_CPC_RE_TRANSMIT                    (5)
#define SLI_CPC_MAX_RE_TRANSMIT_TIMEOUT_MS     (5000)
#define SLI_CPC_MIN_RE_TRANSMIT_TIMEOUT_MS     (250)
#define SLI_CPC_DISCONNECTION_NOTIFICATION_TIMEOUT_MS  (1000)
#define SLI_CPC_MIN_RE_TRANSMIT_TIMEOUT_MINIMUM_VARIATION_MS (50)

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
#if (SL_CPC_RX_PAYLOAD_MAX_LENGTH > 4079)
  #error Invalid SL_CPC_RX_PAYLOAD_MAX_LENGTH; Must be less or equal to 4079
#endif
#else
#if (SL_CPC_RX_PAYLOAD_MAX_LENGTH > 4087)
  #error Invalid SL_CPC_RX_PAYLOAD_MAX_LENGTH; Must be less or equal to 4087
#endif
#endif

#if !defined(SLI_CPC_ENDPOINT_TEMPORARY_MAX_COUNT)
#if defined(SL_CATALOG_CPC_PERF_PRESENT)
#define SLI_CPC_ENDPOINT_TEMPORARY_MAX_COUNT      (10)
#else
#define SLI_CPC_ENDPOINT_TEMPORARY_MAX_COUNT      (0) // Not yet available
#endif
#endif

#define SLI_CPC_TEMPORARY_ENDPOINT_ID_START   100
#define SLI_CPC_TEMPORARY_ENDPOINT_ID_END     SLI_CPC_TEMPORARY_ENDPOINT_ID_START + SLI_CPC_ENDPOINT_TEMPORARY_MAX_COUNT - 1
#if (SLI_CPC_TEMPORARY_ENDPOINT_ID_END > 254)
#error Invalid TEMPORARY ENDPOINT COUNT
#endif

#define SLI_CPC_ENDPOINT_MIN_COUNT  (SLI_CPC_ENDPOINT_SYSTEM + SL_CPC_ENDPOINT_SECURITY_ENABLED)

#define SLI_CPC_ENDPOINT_INTERNAL_COUNT                 (SLI_CPC_ENDPOINT_SYSTEM + SL_CPC_ENDPOINT_SECURITY_ENABLED + SLI_CPC_ENDPOINT_BLUETOOTH_ENABLED          \
                                                         + SLI_CPC_ENDPOINT_RAIL_ENABLED + SLI_CPC_ENDPOINT_ZIGBEE_ENABLED + SLI_CPC_ENDPOINT_ZWAVE_ENABLED       \
                                                         + SLI_CPC_ENDPOINT_CONNECT_ENABLED + SLI_CPC_ENDPOINT_GPIO_ENABLED + SLI_CPC_ENDPOINT_OPENTHREAD_ENABLED \
                                                         + SLI_CPC_ENDPOINT_WISUN_ENABLED + SLI_CPC_ENDPOINT_WIFI_ENABLED + SLI_CPC_ENDPOINT_CLI_ENABLED          \
                                                         + SL_CPC_ENDPOINT_BLUETOOTH_RCP_ENABLED + SL_CPC_ENDPOINT_ACP_ENABLED)

#if !defined(SLI_CPC_ENDPOINT_COUNT)
#define SLI_CPC_ENDPOINT_COUNT          (SLI_CPC_ENDPOINT_INTERNAL_COUNT + SL_CPC_USER_ENDPOINT_MAX_COUNT + SLI_CPC_ENDPOINT_TEMPORARY_MAX_COUNT)
#endif

#if (SLI_CPC_ENDPOINT_COUNT > SL_CPC_ENDPOINT_MAX_COUNT)
  #error Invalid SLI_CPC_ENDPOINT_COUNT; Must be less than SL_CPC_ENDPOINT_MAX_COUNT
#elif (SLI_CPC_ENDPOINT_COUNT < SLI_CPC_ENDPOINT_MIN_COUNT)
  #error Invalid SLI_CPC_ENDPOINT_COUNT; Must be greater than SLI_CPC_ENDPOINT_MIN_COUNT
#endif

#define SLI_CPC_HDLC_HEADER_MAX_COUNT       (SL_CPC_RX_BUFFER_MAX_COUNT                                       \
                                             + (MIN(SL_CPC_TRANSMIT_WINDOW_MAX_SIZE * SLI_CPC_ENDPOINT_COUNT, \
                                                    SLI_CPC_BUFFER_HANDLE_MAX_COUNT)))

#define SLI_CPC_POP_BUFFER_HANDLE_LIST(head_ptr, item_type) ({    \
    sl_slist_node_t *item_node = sl_slist_pop(head_ptr);          \
    item_type *item = SL_SLIST_ENTRY(item_node, item_type, node); \
    if (item != NULL) {                                           \
      EFM_ASSERT(item->handle != NULL);                           \
      EFM_ASSERT(item->handle->ref_count > 0);                    \
      item->handle->ref_count--;                                  \
    }                                                             \
    item_node;                                                    \
  })

#define SLI_CPC_REMOVE_BUFFER_HANDLE_FROM_LIST(head_ptr, item_ptr, item_type) ({ \
    EFM_ASSERT(item_ptr != NULL);                                                \
    item_type *item = *item_ptr;                                                 \
    EFM_ASSERT(item != NULL);                                                    \
    EFM_ASSERT(item->handle != NULL);                                            \
    EFM_ASSERT(item->handle->ref_count > 0);                                     \
    item->handle->ref_count--;                                                   \
    sl_slist_remove(head_ptr, &item->node);                                      \
  })

#if SL_CPC_USER_ENDPOINT_MAX_COUNT > 10
#error "SL_CPC_USER_ENDPOINT_MAX_COUNT must be less than 10"
#endif

// Signal count max = closing signal per endpoint + tx queue items + rx queue items
#define EVENT_SIGNAL_MAX_COUNT    SLI_CPC_ENDPOINT_COUNT + SL_CPC_TX_QUEUE_ITEM_MAX_COUNT + SLI_CPC_RX_QUEUE_ITEM_MAX_COUNT

SL_ENUM(sl_cpc_signal_type_t) {
  SL_CPC_SIGNAL_RX,
  SL_CPC_SIGNAL_TX,
  SL_CPC_SIGNAL_CLOSED,
  SL_CPC_SIGNAL_SYSTEM,
};

SL_ENUM(sl_cpc_reject_reason_t){
  SL_CPC_REJECT_NO_ERROR = 0,
  SL_CPC_REJECT_CHECKSUM_MISMATCH,
  SL_CPC_REJECT_SEQUENCE_MISMATCH,
  SL_CPC_REJECT_OUT_OF_MEMORY,
  SL_CPC_REJECT_SECURITY_ISSUE,
  SL_CPC_REJECT_UNREACHABLE_ENDPOINT,
  SL_CPC_REJECT_ERROR
};

SL_ENUM(sl_cpc_buffer_type_t) {
  SL_CPC_UNKNOWN_BUFFER,
  SL_CPC_RX_BUFFER,
  SL_CPC_HDLC_REJECT_BUFFER
};

typedef struct {
  sl_slist_node_t node;
  sl_slist_node_t node_closing;
  uint8_t id;
  uint8_t flags;
  uint8_t seq;
  uint8_t ack;
  uint8_t configured_tx_window_size;
  uint8_t current_tx_window_space;
  uint8_t frames_count_re_transmit_queue;
  uint8_t packet_re_transmit_count;
  uint32_t re_transmit_timeout;
  uint64_t last_iframe_sent_timestamp;
  uint64_t smoothed_rtt;
  uint64_t rtt_variation;
  sl_sleeptimer_timer_handle_t re_transmit_timer;
  sl_sleeptimer_timer_handle_t close_timer;
  sl_cpc_endpoint_state_t state;
  sl_cpc_poll_final_t poll_final;
  sl_cpc_on_write_completed_t on_iframe_write_completed;
  sl_cpc_on_data_reception_t on_iframe_data_reception;
  void *on_iframe_data_reception_arg;
  sl_cpc_on_write_completed_t on_uframe_write_completed;
  sl_cpc_on_data_reception_t on_uframe_data_reception;
  void *on_uframe_data_reception_arg;
  sl_cpc_on_error_callback_t on_error;
  void *on_error_arg;
  sl_slist_node_t *iframe_receive_queue;
  sl_slist_node_t *uframe_receive_queue;
  sl_slist_node_t *re_transmit_queue;
  sl_slist_node_t *holding_list;
#if (SL_CPC_DEBUG_ENDPOINT_EVENT_COUNTERS == 1)
  sl_cpc_endpoint_debug_counters_t debug_counters;
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  __ALIGNED(4) uint8_t lock_cb[osMutexCbSize];
  osMutexId_t lock;
  osSemaphoreId_t receive_signal;
  bool read_aborted;
#endif
#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  bool packets_held_for_security;
#endif
} sl_cpc_endpoint_t;

typedef struct {
  void *hdlc_header;
  void *data;
#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  void *security_tag;
#endif
  uint16_t data_length;
  uint8_t fcs[2];
  uint8_t control;
  uint8_t address;
  uint8_t ref_count;
  sl_cpc_buffer_type_t buffer_type;
  sl_cpc_endpoint_t *endpoint;
  sl_cpc_reject_reason_t reason;
  void *arg;
  bool on_write_complete_pending;
} sl_cpc_buffer_handle_t;

typedef struct {
  uint8_t id;
  sl_cpc_on_write_completed_t on_iframe_write_completed;
  sl_cpc_on_write_completed_t on_uframe_write_completed;
  void *arg;
} sl_cpc_endpoint_closed_arg_t;

typedef struct {
  sl_slist_node_t node;
  sl_cpc_buffer_handle_t *handle;
} sl_cpc_transmit_queue_item_t;

typedef struct {
  sl_slist_node_t node;
  void *data;
  uint16_t data_length;
  sl_cpc_buffer_type_t buffer_type;
} sl_cpc_receive_queue_item_t;

typedef void (*sl_cpc_dispatcher_fnct_t)(void *data);

typedef struct {
  sl_slist_node_t node;
  sl_cpc_dispatcher_fnct_t fnct;
  void *data;
  bool submitted;
} sl_cpc_dispatcher_handle_t;

#ifdef __cplusplus
extern "C"
{
#endif
// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * Open Silicon Labs Internal Service endpoint.
 *
 * @param[in] endpoint_handle  Endpoint Handle.
 *
 * @param[in] id  Endpoint ID.
 *
 * @param[in] flags  Endpoint flags.
 *
 * @param[in] tx_window_size  Endpoint TX Window size.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_open_service_endpoint (sl_cpc_endpoint_handle_t *endpoint_handle,
                                           sl_cpc_service_endpoint_id_t id,
                                           uint8_t flags,
                                           uint8_t tx_window_size);

/***************************************************************************//**
 * Open temporary endpoint.
 *
 * @param[in] endpoint_handle  Endpoint Handle.
 *
 * @param[out] id  Endpoint ID.
 *
 * @param[in] flags  Endpoint flags.
 *
 * @param[in] tx_window_size  Endpoint TX Window size.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_open_temporary_endpoint(sl_cpc_endpoint_handle_t *endpoint_handle,
                                            uint8_t *id,
                                            uint8_t flags,
                                            uint8_t tx_window_size);

/***************************************************************************//**
 * Initialize CPC buffers' handling module.
 ******************************************************************************/
void sli_cpc_init_buffers(void);

/***************************************************************************//**
 * Get a CPC buffer handle.
 *
 * @param[out] handle  Address of the variable that will receive the handle
 *                     pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_buffer_handle(sl_cpc_buffer_handle_t **handle);

/***************************************************************************//**
 * Get a CPC header buffer.
 *
 * @param[out] buffer  Address of the variable that will receive the buffer
 *                     pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_hdlc_header_buffer(void **buffer);

/***************************************************************************//**
 * Get a CPC header and buffer for transmitting a reject packet.
 *
 * @param[out] handle  Address of the variable that will receive the buffer
 *                     pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_reject_buffer(sl_cpc_buffer_handle_t **handle);

/***************************************************************************//**
 * Get a CPC RAW buffer for reception.
 *
 * @param[out] handle  Address of the variable that will receive the RAW buffer
 *                     pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_raw_rx_buffer(void **raw_rx_buffer);

/***************************************************************************//**
 * Free a CPC RAW rx buffer.
 *
 * @param[out] handle  Address of the variable that will receive the RAW buffer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_free_raw_rx_buffer(void *raw_rx_buffer);

/***************************************************************************//**
 * Get a CPC buffer for reception. This also allocates a buffer for the HDLC
 * header and a RX buffer if necessary.
 *
 * @param[out] handle  Address of the variable that will receive the buffer
 *                     pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_buffer_handle_for_rx(sl_cpc_buffer_handle_t **handle);

/***************************************************************************//**
 * Free header, buffer and handle.
 *
 * @param[in] handle  Handle to free.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_drop_buffer_handle(sl_cpc_buffer_handle_t *handle);

/***************************************************************************//**
 * Allocate queue item and push data buffer in receive queue then free
 * header and buffer handle.
 *
 * @param[in] handle  Handle to free.
 * @param[in] head    Queue head pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_push_back_rx_data_in_receive_queue(sl_cpc_buffer_handle_t *handle,
                                                       sl_slist_node_t **head,
                                                       uint16_t data_length);

/***************************************************************************//**
 * Free CPC buffer.
 *
 * @param[in] buffer  Pointer to hdlc header to free.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_free_hdlc_header(void *data);

/***************************************************************************//**
 * Free CPC system command buffer.
 *
 * @param[in] item Pointer to system command buffer to free.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_free_command_buffer(sli_cpc_system_cmd_t *item);

/***************************************************************************//**
 * Get a a system command buffer.
 *
 * @param[out] item Address of the variable that will receive the item pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_system_command_buffer(sli_cpc_system_cmd_t **item);

/***************************************************************************//**
 * Get a receive queue item.
 *
 * @param[out] item  Address of the variable that will receive the item pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_receive_queue_item(sl_cpc_receive_queue_item_t **item);

/***************************************************************************//**
 * Free receive queue item.
 *
 * @param[in] item  Pointer to item to free.
 *
 * @param[out] data  Pointer to variable that will receive the rx buffer pointer.
 *
 * @param[out] data_length  Pointer to variable that will receive the data length.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_free_receive_queue_item(sl_cpc_receive_queue_item_t *item,
                                            void **data,
                                            uint16_t *data_length);

/***************************************************************************//**
 * Free receive queue item and data buffer.
 *
 * @param[in] item  Pointer to item to free.
 ******************************************************************************/
void sli_cpc_drop_receive_queue_item(sl_cpc_receive_queue_item_t *item);

/***************************************************************************//**
 * Get a transmit queue item.
 *
 * @param[out] item  Address of the variable that will receive the item pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_transmit_queue_item(sl_cpc_transmit_queue_item_t **item);

/***************************************************************************//**
 * Free transmit queue item.
 *
 * @param[in] item  Pointer to item to free.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_free_transmit_queue_item(sl_cpc_transmit_queue_item_t *item);

/***************************************************************************//**
 * Get a transmit queue item for S-Frame.
 *
 * @param[out] item  Address of the variable that will receive the item pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_sframe_transmit_queue_item(sl_cpc_transmit_queue_item_t **item);

/***************************************************************************//**
 * Free transmit queue item from S-Frame pool.
 *
 * @param[in] item  Pointer to item to free.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_free_sframe_transmit_queue_item(sl_cpc_transmit_queue_item_t *item);

/***************************************************************************//**
 * Get an endoint.
 *
 * @param[out] endpoint  Address of the variable that will receive the item pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_endpoint(sl_cpc_endpoint_t **endpoint);

/***************************************************************************//**
 * Free endpoint.
 *
 * @param[in] endpoint  Pointer to endpoint to free.
 ******************************************************************************/
void sli_cpc_free_endpoint(sl_cpc_endpoint_t *endpoint);

/***************************************************************************//**
 * Get endpoint closed argument item.
 *
 *@param[out] endpoint  Address of the variable that will receive the argument
 *                      item pointer.
 *
 *@return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_closed_arg(sl_cpc_endpoint_closed_arg_t **arg);

/***************************************************************************//**
 * Free endpoint closed argument.
 *
 * @param[in] endpoint  Pointer to endpoint to free.
 ******************************************************************************/
void sli_cpc_free_closed_arg(sl_cpc_endpoint_closed_arg_t *arg);

/***************************************************************************//**
 * Signal process needed.
 ******************************************************************************/
void sli_cpc_signal_event(sl_cpc_signal_type_t signal_type);

/***************************************************************************//**
 * Push back a list item containing an allocated buffer handle.
 * This list must then be popped using the macro SLI_CPC_POP_BUFFER_HANDLE_LIST.
 ******************************************************************************/
void sli_cpc_push_back_buffer_handle(sl_slist_node_t **head, sl_slist_node_t *item, sl_cpc_buffer_handle_t *buf_handle);

/***************************************************************************//**
 * Push a list item containing an allocated buffer handle.
 * This list must then be popped using the macro SLI_CPC_POP_BUFFER_HANDLE_LIST.
 ******************************************************************************/
void sli_cpc_push_buffer_handle(sl_slist_node_t **head, sl_slist_node_t *item, sl_cpc_buffer_handle_t *buf_handle);

/***************************************************************************//**
 * Get a buffer to store a security tag.
 *
 * @param[out] tag_buffer  Address of the variable that will receive the argument
 *                         tag_buffer pointer.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_get_security_tag_buffer(void **tag_buffer);

/***************************************************************************//**
 * Free a security tag buffer.
 *
 * @param[in] tag_buffer  Pointer to the buffer to free.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sli_cpc_free_security_tag_buffer(void *tag_buffer);

/***************************************************************************//**
 * Initialize CPC System Endpoint.
 *
 * @brief
 *   This function initializes the system endpoint module by opening the system
 *   endpoint. This function must be called after CPC init.
 ******************************************************************************/
sl_status_t sli_cpc_system_init(void);

/***************************************************************************//**
 * Process the system endpoint.
 *
 * @brief
 *   This function reads incoming commands on the system endpoint and processes
 *   them accordingly.
 ******************************************************************************/
void sli_cpc_system_process(void);

/***************************************************************************//**
 * Initialize the dispatcher handle.
 *
 * @param[in] handle  Dispatch queue node.
 ******************************************************************************/
void sli_cpc_dispatcher_init_handle(sl_cpc_dispatcher_handle_t *handle);

/***************************************************************************//**
 * Push function in dispatch queue along with the data to be passed when
 * dispatched.
 *
 * @param[in] handle  Dispatch queue node.
 * @param[in] fnct    Function to be dispatched.
 * @param[in] data    Data to pass to the function.
 ******************************************************************************/
void sli_cpc_dispatcher_push(sl_cpc_dispatcher_handle_t *handle,
                             sl_cpc_dispatcher_fnct_t fnct,
                             void *data);

/***************************************************************************//**
 * Remove function from dispatch queue along with the data to be passed when
 * dispatched.
 *
 * @param[in] handle  Dispatch queue node.
 ******************************************************************************/
void sli_cpc_dispatcher_cancel(sl_cpc_dispatcher_handle_t *handle);

/***************************************************************************//**
 * Process the dispatch queue.
 *
 * @brief
 *   This function empty the dispatch queue by calling all the functions
 *   registered.
 ******************************************************************************/
void sli_cpc_dispatcher_process(void);

/***************************************************************************//**
 * Notify the user that an endpoint on the host has closed.
 ******************************************************************************/
void sli_cpc_remote_disconnected(uint8_t endpoint_id);

/***************************************************************************//**
 * Endpoint was closed, notify the host.
 ******************************************************************************/
sl_status_t sli_cpc_send_disconnection_notification(uint8_t endpoint_id);

/***************************************************************************//**
 * Called on re-transmition of frame.
 ******************************************************************************/
void sli_cpc_on_frame_retransmit(sl_cpc_transmit_queue_item_t* item);

/** @} (end addtogroup cpc) */

#ifdef __cplusplus
}
#endif

#endif // SLI_CPC_H
