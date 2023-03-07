/******************************************************************************/
/**
 * @file
 * @brief Development Kit debugging utilities
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 *
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 *
 * This software is distributed to you in Source Code format and is governed by
 * the sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __DEBUG_CHANNEL_H__
#define __DEBUG_CHANNEL_H__

#include <sl_iostream_swo_itm_8.h>
#include <stdbool.h>
#include <stdint.h>

//******************************************************************************

/**
 * @addtogroup debug
 * @brief A set of utilities for printing to the debug backchannel.
 * @{
 */

/**
 * @brief Builds and prints a message on the debug channel using a SL-style format string
 *
 * @param[in]   debugType       the debug message type
 * @param[in]   formatString    SL-style format string
 */
void sl_debug_binary_format(sl_iostream_swo_itm_8_msg_type_t debugType, const char *formatString, ...);

/**
 * @brief Wrapper for sl_iostream_printf using the debug handle
 *
 * @param formatString
 * @param ...
 */
void sl_debug_printf(const char *formatString, ...);

/**
 * @}
 *
 */

#endif //__DEBUG_CHANNEL_H__
