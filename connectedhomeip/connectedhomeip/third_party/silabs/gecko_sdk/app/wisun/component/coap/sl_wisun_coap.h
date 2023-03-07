/***************************************************************************//**
 * @file
 * @brief Wi-SUN CoAP handler
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

#ifndef __SL_WISUN_COAP_H__
#define __SL_WISUN_COAP_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sn_config.h"
#include "sn_coap_protocol.h"
#include "sn_coap_header.h"
#include "sli_wisun_coap_mem.h"

/**************************************************************************//**
 * @addtogroup SL_WISUN_COAP_API CoAP
 * @ingroup SL_WISUN_COAP
 * @{
 *****************************************************************************/

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @defgroup SL_WISUN_COAP_TYPES CoAP type definitions
 * @ingroup SL_WISUN_COAP
 * @{
 *****************************************************************************/
/// Maximum size of the URI path string
#define SL_WISUN_COAP_URI_PATH_MAX_SIZE   (128U)

/// Wi-SUN CoAP handler for Wi-SUN component
typedef struct coap_s sl_wisun_coap_handle_t;

/// Wi-SUN CoAP malloc function pointer typedef
typedef void *(*sl_wisun_coap_malloc_t)(uint16_t size);

/// Wi-SUN CoAP free function pointer typedef
typedef void (*sl_wisun_coap_free_t)(void *mem);

/// Wi-SUN CoAP version typedef
typedef coap_version_e sl_wisun_coap_version_t;

/// Wi-SUN CoAP TX callback function pointer typedef
typedef uint8_t (*sl_wisun_coap_tx_callback)(uint8_t *packet_data, uint16_t packet_data_size,
                                             sn_nsdl_addr_s *addr, void *param);
/// Wi-SUN CoAP RX callback function pointer typedef
typedef int8_t (*sl_wisun_coap_rx_callback)(sn_coap_hdr_s *header, sn_nsdl_addr_s *addr, void *param);

/// Wi-SUN CoAP message typedef
typedef sn_coap_hdr_s sl_wisun_coap_packet_t;

/// Wi-SUN CoAP message code typedef
typedef sn_coap_msg_code_e sl_wisun_coap_message_code_t;

/// Wi-SUN CoAP message type typedef
typedef sn_coap_msg_type_e sl_wisun_coap_message_type_t;

/// Wi-SUN CoAP option number typedef
typedef sn_coap_option_numbers_e sl_wisun_coap_option_num_t;

/// Wi-SUN CoAP option list typedef
typedef sn_coap_options_list_s sl_wisun_coap_option_list_t;

/// Wi-SUN CoAP descriptor structure
typedef struct sl_wisun_coap {
  /// lib handler
  sl_wisun_coap_handle_t*    handler;
  /// malloc function
  sl_wisun_coap_malloc_t     malloc;
  /// free function
  sl_wisun_coap_free_t       free;
  /// TX callback
  sl_wisun_coap_tx_callback  tx_callback;
  /// RX callback
  sl_wisun_coap_rx_callback  rx_callback;
  /// CoAP version
  sl_wisun_coap_version_t    version;
} sl_wisun_coap_t;

/** @} (end SL_WISUN_COAP_TYPES) */

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Initialize Wi-SUN CoAP.
 * @details Set the Wi-SUN CoAP internal descriptor.
 * @param[in] tx_callback TX callback, if it's NULL, the default callback is applied
 * @param[in] rx_callback RX callback, if it's NULL, the default callback is applied
 * @param[in] version CoAP version
 * @return true Proper initialization
 * @return false Error
 *****************************************************************************/
bool sl_wisun_coap_init(const sl_wisun_coap_tx_callback tx_callback,
                        const sl_wisun_coap_rx_callback rx_callback,
                        const sl_wisun_coap_version_t   version);

/**************************************************************************//**
 * @brief Initialize Wi-SUN CoAP default.
 * @details Initializes the internal descriptor with default values.
 * @return true Proper initialization
 * @return false Error
 *****************************************************************************/
static inline bool sl_wisun_coap_init_default(void)
{
  return sl_wisun_coap_init(NULL, NULL, COAP_VERSION_1);
}

/**************************************************************************//**
 * @brief Get lib state.
 * @details Gets the initialized state of the CoAP library to decide whether the init is needed.
 * @return true Initialized
 * @return false Not initialized
 *****************************************************************************/
bool sl_wisun_coap_is_initialized(void);

/**************************************************************************//**
 * @brief Implement malloc.
 * @details OS-dependent thread-safe implementation.
 * @param size size for malloc
 * @return void* the memory pointer
 *****************************************************************************/
void *sl_wisun_coap_malloc(uint16_t size);

/**************************************************************************//**
 * @brief Free Wi-SUN CoAP.
 * @details OS-dependent thread-safe implementation.
 * @param addr address ptr
 *****************************************************************************/
void sl_wisun_coap_free(void *addr);

/**************************************************************************//**
 * @brief CoAP parser wrapper function.
 * @details Used sn_coap_parser
 * @param[in] packet_data_len packet data size
 * @param[in] packet_data packet data ptr
 *****************************************************************************/
sl_wisun_coap_packet_t* sl_wisun_coap_parser(uint16_t packet_data_len,
                                             uint8_t *packet_data);

/**************************************************************************//**
 * @brief CoAP packet calc size wrapper.
 * @details Used sn_coap_builder_calc_needed_packet_data_size.
 * @param[in] message message ptr
 * @return uint16_t size
 *****************************************************************************/
uint16_t sl_wisun_coap_builder_calc_size(const sl_wisun_coap_packet_t *message);

/**************************************************************************//**
 * @brief CoAP message builder Wi-SUN.
 * @details Used sl_wisun_coap_builder.
 * @param[out] dest_buff destination buffer for raw message
 * @param[in] message message structure
 * @return int16_t On success bytes of built message,
 *                 on failure -1 if CoAP header structure is wrong
 *                            -2 if NULL ptr set as argument
 *****************************************************************************/
int16_t sl_wisun_coap_builder(uint8_t *dest_buff,
                              const sl_wisun_coap_packet_t *message);

/**************************************************************************//**
 * @brief Build generic response for request wrapper function.
 * @details Used sn_coap_build_response.
 * @param[in] req request
 * @param[in] msg_code message code to build
 * @return sl_wisun_coap_header_t* built packet ptr on the heap
 *****************************************************************************/
sl_wisun_coap_packet_t *sl_wisun_coap_build_response(const sl_wisun_coap_packet_t *req,
                                                     sl_wisun_coap_message_code_t msg_code);

/**************************************************************************//**
 * @brief Print CoAP packets and all of attached buffer, payload, token, uri_path.
 * @details Pretty printer function, with hex format option for buffers
 * @param[in] packet packet to print
 * @param[in] hex_format hex format bool to decide buffer print format
 *****************************************************************************/
void sl_wisun_coap_print_packet(const sl_wisun_coap_packet_t *packet,
                                const bool hex_format);

/**************************************************************************//**
 * @brief Get the last parsed URI path.
 * @details Thread safe way to get the URI path from last parsed packet
 * @param dest_buff destination buff
 * @param buff_length destination buffer size
 * @return const char* On success, pointer to the destination buffer, NULL on failure
 *****************************************************************************/
const char *sl_wisun_coap_get_parsed_uri_path(char *dest_buff,
                                              const uint16_t buff_length);

/**************************************************************************//**
 * @brief Get payload.
 * @details Thread-safe deep copy
 * @param[in] source_packet source packet
 * @param[out] dest_buff destination buffer
 * @param[in] buff_length length of destination buffer
 * @return true Success
 * @return false Failure
 *****************************************************************************/
bool sl_wisun_coap_get_payload(const sl_wisun_coap_packet_t *source_packet,
                               uint8_t *dest_buff,
                               const uint16_t buff_length);
/**************************************************************************//**
 * @brief Set payload.
 * @details Set the corresponding packet member ptr and length.
 * @param[out] dest_packet destination packet
 * @param[in] source_buff source buffer
 * @param[in] buff_length source buffer length
 *****************************************************************************/
void sl_wisun_coap_set_payload(sl_wisun_coap_packet_t *dest_packet,
                               uint8_t *source_buff,
                               const uint16_t buff_length);
/**************************************************************************//**
 * @brief Get token.
 * @details Thread-safe deep copy
 * @param[in] source_packet source packet
 * @param[out] dest_buff destination buffer
 * @param[in] buff_length length of destination buffer
 * @return true Success
 * @return false Failure
 *****************************************************************************/
bool sl_wisun_coap_get_token(const sl_wisun_coap_packet_t *source_packet,
                             uint8_t *dest_buff,
                             const uint16_t buff_length);

/**************************************************************************//**
 * @brief Set token.
 * @details Set the corresponding packet member ptr and length.
 * @param dest_packet destination packet
 * @param source_buff source buffer
 * @param buff_length source buffer length
 *****************************************************************************/
void sl_wisun_coap_set_token(sl_wisun_coap_packet_t *dest_packet,
                             uint8_t *source_buff,
                             const uint8_t buff_length);

/**************************************************************************//**
 * @brief Get options list.
 * @details Thread-safe deep copy
 * @param source_packet source packet
 * @param dest destination option list structure
 * @return true Success
 * @return false Failure
 *****************************************************************************/
bool sl_wisun_coap_get_options_list(const sl_wisun_coap_packet_t *source_packet,
                                    sl_wisun_coap_option_list_t *dest);

/**************************************************************************//**
 * @brief Set options list.
 * @details Set the option list pointer.
 * @param[out] dest_packet destination packet ptr
 * @param[in] source source option list
 *****************************************************************************/
void sl_wisun_coap_set_options_list(sl_wisun_coap_packet_t *dest_packet,
                                    sl_wisun_coap_option_list_t *source);

/**************************************************************************//**
 * @brief Get the library handler pointer from the internal handler structure.
 * @details Not thread safe! It is needed only to use Pelion mbed-coap library functions
 * @return const sl_wisun_coap_handle_t* pointer to the lib handler
 *****************************************************************************/
const sl_wisun_coap_handle_t* sl_wisun_coap_get_lib_handler(void);

/**************************************************************************//**
 * @brief Destroy packet.
 * @details It must be used to avoid memory leaks!
 *          Free the all of allocated buffer for packet
 * @param packet packet
 *****************************************************************************/
void sl_wisun_coap_destroy_packet(sl_wisun_coap_packet_t *packet);

/**************************************************************************//**
 * @brief Compare URI path with the stored from the previous parsed packet.
 * @details Thread-safe way to compare URI paths.
 *          The buffer length must be set, eg strlen(uri_path) + 1
 * @param uri_path URI path
 * @param buff_max_len buffer size of the URI path
 * @return true Matched
 * @return false Not matched
 *****************************************************************************/
bool sl_wisun_coap_compare_uri_path(const char *uri_path,
                                    const uint16_t buff_max_len);

/**************************************************************************//**
 * @brief Helper function to compare two strings with known lengths.
 * @param str1 pointer to the first string
 * @param len1 length of the first string
 * @param str2 pointer to the other string
 * @param len2 length of the other string
 * @return true Matched
 * @return false Not matched
 *****************************************************************************/
bool sl_wisun_coap_compare(const char* str1,
                           const uint16_t len1,
                           const char* str2,
                           const uint16_t len2);

/** @}*/

#ifdef __cplusplus
}
#endif
#endif
