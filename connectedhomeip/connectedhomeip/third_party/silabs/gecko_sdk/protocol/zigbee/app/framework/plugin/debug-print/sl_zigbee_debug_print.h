/***************************************************************************//**
 * @file
 * @brief ZigBee specific debug print macros and API declarations.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_ZIGBEE_DEBUG_PRINT_H
#define SL_ZIGBEE_DEBUG_PRINT_H

#include <stdbool.h>
#include <stdint.h>
#include "sl_status.h"
#include "sl_zigbee_debug_print_config.h"

/**
 * @defgroup debug-print Debug Print
 * @ingroup component
 * @brief API and Callbacks for the Debug Print Component
 */

/**
 * @addtogroup debug-print
 * @{
 */

//------------------------------------------------------------------------------
// Internal APIs and defines

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void sli_zigbee_debug_print(uint32_t group_type, const char* format, ...);
#endif

#ifdef EMBER_TEST
#include <stdint.h>
#include "printf-sim.h"
#define local_printf printf_sim
#define local_vprintf vprintf_sim
#else
#ifdef EZSP_HOST
#include <stdio.h>
#else // EZSP_HOST
#include "printf.h"
#endif // EZSP_HOST
#define local_printf printf
#define local_vprintf vprintf
#endif

//------------------------------------------------------------------------------
// Public types and APIs

enum sl_zigbee_debug_print_type {
  SL_ZIGBEE_DEBUG_PRINT_TYPE_STACK             = 0x01,
  SL_ZIGBEE_DEBUG_PRINT_TYPE_CORE              = 0x02,
  SL_ZIGBEE_DEBUG_PRINT_TYPE_APP               = 0x04,
  SL_ZIGBEE_DEBUG_PRINT_TYPE_ZCL               = 0x08,
  SL_ZIGBEE_DEBUG_PRINT_TYPE_LEGACY_AF_DEBUG   = 0x10,
};

/**
 * @name API
 * @{
 */
/** Enable groups in debug prints.
 *
 * @param group_type Ver.: always
 * @param enabled value Ver.: always
 *
 * @return sl_status_t status code
 *
 */
sl_status_t sl_zigbee_debug_print_enable_group(uint32_t group_type, bool enable);

void sli_zigbee_debug_print_buffer(uint32_t group_type,
                                   const uint8_t *buffer,
                                   uint16_t buffer_length,
                                   bool with_space);

/** @} */ // end of name APIs
#if (SL_ZIGBEE_DEBUG_STACK_GROUP_ENABLED == 1)
#define sl_zigbee_stack_debug_print(...) sli_zigbee_debug_print(((uint32_t)SL_ZIGBEE_DEBUG_PRINT_TYPE_STACK), __VA_ARGS__)
#else
#define sl_zigbee_stack_debug_print(...)
#endif

#if (SL_ZIGBEE_DEBUG_CORE_GROUP_ENABLED == 1)
#define sl_zigbee_core_debug_print(...) sli_zigbee_debug_print(((uint32_t)SL_ZIGBEE_DEBUG_PRINT_TYPE_CORE), __VA_ARGS__)
#else
#define sl_zigbee_core_debug_print(...)
#endif

#if (SL_ZIGBEE_DEBUG_APP_GROUP_ENABLED == 1)
#define sl_zigbee_app_debug_print(...) sli_zigbee_debug_print(((uint32_t)SL_ZIGBEE_DEBUG_PRINT_TYPE_APP), __VA_ARGS__)
#define sl_zigbee_app_debug_print_buffer(buffer, length, with_space) sli_zigbee_debug_print_buffer(((uint32_t)SL_ZIGBEE_DEBUG_PRINT_TYPE_APP), (buffer), (length), (with_space))
#else
#define sl_zigbee_app_debug_print(...)
#define sl_zigbee_app_debug_print_buffer(buffer, length, with_space)
#endif

#if (SL_ZIGBEE_DEBUG_ZCL_GROUP_ENABLED == 1)
#define sl_zigbee_zcl_debug_print(...) sli_zigbee_debug_print(((uint32_t)SL_ZIGBEE_DEBUG_PRINT_TYPE_ZCL), __VA_ARGS__)
#else
#define sl_zigbee_zcl_debug_print(...)
#endif

#if (SL_ZIGBEE_DEBUG_PRINTS_LEGACY_AF_DEBUG_ENABLED == 1)
#define sl_zigbee_legacy_af_debug_print(...) sli_zigbee_debug_print(((uint32_t)SL_ZIGBEE_DEBUG_PRINT_TYPE_LEGACY_AF_DEBUG), __VA_ARGS__)
#else
#define sl_zigbee_debug_print(...)
#endif

/** @} */ // end of debug-print

#endif // SL_ZIGBEE_DEBUG_PRINT_H
