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

#ifndef __SL_WISUN_PING_H__
#define __SL_WISUN_PING_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @addtogroup SL_WISUN_PING_API Ping
 * @{
 *****************************************************************************/

#include <stdint.h>
#include "socket.h"
#include "sl_wisun_ping_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @addtogroup SL_WISUN_PING_API_TYPES Ping API type definitions
 * @ingroup SL_WISUN_PING_API
 * @{
 *****************************************************************************/

/// Maximum count of ping request/response for message queues
#define SL_WISUN_PING_MAX_REQUEST_RESPONSE   (128U)

/// Minimum packet length with 1 byte payload
#define SL_WISUN_PING_MIN_PACKET_LENGTH      (9U)

/// Max packet length
#define SL_WISUN_PING_MAX_PACKET_LENGTH      (SL_WISUN_PING_MIN_PACKET_LENGTH - 1 + SL_WISUN_PING_MAX_PAYLOAD_LENGTH)

#pragma pack(1)
/// Ping echo request packed structure type definitions
typedef struct sl_wisun_ping_echo_request {
  /// type
  uint8_t type;
  /// Code
  uint8_t code;
  /// Checksum
  uint16_t checksum;
  /// Identifier
  uint16_t identifier;
  /// Sequence number
  uint16_t sequence_number;
  /// Payload array
  uint8_t payload[SL_WISUN_PING_MAX_PAYLOAD_LENGTH];
} sl_wisun_ping_echo_request_t;
#pragma pack()

// Ping response type definition
typedef sl_wisun_ping_echo_request_t sl_wisun_ping_echo_response_t;

/// Ping echo request type field value
#define SL_WISUN_PING_TYPE_ECHO_REQUEST             (128U)

/// Ping echo response type field value
#define SL_WISUN_PING_TYPE_ECHO_RESPONSE            (129U)

/// Ping echo request code field value
#define SL_WISUN_PING_CODE_ECHO_REQUEST             (0U)

/// Ping echo response code field value
#define SL_WISUN_PING_CODE_ECHO_RESPONSE            (0U)

/// Dedicated port for ICMPv6 echo messages
#define SL_WISUN_PING_ICMP_PORT                     (0U)

/// Ping status socket error mask
#define SL_WISUN_PING_STATUS_SOCKET_ERROR           (1LU << 1LU)

/// Ping status connect error mask
#define SL_WISUN_PING_STATUS_CONNECT_ERROR          (1LU << 2LU)

/// Ping status send error mask
#define SL_WISUN_PING_STATUS_SEND_ERROR             (1LU << 3LU)

/// Ping interrupt requested mask
#define SL_WISUN_PING_STATUS_INTERRUPT_REQUESTED    (1LU << 4LU)

/// Ping info structure type definition
typedef struct sl_wisun_ping_info {
  /// Identifier
  uint16_t identifier;
  /// Sequence number
  uint16_t sequence_number;
  /// ICMPv6 packet length including header
  uint16_t packet_length;
  /// Response time millisecond
  uint32_t response_time_ms;
  /// Wi-SUN remote address
  wisun_addr_t remote_addr;
  /// Start time stamp
  uint32_t start_time_stamp;
  /// Stop time stamp
  uint32_t stop_time_stamp;
  /// Lost packet flag
  bool lost;
  /// Status flag storage
  uint32_t status;
} sl_wisun_ping_info_t;

/// Statistic ping type definition
typedef struct sl_wisun_ping_stat {
  /// Wi-SUN remote address
  wisun_addr_t remote_addr;
  /// Packet count
  uint16_t packet_count;
  /// Packet length
  uint16_t packet_length;
  /// Lost packet count
  uint16_t lost;
  /// Minimum echo time millisecond
  uint32_t min_time_ms;
  /// Maximum echo time millisecond
  uint32_t max_time_ms;
  /// Average echo time millisecond
  uint32_t avg_time_ms;
} sl_wisun_ping_stat_t;

/// Ping statistic typedef
typedef void (*sl_wisun_ping_stat_hnd_t)(sl_wisun_ping_stat_t *stat);

/// Ping request/response sent handler
typedef void (*sl_wisun_ping_req_resp_done_hnd_t)(sl_wisun_ping_info_t *req, sl_wisun_ping_info_t *resp);

/** @} (end SL_WISUN_PING_API_TYPES) */

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Initialize the ping service module.
 * @details This function initializes the service thread, mutex,
 *          and message queues.
 *****************************************************************************/
void sl_wisun_ping_init(void);

/**************************************************************************//**
 * @brief Send a ping request.
 * @details The function sends a single ICMPv6 request.
 * @param[in] ping_request Ping Request Information
 *****************************************************************************/
void sl_wisun_ping_request(const sl_wisun_ping_info_t * const ping_request);

/**************************************************************************//**
 * @brief Retrieve a ping response.
 * @details The function retrieves a ping response information.
 * @param[out] ping_response Ping Response Information
 *****************************************************************************/
void sl_wisun_ping_response(sl_wisun_ping_info_t * const ping_response);

/**************************************************************************//**
 * @brief Provide a high level ping API.
 * @details The function provides an interface for periodically sending and
 *          receiving ping ICMPv6 packets, and collecting statistic data.
 * @param[in] remote_addr Remote destination address
 * @param[in] packet_count Count of packets
 * @param[in] packet_length ICMPv6 packet length including header
 * @param[in] stat_hnd Custom statistic handler function
 * @param[in] req_resp_sent_hnd Request/Response sent handler function
 * @return true 0% packet lost
 * @return false Higher than 0% packet lost
 *****************************************************************************/
bool sl_wisun_ping(const wisun_addr_t *const remote_addr,
                   const uint16_t packet_count,
                   const uint16_t packet_length,
                   sl_wisun_ping_stat_hnd_t stat_hnd,
                   sl_wisun_ping_req_resp_done_hnd_t req_resp_sent_hnd);

/**************************************************************************//**
 * @brief Stop the current ping process.
 * @details Reset request and response queues and send a special
 *          ping request with interrupt ping status.
 *****************************************************************************/
void sl_wisun_ping_stop(void);

/** @}*/

#ifdef __cplusplus
}
#endif

#endif // __SL_WISUN_PING_H__
