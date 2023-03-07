/***************************************************************************/ /**
 * @file
 * @brief BT Mesh Configurator Component - Task
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

#ifndef BTMESH_CONF_TASK_H
#define BTMESH_CONF_TASK_H

#include <stdint.h>
#include "sl_status.h"
#include "sl_btmesh_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************//**
 * @addtogroup btmesh_conf BT Mesh Configurator Component
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_conf_task BT Mesh Configuration Task
 * @brief BT Mesh Configuration Tasks implement the different configuration
 *   procedures to set and get configuration server state of a node.
 *   @n Each specific configuration task type implements the interface specified
 *   in @ref btmesh_conf_task_t structure.
 * @{
 ******************************************************************************/

/// BT Mesh Configuration Task result is unknown because it is not finished yet
#define BTMESH_CONF_TASK_RESULT_UNKNOWN  ((sl_status_t) 0xFFFFFFFF)

/// BT Mesh Configuration Task identifiers
typedef enum btmesh_conf_task_id_t {
  /// Add application key
  BTMESH_CONF_TASK_ID_APPKEY_ADD,
  /// Remove application key
  BTMESH_CONF_TASK_ID_APPKEY_REMOVE,
  /// List application keys (all)
  BTMESH_CONF_TASK_ID_APPKEY_LIST,
  /// Bind application key to model
  BTMESH_CONF_TASK_ID_MODEL_BIND,
  /// Unbind application key to model
  BTMESH_CONF_TASK_ID_MODEL_UNBIND,
  /// List application keys bound to model
  BTMESH_CONF_TASK_ID_MODEL_BINDINGS_LIST,
  /// Set model publication data
  BTMESH_CONF_TASK_ID_MODEL_PUB_SET,
  /// Get model publication data
  BTMESH_CONF_TASK_ID_MODEL_PUB_GET,
  /// Add address to subscription list of a model
  BTMESH_CONF_TASK_ID_MODEL_SUB_ADD,
  /// Overwrite existing address in the subscription list of a model
  BTMESH_CONF_TASK_ID_MODEL_SUB_SET,
  /// Remove address from the subscription list of a model
  BTMESH_CONF_TASK_ID_MODEL_SUB_REMOVE,
  /// Clear addresses from the subscription list of a model
  BTMESH_CONF_TASK_ID_MODEL_SUB_CLEAR,
  /// List addresses in the subscription list of a model
  BTMESH_CONF_TASK_ID_MODEL_SUB_LIST,
  /// Set default TTL
  BTMESH_CONF_TASK_ID_DEFAULT_TTL_SET,
  /// Get default TTL
  BTMESH_CONF_TASK_ID_DEFAULT_TTL_GET,
  /// Set proxy feature
  BTMESH_CONF_TASK_ID_GATT_PROXY_SET,
  /// Get proxy feature
  BTMESH_CONF_TASK_ID_GATT_PROXY_GET,
  /// Set relay feature (feature turn on/off and retransmission parameters)
  BTMESH_CONF_TASK_ID_RELAY_SET,
  /// Get relay feature (feature on/off and retransmission parameters)
  BTMESH_CONF_TASK_ID_RELAY_GET,
  /// Set network transmit count and interval
  BTMESH_CONF_TASK_ID_NETWORK_TRANSMIT_SET,
  /// Get network transmit count and interval
  BTMESH_CONF_TASK_ID_NETWORK_TRANSMIT_GET,
  /// Set friend feature
  BTMESH_CONF_TASK_ID_FRIEND_SET,
  /// Get friend feature
  BTMESH_CONF_TASK_ID_FRIEND_GET,
  /// Get Device Composition Data
  BTMESH_CONF_TASK_ID_DCD_GET,
  /// Reset remote node
  BTMESH_CONF_TASK_ID_RESET_NODE,
  /// Number of configuration tasks
  BTMESH_CONF_TASK_ID_COUNT,
  /// Invalid configuration task
  BTMESH_CONF_TASK_ID_INVALID
} btmesh_conf_task_id_t;

/// Task specific data of add/remove application key data
typedef struct {
  /// Index of the application key
  uint16_t appkey_index;
  /// Index of the network key which the application key is bound to
  uint16_t netkey_index;
} btmesh_conf_task_appkey_t;

