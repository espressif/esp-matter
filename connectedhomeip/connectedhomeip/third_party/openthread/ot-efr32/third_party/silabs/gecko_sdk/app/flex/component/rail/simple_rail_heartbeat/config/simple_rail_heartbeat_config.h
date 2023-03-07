/***************************************************************************//**
 * @file
 * @brief simple_rail_heartbeat_config.h
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
#ifndef SIMPLE_RAIL_HEARTBEAT_CONFIG_H
#define SIMPLE_RAIL_HEARTBEAT_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
// <o SIMPLE_RAIL_HEARTBEAT_TIME_PERIOD_MS> Simple RAIL Heartbeat Time Period [ms] <0-4294967295>
// <i> Default: 1000
// <i> Define the heartbeat period [ms].
#define SIMPLE_RAIL_HEARTBEAT_TIME_PERIOD_MS     (1000)

// <o SIMPLE_RAIL_HEARTBEAT_TIMER_PRIORITY> Simple RAIL Heartbeat Timer priority <0-255>
// <i> Default: 10
// <i> Define the heartbeat timer priority (0 = highest priority).
#define SIMPLE_RAIL_HEARTBEAT_TIMER_PRIORITY     (10)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

// <<< end of configuration section >>>
#endif  // SIMPLE_RAIL_HEARTBEAT_CONFIG_H
