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

#ifndef SL_CPC_H
#define SL_CPC_H

#if defined(__linux__)
#error Wrong platform - this header file is intended for the secondary application
#endif

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include "sl_enum.h"
#include "sl_status.h"
#include "sl_slist.h"
#include "sl_cpc_config.h"

#include <stddef.h>
#include <stdarg.h>

#if (defined(SL_CATALOG_CPC_SECURITY_PRESENT))
#include "sl_cpc_security.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************/ /**
 * @addtogroup cpc CPC
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Data Types

/// @brief Enumeration representing user endpoint.
SL_ENUM(sl_cpc_user_endpoint_id_t){
  SL_CPC_ENDPOINT_USER_ID_0 = 90, ///< User endpoint ID 0
  SL_CPC_ENDPOINT_USER_ID_1 = 91, ///< User endpoint ID 1
  SL_CPC_ENDPOINT_USER_ID_2 = 92, ///< User endpoint ID 2
  SL_CPC_ENDPOINT_USER_ID_3 = 93, ///< User endpoint ID 3
  SL_CPC_ENDPOINT_USER_ID_4 = 94, ///< User endpoint ID 4
  SL_CPC_ENDPOINT_USER_ID_5 = 95, ///< User endpoint ID 5
  SL_CPC_ENDPOINT_USER_ID_6 = 96, ///< User endpoint ID 6
  SL_CPC_ENDPOINT_USER_ID_7 = 97, ///< User endpoint ID 7
  SL_CPC_ENDPOINT_USER_ID_8 = 98, ///< User endpoint ID 8
  SL_CPC_ENDPOINT_USER_ID_9 = 99, ///< User endpoint ID 9
};

/// @brief Enumeration representing the possible endpoint state.
SL_ENUM(sl_cpc_endpoint_state_t){
  SL_CPC_STATE_OPEN = 0,                      ///< State open
  SL_CPC_STATE_CLOSED,                        ///< State close
  SL_CPC_STATE_CLOSING,                       ///< State closing
  SL_CPC_STATE_ERROR_DESTINATION_UNREACHABLE, ///< Error state, destination unreachable
  SL_CPC_STATE_ERROR_SECURITY_INCIDENT,       ///< Error state, security incident
  SL_CPC_STATE_ERROR_FAULT,                   ///< Error state, fault
  SL_CPC_STATE_FREED,                         ///< State freed
};

/// @brief Enumeration representing the possible configurable options for an endpoint.
SL_ENUM(sl_cpc_endpoint_option_t){
  SL_CPC_ENDPOINT_ON_IFRAME_RECEIVE = 0,     ///< Set callback for iframe received notification
  SL_CPC_ENDPOINT_ON_IFRAME_RECEIVE_ARG,     ///< Set callback argument for iframe received notification
  SL_CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED, ///< Set callback for write complete notification
  SL_CPC_ENDPOINT_ON_ERROR,                  ///< Set callback for error notification
  SL_CPC_ENDPOINT_ON_ERROR_ARG,              ///< Set callback argument for error notification
  // Private options
  SL_CPC_ENDPOINT_ON_UFRAME_RECEIVE,
  SL_CPC_ENDPOINT_ON_UFRAME_RECEIVE_ARG,
  SL_CPC_ENDPOINT_ON_UFRAME_WRITE_COMPLETED,
  SL_CPC_ENDPOINT_ON_POLL,
  SL_CPC_ENDPOINT_ON_POLL_ARG,
  SL_CPC_ENDPOINT_ON_FINAL,
  SL_CPC_ENDPOINT_ON_FINAL_ARG,
};

#define SL_CPC_OPEN_ENDPOINT_FLAG_NONE              0

/***************************************************************************//**
 * Typedef for the user - supplied callback function which is called when
 * CPC is done with the provided buffer.
 *
 * @param endpoint_id   Endpoint ID
 *
 * @param buffer  Pointer to data buffer.
 *
 * @param arg   Transaction user's argument.
 *
 * @param status  Status indicating the transaction result:
 *                  SL_STATUS_OK  Remote should have receive the frame
 *                  SL_STATUS_TRANSMIT_INCOMPLETE Remote has not received the frame.
 ******************************************************************************/
typedef void (*sl_cpc_on_write_completed_t)(sl_cpc_user_endpoint_id_t endpoint_id,
                                            void *buffer,
                                            void *arg,
                                            sl_status_t status);

/***************************************************************************//**
 * Typedef for the user - supplied callback function which is called when
 * CPC receive data on an endpoint.
 *
 * @param endpoint_id   Endpoint ID
 *
 * @param arg   User-specific argument .
 ******************************************************************************/
