// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <app/CASESessionManager.h>
#include <app/DeviceProxy.h>
#include <app/InteractionModelEngine.h>
#include <app/util/af-types.h>
#include <esp_err.h>
#include <esp_matter_attribute_utils.h>
#include <app/AttributePathParams.h>
#include <app/CommandPathParams.h>
#include <app/EventPathParams.h>

using chip::app::ConcreteCommandPath;
using chip::DeviceLayer::ChipDeviceEvent;
using chip::TLV::TLVReader;

namespace esp_matter {

/** Generic handle */
typedef size_t handle_t;
/** Node handle */
typedef handle_t node_t;
/** Endpoint handle */
typedef handle_t endpoint_t;
/** Cluster handle */
typedef handle_t cluster_t;
/** Attribute handle */
typedef handle_t attribute_t;
/** Command handle */
typedef handle_t command_t;
/** Event handle */
typedef handle_t event_t;

/** Event callback
 *
 * @param[in] event event data pointer.
 * @param[in] arg Pointer to the private data passed while setting the callback.
 */
/** TODO: Change this */
typedef void (*event_callback_t)(const ChipDeviceEvent *event, intptr_t arg);

/** Return whether the Matter is intialized and started
 *
 *  @return true if Matter is started
 *  @return false if Matter is not started
 */
bool is_started();

/** ESP Matter Start
 *
 * Initialize and start the matter thread.
 *
 * @param[in] callback event callback.
 * @param[in] callback_arg private data to pass to callback function, optional argument, by default set to NULL.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t start(event_callback_t callback, intptr_t callback_arg = static_cast<intptr_t>(NULL));

/** Factory reset
 *
 * Perform factory reset and erase the data stored in the non volatile storage. This also restarts the device.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t factory_reset();

namespace lock {

/** Lock status */
typedef enum status {
    /** Lock failed */
    FAILED,
    /** Lock was already taken */
    ALREADY_TAKEN,
    /** Lock success */
    SUCCESS,
} status_t;

/** Stack lock
 *
 * This API should be called before calling any upstream APIs.
 *
 * @param[in] ticks_to_wait number of ticks to wait for trying to take the lock. Accepted values: 0 to portMAX_DELAY.
 *
 * @return FAILED if the lock was not taken within the specified ticks.
 * @return ALREADY_TAKEN if the lock was already taken by the same task context.
 * @return SUCCESS if the lock was taken successfully.
 */
status_t chip_stack_lock(uint32_t ticks_to_wait);

/** Stack unlock
 *
 * This API should be called after the upstream APIs have been done calling.
 *
 * @note: This should only be called if `chip_stack_lock()` returns `SUCCESS`.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t chip_stack_unlock();

} /* lock */

namespace node {

/** Create raw node
 *
 * @return Node handle on success.
 * @return NULL in case of failure.
 */
node_t *create_raw();

/** Destroy raw node
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t destroy_raw();

/** Get node
 *
 * @return Node handle on success.
 * @return NULL in case of failure.
 */
node_t *get();

/** Destroy node
 *
 * This will destroy the node and all the endpoints, clusters, attributes, commands and events associated with it.
 *
 * @note: Call this function only if matter is not running.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t destroy();

/** Get the endpoint count for a server cluster
 *
 * Get the number of endpoints that have the given cluster ID as a server cluster.
 *
 * @param[in] cluster_id Cluster ID.
 *
 * @return Endpoint count on success.
 * @return 0 in case of failure or if not found on any endpoint.
 */

uint32_t get_server_cluster_endpoint_count(uint32_t cluster_id);

/** Get the endpoint count for a client cluster
 *
 * Get the number of endpoints that have the given cluster ID as a client cluster.
 *
 * @param[in] cluster_id Cluster ID.
 *
 * @return Endpoint count on success.
 * @return 0 in case of failure or if not found on any endpoint.
 */
uint32_t get_client_cluster_endpoint_count(uint32_t cluster_id);
} /* node */

namespace endpoint {

/** Create endpoint
 *
 * This will create a new endpoint with a unique endpoint_id and add the endpoint to the node.
 *
 * @param[in] node Node handle.
 * @param[in] flags Bitmap of `endpoint_flags_t`.
 * @param[in] priv_data (Optional) Private data associated with the endpoint. This will be passed to the
 * attribute_update and identify callbacks. It should stay allocated throughout the lifetime of the device.
 *
 * @return Endpoint handle on success.
 * @return NULL in case of failure.
 */
endpoint_t *create(node_t *node, uint8_t flags, void *priv_data);

/** Resume endpoint
 *
 * This will resume an endpoint after reboot and add it to the node.
 *
 * @param[in] node Node handle.
 * @param[in] flags Bitmap of `endpoint_flags_t`.
 * @param[in] endpoint_id Endpoint ID of the endpoint resumed.
 * @param[in] priv_data (Optional) Private data associated with the endpoint. This will be passed to the
 * attribute_update and identify callbacks. It should stay allocated throughout the lifetime of the device.
 *
 * @return Endpoint handle on success.
 * @return NULL in case of failure.
 */
endpoint_t *resume(node_t *node, uint8_t flags, uint16_t endpoint_id, void *priv_data);

/** Destroy endpoint
 *
 * This will destroy the endpoint which has been created and added to the node. It also destroys the associated
 * clusters, attributes and commands.
 *
 * @param[in] node Node handle.
 * @param[in] endpoint Endpoint handle.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t destroy(node_t *node, endpoint_t *endpoint);

/** Get endpoint
 *
 * Get the endpoint present on the node.
 *
 * @param[in] node Node handle.
 * @param[in] endpoint_id Endpoint ID of the endpoint.
 *
 * @return Endpoint handle on success.
 * @return NULL in case of failure.
 */
endpoint_t *get(node_t *node, uint16_t endpoint_id);

/** Get endpoint
 *
 * Get the endpoint present on the node.
 *
 * @param[in] endpoint_id Endpoint ID of the endpoint.
 *
 * @return Endpoint handle on success.
 * @return NULL in case of failure.
 */
endpoint_t *get(uint16_t endpoint_id);

/** Get first endpoint
 *
 * Get the first endpoint present on the node.
 *
 * @param[in] node Node handle.
 *
 * @return Endpoint handle on success.
 * @return NULL in case of failure.
 */
endpoint_t *get_first(node_t *node);

/** Get next endpoint
 *
 * Get the next endpoint present on the node.
 *
 * @param[in] endpoint Endpoint handle.
 *
 * @return Endpoint handle on success.
 * @return NULL in case of failure.
 */
endpoint_t *get_next(endpoint_t *endpoint);

/** Get endpoint count
 *
 * Get the endpoint count present on the node.
 *
 * @param[in] node Node handle.
 *
 * @return Endpoint count on success.
 * @return 0 in case of failure.
 */
uint16_t get_count(node_t *node);

/** Get endpoint ID
 *
 * Get the endpoint ID for the endpoint.
 *
 * @param[in] endpoint Endpoint handle.
 *
 * @return Endpoint ID on success.
 * @return Invalid Endpoint ID (0xFFFF) in case of failure.
 */
uint16_t get_id(endpoint_t *endpoint);

/** Add device type ID and verision
 *
 * Add the device type ID and version for the endpoint.
 *
 * @param[in] endpoint Endpoint handle.
 * @param[in] device_type_id Device type ID.
 * @param[in] device_type_version Device type version.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t add_device_type(endpoint_t *endpoint, uint32_t device_type_id, uint8_t device_type_version);

/** Get device type ID array
 *
 * Get the device type ID array for the endpoint. This array is aligned with the device type version array.
 *
 * @param[in] endpoint Endpoint handle.
 * @param[out] device_type_count_ptr the pointer of device type ID array length.
 *
 * @return device type ID array on success.
 * @return NULL when the endpoint or the device_type_count_ptr is NULL.
 */
uint32_t *get_device_type_ids(endpoint_t *endpoint, uint8_t *device_type_count_ptr);

/** Get device type version array
 *
 * Get the device type version array for the endpoint. This array is aligned with the device type ID array.
 *
 * @param[in] endpoint Endpoint handle.
 * @param[out] device_type_count_ptr the pointer of device type version array length.
 *
 * @return device type version array on success.
 * @return NULL when the endpoint or the device_type_count_ptr is NULL.
 */
uint8_t *get_device_type_versions(endpoint_t *endpoint, uint8_t *device_type_count_ptr);

/** Set parent endpoint
 *
 * Set the parent endpoint. This is useful in correctly setting the parts_list attribute for the parent, when the
 * parent is a composite endpoint.
 *
 * @param[in] endpoint Endpoint handle.
 * @param[out] parent_endpoint Parent endpoint handle.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_parent_endpoint(endpoint_t *endpoint, endpoint_t *parent_endpoint);

/** Get private data
 *
 * Get the private data passed while creating the endpoint.
 *
 * @param[in] endpoint_id Endpoint ID of the endpoint.
 *
 * @return Private data on success.
 * @return NULL in case of failure.
 */
void *get_priv_data(uint16_t endpoint_id);

/** Set private data
 *
 * Set the private data after creating the endpoint.
 *
 * @param[in] endpoint_id Endpoint ID of the endpoint.
 * @param[in] priv_data Private data of the endpoint.
 *
 * @return ESP_OK on success.
 * @return ESP_ERR_INVALID_STATE or ESP_ERR_NOT_FOUND in case of failure.
 */
esp_err_t set_priv_data(uint16_t endpoint_id, void *priv_data);

/** Set identify
 *
 * Set identify to the endpoint. The identify pointer should be dynamically allocated using 'chip::Platform::New<Identify>()',
 * and once Matter stack is done using it, it will be freed by 'chip::Platform::Delete()'.
 *
 * @param[in] endpoint_id Endpoint id.
 * @param[in] identify Identify pointer.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_identify(uint16_t endpoint_id, void *identify);

/** Enable endpoint
 *
 * Enable the endpoint which has been previously created.
 *
 * @note: This API only needs to be called for endpoints created after calling esp_matter::start(). It should be
 * called after all the clusters, attributes and commands have been added to the created endpoint.
 *
 * @param[in] endpoint Endpoint handle.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t enable(endpoint_t *endpoint);

} /* endpoint */

namespace cluster {

/** Cluster plugin server init callback
 *
 * This callback will be called when the endpoints are initialised. This should be set to upstream's
 * `Matter<cluster_name>PluginServerInitCallback` API, if it exists.
 */
typedef void (*plugin_server_init_callback_t)();

/** Cluster delegate server init callback
 *
 * This callback will be called when the endpoints are initialised.
 */
typedef void (*delegate_init_callback_t)(void *ptr, uint16_t endpoint_id);

/** Cluster add bounds callback
 *
 * This callback will be called when the endpoints are initialised.
 */
typedef void (*add_bounds_callback_t)(cluster_t *cluster);

/** Generic function
 *
 * This can be used to add additional functions based on `cluster_flags_t`.
 */
typedef void (*function_generic_t)();

/** Create cluster
 *
 * This will create a new cluster and add it to the endpoint.
 *
 * @param[in] endpoint Endpoint handle.
 * @param[in] cluster_id Cluster ID for the cluster.
 * @param[in] flags Bitmap of `cluster_flags_t`.
 *
 * @return Cluster handle on success.
 * @return NULL in case of failure.
 */
cluster_t *create(endpoint_t *endpoint, uint32_t cluster_id, uint8_t flags);

/** Get cluster
 *
 * Get the cluster present on the endpoint.
 *
 * @param[in] endpoint Endpoint handle.
 * @param[in] cluster_id Cluster ID for the cluster.
 *
 * @return Cluster handle on success.
 * @return NULL in case of failure.
 */
cluster_t *get(endpoint_t *endpoint, uint32_t cluster_id);

/** Get cluster
 *
 * Get the cluster present on the endpoint.
 *
 * @param[in] endpoint_id Endpoint id.
 * @param[in] cluster_id Cluster ID for the cluster.
 *
 * @return Cluster handle on success.
 * @return NULL in case of failure.
 */
cluster_t *get(uint16_t endpoint_id, uint32_t cluster_id);

/** Get first cluster
 *
 * Get the first cluster present on the endpoint.
 *
 * @param[in] endpoint Endpoint handle.
 *
 * @return Cluster handle on success.
 * @return NULL in case of failure.
 */
cluster_t *get_first(endpoint_t *endpoint);

/** Get next cluster
 *
 * Get the next cluster present on the endpoint.
 *
 * @param[in] cluster Cluster handle.
 *
 * @return Cluster handle on success.
 * @return NULL in case of failure.
 */
cluster_t *get_next(cluster_t *cluster);

/** Get cluster ID
 *
 * Get the cluster ID for the cluster.
 *
 * @param[in] cluster Cluster handle.
 *
 * @return Cluster ID on success.
 * @return Invalid CLuster ID (0xFFFF'FFFF) in case of failure.
 */
uint32_t get_id(cluster_t *cluster);

/** Get delegate pointer
 *
 * Get the delegate pointer for the cluster.
 *
 * @param[in] cluster Cluster handle.
 *
 * @return pointer of delegate class on success.
 * @return nullptr in case of failure.
 */
void *get_delegate_impl(cluster_t *cluster);

/** Set cluster plugin server init callback
 *
 * Set the cluster plugin server init callback. This callback will be called when the endpoints are initialised. The
 * callback should be set to upstream's `Matter<cluster_name>PluginServerInitCallback` API for the cluster, if it
 * exists.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] callback Plugin server init callback.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_plugin_server_init_callback(cluster_t *cluster, plugin_server_init_callback_t callback);

/** Set server cluster delegate init callback
 *
 * @param[in] cluster Cluster handle.
 * @param[in] callback Delegate server init callback.
 * @param[in] delegate Pointer to delegate impl..
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_delegate_and_init_callback(cluster_t *cluster, delegate_init_callback_t callback, void *delegate);

/** Set server cluster add bounds callback
 *
 * @param[in] cluster Cluster handle.
 * @param[in] callback Add bounds callback.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_add_bounds_callback(cluster_t *cluster, add_bounds_callback_t callback);

/** Get cluster plugin server init callback
 *
 * Get the cluster plugin server init callback which has previously been set.
 *
 * @param[in] cluster Cluster handle.
 *
 * @return Plugin server init callback.
 * @return NULL in case of failure or if it has not been set.
 */
plugin_server_init_callback_t get_plugin_server_init_callback(cluster_t *cluster);

/** Get server cluster delegate init callback
 *
 * @param[in] cluster Cluster handle.
 *
 * @return Delegate init callback.
 * @return NULL in case of failure or if it has not been set.
 */
delegate_init_callback_t get_delegate_init_callback(cluster_t *cluster);

/** Get server cluster add bounds callback
 *
 * @param[in] cluster Cluster handle.
 *
 * @return add bounds callback.
 * @return NULL in case of failure or if it has not been set.
 */
add_bounds_callback_t get_add_bounds_callback(cluster_t *cluster);

/** Add cluster function list
 *
 * This API can be used to add additional cluster functions based on `cluster_flags_t`. This should be set
 * to upstream's `Matter<cluster_name>Server<function_type>Callback` or
 * `emberAf<cluster_name>Server<function_type>Callback` API, if it exists. The corresponding function_flags must be be
 * set.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] function_list Array of function_generic_t.
 * @param[in] function_flags Bitmap of cluster flags corresponding to the function_list.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t add_function_list(cluster_t *cluster, const function_generic_t *function_list, int function_flags);

} /* cluster */

namespace attribute {

/** Create attribute
 *
 * This will create a new attribute and add it to the cluster.
 *
 * @param[in] cluster       Cluster handle.
 * @param[in] attribute_id  Attribute ID for the attribute.
 * @param[in] flags         Bitmap of `attribute_flags_t`.
 * @param[in] val           Default type and value of the attribute. Use appropriate elements as per the value type.
 * @param[in] max_val_size  For attributes of type char string and long char string, the size should correspond to the
 *                          maximum size defined in the specification. However, for other types of attributes, this
 *                          parameter remains unused, and therefore the default value is set to 0
 *
 * @return Attribute handle on success.
 * @return NULL in case of failure.
 */
attribute_t *create(cluster_t *cluster, uint32_t attribute_id, uint16_t flags, esp_matter_attr_val_t val,
                    uint16_t max_val_size = 0);

/** Get attribute
 *
 * Get the attribute present on the cluster.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] attribute_id Attribute ID for the attribute.
 *
 * @return Attribute handle on success.
 * @return NULL in case of failure.
 */
attribute_t *get(cluster_t *cluster, uint32_t attribute_id);

/** Get attribute
 *
 * Get the attribute present on the cluster.
 *
 * @param[in] endpoint_id Endpoint id..
 * @param[in] cluster_id Cluster ID for the Cluster.
 * @param[in] attribute_id Attribute ID for the attribute.
 *
 * @return Attribute handle on success.
 * @return NULL in case of failure.
 */
attribute_t *get(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id);


/** Get first attribute
 *
 * Get the first attribute present on the cluster.
 *
 * @param[in] cluster Cluster handle.
 *
 * @return Attribute handle on success.
 * @return NULL in case of failure.
 */
attribute_t *get_first(cluster_t *cluster);

/** Get next attribute
 *
 * Get the next attribute present on the cluster.
 *
 * @param[in] attribute Attribute handle.
 *
 * @return Attribute handle on success.
 * @return NULL in case of failure.
 */
attribute_t *get_next(attribute_t *attribute);

/** Get attribute ID
 *
 * Get the attribute ID for the attribute.
 *
 * @param[in] attribute Attribute handle.
 *
 * @return Attribute ID on success.
 * @return Invalid Attribute ID (0xFFFF'FFFF) in case of failure.
 */
uint32_t get_id(attribute_t *attribute);

/** Set attribute val
 *
 * Set/Update the value of the attribute (has `ATTRIBUTE_FLAG_EXTERNAL_STORAGE` flag) in the database.
 *
 * @note: Once `esp_matter::start()` is done, `attribute::update()` should be used to update the attribute value.
 *
 * @param[in] attribute Attribute handle.
 * @param[in] val Pointer to `esp_matter_attr_val_t`. Use appropriate elements as per the value type.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_val(attribute_t *attribute, esp_matter_attr_val_t *val);

/** Get attribute val
 *
 * Get the value of the attribute (has `ATTRIBUTE_FLAG_EXTERNAL_STORAGE` flag) from the database.
 *
 * @param[in] attribute Attribute handle.
 * @param[out] val Pointer to `esp_matter_attr_val_t`. Use appropriate elements as per the value type.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t get_val(attribute_t *attribute, esp_matter_attr_val_t *val);

/** Get attribute val raw
 *
 * Get the value of the attribute in the database, without the attribute handle.
 *
 * @param[in] endpoint_id Endpoint ID of the attribute.
 * @param[in] cluster_id Cluster ID of the attribute.
 * @param[in] attribute_id Attribute ID of the attribute.
 * @param[out] value Pointer to an allocated buffer for the attribute value.
 * @param[in] attribute_size Size of the allocated buffer.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t get_val_raw(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, uint8_t *value,
                      uint16_t attribute_size);

/** Add attribute bounds
 *
 * Add bounds to the attribute (has `ATTRIBUTE_FLAG_EXTERNAL_STORAGE` flag). Bounds cannot be added to string/array type attributes.
 *
 * @param[in] attribute Attribute handle.
 * @param[in] min Minimum allowed value.
 * @param[in] max Maximum allowed value.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t add_bounds(attribute_t *attribute, esp_matter_attr_val_t min, esp_matter_attr_val_t max);

/** Get attribute bounds
 *
 * Get the bounds which have been added to the attribute (has `ATTRIBUTE_FLAG_EXTERNAL_STORAGE` flag).
 *
 * @param[in] attribute Attribute handle.
 * @param[in] bounds Pointer to `esp_matter_attr_bounds_t`.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t get_bounds(attribute_t *attribute, esp_matter_attr_bounds_t *bounds);

/** Get attribute flags
 *
 * Get the attribute flags for the attribute.
 *
 * @param[in] attribute Attribute handle.
 *
 * @return Attribute flags.
 */
uint16_t get_flags(attribute_t *attribute);

/** Set attribute override
 *
 * Set the override callback for the attribute (has `ATTRIBUTE_FLAG_EXTERNAL_STORAGE` flag). For attribute read and write calls, instead of doing that from the
 * common database, this callback will be called.
 *
 * This can be used if the application or some component wants to maintain the attribute's value in the application or
 * in that component respectively. It can also be used if the attribute value needs to be dynamically fetched and is
 * difficult to maintain in the database.
 *
 * @param[in] attribute Attribute handle.
 * @param[in] callback Override callback.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_override_callback(attribute_t *attribute, callback_t callback);

/** Get attribute override
 *
 * Get the override callback for the attribute (has `ATTRIBUTE_FLAG_EXTERNAL_STORAGE` flag).
 *
 * @param[in] attribute Attribute handle.
 *
 * @return Attribute override callback.
 */
callback_t get_override_callback(attribute_t *attribute);

/** Set attribute (has `ATTRIBUTE_FLAG_EXTERNAL_STORAGE` flag) deferred persistence
 *
 * Only non-volatile attributes can be set with deferred presistence. If an attribute is configured with deferred
 * presistence, any modifications to it will be enacted in its persistent storage with a specific delay
 * (CONFIG_ESP_MATTER_DEFERRED_ATTR_PERSISTENCE_TIME_MS)
 *
 * It could be used for the non-volatile attribues which might be changed rapidly, such as CurrentLevel in LevelControl
 * cluster.
 *
 * @param[in] attribute Attribute handle.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_deferred_persistence(attribute_t *attribute);

} /* attribute */

namespace command {

/** Command callback
 *
 * Command callback which is called when the command is invoked.
 *
 * @note: If the `COMMAND_FLAG_CUSTOM` is set, the default command response is sent internally based on the
 * return value from the callback.
 *
 * @param[in] command_path Common structure for endpoint, cluster and commands IDs.
 * @param[in] tlv_data Command data in TLV format.
 * @param[in] opaque_ptr This is a pointer which is used internally.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
typedef esp_err_t (*callback_t)(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr);

/** Create command
 *
 * This will create a new command and add it to the cluster.
 *
 * @note: For commands which are not defined in the spec the `COMMAND_FLAG_CUSTOM` flag must be set. This
 * will send the command response internally, after the command callback is called.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] command_id Command ID for the command.
 * @param[in] flags Bitmap of `command_flags_t`.
 * @param[in] callback Command callback
 *
 * @return Command handle on success.
 * @return NULL in case of failure.
 */
command_t *create(cluster_t *cluster, uint32_t command_id, uint8_t flags, callback_t callback);

/** Get command
 *
 * Get the command present on the cluster.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] command_id Command ID for the command.
 * @param[in] flags Command flags for the command to be fetched.
 *
 * @return Command handle on success.
 * @return NULL in case of failure.
 */
command_t *get(cluster_t *cluster, uint32_t command_id, uint16_t flags);

/** Get first command
 *
 * Get the first command present on the cluster.
 *
 * @param[in] cluster Cluster handle.
 *
 * @return Command handle on success.
 * @return NULL in case of failure.
 */
command_t *get_first(cluster_t *cluster);

/** Get next command
 *
 * Get the next command present on the cluster.
 *
 * @param[in] command Command handle.
 *
 * @return Command handle on success.
 * @return NULL in case of failure.
 */
command_t *get_next(command_t *command);

/** Get command ID
 *
 * Get the command ID for the command.
 *
 * @param[in] command Command handle.
 *
 * @return Command ID on success.
 * @return Invalid Command ID (0xFFFF'FFFF) in case of failure.
 */
uint32_t get_id(command_t *command);

/** Get command callback
 *
 * Get the command callback for the command.
 *
 * @param[in] command Command handle.
 *
 * @return Command callback on success.
 * @return NULL in case of failure or if the callback was not set when creating the command.
 */
callback_t get_callback(command_t *command);

/** Get command user_callback
 *
 * Get the command user_callback for the command.
 *
 * @param[in] command Command handle.
 *
 * @return Command user_callback on success.
 * @return NULL in case of failure or if the callback was not set when creating the command.
 */
callback_t get_user_callback(command_t *command);

/** Set command user_callback
 *
 * Set the user_callback for the command.
 *
 * @param[in] command Command handle.
 * @param[in] user_callback callback_t.
 *
 * @return void
 */
void set_user_callback(command_t *command, callback_t user_callback);

/** Get command flags
 *
 * Get the command flags for the command.
 *
 * @param[in] command Command handle.
 *
 * @return Command flags.
 */
uint16_t get_flags(command_t *command);

} /* command */

namespace event {

/** Create event
 *
 * This will create a new event and add it to the cluster.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] event_id Event ID for the event.
 *
 * @return Event handle on success.
 * @return NULL in case of failure.
 */
event_t *create(cluster_t *cluster, uint32_t event_id);

/** Get event
 *
 * Get the event present on the cluster.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] event_id Event ID for the command.
 *
 * @return Event handle on success.
 * @return NULL in case of failure.
 */
event_t *get(cluster_t *cluster, uint32_t event_id);

/** Get first event
 *
 * Get the first event present on the cluster.
 *
 * @param[in] cluster Cluster handle.
 *
 * @return Event handle on success.
 * @return NULL in case of failure.
 */
event_t *get_first(cluster_t *cluster);

/** Get next event
 *
 * Get the next event present on the cluster.
 *
 * @param[in] event Event handle.
 *
 * @return Event handle on success.
 * @return NULL in case of failure.
 */
event_t *get_next(event_t *event);

/** Get event ID
 *
 * Get the event ID for the event.
 *
 * @param[in] event Event handle.
 *
 * @return Event ID on success.
 * @return Invalid Event ID (0xFFFF'FFFF) in case of failure.
 */
uint32_t get_id(event_t *event);

} /* event */

/* Client APIs */
namespace client {

/** Client request types
 */
typedef enum {
    INVOKE_CMD = 0,
    READ_ATTR = 1,
    READ_EVENT = 2,
    WRITE_ATTR = 3,
    SUBSCRIBE_ATTR = 4,
    SUBSCRIBE_EVENT = 5,
} request_type_t;

/** Request handle as the input when calling `connect()` or `cluster_update()`
 *
 */

typedef struct request_handle {
    request_type_t type;
    union {
        chip::app::AttributePathParams attribute_path;
        chip::app::EventPathParams event_path;
        chip::app::CommandPathParams command_path;
    };
    /* This could be the command data field when the request type is INVOKE_CMD,
     * or the attribute value data when the request type is WRITE_ATTR.
     */
    void *request_data;
    request_handle() : type(INVOKE_CMD), request_data(NULL) {}
    request_handle(struct request_handle* req) : type(req->type), request_data(req->request_data)
    {
        if (req->type == INVOKE_CMD) {
            command_path = req->command_path;
        } else if (req->type == WRITE_ATTR || req->type == READ_ATTR || req->type == SUBSCRIBE_ATTR) {
            attribute_path = req->attribute_path;
        } else if (req->type == READ_EVENT || req->type == SUBSCRIBE_EVENT) {
            event_path = req->event_path;
        }
    }
} request_handle_t;

/** Peer device handle */
typedef chip::DeviceProxy peer_device_t;

/** CASE Session Manager */
typedef chip::CASESessionManager case_session_mgr_t;

/** Request send callback
 *
 * This callback will be called when `connect()` or `cluster_update()` is called and the connection completes. The
 * send_request APIs can then be called from the callback.
 *
 * @param[in] peer_device Peer device handle. This can be passed to the send_command APIs.
 * @param[in] req_handle Request handle used by `connect()` or `cluster_update()`.
 * @param[in] priv_data (Optional) Private data associated with the callback. This will be passed to callback. It
 * should stay allocated throughout the lifetime of the device.
 */
typedef void (*request_callback_t)(peer_device_t *peer_device, request_handle_t *req_handle, void *priv_data);

/** Group request send callback
 *
 * This callback will be called when `cluster_update()` is called and the group request is triggered for binding cluster.
 *
 * @note: The request type should be INVOKE_CMD and the command should not expect a response.
 *
 * @param[in] fabric_index The index of the fabric that the group command is sent to.
 * @param[in] req_handle Request handle used by `cluster_update()`.
 * @param[in] priv_data (Optional) Private data associated with the callback. This will be passed to callback. It
 * should stay allocated throughout the lifetime of the device.
 */
typedef void (*group_request_callback_t)(uint8_t fabric_index, request_handle_t *req_handle, void *priv_data);

/** Initialize binding
 *
 * This should be called if the Binding cluster has been created. It just sets a flag for the binding manager to be
 * initialized.
 * If the cluster::binding::create() is being used, this is called internally.
 */
void binding_init();

/** Initialize binding manager
 *
 * This initializes the binding manager. It is called after the matter thread has been started.
 */
void binding_manager_init();

/** Connect
 *
 * Connect to another device on the same fabric to send a request.
 *
 * @param[in] case_session_mgr CASE Session Manager to find or establish the session
 * @param[in] fabric_index Fabric index.
 * @param[in] node_id Node ID of the other device.
 * @param[in] req_handle Request to be sent to the remote device.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t connect(case_session_mgr_t *case_session_mgr, uint8_t fabric_index, uint64_t node_id,
                  request_handle_t *req_handle);

/** group_request_send
 *
 * on the same fabric to send a group request.
 *
 * @param[in] fabric_index Fabric index.
 * @param[in] req_handle Request to be sent to the group.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t group_request_send(uint8_t fabric_index, request_handle_t *req_handle);

/** Set request send callback
 *
 * Set the common request send callback and the group request send callback. The common callback will be called
 * when `connect()` or `cluster_update()` is called and the connection completes. The group callback will be called
 * when `cluster_update()` is called and the group request is triggered.
 *
 * @param[in] callback request send callback.
 * @param[in] g_callback group request send callback
 * @param[in] priv_data (Optional) Private data associated with the callback. This will be passed to callback. It
 * should stay allocated throughout the lifetime of the device.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_request_callback(request_callback_t callback, group_request_callback_t g_callback, void *priv_data);

/** Cluster update
 *
 * For an already binded device, this API can be used to get the request send callback, and the send_request APIs can
 * then be called from the callback.
 *
 * @param[in] local_endpoint_id The ID of the local endpoint with a binding cluster.
 * @param[in] req_handle Request information to notify the bound cluster changed.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t cluster_update(uint16_t local_endpoint_id, request_handle_t *req_handle);

} /* client */
} /* esp_matter */
