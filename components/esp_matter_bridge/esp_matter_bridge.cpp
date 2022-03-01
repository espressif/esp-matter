// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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
#include <string.h>

#include <esp_matter_bridge.h>

static const char *TAG = "esp_matter_bridge";
esp_matter_endpoint_bridged_node_config_t bridged_node_config = ENDPOINT_CONFIG_BRIDGED_NODE_DEFAULT();

esp_matter_bridge_device_t *esp_matter_bridge_create_device(esp_matter_node_t *node)
{
    esp_matter_bridge_device_t *dev = (esp_matter_bridge_device_t *)calloc(1, sizeof(esp_matter_bridge_device_t));
    dev->node = node;
    dev->endpoint = esp_matter_endpoint_create_bridged_node(node, &bridged_node_config, ENDPOINT_MASK_DELETABLE);
    if (!(dev->endpoint)) {
        ESP_LOGE(TAG, "Could not create esp_matter endpoint for bridged device");
        free(dev);
        return NULL;
    }
    dev->endpoint_id = esp_matter_endpoint_get_id(dev->endpoint);
    return dev;
}

esp_err_t esp_matter_bridge_remove_device(esp_matter_bridge_device_t *bridged_device)
{
    if (!bridged_device) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t error = esp_matter_endpoint_delete(bridged_device->node, bridged_device->endpoint);
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete bridged endpoint");
    }
    free(bridged_device);
    return error;
}
