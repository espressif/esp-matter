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

#ifndef __SL_WISUN_METER_H__
#define __SL_WISUN_METER_H__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <inttypes.h>
#include "socket_hnd.h"
#include "socket.h"
#include "sli_wisun_meter_collector.h"
#include "sl_wisun_meter_collector_config.h"

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
 * @brief Init meter component.
 * @details Init meter collector common component and RHT measurement
 *****************************************************************************/
void sl_wisun_meter_init(void);

/**************************************************************************//**
 * @brief Generate packet id for packet.
 * @details Weak implementation, user can override it
 * @param[out] packet packet
 *****************************************************************************/
void sl_wisun_meter_gen_packet_id(sl_wisun_meter_packet_t *packet);

/**************************************************************************//**
 * @brief Measure temperature and put it to the packet.
 * @details Weak implementation, user can override it
 * @param[out] packet packet
 *****************************************************************************/
void sl_wisun_meter_get_temperature(sl_wisun_meter_packet_t *packet);

/**************************************************************************//**
 * @brief Measure relative humidity and put it to the packet.
 * @details Weak implementation, user can override it
 * @param[out] packet packet
 *****************************************************************************/
void sl_wisun_meter_get_humidity(sl_wisun_meter_packet_t *packet);

/**************************************************************************//**
 * @brief Measure light and put it to the packet.
 * @details Weak implementation, user can override it
 * @param[out] packet packet
 *****************************************************************************/
void sl_wisun_meter_get_light(sl_wisun_meter_packet_t *packet);

/**************************************************************************//**
 * @brief Measure parameters and send to the Collector (client).
 * @details Generate packet id, measure temperature, humidity and light
 * @param[in] sockd_meter socket id of meter (UDP server)
 * @param[in] collector_addr Collector address structure
 * @return true On success
 * @return false On error
 *****************************************************************************/
bool sl_wisun_meter_meas_params_and_send(const int32_t sockd_meter, const sockaddr_in6_t *collector_addr);

/**************************************************************************//**
 * @brief Compare token with arrived bytes in buffer.
 * @details If the token matched, the meter send the measurement packet
 * @param[in] token arrived bytes
 * @param[in] token_size arrived bytes
 * @return true On success
 * @return false On error
 *****************************************************************************/
bool sl_wisun_mc_compare_token(const uint8_t *token, const uint16_t token_size);

#endif
