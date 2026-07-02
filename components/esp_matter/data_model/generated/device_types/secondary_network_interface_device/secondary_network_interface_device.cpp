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
#include <secondary_network_interface_device.h>

using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::endpoint;

namespace esp_matter {
namespace endpoint {
namespace secondary_network_interface {
uint32_t get_device_type_id()
{
    return ESP_MATTER_SECONDARY_NETWORK_INTERFACE_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_SECONDARY_NETWORK_INTERFACE_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = esp_matter::endpoint::create(node, flags, priv_data);
    VerifyOrReturnValue(endpoint != nullptr, NULL, ESP_LOGE("secondary_network_interface", "Failed to create endpoint"));

    cluster_t *descriptor_cluster = descriptor::create(endpoint, &(config->descriptor), CLUSTER_FLAG_SERVER);
    VerifyOrReturnValue(descriptor_cluster != nullptr, NULL, ESP_LOGE("secondary_network_interface", "Failed to create descriptor cluster"));

    VerifyOrReturnValue(add(endpoint, config) == ESP_OK, NULL, ESP_LOGE("secondary_network_interface", "Failed to add device type"));
    return endpoint;
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

#if !defined(CONFIG_CUSTOM_NETWORK_CONFIG)
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
    config->network_commissioning.feature_flags |= cluster::network_commissioning::feature::wi_fi_network_interface::get_id();
#elif CHIP_DEVICE_CONFIG_ENABLE_THREAD
    config->network_commissioning.feature_flags |= cluster::network_commissioning::feature::thread_network_interface::get_id();
#else
    config->network_commissioning.feature_flags |= cluster::network_commissioning::feature::ethernet_network_interface::get_id();
#endif
    cluster::network_commissioning::create(endpoint, &(config->network_commissioning), CLUSTER_FLAG_SERVER);
#endif // !defined(CONFIG_CUSTOM_NETWORK_CONFIG)
    return ESP_OK;
}

} /* secondary_network_interface */
} /* endpoint */
} /* esp_matter */