/// Task specific data of list application keys (all)
typedef struct {
  /// Network key index for the key used as the query parameter
  uint16_t netkey_index;
  /// List of application key indexes which are bound to the network key index
  uint16_t *appkey_indexes;
  /// Number of application key indexes which are bound to the network key index
  uint16_t appkey_count;
} btmesh_conf_task_appkey_list_t;

/// Task specific data of bind/unbind application key to model
typedef struct {
  /// Index of the element where the model resides on the node
  uint8_t elem_index;
  /// Vendor ID for the model (0xFFFF for Bluetooth SIG models)
  uint16_t vendor_id;
  /// Model ID for the model
  uint16_t model_id;
  /// Index of the application key bound to the model
  uint16_t appkey_index;
} btmesh_conf_task_model_binding_t;

/// Task specific data of list application keys bound to model
typedef struct {
  /// Index of the element where the model resides on the node
  uint8_t elem_index;
  /// Vendor ID for the model (0xFFFF for Bluetooth SIG models)
  uint16_t vendor_id;
  /// Model ID for the model
  uint16_t model_id;
  /// List of key indexes for the application keys which are bound to a model
  uint16_t *appkey_indexes;
  /// Number of key indexes for the application keys which are bound to a model
  uint16_t appkey_count;
} btmesh_conf_task_model_bindings_list_t;

/// Task specific data of set/get model publication data
typedef struct {
  /// Index of the element where the model resides on the node
  uint8_t elem_index;
  /// Vendor ID for the model (0xFFFF for Bluetooth SIG models)
  uint16_t vendor_id;
  /// Model ID for the model
  uint16_t model_id;
  /// @brief The unicast or group address to publish to.
  /// It can also be the unassigned address to stop the model from publishing.
  /// Note: Task type determines whether @p address or @p va_addres is used.
  uint16_t address;
  /// @brief The virtual address to publish to.
  /// Note: Task type determines whether @p address or @p va_addres is used.
  uuid_128 va_address;
  /// The application key index to use for the published messages.
  uint16_t appkey_index;
  /// @brief Friendship credential flag. If zero, publication is done using
  /// normal credentials. If one, it is done with friendship credentials,
  /// meaning only the friend can decrypt the published message and relay it
  /// forward using the normal credentials. The default value is 0.
  uint8_t credentials;
  /// Publication time-to-live value
  uint8_t ttl;
  /// @brief Publication period in milliseconds. Note that the resolution of the
  /// publication period is limited by the specification to 100 ms up to a
  /// period of 6.3 s, 1 s up to a period of 63 s, 10 s up to a period
  /// of 630 s, and 10 minutes above that. Maximum period allowed is 630 minutes.
  uint32_t period_ms;
  /// Publication retransmission count. Valid values range from 0 to 7.
  uint8_t retransmit_count;
  /// @brief Publication retransmission interval in millisecond units. The range of
  /// value is 50 to 1600 ms, and the resolution of the value is 50 milliseconds.
  uint16_t retransmit_interval_ms;
} btmesh_conf_task_model_pub_t;

/// @brief Task specific data of the following model subscription list operations
///   - Add address to model subscription list
///   - Set address to model subscription list (clear list & add group address)
///   - Remove address to model subscription list
typedef struct {
  /// Index of the element where the model resides on the node
  uint8_t elem_index;
  /// Vendor ID for the model (0xFFFF for Bluetooth SIG models)
  uint16_t vendor_id;
  /// Model ID for the model
  uint16_t model_id;
  /// @brief The group address in the subscription list.
  /// Note: Task type determines whether @ref sub_address or @ref sub_address_va
  /// is used.
  uint16_t sub_address;
  /// @brief The full virtual address in the subscription list.
  /// Note: Task type determines whether @ref sub_address or @ref sub_address_va
  /// is used.
  uuid_128 sub_address_va;
} btmesh_conf_task_model_sub_t;

/// Task specific data of clear addresses from the subscription list of a model
typedef struct {
  /// Index of the element where the model resides on the node
  uint8_t elem_index;
  /// Vendor ID for the model (0xFFFF for Bluetooth SIG models)
  uint16_t vendor_id;
  /// Model ID for the model
  uint16_t model_id;
} btmesh_conf_task_model_sub_clear_t;

