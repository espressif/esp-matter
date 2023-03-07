/***************************************************************************//**
 * @file
 * @brief
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

#ifndef __SL_IPERF_LOG_H__
#define __SL_IPERF_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "sl_iperf_types.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @addtogroup SL_IPERF_LOG_API iPerf - Utilities
 * @ingroup SL_IPERF
 * @{
 *****************************************************************************/

#define sl_iperf_log_print(__log_ptr, __format, ...)        \
  do {                                                      \
    (__log_ptr)->print(__log_ptr, __format, ##__VA_ARGS__); \
  } while (0)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Init log instance
 * @details Init log instance
 * @param[in,out] log Logger instance
 *****************************************************************************/
void sl_iperf_log_init(sl_iperf_log_t * const log);

/**************************************************************************//**
 * @brief Set stdout printer callback
 * @details Set stdout print function pointer
 * @param[in,out] log Logger instance
 * @param[in] printer Printer function ptr
 *****************************************************************************/
void sl_iperf_log_set_printer(sl_iperf_log_t * const log, sl_iperf_log_print_t printer);

/**************************************************************************//**
 * @brief Set buffer printer callback
 * @details Set buff print function pointer
 * @param[in,out] log Logger instance
 * @param[in] buff_printer Buff Printer function ptr
 * @param[in] buff Buffer ptr
 * @param[in] buff_size Buffer size
 *****************************************************************************/
void sl_iperf_log_set_buff_printer(sl_iperf_log_t * const log,
                                   char * const buff,
                                   const size_t buff_size);
/**************************************************************************//**
 * @brief Set log buffer
 * @details Set buffer string instance in log
 * @param[in,out] log Logger instance
 * @param[in] buff Buffer ptr
 * @param[in] size Buffer size
 *****************************************************************************/
void sl_iperf_log_set_buff(sl_iperf_log_t * const log,
                           char * const buff,
                           const size_t size);

/**************************************************************************//**
 * @brief Reset log buffer
 * @details Reset buffer string instance in log
 * @param[in,out] log Logger instance
 *****************************************************************************/
void sl_iperf_log_reset_buff(sl_iperf_log_t * const log);

/**************************************************************************//**
 * @brief Destroy log buffer
 * @details Destroy buffer string instance in log
 * @param[in,out] log Logger instance
 *****************************************************************************/
void sl_iperf_log_destroy_buff(sl_iperf_log_t * const log);

/**************************************************************************//**
 * @brief Is log buff is full
 * @details Indicate log buffer full state
 * @param[in] log Logger instance
 *****************************************************************************/
bool sl_iperf_log_buff_is_full(sl_iperf_log_t * const log);

/** @}*/

#ifdef __cplusplus
}
#endif
#endif