typedef void (*sl_cpc_on_data_reception_t)(uint8_t endpoint_id, void *arg);

/***************************************************************************//**
 * Typedef for the user-supplied callback function which is called when
 * CPC detects a fatal error on an endpoint.
 *
 * @param endpoint_id   Endpoint ID
 *
 * @param arg   User-specific argument .
 ******************************************************************************/
typedef void (*sl_cpc_on_error_callback_t)(uint8_t endpoint_id, void *arg);

/// @brief Struct representing an CPC endpoint handle.
typedef struct {
  void *ep;           ///< Endpoint object; Do not touch
  uint8_t id;         ///< Endpoint ID; Do not touch
  uint32_t ref_count; ///< Endpoint reference counter; Do not touch
} sl_cpc_endpoint_handle_t;

/// @brief Struct representing CPC Core debug stats.
typedef struct {
  uint32_t rxd_packet;                            ///< Number of packet received
  uint32_t rxd_data_frame;                        ///< Number of frame with payload (dataframe);
  uint32_t rxd_data_frame_queued;                 ///< Number of dataframe with data queued
  uint32_t rxd_data_frame_dropped;                ///< Number of dataframe with data dropped

  uint32_t rxd_supervisory_frame;                 ///< Number of supervisory frame received
  uint32_t rxd_supervisory_frame_processed;       ///< Number of supervisory frame processed
  uint32_t rxd_supervisory_frame_dropped;         ///< Number of supervisory frame dropped

  uint32_t rxd_unnumbered_frame;                  ///< Number of unnumbered frame received
  uint32_t rxd_unnumbered_frame_processed;        ///< Number of unnumbered frame processed
  uint32_t rxd_unnumbered_frame_dropped;          ///< Number of unnumbered frame dropped

  uint32_t rxd_duplicate_data_frame;              ///< Number of duplicate dataframe received
  uint32_t rxd_ack;                               ///< Number of ACK supervisory-frame received
  uint32_t rxd_reject_destination_unreachable;    ///< Number of destination unreachable supervisory-frame received
  uint32_t rxd_reject_seq_mismatch;               ///< Number of out of order supervisory-frame received
  uint32_t rxd_reject_checksum_mismatch;          ///< Number of checksum error supervisory-frame received
  uint32_t rxd_reject_security_issue;             ///< Number of security issue supervisory-frame received
  uint32_t rxd_reject_out_of_memory;              ///< Number of out of memorry supervisory-frame received
  uint32_t rxd_reject_fault;                      ///< Number of fault supervisory-frame received

  uint32_t txd_data_frame;                        ///< Number of dataframe transmitted
  uint32_t txd_ack;                               ///< Number of ACK supervisory-frame transmitted
  uint32_t txd_reject_destination_unreachable;    ///< Number of destination unreachable supervisory-frame transmitted
  uint32_t txd_reject_seq_mismatch;               ///< Number of out of order supervisory-frame transmitted
  uint32_t txd_reject_checksum_mismatch;          ///< Number of checksum error supervisory-frame transmitted
  uint32_t txd_reject_security_issue;             ///< Number of security issue supervisory-frame transmitted
  uint32_t txd_reject_out_of_memory;              ///< Number of out of memorry supervisory-frame transmitted
  uint32_t txd_reject_fault;                      ///< Number of fault supervisory-frame transmitted

  uint32_t retxd_data_frame;                      ///< Number of dataframe retransmitted

  uint32_t frame_transmit_submitted;              ///< Number of frame submitted to the driver
  uint32_t frame_transmit_completed;              ///< Number of frame confirmed sent by the driver
  uint32_t data_frame_transmit_completed;         ///< Number of dataframe-frame confirmed sent by the driver
  uint32_t supervisory_frame_transmit_completed;  ///< Number of supervisory-frame confirmed sent by the driver
} sl_cpc_endpoint_debug_counters_t;

/// @brief Struct representing CPC Core debug counters.
typedef struct {
  uint32_t endpoint_opened;                     ///< Number of endpoint opened
  uint32_t endpoint_closed;                     ///< Number of endpoint closed
  uint32_t rxd_frame;                           ///< Total number of frame received
  uint32_t rxd_valid_iframe;                    ///< Total number of i-frame received
  uint32_t rxd_valid_uframe;                    ///< Total number of u-frame received
  uint32_t rxd_valid_sframe;                    ///< Total number of s-frame received
  uint32_t rxd_data_frame_dropped;              ///< Total number of frame dropped
  uint32_t txd_reject_destination_unreachable;  ///< Total number of unreachable destination transmisted
  uint32_t txd_reject_error_fault;              ///< Total number of fault transmisted
  uint32_t txd_completed;                       ///< Total number of frame confirmed sent by the driver
  uint32_t retxd_data_frame;                    ///< Total number of dataframe retransmission
  uint32_t driver_error;                        ///< Total number of error reported by the driver
  uint32_t driver_packet_dropped;               ///< Total number of frame dropped by the driver
  uint32_t invalid_header_checksum;             ///< Total number of frame received with invalid header checksum
  uint32_t invalid_payload_checksum;            ///< Total number of frame received with invalid frame checksum
} sl_cpc_core_debug_counters_t;

