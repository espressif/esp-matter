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
#include <stdint.h>

#define CLUSTER_CONFIG_DESCRIPTOR_DEFAULT() \
    {                                       \
        .cluster_revision = 1,              \
        .device_type_list = {0},            \
        .server_list = {0},                 \
        .client_list = {0},                 \
        .parts_list = {0},                  \
    }

#define CLUSTER_CONFIG_ACCESS_CONTROL_DEFAULT() \
    {                                           \
        .cluster_revision = 1,                  \
        .acl = {0},                             \
        .extension = {0},                       \
    }

#define CLUSTER_CONFIG_BASIC_DEFAULT()  \
    {                                   \
        .cluster_revision = 3,          \
        .interaction_model_version = 1, \
        .vendor_name = {0},             \
        .vendor_id = 0,                 \
        .product_name = {0},            \
        .product_id = 0,                \
        .node_label = {0},              \
        .location = {0},                \
        .hardware_version = 0,          \
        .hardware_version_string = {0}, \
        .software_version = 0,          \
        .software_version_string = {0}, \
        .serial_number = {0},           \
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
        .default_ota_providers = {0},          \
        .update_possible = 0,                  \
        .update_state = 0,                     \
        .update_state_progress = 0,            \
    }

#define CLUSTER_CONFIG_GENERAL_COMMISSIONING_DEFAULT() \
    {                                                  \
        .cluster_revision = 1,                         \
        .breadcrumb = 0,                               \
        .basic_commissioning_info = {0},               \
        .regulatory_config = 0,                        \
        .location_capability = 0,                      \
    }

#define CLUSTER_CONFIG_NETWORK_COMMISSIONING_DEFAULT() \
    {                                                  \
        .cluster_revision = 1,                         \
        .feature_map = 1,                              \
        .max_networks = 1,                             \
        .networks = {0},                               \
        .scan_max_time_seconds = 0,                    \
        .connect_max_time_seconds = 0,                 \
        .interface_enabled = 0,                        \
        .last_networking_status = 0,                   \
        .last_network_id = {0},                        \
        .last_connect_error_value = 0,                 \
    }

#define CLUSTER_CONFIG_GENERAL_DIAGNOSTICS_DEFAULT() \
    {                                                \
        .cluster_revision = 1,                       \
        .network_interfaces = {0},                   \
        .reboot_count = 0,                           \
    }

#define CLUSTER_CONFIG_ADMINISTRATOR_COMMISSIONING_DEFAULT() \
    {                                                        \
        .cluster_revision = 1,                               \
        .window_status = 0,                                  \
        .admin_fabric_index = 0,                             \
        .admin_vendor_id = 0,                                \
    }

#define CLUSTER_CONFIG_OPERATIONAL_CREDENTIALS_DEFAULT() \
    {                                                    \
        .cluster_revision = 1,                           \
        .nocs = {0},                                     \
        .fabrics = {0},                                  \
        .supported_fabrics = 5,                          \
        .commissioned_fabrics = 0,                       \
        .trusted_root_certificates = {0},                \
        .current_fabric_index = 0,                       \
    }

#define CLUSTER_CONFIG_GROUP_KEY_MANAGEMENT_DEFAULT() \
    {                                                 \
        .cluster_revision = 1,                        \
        .group_key_map = {0},                         \
        .group_table = {0},                           \
        .max_groups_per_fabric = 0,                   \
        .max_group_keys_per_fabric = 1,               \
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
        .name_support = 0,              \
    }

#define CLUSTER_CONFIG_SCENES_DEFAULT() \
    {                                   \
        .cluster_revision = 3,          \
        .scene_count = 0,               \
        .current_scene = 0,             \
        .current_group = 0,             \
        .scene_valid = false,           \
        .name_support = 0,              \
    }

#define CLUSTER_CONFIG_ON_OFF_DEFAULT() \
    {                                   \
        .cluster_revision = 4,          \
        .on_off = false,                \
    }

#define CLUSTER_CONFIG_LEVEL_CONTROL_DEFAULT() \
    {                                          \
        .cluster_revision = 3,                 \
        .current_level = 0,                    \
        .on_level = 0,                         \
        .options = 0,                          \
    }

#define CLUSTER_CONFIG_COLOR_CONTROL_DEFAULT() \
    {                                          \
        .cluster_revision = 3,                 \
        .feature_map = 1,                      \
        .current_hue = 0,                      \
        .current_saturation = 0,               \
        .color_mode = 1,                       \
        .options = 0,                          \
        .enhanced_color_mode = 1,              \
        .color_capabilities = 0,               \
    }

typedef struct esp_matter_cluster_descriptor_config {
    uint16_t cluster_revision;
    uint8_t device_type_list[254];
    uint8_t server_list[254];
    uint8_t client_list[254];
    uint8_t parts_list[254];
} esp_matter_cluster_descriptor_config_t;

typedef struct esp_matter_cluster_access_control_config {
    uint16_t cluster_revision;
    uint8_t acl[254];
    uint8_t extension[254];
} esp_matter_cluster_access_control_config_t;

typedef struct esp_matter_cluster_basic_config {
    uint16_t cluster_revision;
    uint16_t interaction_model_version;
    char vendor_name[32];
    uint16_t vendor_id;
    char product_name[32];
    uint16_t product_id;
    char node_label[32];
    char location[2];
    uint16_t hardware_version;
    char hardware_version_string[64];
    uint32_t software_version;
    char software_version_string[64];
    char serial_number[32];
} esp_matter_cluster_basic_config_t;

