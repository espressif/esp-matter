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
#ifndef SL_WISUN_COAP_METER_COLLECTOR_CONFIG_H
#define SL_WISUN_COAP_METER_COLLECTOR_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Wi-SUN CoAP Meter - Collector common configuration

// <s SL_WISUN_COAP_METER_COLLECTOR_MEASUREMENT_URI_PATH> Meter measurement resource URI path
// <i> Default: "/measurement"
// <i> This is the shared resource URI path to get mesaurement results
#define SL_WISUN_COAP_METER_COLLECTOR_MEASUREMENT_URI_PATH             "/measurement"

// <s SL_WISUN_COAP_METER_COLLECTOR_LED_TOGGLE_URI_PATH> Meter LEDs resource URI path
// <i> Default: "/led"
// <i> This is the shared resource URI path to toggle LEDs
#define SL_WISUN_COAP_METER_COLLECTOR_LED_TOGGLE_URI_PATH              "/led"

// <o SL_WISUN_COAP_METER_COLLECTOR_URI_PATH_MAX_SIZE> Allocated reference URI path buffer size [bytes]
// <i> Default: 64
// <i> SL_WISUN_COAP_METER_COLLECTOR_MEASUREMENT_URI_PATH string length should be lower
#define SL_WISUN_COAP_METER_COLLECTOR_URI_PATH_MAX_SIZE                 64U

// <o SL_WISUN_COAP_METER_COLLECTOR_RECV_BUFF_SIZE> Allocated received buffer size
// <i> Default: 128
#define SL_WISUN_COAP_METER_COLLECTOR_RECV_BUFF_SIZE                    128U

// <o SL_WISUN_COAP_METER_COLLECTOR_DEFAULT_MESSAGE_ID> Default message ID for coap packets [bytes]
// <i> Default: 7
#define SL_WISUN_COAP_METER_COLLECTOR_DEFAULT_MESSAGE_ID                7U
// </h>

// <<< end of configuration section >>>

#endif // SL_WISUN_COAP_METER_COLLECTOR_CONFIG_H
