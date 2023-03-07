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

#ifndef SL_RAIL_TEST_CONFIG_H
#define SL_RAIL_TEST_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
// <h> RAILtest Configuration

// <s.50 SL_RAIL_TEST_APP_NAME> Application name displayed by CLI on boot
// <i> Default: "RAILtest"
#define SL_RAIL_TEST_APP_NAME  "RAILtest"

// <o SL_RAIL_TEST_MAX_PACKET_LENGTH> Maximum packet length
// <i> Default: 1024
#define SL_RAIL_TEST_MAX_PACKET_LENGTH  2058  //SUN 802.15.4 max frame length + PHR + trailing data

// <o SL_RAIL_TEST_CONTINUOUS_TRANSFER_PERIOD> Continuous transfer period
// <i> Default: 250
#define SL_RAIL_TEST_CONTINUOUS_TRANSFER_PERIOD  250

// <o SL_RAIL_TEST_TX_BUFFER_SIZE> Transmit buffer size
// <i> Default: 512
#define SL_RAIL_TEST_TX_BUFFER_SIZE  4096

// <o SL_RAIL_TEST_RX_BUFFER_SIZE> Receive buffer size
// <i> Default: 512
#define SL_RAIL_TEST_RX_BUFFER_SIZE  4096

// <o SL_RAIL_TEST_BUTTON_LONG_HOLD_DURATION_MS> Button Long Press Duration (milliseconds)
// <i> Default: 1000
#define SL_RAIL_TEST_BUTTON_LONG_HOLD_DURATION_MS  1000

// </h>
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <gpio optional=true> SL_RAIL_TEST_PER
// $[GPIO_SL_RAIL_TEST_PER]
// #define SL_RAIL_TEST_PER_PORT                    gpioPortC
// #define SL_RAIL_TEST_PER_PIN                     2
// [GPIO_SL_RAIL_TEST_PER]$
// <<< sl:end pin_tool >>>

#endif // SL_RAIL_TEST_CONFIG_H
