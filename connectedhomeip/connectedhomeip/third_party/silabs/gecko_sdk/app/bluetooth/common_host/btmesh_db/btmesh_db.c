/***************************************************************************/ /**
 * @file
 * @brief BT Mesh Node Database component
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
// Includes

#include <stdlib.h>
#include "btmesh_db.h"
#include "app_assert.h"
#include "sl_slist.h"

// -----------------------------------------------------------------------------
// Macros

#define NL APP_LOG_NL

// -----------------------------------------------------------------------------
// Type definitions

// -----------------------------------------------------------------------------
// Static Function Declarations

/***************************************************************************//**
 * Allocate a node element for a network's node list
 *
 * @return Pointer to the newly created node. NULL if allocation fails.
 ******************************************************************************/
static btmesh_db_nodelist_elem_t* allocate_network_node(void);

/***************************************************************************//**
 * Return a node in a network's node list by its UUID
 *
 * @param[in] nodelist Pointer to the head of the network's node list
 * @param[in] uuid UUID of the node
 * @return Pointer to the nodelist element. NULL if UUID does not match any
 *         nodes in the list.
 ******************************************************************************/
static btmesh_db_nodelist_elem_t* get_nodelist_elem_by_uuid(sl_slist_node_t *nodelist,
                                                            uuid_128 uuid);

/***************************************************************************//**
 * Remove a node from a network's node list
 *
 * @param[in] node Pointer to the node that needs to be removed
 ******************************************************************************/
static void btmesh_db_remove_node_from_dcd(btmesh_db_node_t *node);

/***************************************************************************//**
 * Remove a node from all networks' node list
 *
 * @param[in] node Pointer to the node that needs to be removed
 * @return Status of the removal.
 * @retval SL_STATUS_OK If the node was successfully removed from all networks.
 *         Error code otherwise.
 ******************************************************************************/
static sl_status_t btmesh_db_remove_node_from_all_networks(btmesh_db_node_t *node);

// -----------------------------------------------------------------------------
// Static Variables

static sl_slist_node_t *node_list;    ///< Linked list of all known nodes
static sl_slist_node_t *network_list; ///< Linked list of all known networks

// -----------------------------------------------------------------------------
// Function definitions

void btmesh_db_init(void)
{
  sl_slist_init(&node_list);
  sl_slist_init(&network_list);
}

sl_slist_node_t *btmesh_db_get_node_list(void)
{
  return node_list;
}
sl_slist_node_t *btmesh_db_get_network_list(void)
{
  return network_list;
}

sl_status_t btmesh_db_create_network(uint16_t netkey_index)
{
  btmesh_db_network_t *network;
  SL_SLIST_FOR_EACH_ENTRY(network_list, network, btmesh_db_network_t, list_elem) {
    if (network->netkey_index == netkey_index) {
      return SL_STATUS_ALREADY_EXISTS;
    }
  }
  network = (btmesh_db_network_t *)malloc(sizeof(btmesh_db_network_t));
  if (NULL == network) {
    return SL_STATUS_ALLOCATION_FAILED;
  }
  sl_slist_init(&network->node_list);
  network->netkey_index = netkey_index;
  network->node_count = 0;
  sl_slist_push_back(&network_list, &network->list_elem);
  return SL_STATUS_OK;
}

sl_status_t btmesh_db_create_node(uuid_128 uuid,
                                  bd_addr mac_address,
                                  uint16_t prim_address,
                                  uint8_t element_count,
                                  uint8_t bearer_type)
{
  btmesh_db_node_t *node;
  SL_SLIST_FOR_EACH_ENTRY(node_list, node, btmesh_db_node_t, list_elem) {
    if (0 == memcmp(&node->prov.uuid, &uuid, sizeof(uuid_128))) {
      return SL_STATUS_ALREADY_EXISTS;
    }
  }
  node = (btmesh_db_node_t *)malloc(sizeof(btmesh_db_node_t));
  if (NULL == node) {
    return SL_STATUS_ALLOCATION_FAILED;
  }
  node->prov.uuid = uuid;
  node->prov.mac_address = mac_address;
  node->prov.bearer_type = bearer_type;
  node->prov.prim_address = prim_address;
  node->prov.element_count = element_count;
  node->dcd_available = false;
  node->dcd_modified = false;
  memset(&node->dcd, 0, sizeof(node->dcd));
  node->node_available = false;
  node->primary_subnet = NULL;
  sl_slist_push_back(&node_list, &node->list_elem);

  return SL_STATUS_OK;
}

