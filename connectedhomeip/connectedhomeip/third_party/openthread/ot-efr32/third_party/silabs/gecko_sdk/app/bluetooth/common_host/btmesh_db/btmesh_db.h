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
********************************************************************************/

#ifndef BTMESH_DB_H
#define BTMESH_DB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_slist.h"

/***************************************************************************//**
 * @addtogroup btmesh_db BT Mesh Database
 * @{
 ******************************************************************************/

typedef struct btmesh_db_network_t btmesh_db_network_t;

/// Common model identifier structure for Bluetooth SIG and Vendor models
typedef struct btmesh_db_model_id_t {
  /// Vendor ID of the model (@p MESH_SPEC_VENDOR_ID for Bluetooth SIG models)
  uint16_t vendor;
  /// Model ID of the Vendor or Bluetooth SIG model
  uint16_t model;
} btmesh_db_model_id_t;

/// Node element represention in the Device Composition Data (DCD)
typedef struct btmesh_db_element_t {
  /// Network address of the element (each element has its own address)
  uint16_t address;
  /// @brief Location description as defined in the GATT Bluetooth Namespace
  /// Descriptors section of the Bluetooth SIG Assigned Numbers
  uint16_t location;
  /// Total number of models in this element (Bluetooth SIG plus Vendor models)
  uint16_t model_count;
  /// Array of models in this element (Bluetooth SIG plus Vendor models)
  btmesh_db_model_id_t *models;
} btmesh_db_element_t;

/// Representation of Device Composition Data (DCD)
typedef struct btmesh_db_node_dcd_t {
  /// Company identifier assigned by the Bluetooth SIG
  uint16_t company_id;
  /// Vendor-assigned product identifier
  uint16_t product_id;
  /// Vendor-assigned product version identifier
  uint16_t version_id;
  /// @brief Value representing the minimum number of replay protection list
  /// entries in a device
  uint16_t min_replay_prot_list_len;
  /// Relay feature support
  bool feature_relay;
  /// Proxy feature support
  bool feature_proxy;
  /// Friend feature support
  bool feature_friend;
  /// Low power feature support
  bool feature_low_power;
  /// Number of elements
  uint16_t element_count;
  /// Array of elements
  btmesh_db_element_t *elements;
} btmesh_db_node_dcd_t;

/// The provisioning-related information of a node
typedef struct btmesh_db_node_prov_t {
  /// UUID of the node.
  uuid_128 uuid;
  /// Bluetooth address of the node. This is only known
  /// if the node is unprovisioned beaconing.
  bd_addr mac_address;
  /// Type of the provisioning bearer. One of PB-ADV(0) or PB-GATT(1).
  uint8_t bearer_type;
  /// Primary element address of the node. This is only known
  /// if the node is part of a network.
  uint16_t prim_address;
  /// Element count of the node.
  uint16_t element_count;
} btmesh_db_node_prov_t;

/// Struct representing one single node
typedef struct btmesh_db_node_t {
  sl_slist_node_t list_elem;            ///< Linked list of nodes
  btmesh_db_node_prov_t prov;           ///< Provisioning information
  bool dcd_available;                   ///< DCD data available
  bool dcd_modified;                    ///< DCD data modified
  btmesh_db_node_dcd_t dcd;             ///< DCD information
  bool node_available;                  ///< Node is part of a network
  btmesh_db_network_t *primary_subnet;  ///< The node's primary subnet's address
} btmesh_db_node_t;

/// Struct representing one single network
struct btmesh_db_network_t {
  sl_slist_node_t list_elem;            ///< Linked list of networks
  uint16_t netkey_index;                ///< Netkey index of the network
  uint16_t node_count;                  ///< Number of nodes in the network
  sl_slist_node_t *node_list;           ///< Linked list of nodes in the network
};

/// Helper struct to keep track of which nodes are part of a given network
typedef struct btmesh_db_nodelist_elem_t {
  sl_slist_node_t list_elem;            ///< Linked list of nodes
  btmesh_db_node_t *node;               ///< Pointer to the node
} btmesh_db_nodelist_elem_t;

/***************************************************************************//**
* Initialize the database.
*******************************************************************************/
void btmesh_db_init(void);

/***************************************************************************//**
* Get the node list.
*
* @return Pointer to the head of the node linked list
*******************************************************************************/
sl_slist_node_t *btmesh_db_get_node_list(void);

/**************************************************************************/ /**
* Get the network list.
*
* @return Pointer to the head of the network linked list
*******************************************************************************/
sl_slist_node_t *btmesh_db_get_network_list(void);

