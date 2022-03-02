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

#include <esp_matter_cluster.h>
#include <esp_matter_core.h>

#define ESP_MATTER_ROOT_NODE_DEVICE_TYPE_ID 0x0016
#define ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID 0x0100  
#define ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID 0x0101 
#define ESP_MATTER_COLOR_DIMMABLE_LIGHT_DEVICE_TYPE_ID 0x0102
#define ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID 0x0103
#define ESP_MATTER_THERMOSTAT_DEVICE_TYPE_ID 0x0301
#define ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_ID 0x0013
#define ESP_MATTER_FAN_DEVICE_TYPE_ID 0x002B

#define ENDPOINT_CONFIG_ROOT_NODE_DEFAULT()                                                                           \
    {                                                                                                                 \
        .descriptor = CLUSTER_CONFIG_DESCRIPTOR_DEFAULT(),                                                            \
        .access_control = CLUSTER_CONFIG_ACCESS_CONTROL_DEFAULT(),                                                    \
        .basic = CLUSTER_CONFIG_BASIC_DEFAULT(),                                                                      \
        .ota_provider = CLUSTER_CONFIG_OTA_PROVIDER_DEFAULT(),                                                        \
        .ota_requestor = CLUSTER_CONFIG_OTA_REQUESTOR_DEFAULT(),                                                      \
        .general_commissioning = CLUSTER_CONFIG_GENERAL_COMMISSIONING_DEFAULT(),                                      \
        .network_commissioning = CLUSTER_CONFIG_NETWORK_COMMISSIONING_DEFAULT(),                                      \
        .general_diagnostics = CLUSTER_CONFIG_GENERAL_DIAGNOSTICS_DEFAULT(),                                          \
        .administrator_commissioning = CLUSTER_CONFIG_ADMINISTRATOR_COMMISSIONING_DEFAULT(),                          \
        .operational_credentials = CLUSTER_CONFIG_OPERATIONAL_CREDENTIALS_DEFAULT(),                                  \
        .group_key_management = CLUSTER_CONFIG_GROUP_KEY_MANAGEMENT_DEFAULT(),                                        \
    }

#define ENDPOINT_CONFIG_ON_OFF_LIGHT_DEFAULT()                                                            \
    {                                                                                                     \
        .identify = CLUSTER_CONFIG_IDENTIFY_DEFAULT(),                                                    \
        .groups = CLUSTER_CONFIG_GROUPS_DEFAULT(),                                                        \
        .scenes = CLUSTER_CONFIG_SCENES_DEFAULT(),                                                        \
        .on_off = CLUSTER_CONFIG_ON_OFF_DEFAULT(),                                                        \
        .basic = CLUSTER_CONFIG_BASIC_DEFAULT(),                                                          \
        .descriptor = CLUSTER_CONFIG_DESCRIPTOR_DEFAULT(),                                                \
    }

#define ENDPOINT_CONFIG_DIMMABLE_LIGHT_DEFAULT()                                                          \
    {                                                                                                     \
        .identify = CLUSTER_CONFIG_IDENTIFY_DEFAULT(),                                                    \
        .groups = CLUSTER_CONFIG_GROUPS_DEFAULT(),                                                        \
        .scenes = CLUSTER_CONFIG_SCENES_DEFAULT(),                                                        \
        .on_off = CLUSTER_CONFIG_ON_OFF_DEFAULT(),                                                        \
        .level_control = CLUSTER_CONFIG_LEVEL_CONTROL_DEFAULT(),                                          \
        .basic = CLUSTER_CONFIG_BASIC_DEFAULT(),                                                          \
        .descriptor = CLUSTER_CONFIG_DESCRIPTOR_DEFAULT(),                                                \
    }

#define ENDPOINT_CONFIG_COLOR_DIMMABLE_LIGHT_DEFAULT()                                                    \
    {                                                                                                     \
        .identify = CLUSTER_CONFIG_IDENTIFY_DEFAULT(),                                                    \
        .groups = CLUSTER_CONFIG_GROUPS_DEFAULT(),                                                        \
        .scenes = CLUSTER_CONFIG_SCENES_DEFAULT(),                                                        \
        .on_off = CLUSTER_CONFIG_ON_OFF_DEFAULT(),                                                        \
        .level_control = CLUSTER_CONFIG_LEVEL_CONTROL_DEFAULT(),                                          \
        .basic = CLUSTER_CONFIG_BASIC_DEFAULT(),                                                          \
        .color_control = CLUSTER_CONFIG_COLOR_CONTROL_DEFAULT(),                                          \
        .descriptor = CLUSTER_CONFIG_DESCRIPTOR_DEFAULT(),                                                \
    }