/// @brief Struct representing a memory pool handle.
typedef struct sl_cpc_mem_pool_handle_t {
  void *pool_handle;            ///< Pool handle; Do not touch
  uint32_t used_block_cnt;      ///< Number of block in use
} sl_cpc_mem_pool_handle_t;

/// @brief Struct representing a memory pool debug.
typedef struct {
  sl_cpc_mem_pool_handle_t *buffer_handle;            ///< Buffer handle object memory pool usage
  sl_cpc_mem_pool_handle_t *hdlc_header;              ///< HDLC object memory pool usage
  sl_cpc_mem_pool_handle_t *hdlc_reject;              ///< HDLC reject object  memory pool usage
  sl_cpc_mem_pool_handle_t *rx_buffer;                ///< RX buffer memory pool usage
  sl_cpc_mem_pool_handle_t *endpoint;                 ///< Endpoint object memory pool usage
  sl_cpc_mem_pool_handle_t *rx_queue_item;            ///< RX queue object memory pool usage
  sl_cpc_mem_pool_handle_t *tx_queue_item;            ///< TX queue object memory pool usage
  sl_cpc_mem_pool_handle_t *endpoint_closed_arg_item; ///< Endpoint closing object memory pool usage
  sl_cpc_mem_pool_handle_t *system_command;           ///< System endpoint object memory pool usage
} sl_cpc_debug_memory_t;

/// @brief Struct representing a core debug
#if ((SL_CPC_DEBUG_CORE_EVENT_COUNTERS == 1) \
  || (SL_CPC_DEBUG_MEMORY_ALLOCATOR_COUNTERS == 1))
typedef struct {
#if (SL_CPC_DEBUG_MEMORY_ALLOCATOR_COUNTERS == 1)
  sl_cpc_debug_memory_t memory_pool;            ///< Memory pools usage
#endif
#if (SL_CPC_DEBUG_CORE_EVENT_COUNTERS == 1)
  sl_cpc_core_debug_counters_t core_counters;   ///< Core debug counters
#endif
} sl_cpc_core_debug_t;
#endif

#define SL_CPC_USER_ENDPOINT_ID_START     ((uint8_t)SL_CPC_ENDPOINT_USER_ID_0)
#define SL_CPC_USER_ENDPOINT_ID_END       ((uint8_t)(SL_CPC_ENDPOINT_USER_ID_0 + SL_CPC_USER_ENDPOINT_MAX_COUNT - 1))

#define SL_CPC_FLAG_NO_BLOCK    0x01

#define SL_CPC_APP_DATA_MAX_LENGTH  (4087)

#define SL_CPC_TRANSMIT_WINDOW_MIN_SIZE  1
#define SL_CPC_TRANSMIT_WINDOW_MAX_SIZE  1

/// @brief Global variable that contains the core debug information
#if ((SL_CPC_DEBUG_CORE_EVENT_COUNTERS == 1) \
  || (SL_CPC_DEBUG_MEMORY_ALLOCATOR_COUNTERS == 1))
extern sl_cpc_core_debug_t sl_cpc_core_debug;
#endif

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
 * Initialize CPC module.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sl_cpc_init(void);

/***************************************************************************/ /**
 *  The bare metal process action function.
 ******************************************************************************/
#if !defined(SL_CATALOG_KERNEL_PRESENT)
void sl_cpc_process_action(void);
#endif

