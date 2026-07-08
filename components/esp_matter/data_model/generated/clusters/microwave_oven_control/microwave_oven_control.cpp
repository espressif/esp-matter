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
#include <microwave_oven_control.h>
#include <microwave_oven_control_ids.h>
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
namespace microwave_oven_control {

namespace feature {
namespace power_as_number {
uint32_t get_id()
{
    return PowerAsNumber::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_power_setting(cluster, config->power_setting);

    return ESP_OK;
}
} /* power_as_number */

namespace power_number_limits {
uint32_t get_id()
{
    return PowerNumberLimits::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(power_as_number), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_min_power(cluster, config->min_power);
    attribute::create_max_power(cluster, config->max_power);
    attribute::create_power_step(cluster, config->power_step);

    return ESP_OK;
}
} /* power_number_limits */

} /* feature */

namespace attribute {
attribute_t *create_cook_time(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, CookTime::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(1)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_max_cook_time(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxCookTime::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(1)), esp_matter_attr_val(static_cast<uint32_t>(86400)));
    return attribute;
}

attribute_t *create_power_setting(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_as_number), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSetting::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_min_power(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_number_limits), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinPower::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(1)), esp_matter_attr_val(static_cast<uint8_t>(99)));
    return attribute;
}

attribute_t *create_max_power(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_number_limits), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxPower::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(100)));
    return attribute;
}

attribute_t *create_power_step(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_number_limits), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerStep::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_watt_rating(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, WattRating::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_set_cooking_parameters(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetCookingParameters::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_add_more_time(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddMoreTime::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, microwave_oven_control::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, microwave_oven_control::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = MicrowaveOvenControlDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterMicrowaveOvenControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_cook_time(cluster, config->cook_time);
        attribute::create_max_cook_time(cluster, config->max_cook_time);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_EXACT_ONE("PowerAsNumber",
                                    feature::power_as_number::get_id());
        if (feature_map & feature::power_as_number::get_id()) {
            VerifyOrReturnValue(feature::power_as_number::add(cluster, &(config->features.power_as_number)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::power_number_limits::get_id()) {
            VerifyOrReturnValue(feature::power_number_limits::add(cluster, &(config->features.power_number_limits)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_set_cooking_parameters(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterMicrowaveOvenControlClusterServerInitCallback,
                                                 ESPMatterMicrowaveOvenControlClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* microwave_oven_control */
} /* cluster */
} /* esp_matter */