/// Task specific data of list addresses in the subscription list of a model
typedef struct {
  /// Index of the element where the model resides on the node
  uint8_t elem_index;
  /// Vendor ID for the model (0xFFFF for Bluetooth SIG models)
  uint16_t vendor_id;
  /// Model ID for the model
  uint16_t model_id;
  /// @brief List of subscription addresses
  /// @n If the subscription address list entry is a Label UUID
  /// (full virtual address) then the corresponding virtual address
  /// hash is returned in this event.
  uint16_t *addresses;
  /// Number of subscription addresses
  uint16_t address_count;
} btmesh_conf_task_model_sub_list_t;

/// Task specific data of set/get default TTL
typedef struct {
  /// @brief Default TTL value. Valid value range is from 2 to 127 for relayed
  /// PDUs, and 0 to indicate none - relayed PDUs
  uint8_t ttl;
} btmesh_conf_task_default_ttl_t;

/// Task specific data of set/get proxy feature
typedef struct {
  /// @brief GATT proxy value. Valid values are:
  ///     - 0: Proxy feature is disabled
  ///     - 1: Proxy feature is enabled
  uint8_t value;
} btmesh_conf_task_gatt_proxy_t;

/// Task specific data of set/get relay feature
typedef struct {
  /// @brief Relay value. Valid values are:
  ///     - 0: Relay feature is disabled
  ///     - 1: Relay feature is enabled
  uint8_t value;
  /// @brief Relay retransmit count. Valid values range from 0 to 7;
  /// default value is 0 (no retransmissions).
  uint8_t retransmit_count;
  /// @brief Relay retransmit interval in milliseconds.
  /// Valid values range from 10 ms to 320 ms, with a resolution of 10 ms.
  /// The value is ignored if the retransmission count is set to zero.
  uint16_t retransmit_interval_ms;
} btmesh_conf_task_relay_t;

/// Task specific data of set/get network transmit count and interval
typedef struct {
  /// @brief Network transmit count. Valid values range from 1 to 8;
  /// default value is 1 (single transmission; no retransmissions).
  uint8_t transmit_count;
  /// @brief Network transmit interval in milliseconds.
  /// Valid values range from 10 ms to 320 ms, with a resolution of 10 ms.
  /// The value is ignored if the transmission count is set to one.
  uint16_t transmit_interval_ms;
} btmesh_conf_task_network_transmit_t;

/// Task specific data of set/get friend feature
typedef struct {
  /// @brief Friend value. Valid values are:
  ///     - 0: Friend feature is not enabled
  ///     - 1: Friend feature is enabled
  uint8_t value;
} btmesh_conf_task_friend_t;

/// Task specific data of get Device Composition Data (DCD)
typedef struct {
  /// Composition data page containing data
  uint8_t page;
  /// Raw DCD data of the remote node
  uint8_t *raw_dcd_data;
  /// Size of raw DCD data
  uint16_t raw_dcd_data_size;
} btmesh_conf_task_dcd_get_t;

/// Union of BT Mesh Configuration Task specific data structures
typedef union btmesh_conf_task_ext_t {
  /// Task specific data of add/remove application key data
  btmesh_conf_task_appkey_t appkey;
  /// Task specific data of list application keys (all)
  btmesh_conf_task_appkey_list_t appkey_list;
  /// Task specific data of bind/unbind application key to model
  btmesh_conf_task_model_binding_t model_binding;
  /// Task specific data of list application keys bound to model
  btmesh_conf_task_model_bindings_list_t model_bindings_list;
  /// Task specific data of set/get model publication data
  btmesh_conf_task_model_pub_t model_pub;
  /// Task specific data of add/set/remove model subscription list operations
  btmesh_conf_task_model_sub_t model_sub;
  /// Task specific data of clear addresses from the subscription list of a model
  btmesh_conf_task_model_sub_clear_t model_sub_clear;
  /// Task specific data of list addresses in the subscription list of a model
  btmesh_conf_task_model_sub_list_t model_sub_list;
  /// Task specific data of set/get default TTL
  btmesh_conf_task_default_ttl_t default_ttl;
  /// Task specific data of set/get proxy feature
  btmesh_conf_task_gatt_proxy_t gatt_proxy;
  /// Task specific data of set/get relay feature
  btmesh_conf_task_relay_t relay;
  /// Task specific data of set/get network transmit count and interval
  btmesh_conf_task_network_transmit_t network_transmit;
  /// Task specific data of set/get friend feature
  btmesh_conf_task_friend_t friend;
  /// Task specific data of get Device Composition Data (DCD)
  btmesh_conf_task_dcd_get_t dcd;
} btmesh_conf_task_ext_t;

