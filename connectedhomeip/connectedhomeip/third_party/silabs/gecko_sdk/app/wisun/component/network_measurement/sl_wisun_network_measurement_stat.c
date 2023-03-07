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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "sl_wisun_app_core.h"
#include "sl_wisun_network_measurement_stat.h"
#include "sl_wisun_api.h"
#include "sl_wisun_types.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Node type to string
 * @details Helper function
 * @param[in] node_type Router type
 * @return const char* Router name in string format
 *****************************************************************************/
static const char *_node_type_to_str(const sl_wisun_nwm_node_type_t node_type);

/**************************************************************************//**
 * @brief Convert Neighbor type to Node type
 * @details Helper function
 * @param[in] neighbor_type
 * @return sl_wisun_nwm_node_type_t Node type
 *****************************************************************************/
static sl_wisun_nwm_node_type_t _nb_type_to_node_type(const uint32_t neighbor_type);

/**************************************************************************//**
 * @brief Print Node info
 * @details Helper function
 * @param[in] info Router info to print
 *****************************************************************************/
static void _print_node_info(const sl_wisun_nwm_node_stat_t * const info);

/**************************************************************************//**
 * @brief Add ping statistic to the router storage
 * @details Helper function. It has been called from the registered
 *          ping statistic callback
 * @param[in] ping_stat Ping statistic
 * @return true On success
 * @return false On failure
 *****************************************************************************/
static bool _add_ping_stat_to_node_storage(const sl_wisun_ping_stat_t * const ping_stat);

/**************************************************************************//**
 * @brief Get Neighbor by ID
 * @details Helper function for statistic getters
 * @param[in] neighbor_type Neighbor type
 * @param[out] dest Destination statistic storage
 * @return true On success
 * @return false On failure
 *****************************************************************************/
static bool _get_neighbor_stat_by_id(const sl_wisun_nwm_node_type_t neighbor_type,
                                     sl_wisun_nwm_node_stat_t * const dest);

/**************************************************************************//**
 * @brief Mutex Acquire
 * @details Helper function
 *****************************************************************************/
static inline void _nwm_stat_mutex_acquire(void);

/**************************************************************************//**
 * @brief Mutex Release
 * @details Helper function
 *****************************************************************************/
static inline void _nwm_stat_mutex_release(void);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Child label lookup table
static const char *_child_labels[SL_WISUN_MAX_CHILDREN_COUNT] = {
  "Child0",
  "Child1",
  "Child2",
  "Child3",
  "Child4",
  "Child5",
  "Child6",
  "Child7",
  "Child8",
  "Child9",
  "Child10",
  "Child11",
  "Child12",
  "Child13",
  "Child14",
  "Child15",
  "Child16",
  "Child17",
  "Child18",
  "Child19",
  "Child20",
  "Child21",
  "Child22",
  "Child23",
  "Child24",
  "Child25",
  "Child26",
  "Child27",
  "Child28",
  "Child29"
};

/// Current count of children
static uint8_t _current_child_cnt = 0;

/// Temporary MAC address st
static sl_wisun_mac_address_t _mac_addr_storage[SL_WISUN_MAX_NODE_COUNT];

static sl_wisun_nwm_node_stat_t _nodes[SL_WISUN_MAX_NODE_COUNT];

/// Statistic mutex control block
static uint64_t _nwm_stat_mtx_cb[osMutexCbSize] = { 0 };

/// Statistic mutex
static osMutexId_t _nwm_stat_mtx;