static void btmesh_db_remove_node_from_dcd(btmesh_db_node_t *node)
{
  if (NULL == node) {
    return;
  }
  sl_status_t clear_dcd_status = btmesh_db_node_dcd_clear(node->prov.prim_address);
  // The DCD clear shall not fail because the node pointer shall have a valid
  // value when this function is called. (validation in caller functions)
  app_assert_status_f(clear_dcd_status,
                      "Failed to clear DCD of node (addr=0x%04x)." NL,
                      node->prov.prim_address);
}

sl_status_t btmesh_db_remove_node_by_uuid(uuid_128 uuid)
{
  btmesh_db_node_t *node = btmesh_db_node_get_by_uuid(uuid);
  if (NULL == node) {
    return SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  }
  return btmesh_db_remove_node_from_all_networks(node);
}

sl_status_t btmesh_db_remove_node_by_addr(uint16_t prim_address)
{
  btmesh_db_node_t *node = btmesh_db_node_get_by_addr(prim_address);
  if (NULL == node) {
    return SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  }
  return btmesh_db_remove_node_from_all_networks(node);
}

sl_status_t btmesh_db_remove_node_by_mac(bd_addr mac)
{
  btmesh_db_node_t *node = btmesh_db_node_get_by_mac(mac);
  if (NULL == node) {
    return SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  }
  return btmesh_db_remove_node_from_all_networks(node);
}

void btmesh_db_remove_network(btmesh_db_network_t *network)
{
  sl_status_t sc = btmesh_db_remove_all_nodes_from_network(network->netkey_index);
  if (SL_STATUS_OK == sc) {
    sl_slist_remove(&network_list, &network->list_elem);
    free(network);
  }
}

sl_status_t btmesh_db_add_node_to_network_by_uuid(uint16_t netkey_index,
                                                  uuid_128 uuid)
{
  btmesh_db_node_t *curr_node = btmesh_db_node_get_by_uuid(uuid);
  btmesh_db_network_t *curr_network = btmesh_db_network_get_network(netkey_index);

  if (NULL == curr_node || NULL == curr_network) {
    return SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  }

  if (btmesh_db_is_node_in_network(netkey_index, curr_node)) {
    return SL_STATUS_ALREADY_EXISTS;
  }

  btmesh_db_nodelist_elem_t *nodelist_elem = allocate_network_node();
  if (NULL == nodelist_elem) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  nodelist_elem->node = curr_node;
  curr_node->primary_subnet = curr_network;
  curr_node->node_available = true;

  sl_slist_push_back(&curr_network->node_list, &nodelist_elem->list_elem);
  curr_network->node_count++;
  return SL_STATUS_OK;
}

sl_status_t btmesh_db_node_dcd_set_available(uint16_t prim_address)
{
  sl_status_t sc = SL_STATUS_OK;
  btmesh_db_node_t *node = btmesh_db_node_get_by_addr(prim_address);
  if (NULL == node) {
    sc = SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  } else {
    node->dcd_available = true;
    node->dcd_modified = false;
  }
  return sc;
}

sl_status_t btmesh_db_node_dcd_set_header(uint16_t prim_address,
                                          uint16_t company_id,
                                          uint16_t product_id,
                                          uint16_t version_id,
                                          uint16_t min_replay_prot_list_len,
                                          bool feature_relay,
                                          bool feature_proxy,
                                          bool feature_friend,
                                          bool feature_low_power)
{
  sl_status_t sc = SL_STATUS_OK;
  btmesh_db_node_t *node = btmesh_db_node_get_by_addr(prim_address);
  if (NULL == node) {
    sc = SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  } else if (false != node->dcd_available) {
    // The DCD can be modified if it is invalidated first.
    // Note: DCD might change in case of firmware update only.
    sc = SL_STATUS_INVALID_STATE;
  } else {
    node->dcd.company_id = company_id;
    node->dcd.product_id = product_id;
    node->dcd.version_id = version_id;
    node->dcd.min_replay_prot_list_len = min_replay_prot_list_len;
    node->dcd.feature_relay = feature_relay;
    node->dcd.feature_proxy = feature_proxy;
    node->dcd.feature_friend = feature_friend;
    node->dcd.feature_low_power = feature_low_power;
    node->dcd_modified = true;
  }
  return sc;
}

