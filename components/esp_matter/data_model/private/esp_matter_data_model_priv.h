// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_err.h>
#include <esp_matter_attribute_utils.h>
#include <app/util/attribute-storage.h>
#include <app/ConcreteCommandPath.h>

#include <esp_matter_data_model.h>

namespace esp_matter {
namespace command {
void dispatch_single_cluster_command(const chip::app::ConcreteCommandPath &command_path, chip::TLV::TLVReader &tlv_data,
                                     void *opaque_ptr);
} // command

namespace node {

esp_err_t store_min_unused_endpoint_id();

esp_err_t read_min_unused_endpoint_id();

} // namespace node
namespace endpoint {

esp_err_t enable_all();

}

namespace attribute {

/** Get the attribute value from the esp-matter storage
 *
 * @param[in] attribute Attribute handle.
 * @param[out] val Pointer to `esp_matter_attr_val_t`. Use appropriate elements as per the value type.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t get_val_internal(attribute_t *attribute, esp_matter_attr_val_t *val);

/** Execute the attribute update callback
 *
 * This function executes the attribute update callback set via set_callback().
 * This is used internally to notify the application about attribute changes.
 *
 * @param[in] type Callback type (PRE_UPDATE or POST_UPDATE).
 * @param[in] endpoint_id Endpoint ID of the attribute.
 * @param[in] cluster_id Cluster ID of the attribute.
 * @param[in] attribute_id Attribute ID of the attribute.
 * @param[in] val Pointer to the attribute value.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t execute_callback(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                           uint32_t attribute_id, esp_matter_attr_val_t *val);

/** Set the attribute value in the esp-matter storage
 *
 * @param[in] attribute Attribute handle.
 * @param[in] val Pointer to `esp_matter_attr_val_t`. Use appropriate elements as per the value type.
 * @param[in] call_callbacks Whether to call attribute change pre/post callbacks.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_val_internal(attribute_t *attribute, esp_matter_attr_val_t *val, bool call_callbacks = true);

/** Destroy attribute
 *
 * This function destroys an attribute that was created and added to a cluster.
 * It removes the attribute from the cluster's attribute list, frees any
 * allocated memory for the attribute value (e.g., string, array), and deletes
 * the attribute from NVS if it was stored there.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] attribute Attribute handle.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t destroy(cluster_t *cluster, attribute_t *attribute);

} // namespace attribute

namespace command {

    /** Destroy command
 *
 * This function destroys a command that was created and added to a cluster.
 * It removes the command from the cluster's command list.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] command Command handle.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t destroy(cluster_t *cluster, command_t *command);

} // namespace command

namespace event {

    /** Destroy event
 *
 * This function destroys an event that was created and added to a cluster.
 * It removes the event from the cluster's event list.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] event Event handle.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t destroy(cluster_t *cluster, event_t *event);

} // namespace event
} // namespace esp_matter
