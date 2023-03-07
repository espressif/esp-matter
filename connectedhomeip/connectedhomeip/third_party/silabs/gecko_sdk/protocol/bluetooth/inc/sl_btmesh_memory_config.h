/******************************************************************************/
/**
 * @file   sl_btmesh_memory_config.h
 * @brief  Silicon Labs Bluetooth Mesh Memory Configuration API
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SL_BTMESH_MEMORY_CONFIG_H
#define SL_BTMESH_MEMORY_CONFIG_H
#include <stdint.h>

/**
 * Structure defining the heap memory reserved for the BT Mesh stack.
 * Everything is allocated when the stack is initialized, in order
 * to prevent the heap from becoming fragmented.
 *
 * The maximum values for items defined in the DCD (elements, models,
 * replay protection list size, GATT proxy connections) should be set
 * according to what is actually needed. That is, max_elements should
 * be equal to the number of elements present in the DCD, and so on.
 *
 * The memory is wasted if the maximum value is higher than needed, but
 * causes no other side effects. Setting the value to a lower number
 * than required will cause undefined behavior.
 */
typedef struct {
  /** Local sequence number write interval; must be a power of two */
  uint32_t pstore_write_interval_elem_seq;
  /** Size of RAM cache for persistent keys stored under ITS */
  uint16_t its_key_cache;
  /** Maximum number of network keys on the device */
  uint16_t max_net_keys;
  /** Maximum number of application keys on the device */
  uint16_t max_appkeys;
  /** Maximum number of model-application key bindings per model;
      should not be larger than the maximum number of application keys */
  uint16_t max_app_binds;
  /** Maximum number of subscription addresses per model */
  uint16_t max_subscriptions;
  /** Network cache entry size */
  uint16_t net_cache_size;
  /** Replay protection list entry size; must be equal to or larger than
      the number of nodes the device communicates with */
  uint16_t replay_size;
  /** Maximum number of concurrent transport layer segmented messages
      being sent */
  uint16_t max_send_segs;
  /** Maximum number of concurrent transport layer segmented messages
      being received */
  uint16_t max_recv_segs;
  /** Maximum number of virtual addresses on the node */
  uint16_t max_virtual_addresses;
  /** Maximum number of provisioner database entries; ignored for node */
  uint16_t provisioner_max_ddb_entries;
  /** Maximum number of keys to store per node on provisioner;
      ignored for node */
  uint16_t provisioner_max_node_net_keys;
  /** Maximum number of application keys to store per node on provisioner;
      ignored for node */
  uint16_t provisioner_max_node_app_keys;
  /** Friend node total cache buffer count */
  uint16_t friend_max_total_cache;
  /** Friend node friendship queue size */
  uint8_t friend_max_single_cache;
  /** Subscriptions per friendship */
  uint8_t friend_max_subs_list;
  /** maximum deferred send queue for app layer */
  uint8_t app_send_max_queue;
  /** Maximum number number of proxy access control list entries */
  uint8_t proxy_max_access_control_list_entries;
  /** Maximum number of elements on the device */
  uint8_t max_elements;
  /** Maximum number of models on the device */
  uint8_t max_models;
  /** Maximum number of friendships for a friend/LPN device */
  uint8_t max_friendships;
  /** Maximum number of concurrently ongoing foundation model commands */
  uint8_t max_foundation_model_commands;
  /** Maximum number of provisioning sessions */
  uint8_t max_provision_sessions;
  /** Maximum number of provisioning bearers */
  uint8_t max_provision_bearers;
  /** Maximum number of concurrent GATT connections (for
      provisioning and proxying) */
  uint8_t max_gatt_connections;
  /** GATT transport queue size */
  uint8_t gatt_txqueue_size;
} mesh_memory_config_t;

#endif
