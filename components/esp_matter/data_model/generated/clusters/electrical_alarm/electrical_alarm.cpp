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
#include <electrical_alarm.h>
#include <electrical_alarm_ids.h>
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
namespace electrical_alarm {

namespace feature {
namespace reset {
uint32_t get_id()
{
    return Reset::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_latch(cluster, config->latch);
    command::create_reset(cluster);

    return ESP_OK;
}
} /* reset */

namespace adjustable_thresholds {
uint32_t get_id()
{
    return AdjustableThresholds::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_set_electrical_alarm_thresholds(cluster);

    return ESP_OK;
}
} /* adjustable_thresholds */

namespace over_voltage {
uint32_t get_id()
{
    return OverVoltage::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_over_voltage_threshold(cluster, config->over_voltage_threshold);

    return ESP_OK;
}
} /* over_voltage */

namespace under_voltage {
uint32_t get_id()
{
    return UnderVoltage::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_under_voltage_threshold(cluster, config->under_voltage_threshold);

    return ESP_OK;
}
} /* under_voltage */

namespace over_frequency {
uint32_t get_id()
{
    return OverFrequency::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_over_frequency_threshold(cluster, config->over_frequency_threshold);

    return ESP_OK;
}
} /* over_frequency */

namespace under_frequency {
uint32_t get_id()
{
    return UnderFrequency::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_under_frequency_threshold(cluster, config->under_frequency_threshold);

    return ESP_OK;
}
} /* under_frequency */

namespace over_power {
uint32_t get_id()
{
    return OverPower::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_over_power_threshold(cluster, config->over_power_threshold);

    return ESP_OK;
}
} /* over_power */

namespace under_power {
uint32_t get_id()
{
    return UnderPower::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_under_power_threshold(cluster, config->under_power_threshold);

    return ESP_OK;
}
} /* under_power */

namespace over_current {
uint32_t get_id()
{
    return OverCurrent::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_over_current_threshold(cluster, config->over_current_threshold);

    return ESP_OK;
}
} /* over_current */

namespace under_current {
uint32_t get_id()
{
    return UnderCurrent::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_under_current_threshold(cluster, config->under_current_threshold);

    return ESP_OK;
}
} /* under_current */

namespace power_import {
uint32_t get_id()
{
    return PowerImport::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_power_import_threshold(cluster, config->power_import_threshold);

    return ESP_OK;
}
} /* power_import */

namespace power_export {
uint32_t get_id()
{
    return PowerExport::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_power_export_threshold(cluster, config->power_export_threshold);

    return ESP_OK;
}
} /* power_export */

} /* feature */

namespace attribute {
attribute_t *create_mask(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Mask::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(Mask::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(Mask::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_latch(cluster_t *cluster, uint32_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(reset), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, Latch::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(Latch::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(Latch::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_state(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, State::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(State::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(State::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_supported(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Supported::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(Supported::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(Supported::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_over_voltage_threshold(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(over_voltage), NULL);
    return esp_matter::attribute::create(cluster, OverVoltageThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_under_voltage_threshold(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(under_voltage), NULL);
    return esp_matter::attribute::create(cluster, UnderVoltageThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_over_frequency_threshold(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(over_frequency), NULL);
    return esp_matter::attribute::create(cluster, OverFrequencyThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_under_frequency_threshold(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(under_frequency), NULL);
    return esp_matter::attribute::create(cluster, UnderFrequencyThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_over_power_threshold(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(over_power), NULL);
    return esp_matter::attribute::create(cluster, OverPowerThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_under_power_threshold(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(under_power), NULL);
    return esp_matter::attribute::create(cluster, UnderPowerThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_over_current_threshold(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(over_current), NULL);
    return esp_matter::attribute::create(cluster, OverCurrentThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_under_current_threshold(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(under_current), NULL);
    return esp_matter::attribute::create(cluster, UnderCurrentThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_power_import_threshold(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_import), NULL);
    return esp_matter::attribute::create(cluster, PowerImportThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_power_export_threshold(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_export), NULL);
    return esp_matter::attribute::create(cluster, PowerExportThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

} /* attribute */
namespace command {
command_t *create_reset(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(reset), NULL);
    return esp_matter::command::create(cluster, Reset::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_modify_enabled_alarms(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ModifyEnabledAlarms::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_electrical_alarm_thresholds(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(adjustable_thresholds), NULL);
    return esp_matter::command::create(cluster, SetElectricalAlarmThresholds::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_notify(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Notify::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, electrical_alarm::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, electrical_alarm::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterElectricalAlarmPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_mask(cluster, config->mask);
        attribute::create_state(cluster, config->state);
        attribute::create_supported(cluster, config->supported);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("UnderFrequency,UnderCurrent,OverVoltage,OverPower,PowerImport,UnderVoltage,UnderPower,PowerExport,OverFrequency,OverCurrent",
                                       feature::under_frequency::get_id(), feature::under_current::get_id(), feature::over_voltage::get_id(), feature::over_power::get_id(), feature::power_import::get_id(), feature::under_voltage::get_id(), feature::under_power::get_id(), feature::power_export::get_id(), feature::over_frequency::get_id(), feature::over_current::get_id());
        if (feature_map & feature::under_frequency::get_id()) {
            VerifyOrReturnValue(feature::under_frequency::add(cluster, &(config->features.under_frequency)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::under_current::get_id()) {
            VerifyOrReturnValue(feature::under_current::add(cluster, &(config->features.under_current)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::over_voltage::get_id()) {
            VerifyOrReturnValue(feature::over_voltage::add(cluster, &(config->features.over_voltage)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::over_power::get_id()) {
            VerifyOrReturnValue(feature::over_power::add(cluster, &(config->features.over_power)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::power_import::get_id()) {
            VerifyOrReturnValue(feature::power_import::add(cluster, &(config->features.power_import)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::under_voltage::get_id()) {
            VerifyOrReturnValue(feature::under_voltage::add(cluster, &(config->features.under_voltage)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::under_power::get_id()) {
            VerifyOrReturnValue(feature::under_power::add(cluster, &(config->features.under_power)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::power_export::get_id()) {
            VerifyOrReturnValue(feature::power_export::add(cluster, &(config->features.power_export)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::over_frequency::get_id()) {
            VerifyOrReturnValue(feature::over_frequency::add(cluster, &(config->features.over_frequency)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::over_current::get_id()) {
            VerifyOrReturnValue(feature::over_current::add(cluster, &(config->features.over_current)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::reset::get_id()) {
            VerifyOrReturnValue(feature::reset::add(cluster, &(config->features.reset)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::adjustable_thresholds::get_id()) {
            VerifyOrReturnValue(feature::adjustable_thresholds::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        /* Events */
        event::create_notify(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterElectricalAlarmClusterServerInitCallback,
                                                 ESPMatterElectricalAlarmClusterServerShutdownCallback);
    }

    return cluster;
}

} /* electrical_alarm */
} /* cluster */
} /* esp_matter */
