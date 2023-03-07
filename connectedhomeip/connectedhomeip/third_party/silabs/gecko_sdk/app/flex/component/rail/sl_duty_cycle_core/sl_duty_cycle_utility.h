/***************************************************************************//**
 * @file
 * @brief sl_duty_cycle_utility.h
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_DUTY_CYCLE_UTILITY_H
#define SL_DUTY_CYCLE_UTILITY_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include "rail_types.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Calculate the proper length of bit for preamble
 *
 * @param bit_rate: what bitrate is used for sending
 *
 * @param duty_cycle_config: address of a preconfigured structure
 *
 * @param bit_length: address of function output (preamble bit length)
 *
 * @return RAIL_Status_t: status code
 ******************************************************************************/
RAIL_Status_t calculate_preamble_bit_length_from_time(const uint32_t bit_rate, RAIL_RxDutyCycleConfig_t * duty_cycle_config, uint16_t * bit_length);

#endif  // SL_DUTY_CYCLE_UTILITY_H
