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
#include <esp_matter_core.h>
#include <esp_matter.h>

#include <app-common/zap-generated/cluster-enums.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <esp_matter_delegate_callbacks.h>
#include <temperature_control.h>
#include <temperature_control_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace temperature_control {

namespace feature {
namespace temperature_number {
uint32_t get_id()
{
    return TemperatureNumber::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_temperature_setpoint(cluster, config->temperature_setpoint);
    attribute::create_min_temperature(cluster, config->min_temperature);
    attribute::create_max_temperature(cluster, config->max_temperature);

    return ESP_OK;
}
} /* temperature_number */

namespace temperature_level {
uint32_t get_id()
{
    return TemperatureLevel::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_selected_temperature_level(cluster, config->selected_temperature_level);
    attribute::create_supported_temperature_levels(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* temperature_level */

namespace temperature_step {
uint32_t get_id()
{
    return TemperatureStep::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(temperature_number), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_step(cluster, config->step);

    return ESP_OK;
}
} /* temperature_step */

} /* feature */

namespace attribute {
attribute_t *create_temperature_setpoint(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(temperature_number), NULL);
    return esp_matter::attribute::create(cluster, TemperatureSetpoint::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_min_temperature(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(temperature_number), NULL);
    return esp_matter::attribute::create(cluster, MinTemperature::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_max_temperature(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(temperature_number), NULL);
    return esp_matter::attribute::create(cluster, MaxTemperature::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_step(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(temperature_step), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, Step::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(Step::Min)), esp_matter_attr_val(static_cast<int16_t>(Step::Max)));
    return attribute;
}

attribute_t *create_selected_temperature_level(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(temperature_level), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, SelectedTemperatureLevel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(SelectedTemperatureLevel::Min)), esp_matter_attr_val(static_cast<uint8_t>(SelectedTemperatureLevel::Max)));
    return attribute;
}

attribute_t *create_supported_temperature_levels(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(temperature_level), NULL);
    return esp_matter::attribute::create(cluster, SupportedTemperatureLevels::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_set_temperature(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetTemperature::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, temperature_control::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, temperature_control::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = TemperatureControlDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterTemperatureControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_EXACT_ONE("TemperatureNumber,TemperatureLevel",
                                    feature::temperature_number::get_id(), feature::temperature_level::get_id());
        if (feature_map & feature::temperature_number::get_id()) {
            VerifyOrReturnValue(feature::temperature_number::add(cluster, &(config->features.temperature_number)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::temperature_level::get_id()) {
            VerifyOrReturnValue(feature::temperature_level::add(cluster, &(config->features.temperature_level)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::temperature_step::get_id()) {
            VerifyOrReturnValue(feature::temperature_step::add(cluster, &(config->features.temperature_step)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_set_temperature(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterTemperatureControlClusterServerInitCallback,
                                                 ESPMatterTemperatureControlClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* temperature_control */
} /* cluster */
} /* esp_matter */
