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

#pragma once
#include <esp_matter_data_model.h>

namespace esp_matter {
namespace cluster {
namespace thermostat {

const uint8_t k_max_active_schedule_handle_length = 16u;
namespace feature {
namespace heating {
typedef struct config {
    int16_t occupied_heating_setpoint;
    int16_t unoccupied_heating_setpoint;
    config() : occupied_heating_setpoint(2000), unoccupied_heating_setpoint(2000) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* heating */

namespace cooling {
typedef struct config {
    int16_t occupied_cooling_setpoint;
    int16_t unoccupied_cooling_setpoint;
    config() : occupied_cooling_setpoint(2600), unoccupied_cooling_setpoint(2600) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* cooling */

namespace occupancy {
typedef struct config {
    uint8_t occupancy;
    int16_t unoccupied_cooling_setpoint;
    int16_t unoccupied_heating_setpoint;
    config() : occupancy(0), unoccupied_cooling_setpoint(2600), unoccupied_heating_setpoint(2000) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* occupancy */

namespace auto_mode {
typedef struct config {
    int8_t min_setpoint_dead_band;
    config() : min_setpoint_dead_band(20) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* auto_mode */

namespace local_temperature_not_exposed {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* local_temperature_not_exposed */

namespace matter_schedule_configuration {
typedef struct config {
    uint8_t number_of_schedules;
    uint8_t number_of_schedule_transitions;
    nullable<uint8_t> number_of_schedule_transition_per_day;
    uint8_t active_schedule_handle[k_max_active_schedule_handle_length];
    config() : number_of_schedules(1), number_of_schedule_transitions(1), number_of_schedule_transition_per_day(1) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* matter_schedule_configuration */

namespace presets {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* presets */

namespace thermostat_suggestions {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* thermostat_suggestions */

} /* feature */

namespace attribute {
attribute_t *create_local_temperature(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_outdoor_temperature(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_occupancy(cluster_t *cluster, uint8_t value);
attribute_t *create_abs_min_heat_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_abs_max_heat_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_abs_min_cool_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_abs_max_cool_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_local_temperature_calibration(cluster_t *cluster, int8_t value);
attribute_t *create_occupied_cooling_setpoint(cluster_t *cluster, int16_t value);
attribute_t *create_occupied_heating_setpoint(cluster_t *cluster, int16_t value);
attribute_t *create_unoccupied_cooling_setpoint(cluster_t *cluster, int16_t value);
attribute_t *create_unoccupied_heating_setpoint(cluster_t *cluster, int16_t value);
attribute_t *create_min_heat_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_max_heat_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_min_cool_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_max_cool_setpoint_limit(cluster_t *cluster, int16_t value);
attribute_t *create_min_setpoint_dead_band(cluster_t *cluster, int8_t value);
attribute_t *create_remote_sensing(cluster_t *cluster, uint8_t value);
attribute_t *create_control_sequence_of_operation(cluster_t *cluster, uint8_t value);
attribute_t *create_system_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_temperature_setpoint_hold(cluster_t *cluster, uint8_t value);
attribute_t *create_temperature_setpoint_hold_duration(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_thermostat_running_state(cluster_t *cluster, uint16_t value);
attribute_t *create_setpoint_change_source(cluster_t *cluster, uint8_t value);
attribute_t *create_setpoint_change_amount(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_setpoint_change_source_timestamp(cluster_t *cluster, uint32_t value);
attribute_t *create_emergency_heat_delta(cluster_t *cluster, uint8_t value);
attribute_t *create_ac_type(cluster_t *cluster, uint8_t value);
attribute_t *create_ac_capacity(cluster_t *cluster, uint16_t value);
attribute_t *create_ac_refrigerant_type(cluster_t *cluster, uint8_t value);
attribute_t *create_ac_compressor_type(cluster_t *cluster, uint8_t value);
attribute_t *create_ac_error_code(cluster_t *cluster, uint32_t value);
attribute_t *create_ac_louver_position(cluster_t *cluster, uint8_t value);
attribute_t *create_ac_coil_temperature(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_ac_capacity_format(cluster_t *cluster, uint8_t value);
attribute_t *create_preset_types(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_schedule_types(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_number_of_presets(cluster_t *cluster, uint8_t value);
attribute_t *create_number_of_schedules(cluster_t *cluster, uint8_t value);
attribute_t *create_number_of_schedule_transitions(cluster_t *cluster, uint8_t value);
attribute_t *create_number_of_schedule_transition_per_day(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_active_preset_handle(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_active_schedule_handle(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_presets(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_schedules(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_setpoint_hold_expiry_timestamp(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_max_thermostat_suggestions(cluster_t *cluster, uint8_t value);
attribute_t *create_thermostat_suggestions(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_current_thermostat_suggestion(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_thermostat_suggestion_not_following_reason(cluster_t *cluster, nullable<uint16_t> value);
} /* attribute */

namespace command {
command_t *create_setpoint_raise_lower(cluster_t *cluster);
command_t *create_add_thermostat_suggestion_response(cluster_t *cluster);
command_t *create_set_active_schedule_request(cluster_t *cluster);
command_t *create_set_active_preset_request(cluster_t *cluster);
command_t *create_add_thermostat_suggestion(cluster_t *cluster);
command_t *create_remove_thermostat_suggestion(cluster_t *cluster);
} /* command */

typedef struct config {
    nullable<int16_t> local_temperature;
    uint8_t control_sequence_of_operation;
    uint8_t system_mode;
    void *delegate;
    struct {
        feature::heating::config_t heating;
        feature::cooling::config_t cooling;
        feature::occupancy::config_t occupancy;
        feature::auto_mode::config_t auto_mode;
        feature::matter_schedule_configuration::config_t matter_schedule_configuration;
    } features;
    uint32_t feature_flags;
    config() : local_temperature(0), control_sequence_of_operation(0), system_mode(0), delegate(nullptr), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* thermostat */
} /* cluster */
} /* esp_matter */
