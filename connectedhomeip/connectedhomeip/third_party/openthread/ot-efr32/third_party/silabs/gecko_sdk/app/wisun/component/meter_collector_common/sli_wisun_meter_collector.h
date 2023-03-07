/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __SL_WISUN_MC_COMMON_H__
#define __SL_WISUN_MC_COMMON_H__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <inttypes.h>
#include "socket.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Meter LED toggle payload string max lenght
#define SL_WISUN_METER_LED_TOGGLE_PAYLOAD_STR_MAX_LEN     (16U)

/// Meter LED0 toggle payload string
#define SL_WISUN_METER_LED0_TOGGLE_PAYLOAD_STR            "LED0"

/// Meter LED1 toogle payload string
#define SL_WISUN_METER_LED1_TOGGLE_PAYLOAD_STR            "LED1"

/// Meter-Collector mutex acquire macro function
#define sl_wisun_mc_mutex_acquire(__hnd) \
  do {                                   \
    __hnd.resource_hnd.lock();           \
  } while (0)

/// Meter-Collector mutex release macro function
#define sl_wisun_mc_mutex_release(__hnd) \
  do {                                   \
    __hnd.resource_hnd.unlock();         \
  } while (0)

/// Release meter collector mutex and return with value helper macro function
#define sl_wisun_mc_release_mtx_and_return_val(__hnd, __retval) \
  do {                                                          \
    __hnd.resource_hnd.unlock();                                \
    return (__retval);                                          \
  } while (0)

/// Release meter collector mutex and return helper macro function
#define sl_wisun_mc_release_mtx_and_return(__hnd) \
  do {                                            \
    __hnd.resource_hnd.unlock();                  \
    return;                                       \
  } while (0)

/// LED0 ID
#define SL_WISUN_METER_LED0               (0U)

/// LED1 ID
#define SL_WISUN_METER_LED1               (1U)

/// LED unknown
#define SL_WISUN_METER_LED_UNKNOWN        (255U)

/// Meter entry type definition
typedef struct sl_wisun_meter_entry {
  /// Used slot indication
  bool used;
  /// Address structure for meter
  wisun_addr_t addr;
  /// Request sent timestamp
  uint32_t req_sent_timestamp;
  /// Response received timestamp
  uint32_t resp_recv_timestamp;
} sl_wisun_meter_entry_t;

/// Mutex lock callback type definition
typedef void (*sl_wisun_mc_lock_t)(void);

/// Initializer type definition
typedef void (*sl_wisun_mc_init_t)(void);

/// Resource handler
typedef struct sl_wisun_mc_resource_hnd {
  /// Lock common resources
  sl_wisun_mc_lock_t lock;
  /// Unlock common resources
  sl_wisun_mc_lock_t unlock;
  /// Initializer
  sl_wisun_mc_init_t init;
} sl_wisun_mc_resource_hnd_t;

typedef struct sl_wisun_meter_request {
  uint16_t length;
  uint8_t *buff;
} sl_wisun_meter_request_t;

/// Collector receive handler type definition
typedef sl_wisun_meter_entry_t * (*sl_wisun_collector_recv_hnd_t)(int32_t);

/// Collector send handler type definition
typedef bool (*sl_wisun_collector_send_hnd_t)(int32_t, sl_wisun_meter_entry_t *, sl_wisun_meter_request_t *);

/// Collector measurement timeout handler type definition
typedef void (*sl_wisun_collector_timeout_hnd_t)(sl_wisun_meter_entry_t *);

/// Get meter entry by address handler type definition
typedef sl_wisun_meter_entry_t * (*sl_wisun_collector_get_meter_entry_t)(const wisun_addr_t * const);

/// Collector handler type definition
typedef struct sl_wisun_collector_hnd {
  /// Resource handler
  sl_wisun_mc_resource_hnd_t resource_hnd;
  /// Receive handler
  sl_wisun_collector_recv_hnd_t recv;
  /// Send handler
  sl_wisun_collector_send_hnd_t send;
  /// Timeout handler
  sl_wisun_collector_timeout_hnd_t timeout;
  /// Get meter entry by address
  sl_wisun_collector_get_meter_entry_t get_meter;
} sl_wisun_collector_hnd_t;

/// Collector handler type definition
typedef struct sl_wisun_meter_hnd {
  /// Resource handler
  sl_wisun_mc_resource_hnd_t resource_hnd;
} sl_wisun_meter_hnd_t;

/// Measurement packet structure
typedef struct sl_wisun_meter_packet {
  /// ID
  uint16_t id;
  /// Temperature [milliCelsius]
  int32_t temperature;
  /// Humidity    [Rel% * 1000]
  uint32_t humidity;
  /// Light       [Lux]
  uint16_t light;
} sl_wisun_meter_packet_t;

#pragma pack(1)
/// Packed meter packet structure for byte stream conversion
typedef struct sl_wisun_meter_packet_packed {
  /// ID
  uint16_t id;
  /// Temperature [milliCelsius]
  int32_t temperature;
  /// Humidity    [Rel% * 1000]
  uint32_t humidity;
  /// Light       [Lux]
  uint16_t light;
} sl_wisun_meter_packet_packed_t;
#pragma pack()

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Init Collector handler.
 * @details Initialize handler structure
 * @param[in,out] hnd Handler
 *****************************************************************************/
