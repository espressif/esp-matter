// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include <esp_matter_core.h>
#include <esp_matter_feature.h>
#include <stdint.h>

#define CLUSTER_CONFIG_BASIC_DEFAULT()  \
    {                                   \
        .cluster_revision = 3,          \
        .node_label = {0},              \
    }

#define CLUSTER_CONFIG_BINDING_DEFAULT()  \
    {                                     \
        .cluster_revision = 1,            \
    }

#define CLUSTER_CONFIG_OTA_PROVIDER_DEFAULT() \
    {                                         \
        .cluster_revision = 1,                \
    }

#define CLUSTER_CONFIG_OTA_REQUESTOR_DEFAULT() \
    {                                          \
        .cluster_revision = 1,                 \
        .update_possible = 0,                  \
        .update_state = 0,                     \
        .update_state_progress = 0,            \
    }

#define CLUSTER_CONFIG_GENERAL_COMMISSIONING_DEFAULT() \
    {                                                  \
        .cluster_revision = 1,                         \
        .breadcrumb = 0,                               \
    }

#define CLUSTER_CONFIG_NETWORK_COMMISSIONING_DEFAULT() \
    {                                                  \
        .cluster_revision = 1,                         \
    }

#define CLUSTER_CONFIG_GENERAL_DIAGNOSTICS_DEFAULT() \
    {                                                \
        .cluster_revision = 1,                       \
    }

#define CLUSTER_CONFIG_ADMINISTRATOR_COMMISSIONING_DEFAULT() \
    {                                                        \
        .cluster_revision = 1,                               \
    }

#define CLUSTER_CONFIG_OPERATIONAL_CREDENTIALS_DEFAULT() \
    {                                                    \
        .cluster_revision = 1,                           \
    }

#define CLUSTER_CONFIG_IDENTIFY_DEFAULT() \
    {                                     \
        .cluster_revision = 2,            \
        .identify_time = 0,               \
        .identify_type = 0,               \
    }

#define CLUSTER_CONFIG_GROUPS_DEFAULT() \
    {                                   \
        .cluster_revision = 3,          \
        .group_name_support = 0,        \
    }

#define CLUSTER_CONFIG_SCENES_DEFAULT() \
    {                                   \
        .cluster_revision = 3,          \
        .scene_count = 0,               \
        .current_scene = 0,             \
        .current_group = 0,             \
        .scene_valid = false,           \
        .scene_name_support = 0,        \
    }

#define CLUSTER_CONFIG_ON_OFF_DEFAULT()                         \
    {                                                           \
        .cluster_revision = 4,                                  \
        .on_off = false,                                        \
        .lighting = ON_OFF_CLUSTER_LIGHTING_CONFIG_DEFAULT(),   \
    }

#define CLUSTER_CONFIG_LEVEL_CONTROL_DEFAULT()                          \
    {                                                                   \
        .cluster_revision = 3,                                          \
        .current_level = 0,                                             \
        .on_level = 0,                                                  \
        .options = 0,                                                   \
        .lighting = LEVEL_CONTROL_CLUSTER_LIGHTING_CONFIG_DEFAULT(),    \
    }

#define CLUSTER_CONFIG_COLOR_CONTROL_DEFAULT()                                      \
    {                                                                               \
        .cluster_revision = 3,                                                      \
        .color_mode = 1,                                                            \
        .color_control_options = 0,                                                 \
        .enhanced_color_mode = 1,                                                   \
        .color_capabilities = 0,                                                    \
        .hue_saturation = COLOR_CONTROL_CLUSTER_HUE_SATURATION_CONFIG_DEFAULT(),    \
    }

#define CLUSTER_CONFIG_FAN_CONTROL_DEFAULT() \
    {                                        \
        .cluster_revision = 3,               \
        .fan_mode = 5,                       \
        .fan_mode_sequence = 2,              \
    }

#define CLUSTER_CONFIG_THERMOSTAT_DEFAULT()  \
    {                                        \
        .cluster_revision = 3,               \
        .local_temperature = 0,              \
        .occupied_cooling_setpoint = 0x0A28, \
        .occupied_heating_setpoint = 0x07D0, \
        .control_sequence_of_operation = 4,  \
        .system_mode = 1,                    \
    }

#define CLUSTER_CONFIG_DOOR_LOCK_DEFAULT()              \
    {                                                   \
        .cluster_revision = 3,                          \
        .lock_state = 0,                                \
        .lock_type = 0,                                 \
        .actuator_enabled = 0,                          \
        .auto_relock_time = 0,                          \
        .operating_mode = 0,                            \
        .supported_operating_modes = 0,                 \
    }

