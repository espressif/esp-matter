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

#include <initializer_list>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter_attribute_utils.h>
#include <app/util/attribute-storage.h>
#include <app/ConcreteCommandPath.h>

#include <esp_matter_data_model.h>

#define CALL_ONCE(cb)                           \
    [](){                                       \
        static bool is_called = false;          \
        if (!is_called) {                       \
            cb();                               \
            is_called = true;                   \
        }                                       \
    }

/** Feature conformance policy used when validating cluster feature combinations. */
enum class feature_policy {
    k_exact_one = 0,        // O.a
    k_at_least_one = 1,     // 0.a+
};

/** Validate that the enabled features satisfy the given conformance policy.
 *
 * @param[in] feature_flag Feature map of the enabled features.
 * @param[in] policy Conformance policy the features must satisfy.
 * @param[in] feature_name Human-readable feature list, used only for the error log.
 * @param[in] features Feature ids to check against the policy.
 *
 * @return true if the enabled features satisfy the policy.
 * @return false otherwise.
 */
bool validate_features(uint32_t feature_flag, feature_policy policy, const char *feature_name,
                       std::initializer_list<uint32_t> features);

// Macros to reduce repetitive validation code. They expand to validate_features()/ABORT_CLUSTER_CREATE()
// at the cluster ::create() site, where `config` and `cluster` are in scope.
#define VALIDATE_FEATURES_EXACT_ONE(name, ...) \
    do { if (!validate_features(config->feature_flags, feature_policy::k_exact_one, name, {__VA_ARGS__})) \
        return ABORT_CLUSTER_CREATE(cluster); } while(0)

#define VALIDATE_FEATURES_AT_LEAST_ONE(name, ...) \
    do { if (!validate_features(config->feature_flags, feature_policy::k_at_least_one, name, {__VA_ARGS__})) \
        return ABORT_CLUSTER_CREATE(cluster); } while(0)

// O.a+ cluster validation: at least one optional cluster must be enabled
// Used by device types with optional choice clusters (e.g., electrical_sensor, cook_surface)
namespace esp_matter {
namespace endpoint {
namespace validation {

inline bool validate_optional_clusters(uint32_t optional_clusters_mask, const char *device_type_name,
                                       std::initializer_list<uint32_t> cluster_masks)
{
    for (uint32_t mask : cluster_masks) {
        if (optional_clusters_mask & mask) {
            return true;
        }
    }
    ESP_LOGE("esp_matter_endpoint", "%s: At least one O.a+ cluster must be enabled. Use with_*() builder methods.",
             device_type_name);
    return false;
}

} // namespace validation
} // namespace endpoint
} // namespace esp_matter

// Macro for O.a+ cluster validation - returns ESP_ERR_INVALID_ARG if validation fails
#define VALIDATE_OPTIONAL_CLUSTERS_AT_LEAST_ONE(device_name, mask, ...) \
    do { if (!esp_matter::endpoint::validation::validate_optional_clusters(mask, device_name, {__VA_ARGS__})) \
        return ESP_ERR_INVALID_ARG; } while(0)

namespace esp_matter {
namespace command {
void dispatch_single_cluster_command(const chip::app::ConcreteCommandPath &command_path, chip::TLV::TLVReader &tlv_data,
                                     void *opaque_ptr);
} // command

namespace cluster {

/** Update the feature map
 *
 * Merges `value` into the cluster's FeatureMap attribute. Used while the data model is being
 * constructed (before the chip stack has started), so it writes the value directly via the
 * internal accessor without invoking application callbacks.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] value Feature map bits to set.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t update_feature_map(cluster_t *cluster, uint32_t value);

/** Get the feature map value
 *
 * @param[in] cluster Cluster handle.
 *
 * @return Feature map value on success.
 * @return 0 in case of failure.
 */
uint32_t get_feature_map_value(cluster_t *cluster);

/** Abort cluster creation
 *
 * Used in cluster::create(): if the cluster could not be fully set up, this destroys the cluster
 * by calling cluster::destroy() and asserts.
 *
 * @param[in] cluster Cluster handle.
 *
 * @return NULL.
 */
cluster_t *ABORT_CLUSTER_CREATE(cluster_t *cluster);

} // cluster

namespace node {

esp_err_t store_min_unused_endpoint_id();

esp_err_t read_min_unused_endpoint_id();

} // namespace node
namespace endpoint {

esp_err_t enable_all();

/** Invoke the init callbacks for the clusters on the endpoint
 *
 * @param[in] endpoint Endpoint handle.
 */
void invoke_init_callbacks_internal(endpoint_t *endpoint);
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
