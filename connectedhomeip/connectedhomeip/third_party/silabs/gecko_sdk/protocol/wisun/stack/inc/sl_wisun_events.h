/***************************************************************************//**
 * @file sl_wisun_events.h
 * @brief Wi-SUN Stack events
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
 *    appreciated but is not required.q
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SL_WISUN_EVENT_H
#define SL_WISUN_EVENT_H

#include "sl_wisun_types.h"

/**************************************************************************//**
 * @defgroup SL_WISUN_EVT Wi-SUN API events
 * @ingroup SL_WISUN_API
 * @{
 ******************************************************************************/

/// Wi-SUN Message API indication IDs
typedef enum {
  /// This indication is sent when a connection request has been completed.
  SL_WISUN_MSG_CONNECTED_IND_ID                   = 0x81,
  /// This indication is sent when data has been received on a socket.
  SL_WISUN_MSG_SOCKET_DATA_IND_ID                 = 0x82,
  /// This indication is sent when there is buffered data available on a socket.
  SL_WISUN_MSG_SOCKET_DATA_AVAILABLE_IND_ID       = 0x83,
  /// This indication is sent when a socket connect request  has been completed.
  SL_WISUN_MSG_SOCKET_CONNECTED_IND_ID            = 0x84,
  /// This indication is sent when there is a socket connection request waiting.
  SL_WISUN_MSG_SOCKET_CONNECTION_AVAILABLE_IND_ID = 0x85,
  /// This event is sent when a socket is waiting to be closed.
  SL_WISUN_MSG_SOCKET_CLOSING_IND_ID              = 0x86,
  /// This event is sent when a disconnection request has been completed.
  SL_WISUN_MSG_DISCONNECTED_IND_ID                = 0x87,
  /// This event is sent when a connection to Wi-SUN network has been lost
  /// and the device is trying to regain the connection.
  SL_WISUN_MSG_CONNECTION_LOST_IND_ID             = 0x88,
  /// This event is sent when part of the buffered socket data has been sent.
  SL_WISUN_MSG_SOCKET_DATA_SENT_IND_ID            = 0x89,
  /// This event is sent when an internal stack error has occurred.
  SL_WISUN_MSG_ERROR_IND_ID                       = 0x8A,
  /// This event is sent when the join state changes.
  SL_WISUN_MSG_JOIN_STATE_IND_ID                  = 0x8B,
  /// This event is sent when the network has been updated.
  SL_WISUN_MSG_NETWORK_UPDATE_IND_ID              = 0x8C,
  /// This event is sent when regional regulation transmission level changes.
  SL_WISUN_MSG_REGULATION_TX_LEVEL_IND_ID         = 0x8D,
} sl_wisun_msg_ind_id_t;

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_CONNECTED_IND sl_wisun_msg_connected_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_connected_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_connected_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_connected_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_CONNECTED_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_NETWORK_UPDATE_IND sl_wisun_msg_netwotk_update_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
  /// Indicate what modification have been done
  /// bit 0: Global IP address
  /// bit 1: primary parent
  /// bit 2: secondary parent
  uint32_t flags;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_network_update_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_network_update_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_network_update_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_NETWORK_UPDATE_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_SOCKET_DATA_IND sl_wisun_msg_socket_data_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
  /// ID of the socket
  uint32_t socket_id;
  /// IP address of the sender
  sl_wisun_ip_address_t remote_address;
  /// Port number of the sender
  uint16_t remote_port;
  /// Amount of received data
  uint16_t data_length;
  /// Received data
  uint8_t data[];
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_data_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_socket_data_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_data_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_SOCKET_DATA_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_SOCKET_DATA_AVAILABLE_IND sl_wisun_msg_socket_data_available_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
  /// ID of the socket
  uint32_t socket_id;
  /// Amount of data that can be read
  uint16_t data_length;
  /// Reserved, set to zero
  uint16_t reserved;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_data_available_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_socket_data_available_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_data_available_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_SOCKET_DATA_AVAILABLE_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_SOCKET_CONNECTED_IND sl_wisun_msg_socket_connected_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
  /// ID of the socket
  uint32_t socket_id;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_connected_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_socket_connected_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_connected_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_SOCKET_CONNECTED_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_SOCKET_CONNECTION_AVAILABLE_IND sl_wisun_msg_socket_connection_available_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
  /// ID of the socket
  uint32_t socket_id;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_connection_available_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_socket_connection_available_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_connection_available_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_SOCKET_CONNECTION_AVAILABLE_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_SOCKET_CLOSING_IND sl_wisun_msg_socket_closing_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
  /// ID of the socket
  uint32_t socket_id;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_closing_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_socket_closing_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_closing_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_SOCKET_CLOSING_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_DISCONNECTED_IND sl_wisun_msg_disconnected_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_disconnected_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_disconnected_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_disconnected_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_DISCONNECTED_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_CONNECTION_LOST_IND sl_wisun_msg_connection_lost_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_connection_lost_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_connection_lost_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_connection_lost_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_CONNECTION_LOST_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_SOCKET_DATA_SENT_IND sl_wisun_msg_socket_data_sent_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
  /// ID of the socket
  uint32_t socket_id;
  /// Amount of free space in the transmission buffer
  uint32_t socket_space_left;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_data_sent_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_socket_data_sent_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_socket_data_sent_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_SOCKET_DATA_SENT_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_ERROR_IND sl_wisun_msg_error_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_error_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_error_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_error_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_ERROR_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_JOIN_STATE_IND sl_wisun_msg_join_state_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
  /// Join state
  uint32_t join_state;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_join_state_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_join_state_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_join_state_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_JOIN_STATE_IND) */