void sl_wisun_collector_init_hnd(sl_wisun_collector_hnd_t *hnd);

/**************************************************************************//**
 * @brief Set Collector initializer function.
 * @details Set initializer function in the resource handler of the collector
 * @param[in,out] hnd Handler
 * @param[in] initializer Initializer
 *****************************************************************************/
void sl_wisun_collector_set_initializer(sl_wisun_collector_hnd_t *hnd,
                                        sl_wisun_mc_init_t initializer);

/**************************************************************************//**
 * @brief Set callback functions for handler.
 * @details If the callback is NULL, the default handler will be set
 * @param[in,out] hnd Handler
 * @param[in] receiver Receiver callback
 * @param[in] sender Sender Callback
 * @param[in] timeout_hnd Timeout handler
 * @param[in] get_meter_hnd Get meter callback
 *****************************************************************************/
void sl_wisun_collector_set_handler(sl_wisun_collector_hnd_t *hnd,
                                    sl_wisun_collector_recv_hnd_t receiver,
                                    sl_wisun_collector_send_hnd_t sender,
                                    sl_wisun_collector_timeout_hnd_t timeout_hnd,
                                    sl_wisun_collector_get_meter_entry_t get_meter);

/**************************************************************************//**
 * @brief Init Meter handler.
 * @details Initialize handler structure
 * @param[in,out] hnd Handler
 *****************************************************************************/
void sl_wisun_meter_init_hnd(sl_wisun_meter_hnd_t *hnd);

/**************************************************************************//**
 * @brief Set Meter initializer function.
 * @details Set initializer function in the resource handler of the meter
 * @param[in,out] hnd Handler
 * @param[in] initializer Initializer
 *****************************************************************************/
void sl_wisun_meter_set_initializer(sl_wisun_meter_hnd_t *hnd,
                                    sl_wisun_mc_init_t initializer);

/**************************************************************************//**
* @brief Set measurement request.
* @details This request is sent periodically to get
*          measurement data from the meter
* @param[in] req  Measurement request
******************************************************************************/
void sl_wisun_collector_set_measurement_request(const sl_wisun_meter_request_t * const req);

/**************************************************************************//**
 * @brief Init internal token.
 * @details This buffer is used as reference
 * @param[in] token_str Token string from config
 *****************************************************************************/
void sl_wisun_mc_init_token(const char * const token_str);

/**************************************************************************//**
 * @brief Get reference token size.
 * @details return with the reference token size without 0 terminator
 * @return uint16_t token size in bytes
 *****************************************************************************/
uint16_t sl_wisun_mc_get_token_size(void);

/**************************************************************************//**
 * @brief Unpack a packed measurement content byte stream.
 * @details Use the packed structure for conversion
 * @param[out] dest unpacked measurement packet
 * @param[in] src measurement packet which is packed
 * @return const sl_wisun_meter_packet_t* static unpacked measurement packet
 *****************************************************************************/
void sl_wisun_mc_unpack_measurement_packet(sl_wisun_meter_packet_t * const dest,
                                           const sl_wisun_meter_packet_packed_t * const src);

/**************************************************************************//**
 * @brief Pack measurement packet content and convert to byte stream.
 * @details Use the packed structure for conversion
 * @param[out] dest packed measurement packet
 * @param[in] src measurement packet to pack
 * @return const uint8_t* static byte stream buffer
 *****************************************************************************/
void sl_wisun_mc_pack_measurement_packet(sl_wisun_meter_packet_packed_t * const dest,
                                         const sl_wisun_meter_packet_t * const src);

/**************************************************************************//**
 * @brief Print measurement data.
 * @details Print IP address, packet ID, temperature [C], humidity [%], light [lx]
 *          Compatible with packed and unpacked packets as well
 * @param[in] ip_address IP address
 * @param[in] packet packet
 * @param[in] is_packed indicates is it a packed packet or not
 *****************************************************************************/
void sl_wisun_mc_print_mesurement(const char *ip_address,
                                  const void *packet,
                                  const bool is_packed);

/**************************************************************************//**
 * @brief Compare token with arrived bytes in buffer.
 * @details If the token matched, the meter send the measurement packet
 * @param[in] token arrived bytes
 * @param[in] token_size arrived bytes
 * @return true On success
 * @return false On error
 *****************************************************************************/
bool sl_wisun_mc_compare_token(const uint8_t *token, const uint16_t token_size);

/**************************************************************************//**
 * @brief Get LED ID from payload string.
 * @details Helper function
 * @param[in] payload_str Payload string
 * @return const sl_wisun_meter_led_id_t LED ID enum value
 *****************************************************************************/
uint8_t sl_wisun_mc_get_led_id_from_payload(const char *payload_str);

/**************************************************************************//**
 * @brief Get Payload string by LED ID.
 * @details Helper function
 * @param[in] led_id LED ID
 * @return const char* Payload string
 *****************************************************************************/
const char *sl_wisun_mc_get_led_payload_by_id(const uint8_t led_id);
#endif
