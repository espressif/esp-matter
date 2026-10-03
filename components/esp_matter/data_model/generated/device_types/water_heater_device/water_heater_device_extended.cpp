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
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <water_heater_device.h>
#include <water_heater_device_extended.h>

using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::endpoint;

static const char *TAG = "esp_matter_water_heater_ext";

namespace esp_matter {
namespace endpoint {
namespace water_heater {

endpoint_t *create_extended(node_t *node, extended_config_t *config, uint8_t flags, void *priv_data)
{
    // Create base endpoint and clusters using the standard generator
    endpoint_t *endpoint = create(node, &(config->base), flags, priv_data);
    if (endpoint == nullptr) {
        ESP_LOGE(TAG, "Failed to create base water heater endpoint");
        return NULL;
    }

    // Get the water heater management cluster
    cluster_t *mgmt_cluster = cluster::get(endpoint, cluster::water_heater_management::get_id());
    if (mgmt_cluster == nullptr) {
        ESP_LOGE(TAG, "Failed to find water heater management cluster");
        return endpoint;
    }

    // Add optional features if configured
    if (config->energy_management != nullptr) {
        if (cluster::water_heater_management::feature::energy_management::add(mgmt_cluster, config->energy_management) != ESP_OK) {
            ESP_LOGW(TAG, "Failed to add energy_management feature");
        }
    }

    if (config->tank_percent != nullptr) {
        if (cluster::water_heater_management::feature::tank_percent::add(mgmt_cluster, config->tank_percent) != ESP_OK) {
            ESP_LOGW(TAG, "Failed to add tank_percent feature");
        }
    }

    return endpoint;
}

} /* water_heater */
} /* endpoint */
} /* esp_matter */