#define ENDPOINT_CONFIG_ON_OFF_SWITCH_DEFAULT()                                                           \
    {                                                                                                     \
        .identify = CLUSTER_CONFIG_IDENTIFY_DEFAULT(),                                                    \
        .groups = CLUSTER_CONFIG_GROUPS_DEFAULT(),                                                        \
        .scenes = CLUSTER_CONFIG_SCENES_DEFAULT(),                                                        \
        .on_off = CLUSTER_CONFIG_ON_OFF_DEFAULT(),                                                        \
        .basic = CLUSTER_CONFIG_BASIC_DEFAULT(),                                                          \
        .binding = CLUSTER_CONFIG_BINDING_DEFAULT(),                                                      \
        .descriptor = CLUSTER_CONFIG_DESCRIPTOR_DEFAULT(),                                                \
    }

#define ENDPOINT_CONFIG_FAN_DEFAULT()                                                                     \
    {                                                                                                     \
        .identify = CLUSTER_CONFIG_IDENTIFY_DEFAULT(),                                                    \
        .groups = CLUSTER_CONFIG_GROUPS_DEFAULT(),                                                        \
        .descriptor = CLUSTER_CONFIG_DESCRIPTOR_DEFAULT(),                                                \
        .fan_control = CLUSTER_CONFIG_FAN_CONTROL_DEFAULT(),                                              \
    }

#define ENDPOINT_CONFIG_THERMOSTAT_DEFAULT()                                                              \
    {                                                                                                     \
        .identify = CLUSTER_CONFIG_IDENTIFY_DEFAULT(),                                                    \
        .groups = CLUSTER_CONFIG_GROUPS_DEFAULT(),                                                        \
        .scenes = CLUSTER_CONFIG_SCENES_DEFAULT(),                                                        \
        .basic = CLUSTER_CONFIG_BASIC_DEFAULT(),                                                          \
        .thermostat = CLUSTER_CONFIG_THERMOSTAT_DEFAULT(),                                                \
        .descriptor = CLUSTER_CONFIG_DESCRIPTOR_DEFAULT(),                                                \
    }

#define ENDPOINT_CONFIG_BRIDGED_NODE_DEFAULT()                                                            \
    {                                                                                                     \
        .descriptor = CLUSTER_CONFIG_DESCRIPTOR_DEFAULT(),                                                \
        .bridged_device_basic = CLUSTER_CONFIG_BRIDGED_DEVICE_BASIC_DEFAULT(),                            \
        .fixed_label = CLUSTER_CONFIG_FIXED_LABEL_DEFAULT(),                                              \
    }

#define NODE_CONFIG_DEFAULT()                             \
    {                                                     \
        .root_node = ENDPOINT_CONFIG_ROOT_NODE_DEFAULT(), \
    }

typedef struct esp_matter_endpoint_root_node_config {
    esp_matter_cluster_descriptor_config_t descriptor;
    esp_matter_cluster_access_control_config_t access_control;
    esp_matter_cluster_basic_config_t basic;
    esp_matter_cluster_ota_provider_config_t ota_provider;
    esp_matter_cluster_ota_requestor_config_t ota_requestor;
    esp_matter_cluster_general_commissioning_config_t general_commissioning;
    esp_matter_cluster_network_commissioning_config_t network_commissioning;
    esp_matter_cluster_general_diagnostics_config_t general_diagnostics;
    esp_matter_cluster_administrator_commissioning_config_t administrator_commissioning;
    esp_matter_cluster_operational_credentials_config_t operational_credentials;
    esp_matter_cluster_group_key_management_config_t group_key_management;
} esp_matter_endpoint_root_node_config_t;

typedef struct esp_matter_endpoint_on_off_light_config {
    esp_matter_cluster_identify_config_t identify;
    esp_matter_cluster_groups_config_t groups;
    esp_matter_cluster_scenes_config_t scenes;
    esp_matter_cluster_on_off_config_t on_off;
    esp_matter_cluster_basic_config_t basic;
    esp_matter_cluster_descriptor_config_t descriptor;
} esp_matter_endpoint_on_off_light_config_t;

typedef struct esp_matter_endpoint_dimmable_light_config {
    esp_matter_cluster_identify_config_t identify;
    esp_matter_cluster_groups_config_t groups;
    esp_matter_cluster_scenes_config_t scenes;
    esp_matter_cluster_on_off_config_t on_off;
    esp_matter_cluster_level_control_config_t level_control;
    esp_matter_cluster_basic_config_t basic;
    esp_matter_cluster_descriptor_config_t descriptor;
} esp_matter_endpoint_dimmable_light_config_t;

