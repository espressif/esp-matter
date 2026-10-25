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

#include <esp_log.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model_utils.h>
#include <binding.h>

static const char *TAG = "data_model";

namespace esp_matter {
namespace node {

node_t *create(config_t *config, attribute::callback_t attribute_callback,
               identification::callback_t identification_callback, void* priv_data)
{
    node_t *node = create_raw();
    /* Initialize esp-matter nvs partition */
    VerifyOrReturnValue(esp_matter_nvs_init() == ESP_OK, NULL, ESP_LOGE(TAG, "Failed to init esp-matter nvs partition"));
    VerifyOrReturnValue(node != nullptr, NULL, ESP_LOGE(TAG, "Could not create node"));
    endpoint_t *endpoint = esp_matter::endpoint::root_node::create(node, &(config->root_node), ENDPOINT_FLAG_NONE, priv_data);
    if (endpoint == nullptr) {
        destroy_raw();
        return NULL;
    }
    attribute::set_callback(attribute_callback);
    identification::set_callback(identification_callback);
    return node;
}

} /* node */

namespace cluster {
namespace global {
namespace attribute {
attribute_t *create_cluster_revision(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, chip::app::Clusters::Globals::Attributes::ClusterRevision::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_feature_map(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, chip::app::Clusters::Globals::Attributes::FeatureMap::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
}
} // namespace attribute
} // namespace global

cluster_t *create_default_binding_cluster(endpoint_t *endpoint)
{
    /* Don't create binding cluster if it already exists on the endpoint */
    cluster_t *cluster = get(endpoint, chip::app::Clusters::Binding::Id);
    VerifyOrReturnValue(!cluster, cluster);
    ESP_LOGI(TAG, "Creating default binding cluster");
    binding::config_t config;
    return binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

} // namespace cluster
} // namespace esp_matter
