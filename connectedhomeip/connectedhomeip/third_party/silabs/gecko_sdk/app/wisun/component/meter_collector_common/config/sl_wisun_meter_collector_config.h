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
#ifndef SL_WISUN_METER_COLLECTOR_CONFIG_H
#define SL_WISUN_METER_COLLECTOR_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Wi-SUN Meter - Collector common configuration

// <s SL_WISUN_METER_COLLECTOR_TOKEN> Token string. (Not used in CoAP sample apps)
// <i> Default: "abcd"
// <i> This token must be send to meter to get measuement packet as response.
// <i> The length should be lower than SL_WISUN_METER_COLLECTOR_TOKEN_MAX_SIZE
#define SL_WISUN_METER_COLLECTOR_TOKEN                                  "abcd"

// <o SL_WISUN_METER_COLLECTOR_TOKEN_MAX_SIZE> Allocated token buffer size [bytes]. (Not used in CoAP sample apps)
// <i> Default: 64
#define SL_WISUN_METER_COLLECTOR_TOKEN_MAX_SIZE                         64U
// </h>

// <h>Wi-SUN Meter configuration
// <o SL_WISUN_METER_PORT> Meter listener port
// <i> Default: 5683
// <i> This is the port number where Meter is listening
// <1-65536>
#define SL_WISUN_METER_PORT                                             5683U

// <o SL_WISUN_METER_USE_CUSTOM_COLLECTOR_PORT> Send Meter responses to fixed port
//   <true => Respond to SL_WISUN_COLLECTOR_PORT
//   <false => Respond to sender's port
// <i> Default: false
// <i> This option tells Meter apps to send responses to a fixed port rather than the port the request is received from.
#define SL_WISUN_METER_USE_CUSTOM_COLLECTOR_PORT                        false
// </h>

// <h>Wi-SUN Collector configuration
// <o SL_WISUN_COLLECTOR_PORT> Collector receiver port
// <i> Default: 5684
// <i> This is the port number where Collector is listening
// <1-65536>
#define SL_WISUN_COLLECTOR_PORT                                         5684U
// </h>

// <<< end of configuration section >>>

#endif // SL_WISUN_METER_COLLECTOR_CONFIG_H