typedef struct esp_matter_endpoint_color_dimmable_light_config {
    esp_matter_cluster_identify_config_t identify;
    esp_matter_cluster_groups_config_t groups;
    esp_matter_cluster_scenes_config_t scenes;
    esp_matter_cluster_on_off_config_t on_off;
    esp_matter_cluster_level_control_config_t level_control;
    esp_matter_cluster_basic_config_t basic;
    esp_matter_cluster_color_control_config_t color_control;
    esp_matter_cluster_descriptor_config_t descriptor;
} esp_matter_endpoint_color_dimmable_light_config_t;

typedef struct esp_matter_endpoint_on_off_switch_config {
    esp_matter_cluster_identify_config_t identify;
    esp_matter_cluster_groups_config_t groups;
    esp_matter_cluster_scenes_config_t scenes;
    esp_matter_cluster_on_off_config_t on_off;
    esp_matter_cluster_basic_config_t basic;
    esp_matter_cluster_binding_config_t binding;
    esp_matter_cluster_descriptor_config_t descriptor;
} esp_matter_endpoint_on_off_switch_config_t;

typedef struct esp_matter_endpoint_fan_config {
    esp_matter_cluster_identify_config_t identify;
    esp_matter_cluster_groups_config_t groups;
    esp_matter_cluster_descriptor_config_t descriptor;
    esp_matter_cluster_fan_control_config_t fan_control;
} esp_matter_endpoint_fan_config_t;

typedef struct esp_matter_endpoint_thermostat_config {
    esp_matter_cluster_identify_config_t identify;
    esp_matter_cluster_groups_config_t groups;
    esp_matter_cluster_scenes_config_t scenes;
    esp_matter_cluster_basic_config_t basic;
    esp_matter_cluster_thermostat_config_t thermostat;
    esp_matter_cluster_descriptor_config_t descriptor;
} esp_matter_endpoint_thermostat_config_t;

typedef struct esp_matter_endpoint_bridged_node_config {
    esp_matter_cluster_descriptor_config_t descriptor;
    esp_matter_cluster_bridged_device_basic_config_t bridged_device_basic;
    esp_matter_cluster_fixed_label_config_t fixed_label;
} esp_matter_endpoint_bridged_node_config_t;

typedef struct esp_matter_node_config {
    esp_matter_endpoint_root_node_config_t root_node;
} esp_matter_node_config_t;

esp_matter_endpoint_t *esp_matter_endpoint_create_root_node(esp_matter_node_t *node,
                                                            esp_matter_endpoint_root_node_config_t *config,
                                                            uint8_t flags);
esp_matter_endpoint_t *esp_matter_endpoint_create_on_off_light(esp_matter_node_t *node,
                                                               esp_matter_endpoint_on_off_light_config_t *config,
                                                               uint8_t flags);
esp_matter_endpoint_t *esp_matter_endpoint_create_dimmable_light(esp_matter_node_t *node,
                                                                 esp_matter_endpoint_dimmable_light_config_t *config,
                                                                 uint8_t flags);
esp_matter_endpoint_t *esp_matter_endpoint_create_color_dimmable_light(esp_matter_node_t *node,
                                                            esp_matter_endpoint_color_dimmable_light_config_t *config,
                                                            uint8_t flags);
esp_matter_endpoint_t *esp_matter_endpoint_create_on_off_switch(esp_matter_node_t *node,
                                                                esp_matter_endpoint_on_off_switch_config_t *config,
                                                                uint8_t flags);
esp_matter_endpoint_t *esp_matter_endpoint_create_fan(esp_matter_node_t *node,
                                                      esp_matter_endpoint_fan_config_t *config,
                                                      uint8_t flags);
esp_matter_endpoint_t *esp_matter_endpoint_create_thermostat(esp_matter_node_t *node,
                                                             esp_matter_endpoint_thermostat_config_t *config,
                                                             uint8_t flags);
esp_matter_endpoint_t *esp_matter_endpoint_create_bridged_node(esp_matter_node_t *node,
                                                            esp_matter_endpoint_bridged_node_config_t *config,
                                                            uint8_t flags);

esp_matter_node_t *esp_matter_node_create(esp_matter_node_config_t *config, esp_matter_attribute_callback_t callback,
                                          void *priv_data);