/***************************************************************************//**
* Create a new BT Mesh network in the database.
*
* @param[in] netkey_index netkey_index of the network
*
* @return Status of the network creation.
* @retval SL_STATUS_OK If network is correctly created.
* @retval SL_STATUS_ALREADY_EXIST If a network with the given
*                                 netkey_index already exists.
* @retval SL_STATUS_ALLOCATION_FAILED If allocating memory for the
*                                     network struct fails.
*******************************************************************************/
sl_status_t btmesh_db_create_network(uint16_t netkey_index);

/***************************************************************************//**
* Remove network from the database. This also removes the reference to the
* network in all nodes that were part of it.
*
* @param[in] network Pointer to the network to be deleted
*******************************************************************************/
void btmesh_db_remove_network(btmesh_db_network_t *network);

/**************************************************************************/ /**
* Create a new node and add it to the node list.
*
* @param[in] uuid UUID of the node
* @param[in] mac_address Bluetooth address of the node
* @param[in] prim_address Primary address of the node in a network
* @param[in] element_count Element count of the node
* @param[in] bearer_type Type of the provisioning bearer layer. Can be
*                        PB-ADV (0) or PB-GATT (1).
* @return Status of the node creation.
* @retval SL_STATUS_OK If node is successfully created.
* @retval SL_STATUS_ALREADY_EXIST If a node with the given UUID already exists.
* @retval SL_STATUS_ALLOCATION_FAILED If allocating memory for
*                                     the node struct fails.
*******************************************************************************/
sl_status_t btmesh_db_create_node(uuid_128 uuid,
                                  bd_addr mac_address,
                                  uint16_t prim_address,
                                  uint8_t element_count,
                                  uint8_t bearer_type);

/***************************************************************************//**
* Remove a node from the database by its UUID. This function removes the node
* from all networks it is present in.
*
* @param[in] uuid UUID of the node to be removed
* @return Status of the removal.
* @retval SL_STATUS_OK If node is successfully removed.
* @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If the node does not exist.
*******************************************************************************/
sl_status_t btmesh_db_remove_node_by_uuid(uuid_128 uuid);

/***************************************************************************//**
* Remove a node from the database by its primary element address. This
* function removes the node from all networks it is present in.
*
* @param[in] prim_address Primary element address of the node to be removed
* @return Status of the removal.
* @retval SL_STATUS_OK If node is successfully removed.
* @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If the node does not exist.
*******************************************************************************/
sl_status_t btmesh_db_remove_node_by_addr(uint16_t prim_address);

/***************************************************************************//**
* Remove a node from the database by its Bluetooth address. This
* function removes the node from all networks it is present in.
*
* @param[in] mac Bluetooth address of the node to be removed
* @return Status of the removal.
* @retval SL_STATUS_OK If node is successfully removed.
* @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If the node does not exist.
*******************************************************************************/
sl_status_t btmesh_db_remove_node_by_mac(bd_addr mac);

/***************************************************************************//**
* Add an already created node to a network by UUID
*
* @param[in] netkey_index Netkey index of the network
* @param[in] uuid UUID of the node
* @return Status of the addition.
* @retval SL_STATUS_OK If node is successfully added to the network.
* @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If either the node or the network
*                                          does not exist.
* @retval SL_STATUS_ALREADY_EXISTS If the node is already in the network.
* @retval SL_STATUS_ALLOCATION_FAILED If the node \a pointer can't be added
*                                     to the network's list.
*******************************************************************************/
sl_status_t btmesh_db_add_node_to_network_by_uuid(uint16_t netkey_index,
                                                  uuid_128 uuid);

/***************************************************************************//**
 * Makes the DCD information available for reading in the BT Mesh database
 *
 * @param[in] prim_address Primary element address of the node
 * @returns Status of the set available operation
 * @retval SL_STATUS_OK If DCD information is set available
 * @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If the node does not exist.
 *
 * The DCD information of a node is built in several steps in btmesh database:
 *   - @ref btmesh_db_node_dcd_set_header (once)
 *   - @ref btmesh_db_node_dcd_add_element (element count times)
 *   - @ref btmesh_db_node_dcd_add_model (on each element model count times)
 *
 * The DCD information is not valid until all necessary functions are called.
 * This function marks when the DCD information is complete in the database.
 ******************************************************************************/
sl_status_t btmesh_db_node_dcd_set_available(uint16_t prim_address);

