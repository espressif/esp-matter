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
// <h> Coexistence IEEE802.15.4 PHY Configuration
// <q SL_RAIL_UTIL_COEX_PHY_ENABLED> True: Optimize for Wi-Fi Blocker; False: Optimize for 15.4 Radio Blocker
// <i> True: Optimize for Wi-Fi Blocker; False: Optimize for 15.4 Radio Blocker
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_PHY_ENABLED 1
// <h> Phy Select Signal (Phy Select) Configuration
// <e SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT> Enable/Disable
// <i> True: Runtime Optimization; False: No Runtime Optimization
// <i> Default: 0
#define SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT 0
// <o SL_RAIL_UTIL_COEX_PHY_SELECT_ASSERT_LEVEL> Polarity of PHY Select signal level
//   <1=> High
//   <0=> Low
// <i> Polarity of PHY Select signal
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_PHY_SELECT_ASSERT_LEVEL 1
// <o SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT> Wi-Fi Optimization will be selected for 1-254 ms after 'Wi-Fi Select' deasserted
// <0-255:1>
// <i> Wi-Fi Optimization will be selected for 1-254 ms after 'Wi-Fi Select' deasserted
// <i> Default: 0
#define SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT 0
// </e>
// </h>
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
// Pin used for Directional Priority output signal
// <prs gpio=true optional=true> SL_RAIL_UTIL_COEX_DP_OUT
// $[PRS_SL_RAIL_UTIL_COEX_DP_OUT]
// #define SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL             3

// PRS CH3 on PD12
// #define SL_RAIL_UTIL_COEX_DP_OUT_PORT                gpioPortD
// #define SL_RAIL_UTIL_COEX_DP_OUT_PIN                 12
// #define SL_RAIL_UTIL_COEX_DP_OUT_LOC                 11
// [PRS_SL_RAIL_UTIL_COEX_DP_OUT]$

// Inverted REQUEST PRS channel
// <prs> SL_RAIL_UTIL_COEX_DP_REQUEST_INV
// $[PRS_SL_RAIL_UTIL_COEX_DP_REQUEST_INV]
// #define SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL 4

// [PRS_SL_RAIL_UTIL_COEX_DP_REQUEST_INV]$

// Directional PRIORITY Timer module
// <timer channel=CC0 optional=true> SL_RAIL_UTIL_COEX_DP_TIMER
// $[TIMER_SL_RAIL_UTIL_COEX_DP_TIMER]
// #define SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL          TIMER1
// #define SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL_NO       1
#ifndef SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL
#error "SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL undefined"
#endif //SL_RAIL_UTIL_COEX_DP_TIMER_PERIPHERAL

// #define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_CHANNEL         1
// TIMER1 CC1 on PC11
// #define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT            gpioPortC
// #define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN             10
// #define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_LOC             15
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

#if SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
// Pin used for WiFi Blocker optimized PHY select signal
// <gpio optional=true> SL_RAIL_UTIL_COEX_PHY_SELECT
// $[GPIO_SL_RAIL_UTIL_COEX_PHY_SELECT]
// #define SL_RAIL_UTIL_COEX_PHY_SELECT_PORT        gpioPortC
// #define SL_RAIL_UTIL_COEX_PHY_SELECT_PIN         9
// [GPIO_SL_RAIL_UTIL_COEX_PHY_SELECT]$
#endif //SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT

// <<< sl:end pin_tool >>>

#endif // SL_RAIL_UTIL_COEX_CONFIG_H
