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
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <pump_configuration_and_control.h>
#include <pump_configuration_and_control_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 5;

namespace esp_matter {
namespace cluster {
namespace pump_configuration_and_control {

namespace feature {
namespace constant_pressure {
uint32_t get_id()
{
    return ConstantPressure::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_min_const_pressure(cluster, config->min_const_pressure);
    attribute::create_max_const_pressure(cluster, config->max_const_pressure);

    return ESP_OK;
}
} /* constant_pressure */

namespace compensated_pressure {
uint32_t get_id()
{
    return CompensatedPressure::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_min_comp_pressure(cluster, config->min_comp_pressure);
    attribute::create_max_comp_pressure(cluster, config->max_comp_pressure);

    return ESP_OK;
}
} /* compensated_pressure */

namespace constant_flow {
uint32_t get_id()
{
    return ConstantFlow::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_min_const_flow(cluster, config->min_const_flow);
    attribute::create_max_const_flow(cluster, config->max_const_flow);

    return ESP_OK;
}
} /* constant_flow */

namespace constant_speed {
uint32_t get_id()
{
    return ConstantSpeed::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_min_const_speed(cluster, config->min_const_speed);
    attribute::create_max_const_speed(cluster, config->max_const_speed);

    return ESP_OK;
}
} /* constant_speed */

namespace constant_temperature {
uint32_t get_id()
{
    return ConstantTemperature::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_min_const_temp(cluster, config->min_const_temp);
    attribute::create_max_const_temp(cluster, config->max_const_temp);

    return ESP_OK;
}
} /* constant_temperature */

namespace automatic {
uint32_t get_id()
{
    return Automatic::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_min_const_pressure(cluster, config->min_const_pressure);
    attribute::create_max_const_pressure(cluster, config->max_const_pressure);
    attribute::create_min_comp_pressure(cluster, config->min_comp_pressure);
    attribute::create_max_comp_pressure(cluster, config->max_comp_pressure);
    attribute::create_min_const_speed(cluster, config->min_const_speed);
    attribute::create_max_const_speed(cluster, config->max_const_speed);
    attribute::create_min_const_flow(cluster, config->min_const_flow);
    attribute::create_max_const_flow(cluster, config->max_const_flow);
    attribute::create_min_const_temp(cluster, config->min_const_temp);
    attribute::create_max_const_temp(cluster, config->max_const_temp);

    return ESP_OK;
}
} /* automatic */

namespace local_operation {
uint32_t get_id()
{
    return LocalOperation::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* local_operation */

} /* feature */

namespace attribute {
attribute_t *create_max_pressure(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxPressure::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_max_speed(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxSpeed::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_max_flow(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxFlow::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_min_const_pressure(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinConstPressure::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_max_const_pressure(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxConstPressure::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_min_comp_pressure(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinCompPressure::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_max_comp_pressure(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxCompPressure::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_min_const_speed(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinConstSpeed::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_max_const_speed(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxConstSpeed::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_min_const_flow(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinConstFlow::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_max_const_flow(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxConstFlow::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_min_const_temp(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinConstTemp::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-27315)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_max_const_temp(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxConstTemp::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-27315)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_pump_status(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PumpStatus::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint16_t>(511), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_effective_operation_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, EffectiveOperationMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_effective_control_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, EffectiveControlMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(5), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_capacity(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Capacity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_speed(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Speed::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_lifetime_running_hours(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LifetimeRunningHours::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_power(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Power::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_lifetime_energy_consumed(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LifetimeEnergyConsumed::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_operation_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, OperationMode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_control_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ControlMode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(5), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

} /* attribute */

namespace event {
event_t *create_supply_voltage_low(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SupplyVoltageLow::Id);
}

event_t *create_supply_voltage_high(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SupplyVoltageHigh::Id);
}

event_t *create_power_missing_phase(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PowerMissingPhase::Id);
}

event_t *create_system_pressure_low(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SystemPressureLow::Id);
}

event_t *create_system_pressure_high(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SystemPressureHigh::Id);
}

event_t *create_dry_running(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DryRunning::Id);
}

event_t *create_motor_temperature_high(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, MotorTemperatureHigh::Id);
}

event_t *create_pump_motor_fatal_failure(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpMotorFatalFailure::Id);
}

event_t *create_electronic_temperature_high(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ElectronicTemperatureHigh::Id);
}

event_t *create_pump_blocked(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpBlocked::Id);
}

event_t *create_sensor_failure(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SensorFailure::Id);
}

event_t *create_electronic_non_fatal_failure(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ElectronicNonFatalFailure::Id);
}

event_t *create_electronic_fatal_failure(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ElectronicFatalFailure::Id);
}

event_t *create_general_fault(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, GeneralFault::Id);
}

event_t *create_leakage(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Leakage::Id);
}

event_t *create_air_detection(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, AirDetection::Id);
}

event_t *create_turbine_operation(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, TurbineOperation::Id);
}

} /* event */

const function_generic_t function_list[] = {
    (function_generic_t)emberAfPumpConfigurationAndControlClusterServerInitCallback,
    (function_generic_t)MatterPumpConfigurationAndControlClusterServerAttributeChangedCallback,
    (function_generic_t)MatterPumpConfigurationAndControlClusterServerPreAttributeChangedCallback,
};

const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, pump_configuration_and_control::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, pump_configuration_and_control::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterPumpConfigurationAndControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_max_pressure(cluster, config->max_pressure);
        attribute::create_max_speed(cluster, config->max_speed);
        attribute::create_max_flow(cluster, config->max_flow);
        attribute::create_effective_operation_mode(cluster, config->effective_operation_mode);
        attribute::create_effective_control_mode(cluster, config->effective_control_mode);
        attribute::create_capacity(cluster, config->capacity);
        attribute::create_operation_mode(cluster, config->operation_mode);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("ConstantPressure,ConstantTemperature,CompensatedPressure,ConstantFlow,ConstantSpeed",
                                       feature::constant_pressure::get_id(), feature::constant_temperature::get_id(), feature::compensated_pressure::get_id(), feature::constant_flow::get_id(), feature::constant_speed::get_id());
        if (feature_map & feature::constant_pressure::get_id()) {
            VerifyOrReturnValue(feature::constant_pressure::add(cluster, &(config->features.constant_pressure)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::constant_temperature::get_id()) {
            VerifyOrReturnValue(feature::constant_temperature::add(cluster, &(config->features.constant_temperature)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::compensated_pressure::get_id()) {
            VerifyOrReturnValue(feature::compensated_pressure::add(cluster, &(config->features.compensated_pressure)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::constant_flow::get_id()) {
            VerifyOrReturnValue(feature::constant_flow::add(cluster, &(config->features.constant_flow)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::constant_speed::get_id()) {
            VerifyOrReturnValue(feature::constant_speed::add(cluster, &(config->features.constant_speed)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::automatic::get_id()) {
            VerifyOrReturnValue(feature::automatic::add(cluster, &(config->features.automatic)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::local_operation::get_id()) {
            VerifyOrReturnValue(feature::local_operation::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* pump_configuration_and_control */
} /* cluster */
} /* esp_matter */
