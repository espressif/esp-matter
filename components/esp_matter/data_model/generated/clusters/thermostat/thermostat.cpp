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
#include <esp_matter_delegate_callbacks.h>
#include <thermostat.h>
#include <thermostat_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 11;

static esp_err_t esp_matter_command_callback_setpoint_raise_lower(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                  void *opaque_ptr)
{
    chip::app::Clusters::Thermostat::Commands::SetpointRaiseLower::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterSetpointRaiseLowerCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_active_schedule_request(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                         void *opaque_ptr)
{
    chip::app::Clusters::Thermostat::Commands::SetActiveScheduleRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterSetActiveScheduleRequestCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_active_preset_request(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                       void *opaque_ptr)
{
    chip::app::Clusters::Thermostat::Commands::SetActivePresetRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterSetActivePresetRequestCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_thermostat_suggestion(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                       void *opaque_ptr)
{
    chip::app::Clusters::Thermostat::Commands::AddThermostatSuggestion::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterAddThermostatSuggestionCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_thermostat_suggestion(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                          void *opaque_ptr)
{
    chip::app::Clusters::Thermostat::Commands::RemoveThermostatSuggestion::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterRemoveThermostatSuggestionCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace thermostat {

namespace feature {
namespace heating {
uint32_t get_id()
{
    return Heating::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_occupied_heating_setpoint(cluster, config->occupied_heating_setpoint);
    attribute::create_unoccupied_heating_setpoint(cluster, config->unoccupied_heating_setpoint);

    return ESP_OK;
}
} /* heating */

namespace cooling {
uint32_t get_id()
{
    return Cooling::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_occupied_cooling_setpoint(cluster, config->occupied_cooling_setpoint);
    attribute::create_unoccupied_cooling_setpoint(cluster, config->unoccupied_cooling_setpoint);

    return ESP_OK;
}
} /* cooling */

namespace occupancy {
uint32_t get_id()
{
    return Occupancy::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_occupancy(cluster, config->occupancy);
    attribute::create_unoccupied_cooling_setpoint(cluster, config->unoccupied_cooling_setpoint);
    attribute::create_unoccupied_heating_setpoint(cluster, config->unoccupied_heating_setpoint);

    return ESP_OK;
}
} /* occupancy */

namespace auto_mode {
uint32_t get_id()
{
    return AutoMode::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_min_setpoint_dead_band(cluster, config->min_setpoint_dead_band);

    return ESP_OK;
}
} /* auto_mode */

namespace local_temperature_not_exposed {
uint32_t get_id()
{
    return LocalTemperatureNotExposed::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* local_temperature_not_exposed */

namespace matter_schedule_configuration {
uint32_t get_id()
{
    return MatterScheduleConfiguration::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_number_of_schedules(cluster, config->number_of_schedules);
    attribute::create_number_of_schedule_transitions(cluster, config->number_of_schedule_transitions);
    attribute::create_number_of_schedule_transition_per_day(cluster, config->number_of_schedule_transition_per_day);
    attribute::create_active_schedule_handle(cluster, config->active_schedule_handle, sizeof(config->active_schedule_handle));
    attribute::create_schedule_types(cluster, NULL, 0, 0);
    attribute::create_schedules(cluster, NULL, 0, 0);
    command::create_set_active_schedule_request(cluster);

    return ESP_OK;
}
} /* matter_schedule_configuration */

namespace presets {
uint32_t get_id()
{
    return Presets::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_preset_types(cluster, NULL, 0, 0);
    attribute::create_number_of_presets(cluster, 0);
    attribute::create_active_preset_handle(cluster, NULL, 0);
    attribute::create_presets(cluster, NULL, 0, 0);
    command::create_set_active_preset_request(cluster);

    return ESP_OK;
}
} /* presets */

namespace thermostat_suggestions {
uint32_t get_id()
{
    return ThermostatSuggestions::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(presets), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_max_thermostat_suggestions(cluster, 0);
    attribute::create_thermostat_suggestions(cluster, NULL, 0, 0);
    attribute::create_current_thermostat_suggestion(cluster, NULL, 0, 0);
    attribute::create_thermostat_suggestion_not_following_reason(cluster, 0);
    command::create_add_thermostat_suggestion_response(cluster);
    command::create_add_thermostat_suggestion(cluster);
    command::create_remove_thermostat_suggestion(cluster);

    return ESP_OK;
}
} /* thermostat_suggestions */

} /* feature */

namespace attribute {
attribute_t *create_local_temperature(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LocalTemperature::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_outdoor_temperature(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, OutdoorTemperature::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_occupancy(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(occupancy), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, Occupancy::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(1), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_abs_min_heat_setpoint_limit(cluster_t *cluster, int16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AbsMinHeatSetpointLimit::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_abs_max_heat_setpoint_limit(cluster_t *cluster, int16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AbsMaxHeatSetpointLimit::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_abs_min_cool_setpoint_limit(cluster_t *cluster, int16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AbsMinCoolSetpointLimit::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_abs_max_cool_setpoint_limit(cluster_t *cluster, int16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AbsMaxCoolSetpointLimit::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_local_temperature_calibration(cluster_t *cluster, int8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LocalTemperatureCalibration::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int8_t>(-128)), esp_matter_attr_val(static_cast<int8_t>(126)));
    return attribute;
}

attribute_t *create_occupied_cooling_setpoint(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(cooling), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, OccupiedCoolingSetpoint::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_occupied_heating_setpoint(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(heating), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, OccupiedHeatingSetpoint::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_unoccupied_cooling_setpoint(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(cooling)) && (has_feature(occupancy))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, UnoccupiedCoolingSetpoint::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_unoccupied_heating_setpoint(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(heating)) && (has_feature(occupancy))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, UnoccupiedHeatingSetpoint::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_min_heat_setpoint_limit(cluster_t *cluster, int16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinHeatSetpointLimit::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_max_heat_setpoint_limit(cluster_t *cluster, int16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxHeatSetpointLimit::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_min_cool_setpoint_limit(cluster_t *cluster, int16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinCoolSetpointLimit::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_max_cool_setpoint_limit(cluster_t *cluster, int16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxCoolSetpointLimit::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(-32768)), esp_matter_attr_val(static_cast<int16_t>(32766)));
    return attribute;
}

attribute_t *create_min_setpoint_dead_band(cluster_t *cluster, int8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(auto_mode), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinSetpointDeadBand::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int8_t>(0)), esp_matter_attr_val(static_cast<int8_t>(127)));
    return attribute;
}

attribute_t *create_remote_sensing(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RemoteSensing::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(7), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_control_sequence_of_operation(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ControlSequenceOfOperation::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(5), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_system_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SystemMode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(8), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_temperature_setpoint_hold(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TemperatureSetpointHold::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(1), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_temperature_setpoint_hold_duration(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TemperatureSetpointHoldDuration::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(1440)));
    return attribute;
}

attribute_t *create_thermostat_running_state(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ThermostatRunningState::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint16_t>(65535), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_setpoint_change_source(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SetpointChangeSource::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_setpoint_change_amount(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SetpointChangeAmount::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_setpoint_change_source_timestamp(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SetpointChangeSourceTimestamp::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_emergency_heat_delta(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, EmergencyHeatDelta::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_ac_type(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ACType::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(4), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_ac_capacity(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ACCapacity::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_ac_refrigerant_type(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ACRefrigerantType::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_ac_compressor_type(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ACCompressorType::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_ac_error_code(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ACErrorCode::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(4294967295), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_ac_louver_position(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ACLouverPosition::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(4), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_ac_coil_temperature(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ACCoilTemperature::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_ac_capacity_format(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ACCapacityFormat::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_preset_types(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(presets), NULL);
    return esp_matter::attribute::create(cluster, PresetTypes::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_schedule_types(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(matter_schedule_configuration), NULL);
    return esp_matter::attribute::create(cluster, ScheduleTypes::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_number_of_presets(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(presets), NULL);
    return esp_matter::attribute::create(cluster, NumberOfPresets::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

attribute_t *create_number_of_schedules(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(matter_schedule_configuration), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NumberOfSchedules::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(1)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_number_of_schedule_transitions(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(matter_schedule_configuration), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NumberOfScheduleTransitions::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(1)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_number_of_schedule_transition_per_day(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(matter_schedule_configuration), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NumberOfScheduleTransitionPerDay::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(1)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_active_preset_handle(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(presets), NULL);
    return esp_matter::attribute::create(cluster, ActivePresetHandle::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length));
}

attribute_t *create_active_schedule_handle(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(matter_schedule_configuration), NULL);
    VerifyOrReturnValue(length <= k_max_active_schedule_handle_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound. cluster_id: 0x%08" PRIX32, thermostat::Id));
    return esp_matter::attribute::create(cluster, ActiveScheduleHandle::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length));
}

attribute_t *create_presets(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(presets), NULL);
    return esp_matter::attribute::create(cluster, Presets::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_schedules(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(matter_schedule_configuration), NULL);
    return esp_matter::attribute::create(cluster, Schedules::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_setpoint_hold_expiry_timestamp(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SetpointHoldExpiryTimestamp::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_max_thermostat_suggestions(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(thermostat_suggestions), NULL);
    return esp_matter::attribute::create(cluster, MaxThermostatSuggestions::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

attribute_t *create_thermostat_suggestions(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(thermostat_suggestions), NULL);
    return esp_matter::attribute::create(cluster, ThermostatSuggestions::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_thermostat_suggestion(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(thermostat_suggestions), NULL);
    return esp_matter::attribute::create(cluster, CurrentThermostatSuggestion::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_thermostat_suggestion_not_following_reason(cluster_t *cluster, nullable<uint16_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(thermostat_suggestions), NULL);
    return esp_matter::attribute::create(cluster, ThermostatSuggestionNotFollowingReason::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
}

} /* attribute */
namespace command {
command_t *create_setpoint_raise_lower(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetpointRaiseLower::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_setpoint_raise_lower);
}

command_t *create_add_thermostat_suggestion_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(thermostat_suggestions), NULL);
    return esp_matter::command::create(cluster, AddThermostatSuggestionResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_set_active_schedule_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(matter_schedule_configuration), NULL);
    return esp_matter::command::create(cluster, SetActiveScheduleRequest::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_active_schedule_request);
}

command_t *create_set_active_preset_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(presets), NULL);
    return esp_matter::command::create(cluster, SetActivePresetRequest::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_active_preset_request);
}

command_t *create_add_thermostat_suggestion(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(thermostat_suggestions), NULL);
    return esp_matter::command::create(cluster, AddThermostatSuggestion::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_thermostat_suggestion);
}

command_t *create_remove_thermostat_suggestion(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(thermostat_suggestions), NULL);
    return esp_matter::command::create(cluster, RemoveThermostatSuggestion::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_thermostat_suggestion);
}

} /* command */

const function_generic_t function_list[] = {
    (function_generic_t)emberAfThermostatClusterServerInitCallback,
    (function_generic_t)MatterThermostatClusterServerAttributeChangedCallback,
    (function_generic_t)MatterThermostatClusterServerShutdownCallback,
    (function_generic_t)MatterThermostatClusterServerPreAttributeChangedCallback,
};

const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, thermostat::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, thermostat::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ThermostatDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterThermostatPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_local_temperature(cluster, config->local_temperature);
        attribute::create_control_sequence_of_operation(cluster, config->control_sequence_of_operation);
        attribute::create_system_mode(cluster, config->system_mode);

        uint32_t feature_map = config->feature_flags;
        if (feature_map & feature::auto_mode::get_id()) {
            VerifyOrReturnValue(feature::auto_mode::add(cluster, &(config->features.auto_mode)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            VerifyOrReturnValue(feature::heating::add(cluster, &(config->features.heating)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            VerifyOrReturnValue(feature::cooling::add(cluster, &(config->features.cooling)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        } else {
            VALIDATE_FEATURES_AT_LEAST_ONE("Heating,Cooling",
                                           feature::heating::get_id(), feature::cooling::get_id());
            if (feature_map & feature::heating::get_id()) {
                VerifyOrReturnValue(feature::heating::add(cluster, &(config->features.heating)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            }
            if (feature_map & feature::cooling::get_id()) {
                VerifyOrReturnValue(feature::cooling::add(cluster, &(config->features.cooling)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            }
        }
        if (feature_map & feature::occupancy::get_id()) {
            VerifyOrReturnValue(feature::occupancy::add(cluster, &(config->features.occupancy)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::local_temperature_not_exposed::get_id()) {
            VerifyOrReturnValue(feature::local_temperature_not_exposed::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::matter_schedule_configuration::get_id()) {
            VerifyOrReturnValue(feature::matter_schedule_configuration::add(cluster, &(config->features.matter_schedule_configuration)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::presets::get_id()) {
            VerifyOrReturnValue(feature::presets::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::thermostat_suggestions::get_id()) {
            VerifyOrReturnValue(feature::thermostat_suggestions::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_setpoint_raise_lower(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* thermostat */
} /* cluster */
} /* esp_matter */