/**************************************************************************//**
 * @defgroup SL_WISUN_MSG_REGULATION_TX_LEVEL_IND sl_wisun_msg_regulation_tx_level_ind
 * @{
 ******************************************************************************/

/// Indication message body
SL_PACK_START(1)
typedef struct {
  /// Status of the indication
  uint32_t status;
  /// Sum of transmission durations during last hour in milliseconds
  uint32_t tx_duration_ms;
  /// Transmission level, one value of #sl_wisun_regulation_tx_level_t
  uint8_t tx_level;
  /// Reserved
  uint8_t reserved[3];
} SL_ATTRIBUTE_PACKED sl_wisun_msg_regulation_tx_level_ind_body_t;
SL_PACK_END()

/// Indication message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Indication message body
  sl_wisun_msg_regulation_tx_level_ind_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_msg_regulation_tx_level_ind_t;
SL_PACK_END()

/** @} (end SL_WISUN_MSG_REGULATION_TX_LEVEL_IND) */

/// @brief Wi-SUN event definitions
/// @details This structure contains a Wi-SUN API event and its associated data.
SL_PACK_START(1)
typedef struct {
  /// @brief Common event header
  /// @details This structure contains common information for all events.
  ///          ID of the event is stored in the sl_wisun_msg_header_t.id
  ///          field and is one of the values of @ref sl_wisun_msg_ind_id_t.
  ///          The other fields can be ignored.
  sl_wisun_msg_header_t header;
  /// @brief Event-specific data
  /// @details This structure contains the event-specific data.
  union {
    /// #SL_WISUN_MSG_CONNECTED_IND_ID event data
    sl_wisun_msg_connected_ind_body_t connected;
    /// #SL_WISUN_MSG_SOCKET_DATA_IND_ID event data
    sl_wisun_msg_socket_data_ind_body_t socket_data;
    /// #SL_WISUN_MSG_SOCKET_DATA_AVAILABLE_IND_ID event data
    sl_wisun_msg_socket_data_available_ind_body_t socket_data_available;
    /// #SL_WISUN_MSG_SOCKET_CONNECTED_IND_ID event data
    sl_wisun_msg_socket_connected_ind_body_t socket_connected;
    /// #SL_WISUN_MSG_SOCKET_CONNECTION_AVAILABLE_IND_ID event data
    sl_wisun_msg_socket_connection_available_ind_body_t socket_connection_available;
    /// #SL_WISUN_MSG_SOCKET_CLOSING_IND_ID event data
    sl_wisun_msg_socket_closing_ind_body_t socket_closing;
    /// #SL_WISUN_MSG_DISCONNECTED_IND_ID event data
    sl_wisun_msg_disconnected_ind_body_t disconnected;
    /// #SL_WISUN_MSG_CONNECTION_LOST_IND_ID event data
    sl_wisun_msg_connection_lost_ind_body_t connection_lost;
    /// #SL_WISUN_MSG_SOCKET_DATA_SENT_IND_ID event data
    sl_wisun_msg_socket_data_sent_ind_body_t socket_data_sent;
    /// #SL_WISUN_MSG_ERROR_IND_ID event data
    sl_wisun_msg_error_ind_body_t error;
    /// #SL_WISUN_MSG_JOIN_STATE_IND_ID event data
    sl_wisun_msg_join_state_ind_body_t join_state;
    /// #SL_WISUN_MSG_NETWORK_UPDATE_IND_ID event data
    sl_wisun_msg_network_update_ind_body_t network_update;
    /// #SL_WISUN_MSG_REGULATION_TX_LEVEL_IND_ID event data
    sl_wisun_msg_regulation_tx_level_ind_body_t regulation_tx_level;
  } evt;
} SL_ATTRIBUTE_PACKED sl_wisun_evt_t;
SL_PACK_END()

/** @} (end SL_WISUN_EVT) */

#endif // SL_WISUN_EVENT_H
