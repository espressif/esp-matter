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
#if CHIP_CONFIG_ENABLE_ICD_SERVER
#include <esp_matter_icd_configuration.h>
#endif // CHIP_CONFIG_ENABLE_ICD_SERVER
#include <root_node_device.h>

using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::endpoint;

static const char *TAG = "esp_matter_endpoint";

namespace esp_matter {
namespace endpoint {
namespace root_node {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ROOT_NODE_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_ROOT_NODE_DEVICE_TYPE_VERSION;
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

    cluster::access_control::create(endpoint, &(config->access_control), CLUSTER_FLAG_SERVER);
    cluster::basic_information::create(endpoint, &(config->basic_information), CLUSTER_FLAG_SERVER);
    cluster::general_commissioning::create(endpoint, &(config->general_commissioning), CLUSTER_FLAG_SERVER);
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
    cluster::general_diagnostics::create(endpoint, &(config->general_diagnostics), CLUSTER_FLAG_SERVER);
    cluster::administrator_commissioning::create(endpoint, &(config->administrator_commissioning), CLUSTER_FLAG_SERVER);
    cluster::operational_credentials::create(endpoint, &(config->operational_credentials), CLUSTER_FLAG_SERVER);
    cluster::group_key_management::create(endpoint, &(config->group_key_management), CLUSTER_FLAG_SERVER);
#if CHIP_CONFIG_ENABLE_ICD_SERVER
    if (icd::get_icd_server_enabled()) {
        cluster_t *icd_management = cluster::icd_management::create(endpoint, &(config->icd_management), CLUSTER_FLAG_SERVER);
        VerifyOrReturnValue(icd_management != NULL, ESP_FAIL, ESP_LOGE(TAG, "Failed to create cluster: icd_management. device_type_id: 0x%08" PRIX32, get_device_type_id()));
#if CHIP_CONFIG_ENABLE_ICD_LIT
        cluster::icd_management::feature::long_idle_time_support::add(icd_management, &(config->icd_management_long_idle_time_support));
#endif // CHIP_CONFIG_ENABLE_ICD_LIT
#if CHIP_CONFIG_ENABLE_ICD_CIP
        cluster::icd_management::feature::check_in_protocol_support::add(icd_management, &(config->icd_management_check_in_protocol_support));
#endif // CHIP_CONFIG_ENABLE_ICD_CIP
#if CHIP_CONFIG_ENABLE_ICD_UAT
        cluster::icd_management::feature::user_active_mode_trigger::add(icd_management, &(config->icd_management_user_active_mode_trigger));
#endif // CHIP_CONFIG_ENABLE_ICD_UAT
    }
#endif // CHIP_CONFIG_ENABLE_ICD_SERVER
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI && defined(CONFIG_SUPPORT_WIFI_NETWORK_DIAGNOSTICS_CLUSTER)
    cluster::wi_fi_network_diagnostics::create(endpoint, &(config->wi_fi_network_diagnostics), CLUSTER_FLAG_SERVER);
#endif // CHIP_DEVICE_CONFIG_ENABLE_WIFI && defined(CONFIG_SUPPORT_WIFI_NETWORK_DIAGNOSTICS_CLUSTER)
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD && defined(CONFIG_SUPPORT_THREAD_NETWORK_DIAGNOSTICS_CLUSTER)
    cluster::thread_network_diagnostics::create(endpoint, &(config->thread_network_diagnostics), CLUSTER_FLAG_SERVER);
#endif // CHIP_DEVICE_CONFIG_ENABLE_THREAD && defined(CONFIG_SUPPORT_THREAD_NETWORK_DIAGNOSTICS_CLUSTER)
#if defined(CONFIG_SUPPORT_GROUPCAST_CLUSTER)
    cluster::groupcast::create(endpoint, &(config->groupcast), CLUSTER_FLAG_SERVER);
#endif // defined(CONFIG_SUPPORT_GROUPCAST_CLUSTER)
    return ESP_OK;
}

} /* root_node */
} /* endpoint */
} /* esp_matter */
