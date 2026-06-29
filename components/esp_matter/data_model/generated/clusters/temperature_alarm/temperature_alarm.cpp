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
#include <temperature_alarm.h>
#include <temperature_alarm_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace temperature_alarm {

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

namespace over_temperature {
uint32_t get_id()
{
    return OverTemperature::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_critical_over_temperature_threshold(cluster, config->critical_over_temperature_threshold);
    attribute::create_major_over_temperature_threshold(cluster, config->major_over_temperature_threshold);
    attribute::create_minor_over_temperature_threshold(cluster, config->minor_over_temperature_threshold);

    return ESP_OK;
}
} /* over_temperature */

namespace under_temperature {
uint32_t get_id()
{
    return UnderTemperature::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_minor_under_temperature_threshold(cluster, config->minor_under_temperature_threshold);
    attribute::create_major_under_temperature_threshold(cluster, config->major_under_temperature_threshold);
    attribute::create_critical_under_temperature_threshold(cluster, config->critical_under_temperature_threshold);

    return ESP_OK;
}
} /* under_temperature */

namespace major_threshold {
uint32_t get_id()
{
    return MajorThreshold::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_major_over_temperature_threshold(cluster, config->major_over_temperature_threshold);
    attribute::create_major_under_temperature_threshold(cluster, config->major_under_temperature_threshold);

    return ESP_OK;
}
} /* major_threshold */

namespace minor_threshold {
uint32_t get_id()
{
    return MinorThreshold::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(major_threshold), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_minor_over_temperature_threshold(cluster, config->minor_over_temperature_threshold);
    attribute::create_minor_under_temperature_threshold(cluster, config->minor_under_temperature_threshold);

    return ESP_OK;
}
} /* minor_threshold */

namespace over_critical_adjustable {
uint32_t get_id()
{
    return OverCriticalAdjustable::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(over_temperature), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_set_temperature_alarm_thresholds(cluster);

    return ESP_OK;
}
} /* over_critical_adjustable */

namespace over_major_adjustable {
uint32_t get_id()
{
    return OverMajorAdjustable::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(over_temperature)) && (has_feature(major_threshold))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_set_temperature_alarm_thresholds(cluster);

    return ESP_OK;
}
} /* over_major_adjustable */

namespace over_minor_adjustable {
uint32_t get_id()
{
    return OverMinorAdjustable::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(over_temperature)) && (has_feature(minor_threshold))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_set_temperature_alarm_thresholds(cluster);

    return ESP_OK;
}
} /* over_minor_adjustable */

namespace under_minor_adjustable {
uint32_t get_id()
{
    return UnderMinorAdjustable::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(under_temperature)) && (has_feature(minor_threshold))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_set_temperature_alarm_thresholds(cluster);

    return ESP_OK;
}
} /* under_minor_adjustable */

namespace under_major_adjustable {
uint32_t get_id()
{
    return UnderMajorAdjustable::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(under_temperature)) && (has_feature(major_threshold))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_set_temperature_alarm_thresholds(cluster);

    return ESP_OK;
}
} /* under_major_adjustable */

namespace under_critical_adjustable {
uint32_t get_id()
{
    return UnderCriticalAdjustable::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(under_temperature), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_set_temperature_alarm_thresholds(cluster);

    return ESP_OK;
}
} /* under_critical_adjustable */

} /* feature */

namespace attribute {
attribute_t *create_mask(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Mask::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(4294967295), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_latch(cluster_t *cluster, uint32_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(reset), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, Latch::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(4294967295), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_state(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, State::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(4294967295), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_supported(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Supported::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(4294967295), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_critical_over_temperature_threshold(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(over_temperature), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CriticalOverTemperatureThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_major_over_temperature_threshold(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(over_temperature)) && (has_feature(major_threshold))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MajorOverTemperatureThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_minor_over_temperature_threshold(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(over_temperature)) && (has_feature(minor_threshold))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinorOverTemperatureThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_minor_under_temperature_threshold(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(under_temperature)) && (has_feature(minor_threshold))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinorUnderTemperatureThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_major_under_temperature_threshold(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(under_temperature)) && (has_feature(major_threshold))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MajorUnderTemperatureThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_critical_under_temperature_threshold(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(under_temperature), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CriticalUnderTemperatureThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
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

command_t *create_set_temperature_alarm_thresholds(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(over_critical_adjustable)) || (has_feature(over_major_adjustable)) || (has_feature(over_minor_adjustable)) || (has_feature(under_minor_adjustable)) || (has_feature(under_major_adjustable)) || (has_feature(under_critical_adjustable))), NULL);
    return esp_matter::command::create(cluster, SetTemperatureAlarmThresholds::Id, COMMAND_FLAG_ACCEPTED, NULL);
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
    cluster_t *cluster = esp_matter::cluster::create(endpoint, temperature_alarm::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, temperature_alarm::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_mask(cluster, config->mask);
        attribute::create_state(cluster, config->state);
        attribute::create_supported(cluster, config->supported);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("OverTemperature,UnderTemperature",
                                       feature::over_temperature::get_id(), feature::under_temperature::get_id());
        if (feature_map & feature::over_temperature::get_id()) {
            VerifyOrReturnValue(feature::over_temperature::add(cluster, &(config->features.over_temperature)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::under_temperature::get_id()) {
            VerifyOrReturnValue(feature::under_temperature::add(cluster, &(config->features.under_temperature)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::reset::get_id()) {
            VerifyOrReturnValue(feature::reset::add(cluster, &(config->features.reset)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::major_threshold::get_id()) {
            VerifyOrReturnValue(feature::major_threshold::add(cluster, &(config->features.major_threshold)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::minor_threshold::get_id()) {
            VerifyOrReturnValue(feature::minor_threshold::add(cluster, &(config->features.minor_threshold)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::over_critical_adjustable::get_id()) {
            VerifyOrReturnValue(feature::over_critical_adjustable::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::over_major_adjustable::get_id()) {
            VerifyOrReturnValue(feature::over_major_adjustable::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::over_minor_adjustable::get_id()) {
            VerifyOrReturnValue(feature::over_minor_adjustable::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::under_minor_adjustable::get_id()) {
            VerifyOrReturnValue(feature::under_minor_adjustable::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::under_major_adjustable::get_id()) {
            VerifyOrReturnValue(feature::under_major_adjustable::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::under_critical_adjustable::get_id()) {
            VerifyOrReturnValue(feature::under_critical_adjustable::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        /* Events */
        event::create_notify(cluster);
    }

    return cluster;
}

} /* temperature_alarm */
} /* cluster */
} /* esp_matter */
