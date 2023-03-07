/***************************************************************************//**
 * @file
 * @brief Wi-SUN Application core component utility configuration header.
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
#ifndef SL_WISUN_APPLICATION_UTIL_CONFIG_H
#define SL_WISUN_APPLICATION_UTIL_CONFIG_H

/**************************************************************************//**
 * @defgroup SL_WISUN_APP_CORE_UTIL_CONFIG Utility configuration
 * @ingroup SL_WISUN_APP_CORE
 * @{
 *****************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Heartbeat Enable

// <q HEARTBEAT_ENABLED> Enable
// <i> Enables The heartbeat character and section breaks on the console output.
#define HEARTBEAT_ENABLED                           0

// </h> End Heartbeat Enable

// <h> Heartbeat section length

// <o HEARBEAT_SECTION_LENGTH> Length of one section is seconds where the heartbeat breaks line.
// <i> Default: 60
// <i> Define the number seconds which is printed on one line
#define HEARBEAT_SECTION_LENGTH                     (60)

// </h> End Heartbeat section length

// <h> Regional Regulation

// corresponds to SL_WISUN_REGULATION_NONE
#define REGULATION_NONE                              (0)
// corresponds to SL_WISUN_REGULATION_ARIB
#define REGULATION_ARIB                              (1)

// <h> Regulation
// <o WISUN_APP_REGULATION> Name of the Wi-SUN regional regulation or None
// <REGULATION_NONE=> None
// <REGULATION_ARIB=> ARIB
// <i> Default: SL_WISUN_REGULATION_NONE
#define WISUN_APP_REGULATION                        REGULATION_NONE
// </h> Regulation

// <h> Wi-SUN regulation transmission budget
// <o WISUN_APP_TX_BUDGET> Number of milliseconds per hour allowed to transmit in case of regional regulation.
// <i> Default: 720000 ms(corresponding to ARIB regulation)
#define WISUN_APP_TX_BUDGET                         (720000)
// </h> Wi-SUN regulation transmission budget in milliseconds.

// <h> Wi-SUN regulation warning and alert thresholds
// <o WISUN_DEFAULT_REGULATION_WARNING_THRESHOLD> Transmission duration level warning threshold in percent or -1 to disable
// <i> Default: 85 %
#define WISUN_DEFAULT_REGULATION_WARNING_THRESHOLD  85

// <o WISUN_DEFAULT_REGULATION_ALERT_THRESHOLD> Transmission duration level alert threshold in percent or -1 to disable
// <i> Default: 95 %
#define WISUN_DEFAULT_REGULATION_ALERT_THRESHOLD    95
// </h> Wi-SUN regulation warning and alert thresholds

// </h> Regional Regulation set upon connection
// <<< end of configuration section >>>

/** @}*/

#endif // SL_WISUN_APPLICATION_UTIL_CONFIG_H
