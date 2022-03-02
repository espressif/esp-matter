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

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_endpoint.h>

static const char *TAG = "esp_matter_endpoint";

esp_matter_endpoint_t *esp_matter_endpoint_create_root_node(esp_matter_node_t *node,
                                                            esp_matter_endpoint_root_node_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_raw(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    esp_matter_endpoint_set_device_type_id(endpoint, ESP_MATTER_ROOT_NODE_DEVICE_TYPE_ID);

    esp_matter_cluster_create_descriptor(endpoint, &(config->descriptor), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_access_control(endpoint, &(config->access_control), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_basic(endpoint, &(config->basic), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_ota_provider(endpoint, &(config->ota_provider), ESP_MATTER_CLUSTER_FLAG_CLIENT);
    esp_matter_cluster_create_ota_requestor(endpoint, &(config->ota_requestor), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_general_commissioning(endpoint, &(config->general_commissioning),
                                                    ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_network_commissioning(endpoint, &(config->network_commissioning),
                                                    ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_general_diagnostics(endpoint, &(config->general_diagnostics),
                                                  ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_administrator_commissioning(endpoint, &(config->administrator_commissioning),
                                                          ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_operational_credentials(endpoint, &(config->operational_credentials),
                                                      ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_group_key_management(endpoint, &(config->group_key_management),
                                                   ESP_MATTER_CLUSTER_FLAG_SERVER);

    return endpoint;
}

esp_matter_endpoint_t *esp_matter_endpoint_create_on_off_light(esp_matter_node_t *node,
                                                               esp_matter_endpoint_on_off_light_config_t *config,
                                                               uint8_t flags)
{
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_raw(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    esp_matter_endpoint_set_device_type_id(endpoint, ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID);

    esp_matter_cluster_create_identify(endpoint, &(config->identify), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_groups(endpoint, &(config->groups), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_scenes(endpoint, &(config->scenes), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_on_off(endpoint, &(config->on_off), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_basic(endpoint, &(config->basic), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_descriptor(endpoint, &(config->descriptor), ESP_MATTER_CLUSTER_FLAG_SERVER);

    return endpoint;
}

esp_matter_endpoint_t *esp_matter_endpoint_create_dimmable_light(esp_matter_node_t *node,
                                                                 esp_matter_endpoint_dimmable_light_config_t *config,
                                                                 uint8_t flags)
{
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_raw(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    esp_matter_endpoint_set_device_type_id(endpoint, ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID);

    esp_matter_cluster_create_identify(endpoint, &(config->identify), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_groups(endpoint, &(config->groups), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_scenes(endpoint, &(config->scenes), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_on_off(endpoint, &(config->on_off), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_level_control(endpoint, &(config->level_control), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_basic(endpoint, &(config->basic), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_descriptor(endpoint, &(config->descriptor), ESP_MATTER_CLUSTER_FLAG_SERVER);

    return endpoint;
}

esp_matter_endpoint_t *esp_matter_endpoint_create_color_dimmable_light(esp_matter_node_t *node,
                                                            esp_matter_endpoint_color_dimmable_light_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_raw(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    esp_matter_endpoint_set_device_type_id(endpoint, ESP_MATTER_COLOR_DIMMABLE_LIGHT_DEVICE_TYPE_ID);

    esp_matter_cluster_create_identify(endpoint, &(config->identify), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_groups(endpoint, &(config->groups), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_scenes(endpoint, &(config->scenes), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_on_off(endpoint, &(config->on_off), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_level_control(endpoint, &(config->level_control), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_basic(endpoint, &(config->basic), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_descriptor(endpoint, &(config->descriptor), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_color_control(endpoint, &(config->color_control), ESP_MATTER_CLUSTER_FLAG_SERVER);

    return endpoint;
}

esp_matter_endpoint_t *esp_matter_endpoint_create_on_off_switch(esp_matter_node_t *node,
                                                                esp_matter_endpoint_on_off_switch_config_t *config,
                                                                uint8_t flags)
{
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_raw(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    esp_matter_endpoint_set_device_type_id(endpoint, ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID);

    esp_matter_cluster_create_identify(endpoint, &(config->identify), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_groups(endpoint, &(config->groups), ESP_MATTER_CLUSTER_FLAG_CLIENT);
    esp_matter_cluster_create_scenes(endpoint, &(config->scenes), ESP_MATTER_CLUSTER_FLAG_CLIENT);
    esp_matter_cluster_create_on_off(endpoint, &(config->on_off), ESP_MATTER_CLUSTER_FLAG_CLIENT);
    esp_matter_cluster_create_basic(endpoint, &(config->basic), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_descriptor(endpoint, &(config->descriptor), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_binding(endpoint, &(config->binding), ESP_MATTER_CLUSTER_FLAG_SERVER);

    return endpoint;
}

esp_matter_endpoint_t *esp_matter_endpoint_create_fan(esp_matter_node_t *node,
                                                      esp_matter_endpoint_fan_config_t *config,
                                                      uint8_t flags)
{
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_raw(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    esp_matter_endpoint_set_device_type_id(endpoint, ESP_MATTER_FAN_DEVICE_TYPE_ID);

    esp_matter_cluster_create_identify(endpoint, &(config->identify), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_groups(endpoint, &(config->groups), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_descriptor(endpoint, &(config->descriptor), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_fan_control(endpoint, &(config->fan_control), ESP_MATTER_CLUSTER_FLAG_SERVER);

    return endpoint;
}

esp_matter_endpoint_t *esp_matter_endpoint_create_thermostat(esp_matter_node_t *node,
                                                             esp_matter_endpoint_thermostat_config_t *config,
                                                             uint8_t flags)
{
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_raw(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    esp_matter_endpoint_set_device_type_id(endpoint, ESP_MATTER_THERMOSTAT_DEVICE_TYPE_ID);

    esp_matter_cluster_create_identify(endpoint, &(config->identify), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_groups(endpoint, &(config->groups), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_scenes(endpoint, &(config->scenes), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_basic(endpoint, &(config->basic), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_descriptor(endpoint, &(config->descriptor), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_thermostat(endpoint, &(config->thermostat), ESP_MATTER_CLUSTER_FLAG_SERVER);

    return endpoint;
}

esp_matter_endpoint_t *esp_matter_endpoint_create_bridged_node(esp_matter_node_t *node,
                                                            esp_matter_endpoint_bridged_node_config_t *config,
                                                            uint8_t flags)
{
    // bridged node endpoints are always deletable
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_raw(node, flags | ESP_MATTER_ENDPOINT_FLAG_DELETABLE);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }

    esp_matter_endpoint_set_device_type_id(endpoint, ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_ID);

    esp_matter_cluster_create_descriptor(endpoint, &(config->descriptor), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_bridged_device_basic(endpoint, &(config->bridged_device_basic),
                                                   ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_fixed_label(endpoint, &(config->fixed_label), ESP_MATTER_CLUSTER_FLAG_SERVER);

    return endpoint;
}

esp_matter_endpoint_t *esp_matter_endpoint_create_door_lock(esp_matter_node_t *node,
                                                            esp_matter_endpoint_door_lock_config_t *config,
                                                            uint8_t flags)
{
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_raw(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }

    esp_matter_endpoint_set_device_type_id(endpoint, ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_ID);

    esp_matter_cluster_create_identify(endpoint, &(config->identify), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_descriptor(endpoint, &(config->descriptor), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_door_lock(endpoint, &(config->door_lock), ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_cluster_create_time_synchronization(endpoint, &(config->time_synchronization), ESP_MATTER_CLUSTER_FLAG_SERVER);

    return endpoint;
}

esp_matter_node_t *esp_matter_node_create(esp_matter_node_config_t *config, esp_matter_attribute_callback_t callback,
                                          void *priv_data)
{
    esp_matter_attribute_callback_set(callback, priv_data);

    esp_matter_node_t *node = esp_matter_node_create_raw();
    if (!node) {
        ESP_LOGE(TAG, "Could not create node");
        return NULL;
    }

    esp_matter_endpoint_create_root_node(node, &(config->root_node), ESP_MATTER_ENDPOINT_FLAG_NONE);

    return node;
}
