/***************************************************************************//**
 * @file
 * @brief Coexistence configuration header file
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_RAIL_UTIL_COEX_CONFIG_H
#define SL_RAIL_UTIL_COEX_CONFIG_H

#include "sl_rail_util_coex_common_config.h"

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Coexistence signal identifier configuration
// <h> IEEE802.15.4 only configuration
// <q SL_RAIL_UTIL_COEX_IEEE802154_SIGNAL_IDENTIFIER_ENABLED> Enable coexistence IEEE802.15.4 signal identifier
// <i> Default: 0
#define SL_RAIL_UTIL_COEX_IEEE802154_SIGNAL_IDENTIFIER_ENABLED 0
// </h>
// <h> BLE only Configuration
// <q SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED> Enable coexistence BLE signal identifier
// <i> Default: 0
#define SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED 0
// <o SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_MODE> BLE signal identifier mode
//    <1=> BLE 1Mbps
//    <2=> BLE 2Mbps
// <i> 1: Signal identifier detects BLE 1Mbps signals 2: Signal identifier detects BLE 2Mbps signals
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_MODE 1
// <o SL_RAIL_UTIL_COEX_BLE_MIN_TIME_FOR_HOPPING> Minimum number of microseconds that should be available in the scanning window for hopping to next channel when signal identifier is enabled
// <300-2000:1>
// <i> Default: 300
#define SL_RAIL_UTIL_COEX_BLE_MIN_TIME_FOR_HOPPING 300
// </h>
// <o SL_RAIL_UTIL_COEX_WIFI_TX_ASSERT_LEVEL> Polarity of Wifi Tx signal
//   <1=> High
//   <0=> Low
// <i> Polarity of WiFi Tx signal
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_WIFI_TX_ASSERT_LEVEL 1
// </h>
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

#if SL_RAIL_UTIL_COEX_GNT_ENABLED
// Pin used for grant (GNT) signal
// <gpio optional=true> SL_RAIL_UTIL_COEX_GNT
// $[GPIO_SL_RAIL_UTIL_COEX_GNT]
// #define SL_RAIL_UTIL_COEX_GNT_PORT               gpioPortC
// #define SL_RAIL_UTIL_COEX_GNT_PIN                9
// [GPIO_SL_RAIL_UTIL_COEX_GNT]$
#ifndef SL_RAIL_UTIL_COEX_GNT_PORT
#error "SL_RAIL_UTIL_COEX_GNT undefined"
#endif //SL_RAIL_UTIL_COEX_GNT_PORT
#endif //SL_RAIL_UTIL_COEX_GNT_ENABLED

#if SL_RAIL_UTIL_COEX_PRI_ENABLED
// Pin used for PRIORITY signal
// <gpio optional=true> SL_RAIL_UTIL_COEX_PRI
// $[GPIO_SL_RAIL_UTIL_COEX_PRI]
// #define SL_RAIL_UTIL_COEX_PRI_PORT               gpioPortD
// #define SL_RAIL_UTIL_COEX_PRI_PIN                13
// [GPIO_SL_RAIL_UTIL_COEX_PRI]$
#if !defined(SL_RAIL_UTIL_COEX_PRI_PORT) && !SL_RAIL_UTIL_COEX_DP_ENABLED
#error "SL_RAIL_UTIL_COEX_PRI undefined"
#endif //!defined(SL_RAIL_UTIL_COEX_PRI_PORT) && !SL_RAIL_UTIL_COEX_DP_ENABLED
#endif //SL_RAIL_UTIL_COEX_PRI_ENABLED

#if SL_RAIL_UTIL_COEX_REQ_ENABLED
// Pin used for Request signal
// <gpio optional=true> SL_RAIL_UTIL_COEX_REQ
// $[GPIO_SL_RAIL_UTIL_COEX_REQ]
// #define SL_RAIL_UTIL_COEX_REQ_PORT               gpioPortC
// #define SL_RAIL_UTIL_COEX_REQ_PIN                10
// [GPIO_SL_RAIL_UTIL_COEX_REQ]$
#ifndef SL_RAIL_UTIL_COEX_REQ_PORT
#error "SL_RAIL_UTIL_COEX_REQ undefined"
#endif //SL_RAIL_UTIL_COEX_REQ_PORT
#endif //SL_RAIL_UTIL_COEX_REQ_ENABLED

#if SL_RAIL_UTIL_COEX_PWM_REQ_ENABLED && SL_RAIL_UTIL_COEX_REQ_SHARED
// Pin used for PWM Request signal
// <gpio optional=true> SL_RAIL_UTIL_COEX_PWM_REQ
// $[GPIO_SL_RAIL_UTIL_COEX_PWM_REQ]
// #define SL_RAIL_UTIL_COEX_PWM_REQ_PORT           gpioPortC
// #define SL_RAIL_UTIL_COEX_PWM_REQ_PIN            11
// [GPIO_SL_RAIL_UTIL_COEX_PWM_REQ]$
#ifndef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
#error "SL_RAIL_UTIL_COEX_PWM_REQ undefined"
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_ENABLED && SL_RAIL_UTIL_COEX_REQ_SHARED

#if SL_RAIL_UTIL_COEX_RHO_ENABLED
// Pin used for Radio Holdoff signal
// <gpio optional=true> SL_RAIL_UTIL_COEX_RHO
// $[GPIO_SL_RAIL_UTIL_COEX_RHO]
// #define SL_RAIL_UTIL_COEX_RHO_PORT               gpioPortC
// #define SL_RAIL_UTIL_COEX_RHO_PIN                8
// [GPIO_SL_RAIL_UTIL_COEX_RHO]$
#ifndef SL_RAIL_UTIL_COEX_RHO_PORT
#error "SL_RAIL_UTIL_COEX_RHO undefined"
#endif //SL_RAIL_UTIL_COEX_RHO_PORT
#endif //SL_RAIL_UTIL_COEX_RHO_ENABLED

#if SL_RAIL_UTIL_COEX_DP_ENABLED
// Pin used for Directional Priority signal
// <prs gpio=true optional=true> SL_RAIL_UTIL_COEX_DP_OUT
// $[PRS_SL_RAIL_UTIL_COEX_DP_OUT]
// #define SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL             3

// PRS CH3 on PD12
// #define SL_RAIL_UTIL_COEX_DP_OUT_PORT                gpioPortD
// #define SL_RAIL_UTIL_COEX_DP_OUT_PIN                 12
// [PRS_SL_RAIL_UTIL_COEX_DP_OUT]$

// Directional Priority timer module
// <timer channel=CC0 optional=true> SL_RAIL_UTIL_COEX_DP_TIMER
// $[TIMER_SL_RAIL_UTIL_COEX_DP_TIMER]
// #define SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL          TIMER1
// #define SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL_NO       1
#ifndef SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL
#error "SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL undefined"
#endif //SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL

// #define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_CHANNEL         1
// [TIMER_SL_RAIL_UTIL_COEX_DP_TIMER]$
#ifndef SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL
#error "SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL undefined"
#endif //SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL
#endif //SL_RAIL_UTIL_COEX_DP_ENABLED

#if SL_RAIL_UTIL_COEX_RX_ACTIVE_ENABLED
// Pin used for RX active signal
// <prs gpio=true optional=true> SL_RAIL_UTIL_COEX_RX_ACTIVE
// $[PRS_SL_RAIL_UTIL_COEX_RX_ACTIVE]
// #define SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL      8

// PRS CH8 on PD13
// #define SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT         gpioPortD
// #define SL_RAIL_UTIL_COEX_RX_ACTIVE_PIN          13
// [PRS_SL_RAIL_UTIL_COEX_RX_ACTIVE]$
#ifndef SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT
#error "SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT undefined"
#endif //SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT
#ifndef SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL
#error "SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL undefined"
#endif //SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL
#endif //SL_RAIL_UTIL_COEX_RX_ACTIVE_ENABLED

#if (SL_RAIL_UTIL_COEX_IEEE802154_SIGNAL_IDENTIFIER_ENABLED || SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED)
// Pin used for WiFi Tx
// <prs gpio=true optional=true> SL_RAIL_UTIL_COEX_WIFI_TX
// $[GPIO_SL_RAIL_UTIL_COEX_WIFI_TX]
// #define SL_RAIL_UTIL_COEX_WIFI_TX_CHANNEL     4

// #define SL_RAIL_UTIL_COEX_WIFI_TX_PORT        gpioPortC
// #define SL_RAIL_UTIL_COEX_WIFI_TX_PIN         9
// [GPIO_SL_RAIL_UTIL_COEX_WIFI_TX]$
#ifndef SL_RAIL_UTIL_COEX_WIFI_TX_PORT
#error "SL_RAIL_UTIL_COEX_WIFI_TX_PORT undefined"
#endif // SL_RAIL_UTIL_COEX_WIFI_TX_PORT
#endif // SL_RAIL_UTIL_COEX_IEEE802154_SIGNAL_IDENTIFIER_ENABLED || SL_RAIL_UTIL_COEX_BLE_SIGNAL_IDENTIFIER_ENABLED

// <<< sl:end pin_tool >>>

#endif // SL_RAIL_UTIL_COEX_CONFIG_H
