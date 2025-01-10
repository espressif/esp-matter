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

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_endpoint.h>

static const char *TAG = "esp_matter_endpoint";

using namespace chip::app::Clusters;
namespace esp_matter {
using namespace cluster;

namespace endpoint {

namespace common {

template <typename T>
using add_fn = esp_err_t (*)(endpoint_t *endpoint, T *config);

template <typename T>
endpoint_t *create(node_t *node, T *config, uint8_t flags, void *priv_data, add_fn<T> add)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    VerifyOrReturnValue(endpoint != nullptr, NULL, ESP_LOGE(TAG, "Failed to create endpoint"));

    cluster_t *descriptor_cluster = descriptor::create(endpoint, &(config->descriptor), CLUSTER_FLAG_SERVER);
    VerifyOrReturnValue(descriptor_cluster != nullptr, NULL, ESP_LOGE(TAG, "Failed to create descriptor cluster"));

    VerifyOrReturnValue(ESP_OK == add(endpoint, config), NULL, ESP_LOGE(TAG, "Failed to add cluster"));
    return endpoint;
}

} // namespace common

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
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    access_control::create(endpoint, &(config->access_control), CLUSTER_FLAG_SERVER);
    basic_information::create(endpoint, &(config->basic_information), CLUSTER_FLAG_SERVER);
    general_commissioning::create(endpoint, &(config->general_commissioning), CLUSTER_FLAG_SERVER);
    network_commissioning::create(endpoint, &(config->network_commissioning), CLUSTER_FLAG_SERVER);
    general_diagnostics::create(endpoint, &(config->general_diagnostics), CLUSTER_FLAG_SERVER);
    administrator_commissioning::create(endpoint, &(config->administrator_commissioning), CLUSTER_FLAG_SERVER,
                                        ESP_MATTER_NONE_FEATURE_ID);
    operational_credentials::create(endpoint, &(config->operational_credentials), CLUSTER_FLAG_SERVER);
    group_key_management::create(endpoint, CLUSTER_FLAG_SERVER);

#if CHIP_CONFIG_ENABLE_ICD_SERVER
    icd_management::create(endpoint, &(config->icd_management), CLUSTER_FLAG_SERVER,
#if CHIP_CONFIG_ENABLE_ICD_LIT
                           icd_management::feature::long_idle_time_support::get_id() |
#if CHIP_CONFIG_ENABLE_ICD_CIP
                           icd_management::feature::check_in_protocol_support::get_id() |
#endif // CHIP_CONFIG_ENABLE_ICD_CIP
#if CHIP_CONFIG_ENABLE_ICD_UAT
                           icd_management::feature::user_active_mode_trigger::get_id() |
#endif // CHIP_CONFIG_ENABLE_ICD_UAT
#endif // CHIP_CONFIG_ENABLE_ICD_LIT
                            0);
#endif // CHIP_CONFIG_ENABLE_ICD_SERVER
    return ESP_OK;
}
} /* root_node */

namespace ota_requestor{
uint32_t get_device_type_id()
{
    return ESP_MATTER_OTA_REQUESTOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_OTA_REQUESTOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
#ifdef CONFIG_ENABLE_OTA_REQUESTOR
    return common::create<config_t>(node, config, flags, priv_data, add);
#else
    ESP_LOGE(TAG, "Need enable CONFIG_ENABLE_OTA_REQUESTOR to enable ota requestor function");
    return nullptr;
#endif
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
#ifdef CONFIG_ENABLE_OTA_REQUESTOR
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster_t *cluster_p = cluster::ota_provider::create(endpoint, NULL, CLUSTER_FLAG_CLIENT);
    cluster_t *cluster_r = cluster::ota_requestor::create(endpoint, &(config->ota_requestor), CLUSTER_FLAG_SERVER);
    if (!cluster_p || !cluster_r) {
        return ESP_FAIL;
    }

    return ESP_OK;
#else
    ESP_LOGE(TAG, "Need enable CONFIG_ENABLE_OTA_REQUESTOR to enable ota requestor function");
    return ESP_FAIL;
#endif
}

} /** ota_requestor **/