typedef struct btmesh_conf_task_t btmesh_conf_task_t;

/// @brief BT Mesh Configuration Task is abstraction of configuration procedures
/// @n The specific configuration tasks are instances of this structure with
///    different task identifiers and function pointers (e.g. @p conf_request,
///    @p on_event etc.) furthermore different members of the
///    @p btmesh_conf_task_ext_t union is used based on the task type.
struct btmesh_conf_task_t {
  /// Task identifier which determines the specific task type
  btmesh_conf_task_id_t task_id;
  /// @brief Task result.
  /// Initialized to @ref BTMESH_CONF_TASK_RESULT_UNKNOWN and it set to the
  /// final value at the end of task.
  /// @n If the configuration task was successful then it is set to SL_STATUS_OK.
  /// @n If @p conf_request or @p on_event fails then it set to an error code
  /// returned by the BT Mesh Stack.
  sl_status_t result;
  /// @brief Abstract configuration request which sends the task specific
  /// configuration request to the configuration server on a node.
  /// @param[in] self Pointer to the configuration task instance
  /// @param[in] enc_netkey_index Network key used to encrypt the config request
  /// @param[in] server_address Destination node primary element address
  /// @param[out] handle BT Mesh Stack configuration handle which is returned
  ///   by the BT Mesh Stack configuration request (BT Mesh Stack API)
  /// @return Task request status
  /// @retval SL_STATUS_OK If request is accepted.
  /// @retval SL_STATUS_BUSY If request is rejected due to busy lower layers.
  /// @retval SL_STATUS_FAIL If request is rejected due to unrecoverable error.
  sl_status_t (*conf_request)(btmesh_conf_task_t *const self,
                              uint16_t enc_netkey_index,
                              uint16_t server_address,
                              uint32_t *handle);
  /// @brief Abstract configuration event handler which processes the BT Mesh
  ///   Stack event.
  /// @n Only those BT Mesh Configuration Client events shall be forwarded to
  /// this task event handler which matches the configuration handle of the last
  /// @p conf_request in order to make task implementation simpler.
  /// @param[in] self Pointer to the configuration task instance
  /// @param[in] enc_netkey_index Network key used to encrypt the config request
  /// @param[in] server_address Destination node primary element address
  /// @param[in] evt BT Mesh Stack event
  /// @return Current status of the task.
  /// @retval SL_STATUS_OK Task is finished successfully.
  /// @retval SL_STATUS_IN_PROGRESS Task is waiting for additional events.
  /// @retval SL_STATUS_TIMEOUT Task timed out while it was waiting for events.
  /// @retval SL_STATUS_FAIL Task failed due to server side error.
  /// @retval SL_STATUS_ABORT Task failed unexpectedly and config task shall be.
  ///   aborted so cancellation of config request is required.
  sl_status_t (*on_event)(btmesh_conf_task_t *const self,
                          uint16_t enc_netkey_index,
                          uint16_t server_address,
                          const sl_btmesh_msg_t *evt);
  /// @brief Abstract function which provides the string representation of the
  ///   configuration task.
  /// @n The function always writes null terminated string into the @p buffer.
  /// @param[in] self Pointer to the configuration task instance
  /// @param[out] buffer Buffer where the string representation is written to
  /// @param[in] size Size of the output @p buffer
  /// @return Return value of functions shall be interpreted as snprintf.
  int32_t (*to_string)(btmesh_conf_task_t *const self,
                       char *buffer,
                       uint32_t size);
  /// @brief Deallocation of BT Mesh Configuration Task instance
  /// @param[in] self Pointer to the configuration task instance
  void (*destroy)(btmesh_conf_task_t *const self);
  /// Next task if this task is successful (@p result is SL_STATUS_OK)
  btmesh_conf_task_t *next_on_success;
  /// Next task if this task fails (@p result is not SL_STATUS_OK)
  btmesh_conf_task_t *next_on_failure;
  /// @brief Task specific data (parameters, runtime data, results)
  /// @n Union of task specific data structures.
  btmesh_conf_task_ext_t ext;
};

