/***************************************************************************//**
 * @file
 * @brief
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

#ifndef SL_RAIL_UTIL_IEEE802154_PHY_SELECT_H
#define SL_RAIL_UTIL_IEEE802154_PHY_SELECT_H

#include "rail_types.h"
#include "sl_rail_util_ieee802154_stack_event.h"

// Backwards compatibility macros
#define sl_rail_util_radio_config_t \
  sl_rail_util_ieee802154_radio_config_t
#define SL_RAIL_UTIL_RADIO_CONFIG_154_2P4_DEFAULT \
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_DEFAULT
#define SL_RAIL_UTIL_RADIO_CONFIG_154_2P4_ANTDIV \
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV
#define SL_RAIL_UTIL_RADIO_CONFIG_154_2P4_COEX \
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_COEX
#define SL_RAIL_UTIL_RADIO_CONFIG_154_2P4_ANTDIV_COEX \
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV_COEX
#define SL_RAIL_UTIL_RADIO_CONFIG_154_2P4_FEM \
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_FEM
#define SL_RAIL_UTIL_RADIO_CONFIG_154_2P4_ANTDIV_FEM \
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV_FEM
#define SL_RAIL_UTIL_RADIO_CONFIG_154_2P4_COEX_FEM \
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_COEX_FEM
#define SL_RAIL_UTIL_RADIO_CONFIG_154_2P4_ANTDIV_COEX_FEM \
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV_COEX_FEM
#define sl_rail_util_get_active_radio_config \
  sl_rail_util_ieee802154_get_active_radio_config
#define sl_rail_util_plugin_config_2p4ghz_radio \
  sl_rail_util_ieee802154_config_radio

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup IEEE802154_Phy_Select IEEE802.15.4 Phy Select
 * @{
 */

/**
 * @enum sl_rail_util_ieee802154_radio_config_t
 * @brief IEEE802.15.4 2.4Ghz radio configuration index.
 */
RAIL_ENUM(sl_rail_util_ieee802154_radio_config_t) {
/**
 * @def SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_DEFAULT
 * @brief Default IEEE802.15.4 2.4Ghz radio configuration
 */
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_DEFAULT,
/**
 * @def SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV
 * @brief IEEE802.15.4 2.4Ghz radio configuration for RX antenna diversity
 */
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV,
/**
 * @def SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_COEX
 * @brief IEEE802.15.4 2.4Ghz radio configuration optimized for radio coexistence
 */
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_COEX,
/**
 * @def SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV_COEX
 * @brief IEEE802.15.4 2.4Ghz radio configuration for RX antenna diversity
 * optimized for radio coexistence
 */
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV_COEX,
/**
 * @def SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_FEM
 * @brief IEEE802.15.4 2.4Ghz radio configuration optimized for a front end module
 */
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_FEM,
/**
 * @def SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV_FEM
 * @brief IEEE802.15.4 2.4Ghz radio configuration for RX antenna diversity
 * optimized for a front end module
 */
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV_FEM,
/**
 * @def SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_COEX_FEM
 * @brief IEEE802.15.4 2.4Ghz radio configuration optimized for radio coexistence
 * and a front end module
 */
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_COEX_FEM,
/**
 * @def SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV_COEX_FEM
 * @brief IEEE802.15.4 2.4Ghz radio configuration for RX antenna diversity
 * optimized for radio coexistence and a front end module
 */
  SL_RAIL_UTIL_IEEE802154_RADIO_CONFIG_2P4_ANTDIV_COEX_FEM,
};

/**
 * PHY select contribution for IEEE802.15.4 stack event handler
 *
 * @param[in] stack_event event to handle
 * @param[in] supplement optional event information
 * @return Status code indicating success of the function call.
 */
sl_rail_util_ieee802154_stack_status_t sl_rail_util_ieee802154_phy_select_on_event(
  sl_rail_util_ieee802154_stack_event_t stack_event,
  uint32_t supplement);

/**
 * Get the active IEEE802.15.4 2.4Ghz radio configuration.
 *
 * @return Active IEEE802.15.4 2.4Ghz radio configuration
 */
sl_rail_util_radio_config_t sl_rail_util_ieee802154_get_active_radio_config(void);

/**
 * Configure IEEE802.15.4 2.4Ghz radio configuration.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t sl_rail_util_ieee802154_config_radio(RAIL_Handle_t railHandle);

/**
 * @}
 * end of IEEE802154_PHY_SELECT_API
 */

#ifdef __cplusplus
}
#endif

#endif // SL_RAIL_UTIL_IEEE802154_PHY_SELECT_H
