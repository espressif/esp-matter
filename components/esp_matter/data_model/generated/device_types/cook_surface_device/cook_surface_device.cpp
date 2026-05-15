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

/* This is a Generated File */

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_macros.h>
#include <cook_surface_device.h>

using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::endpoint;

namespace esp_matter {
namespace endpoint {
namespace cook_surface {
uint32_t get_device_type_id()
{
    return ESP_MATTER_COOK_SURFACE_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_COOK_SURFACE_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = esp_matter::endpoint::create(node, flags, priv_data);
    VerifyOrReturnValue(endpoint != nullptr, NULL, ESP_LOGE("cook_surface", "Failed to create endpoint"));

    cluster_t *descriptor_cluster = descriptor::create(endpoint, &(config->descriptor), CLUSTER_FLAG_SERVER);
    VerifyOrReturnValue(descriptor_cluster != nullptr, NULL, ESP_LOGE("cook_surface", "Failed to create descriptor cluster"));

    VerifyOrReturnValue(add(endpoint, config) == ESP_OK, NULL, ESP_LOGE("cook_surface", "Failed to add device type"));
    return endpoint;
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    // Validate O.a+ clusters: at least one must be enabled
    VALIDATE_OPTIONAL_CLUSTERS_AT_LEAST_ONE("cook_surface", config->optional_clusters_mask, COOK_SURFACE_OPTIONAL_CLUSTER_TEMPERATURE_CONTROL, COOK_SURFACE_OPTIONAL_CLUSTER_TEMPERATURE_MEASUREMENT);

    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    if (config->optional_clusters_mask & COOK_SURFACE_OPTIONAL_CLUSTER_TEMPERATURE_CONTROL) {
        cluster::temperature_control::create(endpoint, &(config->temperature_control), CLUSTER_FLAG_SERVER);
    }
    if (config->optional_clusters_mask & COOK_SURFACE_OPTIONAL_CLUSTER_TEMPERATURE_MEASUREMENT) {
        cluster::temperature_measurement::create(endpoint, &(config->temperature_measurement), CLUSTER_FLAG_SERVER);
    }
    return ESP_OK;
}

} /* cook_surface */
} /* endpoint */
} /* esp_matter */