#define CLUSTER_CONFIG_TIME_SYNCHRONIZATION_DEFAULT() \
    {                                                 \
        .cluster_revision = 1,                        \
    }

#define CLUSTER_CONFIG_BRIDGED_DEVICE_BASIC_DEFAULT() \
    {                                                 \
        .cluster_revision = 1,                        \
        .node_label = {0},                            \
        .reachable = true,                            \
    }

#define CLUSTER_CONFIG_FIXED_LABEL_DEFAULT() \
    {                                        \
        .cluster_revision = 1,               \
    }

#define CLUSTER_CONFIG_SWITCH_DEFAULT() \
    {                                   \
        .cluster_revision = 1,          \
        .number_of_positions = 2,       \
        .current_position = 0,          \
        .multi_press_max = 2,           \
    }

#define CLUSTER_CONFIG_TEMPERATURE_MEASUREMENT_DEFAULT() \
    {                                          \
        .cluster_revision = 3,                 \
        .measured_value = -32768,              \
        .min_measured_value = -32768,          \
        .max_measured_value = -32768,          \
    }

typedef struct esp_matter_cluster_basic_config {
    uint16_t cluster_revision;
    char node_label[32];
} esp_matter_cluster_basic_config_t;

typedef struct esp_matter_cluster_binding_config {
    uint16_t cluster_revision;
} esp_matter_cluster_binding_config_t;

typedef struct esp_matter_cluster_ota_provider_config {
    uint16_t cluster_revision;
} esp_matter_cluster_ota_provider_config_t;

typedef struct esp_matter_cluster_ota_requestor_config {
    uint16_t cluster_revision;
    bool update_possible;
    uint8_t update_state;
    uint8_t update_state_progress;
} esp_matter_cluster_ota_requestor_config_t;

typedef struct esp_matter_cluster_general_commissioning_config {
    uint16_t cluster_revision;
    uint64_t breadcrumb;
} esp_matter_cluster_general_commissioning_config_t;

typedef struct esp_matter_cluster_network_commissioning_config {
    uint16_t cluster_revision;
} esp_matter_cluster_network_commissioning_config_t;

typedef struct esp_matter_cluster_general_diagnostics_config {
    uint16_t cluster_revision;
} esp_matter_cluster_general_diagnostics_config_t;

typedef struct esp_matter_cluster_administrator_commissioning_config {
    uint16_t cluster_revision;
} esp_matter_cluster_administrator_commissioning_config_t;

typedef struct esp_matter_cluster_operational_credentials_config {
    uint16_t cluster_revision;
} esp_matter_cluster_operational_credentials_config_t;

typedef struct esp_matter_cluster_identify_config {
    uint16_t cluster_revision;
    uint16_t identify_time;
    uint8_t identify_type;
} esp_matter_cluster_identify_config_t;

typedef struct esp_matter_cluster_groups_config {
    uint16_t cluster_revision;
    uint8_t group_name_support;
} esp_matter_cluster_groups_config_t;

typedef struct esp_matter_cluster_scenes_config {
    uint16_t cluster_revision;
    uint8_t scene_count;
    uint8_t current_scene;
    uint16_t current_group;
    bool scene_valid;
    uint8_t scene_name_support;
} esp_matter_cluster_scenes_config_t;

typedef struct esp_matter_cluster_on_off_config {
    uint16_t cluster_revision;
    bool on_off;
    esp_matter_on_off_cluster_lighting_config_t lighting;
} esp_matter_cluster_on_off_config_t;

typedef struct esp_matter_cluster_level_control_config {
    uint16_t cluster_revision;
    uint8_t current_level;
    uint8_t on_level;
    uint8_t options;
    esp_matter_level_control_cluster_lighting_config_t lighting;
} esp_matter_cluster_level_control_config_t;

typedef struct esp_matter_cluster_color_control_config {
    uint16_t cluster_revision;
    uint8_t color_mode;
    uint8_t color_control_options;
    uint8_t enhanced_color_mode;
    uint16_t color_capabilities;
    esp_matter_color_control_cluster_hue_saturation_config_t hue_saturation;
} esp_matter_cluster_color_control_config_t;

typedef struct esp_matter_cluster_fan_control_config {
    uint16_t cluster_revision;
    uint8_t fan_mode;
    uint8_t fan_mode_sequence;
    /* Not implemented
    uint8_t percent_setting;
    uint8_t percent_current;
    */
} esp_matter_cluster_fan_control_config_t;

