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
#include <device_energy_management.h>
#include <device_energy_management_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 4;

namespace esp_matter {
namespace cluster {
namespace device_energy_management {

namespace feature {
namespace power_adjustment {
uint32_t get_id()
{
    return PowerAdjustment::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_opt_out_state(cluster, config->opt_out_state);
    attribute::create_power_adjustment_capability(cluster, NULL, 0, 0);
    command::create_power_adjust_request(cluster);
    command::create_cancel_power_adjust_request(cluster);
    event::create_power_adjust_start(cluster);
    event::create_power_adjust_end(cluster);

    return ESP_OK;
}
} /* power_adjustment */

namespace power_forecast_reporting {
uint32_t get_id()
{
    return PowerForecastReporting::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_forecast(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* power_forecast_reporting */

namespace state_forecast_reporting {
uint32_t get_id()
{
    return StateForecastReporting::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(!(has_feature(power_adjustment)), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_forecast(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* state_forecast_reporting */

namespace start_time_adjustment {
uint32_t get_id()
{
    return StartTimeAdjustment::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_opt_out_state(cluster, config->opt_out_state);
    command::create_start_time_adjust_request(cluster);
    command::create_cancel_request(cluster);

    return ESP_OK;
}
} /* start_time_adjustment */

namespace pausable {
uint32_t get_id()
{
    return Pausable::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_opt_out_state(cluster, config->opt_out_state);
    command::create_pause_request(cluster);
    command::create_resume_request(cluster);
    event::create_paused(cluster);
    event::create_resumed(cluster);

    return ESP_OK;
}
} /* pausable */

namespace forecast_adjustment {
uint32_t get_id()
{
    return ForecastAdjustment::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_opt_out_state(cluster, config->opt_out_state);
    command::create_modify_forecast_request(cluster);
    command::create_cancel_request(cluster);

    return ESP_OK;
}
} /* forecast_adjustment */

namespace constraint_based_adjustment {
uint32_t get_id()
{
    return ConstraintBasedAdjustment::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_opt_out_state(cluster, config->opt_out_state);
    command::create_request_constraint_based_forecast(cluster);
    command::create_cancel_request(cluster);

    return ESP_OK;
}
} /* constraint_based_adjustment */

} /* feature */

namespace attribute {
attribute_t *create_esa_type(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ESAType::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(14), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_esa_can_generate(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ESACanGenerate::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_esa_state(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ESAState::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(4), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_abs_min_power(cluster_t *cluster, int64_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AbsMinPower::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int64_t>(-2147483648)), esp_matter_attr_val(static_cast<int64_t>(2147483646)));
    return attribute;
}

attribute_t *create_abs_max_power(cluster_t *cluster, int64_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AbsMaxPower::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int64_t>(-2147483648)), esp_matter_attr_val(static_cast<int64_t>(2147483646)));
    return attribute;
}

attribute_t *create_power_adjustment_capability(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_adjustment), NULL);
    return esp_matter::attribute::create(cluster, PowerAdjustmentCapability::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_forecast(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(power_forecast_reporting)) || (has_feature(state_forecast_reporting))), NULL);
    return esp_matter::attribute::create(cluster, Forecast::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_opt_out_state(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(power_adjustment)) || (has_feature(start_time_adjustment)) || (has_feature(pausable)) || (has_feature(forecast_adjustment)) || (has_feature(constraint_based_adjustment))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, OptOutState::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_power_adjust_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_adjustment), NULL);
    return esp_matter::command::create(cluster, PowerAdjustRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_cancel_power_adjust_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_adjustment), NULL);
    return esp_matter::command::create(cluster, CancelPowerAdjustRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_start_time_adjust_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(start_time_adjustment), NULL);
    return esp_matter::command::create(cluster, StartTimeAdjustRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_pause_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(pausable), NULL);
    return esp_matter::command::create(cluster, PauseRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_resume_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(pausable), NULL);
    return esp_matter::command::create(cluster, ResumeRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_modify_forecast_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(forecast_adjustment), NULL);
    return esp_matter::command::create(cluster, ModifyForecastRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_request_constraint_based_forecast(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(constraint_based_adjustment), NULL);
    return esp_matter::command::create(cluster, RequestConstraintBasedForecast::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_cancel_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(start_time_adjustment)) || (has_feature(forecast_adjustment)) || (has_feature(constraint_based_adjustment))), NULL);
    return esp_matter::command::create(cluster, CancelRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_power_adjust_start(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_adjustment), NULL);
    return esp_matter::event::create(cluster, PowerAdjustStart::Id);
}

event_t *create_power_adjust_end(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_adjustment), NULL);
    return esp_matter::event::create(cluster, PowerAdjustEnd::Id);
}

event_t *create_paused(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(pausable), NULL);
    return esp_matter::event::create(cluster, Paused::Id);
}

event_t *create_resumed(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(pausable), NULL);
    return esp_matter::event::create(cluster, Resumed::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, device_energy_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, device_energy_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = DeviceEnergyManagementDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterDeviceEnergyManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_esa_type(cluster, config->esa_type);
        attribute::create_esa_can_generate(cluster, config->esa_can_generate);
        attribute::create_esa_state(cluster, config->esa_state);
        attribute::create_abs_min_power(cluster, config->abs_min_power);
        attribute::create_abs_max_power(cluster, config->abs_max_power);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_EXACT_ONE("PowerForecastReporting,StateForecastReporting",
                                    feature::power_forecast_reporting::get_id(), feature::state_forecast_reporting::get_id());
        if (feature_map & feature::power_forecast_reporting::get_id()) {
            VerifyOrReturnValue(feature::power_forecast_reporting::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::state_forecast_reporting::get_id()) {
            VerifyOrReturnValue(feature::state_forecast_reporting::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::power_adjustment::get_id()) {
            VerifyOrReturnValue(feature::power_adjustment::add(cluster, &(config->features.power_adjustment)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::start_time_adjustment::get_id()) {
            VerifyOrReturnValue(feature::start_time_adjustment::add(cluster, &(config->features.start_time_adjustment)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::pausable::get_id()) {
            VerifyOrReturnValue(feature::pausable::add(cluster, &(config->features.pausable)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::forecast_adjustment::get_id()) {
            VerifyOrReturnValue(feature::forecast_adjustment::add(cluster, &(config->features.forecast_adjustment)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::constraint_based_adjustment::get_id()) {
            VerifyOrReturnValue(feature::constraint_based_adjustment::add(cluster, &(config->features.constraint_based_adjustment)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterDeviceEnergyManagementClusterServerInitCallback,
                                                 ESPMatterDeviceEnergyManagementClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* device_energy_management */
} /* cluster */
} /* esp_matter */
