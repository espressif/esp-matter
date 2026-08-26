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
namespace pump_configuration_and_control {

namespace feature {
namespace constant_pressure {
typedef struct config {
    nullable<int16_t> min_const_pressure;
    nullable<int16_t> max_const_pressure;
    config() : min_const_pressure(), max_const_pressure() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* constant_pressure */

namespace compensated_pressure {
typedef struct config {
    nullable<int16_t> min_comp_pressure;
    nullable<int16_t> max_comp_pressure;
    config() : min_comp_pressure(), max_comp_pressure() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* compensated_pressure */

namespace constant_flow {
typedef struct config {
    nullable<uint16_t> min_const_flow;
    nullable<uint16_t> max_const_flow;
    config() : min_const_flow(), max_const_flow() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* constant_flow */

namespace constant_speed {
typedef struct config {
    nullable<uint16_t> min_const_speed;
    nullable<uint16_t> max_const_speed;
    config() : min_const_speed(), max_const_speed() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* constant_speed */

namespace constant_temperature {
typedef struct config {
    nullable<int16_t> min_const_temp;
    nullable<int16_t> max_const_temp;
    config() : min_const_temp(), max_const_temp() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* constant_temperature */

namespace automatic {
typedef struct config {
    nullable<int16_t> min_const_pressure;
    nullable<int16_t> max_const_pressure;
    nullable<int16_t> min_comp_pressure;
    nullable<int16_t> max_comp_pressure;
    nullable<uint16_t> min_const_speed;
    nullable<uint16_t> max_const_speed;
    nullable<uint16_t> min_const_flow;
    nullable<uint16_t> max_const_flow;
    nullable<int16_t> min_const_temp;
    nullable<int16_t> max_const_temp;
    config() : min_const_pressure(), max_const_pressure(), min_comp_pressure(), max_comp_pressure(), min_const_speed(), max_const_speed(), min_const_flow(), max_const_flow(), min_const_temp(), max_const_temp() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* automatic */

namespace local_operation {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* local_operation */

} /* feature */

namespace attribute {
attribute_t *create_max_pressure(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_max_speed(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_max_flow(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_min_const_pressure(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_max_const_pressure(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_min_comp_pressure(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_max_comp_pressure(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_min_const_speed(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_max_const_speed(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_min_const_flow(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_max_const_flow(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_min_const_temp(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_max_const_temp(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_pump_status(cluster_t *cluster, uint16_t value);
attribute_t *create_effective_operation_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_effective_control_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_capacity(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_speed(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_lifetime_running_hours(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_power(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_lifetime_energy_consumed(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_operation_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_control_mode(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace event {
event_t *create_supply_voltage_low(cluster_t *cluster);
event_t *create_supply_voltage_high(cluster_t *cluster);
event_t *create_power_missing_phase(cluster_t *cluster);
event_t *create_system_pressure_low(cluster_t *cluster);
event_t *create_system_pressure_high(cluster_t *cluster);
event_t *create_dry_running(cluster_t *cluster);
event_t *create_motor_temperature_high(cluster_t *cluster);
event_t *create_pump_motor_fatal_failure(cluster_t *cluster);
event_t *create_electronic_temperature_high(cluster_t *cluster);
event_t *create_pump_blocked(cluster_t *cluster);
event_t *create_sensor_failure(cluster_t *cluster);
event_t *create_electronic_non_fatal_failure(cluster_t *cluster);
event_t *create_electronic_fatal_failure(cluster_t *cluster);
event_t *create_general_fault(cluster_t *cluster);
event_t *create_leakage(cluster_t *cluster);
event_t *create_air_detection(cluster_t *cluster);
event_t *create_turbine_operation(cluster_t *cluster);
} /* event */

typedef struct config {
    nullable<int16_t> max_pressure;
    nullable<uint16_t> max_speed;
    nullable<uint16_t> max_flow;
    uint8_t effective_operation_mode;
    uint8_t effective_control_mode;
    nullable<int16_t> capacity;
    uint8_t operation_mode;
    struct {
        feature::constant_pressure::config_t constant_pressure;
        feature::compensated_pressure::config_t compensated_pressure;
        feature::constant_flow::config_t constant_flow;
        feature::constant_speed::config_t constant_speed;
        feature::constant_temperature::config_t constant_temperature;
        feature::automatic::config_t automatic;
    } features;
    uint32_t feature_flags;
    config() : max_pressure(), max_speed(), max_flow(), effective_operation_mode(0), effective_control_mode(0), capacity(), operation_mode(0), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* pump_configuration_and_control */
} /* cluster */
} /* esp_matter */
