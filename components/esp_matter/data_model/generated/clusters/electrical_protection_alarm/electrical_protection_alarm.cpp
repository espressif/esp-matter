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
#include <electrical_protection_alarm.h>
#include <electrical_protection_alarm_ids.h>
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
namespace electrical_protection_alarm {

namespace feature {
namespace short_circuit {
uint32_t get_id()
{
    return ShortCircuit::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_short_circuit_rating(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* short_circuit */

namespace over_load {
uint32_t get_id()
{
    return OverLoad::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_over_load_rating(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* over_load */

namespace over_voltage {
uint32_t get_id()
{
    return OverVoltage::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_over_voltage_rating(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* over_voltage */

namespace surge_protection {
uint32_t get_id()
{
    return SurgeProtection::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_surge_protection_rating(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* surge_protection */

namespace residual_current {
uint32_t get_id()
{
    return ResidualCurrent::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_residual_current_rating(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* residual_current */

namespace arc_fault {
uint32_t get_id()
{
    return ArcFault::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_arc_cause(cluster, config->arc_cause);
    attribute::create_arc_fault_rating(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* arc_fault */

namespace self_test {
uint32_t get_id()
{
    return SelfTest::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* self_test */

} /* feature */

namespace attribute {
attribute_t *create_mask(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Mask::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(Mask::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(Mask::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
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

attribute_t *create_arc_cause(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(arc_fault), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ArcCause::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(ArcCause::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(nullable<uint8_t>(ArcCause::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_over_load_rating(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(over_load), NULL);
    return esp_matter::attribute::create(cluster, OverLoadRating::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_over_voltage_rating(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(over_voltage), NULL);
    return esp_matter::attribute::create(cluster, OverVoltageRating::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_surge_protection_rating(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(surge_protection), NULL);
    return esp_matter::attribute::create(cluster, SurgeProtectionRating::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_short_circuit_rating(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(short_circuit), NULL);
    return esp_matter::attribute::create(cluster, ShortCircuitRating::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_residual_current_rating(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(residual_current), NULL);
    return esp_matter::attribute::create(cluster, ResidualCurrentRating::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_arc_fault_rating(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(arc_fault), NULL);
    return esp_matter::attribute::create(cluster, ArcFaultRating::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_modify_enabled_alarms(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ModifyEnabledAlarms::Id, COMMAND_FLAG_ACCEPTED, NULL);
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
    cluster_t *cluster = esp_matter::cluster::create(endpoint, electrical_protection_alarm::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, electrical_protection_alarm::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterElectricalProtectionAlarmPluginServerInitCallback);
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
        VALIDATE_FEATURES_AT_LEAST_ONE("ShortCircuit,ResidualCurrent,OverLoad,ArcFault,OverVoltage,SurgeProtection",
                                       feature::short_circuit::get_id(), feature::residual_current::get_id(), feature::over_load::get_id(), feature::arc_fault::get_id(), feature::over_voltage::get_id(), feature::surge_protection::get_id());
        if (feature_map & feature::short_circuit::get_id()) {
            VerifyOrReturnValue(feature::short_circuit::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::residual_current::get_id()) {
            VerifyOrReturnValue(feature::residual_current::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::over_load::get_id()) {
            VerifyOrReturnValue(feature::over_load::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::arc_fault::get_id()) {
            VerifyOrReturnValue(feature::arc_fault::add(cluster, &(config->features.arc_fault)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::over_voltage::get_id()) {
            VerifyOrReturnValue(feature::over_voltage::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::surge_protection::get_id()) {
            VerifyOrReturnValue(feature::surge_protection::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::self_test::get_id()) {
            VerifyOrReturnValue(feature::self_test::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        /* Events */
        event::create_notify(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterElectricalProtectionAlarmClusterServerInitCallback,
                                                 ESPMatterElectricalProtectionAlarmClusterServerShutdownCallback);
    }

    return cluster;
}

} /* electrical_protection_alarm */
} /* cluster */
} /* esp_matter */
