/***************************************************************************//**
 * @file
 * @brief Wi-SUN Application Core Component
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

#ifndef __SL_WISUN_APP_CORE_H__
#define __SL_WISUN_APP_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @addtogroup SL_WISUN_APP_CORE_API Application Core
 * @{
 *****************************************************************************/

#include "sl_wisun_util.h"
#include "sl_wisun_trace_util.h"
#include "sl_wisun_app_core_util.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @addtogroup SL_WISUN_APP_CORE_API_TYPES Application Core API type definitions
 * @ingroup SL_WISUN_APP_CORE_API
 * @{
 *****************************************************************************/

/// Error flag bits enum type definition
typedef enum app_core_error_state_flag{
  /// Setting Error Flag bit
  SETTING_ERROR_FLAG_BIT = 0,
  /// Connection Failed Error Flag bit
  CONNECTION_FAILED_ERROR_FLAG_BIT,
  /// Network Size Error Flag bit
  SET_NETWORK_SIZE_ERROR_FLAG_BIT,
  /// TX Power Error Flag bit
  SET_TX_POWER_ERROR_FLAG_BIT,
  /// Dwel interval Error flag bit
  SET_DWELL_INTERVAL_ERROR_FLAG_BIT,
  /// Setting MAC address Error Flag bit
  SET_MAC_ADDR_ERROR_FLAG_BIT,
  /// setting Allow mac address Error Flag bit
  SET_ALLOW_MAC_ADDR_ERROR_FLAG_BIT,
  /// setting Deny mac address Error Flag bit
  SET_DENY_MAC_ADDR_ERROR_FLAG_BIT,
  /// Trusted Certificate Error Flag bit
  SET_TRUSTED_CERTIFICATE_ERROR_FLAG_BIT,
  /// Device Certificate Error Flag bit
  SET_DEVICE_CERTIFICATE_ERROR_FLAG_BIT,
  /// Device Private Key Error Flag bit
  SET_DEVICE_PRIVATE_KEY_ERROR_FLAG_BIT,
  /// Get RF Setting Error Flag bit
  GET_RF_SETTINGS_ERROR_FLAG_BIT,
} app_core_error_state_flag_t;

/// Current address storage structure definition
typedef struct current_addr {
  /// Link local address
  sl_wisun_ip_address_t link_local;
  /// Global address
  sl_wisun_ip_address_t global;
  /// Border Router address
  sl_wisun_ip_address_t border_router;
  /// Primary Parent address
  sl_wisun_ip_address_t primary_parent;
  /// Secondary Parent address
  sl_wisun_ip_address_t secondary_parent;
} current_addr_t;

/** @} (end SL_WISUN_APP_CORE_API_TYPES) */

typedef struct regulation_thresholds {
  int8_t warning_threshold;
  int8_t alert_threshold;
} regulation_thresholds_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Initialize Wi-SUN application core.
 * @details Initializing mutex, socket handler and set Wi-SUN settings.
 *****************************************************************************/
void app_wisun_core_init(void);

/**************************************************************************//**
 * @brief Get application core error.
 * @details The function retrieves the application core error status based on
 *          the flag.
 * @param[in] flag is the indicator of the error
 *****************************************************************************/
bool app_wisun_core_get_error(app_core_error_state_flag_t flag);

/**************************************************************************//**
 * @brief Connect to the Wi-SUN network.
 * @details Network initialization and connection.
 *          The function initializes the network with parameters
 *          (Network name, TX Power, Network size, etc.)
 *          by the stored settings in NVM
 *          if the settings component is added to the project,
 *          otherwise with the default settings.
 *****************************************************************************/
void app_wisun_network_connect(void);

/**************************************************************************//**
 * @brief Get the current addresses.
 * @details Copy cached addresses into destination.
 * @param[out] dest_addresses Destination
 *****************************************************************************/
void app_wisun_get_current_addresses(current_addr_t * const dest_addresses);

/**************************************************************************//**
 * @brief Set the regional regulation to active or passive.
 * @details After a stack API call for regional regulation, this function can
 * be used to store the status of the regulation (active or not).
 * @param[in] enabled (true = active, false = not active)
 * @return None
 *****************************************************************************/
void app_wisun_set_regulation_active(bool enabled);

/**************************************************************************//**
 * @brief Return the state of the regional regulation.
 * @details This function tells the caller if a regulation is currently active.
 * @param[in] None
 * @return Boolean indicating if a regional regulation is currently active.
 *****************************************************************************/
bool app_wisun_get_regulation_active(void);

/**************************************************************************//**
 * @brief Get the remaining budget from the transmission quota.
 * @details Returns the state of the regional regulation and the remaining
 * budget in ms if applicable, or zero budget if exceeded or not regulated.
 * @param[out] budget_out pointer to return the remaining budget to.
 * @return Boolean to indicate if the returned value reflects a usable value.
 *****************************************************************************/
bool app_wisun_get_remaining_tx_budget(uint32_t* const budget_out);

/**************************************************************************//**
 * @brief Set up warning and alert thresholds for the regional regulation.
 * @details Sets up the percentages of warnings and alerts where the regulation
 * indicate that the transmission quota is approached/exceeded.
 * @param[in] warning_level new percentage for the warning threshold
 * @param[in] alert_level new percentage for the alert threshold
 * @return None
 *****************************************************************************/
void app_wisun_set_regulation_thresholds(const int8_t warning_level, const int8_t alert_level);

/**************************************************************************//**
 * @brief Get the warning and alert levels for approaching/exceeded the TX budget.
 * @details Values representing percentages of the allowed transmission quota in
 * ms are returned for the warning and alert levels, respectively.
 * @param[out] thresholds_out pointer to the struct to hold the thresholds
 * @return Boolean to indicate if the operation was successful
 *****************************************************************************/
bool app_wisun_get_regulation_thresholds(regulation_thresholds_t* thresholds_out);

/** @}*/

#ifdef __cplusplus
}
#endif
#endif