/// Statistic mutex attributes
static const osMutexAttr_t _nwm_stat_mtx_attr = {
  .name      = "NwmStatMtx",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _nwm_stat_mtx_cb,
  .cb_size   = sizeof(_nwm_stat_mtx_cb)
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void sl_wisun_nwm_stat_storage_init(void)
{
  osKernelState_t kernel_state = osKernelLocked;
  _nwm_stat_mtx = osMutexNew(&_nwm_stat_mtx_attr);
  assert(_nwm_stat_mtx != NULL);

  kernel_state = osKernelGetState();
  if (kernel_state == osKernelRunning) {
    _nwm_stat_mutex_acquire();
  }
  for (uint8_t i = 0; i < SL_WISUN_MAX_NODE_COUNT; ++i) {
    _nodes[i].type = SL_WISUN_NWM_NODE_TYPE_NOT_DEFINED;
  }

  if (kernel_state == osKernelRunning) {
    _nwm_stat_mutex_release();
  }
}

/* Statistic printer */
void sl_wisun_nwm_stat_handler(sl_wisun_ping_stat_t *stat)
{
  assert(stat != NULL);
  assert(stat->packet_count != 0);

  _nwm_stat_mutex_acquire();

  // save statistic
  (void) _add_ping_stat_to_node_storage(stat);

  // print
  printf("                     Summary                    \n");
  printf("--------------------+---------------------------\n");
  printf("Meas. packets       | %u\n", stat->packet_count);
  printf("Meas. packet length | %u\n", stat->packet_length);
  printf("Lost packets        | %u\n", stat->lost);
  printf("Packet loss         | %u%%\n", (stat->lost * 100) / stat->packet_count);
  printf("Min. Time           | %lums\n", stat->min_time_ms);
  printf("Max. Time           | %lums\n", stat->max_time_ms);
  printf("Avg. Time           | %lums\n\n", stat->avg_time_ms);

  _nwm_stat_mutex_release();
}

uint8_t sl_wisun_nwm_get_nodes(sl_wisun_nwm_measurable_node_t * const storage,
                               uint8_t max_storage_size)
{
  uint8_t neighbors_count = 0;
  sl_wisun_neighbor_info_t info = { 0 };

  _nwm_stat_mutex_acquire();

  // get count of neighbors
  assert(sl_wisun_get_neighbor_count(&neighbors_count) == SL_STATUS_OK);

  assert(neighbors_count <= (SL_WISUN_MAX_NODE_COUNT - 1));

  assert(sl_wisun_get_neighbors(&neighbors_count, _mac_addr_storage) == SL_STATUS_OK);

  if (storage != NULL && max_storage_size < neighbors_count) {
    return 0;
  }

  // Get Border Router
  sl_wisun_get_ip_address(SL_WISUN_IP_ADDRESS_TYPE_BORDER_ROUTER,
                          (sl_wisun_ip_address_t *)&_nodes[0].addr.sin6_addr);
  _nodes[0].type = SL_WISUN_NWM_NODE_TYPE_BORDER_ROUTER;
  _nodes[0].name =  _node_type_to_str(_nodes[0].type);
  if (storage != NULL) {
    storage[0].type = _nodes[0].type;
    storage[0].name = _nodes[0].name;
    memcpy(&storage[0].addr.sin6_addr, &_nodes[0].addr.sin6_addr, sizeof(in6addr_any));
    memset(&_nodes[0].ping_stat, 0, sizeof(sl_wisun_ping_stat_t));
  }

  memset(&_nodes[0].stat, 0, sizeof(sl_wisun_neighbor_stat_t));
  memset(&_nodes[0].ping_stat, 0, sizeof(sl_wisun_ping_stat_t));

  _print_node_info(&_nodes[0]);

  _current_child_cnt = 0;

  // Get Neighbors
  for (uint8_t i = 0, j = 1; i < neighbors_count; ++i) {
    if (sl_wisun_get_neighbor_info(&_mac_addr_storage[i], &info) == SL_STATUS_FAIL) {
      continue;
    }
    _nodes[j].type =  _nb_type_to_node_type(info.type);
    _nodes[j].name =  _node_type_to_str(_nodes[j].type);

    if (_nodes[j].type == SL_WISUN_NWM_NODE_TYPE_CHILD) {
      ++_current_child_cnt;
    }
    memcpy(&_nodes[j].addr.sin6_addr, &info.global_address, sizeof(in6addr_any));

    if (storage != NULL) {
      storage[j].type = _nodes[j].type;
      storage[j].name = _nodes[j].name;
      memcpy(&storage[j].addr.sin6_addr, &_nodes[j].addr.sin6_addr, sizeof(in6addr_any));
      memset(&_nodes[j].ping_stat, 0, sizeof(sl_wisun_ping_stat_t));
    }

    _nodes[j].stat.lifetime                   = info.lifetime;
    _nodes[j].stat.mac_tx_count               = info.mac_tx_count;
    _nodes[j].stat.mac_tx_failed_count        = info.mac_tx_failed_count;
    _nodes[j].stat.mac_tx_ms_count            = info.mac_tx_ms_count;
    _nodes[j].stat.mac_tx_ms_failed_count     = info.mac_tx_ms_failed_count;
    _nodes[j].stat.rpl_rank                   = info.rpl_rank;
    _nodes[j].stat.etx                        = info.etx;
    _nodes[j].stat.rsl_out                    = info.rsl_out;
    _nodes[j].stat.rsl_in                     = info.rsl_in;

    _print_node_info(&_nodes[j]);
    ++j;
  }
  _nwm_stat_mutex_release();

  return (uint8_t)(neighbors_count + 1);
}

void sl_wisun_nwm_get_border_router_stat(sl_wisun_nwm_node_stat_t * const dest)
{
  _nwm_stat_mutex_acquire();
  memcpy(dest, &_nodes[0], sizeof(sl_wisun_nwm_node_stat_t));
  _nwm_stat_mutex_release();
}

bool sl_wisun_nwm_get_primary_parent_stat(sl_wisun_nwm_node_stat_t * const dest)
{
  bool res = false;
  _nwm_stat_mutex_acquire();
  res = _get_neighbor_stat_by_id(SL_WISUN_NWM_NODE_TYPE_PRIMARY_PARENT, dest);
  _nwm_stat_mutex_release();
  return res;
}

bool sl_wisun_nwm_get_secondary_parent_stat(sl_wisun_nwm_node_stat_t * const dest)
{
  bool res = false;
  _nwm_stat_mutex_acquire();
  res = _get_neighbor_stat_by_id(SL_WISUN_NWM_NODE_TYPE_SECONDARY_PARENT, dest);
  _nwm_stat_mutex_release();
  return res;
}

void sl_wisun_nwm_get_children_stat(sl_wisun_nwm_node_stat_t * const dest,
                                    const uint8_t dest_size,
                                    uint8_t * const children_count)
{
  uint8_t children = 0;

  if (dest_size < SL_WISUN_MAX_CHILDREN_COUNT) {
    *children_count = 0;
    return;
  }

  _nwm_stat_mutex_acquire();

  for (uint8_t i = 0; i < SL_WISUN_MAX_NODE_COUNT; ++i) {
    if (_nodes[i].type == SL_WISUN_NWM_NODE_TYPE_CHILD) {
      memcpy(&dest[children++], &_nodes[i], sizeof(sl_wisun_nwm_node_stat_t));
    }
  }
  *children_count = children;
  _nwm_stat_mutex_release();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* Mutex acquire */
static inline void _nwm_stat_mutex_acquire(void)
{
  assert(osMutexAcquire(_nwm_stat_mtx, osWaitForever) == osOK);
}

/* Mutex release */
static inline void _nwm_stat_mutex_release(void)
{
  assert(osMutexRelease(_nwm_stat_mtx) == osOK);
}

static const char *_node_type_to_str(const sl_wisun_nwm_node_type_t node_type)
{
  switch (node_type) {
    case SL_WISUN_NWM_NODE_TYPE_PRIMARY_PARENT:   return "Primary Parent";
    case SL_WISUN_NWM_NODE_TYPE_SECONDARY_PARENT: return "Secondary Parent";
    case SL_WISUN_NWM_NODE_TYPE_BORDER_ROUTER:    return "Border Router";
    case SL_WISUN_NWM_NODE_TYPE_CHILD:            return _child_labels[_current_child_cnt];
    default:                                        return "Unknown";
  }
}

static void _print_node_info(const sl_wisun_nwm_node_stat_t * const info)
{
  printf("[Node: %s]\n", app_wisun_trace_util_get_ip_address_str(&info->addr.sin6_addr));
  printf("  type: %s\n", info->name);
  printf("  statistic:\n");
  printf("    lifetime:               %lu\n", info->stat.lifetime);
  printf("    mac_tx_count:           %lu\n", info->stat.mac_tx_count);
  printf("    mac_tx_failed_count:    %lu\n", info->stat.mac_tx_failed_count);
  printf("    mac_tx_ms_count:        %lu\n", info->stat.mac_tx_ms_count);
  printf("    mac_tx_ms_failed_count: %lu\n", info->stat.mac_tx_ms_failed_count);
  printf("    rpl_rank:               %u\n", info->stat.rpl_rank);
  printf("    etx:                    %u\n", info->stat.etx);
  printf("    rsl_out:                %u\n", info->stat.rsl_out);
  printf("    rsl_in:                 %u\n", info->stat.rsl_in);
}

static bool _add_ping_stat_to_node_storage(const sl_wisun_ping_stat_t * const ping_stat)
{
  bool retval = false;
  for (uint8_t i = 0; i < SL_WISUN_MAX_NODE_COUNT; ++i) {
    if (!memcmp(&ping_stat->remote_addr.sin6_addr, &_nodes[i].addr.sin6_addr, sizeof(in6_addr_t))) {
      memcpy(&_nodes[i].ping_stat, ping_stat, sizeof(sl_wisun_ping_stat_t));
      retval = true;
      break;
    }
  }
  return retval;
}

static bool _get_neighbor_stat_by_id(const sl_wisun_nwm_node_type_t neighbor_type,
                                     sl_wisun_nwm_node_stat_t * const dest)
{
  bool retval = false;

  for (uint8_t i = 0; i < SL_WISUN_MAX_NODE_COUNT; ++i) {
    if (_nodes[i].type == neighbor_type) {
      memcpy(dest, &_nodes[i], sizeof(sl_wisun_nwm_node_stat_t));
      retval = true;
      break;
    }
  }

  return retval;
}

static sl_wisun_nwm_node_type_t _nb_type_to_node_type(const uint32_t neighbor_type)
{
  switch (neighbor_type) {
    case SL_WISUN_NEIGHBOR_TYPE_PRIMARY_PARENT:   return SL_WISUN_NWM_NODE_TYPE_PRIMARY_PARENT;
    case SL_WISUN_NEIGHBOR_TYPE_SECONDARY_PARENT: return SL_WISUN_NWM_NODE_TYPE_SECONDARY_PARENT;
    case SL_WISUN_NEIGHBOR_TYPE_CHILD:            return SL_WISUN_NWM_NODE_TYPE_CHILD;
    default:                                      return SL_WISUN_NWM_NODE_TYPE_NOT_DEFINED;
  }
}
