/***************************************************************************//**
 * @file
 * @brief Wi-SUN CoAP resource handler
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __SL_WISUN_COAP_RESOURCE_MANAGER_H__
#define __SL_WISUN_COAP_RESOURCE_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "sl_status.h"
#include "sl_wisun_coap.h"
#include "socket.h"

/**************************************************************************//**
 * @addtogroup SL_WISUN_COAP_API CoAP
 * @ingroup SL_WISUN_COAP
 * @{
 *****************************************************************************/

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Standard port for Coap Resource Discovery
#define SL_WISUN_COAP_DISCOVERY_PORT              5683U

/// Maximum URI string length
#define SL_WISUN_COAP_RESOURCE_URI_MAX_SIZE       64U
/// Maximum resource type string length
#define SL_WISUN_COAP_RESOURCE_RT_MAX_SIZE        64U
/// Maximum interface string length
#define SL_WISUN_COAP_RESOURCE_IF_MAX_SIZE        64U

/// Resource discovery standard request
#define SL_WISUN_COAP_RESOURCE_DISCOVERY_STR      ".well-known/core"

/// maximum payload size for resource discovery response
#define SL_WISUN_COAP_RESOURCE_MAXIMUM_PAYLOAD    1024
/// return value interpretation
#define SL_WISUN_COAP_RESOURCE_NOT_ENOUGH_SPACE   (-1)

/**************************************************************************//**
 * @defgroup SL_WISUN_COAP_RESOURCE_TYPES CoAP type definitions
 * @ingroup SL_WISUN_COAP
 * @{
 *****************************************************************************/

/// Resource table data
typedef struct sl_wisun_coap_resource_data {
  /// resource URI path string
  char* uri_path;
  /// resource type string
  char* resource_type;
  /// inteface type string
  char* interface;
  /// length of URI path
  uint16_t uri_path_len;
  /// length of resource type
  uint16_t resource_type_len;
  /// length of interface type
  uint16_t interface_len;
} sl_wisun_coap_resource_data_t;

/// Enum for the resource discovery visibility
typedef enum sl_wisun_coap_discovery_visibility {
  /// resource is returned in a resource discovery request
  discoverable,
  /// resource is not returned in a resource discovery request
  undiscoverable
} sl_wisun_coap_discovery_visibility_t;

/// Resource table entry
typedef struct sl_wisun_coap_resource {
  /// Resource data
  sl_wisun_coap_resource_data_t data;
  /// resource slot used
  bool used;
  /// resource discoverable
  sl_wisun_coap_discovery_visibility_t discoverable;
} sl_wisun_coap_resource_t;

/// type for the buffer insertion mode
typedef enum insert_mode {
  /// Copy data into the destination buffer and calculate space consumed
  copy_and_calc,
  /// Only calculate space required but not operation
  only_calc
} insert_mode_t;

/** @} (end SL_WISUN_COAP_RESOURCE_TYPES) */

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Initialization of the CoAP Resource Discovery internals.
 *****************************************************************************/
void sl_wisun_coap_resource_init(void);

/**************************************************************************//**
 * @brief Initialization of the CoAP Resource Discovery internals.
 *****************************************************************************/
void sl_wisun_coap_resource_init_table(void);

/**************************************************************************//**
 * @brief Register a new resource into the CoAP Resource table
 * @param[in] resource_data the new resource's infomation (name, type etc.)
 * @param[in] discoverable discoverable: entry is returned upon query
 *                         undiscoverable: entry is not returned upon query
 * @return True if the registration is successful
 *****************************************************************************/
bool sl_wisun_coap_resource_register(const sl_wisun_coap_resource_data_t* resource_data,
                                     const sl_wisun_coap_discovery_visibility_t discoverable);

/**************************************************************************//**
 * @brief Remove a resource from the CoAP Resource table referenced by name
 * @param[in] uri_path URI path of the resource to be removed
 * @param[in] uri_path_len length of the URI path name
 * @return True if removal is successful
 *****************************************************************************/
bool sl_wisun_coap_resource_remove_uri(const char* uri_path, const uint16_t uri_path_len);

/**************************************************************************//**
 * @brief Get a resource referenced by name
 * @param[in] uri_path URI path of the resource to be returned
 * @param[in] uri_path_len length of the URI path name
 * @return pointer to the resource table element found or NULL if not found
 *****************************************************************************/
sl_wisun_coap_resource_t* sl_wisun_coap_resource_get_uri(const char* uri_path,
                                                         const uint16_t uri_path_len);

/**************************************************************************//**
 * @brief Determine if a packet is a standard Resource Discovery request
 * @details the request is expected to be a GET .well-known/core request
 * @param[in] parsed_msg the CoAP packet to check
 * @return True if the packet is a CoAP Resource Discovery request
 *****************************************************************************/
bool sl_wisun_coap_resource_discovery_requested(const sl_wisun_coap_packet_t* parsed_msg);

/**************************************************************************//**
 * @brief Constructs the Resource Discovery response on CoRE Link format
 * @details By calling with the calc_size_only parameter set to true, the function
 * will only return the space needed for the response. Otherwise it will copy the
 * response into the buffer or the elements that would still fit in.
 * @param[out] buffer the destiantion bufer
 * @param[in] buffer_size the capactiy of destination buffer in bytes
 * @param[in] calc_size_only if true, only the required space is calculated
 * @return The space needed/consumed for the response
 *****************************************************************************/
int16_t sl_wisun_coap_resource_discovery(char* buffer,
                                         const uint16_t buffer_size,
                                         insert_mode_t op_mode);

/* send a response to a resource discovery request */

/**************************************************************************//**
 * @brief Compose the CoAP packet as a response of a Resource Discovers request
 * @param[in] sock_id the socket to use for transmitting the response
 * @param[in] message_id message ID from the sender
 * @param[in] dest_addr points to the address/port of the destination node
 * @return True if the packet is a CoAP Resource Discovery request
 *****************************************************************************/
sl_status_t sl_wisun_coap_resource_discovery_response(const int32_t sock_id,
                                                      const uint16_t message_id,
                                                      const sockaddr_in6_t *dest_addr);

/** @}*/

#ifdef __cplusplus
}
#endif

#endif /*__SL_WISUN_COAP_RESOURCE_MANAGER_H__*/