sl_status_t btmesh_db_node_dcd_add_element(uint16_t prim_address,
                                           uint16_t location)
{
  sl_status_t sc = SL_STATUS_OK;
  btmesh_db_node_t *node = btmesh_db_node_get_by_addr(prim_address);
  if (NULL == node) {
    sc = SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  } else if (false != node->dcd_available) {
    // The DCD can be modified if it is invalidated first.
    // Note: DCD might change in case of firmware update only.
    sc = SL_STATUS_INVALID_STATE;
  } else {
    btmesh_db_element_t *allocated_elements;
    if (0 == node->dcd.element_count) {
      node->dcd.elements = NULL;
    }
    allocated_elements = realloc(node->dcd.elements,
                                 (node->dcd.element_count + 1)
                                 * sizeof(btmesh_db_element_t));
    if (NULL == allocated_elements) {
      sc = SL_STATUS_ALLOCATION_FAILED;
    } else {
      uint16_t element_idx = node->dcd.element_count;
      node->dcd.elements = allocated_elements;
      node->dcd.elements[element_idx].address = prim_address + element_idx;
      node->dcd.elements[element_idx].location = location;
      node->dcd.elements[element_idx].model_count = 0;
      node->dcd.elements[element_idx].models = NULL;
      node->dcd.element_count++;
      node->dcd_modified = true;
    }
  }
  return sc;
}

sl_status_t btmesh_db_node_dcd_add_model(uint16_t prim_address,
                                         uint16_t element_index,
                                         btmesh_db_model_id_t model_id)
{
  sl_status_t sc = SL_STATUS_OK;
  btmesh_db_node_t *node = btmesh_db_node_get_by_addr(prim_address);

  if ((NULL == node)
      || (node->dcd.element_count <= element_index)) {
    sc = SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  } else if (false != node->dcd_available) {
    // The DCD can be modified if it is invalidated first.
    // Note: DCD might change in case of firmware update only.
    sc = SL_STATUS_INVALID_STATE;
  } else {
    btmesh_db_element_t *element = &node->dcd.elements[element_index];
    for (uint16_t idx = 0; idx < element->model_count; idx++) {
      if ((element->models[idx].model == model_id.model)
          && (element->models[idx].vendor == model_id.vendor)) {
        // The model IDs shall be unique in the context of a node element
        sc = SL_STATUS_ALREADY_EXISTS;
        break;
      }
    }

    if (SL_STATUS_OK == sc) {
      btmesh_db_model_id_t *allocated_models;
      if (0 == element->model_count) {
        element->models = NULL;
      }
      allocated_models = realloc(element->models,
                                 (element->model_count + 1)
                                 * sizeof(btmesh_db_model_id_t));
      if (NULL == allocated_models) {
        sc = SL_STATUS_ALLOCATION_FAILED;
      } else {
        uint16_t model_idx = element->model_count;
        element->models = allocated_models;
        element->models[model_idx] = model_id;
        element->model_count++;
        node->dcd_modified = true;
      }
    }
  }
  return sc;
}

sl_status_t btmesh_db_node_dcd_clear(uint16_t prim_address)
{
  sl_status_t sc = SL_STATUS_FAIL;
  btmesh_db_node_t *node = btmesh_db_node_get_by_addr(prim_address);
  if (NULL == node) {
    sc = SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  } else if ((false == node->dcd_available)
             && (false == node->dcd_modified)) {
    // Check whether the DCD is available at all because if it is then
    // no operation is required
    sc = SL_STATUS_OK;
  } else if (NULL != node->dcd.elements) {
    for (uint16_t elem_idx = 0; elem_idx < node->dcd.element_count; elem_idx++) {
      if (0 < node->dcd.elements[elem_idx].model_count) {
        free(node->dcd.elements[elem_idx].models);
      }
    }
    free(node->dcd.elements);
    node->dcd.elements = NULL;
    node->dcd.element_count = 0;
    node->dcd_available = false;
    node->dcd_modified = false;
    sc = SL_STATUS_OK;
  }
  return sc;
}