/***************************************************************************//**
 * Sets the DCD header data (page 0) in the BT Mesh database
 *
 * @param[in] prim_address Primary element address of the node
 * @param[in] company_id Company identifier assigned by the Bluetooth SIG
 * @param[in] product_id Vendor-assigned product identifier
 * @param[in] version_id Vendor-assigned product version identifier
 * @param[in] min_replay_prot_list_len Min number of replay protection list entries
 * @param[in] feature_relay Relay feature support
 * @param[in] feature_proxy Proxy feature support
 * @param[in] feature_friend Friend feature support
 * @param[in] feature_low_power Low power feature support
 * @returns Status of the set node DCD header operation
 * @retval SL_STATUS_OK If DCD header data is set successfully.
 * @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If the node does not exist.
 * @retval SL_STATUS_INVALID_STATE If the DCD can't be modified because it was
 *   set available previously.
 *
 * Sets DCD header data of the node chosen by @ref prim_address in BT Mesh
 * database. The DCD header data not vary with the number of elements or models.
 *
 * If the DCD is set available then the DCD data can be modified only if it is
 * cleared first by the @ref btmesh_db_node_dcd_clear function.
 *
 * @note If the firmware is updated on the node then the DCD of the node might
 *   change (not necessarily). The BT Mesh specification guarantees that the DCD
 *   is constant between firmware updates.
 *
 ******************************************************************************/
sl_status_t btmesh_db_node_dcd_set_header(uint16_t prim_address,
                                          uint16_t company_id,
                                          uint16_t product_id,
                                          uint16_t version_id,
                                          uint16_t min_replay_prot_list_len,
                                          bool feature_relay,
                                          bool feature_proxy,
                                          bool feature_friend,
                                          bool feature_low_power);

/***************************************************************************//**
 * Adds an empty element to the DCD of specified node in BT Mesh database
 *
 * @param[in] prim_address Primary element address of the node
 * @param[in] location Location description as defined in the GATT Bluetooth
 *   Namespace Descriptors section of the Bluetooth SIG Assigned Numbers
 * @returns Status of element addition
 * @retval SL_STATUS_OK If the element is added successfully.
 * @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If the node does not exist.
 * @retval SL_STATUS_INVALID_STATE If the DCD can't be modified because it was
 *   set available previously.
 * @retval SL_STATUS_ALLOCATION_FAILED If the memory allocation for the new
 *   element is failed.
 *
 * The new empty element is added to end of element array of DCD in BT Mesh
 * database. Therefore the elements shall be added in the increasing order of
 * element indexes.
 *
 ******************************************************************************/
sl_status_t btmesh_db_node_dcd_add_element(uint16_t prim_address,
                                           uint16_t location);

/***************************************************************************//**
 * Adds a model to the specified DCD element in BT Mesh database
 *
 * @param[in] prim_address Primary element address of the node
 * @param[in] element_index Index of element where the model resides on the node
 * @param[in] model_id Model ID of the model
 * @returns Status of the model addition
 * @retval SL_STATUS_OK If the model is added successfully.
 * @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If the node or element does not exist.
 * @retval SL_STATUS_INVALID_STATE If the DCD can't be modified because it was
 *   set available previously.
 * @retval SL_STATUS_ALREADY_EXISTS If the model ID is already added to element.
 *   The model IDs shall be unique in the context of a node element.
 * @retval SL_STATUS_ALLOCATION_FAILED If the memory allocation for the new
 *   model is failed.
 *
 ******************************************************************************/
sl_status_t btmesh_db_node_dcd_add_model(uint16_t prim_address,
                                         uint16_t element_index,
                                         btmesh_db_model_id_t model_id);

/***************************************************************************//**
 * Clears the DCD of the specified node in BT Mesh database
 *
 * @param[in] prim_address Primary element address of the node
 * @returns Status of the DCD clear operation
 * @retval SL_STATUS_OK If the DCD is cleared successfully.
 * @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If the node does not exist.
 *
 * The DCD clear operation deallocates all dynamically allocated resources
 * belonging to the DCD of the specified node and sets the DCD unavailable.
 * If the DCD was set available then it can be modified only if the current
 * DCD of the node is cleared first by calling this function.
 ******************************************************************************/
sl_status_t btmesh_db_node_dcd_clear(uint16_t prim_address);

/***************************************************************************//**
* Remove a node from a given network.
*
* @param[in] netkey_index Netkey index of the network
* @param[in] node Pointer to the node
* @return Status of the node removal.
* @retval SL_STATUS_OK If node is successfully removed.
* @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If either the network does
*                                          not exist or the node is not
*                                          in the network.
*******************************************************************************/
sl_status_t btmesh_db_remove_node_from_network(uint16_t netkey_index,
                                               btmesh_db_node_t *node);