/***************************************************************************//**
 * Create generic BT Mesh Configuration Task
 *
 * @param[in] task_id Task identifier which shall be set
 * @return Created configuration task.
 * @retval NULL If the configuration task creation fails.
 *
 * Allocates and initializes a generic configuration task.
 * @warning This function shall be called from derived (specific) configuration
 *   task create functions only to create an initialized task instance which
 *   is further customized by the specific configuration task create function.
 *   For example @ref btmesh_conf_task_appkey_add_create.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_create(btmesh_conf_task_id_t task_id);

/***************************************************************************//**
 * Deallocates the BT Mesh Configuration Task instance
 *
 * @param[in] self Pointer to the configuration task which shall be destroyed
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Set next task pointers of the BT Mesh Configuration Task to the specified
 * other task.
 *
 * @param[in] self Pointer to the configuration task
 * @param[in] next_task Next task which shall be executed after this task (@p self)
 * @returns Status of next task setup.
 * @retval SL_STATUS_OK If the next task is set successfully.
 * @retval SL_STATUS_NULL_POINTER If @p self or @p next_task is NULL.
 * @retval SL_STATUS_INVALID_PARAMETER  If the @p self and @p next_task is the
 *   same or the next task of @p next_task is not NULL.
 *
 * The task which shall be executed after the end of @p self task is set to
 * @p next_task. The @p next_task is independent from the result of @p self task.
 *
 * @warning It is expected that the @p next_task does not have any next task
 *   when this function is called in order to guarantee that there will be no
 *   cyclic references in the task tree. Therefore, the task tree shall be built
 *   in the order of execution. This is enforced by the implementation and
 *   SL_STATUS_INVALID_PARAMETER is returned if this rule is not respected.
 ******************************************************************************/
sl_status_t btmesh_conf_task_set_next_unconditional(btmesh_conf_task_t *self,
                                                    btmesh_conf_task_t *const next_task);

/***************************************************************************//**
 * Set next task pointer of the BT Mesh Configuration Task to the specified
 * other task if this task is completed successfully.
 *
 * @param[in] self Pointer to the configuration task
 * @param[in] next_task Next task which shall be executed if this task (@p self)
 *   is completed successfully.
 * @returns Status of next task setup.
 * @retval SL_STATUS_OK If the next task is set successfully.
 * @retval SL_STATUS_NULL_POINTER If @p self or @p next_task is NULL.
 * @retval SL_STATUS_INVALID_PARAMETER  If the @p self and @p next_task is the
 *   same or the next task of @p next_task is not NULL.
 *
 * @warning It is expected that the @p next_task does not have any next task
 *   when this function is called in order to guarantee that there will be no
 *   cyclic references in the task tree. Therefore, the task tree shall be built
 *   in the order of execution. This is enforced by the implementation and
 *   SL_STATUS_INVALID_PARAMETER is returned if this rule is not respected.
 ******************************************************************************/
sl_status_t btmesh_conf_task_set_next_on_success(btmesh_conf_task_t *self,
                                                 btmesh_conf_task_t *const next_task);

/***************************************************************************//**
 * Set next task pointer of the BT Mesh Configuration Task to the specified
 * other task if this task fails.
 *
 * @param[in] self Pointer to the configuration task
 * @param[in] next_task Next task which shall be executed if this task
 *   (@p self) fails.
 * @returns Status of next task setup.
 * @retval SL_STATUS_OK If the next task is set successfully.
 * @retval SL_STATUS_NULL_POINTER If @p self or @p next_task is NULL.
 * @retval SL_STATUS_INVALID_PARAMETER  If the @p self and @p next_task is the
 *   same or the next task of @p next_task is not NULL.
 *
 * @warning It is expected that the @p next_task does not have any next task
 *   when this function is called in order to guarantee that there will be no
 *   cyclic references in the task tree. Therefore, the task tree shall be built
 *   in the order of execution. This is enforced by the implementation and
 *   SL_STATUS_INVALID_PARAMETER is returned if this rule is not respected.
 ******************************************************************************/