static sl_status_t btmesh_db_remove_node_from_all_networks(btmesh_db_node_t *node)
{
  btmesh_db_network_t *network;
  sl_status_t remove_status = SL_STATUS_OK;

  SL_SLIST_FOR_EACH_ENTRY(network_list, network, btmesh_db_network_t, list_elem) {
    sl_status_t network_status = btmesh_db_remove_node_from_network(network->netkey_index,
                                                                    node);
    // The node is removed from every network and it is possible that it is
    // part of only a subset of networks.
    if ((SL_STATUS_OK != network_status)
        && (SL_STATUS_BT_MESH_DOES_NOT_EXIST != network_status)) {
      // If error occurs in one network then the iteration shall be continued
      // because it can occur at any point of iteration and this approach provides
      // the most unifrom result.
      remove_status = network_status;
    }
  }
  return remove_status;
}

sl_status_t btmesh_db_remove_node_from_network(uint16_t netkey_index, btmesh_db_node_t *node)
{
  btmesh_db_network_t *network = btmesh_db_network_get_network(netkey_index);
  if (NULL == network) {
    return SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  }

  if (true == btmesh_db_is_node_in_network(netkey_index, node)) {
    btmesh_db_nodelist_elem_t *nodelist_elem = get_nodelist_elem_by_uuid(network->node_list,
                                                                         node->prov.uuid);
    network->node_count--;
    sl_slist_remove(&network->node_list, &nodelist_elem->list_elem);
    free(nodelist_elem);
    if (false == btmesh_db_is_node_in_any_network(node)) {
      // If node is not present anywhere anymore, set it as unprovisioned
      btmesh_db_node_set_availability(node, false);
      btmesh_db_node_set_primary_address(node, 0x0000);
      btmesh_db_remove_node_from_dcd(node);
    }
    return SL_STATUS_OK;
  }
  return SL_STATUS_BT_MESH_DOES_NOT_EXIST;
}

sl_status_t btmesh_db_remove_node_from_network_by_addr(uint16_t netkey_index,
                                                       uint16_t prim_address)
{
  btmesh_db_node_t *node = btmesh_db_node_get_by_addr(prim_address);
  if (NULL == node) {
    return SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  }
  sl_status_t sc = btmesh_db_remove_node_from_network(netkey_index, node);
  return sc;
}

sl_status_t btmesh_db_remove_node_from_network_by_uuid(uint16_t netkey_index, uuid_128 uuid)
{
  btmesh_db_node_t *node = btmesh_db_node_get_by_uuid(uuid);
  if (NULL == node) {
    return SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  }
  sl_status_t sc = btmesh_db_remove_node_from_network(netkey_index, node);
  return sc;
}

sl_status_t btmesh_db_remove_node_from_network_by_mac(uint16_t netkey_index, bd_addr mac)
{
  btmesh_db_node_t *node = btmesh_db_node_get_by_mac(mac);
  if (NULL == node) {
    return SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  }
  sl_status_t sc = btmesh_db_remove_node_from_network(netkey_index, node);
  return sc;
}

sl_status_t btmesh_db_remove_all_nodes_from_network(uint16_t netkey_index)
{
  btmesh_db_network_t *network = btmesh_db_network_get_network(netkey_index);
  if (NULL == network) {
    return SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  }

  btmesh_db_nodelist_elem_t *nodelist_elem;
  SL_SLIST_FOR_EACH_ENTRY(network->node_list, nodelist_elem, btmesh_db_nodelist_elem_t, list_elem) {
    network->node_count--;
    sl_slist_remove(&network->node_list, &nodelist_elem->list_elem);
    btmesh_db_node_t *node = nodelist_elem->node;
    if (false == btmesh_db_is_node_in_any_network(node)) {
      // If node is not present anywhere anymore, set it as unprovisioned
      btmesh_db_node_set_availability(node, false);
      btmesh_db_node_set_primary_address(node, 0x0000);
      btmesh_db_remove_node_from_dcd(node);
    }
  }
  return SL_STATUS_OK;
}

