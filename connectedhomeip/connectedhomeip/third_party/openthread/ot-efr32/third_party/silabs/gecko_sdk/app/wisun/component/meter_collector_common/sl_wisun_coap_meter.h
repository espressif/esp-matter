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
#ifndef __SL_WISUN_COAP_METER_H__
#define __SL_WISUN_COAP_METER_H__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sl_wisun_app_core.h"
#include "sl_wisun_coap.h"
#include "sli_wisun_meter_collector.h"
#include "sl_wisun_meter.h"
#include "sl_wisun_coap_meter_collector_config.h"
#include "sl_wisun_led_driver.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Init CoAP meter component.
 * @details Init common collector meter component and rht measurement
 *****************************************************************************/
void sl_wisun_coap_meter_init(void);

/**************************************************************************//**
 * @brief Measure parameters and send to the CoAP Collector (client).
 * @details Generate packet id, measure temperature, humidity and light
 * @param[in] sockd_meter Meter socket id
 * @param[in] collector_addr Collector address structure
 * @return size of the packet On success, -1 on failure
 *****************************************************************************/
int32_t sl_wisun_coap_meter_meas_params_and_send(const int32_t sockd_meter, const sockaddr_in6_t *collector_addr);

/**************************************************************************//**
 * @brief Response send to invalid URI path.
 * @details Resource not found response
 * @param[in] sockd_meter Meter socket id
 * @param[in] request Request
 * @param[in] collector_addr Collector address
 *****************************************************************************/
void sl_wisun_coap_meter_resource_not_found_response(const int32_t sockd_meter,
                                                     const sl_wisun_coap_packet_t *request,
                                                     const sockaddr_in6_t *collector_addr);

/**************************************************************************//**
 * @brief Convert LED ID.
 * @details Helper function to convert uint8_t LED ID to sl_wisun_led_id_t
 * @param[in] led_id LED ID raw value
 * @return sl_wisun_led_id_t converted LED ID
 *****************************************************************************/
sl_wisun_led_id_t sl_wisun_coap_meter_convert_led_id(const uint8_t led_id);

#endif
