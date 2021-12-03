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

#include <esp_matter.h>
#include <esp_matter_endpoint.h>

esp_matter_endpoint_t *esp_matter_endpoint_create_root_node(esp_matter_node_t *node,
                                                            esp_matter_endpoint_root_node_config_t *config)
{
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_raw(node, ESP_MATTER_ROOT_NODE_ENDPOINT_ID);

    esp_matter_cluster_create_descriptor(endpoint, &(config->descriptor), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_access_control(endpoint, &(config->access_control), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_basic(endpoint, &(config->basic), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_ota_provider(endpoint, &(config->ota_provider), CLUSTER_MASK_CLIENT);
    esp_matter_cluster_create_ota_requestor(endpoint, &(config->ota_requestor), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_general_commissioning(endpoint, &(config->general_commissioning), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_network_commissioning(endpoint, &(config->network_commissioning), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_general_diagnostics(endpoint, &(config->general_diagnostics), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_administrator_commissioning(endpoint, &(config->administrator_commissioning),
                                                          CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_operational_credentials(endpoint, &(config->operational_credentials),
                                                      CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_group_key_management(endpoint, &(config->group_key_management), CLUSTER_MASK_SERVER);

    return endpoint;
}

esp_matter_endpoint_t *esp_matter_endpoint_create_color_dimmable_light(esp_matter_node_t *node,
                                                            esp_matter_endpoint_color_dimmable_light_config_t *config)
{
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_raw(node, ESP_MATTER_COLOR_DIMMABLE_LIGHT_ENDPOINT_ID);

    esp_matter_cluster_create_identify(endpoint, &(config->identify), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_groups(endpoint, &(config->groups), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_scenes(endpoint, &(config->scenes), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_on_off(endpoint, &(config->on_off), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_level_control(endpoint, &(config->level_control), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_basic(endpoint, &(config->basic), CLUSTER_MASK_SERVER);
    esp_matter_cluster_create_color_control(endpoint, &(config->color_control), CLUSTER_MASK_SERVER);

    return endpoint;
}

esp_matter_node_t *esp_matter_node_create(esp_matter_node_config_t *config, esp_matter_attribute_callback_t callback,
                                          void *priv_data)
{
    esp_matter_node_t *node = esp_matter_node_create_raw();

    esp_matter_attribute_callback_set(callback, priv_data);

    esp_matter_endpoint_create_root_node(node, &(config->root_node));

    return node;
}