btmesh_db_node_t* btmesh_db_node_get_by_addr(uint16_t address)
{
  btmesh_db_node_t *node;
  SL_SLIST_FOR_EACH_ENTRY(node_list, node, btmesh_db_node_t, list_elem) {
    if (node->prov.prim_address == address) {
      return node;
    }
  }
  return NULL;
}

btmesh_db_node_t* btmesh_db_node_get_by_uuid(uuid_128 uuid)
{
  btmesh_db_node_t *node;
  SL_SLIST_FOR_EACH_ENTRY(node_list, node, btmesh_db_node_t, list_elem) {
    if (0 == memcmp(&node->prov.uuid, &uuid, sizeof(uuid_128))) {
      return node;
    }
  }
  return NULL;
}

btmesh_db_node_t* btmesh_db_node_get_by_mac(bd_addr mac)
{
  btmesh_db_node_t *node;
  SL_SLIST_FOR_EACH_ENTRY(node_list, node, btmesh_db_node_t, list_elem) {
    if (0 == memcmp(&node->prov.mac_address, &mac, sizeof(bd_addr))) {
      return node;
    }
  }
  return NULL;
}

sl_status_t btmesh_db_node_set_availability(btmesh_db_node_t *node,
                                            bool availability)
{
  if (NULL == node) {
    return SL_STATUS_NULL_POINTER;
  }
  node->node_available = availability;
  return SL_STATUS_OK;
}

sl_status_t btmesh_db_node_set_primary_address(btmesh_db_node_t *node,
                                               uint16_t primary_address)
{
  if (NULL == node) {
    return SL_STATUS_NULL_POINTER;
  }
  node->prov.prim_address = primary_address;
  return SL_STATUS_OK;
}

btmesh_db_network_t* btmesh_db_network_get_network(uint16_t netkey_index)
{
  btmesh_db_network_t *network;
  SL_SLIST_FOR_EACH_ENTRY(network_list, network, btmesh_db_network_t, list_elem) {
    if (network->netkey_index == netkey_index) {
      return network;
    }
  }
  return NULL;
}

bool btmesh_db_is_node_in_network(uint16_t netkey_index, btmesh_db_node_t *node)
{
  btmesh_db_network_t *network = btmesh_db_network_get_network(netkey_index);
  sl_slist_node_t *nodes_in_network = network->node_list;
  btmesh_db_nodelist_elem_t *curr_nodelist_elem;
  SL_SLIST_FOR_EACH_ENTRY(nodes_in_network, curr_nodelist_elem, btmesh_db_nodelist_elem_t, list_elem) {
    if (node == curr_nodelist_elem->node) {
      return true;
    }
  }
  return false;
}

bool btmesh_db_is_node_in_any_network(btmesh_db_node_t *node)
{
  btmesh_db_network_t *curr_network;
  SL_SLIST_FOR_EACH_ENTRY(network_list, curr_network, btmesh_db_network_t, list_elem) {
    sl_slist_node_t *nodes_in_network = curr_network->node_list;
    btmesh_db_nodelist_elem_t *curr_nodelist_elem;
    SL_SLIST_FOR_EACH_ENTRY(nodes_in_network, curr_nodelist_elem, btmesh_db_nodelist_elem_t, list_elem) {
      if (node == curr_nodelist_elem->node) {
        return true;
      }
    }
  }
  return false;
}

btmesh_db_nodelist_elem_t* allocate_network_node(void)
{
  btmesh_db_nodelist_elem_t *new_node;
  new_node = (btmesh_db_nodelist_elem_t *)malloc(sizeof(btmesh_db_nodelist_elem_t));
  return new_node;
}

btmesh_db_nodelist_elem_t* get_nodelist_elem_by_uuid(sl_slist_node_t *nodelist,
                                                     uuid_128 uuid)
{
  btmesh_db_nodelist_elem_t *curr_elem;
  SL_SLIST_FOR_EACH_ENTRY(nodelist, curr_elem, btmesh_db_nodelist_elem_t, list_elem) {
    btmesh_db_node_t *curr_node = curr_elem->node;
    if (0 == memcmp(&uuid, &curr_node->prov.uuid, sizeof(uuid_128))) {
      return curr_elem;
    }
  }
  return NULL;
}
