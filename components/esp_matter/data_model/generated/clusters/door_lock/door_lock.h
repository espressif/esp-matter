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
namespace door_lock {

const uint8_t k_max_language_length = 3u;
namespace feature {
namespace pin_credential {
typedef struct config {
    uint16_t number_of_pin_users_supported;
    uint8_t max_pin_code_length;
    uint8_t min_pin_code_length;
    bool require_pin_for_remote_operation;
    config() : number_of_pin_users_supported(0), max_pin_code_length(0), min_pin_code_length(0), require_pin_for_remote_operation(false) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* pin_credential */

namespace weekday_access_schedules {
typedef struct config {
    uint8_t number_of_week_day_schedules_supported_per_user;
    config() : number_of_week_day_schedules_supported_per_user(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* weekday_access_schedules */

namespace door_position_sensor {
typedef struct config {
    nullable<uint8_t> door_state;
    config() : door_state() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* door_position_sensor */

namespace credential_over_the_air_access {
typedef struct config {
    bool require_pin_for_remote_operation;
    config() : require_pin_for_remote_operation(false) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* credential_over_the_air_access */

namespace year_day_access_schedules {
typedef struct config {
    uint8_t number_of_year_day_schedules_supported_per_user;
    config() : number_of_year_day_schedules_supported_per_user(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* year_day_access_schedules */

namespace holiday_schedules {
typedef struct config {
    uint8_t number_of_holiday_schedules_supported;
    config() : number_of_holiday_schedules_supported(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* holiday_schedules */

namespace unbolting {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* unbolting */

namespace aliro_provisioning {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* aliro_provisioning */

namespace aliro_bleuwb {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* aliro_bleuwb */

} /* feature */

namespace attribute {
attribute_t *create_lock_state(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_lock_type(cluster_t *cluster, uint8_t value);
attribute_t *create_actuator_enabled(cluster_t *cluster, bool value);
attribute_t *create_door_state(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_door_open_events(cluster_t *cluster, uint32_t value);
attribute_t *create_door_closed_events(cluster_t *cluster, uint32_t value);
attribute_t *create_open_period(cluster_t *cluster, uint16_t value);
attribute_t *create_number_of_pin_users_supported(cluster_t *cluster, uint16_t value);
attribute_t *create_number_of_week_day_schedules_supported_per_user(cluster_t *cluster, uint8_t value);
attribute_t *create_number_of_year_day_schedules_supported_per_user(cluster_t *cluster, uint8_t value);
attribute_t *create_number_of_holiday_schedules_supported(cluster_t *cluster, uint8_t value);
attribute_t *create_max_pin_code_length(cluster_t *cluster, uint8_t value);
attribute_t *create_min_pin_code_length(cluster_t *cluster, uint8_t value);
attribute_t *create_language(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_led_settings(cluster_t *cluster, uint8_t value);
attribute_t *create_auto_relock_time(cluster_t *cluster, uint32_t value);
attribute_t *create_sound_volume(cluster_t *cluster, uint8_t value);
attribute_t *create_operating_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_supported_operating_modes(cluster_t *cluster, uint16_t value);
attribute_t *create_default_configuration_register(cluster_t *cluster, uint16_t value);
attribute_t *create_enable_local_programming(cluster_t *cluster, bool value);
attribute_t *create_enable_one_touch_locking(cluster_t *cluster, bool value);
attribute_t *create_enable_inside_status_led(cluster_t *cluster, bool value);
attribute_t *create_enable_privacy_mode_button(cluster_t *cluster, bool value);
attribute_t *create_local_programming_features(cluster_t *cluster, uint8_t value);
attribute_t *create_require_pin_for_remote_operation(cluster_t *cluster, bool value);
attribute_t *create_aliro_reader_verification_key(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_aliro_reader_group_identifier(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_aliro_reader_group_sub_identifier(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_aliro_expedited_transaction_supported_protocol_versions(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_aliro_group_resolving_key(cluster_t *cluster, uint8_t * value, uint16_t length);
attribute_t *create_aliro_supported_bleuwb_protocol_versions(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_aliro_ble_advertising_version(cluster_t *cluster, uint8_t value);
attribute_t *create_number_of_aliro_credential_issuer_keys_supported(cluster_t *cluster, uint16_t value);
attribute_t *create_number_of_aliro_endpoint_keys_supported(cluster_t *cluster, uint16_t value);
} /* attribute */

namespace command {
command_t *create_lock_door(cluster_t *cluster);
command_t *create_unlock_door(cluster_t *cluster);
command_t *create_unlock_with_timeout(cluster_t *cluster);
command_t *create_set_week_day_schedule(cluster_t *cluster);
command_t *create_get_week_day_schedule(cluster_t *cluster);
command_t *create_get_week_day_schedule_response(cluster_t *cluster);
command_t *create_clear_week_day_schedule(cluster_t *cluster);
command_t *create_set_year_day_schedule(cluster_t *cluster);
command_t *create_get_year_day_schedule(cluster_t *cluster);
command_t *create_get_year_day_schedule_response(cluster_t *cluster);
command_t *create_clear_year_day_schedule(cluster_t *cluster);
command_t *create_set_holiday_schedule(cluster_t *cluster);
command_t *create_get_holiday_schedule(cluster_t *cluster);
command_t *create_get_holiday_schedule_response(cluster_t *cluster);
command_t *create_clear_holiday_schedule(cluster_t *cluster);
command_t *create_unbolt_door(cluster_t *cluster);
command_t *create_set_aliro_reader_config(cluster_t *cluster);
command_t *create_clear_aliro_reader_config(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_door_lock_alarm(cluster_t *cluster);
event_t *create_door_state_change(cluster_t *cluster);
event_t *create_lock_operation(cluster_t *cluster);
event_t *create_lock_operation_error(cluster_t *cluster);
} /* event */

typedef struct config {
    nullable<uint8_t> lock_state;
    uint8_t lock_type;
    bool actuator_enabled;
    uint8_t operating_mode;
    uint16_t supported_operating_modes;
    void *delegate;
    config() : lock_state(), lock_type(0), actuator_enabled(false), operating_mode(0), supported_operating_modes(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* door_lock */
} /* cluster */
} /* esp_matter */
