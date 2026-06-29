// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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
#include <esp_matter_data_model.h>
#include <esp_matter_macros.h>
#include <esp_matter_identify.h>
#include <root_node_device.h>
// this space is for feature validation functions

enum class feature_policy {
    k_exact_one = 0,        // O.a
    k_at_least_one = 1,     // 0.a+
};

bool validate_features(uint32_t feature_flag, feature_policy policy, const char *feature_name,
                       std::initializer_list<uint32_t> features);

namespace esp_matter {

namespace node {
/** Standard node create
 *
 * This creates the node, sets the attribute and the identification callbacks and also adds the root node device type, which
 * is by default added to endpoint 0 (since this is the first endpoint which is created).
 */

typedef struct config {
    esp_matter::endpoint::root_node::config_t root_node;
} config_t;

/**
 * @param[in] config             Configuration of the root node, a pointer to an object of type `node::config_t`.
 * @param[in] attribute_callback This callback is called for every attribute update. The callback implementation shall
 *                               handle the desired attributes and return an appropriate error code. If the attribute
 *                               is not of your interest, please do not return an error code and strictly return ESP_OK.
 * @param[in] identify_callback  This callback is invoked when clients interact with the Identify Cluster.
 *                               In the callback implementation, an endpoint can identify itself.
 *                               (e.g., by flashing an LED or light).
 * @param[in] priv_data          Private data to send to the node. This parameter is optional
 *                               and defaults to nullptr.This private data can be accessed in the attribute callback
 *                               for the root endpoint only.
 */
node_t *create(config_t *config, attribute::callback_t attribute_callback,
               identification::callback_t identify_callback, void* priv_data = nullptr);

} /* node */

namespace cluster {
namespace global {
namespace attribute {
attribute_t *create_cluster_revision(cluster_t *cluster, uint16_t value);
attribute_t *create_feature_map(cluster_t *cluster, uint32_t value);
} // namespace attribute
} // namespace global

/** Update feature map
 *
 * Update the feature map for the cluster.
 *
 * @param[in] cluster Cluster handle.
 * @param[in] value Feature map value.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t update_feature_map(cluster_t *cluster, uint32_t value);

/** Get feature map value
 *
 * Get the feature map value for the cluster.
 *
 * @param[in] cluster Cluster handle.
 *
 * @return Feature map value on success.
 * @return 0 in case of failure.
 */
uint32_t get_feature_map_value(cluster_t *cluster);

/** Abort cluster creation
 * This is primarily used in cluster::create() and if somehow we failed to create cluster then it cleans up the cluster by calling cluster::destroy() and asserts.
 *
 * @param[in] cluster Cluster handle.
 *
 * @return Cluster handle on success.
 * @return NULL in case of failure.
 */
cluster_t *ABORT_CLUSTER_CREATE(cluster_t *cluster);

/** Create the default binding cluster on an endpoint
 *
 * Application clusters with a client role get a binding cluster created alongside them. This adds
 * the binding cluster once per endpoint, doing nothing if one is already present.
 *
 * @param[in] endpoint Endpoint handle.
 */
void create_default_binding_cluster(endpoint_t *endpoint);

} // namespace cluster
} // namespace esp_matter