typedef struct esp_matter_cluster_door_lock_config {
    uint16_t cluster_revision;
    uint8_t lock_state;
    uint8_t lock_type;
    bool actuator_enabled;
    uint32_t auto_relock_time;
    uint8_t operating_mode;
    uint16_t supported_operating_modes;
} esp_matter_cluster_door_lock_config_t;

typedef struct esp_matter_cluster_time_synchronization_config {
    uint16_t cluster_revision;
} esp_matter_cluster_time_synchronization_config_t;

typedef struct esp_matter_cluster_thermostat_config {
    uint16_t cluster_revision;
    int16_t local_temperature;
    int16_t occupied_cooling_setpoint;
    int16_t occupied_heating_setpoint;
    uint8_t control_sequence_of_operation;
    uint8_t system_mode;
} esp_matter_cluster_thermostat_config_t;

typedef struct esp_matter_cluster_bridged_device_basic_config {
    uint16_t cluster_revision;
    char node_label[32];
    bool reachable;
} esp_matter_cluster_bridged_device_basic_config_t;

typedef struct esp_matter_cluster_fixed_label_config {
    uint16_t cluster_revision;
} esp_matter_cluster_fixed_label_config_t;

typedef struct esp_matter_cluster_switch_config {
    uint16_t cluster_revision;
    uint8_t number_of_positions;
    uint8_t current_position;
    uint8_t multi_press_max;
} esp_matter_cluster_switch_config_t;

typedef struct esp_matter_cluster_temperature_measurement_config {
    uint16_t cluster_revision;
    int16_t measured_value;
    int16_t min_measured_value;
    int16_t max_measured_value;
} esp_matter_cluster_temperature_measurement_config_t;

void esp_matter_cluster_plugin_init_callback_common();

esp_matter_cluster_t *esp_matter_cluster_create_descriptor(esp_matter_endpoint_t *endpoint, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_access_control(esp_matter_endpoint_t *endpoint, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_basic(esp_matter_endpoint_t *endpoint,
                                                      esp_matter_cluster_basic_config_t *config, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_binding(esp_matter_endpoint_t *endpoint,
                                                        esp_matter_cluster_binding_config_t *config, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_ota_provider(esp_matter_endpoint_t *endpoint,
                                                             esp_matter_cluster_ota_provider_config_t *config,
                                                             uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_ota_requestor(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_ota_requestor_config_t *config,
                                                              uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_general_commissioning(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_general_commissioning_config_t *config,
                                                            uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_network_commissioning(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_network_commissioning_config_t *config,
                                                            uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_general_diagnostics(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_general_diagnostics_config_t *config,
                                                            uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_administrator_commissioning(esp_matter_endpoint_t *endpoint,
                                                    esp_matter_cluster_administrator_commissioning_config_t *config,
                                                    uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_operational_credentials(esp_matter_endpoint_t *endpoint,
                                                        esp_matter_cluster_operational_credentials_config_t *config,
                                                        uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_group_key_management(esp_matter_endpoint_t *endpoint, uint8_t flags);

esp_matter_cluster_t *esp_matter_cluster_create_identify(esp_matter_endpoint_t *endpoint,
                                                         esp_matter_cluster_identify_config_t *config, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_groups(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_groups_config_t *config, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_scenes(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_scenes_config_t *config, uint8_t flags);

esp_matter_cluster_t *esp_matter_cluster_create_on_off(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_on_off_config_t *config, uint8_t flags,
                                                       uint32_t features);
esp_matter_cluster_t *esp_matter_cluster_create_level_control(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_level_control_config_t *config,
                                                              uint8_t flags, uint32_t features);
esp_matter_cluster_t *esp_matter_cluster_create_color_control(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_color_control_config_t *config,
                                                              uint8_t flags, uint32_t features);
esp_matter_cluster_t *esp_matter_cluster_create_fan_control(esp_matter_endpoint_t *endpoint,
                                                           esp_matter_cluster_fan_control_config_t *config,
                                                           uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_door_lock(esp_matter_endpoint_t *endpoint,
                                                          esp_matter_cluster_door_lock_config_t *config,
                                                          uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_time_synchronization(esp_matter_endpoint_t *endpoint,
                                                          esp_matter_cluster_time_synchronization_config_t *config,
                                                          uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_thermostat(esp_matter_endpoint_t *endpoint,
                                                           esp_matter_cluster_thermostat_config_t *config,
                                                           uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_bridged_device_basic(esp_matter_endpoint_t *endpoint,
                                                                     esp_matter_cluster_bridged_device_basic_config_t *config,
                                                                     uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_fixed_label(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_fixed_label_config_t *config, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_switch(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_switch_config_t *config, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_temperature_measurement(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_temperature_measurement_config_t *config,
                                                              uint8_t flags);
