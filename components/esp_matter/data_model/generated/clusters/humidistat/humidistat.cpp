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
#include <humidistat.h>
#include <humidistat_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace humidistat {

namespace feature {
namespace humidifier {
uint32_t get_id()
{
    return Humidifier::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_mist_type(cluster, config->mist_type);

    return ESP_OK;
}
} /* humidifier */

namespace dehumidifier {
uint32_t get_id()
{
    return Dehumidifier::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* dehumidifier */

namespace continuous {
uint32_t get_id()
{
    return Continuous::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_continuous(cluster, config->continuous);

    return ESP_OK;
}
} /* continuous */

namespace sensor {
uint32_t get_id()
{
    return Sensor::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_user_setpoint(cluster, config->user_setpoint);
    attribute::create_min_setpoint(cluster, config->min_setpoint);
    attribute::create_max_setpoint(cluster, config->max_setpoint);
    attribute::create_step(cluster, config->step);
    attribute::create_target_setpoint(cluster, config->target_setpoint);

    return ESP_OK;
}
} /* sensor */

namespace fan_auto {
uint32_t get_id()
{
    return FanAuto::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(humidifier)) && (has_feature(dehumidifier)) && (has_feature(sensor))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* fan_auto */

namespace fan_only {
uint32_t get_id()
{
    return FanOnly::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* fan_only */

namespace optimal {
uint32_t get_id()
{
    return Optimal::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(sensor), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_target_setpoint(cluster, config->target_setpoint);
    attribute::create_optimal(cluster, config->optimal);

    return ESP_OK;
}
} /* optimal */

namespace warm_mist {
uint32_t get_id()
{
    return WarmMist::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(humidifier), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* warm_mist */

namespace cold_mist {
uint32_t get_id()
{
    return ColdMist::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(humidifier), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* cold_mist */

namespace cond_pump {
uint32_t get_id()
{
    return CondPump::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_cond_pump_enabled(cluster, config->cond_pump_enabled);
    attribute::create_cond_run_count(cluster, config->cond_run_count);

    return ESP_OK;
}
} /* cond_pump */

} /* feature */

namespace attribute {
attribute_t *create_supported_modes(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, SupportedModes::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Mode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(Mode::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(Mode::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_system_state(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SystemState::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(SystemState::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(SystemState::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_user_setpoint(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(sensor), NULL);
    return esp_matter::attribute::create(cluster, UserSetpoint::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_min_setpoint(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(sensor), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinSetpoint::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(MinSetpoint::Min)), esp_matter_attr_val(static_cast<uint8_t>(MinSetpoint::Max)));
    return attribute;
}

attribute_t *create_max_setpoint(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(sensor), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxSetpoint::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(MaxSetpoint::Min)), esp_matter_attr_val(static_cast<uint8_t>(MaxSetpoint::Max)));
    return attribute;
}

attribute_t *create_step(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(sensor), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, Step::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(Step::Min)), esp_matter_attr_val(static_cast<uint8_t>(Step::Max)));
    return attribute;
}

attribute_t *create_target_setpoint(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, TargetSetpoint::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_mist_type(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(humidifier), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MistType::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(MistType::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(nullable<uint8_t>(MistType::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_continuous(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(continuous), NULL);
    return esp_matter::attribute::create(cluster, Continuous::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_sleep(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, Sleep::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_optimal(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(optimal), NULL);
    return esp_matter::attribute::create(cluster, Optimal::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_cond_pump_enabled(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(cond_pump), NULL);
    return esp_matter::attribute::create(cluster, CondPumpEnabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_cond_run_count(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(cond_pump), NULL);
    return esp_matter::attribute::create(cluster, CondRunCount::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

} /* attribute */
namespace command {
command_t *create_set_settings(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetSettings::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, humidistat::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, humidistat::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterHumidistatPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_mode(cluster, config->mode);
        attribute::create_system_state(cluster, config->system_state);
        attribute::create_supported_modes(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        if (feature_map & feature::humidifier::get_id()) {
            VALIDATE_FEATURES_AT_LEAST_ONE("WarmMist,ColdMist",
                                           feature::warm_mist::get_id(), feature::cold_mist::get_id());
            if (feature_map & feature::warm_mist::get_id()) {
                VerifyOrReturnValue(feature::warm_mist::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            }
            if (feature_map & feature::cold_mist::get_id()) {
                VerifyOrReturnValue(feature::cold_mist::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            }
        }
        VALIDATE_FEATURES_AT_LEAST_ONE("Humidifier,Dehumidifier",
                                       feature::humidifier::get_id(), feature::dehumidifier::get_id());
        if (feature_map & feature::humidifier::get_id()) {
            VerifyOrReturnValue(feature::humidifier::add(cluster, &(config->features.humidifier)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::dehumidifier::get_id()) {
            VerifyOrReturnValue(feature::dehumidifier::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::continuous::get_id()) {
            VerifyOrReturnValue(feature::continuous::add(cluster, &(config->features.continuous)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::sensor::get_id()) {
            VerifyOrReturnValue(feature::sensor::add(cluster, &(config->features.sensor)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::fan_auto::get_id()) {
            VerifyOrReturnValue(feature::fan_auto::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::fan_only::get_id()) {
            VerifyOrReturnValue(feature::fan_only::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::optimal::get_id()) {
            VerifyOrReturnValue(feature::optimal::add(cluster, &(config->features.optimal)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::cond_pump::get_id()) {
            VerifyOrReturnValue(feature::cond_pump::add(cluster, &(config->features.cond_pump)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_set_settings(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterHumidistatClusterServerInitCallback,
                                                 ESPMatterHumidistatClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* humidistat */
} /* cluster */
} /* esp_matter */