sl_status_t btmesh_conf_task_set_next_on_failure(btmesh_conf_task_t *self,
                                                 btmesh_conf_task_t *const next_task);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to add an application key to a node.
 *
 * @param[in] appkey_index Index of the application key
 * @param[in] netkey_index Index of the network key which the application key
 *   is bound to
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_appkey_add_create(uint16_t appkey_index,
                                                       uint16_t netkey_index);
/***************************************************************************//**
 * Deallocation of Add Application Key BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_appkey_add_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to remove an application key from a node.
 *
 * @param[in] appkey_index Index of the application key
 * @param[in] netkey_index Index of the network key which the application key
 *   is bound to
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_appkey_remove_create(uint16_t appkey_index,
                                                          uint16_t netkey_index);

/***************************************************************************//**
 * Deallocation of Remove Application Key BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_appkey_remove_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to list the application keys on a node.
 *
 * @param[in] netkey_index Network key index for the key used as query parameter
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_appkey_list_create(uint16_t netkey_index);

/***************************************************************************//**
 * Deallocation of List Application Keys BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_appkey_list_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to bind an application key to a model.
 *
 * @param[in] elem_index Index of the element where the model resides on the node
 * @param[in] vendor_id Vendor ID for the model (0xFFFF for Bluetooth SIG models)
 * @param[in] model_id Model ID for the model
 * @param[in] appkey_index Index of the application key to bind to the model
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_model_bind_create(uint8_t elem_index,
                                                       uint16_t vendor_id,
                                                       uint16_t model_id,
                                                       uint16_t appkey_index);

/***************************************************************************//**
 * Deallocation of Bind Model BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_model_bind_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to unbind an application key from a model.
 *
 * @param[in] elem_index Index of the element where the model resides on the node
 * @param[in] vendor_id Vendor ID for the model (0xFFFF for Bluetooth SIG models)
 * @param[in] model_id Model ID for the model
 * @param[in] appkey_index Index of the application key to unbind from the model
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_model_unbind_create(uint8_t elem_index,
                                                         uint16_t vendor_id,
                                                         uint16_t model_id,
                                                         uint16_t appkey_index);
/***************************************************************************//**
 * Deallocation of Unbind Model BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_model_unbind_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to list application key bindings of a model.
 *
 * @param[in] elem_index Index of the element where the model resides on the node
 * @param[in] vendor_id Vendor ID for the model (0xFFFF for Bluetooth SIG models)
 * @param[in] model_id Model ID for the model
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_model_bindings_list_create(uint8_t elem_index,
                                                                uint16_t vendor_id,
                                                                uint16_t model_id);

/***************************************************************************//**
 * Deallocation of BT List Model Bindings Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_model_bindings_list_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to set the model publication state.
 *
 * @param[in] elem_index Index of the element where the model resides on the node.
 * @param[in] vendor_id Vendor ID for the model. (0xFFFF for Bluetooth SIG models)
 * @param[in] model_id Model ID for the model.
 * @param[in] address The unicast or group address to publish to. It can also be
 *   the unassigned address to stop the model from publishing.
 * @param[in] appkey_index Application key index to use for published messages.
 * @param[in] credentials Friendship credential flag. The default value is 0.
 *   - If zero, publication is done using normal credentials.
 *   - If one, it is done with friendship credentials, meaning only the friend
 *     can decrypt the published message and relay it forward using the normal
 *     credentials.
 * @param[in] ttl Publication time-to-live value
 * @param[in] period_ms Publication period in milliseconds.
 *   Note that the resolution of the publication period is limited by the
 *   specification to 100ms up to a period of 6.3s, 1s up to a period of 63s,
 *   10s up to a period of 630s, and 10 minutes above that.
 *   Maximum period allowed is 630 minutes.
 * @param[in] retransmit_count Publication retransmission count. Valid values
 *   range from 0 to 7.
 * @param[in] retransmit_interval_ms Publication retransmission interval in
 *   millisecond units. The range of value is 50 to 1600 ms, and the resolution
 *   of the value is 50 milliseconds.
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_model_pub_set_create(uint8_t elem_index,
                                                          uint16_t vendor_id,
                                                          uint16_t model_id,
                                                          uint16_t address,
                                                          uint16_t appkey_index,
                                                          uint8_t credentials,
                                                          uint8_t ttl,
                                                          uint32_t period_ms,
                                                          uint8_t retransmit_count,
                                                          uint16_t retransmit_interval_ms);

/***************************************************************************//**
 * Deallocation of Set Model Publication BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_model_pub_set_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to get the model publication state.
 *
 * @param[in] elem_index Index of the element where the model resides on the node.
 * @param[in] vendor_id Vendor ID for the model. (0xFFFF for Bluetooth SIG models)
 * @param[in] model_id Model ID for the model.
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_model_pub_get_create(uint8_t elem_index,
                                                          uint16_t vendor_id,
                                                          uint16_t model_id);
/***************************************************************************//**
 * Deallocation of Get Model Publication BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_model_pub_get_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to add an address to the model
 * subscription list.
 *
 * @param[in] elem_index Index of the element where the model resides on the node.
 * @param[in] vendor_id Vendor ID for the model. (0xFFFF for Bluetooth SIG models)
 * @param[in] model_id Model ID for the model.
 * @param[in] sub_address Group address which shall be added to the subscription
 *   list of the model.
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_model_sub_add_create(uint8_t elem_index,
                                                          uint16_t vendor_id,
                                                          uint16_t model_id,
                                                          uint16_t sub_address);

/***************************************************************************//**
 * Deallocation of Add Model Subscription BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_model_sub_add_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to set (overwrite) model subscription
 * address list to a single address.
 * See Config Model Subscription Overwrite BT Mesh message in foundation models
 * chapter of BT Mesh Profile Specification.
 *
 * @param[in] elem_index Index of the element where the model resides on the node.
 * @param[in] vendor_id Vendor ID for the model. (0xFFFF for Bluetooth SIG models)
 * @param[in] model_id Model ID for the model.
 * @param[in] sub_address Group address which shall be added to the cleared
 *   subscription list of the model.
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_model_sub_set_create(uint8_t elem_index,
                                                          uint16_t vendor_id,
                                                          uint16_t model_id,
                                                          uint16_t sub_address);

/***************************************************************************//**
 * Deallocation of Set Model Subscription BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_model_sub_set_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to remove an address from the model
 * subscription list.
 *
 * @param[in] elem_index Index of the element where the model resides on the node.
 * @param[in] vendor_id Vendor ID for the model. (0xFFFF for Bluetooth SIG models)
 * @param[in] model_id Model ID for the model.
 * @param[in] sub_address Group address which shall be removed from the
 *   subscription list of the model.
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_model_sub_remove_create(uint8_t elem_index,
                                                             uint16_t vendor_id,
                                                             uint16_t model_id,
                                                             uint16_t sub_address);
/***************************************************************************//**
 * Deallocation of Remove Model Subscription BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_model_sub_remove_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to clear (empty) the model subscription
 * address list.
 *
 * @param[in] elem_index Index of the element where the model resides on the node.
 * @param[in] vendor_id Vendor ID for the model. (0xFFFF for Bluetooth SIG models)
 * @param[in] model_id Model ID for the model.
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_model_sub_clear_create(uint8_t elem_index,
                                                            uint16_t vendor_id,
                                                            uint16_t model_id);

/***************************************************************************//**
 * Deallocation of Clear Model Subscription List BT Mesh Configuration
 * Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_model_sub_clear_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to get the subscription address list of
 * a model.
 *
 * @param[in] elem_index Index of the element where the model resides on the node.
 * @param[in] vendor_id Vendor ID for the model. (0xFFFF for Bluetooth SIG models)
 * @param[in] model_id Model ID for the model.
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_model_sub_list_create(uint8_t elem_index,
                                                           uint16_t vendor_id,
                                                           uint16_t model_id);
/***************************************************************************//**
 * Deallocation of List Model Subscription BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_model_sub_list_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to set node default TTL state.
 *
 * @param[in] default_ttl Default TTL value. Valid value range is from 2 to 127
 *   for relayed PDUs, and 0 to indicate non-relayed PDUs.
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_default_ttl_set_create(uint8_t default_ttl);

/***************************************************************************//**
 * Deallocation of Set Default TTL BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_default_ttl_set_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to get node default TTL state.
 *
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_default_ttl_get_create(void);

/***************************************************************************//**
 * Deallocation of Get Default TTL BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_default_ttl_get_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to set node GATT proxy state.
 *
 * @param[in] value GATT proxy value to set.
 *   Valid values are:
 *     - 0: Proxy feature is disabled
 *     - 1: Proxy feature is enabled
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_gatt_proxy_set_create(uint8_t value);

/***************************************************************************//**
 * Deallocation of Set GATT Proxy BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_gatt_proxy_set_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to get node GATT proxy state.
 *
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_gatt_proxy_get_create(void);

/***************************************************************************//**
 * Deallocation of Get GATT Proxy BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_gatt_proxy_get_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to set node relay state.
 *
 * @param[in] value Relay value to set.
 *   Valid values are:
 *     - 0: Relay feature is disabled
 *     - 1: Relay feature is enabled
 * @param[in] retransmit_count Relay retransmit count. Valid values range from 0
 *   to 7; default value is 0 (no retransmissions).
 * @param[in] retransmit_interval_ms Relay retransmit interval in milliseconds.
 *   Valid values range from 10 ms to 320 ms, with a resolution of 10 ms. The
 *   value is ignored if the retransmission count is set to zero.
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_relay_set_create(uint8_t value,
                                                      uint8_t retransmit_count,
                                                      uint16_t retransmit_interval_ms);
/***************************************************************************//**
 * Deallocation of Set Relay BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_relay_set_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to get node relay state.
 *
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_relay_get_create(void);

/***************************************************************************//**
 * Deallocation of Get Relay BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_relay_get_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to set node network transmit state.
 *
 * @param[in] transmit_count Network transmit count. Valid values range from 1
 *   to 8; default value is 1 (single transmission; no retransmissions).
 * @param[in] transmit_interval_ms Network transmit interval in milliseconds.
 *   Valid values range from 10 ms to 320 ms, with a resolution of 10 ms.
 *   The value is ignored if the transmission count is set to one.
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_network_transmit_set_create(uint8_t transmit_count,
                                                                 uint16_t transmit_interval_ms);

/***************************************************************************//**
 * Deallocation of Set Network Transmit BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_network_transmit_set_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to get node network transmit state.
 *
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_network_transmit_get_create(void);

/***************************************************************************//**
 * Deallocation of Get Network Transmit BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_network_transmit_get_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to set node friend state.
 *
 * @param[in] value Friend value to set. Valid values are:
 *   - 0: Friend feature is not enabled
 *   - 1: Friend feature is enabled
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_friend_set_create(uint8_t value);

/***************************************************************************//**
 * Deallocation of Set Friend BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_friend_set_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to get node friend state.
 *
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_friend_get_create(void);

/***************************************************************************//**
 * Deallocation of Get Friend BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_friend_get_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to get composition data of a device.
 *
 * @param[in] page Composition data page to query
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_dcd_get_create(uint8_t page);

/***************************************************************************//**
 * Deallocation of Get DCD BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_dcd_get_destroy(btmesh_conf_task_t *self);

/***************************************************************************//**
 * Create BT Mesh Configuration Task to request a node to unprovision itself.
 * Use this function when a node shall be removed from the network.
 *
 * @returns Created configuration task.
 * @retval NULL If the configuration task creation fails.
 ******************************************************************************/
btmesh_conf_task_t *btmesh_conf_task_reset_node_create(void);

/***************************************************************************//**
 * Deallocation of Reset Node BT Mesh Configuration Task instance.
 *
 * @param[in] self Pointer to the configuration task instance
 *
 * If btmesh_conf_task_t::next_on_success or btmesh_conf_task_t::next_on_failure
 * is set then those tasks are destroyed as well.
 ******************************************************************************/
void btmesh_conf_task_reset_node_destroy(btmesh_conf_task_t *self);

/** @} (end addtogroup btmesh_conf_task) */
/** @} (end addtogroup btmesh_conf) */

#ifdef __cplusplus
};
#endif

#endif /* BTMESH_CONF_TASK_H */
