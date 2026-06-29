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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <pump_device.h>

using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::endpoint;

static const char *TAG = "esp_matter_endpoint";

namespace esp_matter {
namespace endpoint {
namespace pump {
uint32_t get_device_type_id()
{
    return ESP_MATTER_PUMP_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_PUMP_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = esp_matter::endpoint::create(node, flags, priv_data);
    VerifyOrReturnValue(endpoint != nullptr, NULL, ESP_LOGE(TAG, "Failed to create endpoint. device_type_id: 0x%08" PRIX32, get_device_type_id()));

    cluster_t *descriptor_cluster = descriptor::create(endpoint, &(config->descriptor), CLUSTER_FLAG_SERVER);
    VerifyOrReturnValue(descriptor_cluster != nullptr, NULL, ESP_LOGE(TAG, "Failed to create descriptor cluster. device_type_id: 0x%08" PRIX32, get_device_type_id()));

    VerifyOrReturnValue(add(endpoint, config) == ESP_OK, NULL, ESP_LOGE(TAG, "Failed to add device type. device_type_id: 0x%08" PRIX32, get_device_type_id()));
    return endpoint;
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster::identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    cluster::on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER);
    cluster::pump_configuration_and_control::create(endpoint, &(config->pump_configuration_and_control), CLUSTER_FLAG_SERVER);
    return ESP_OK;
}

} /* pump */
} /* endpoint */
} /* esp_matter */