namespace ota_provider{
uint32_t get_device_type_id()
{
    return ESP_MATTER_OTA_PROVIDER_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_OTA_PROVIDER_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster_t *cluster = cluster::ota_provider::create(endpoint, &(config->ota_provider), CLUSTER_FLAG_SERVER);
    if (!cluster) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

} /** ota_provider **/

namespace power_source_device{
uint32_t get_device_type_id()
{
    return ESP_MATTER_POWER_SOURCE_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_POWER_SOURCE_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster_t *cluster = power_source::create(endpoint, &(config->power_source), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);
    if (!cluster) {
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

} /** power_source_device **/

namespace on_off_light {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kLightOutput);
    cluster_t *identify_cluster = identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());

    return ESP_OK;
}

} /* on_off_light */

namespace dimmable_light {
uint32_t get_device_type_id()
{
    return ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kLightOutput);
    cluster_t *identify_cluster = identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    level_control::create(endpoint, &(config->level_control), CLUSTER_FLAG_SERVER,
                          level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());

    return ESP_OK;
}
} /* dimmable_light */

namespace color_temperature_light {
uint32_t get_device_type_id()
{
    return ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kLightOutput);
    cluster_t *identify_cluster = identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    level_control::create(endpoint, &(config->level_control), CLUSTER_FLAG_SERVER,
                          level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());
    color_control::create(endpoint, &(config->color_control), CLUSTER_FLAG_SERVER,
                          color_control::feature::color_temperature::get_id());

    return ESP_OK;
}

} /* color_temperature_light */

namespace extended_color_light {
uint32_t get_device_type_id()
{
    return ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kLightOutput);
    cluster_t *identify_cluster = identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    level_control::create(endpoint, &(config->level_control), CLUSTER_FLAG_SERVER,
                          level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());
    color_control::create(endpoint, &(config->color_control), CLUSTER_FLAG_SERVER,
                          color_control::feature::color_temperature::get_id() | color_control::feature::xy::get_id());
    return ESP_OK;

}
} /* extended_color_light */

namespace on_off_switch {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t * endpoint = common::create<config_t>(node, config, flags, priv_data, add);
    VerifyOrReturnError(endpoint != nullptr, NULL);

    cluster_t *binding_cluster = binding::create(endpoint, &(config->binding), CLUSTER_FLAG_SERVER);
    VerifyOrReturnValue(binding_cluster != nullptr, NULL, ESP_LOGE(TAG, "Failed to create binding cluster"));

    return endpoint;
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    on_off::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);

    return ESP_OK;
}
} /* on_off_switch */

namespace dimmer_switch {
uint32_t get_device_type_id()
{
    return ESP_MATTER_DIMMER_SWITCH_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_DIMMER_SWITCH_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t * endpoint = common::create<config_t>(node, config, flags, priv_data, add);
    VerifyOrReturnError(endpoint != nullptr, NULL);

    cluster_t *binding_cluster = binding::create(endpoint, &(config->binding), CLUSTER_FLAG_SERVER);
    VerifyOrReturnValue(binding_cluster != nullptr, NULL, ESP_LOGE(TAG, "Failed to create binding cluster"));

    return endpoint;
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    on_off::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    level_control::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);

    return ESP_OK;
}

} /* dimmer_switch */

namespace color_dimmer_switch {
uint32_t get_device_type_id()
{
    return ESP_MATTER_COLOR_DIMMER_SWITCH_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_COLOR_DIMMER_SWITCH_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t * endpoint = common::create<config_t>(node, config, flags, priv_data, add);
    VerifyOrReturnError(endpoint != nullptr, NULL);

    cluster_t *binding_cluster = binding::create(endpoint, &(config->binding), CLUSTER_FLAG_SERVER);
    VerifyOrReturnValue(binding_cluster != nullptr, NULL, ESP_LOGE(TAG, "Failed to create binding cluster"));

    return endpoint;
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    on_off::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    level_control::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    color_control::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);

    return ESP_OK;
}
} /* color_dimmer_switch */

