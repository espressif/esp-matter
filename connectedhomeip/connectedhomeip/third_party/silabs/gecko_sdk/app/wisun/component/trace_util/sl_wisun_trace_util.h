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

#ifndef __SL_WISUN_APP_UTIL_H__
#define __SL_WISUN_APP_UTIL_H__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_wisun_ip6string.h"
#include "sl_string.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Thread loop definition
#define SL_WISUN_THREAD_LOOP                      while (1)

/// Structure for using enum on the CLI
typedef struct {
  /// String value
  char *value_str;
  /// Integer value
  uint32_t value;
} app_enum_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

/// Connection status enum
extern const app_enum_t app_wisun_conn_status_enum[];

/// Regulatory domain/PHY enum
extern const app_enum_t app_wisun_phy_reg_domain_enum[];

/// Network size enum
extern const app_enum_t app_wisun_nw_size_enum[];

/// Regulation enum
extern const app_enum_t app_regulation_enum[];

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Initialization of util component
 *****************************************************************************/
void app_wisun_trace_util_init(void);

/**************************************************************************//**
 * @brief Get IP address.
 * @details Uses the internal circular buffer
 * @param[in] value IP address raw byte values
 * @return const char* converted string ptr
 *****************************************************************************/
const char* app_wisun_trace_util_get_ip_address_str(const void *const addr);

/**************************************************************************//**
 * @brief Convert connection state enum values to string.
 * @details Converter function
 * @param[in] val Value to find
 * @return const char* String value
 *****************************************************************************/
const char* app_wisun_trace_util_conn_state_to_str(const uint32_t val);

/**************************************************************************//**
 * @brief Convert regulatory domain enum values to string.
 * @details Converter function
 * @param[in] val Value to find
 * @return const char* String value
 *****************************************************************************/
const char * app_wisun_trace_util_reg_domain_to_str(const uint32_t val);

/**************************************************************************//**
 * @brief Convert network size enum values to string.
 * @details Converter function
 * @param[in] val Value to find
 * @return const char* String value
 *****************************************************************************/
const char * app_wisun_trace_util_nw_size_to_str(const uint32_t val);

/**************************************************************************//**
 * @brief Swapping short unsigned integer endianess
 * @details It swaps the value pointed.
 * @param[in] num The swappng number
 * @return uint16_t integer
 *****************************************************************************/
static inline uint16_t app_wisun_trace_swap_u16(uint16_t num)
{
  return (((num & 0xFF) << 8) | ((num & 0xFF00) >> 8));
}

/**************************************************************************//**
 * @brief Macro to check a status and print a message if the state is not OK
 * @details Use this print a short message on any location where unhandled state
 *          should be noted.
 * @param[in] __status The status value or variable to compare
 * @return None
 *****************************************************************************/
#if !defined(__CHECK_FOR_STATUS)
#define __CHECK_FOR_STATUS(__status)                                       \
  do {                                                                     \
    if (__status != SL_STATUS_OK) {                                        \
      printf("%s() returned = 0x%08lx \n", __PRETTY_FUNCTION__, __status); \
    }                                                                      \
  } while (0)
#endif

#endif /* __SL_WISUN_APP_UTIL_H__ */
