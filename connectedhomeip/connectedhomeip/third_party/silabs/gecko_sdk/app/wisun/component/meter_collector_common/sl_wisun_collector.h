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

#ifndef __SL_WISUN_COLLECTOR_H__
#define __SL_WISUN_COLLECTOR_H__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "socket_hnd.h"
#include "socket.h"
#include "sli_wisun_meter_collector.h"
#include "sl_wisun_collector_config.h"

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
 * @brief Gets the socket used by the (CoAP) Collector app
 *****************************************************************************/
int32_t sl_wisun_collector_get_shared_socket(void);

/**************************************************************************//**
 * @brief Init collector component.
 * @details Call the common meter-collector init and set collector callback
 *****************************************************************************/
void sl_wisun_collector_init();

/**************************************************************************//**
 * @brief Inherit common handler.
 * @details Function should be called in CoAP Collector init to inherit callbacks
 * @param[in,out] hnd Handler
 *****************************************************************************/
void sl_wisun_collector_inherit_common_hnd(sl_wisun_collector_hnd_t *hnd);

/**************************************************************************//**
 * @brief Init common resources.
 * @details Function should be called in CoAP Collector init
 *****************************************************************************/
void sl_wisun_collector_init_common_resources(void);

/**************************************************************************//**
 * @brief Register Meter.
 * @details Add meter to the meter storage to handle in measurement loop
 * @param[in] meter_addr meter address structure
 * @return true meter has been successfully added
 * @return false on error
 *****************************************************************************/
bool sl_wisun_collector_register_meter(const wisun_addr_t *meter_addr);

/**************************************************************************//**
 * @brief Register Meter.
 * @details Add meter to the meter storage to handle in measurement loop
 * @param[in] meter_addr meter address structure
 * @return true meter has been successfully added
 * @return false on error
 *****************************************************************************/
bool sl_wisun_collector_remove_meter(const wisun_addr_t *meter_addr);

/**************************************************************************//**
 * @brief Get Meter by address.
 * @details Create a meter entry copy to the destination by address
 * @param[in] meter_addr meter address structure
 * @param[in,out] dest_meter destination meter entry
 * @return true meter has been successfully copied
 * @return false on error
 *****************************************************************************/
bool sl_wisun_collector_get_meter(const wisun_addr_t *meter_addr,
                                  sl_wisun_meter_entry_t * const dest_meter);

/**************************************************************************//**
 * @brief Compare byte address.
 * @details byte comparison for addresses
 * @param[in] addr1 address 1
 * @param[in] addr2 address 2
 * @return true if addresses has been matched
 * @return false if addresses has not been matched
 *****************************************************************************/
bool sl_wisun_collector_compare_address(const wisun_addr_t *addr1, const wisun_addr_t *addr2);

/**************************************************************************//**
 * @brief Measurement loop.
 * @details iterate registered meters, send request token and
 *          read the measurement packets
 *****************************************************************************/
void sl_wisun_collector_measurement_request_loop(void);

#endif