/***************************************************************************/ /**
 * Open user endpoint.
 *
 * @param[in] endpoint_handle  Endpoint handle.
 *
 * @param[in] id  Endpoint ID [90 to 99].
 *
 * @param[in] flags   Endpoint type flags.
 *
 * @param[in] tx_window_size  Transmit window size.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sl_cpc_open_user_endpoint(sl_cpc_endpoint_handle_t *endpoint_handle,
                                      sl_cpc_user_endpoint_id_t id,
                                      uint8_t flags,
                                      uint8_t tx_window_size);

/***************************************************************************/ /**
 * Set endpoint option.
 *
 * @param[in] endpoint_handle  Endpoint handle.
 *
 * @param[in] option  Endpoint Option.
 *
 * @param[in] value   Pointer to the option value.
 *
 * @return Status code.
 *
 * @note Public options are:
 *
 * SL_CPC_ENDPOINT_ON_IFRAME_RECEIVE: Set an on iframe receive callback.
 *                                    value is a sl_cpc_on_data_reception_t type.
 *
 * SL_CPC_ENDPOINT_ON_IFRAME_RECEIVE_ARG: Set an on iframe receive argument.
 *
 * SL_CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED: Set an on iframe write completed
 *                                            callback. value is a
 *                                            sl_cpc_on_write_completed_t type.
 *
 * SL_CPC_ENDPOINT_ON_ERROR: Set an on error callback. value is a
 *                           sl_cpc_on_error_callback_t type.
 *
 * SL_CPC_ENDPOINT_ON_ERROR_ARG: Set an on error callback argument.
 ******************************************************************************/
sl_status_t sl_cpc_set_endpoint_option(sl_cpc_endpoint_handle_t *endpoint_handle,
                                       sl_cpc_endpoint_option_t option,
                                       void *value);

/***************************************************************************/ /**
 * Close endpoint.
 *
 * @param[in] endpoint_handle  Endpoint handle.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sl_cpc_close_endpoint(sl_cpc_endpoint_handle_t *endpoint_handle);

/***************************************************************************/ /**
 * Abort read; Allow unblockling task in blocked by a read.
 *
 * @param[in] endpoint_handle   Endpoint handle.
 *
 * @return Status code.
 *
 * @note This function can be called from an ISR.
 ******************************************************************************/
#if defined(SL_CATALOG_KERNEL_PRESENT)
sl_status_t sl_cpc_abort_read(sl_cpc_endpoint_handle_t *endpoint_handle);
#endif

/***************************************************************************/ /**
 * Read data.
 *
 * @param[in] endpoint_handle   Endpoint handle.
 * @param[out] data             Address of the variable that will receive the data pointer.
 * @param[out] data_length      Length of the data contained in the buffer.
 * @param[in] timeout           Timeout in ticks for the read operation. (Requires RTOS).
 *                              Note: No effect if SL_CPC_FLAG_NO_BLOCK is provided as a flag
 * @param[in] flags             Optional flags. i.g. SL_CPC_FLAG_NO_BLOCK.
 *                              Note: SL_CPC_FLAG_NO_BLOCK will cause the function to return
 *                                    SL_STATUS_EMPTY on kernel applications
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sl_cpc_read(sl_cpc_endpoint_handle_t *endpoint_handle,
                        void **data,
                        uint16_t *data_length,
                        uint32_t timeout,
                        uint8_t flags);

/***************************************************************************/ /**
 * Write data.
 *
 * @param[in] endpoint_handle   Endpoint handle.
 * @param[in] data              Pointer to data buffer.
 * @param[in] data_length       Length of the data contained in the buffer.
 * @param[in] flags             Optional flags.
 * @param[in] on_write_completed_arg  Argument that will be passed to on_write_completed().
 *
 * @return Status code.
 *
 * @note This function cannot be called from an ISR.
 ******************************************************************************/
sl_status_t sl_cpc_write(sl_cpc_endpoint_handle_t *endpoint_handle,
                         void* data,
                         uint16_t data_length,
                         uint8_t flag,
                         void *on_write_completed_arg);

/***************************************************************************/ /**
 * Get endpoint state.
 *
 * @param[in] endpoint_handle   Endpoint handle.
 *
 * @return Endpoint state.
 ******************************************************************************/
sl_cpc_endpoint_state_t sl_cpc_get_endpoint_state(sl_cpc_endpoint_handle_t *endpoint_handle);

/***************************************************************************//**
 * Free buffer returned by sl_cpc_read().
 *
 * @param[in] data  Pointer to data buffer to free.
 *
 * @return Status code.
 ******************************************************************************/
sl_status_t sl_cpc_free_rx_buffer(void *data);

// -----------------------------------------------------------------------------
// Internal Prototypes only to be used by Power Manager module.
#if !defined(SL_CATALOG_KERNEL_PRESENT) && defined(SL_CATALOG_POWER_MANAGER_PRESENT)
bool sl_cpc_is_ok_to_sleep(void);
#endif

#if !defined(SL_CATALOG_KERNEL_PRESENT) && defined(SL_CATALOG_POWER_MANAGER_PRESENT)
sl_power_manager_on_isr_exit_t sl_cpc_sleep_on_isr_exit(void);
#endif

/** @} (end addtogroup cpc) */

#ifdef __cplusplus
}
#endif

#endif // SL_CPC_H
