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

#ifndef __SL_WISUN_NETWORK_MEASUREMENT_STAT_H__
#define __SL_WISUN_NETWORK_MEASUREMENT_STAT_H__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sl_wisun_ping.h"
#include "sl_wisun_types.h"
#include "socket.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Maximum measurable router count
#define SL_WISUN_MAX_NODE_COUNT       (33U) // 32 Neighbors + 1 BR

/// Maximum children count
#define SL_WISUN_MAX_CHILDREN_COUNT       (30U)

/// Wi-SUN Node type enum type definition
typedef enum sl_wisun_nwm_node_type {
  /// Not defined
  SL_WISUN_NWM_NODE_TYPE_NOT_DEFINED,
  /// Border Router
  SL_WISUN_NWM_NODE_TYPE_BORDER_ROUTER,
  /// Primary Parent
  SL_WISUN_NWM_NODE_TYPE_PRIMARY_PARENT,
  /// Secondary Parent
  SL_WISUN_NWM_NODE_TYPE_SECONDARY_PARENT,
  /// Child
  SL_WISUN_NWM_NODE_TYPE_CHILD
} sl_wisun_nwm_node_type_t;

/// Neighbor statistic structure type definition
typedef struct sl_wisun_neighbor_stat {
  /// Remaining lifetime (Link lifetime for parents, ARO lifetime for children)
  uint32_t lifetime;
  /// MAC TX packet count
  uint32_t mac_tx_count;
  /// MAC TX failed count
  uint32_t mac_tx_failed_count;
  /// MAC TX Mode switch count
  uint32_t mac_tx_ms_count;
  /// MAC TX Mode switch failed count
  uint32_t mac_tx_ms_failed_count;
  /// RPL Rank value for parents (0xffff if unknown or child)
  uint16_t rpl_rank;
  /// Measured ETX value if known (0xffff if unknown)
  uint16_t etx;
  /// Parent RSSI Out measured RSSI value (0xff if unknown)
  /// Calculated using EWMA specified by Wi-SUN from range of -174 (0) to +80 (254) dBm.
  uint8_t rsl_out;
  /// Parent RSSI In measured RSSI value (0xff if unknown)
  /// Calculated using EWMA specified by Wi-SUN from range of -174 (0) to +80 (254) dBm.
  uint8_t rsl_in;
} sl_wisun_neighbor_stat_t;

/// Wi-SUN Node statistic structure type definition
typedef struct sl_wisun_nwm_node_stat {
  /// Router type
  sl_wisun_nwm_node_type_t type;
  /// Name
  const char *name;
  /// Address (global)
  wisun_addr_t addr;
  /// Neighbor statistics
  sl_wisun_neighbor_stat_t stat;
  /// Ping statistics
  sl_wisun_ping_stat_t ping_stat;
} sl_wisun_nwm_node_stat_t;

/// Measurable Router structure type definition
typedef struct sl_wisun_nwm_measurable_node {
  /// Type
  sl_wisun_nwm_node_type_t type;
  /// Name
  const char *name;
  /// Address (global)
  wisun_addr_t addr;
  /// Measurement requested indication
  bool is_requested;
} sl_wisun_nwm_measurable_node_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Init Network Measurement statistic storage
 * @details Init thread safe storage
 *****************************************************************************/
void sl_wisun_nwm_stat_storage_init(void);

/**************************************************************************//**
 * @brief Network Measurement handler
 * @details This handler is called in the ping request
 * @param[out] stat Ping statistic
 *****************************************************************************/
void sl_wisun_nwm_stat_handler(sl_wisun_ping_stat_t *stat);

/**************************************************************************//**
 * @brief Get Routers
 * @details Fill the internal storage with Border Router and Neighbors
 * @param[out] storage Measurable router storage, for starting iteration/measurement
 * @param[in] max_storage_size Storage size check for assertion
 * @return uint8_t Count of Routers (Border Router + Neighbors)
 *****************************************************************************/
uint8_t sl_wisun_nwm_get_nodes(sl_wisun_nwm_measurable_node_t * const storage,
                               const uint8_t max_storage_size);

/**************************************************************************//**
 * @brief Get Border Router statistics
 * @details Helper function
 * @param[out] dest Destination
 *****************************************************************************/
void sl_wisun_nwm_get_border_router_stat(sl_wisun_nwm_node_stat_t * const dest);

/**************************************************************************//**
 * @brief Get Primary Parent statistics
 * @details Getter for Primary Parent Neighbor statistic
 * @param[out] dest Destination
 * @return true If Primary Parent is available
 * @return false Primary Parent is not available
 *****************************************************************************/
bool sl_wisun_nwm_get_primary_parent_stat(sl_wisun_nwm_node_stat_t * const dest);

/**************************************************************************//**
 * @brief Get Secondary Parent statistics
 * @details Getter for Secondary Parent Neighbor statistic
 * @param[out] dest Destination
 * @return true If Secondary Parent is available
 * @return false Secondary Parent is not available
 *****************************************************************************/
bool sl_wisun_nwm_get_secondary_parent_stat(sl_wisun_nwm_node_stat_t * const dest);

/**************************************************************************//**
 * @brief Get Children Statistics
 * @details Get all of available Childre statistics
 * @param[out] dest Destination buffer start address
 * @param[in] dest_size Destination buffer size for assertion
 * @param[out] children_count Count of children
 *****************************************************************************/
void sl_wisun_nwm_get_children_stat(sl_wisun_nwm_node_stat_t * const dest,
                                    const uint8_t dest_size,
                                    uint8_t * const children_count);

#endif
