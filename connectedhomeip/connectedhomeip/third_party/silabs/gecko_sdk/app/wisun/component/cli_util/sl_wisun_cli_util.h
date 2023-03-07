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

#ifndef WISUN_CLI_UTIL_H
#define WISUN_CLI_UTIL_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stddef.h>
#include "sl_status.h"
#include "sl_wisun_types.h"
#include "sl_wisun_api.h"
#include "sl_wisun_trace_util.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * @brief Maximum length of printable data
 *****************************************************************************/
#define APP_UTIL_PRINTABLE_DATA_MAX_LENGTH 64

/**************************************************************************//**
 * @brief Printable data CTX structure type
 *****************************************************************************/
typedef struct {
  char line_buffer[APP_UTIL_PRINTABLE_DATA_MAX_LENGTH + 1];
  const uint8_t *data;
  uint16_t data_left;
  uint8_t is_hex;
  uint8_t line_length;
} app_printable_data_ctx_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * @brief Get the app util string.
 * @details Get the app util string.
 * @param[out] value_str Value string destination
 * @param[in] value Value
 * @param[in] value_enum_list Value enum list
 * @param[in] is_value_signed Value signed indicator
 * @param[in] is_value_hex Value hex indicator
 * @param[in] value_length Value length
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t app_util_get_string(char *const value_str,
                                uint32_t value,
                                const app_enum_t *const value_enum_list,
                                uint8_t is_value_signed,
                                uint8_t is_value_hex,
                                uint8_t value_length);

/**************************************************************************//**
 * @brief Get an integer from app util.
 * @details Get the integer from app util.
 * @param[out] value Dest value
 * @param[in] value_str Destination value string
 * @param[in] value_enum_list Value enum list
 * @param[in] is_value_signed Value signed indicator
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t app_util_get_integer(uint32_t *const value,
                                 const char *value_str,
                                 const app_enum_t *const value_enum_list,
                                 uint8_t is_value_signed);

/**************************************************************************//**
 * @brief Get a string enum from app util.
 * @details Get the enum by string from app util.
 * @param[in] value_enum_list Value enum list
 * @param[in] value Value string
 * @return const app_enum_t* result ptr or NULL on error
 *****************************************************************************/
const app_enum_t* app_util_get_enum_by_string(const app_enum_t *value_enum_list,
                                              const char *const value);

/**************************************************************************//**
 * @brief App util get enum by integer.
 * @details Get the app util enum ptr.
 * @param[in] value_enum_list Value enum list
 * @param[in] value Integer value
 * @return const app_enum_t* result ptr or NULL on error
 *****************************************************************************/
const app_enum_t* app_util_get_enum_by_integer(const app_enum_t *value_enum_list,
                                               uint32_t value);

/**************************************************************************//**
 * @brief App util get MAC address.
 * @details Get the MAC address bytes from app util.
 * @param[out] value Destination MAC address
 * @param[in] value_str Value string
 * @return sl_status_t SL_STATUS_OK on success, otherwise SL_STATUS_INVALID_KEY
 *****************************************************************************/
sl_status_t app_util_get_mac_address(sl_wisun_mac_address_t *value,
                                     const char *value_str);

/**************************************************************************//**
 * @brief App util get MAC address string.
 * @details Get the MAC address string from app util.
 * @param[out] value_str Destination string
 * @param[in] value MAC address value ptr
 * @return sl_status_t
 *****************************************************************************/
sl_status_t app_util_get_mac_address_string(char *value_str,
                                            const sl_wisun_mac_address_t *value);

/**************************************************************************//**
 * @brief App util printable data init.
 * @details Initialize printable data.
 * @param[in] ctx app_printable_data_ctx_t stucture ptr
 * @param[in] data Data buffer
 * @param[in] data_length Data length
 * @param[in] is_hex Hex format indicator
 * @param[in] line_length Line length
 * @return char* Printable data ptr
 *****************************************************************************/
char *app_util_printable_data_init(app_printable_data_ctx_t *const ctx,
                                   const uint8_t *const data,
                                   const uint16_t data_length,
                                   uint8_t is_hex,
                                   uint8_t line_length);

/**************************************************************************//**
 * @brief App util get next printable data.
 * @details Get the next printable data ptr.
 * @param[in] ctx app_printable_data_ctx_t stucture ptr
 * @return char* Printable data ptr
 *****************************************************************************/
char *app_util_printable_data_next(app_printable_data_ctx_t *const ctx);

#endif // WISUN_CLI_UTIL_H