typedef struct esp_matter_cluster_binding_config {
    uint16_t cluster_revision;
} esp_matter_cluster_binding_config_t;

typedef struct esp_matter_cluster_ota_provider_config {
    uint16_t cluster_revision;
} esp_matter_cluster_ota_provider_config_t;

typedef struct esp_matter_cluster_ota_requestor_config {
    uint16_t cluster_revision;
    uint8_t default_ota_providers[17];
    bool update_possible;
    uint8_t update_state;
    uint8_t update_state_progress;
} esp_matter_cluster_ota_requestor_config_t;

typedef struct esp_matter_cluster_general_commissioning_config {
    uint16_t cluster_revision;
    uint64_t breadcrumb;
    uint8_t basic_commissioning_info[254];
    uint8_t regulatory_config;
    uint8_t location_capability;
} esp_matter_cluster_general_commissioning_config_t;

typedef struct esp_matter_cluster_network_commissioning_config {
    uint16_t cluster_revision;
    uint32_t feature_map;
    uint8_t max_networks;
    uint8_t networks[12];
    uint8_t scan_max_time_seconds;
    uint8_t connect_max_time_seconds;
    bool interface_enabled;
    uint8_t last_networking_status;
    uint8_t last_network_id[32];
    uint32_t last_connect_error_value;
} esp_matter_cluster_network_commissioning_config_t;

typedef struct esp_matter_cluster_general_diagnostics_config {
    uint16_t cluster_revision;
    uint8_t network_interfaces[254];
    uint16_t reboot_count;
} esp_matter_cluster_general_diagnostics_config_t;

typedef struct esp_matter_cluster_administrator_commissioning_config {
    uint16_t cluster_revision;
    uint8_t window_status;
    uint16_t admin_fabric_index;
    uint16_t admin_vendor_id;
} esp_matter_cluster_administrator_commissioning_config_t;

typedef struct esp_matter_cluster_operational_credentials_config {
    uint16_t cluster_revision;
    uint8_t nocs[254];
    uint8_t fabrics[320];
    uint8_t supported_fabrics;
    uint8_t commissioned_fabrics;
    uint8_t trusted_root_certificates[400];
    uint8_t current_fabric_index;
} esp_matter_cluster_operational_credentials_config_t;

typedef struct esp_matter_cluster_group_key_management_config {
    uint16_t cluster_revision;
    uint8_t group_key_map[254];
    uint8_t group_table[254];
    uint16_t max_groups_per_fabric;
    uint16_t max_group_keys_per_fabric;
} esp_matter_cluster_group_key_management_config_t;

typedef struct esp_matter_cluster_identify_config {
    uint16_t cluster_revision;
    uint16_t identify_time;
    uint8_t identify_type;
} esp_matter_cluster_identify_config_t;

typedef struct esp_matter_cluster_groups_config {
    uint16_t cluster_revision;
    uint8_t name_support;
} esp_matter_cluster_groups_config_t;

typedef struct esp_matter_cluster_scenes_config {
    uint16_t cluster_revision;
    uint8_t scene_count;
    uint8_t current_scene;
    uint16_t current_group;
    bool scene_valid;
    uint8_t name_support;
} esp_matter_cluster_scenes_config_t;

typedef struct esp_matter_cluster_on_off_config {
    uint16_t cluster_revision;
    bool on_off;
} esp_matter_cluster_on_off_config_t;

typedef struct esp_matter_cluster_level_control_config {
    uint16_t cluster_revision;
    uint8_t current_level;
    uint8_t on_level;
    uint8_t options;
} esp_matter_cluster_level_control_config_t;

typedef struct esp_matter_cluster_color_control_config {
    uint16_t cluster_revision;
    uint32_t feature_map;
    uint8_t current_hue;
    uint8_t current_saturation;
    uint8_t color_mode;
    uint8_t options;
    uint8_t enhanced_color_mode;
    uint16_t color_capabilities;
} esp_matter_cluster_color_control_config_t;

void esp_matter_cluster_plugin_init_callback_common();

esp_matter_cluster_t *esp_matter_cluster_create_descriptor(esp_matter_endpoint_t *endpoint,
                                                           esp_matter_cluster_descriptor_config_t *config,
                                                           uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_access_control(esp_matter_endpoint_t *endpoint,
                                                               esp_matter_cluster_access_control_config_t *config,
                                                               uint8_t flags);
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
esp_matter_cluster_t *esp_matter_cluster_create_group_key_management(esp_matter_endpoint_t *endpoint,
                                                            esp_matter_cluster_group_key_management_config_t *config,
                                                            uint8_t flags);

esp_matter_cluster_t *esp_matter_cluster_create_identify(esp_matter_endpoint_t *endpoint,
                                                         esp_matter_cluster_identify_config_t *config, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_groups(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_groups_config_t *config, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_scenes(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_scenes_config_t *config, uint8_t flags);

esp_matter_cluster_t *esp_matter_cluster_create_on_off(esp_matter_endpoint_t *endpoint,
                                                       esp_matter_cluster_on_off_config_t *config, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_level_control(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_level_control_config_t *config,
                                                              uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create_color_control(esp_matter_endpoint_t *endpoint,
                                                              esp_matter_cluster_color_control_config_t *config,
                                                              uint8_t flags);
