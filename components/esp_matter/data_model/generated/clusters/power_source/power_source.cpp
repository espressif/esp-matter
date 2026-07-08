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
#include <power_source.h>
#include <power_source_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 3;

namespace esp_matter {
namespace cluster {
namespace power_source {

namespace feature {
namespace wired {
uint32_t get_id()
{
    return Wired::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_wired_current_type(cluster, config->wired_current_type);

    return ESP_OK;
}
} /* wired */

namespace battery {
uint32_t get_id()
{
    return Battery::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_bat_charge_level(cluster, config->bat_charge_level);
    attribute::create_bat_replacement_needed(cluster, config->bat_replacement_needed);
    attribute::create_bat_replaceability(cluster, config->bat_replaceability);

    return ESP_OK;
}
} /* battery */

namespace rechargeable {
uint32_t get_id()
{
    return Rechargeable::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(battery), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_bat_charge_state(cluster, config->bat_charge_state);
    attribute::create_bat_functional_while_charging(cluster, config->bat_functional_while_charging);

    return ESP_OK;
}
} /* rechargeable */

namespace replaceable {
uint32_t get_id()
{
    return Replaceable::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(battery), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_bat_replacement_description(cluster, config->bat_replacement_description, sizeof(config->bat_replacement_description));
    attribute::create_bat_quantity(cluster, config->bat_quantity);

    return ESP_OK;
}
} /* replaceable */

} /* feature */

namespace attribute {
attribute_t *create_status(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Status::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_order(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Order::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_description(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_description_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound. cluster_id: 0x%08" PRIX32, power_source::Id));
    return esp_matter::attribute::create(cluster, Description::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_description_length + 1);
}

attribute_t *create_wired_assessed_input_voltage(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, WiredAssessedInputVoltage::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_wired_assessed_input_frequency(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, WiredAssessedInputFrequency::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_wired_current_type(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(wired), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, WiredCurrentType::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(1), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_wired_assessed_current(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, WiredAssessedCurrent::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_wired_nominal_voltage(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, WiredNominalVoltage::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_wired_maximum_current(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, WiredMaximumCurrent::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_wired_present(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, WiredPresent::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_active_wired_faults(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActiveWiredFaults::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_bat_voltage(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatVoltage::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_bat_percent_remaining(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatPercentRemaining::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(200)));
    return attribute;
}

attribute_t *create_bat_time_remaining(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatTimeRemaining::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_bat_charge_level(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(battery), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatChargeLevel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_bat_replacement_needed(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(battery), NULL);
    return esp_matter::attribute::create(cluster, BatReplacementNeeded::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_bat_replaceability(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(battery), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatReplaceability::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_bat_present(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, BatPresent::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_active_bat_faults(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActiveBatFaults::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_bat_replacement_description(cluster_t *cluster, char *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(replaceable), NULL);
    VerifyOrReturnValue(length <= k_max_bat_replacement_description_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound. cluster_id: 0x%08" PRIX32, power_source::Id));
    return esp_matter::attribute::create(cluster, BatReplacementDescription::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_bat_replacement_description_length + 1);
}

attribute_t *create_bat_common_designation(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatCommonDesignation::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(80), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_bat_ansi_designation(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_bat_ansi_designation_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound. cluster_id: 0x%08" PRIX32, power_source::Id));
    return esp_matter::attribute::create(cluster, BatANSIDesignation::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_bat_ansi_designation_length + 1);
}

attribute_t *create_bat_iec_designation(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_bat_iec_designation_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound. cluster_id: 0x%08" PRIX32, power_source::Id));
    return esp_matter::attribute::create(cluster, BatIECDesignation::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_bat_iec_designation_length + 1);
}

attribute_t *create_bat_approved_chemistry(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatApprovedChemistry::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(32), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_bat_capacity(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatCapacity::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_bat_quantity(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(replaceable), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatQuantity::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_bat_charge_state(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(rechargeable), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatChargeState::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_bat_time_to_full_charge(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatTimeToFullCharge::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_bat_functional_while_charging(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(rechargeable), NULL);
    return esp_matter::attribute::create(cluster, BatFunctionalWhileCharging::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_bat_charging_current(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatChargingCurrent::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_active_bat_charge_faults(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActiveBatChargeFaults::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_endpoint_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, EndpointList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */

namespace event {
event_t *create_wired_fault_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, WiredFaultChange::Id);
}

event_t *create_bat_fault_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BatFaultChange::Id);
}

event_t *create_bat_charge_fault_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BatChargeFaultChange::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, power_source::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, power_source::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterPowerSourcePluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_status(cluster, config->status);
        attribute::create_order(cluster, config->order);
        attribute::create_description(cluster, config->description, sizeof(config->description));
        attribute::create_endpoint_list(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_EXACT_ONE("Wired,Battery",
                                    feature::wired::get_id(), feature::battery::get_id());
        if (feature_map & feature::wired::get_id()) {
            VerifyOrReturnValue(feature::wired::add(cluster, &(config->features.wired)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::battery::get_id()) {
            VerifyOrReturnValue(feature::battery::add(cluster, &(config->features.battery)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::rechargeable::get_id()) {
            VerifyOrReturnValue(feature::rechargeable::add(cluster, &(config->features.rechargeable)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::replaceable::get_id()) {
            VerifyOrReturnValue(feature::replaceable::add(cluster, &(config->features.replaceable)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
    }

    return cluster;
}

} /* power_source */
} /* cluster */
} /* esp_matter */
