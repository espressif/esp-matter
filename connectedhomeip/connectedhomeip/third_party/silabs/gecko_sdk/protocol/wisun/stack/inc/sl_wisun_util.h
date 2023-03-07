/***************************************************************************//**
 * @file sl_wisun_util.h
 * @brief Wi-SUN utilities
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

#ifndef SL_WISUN_UTIL_H
#define SL_WISUN_UTIL_H

#include <stdint.h>
#include "sl_status.h"

/**************************************************************************//**
 * @addtogroup SL_WISUN_UTIL Wi-SUN Util API
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @brief Get frequency band settings of the first RAIL configuration listed in
 *        RAIL's channelConfigs array.
 * @param[out] reg_domain Regulatory domain of the Wi-SUN network
 * @param[out] op_class Operational class of the Wi-SUN network
 * @param[out] op_mode Operational mode of the Wi-SUN network
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_util_get_rf_settings(uint8_t *reg_domain, uint8_t *op_class, uint16_t *op_mode);

/**************************************************************************//**
 * @brief Connect to a Wi-SUN network.
 * @details Since Wi-SUN frequency band settings are deduced from first RAIL
 *          configuration listed in RAIL's channelConfigs array, using this
 *          function is not recommended if more than one RAIL configuration is
 *          described.
 * @param[in] network_name Name of the Wi-SUN network as a zero-terminated string
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sl_wisun_util_connect(const uint8_t * network_name);

/** @} (end addtogroup SL_WISUN_UTIL) */

#endif  // SL_WISUN_UTIL_H
