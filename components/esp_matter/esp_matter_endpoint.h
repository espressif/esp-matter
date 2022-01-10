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

#define ESP_MATTER_ROOT_NODE_ENDPOINT_ID 0x0000
#define ESP_MATTER_COLOR_DIMMABLE_LIGHT_ENDPOINT_ID 0x0001

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

#define ENDPOINT_CONFIG_COLOR_DIMMABLE_LIGHT_DEFAULT()                                                    \
    {                                                                                                     \
        .identify = CLUSTER_CONFIG_IDENTIFY_DEFAULT(),                                                    \
        .groups = CLUSTER_CONFIG_GROUPS_DEFAULT(),                                                        \
        .scenes = CLUSTER_CONFIG_SCENES_DEFAULT(),                                                        \
        .on_off = CLUSTER_CONFIG_ON_OFF_DEFAULT(),                                                        \
        .level_control = CLUSTER_CONFIG_LEVEL_CONTROL_DEFAULT(),                                          \
        .basic = CLUSTER_CONFIG_BASIC_DEFAULT(),                                                          \
        .color_control = CLUSTER_CONFIG_COLOR_CONTROL_DEFAULT(),                                          \
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

typedef struct esp_matter_endpoint_color_dimmable_light_config {
    esp_matter_cluster_identify_config_t identify;
    esp_matter_cluster_groups_config_t groups;
    esp_matter_cluster_scenes_config_t scenes;
    esp_matter_cluster_on_off_config_t on_off;
    esp_matter_cluster_level_control_config_t level_control;
    esp_matter_cluster_basic_config_t basic;
    esp_matter_cluster_color_control_config_t color_control;
} esp_matter_endpoint_color_dimmable_light_config_t;

typedef struct esp_matter_node_config {
    esp_matter_endpoint_root_node_config_t root_node;
} esp_matter_node_config_t;

esp_matter_endpoint_t *esp_matter_endpoint_create_root_node(esp_matter_node_t *node,
                                                            esp_matter_endpoint_root_node_config_t *config);
esp_matter_endpoint_t *esp_matter_endpoint_create_color_dimmable_light(esp_matter_node_t *node,
                                                            esp_matter_endpoint_color_dimmable_light_config_t *config);

esp_matter_node_t *esp_matter_node_create(esp_matter_node_config_t *config, esp_matter_attribute_callback_t callback,
                                          void *priv_data);