namespace generic_switch {
uint32_t get_device_type_id()
{
    return ESP_MATTER_GENERIC_SWITCH_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_GENERIC_SWITCH_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    switch_cluster::create(endpoint, &(config->switch_cluster), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* generic_switch */

namespace on_off_plugin_unit {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ON_OFF_PLUGIN_UNIT_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_ON_OFF_PLUGIN_UNIT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    cluster_t *identify_cluster = identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());

    return ESP_OK;
}
} /* on_off_plugin_unit */

namespace dimmable_plugin_unit {
uint32_t get_device_type_id()
{
    return ESP_MATTER_DIMMABLE_PLUGIN_UNIT_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_DIMMABLE_PLUGIN_UNIT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    cluster_t *identify_cluster = identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    level_control::create(endpoint, &(config->level_control), CLUSTER_FLAG_SERVER,
                          level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());

    return ESP_OK;
}
} /* dimmable_plugin_unit */

namespace fan {
uint32_t get_device_type_id()
{
    return ESP_MATTER_FAN_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_FAN_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kActuator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    fan_control::create(endpoint, &(config->fan_control), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* fan */

namespace thermostat {
uint32_t get_device_type_id()
{
    return ESP_MATTER_THERMOSTAT_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_THERMOSTAT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    cluster::thermostat::create(endpoint, &(config->thermostat), CLUSTER_FLAG_SERVER, cluster::thermostat::feature::cooling::get_id()
                                                                | cluster::thermostat::feature::heating::get_id());

    return ESP_OK;
}
} /* thermostat */

namespace aggregator {
uint32_t get_device_type_id()
{
    return ESP_MATTER_AGGREGATOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_AGGREGATOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    return ESP_OK;
}
} /* aggregator */

namespace control_bridge {
uint32_t get_device_type_id()
{
    return ESP_MATTER_CONTROL_BRIDGE_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_CONTROL_BRIDGE_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t * endpoint = common::create<config_t>(node, config, flags, priv_data, add);
    VerifyOrReturnError(endpoint != nullptr, NULL);

    cluster_t *binding_cluster = binding::create(endpoint, &(config->binding), CLUSTER_FLAG_SERVER);
    VerifyOrReturnValue(binding_cluster != nullptr, NULL, ESP_LOGE(TAG, "Failed to create binding cluster"));

    return endpoint;
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    groups::create(endpoint, NULL, CLUSTER_FLAG_CLIENT);
    on_off::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    level_control::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    color_control::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    return ESP_OK;
}
} /* control_bridge */

namespace air_quality_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_AIR_QUALITY_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_AIR_QUALITY_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    air_quality::create(endpoint, &(config->air_quality), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* air_quality_sensor */

namespace air_purifier {
uint32_t get_device_type_id()
{
    return ESP_MATTER_AIR_PURIFIER_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_AIR_PURIFIER_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kActuator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    fan_control::create(endpoint, &(config->fan_control), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* air_purifier */

namespace dish_washer {
uint32_t get_device_type_id()
{
    return ESP_MATTER_DISH_WASHER_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_DISH_WASHER_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    operational_state::create(endpoint, &(config->operational_state), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* dish_washer */

namespace laundry_washer {
uint32_t get_device_type_id()
{
    return ESP_MATTER_LAUNDRY_WASHER_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_LAUNDRY_WASHER_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    operational_state::create(endpoint, &(config->operational_state), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* laundry_washer */

namespace laundry_dryer {
uint32_t get_device_type_id()
{
    return ESP_MATTER_LAUNDRY_DRYER_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_LAUNDRY_DRYER_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    operational_state::create(endpoint, &(config->operational_state), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* laundry_dryer */

namespace smoke_co_alarm {
uint32_t get_device_type_id()
{
    return ESP_MATTER_SMOKE_CO_ALARM_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_SMOKE_CO_ALARM_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void * priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t * endpoint, config_t * config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kAudibleBeep);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    cluster::smoke_co_alarm::create(endpoint, &(config->smoke_co_alarm), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* smoke_co_alarm */

namespace bridged_node {
uint32_t get_device_type_id()
{
    return ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    // bridged node endpoints are always deletable
    return common::create<config_t>(node, config, flags | ENDPOINT_FLAG_DESTROYABLE, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    bridged_device_basic_information::create(endpoint, &(config->bridged_device_basic_information), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}

endpoint_t *resume(node_t *node, config_t *config, uint8_t flags, uint16_t endpoint_id, void *priv_data)
{
    endpoint_t *endpoint = endpoint::resume(node, flags | ENDPOINT_FLAG_DESTROYABLE, endpoint_id, priv_data);
    VerifyOrReturnValue(endpoint != nullptr, NULL, ESP_LOGE(TAG, "Failed to create endpoint"));

    cluster_t *descriptor_cluster = descriptor::create(endpoint, &(config->descriptor), CLUSTER_FLAG_SERVER);
    VerifyOrReturnValue(descriptor_cluster != nullptr, NULL, ESP_LOGE(TAG, "Failed to create descriptor cluster"));

    VerifyOrReturnValue(ESP_OK == add(endpoint, config), NULL, ESP_LOGE(TAG, "Failed to add cluster"));
    return endpoint;
}
} /* bridged_node */

namespace door_lock {
uint32_t get_device_type_id()
{
    return ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kAudibleBeep);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    cluster::door_lock::create(endpoint, &(config->door_lock), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* door_lock */

namespace window_covering_device {
uint32_t get_device_type_id()
{
    return ESP_MATTER_WINDOW_COVERING_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_WINDOW_COVERING_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void * priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kActuator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    window_covering::create(endpoint, &(config->window_covering), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);

    return ESP_OK;
}
} /* window_covering */

namespace temperature_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    temperature_measurement::create(endpoint, &(config->temperature_measurement), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* temperature_sensor */

namespace humidity_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_HUMIDITY_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_HUMIDITY_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    relative_humidity_measurement::create(endpoint, &(config->relative_humidity_measurement), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* humidity_sensor */

namespace occupancy_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_OCCUPANCY_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_OCCUPANCY_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    occupancy_sensing::create(endpoint, &(config->occupancy_sensing), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* occupancy_sensor */

namespace contact_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_CONTACT_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_CONTACT_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    cluster_t * cluster = boolean_state::create(endpoint, &(config->boolean_state), CLUSTER_FLAG_SERVER);
    boolean_state::event::create_state_change(cluster);

    return ESP_OK;
}
} /* contact_sensor */

namespace light_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_LIGHT_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_LIGHT_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    illuminance_measurement::create(endpoint, &(config->illuminance_measurement), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* light_sensor */

namespace pressure_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_PRESSURE_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_PRESSURE_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    pressure_measurement::create(endpoint, &(config->pressure_measurement), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* pressure_sensor */

namespace flow_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_FLOW_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_FLOW_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    flow_measurement::create(endpoint, &(config->flow_measurement), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* flow_sensor */

namespace pump{

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
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kActuator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);
    pump_configuration_and_control::create(endpoint, &(config->pump_configuration_and_control), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /** pump **/

namespace pump_controller{

uint32_t get_device_type_id()
{
    return ESP_MATTER_PUMP_CONTROLLER_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_PUMP_CONTROLLER_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    pump_configuration_and_control::create(endpoint, &(config->pump_configuration_and_control), CLUSTER_FLAG_CLIENT);
    binding::create(endpoint, &(config->binding), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /** pump_controller **/

namespace mode_select_device {
uint32_t get_device_type_id()
{
    return ESP_MATTER_MODE_SELECT_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_MODE_SELECT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster_t *cluster = mode_select::create(endpoint, &(config->mode_select), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);
    if (!cluster) {
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}
} /** mode_select_device **/

namespace room_air_conditioner{

uint32_t get_device_type_id()
{
    return ESP_MATTER_ROOM_AIR_CONDITIONER_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_ROOM_AIR_CONDITIONER_DEVICE_TYPE_VERSION;
}


endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kActuator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::dead_front_behavior::get_id());
    cluster::thermostat::create(endpoint, &(config->thermostat), CLUSTER_FLAG_SERVER, cluster::thermostat::feature::cooling::get_id());

    return ESP_OK;

}
} /** room_air_conditioner **/

namespace temperature_controlled_cabinet{

uint32_t get_device_type_id()
{
    return ESP_MATTER_TEMPERATURE_CONTROLLED_CABINET_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_TEMPERATURE_CONTROLLED_CABINET_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    temperature_control::create(endpoint, &(config->temperature_control), CLUSTER_FLAG_SERVER, temperature_control::feature::temperature_number::get_id());

    return ESP_OK;
}
} /** temperature_controlled_cabinet**/

namespace refrigerator{

uint32_t get_device_type_id()
{
    return ESP_MATTER_REFRIGERATOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_REFRIGERATOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    return ESP_OK;
}
} /** refrigerator **/

namespace oven {

uint32_t get_device_type_id()
{
    return ESP_MATTER_OVEN_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_OVEN_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    return ESP_OK;
}

} /** oven **/

namespace robotic_vacuum_cleaner{

uint32_t get_device_type_id()
{
    return ESP_MATTER_ROBOTIC_VACCUM_CLEANER_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_ROBOTIC_VACCUM_CLEANER_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kActuator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    rvc_run_mode::create(endpoint, &(config->rvc_run_mode), CLUSTER_FLAG_SERVER);
    rvc_operational_state::create(endpoint, &(config->rvc_operational_state), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /** robotic_vacuum_cleaner **/

namespace water_leak_detector {
uint32_t get_device_type_id()
{
    return ESP_MATTER_WATER_LEAK_DETECTOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_WATER_LEAK_DETECTOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    boolean_state::create(endpoint, &(config->boolean_state), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* water_leak_detector */

namespace water_freeze_detector {
uint32_t get_device_type_id()
{
    return ESP_MATTER_WATER_FREEZE_DETECTOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_WATER_FREEZE_DETECTOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    boolean_state::create(endpoint, &(config->boolean_state), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* water_freeze_detector */

namespace rain_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_RAIN_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_RAIN_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kVisibleIndicator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    cluster_t *cluster = boolean_state::create(endpoint, &(config->boolean_state), CLUSTER_FLAG_SERVER);

    boolean_state::event::create_state_change(cluster);
    return ESP_OK;
}
} /* rain_sensor */

namespace electrical_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ELECTRICAL_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_ELECTRICAL_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    power_topology::create(endpoint, &(config->power_topology), CLUSTER_FLAG_SERVER,
                            power_topology::feature::set_topology::get_id());
    electrical_power_measurement::create(endpoint, &(config->electrical_power_measurement), CLUSTER_FLAG_SERVER,
                            electrical_power_measurement::feature::direct_current::get_id()
                            | electrical_power_measurement::feature::alternating_current::get_id());

    return ESP_OK;
}
} /* electrical_sensor */

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
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    temperature_control::create(endpoint, &(config->temperature_control), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);

    return ESP_OK;
}
} /* cook_surface */

namespace cooktop {
uint32_t get_device_type_id()
{
    return ESP_MATTER_COOKTOP_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_COOKTOP_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::off_only::get_id());

    return ESP_OK;
}
} /* cooktop */

namespace energy_evse {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ENERGY_EVSE_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_ENERGY_EVSE_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster::energy_evse::create(endpoint, &(config->energy_evse), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);
    energy_evse_mode::create(endpoint, &(config->energy_evse_mode), CLUSTER_FLAG_SERVER);
    cluster::device_energy_management::create(endpoint, &(config->device_energy_management), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);

    return ESP_OK;
}
} /* energy_evse */

namespace microwave_oven {
uint32_t get_device_type_id()
{
    return ESP_MATTER_MICROWAVE_OVEN_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_MICROWAVE_OVEN_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster_t *cluster = operational_state::create(endpoint, &(config->operational_state), CLUSTER_FLAG_SERVER);
    operational_state::attribute::create_countdown_time(cluster, 0);
    microwave_oven_mode::create(endpoint, &(config->microwave_oven_mode), CLUSTER_FLAG_SERVER);
    microwave_oven_control::create(endpoint, &(config->microwave_oven_control), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);

    return ESP_OK;
}
} /* microwave_oven */

namespace extractor_hood {
uint32_t get_device_type_id()
{
    return ESP_MATTER_EXTRACTOR_HOOD_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_EXTRACTOR_HOOD_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    fan_control::create(endpoint, &(config->fan_control), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* extractor_hood */

namespace water_valve {

uint32_t get_device_type_id()
{
    return ESP_MATTER_WATER_VALVE_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_WATER_VALVE_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kActuator);
    identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    valve_configuration_and_control::create(endpoint, &(config->valve_configuration_and_control), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);

    return ESP_OK;
}
} /** water_valve **/

namespace device_energy_management {

uint32_t get_device_type_id()
{
    return ESP_MATTER_DEVICE_ENERGY_MANAGEMENT_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_DEVICE_ENERGY_MANAGEMENT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster::device_energy_management::create(endpoint, &(config->device_energy_management), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);
    device_energy_management_mode::create(endpoint, &(config->device_energy_management_mode), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /** device_energy_management **/

namespace thread_border_router {

uint32_t get_device_type_id()
{
    return ESP_MATTER_THREAD_BORDER_ROUTER_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_THREAD_BORDER_ROUTER_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    thread_network_diagnostics::create(endpoint, &(config->thread_network_diagnostics), CLUSTER_FLAG_SERVER);
    thread_border_router_management::create(endpoint, &(config->thread_border_router_management), CLUSTER_FLAG_SERVER,
                                            thread_border_router_management::feature::pan_change::get_id());
    return ESP_OK;
}

} /* thread_border_router */

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
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    network_commissioning::create(endpoint, &(config->network_commissioning), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* secondary_network_interface */

namespace mounted_on_off_control {
uint32_t get_device_type_id()
{
    return ESP_MATTER_MOUNTED_ON_OFF_CONTROL_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_MOUNTED_ON_OFF_CONTROL_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kActuator);
    cluster_t *identify_cluster = identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());

    return ESP_OK;
}

} /* mounted_on_off_control */

namespace mounted_dimmable_load_control {
uint32_t get_device_type_id()
{
    return ESP_MATTER_MOUNTED_DIMMABLE_LOAD_CONTROL_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_MOUNTED_DIMMABLE_LOAD_CONTROL_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    identify::config_t identify_config;
    identify_config.identify_type = chip::to_underlying(Identify::IdentifyTypeEnum::kActuator);
    cluster_t *identify_cluster = identify::create(endpoint, &identify_config, CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    level_control::create(endpoint, &(config->level_control), CLUSTER_FLAG_SERVER,
                          level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());

    return ESP_OK;
}
} /* mounted_dimmable_load_control */

namespace water_heater {
uint32_t get_device_type_id()
{
    return ESP_MATTER_WATER_HEATER_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_WATER_HEATER_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster::thermostat::create(endpoint, &(config->thermostat), CLUSTER_FLAG_SERVER, cluster::thermostat::feature::heating::get_id());
    water_heater_management::create(endpoint, &(config->water_heater_management), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);
    water_heater_mode::create(endpoint, &(config->water_heater_mode), CLUSTER_FLAG_SERVER);

    return ESP_OK;
}
} /* water_heater */

namespace solar_power {
uint32_t get_device_type_id()
{
    return ESP_MATTER_SOLAR_POWER_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_SOLAR_POWER_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster_t *descriptor_cluster = cluster::get(endpoint, Descriptor::Id);
    descriptor::feature::taglist::add(descriptor_cluster);

    power_source_device::add(endpoint, &config->power_source_device);

    cluster_t *power_source_cluster = cluster::get(endpoint, PowerSource::Id);
    power_source::feature::wired::add(power_source_cluster, &config->power_source_device.power_source.wired);
    electrical_sensor::add(endpoint, &config->electrical_sensor);
    electrical_energy_measurement::create(endpoint, &(config->electrical_energy_measurement), CLUSTER_FLAG_SERVER, electrical_energy_measurement::feature::exported_energy::get_id() | electrical_energy_measurement::feature::cumulative_energy::get_id());

    cluster_t *elec_power_measurement_cluster = cluster::get(endpoint, ElectricalPowerMeasurement::Id);

    nullable<int64_t> voltage = 0, active_current = 0;
    electrical_power_measurement::attribute::create_voltage(elec_power_measurement_cluster, voltage);
    electrical_power_measurement::attribute::create_active_current(elec_power_measurement_cluster, active_current);


    return ESP_OK;
}
} /* solar_power */

namespace battery_storage {
uint32_t get_device_type_id()
{
    return ESP_MATTER_BATTERY_STORAGE_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_BATTERY_STORAGE_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster_t *descriptor_cluster = cluster::get(endpoint, Descriptor::Id);
    descriptor::feature::taglist::add(descriptor_cluster);

    power_source_device::add(endpoint, &config->power_source_device);

    cluster_t *power_source_cluster = cluster::get(endpoint, PowerSource::Id);
    power_source::feature::wired::add(power_source_cluster, &config->power_source_device.power_source.wired);
    power_source::feature::battery::add(power_source_cluster, &config->power_source_device.power_source.battery);

    power_source::attribute::create_bat_voltage(power_source_cluster, config->bat_voltage, 0x00, 0xFFFF);
    power_source::attribute::create_bat_percent_remaining(power_source_cluster, config->bat_percent_remaining, 0, 200);
    power_source::attribute::create_bat_time_remaining(power_source_cluster, config->bat_time_remaining, 0x00, 0xFFFF);
    power_source::attribute::create_active_bat_faults(power_source_cluster, NULL, 0, 0);
    power_source::attribute::create_bat_capacity(power_source_cluster, config->bat_capacity, 0x00, 0xFFFF);
    power_source::attribute::create_bat_time_to_full_charge(power_source_cluster, config->bat_time_to_full_charge, 0x00, 0xFFFF);
    power_source::attribute::create_bat_charging_current(power_source_cluster, config->bat_charging_current, 0x00, 0xFFFF);
    power_source::attribute::create_active_bat_charge_faults(power_source_cluster, NULL, 0, 0);

    electrical_sensor::add(endpoint, &config->electrical_sensor);
    electrical_energy_measurement::create(endpoint, &(config->electrical_energy_measurement), CLUSTER_FLAG_SERVER, electrical_energy_measurement::feature::exported_energy::get_id() | electrical_energy_measurement::feature::cumulative_energy::get_id());

    cluster_t *elec_power_measurement_cluster = cluster::get(endpoint, ElectricalPowerMeasurement::Id);

    electrical_power_measurement::attribute::create_voltage(elec_power_measurement_cluster, config->voltage);
    electrical_power_measurement::attribute::create_active_current(elec_power_measurement_cluster, config->active_current);

    device_energy_management::add(endpoint, &config->device_energy_management);

    cluster_t *device_energy_management_cluster = cluster::get(endpoint, DeviceEnergyManagement::Id);
    cluster::device_energy_management::feature::power_adjustment::add(device_energy_management_cluster);
    return ESP_OK;
}
} /* battery_storage */

namespace heat_pump {
uint32_t get_device_type_id()
{
    return ESP_MATTER_HEAT_PUMP_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_HEAT_PUMP_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    return common::create<config_t>(node, config, flags, priv_data, add);
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    VerifyOrReturnError(err == ESP_OK, err);

    cluster_t *descriptor_cluster = cluster::get(endpoint, Descriptor::Id);
    descriptor::feature::taglist::add(descriptor_cluster);

    power_source_device::add(endpoint, &config->power_source_device);

    cluster_t *power_source_cluster = cluster::get(endpoint, PowerSource::Id);
    power_source::feature::wired::add(power_source_cluster, &config->power_source_device.power_source.wired);

    electrical_sensor::add(endpoint, &config->electrical_sensor);

    cluster_t *elec_power_measurement_cluster = cluster::get(endpoint, ElectricalPowerMeasurement::Id);

    electrical_power_measurement::attribute::create_voltage(elec_power_measurement_cluster, config->voltage);
    electrical_power_measurement::attribute::create_active_current(elec_power_measurement_cluster, config->active_current);

    electrical_energy_measurement::create(endpoint, &(config->electrical_energy_measurement), CLUSTER_FLAG_SERVER, electrical_energy_measurement::feature::exported_energy::get_id() | electrical_energy_measurement::feature::cumulative_energy::get_id());

    device_energy_management::add(endpoint, &config->device_energy_management);

    cluster_t *device_energy_management_cluster = cluster::get(endpoint, DeviceEnergyManagement::Id);
    cluster::device_energy_management::feature::power_adjustment::add(device_energy_management_cluster);
    return ESP_OK;
}
} /* heat_pump */

} /* endpoint */

namespace node {

node_t *create(config_t *config, attribute::callback_t attribute_callback,
               identification::callback_t identification_callback, void* priv_data)
{
    attribute::set_callback(attribute_callback);
    identification::set_callback(identification_callback);

    node_t *node = create_raw();
    VerifyOrReturnValue(node != nullptr, NULL, ESP_LOGE(TAG, "Could not create node"));

    endpoint::root_node::create(node, &(config->root_node), ENDPOINT_FLAG_NONE, priv_data);
    return node;
}

} /* node */
} /* esp_matter */
