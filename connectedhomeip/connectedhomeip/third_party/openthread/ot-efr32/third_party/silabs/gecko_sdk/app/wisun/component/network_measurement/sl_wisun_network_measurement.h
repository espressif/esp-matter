/***************************************************************************//**
 * @file
 * @brief Wi-SUN Network measurement component
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

#ifndef __SL_WISUN_NETWORK_MEASUREMENT_H__
#define __SL_WISUN_NETWORK_MEASUREMENT_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <inttypes.h>
#include "socket.h"
#include "sl_wisun_ping.h"
#include "sl_wisun_network_measurement_stat.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Measurement type enum type definition
typedef enum sl_wisun_nwm_target_type {
  /// Only Border Router measurement
  SL_WISUN_NWM_TARGET_TYPE_BORDER_ROUTER,
  /// Only Primary Parent measurement
  SL_WISUN_NWM_TARGET_TYPE_PARENT,
  /// Measure all of available nodes
  SL_WISUN_NWM_TARGET_TYPE_ALL
} sl_wisun_nwm_target_type_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Wi-SUN Quick Network Measurement
 * @details Getting available IP addresses and measure them: Border Router,
 *          Parents, Children (not supported yet)
 * @param[in] meas_count Count of measurement
 * @param[in] meas_packet_length Size of the measurement packet
 *****************************************************************************/
void sl_wisun_nwm_quick_measure(const sl_wisun_nwm_target_type_t meas_type,
                                const uint16_t meas_count,
                                const uint16_t meas_packet_length);

/**************************************************************************//**
 * @brief Wi-SUN Network measurement
 * @details Measure custom IP address.
 * @param[in] remote_address Remote IP address to measure
 * @param[in] meas_count Count of measurement
 * @param[in] meas_packet_length Size of the measurement packet
 * @param[in] update_gui Flag to indicate if the GUI is to be updated
 *****************************************************************************/
void sl_wisun_nwm_measure(const wisun_addr_t * const remote_address,
                          const uint16_t meas_count,
                          const uint16_t meas_packet_length,
                          const bool update_gui);

#ifdef __cplusplus
}
#endif

#endif // __SL_WISUN_NETWORK_MEASUREMENT_H__
