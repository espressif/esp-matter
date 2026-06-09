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
#include <app-common/zap-generated/callback.h>
#include <app/InteractionModelEngine.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <esp_matter_delegate_callbacks.h>
#include <electrical_power_measurement.h>
#include <electrical_power_measurement_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "electrical_power_measurement_cluster";
constexpr uint16_t cluster_revision = 3;

namespace esp_matter {
namespace cluster {
namespace electrical_power_measurement {

namespace feature {
namespace direct_current {
uint32_t get_id()
{
    return DirectCurrent::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* direct_current */

namespace alternating_current {
uint32_t get_id()
{
    return AlternatingCurrent::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* alternating_current */

namespace polyphase_power {
uint32_t get_id()
{
    return PolyphasePower::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(alternating_current), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* polyphase_power */

namespace harmonics {
uint32_t get_id()
{
    return Harmonics::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(alternating_current), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_harmonic_currents(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* harmonics */

namespace power_quality {
uint32_t get_id()
{
    return PowerQuality::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(alternating_current), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_harmonic_phases(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* power_quality */

} /* feature */

namespace attribute {
attribute_t *create_power_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerMode::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_number_of_measurement_types(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, NumberOfMeasurementTypes::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(32)));
    return attribute;
}

attribute_t *create_accuracy(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Accuracy::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_ranges(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Ranges::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_voltage(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Voltage::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-4611686018427387904)), esp_matter_attr_val(nullable<int64_t>(4611686018427387904)));
    return attribute;
}

attribute_t *create_active_current(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ActiveCurrent::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-4611686018427387904)), esp_matter_attr_val(nullable<int64_t>(4611686018427387904)));
    return attribute;
}

attribute_t *create_reactive_current(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ReactiveCurrent::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-4611686018427387904)), esp_matter_attr_val(nullable<int64_t>(4611686018427387904)));
    return attribute;
}

attribute_t *create_apparent_current(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ApparentCurrent::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-4611686018427387904)), esp_matter_attr_val(nullable<int64_t>(4611686018427387904)));
    return attribute;
}

attribute_t *create_active_power(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ActivePower::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-4611686018427387904)), esp_matter_attr_val(nullable<int64_t>(4611686018427387904)));
    return attribute;
}

attribute_t *create_reactive_power(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ReactivePower::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-4611686018427387904)), esp_matter_attr_val(nullable<int64_t>(4611686018427387904)));
    return attribute;
}

attribute_t *create_apparent_power(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ApparentPower::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-4611686018427387904)), esp_matter_attr_val(nullable<int64_t>(4611686018427387904)));
    return attribute;
}

attribute_t *create_rms_voltage(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RMSVoltage::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-4611686018427387904)), esp_matter_attr_val(nullable<int64_t>(4611686018427387904)));
    return attribute;
}

attribute_t *create_rms_current(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RMSCurrent::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-4611686018427387904)), esp_matter_attr_val(nullable<int64_t>(4611686018427387904)));
    return attribute;
}

attribute_t *create_rms_power(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RMSPower::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-4611686018427387904)), esp_matter_attr_val(nullable<int64_t>(4611686018427387904)));
    return attribute;
}

attribute_t *create_frequency(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Frequency::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(0)), esp_matter_attr_val(nullable<int64_t>(1000000)));
    return attribute;
}

attribute_t *create_harmonic_currents(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(harmonics), NULL);
    return esp_matter::attribute::create(cluster, HarmonicCurrents::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_harmonic_phases(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(power_quality), NULL);
    return esp_matter::attribute::create(cluster, HarmonicPhases::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_power_factor(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerFactor::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-10000)), esp_matter_attr_val(nullable<int64_t>(10000)));
    return attribute;
}

attribute_t *create_neutral_current(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, NeutralCurrent::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(-4611686018427387904)), esp_matter_attr_val(nullable<int64_t>(4611686018427387904)));
    return attribute;
}

} /* attribute */

namespace event {
event_t *create_measurement_period_ranges(cluster_t *cluster)
{
    VerifyOrReturnValue(has_attribute(Ranges), NULL);
    return esp_matter::event::create(cluster, MeasurementPeriodRanges::Id);
}

} /* event */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, electrical_power_measurement::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, electrical_power_measurement::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ElectricalPowerMeasurementDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterElectricalPowerMeasurementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_power_mode(cluster, config->power_mode);
        attribute::create_number_of_measurement_types(cluster, config->number_of_measurement_types);
        attribute::create_active_power(cluster, config->active_power);
        attribute::create_accuracy(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("DirectCurrent,AlternatingCurrent",
                                       feature::direct_current::get_id(), feature::alternating_current::get_id());
        if (feature_map & feature::direct_current::get_id()) {
            VerifyOrReturnValue(feature::direct_current::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::alternating_current::get_id()) {
            VerifyOrReturnValue(feature::alternating_current::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::polyphase_power::get_id()) {
            VerifyOrReturnValue(feature::polyphase_power::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::harmonics::get_id()) {
            VerifyOrReturnValue(feature::harmonics::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::power_quality::get_id()) {
            VerifyOrReturnValue(feature::power_quality::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterElectricalPowerMeasurementClusterServerInitCallback,
                                                 ESPMatterElectricalPowerMeasurementClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* electrical_power_measurement */
} /* cluster */
} /* esp_matter */