/***************************************************************************//**
* Remove a node from a given network by the node's primary element address.
*
* @param[in] netkey_index Netkey index of the network
* @param[in] prim_address Primary address of the node
* @return Status of the node removal.
* @retval SL_STATUS_OK If node is successfully removed.
* @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If either the network or node does
*                                          not exist, or the node is not
*                                          in the network.
*******************************************************************************/
sl_status_t btmesh_db_remove_node_from_network_by_addr(uint16_t netkey_index,
                                                       uint16_t prim_address);

/***************************************************************************//**
* Remove a node from a given network by the node's UUID.
*
* @param[in] netkey_index Netkey index of the network
* @param[in] uuid UUID of the network
* @return Status of the node removal.
* @retval SL_STATUS_OK If node is successfully removed.
* @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If either the network or node does
*                                          not exist, or the node is not
*                                          in the network.
*******************************************************************************/
sl_status_t btmesh_db_remove_node_from_network_by_uuid(uint16_t netkey_index, uuid_128 uuid);

/***************************************************************************//**
* Remove a node from a given network by the node's Bluetooth address.
*
* @param[in] netkey_index Netkey index of the network
* @param[in] mac Bluetooth address of the node
* @return Status of the node removal.
* @retval SL_STATUS_OK If node is successfully removed.
* @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If either the network or node does
*                                          not exist, or the node is not
*                                          in the network.
*******************************************************************************/
sl_status_t btmesh_db_remove_node_from_network_by_mac(uint16_t netkey_index, bd_addr mac);

/***************************************************************************//**
* Remove all nodes from a give network
*
* @param[in] netkey_index Netkey index of the network
* @return Status of the node removal.
* @retval SL_STATUS_OK If nodes are successfully removed.
* @retval SL_STATUS_BT_MESH_DOES_NOT_EXIST If the network does not exist.
*******************************************************************************/
sl_status_t btmesh_db_remove_all_nodes_from_network(uint16_t netkey_index);

/***************************************************************************//**
* Get a node by its primary element address
*
* @param[in] address Primary element address of the node
* @return Pointer to the node, NULL if it does not exist.
*******************************************************************************/
btmesh_db_node_t* btmesh_db_node_get_by_addr(uint16_t address);

/***************************************************************************//**
* Get a node by its UUID
*
* @param[in] uuid UUID of the node
* @return Pointer to the node, NULL if it does not exist.
*******************************************************************************/
btmesh_db_node_t* btmesh_db_node_get_by_uuid(uuid_128 uuid);

/***************************************************************************//**
* Get a node by its Bluetooth address
*
* @param[in] mac Bluetooth address of the node
* @return Pointer to the node, NULL if it does not exist.
*******************************************************************************/
btmesh_db_node_t* btmesh_db_node_get_by_mac(bd_addr mac);

/***************************************************************************//**
* Set the availability parameter of a given node
*
* @param[in] node Pointer to the node
* @param[in] availability The requested status of the availability flag
* @return Status of the command.
* @retval SL_STATUS_OK If availability flag is set.
* @retval SL_STATUS_NULL_POINTER If the node does not exist.
*******************************************************************************/
sl_status_t btmesh_db_node_set_availability(btmesh_db_node_t *node,
                                            bool availability);

/***************************************************************************//**
* Set the Primary element address of a given node
*
* @param[in] node Pointer to the node
* @param[in] primary_address The requested primary element address
* @return Status of the command.
* @retval SL_STATUS_OK If primary element address is set.
* @retval SL_STATUS_NULL_POINTER If the node does not exist.
*******************************************************************************/
sl_status_t btmesh_db_node_set_primary_address(btmesh_db_node_t *node,
                                               uint16_t primary_address);

/***************************************************************************//**
* Get a network by its netkey index
*
* @param[in] netkey_index Netkey index of the requested network
* @return pointer to the network. NULL if network does not exist.
*******************************************************************************/
btmesh_db_network_t *btmesh_db_network_get_network(uint16_t netkey_index);

/***************************************************************************//**
* Check if a given node is part of a network
*
* @param[in] netkey_index Netkey index of the network
* @param[in] node Pointer to the node
* @retval TRUE If the node is part of the network.
* @retval FALSE If the node is not part of the network.
*******************************************************************************/
bool btmesh_db_is_node_in_network(uint16_t netkey_index, btmesh_db_node_t *node);

/***************************************************************************//**
* Check if a given node is part of any network
*
* @param[in] node Pointer to the node
* @retval TRUE If the node is part of a network.
* @retval FALSE If the node is not part of any network.
*******************************************************************************/
bool btmesh_db_is_node_in_any_network(btmesh_db_node_t *node);

/** @} (end addtogroup btmesh_db) */

#ifdef __cplusplus
};
#endif

#endif /* BTMESH_DB_H */
