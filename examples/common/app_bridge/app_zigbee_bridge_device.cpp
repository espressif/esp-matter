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

#include <app_zigbee_bridge_device.h>

using esp_matter::node_t;

static const char *TAG = "esp_matter_zigbee_bridge";
static app_zigbee_bridge_device_t *device_list = NULL;
static uint8_t current_bridged_device_count = 0;

app_zigbee_bridge_device_t *app_bridge_create_zigbee_device(node_t *node, uint8_t zigbee_endpointid,
                                                            uint16_t zigbee_shortaddr)
{
    if (current_bridged_device_count >= MAX_BRIDGED_DEVICE_COUNT) {
        ESP_LOGE(TAG, "The device list is full, Could not add a zigbee bridged device");
        return NULL;
    }
    app_zigbee_bridge_device_t *new_dev = (app_zigbee_bridge_device_t *)calloc(1, sizeof(app_zigbee_bridge_device_t));
    new_dev->dev = esp_matter_bridge_create_device(node);
    if (!(new_dev->dev)) {
        ESP_LOGE(TAG, "Failed to create the basic bridged device");
        free(new_dev);
        return NULL;
    }
    new_dev->zigbee_shortaddr = zigbee_shortaddr;
    new_dev->zigbee_endpointid = zigbee_endpointid;
    new_dev->next = device_list;
    device_list = new_dev;
    current_bridged_device_count++;
    return new_dev;
}

uint16_t app_bridge_get_zigbee_shortaddr_by_matter_endpointid(int matter_endpointid)
{
    app_zigbee_bridge_device_t *current_dev = device_list;
    while (current_dev) {
        if (current_dev->dev && (current_dev->dev->endpoint_id == matter_endpointid)) {
            return current_dev->zigbee_shortaddr;
        }
        current_dev = current_dev->next;
    }
    return 0xffff;
}

int app_bridge_get_matter_endpointid_by_zigbee_shortaddr(uint16_t zigbee_shortaddr)
{
    app_zigbee_bridge_device_t *current_dev = device_list;
    while (current_dev) {
        if (current_dev->zigbee_shortaddr == zigbee_shortaddr && current_dev->dev) {
            return current_dev->dev->endpoint_id;
        }
        current_dev = current_dev->next;
    }
    return -1;
}

app_zigbee_bridge_device_t *app_bridge_get_zigbee_device_by_matter_endpointid(int matter_endpointid)
{
    app_zigbee_bridge_device_t *current_dev = device_list;
    while (current_dev) {
        if (current_dev->dev && (current_dev->dev->endpoint_id == matter_endpointid)) {
            return current_dev;
        }
        current_dev = current_dev->next;
    }
    return NULL;
}

app_zigbee_bridge_device_t *app_bridge_get_zigbee_device_by_zigbee_shortaddr(uint16_t zigbee_shortaddr)
{
    app_zigbee_bridge_device_t *current_dev = device_list;
    while (current_dev) {
        if (current_dev->zigbee_shortaddr == zigbee_shortaddr && current_dev->dev) {
            return current_dev;
        }
        current_dev = current_dev->next;
    }
    return NULL;
}

esp_err_t app_bridge_remove_zigbee_device(app_zigbee_bridge_device_t *bridged_device)
{
    esp_err_t error = ESP_OK;
    app_zigbee_bridge_device_t *current_dev = NULL;
    if (!bridged_device) {
        return ESP_ERR_INVALID_ARG;
    }
    if (device_list == bridged_device) {
        // the delete bridged device is on the head of device list
        device_list = bridged_device->next;
    } else {
        current_dev = device_list;
        while (current_dev && current_dev->next) {
            if (current_dev->next == bridged_device) {
                break;
            }
            current_dev = current_dev->next;
        }
        if (current_dev->next == bridged_device) {
            current_dev->next = bridged_device->next;
        } else {
            return ESP_ERR_NOT_FOUND;
        }
    }
    error = esp_matter_bridge_remove_device(bridged_device->dev);
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete basic bridged devie");
    }
    free(bridged_device);
    return error;
}
