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

#include <app_bridged_device.h>

using namespace esp_matter;

static const char *TAG = "app_bridged_device";
static app_bridged_device_t *g_bridged_device_list = NULL;
static uint8_t g_current_bridged_device_count = 0;

/** Bridged Device's Address APIs */
app_bridged_device_address_t app_bridge_zigbee_address(uint8_t zigbee_endpointid, uint16_t zigbee_shortaddr)
{
    app_bridged_device_address_t bridged_address = {
        {
            .zigbee_endpointid = zigbee_endpointid,
            .zigbee_shortaddr = zigbee_shortaddr,
        },
    };
    return bridged_address;
}

app_bridged_device_address_t app_bridge_blemesh_address(uint16_t blemesh_addr)
{
    app_bridged_device_address_t bridged_address = {
        .blemesh_addr = blemesh_addr,
    };
    return bridged_address;
}

/** Bridged Device APIs */
app_bridged_device_t *app_bridge_create_bridged_device(node_t *node,
                        app_bridged_device_type_t bridged_device_type, app_bridged_device_address_t bridged_device_address)
{
    if (g_current_bridged_device_count >= MAX_BRIDGED_DEVICE_COUNT) {
        ESP_LOGE(TAG, "The device list is full, Could not add a zigbee bridged device");
        return NULL;
    }
    app_bridged_device_t *new_dev = (app_bridged_device_t *)calloc(1, sizeof(app_bridged_device_t));
    new_dev->dev = esp_matter_bridge_create_device(node);
    if (!(new_dev->dev)) {
        ESP_LOGE(TAG, "Failed to create the basic bridged device");
        free(new_dev);
        return NULL;
    }
    new_dev->dev_type = bridged_device_type;
    new_dev->dev_addr = bridged_device_address;
    new_dev->next = g_bridged_device_list;
    g_bridged_device_list = new_dev;
    g_current_bridged_device_count++;
    return new_dev;
}

app_bridged_device_t *app_bridge_get_device_by_matter_endpointid(uint16_t matter_endpointid)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if (current_dev->dev && (current_dev->dev->endpoint_id == matter_endpointid)) {
            return current_dev;
        }
        current_dev = current_dev->next;
    }
    return NULL;
}

esp_err_t app_bridge_remove_device(app_bridged_device_t *bridged_device)
{
    esp_err_t error = ESP_OK;
    app_bridged_device_t *current_dev = NULL;
    if (!bridged_device) {
        return ESP_ERR_INVALID_ARG;
    }
    if (g_bridged_device_list == bridged_device) {
        // the delete bridged device is on the head of device list
        g_bridged_device_list = bridged_device->next;
    } else {
        current_dev = g_bridged_device_list;
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

/** ZigBee Device APIs */
app_bridged_device_t *app_bridge_get_device_by_zigbee_shortaddr(uint16_t zigbee_shortaddr)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if (current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE && current_dev->dev
             && current_dev->dev_addr.zigbee_shortaddr == zigbee_shortaddr) {
            return current_dev;
        }
        current_dev = current_dev->next;
    }
    return NULL;
}

uint16_t app_bridge_get_matter_endpointid_by_zigbee_shortaddr(uint16_t zigbee_shortaddr)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if (current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE && current_dev->dev
             && current_dev->dev_addr.zigbee_shortaddr == zigbee_shortaddr) {
            return current_dev->dev->endpoint_id;
        }
        current_dev = current_dev->next;
    }
    return 0xFFFF;
}

uint16_t app_bridge_get_zigbee_shortaddr_by_matter_endpointid(uint16_t matter_endpointid)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if ((current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE) && current_dev->dev
                && (current_dev->dev->endpoint_id == matter_endpointid)) {
            return current_dev->dev_addr.zigbee_shortaddr;
        }
        current_dev = current_dev->next;
    }
    return 0xFFFF;
}

/** BLE Mesh Device APIs */
app_bridged_device_t *app_bridge_get_device_by_blemesh_addr(uint16_t blemesh_addr)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if ((current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_BLEMESH) && current_dev->dev
             && (current_dev->dev_addr.blemesh_addr == blemesh_addr)) {
            return current_dev;
        }
        current_dev = current_dev->next;
    }
    return NULL;
}

uint16_t app_bridge_get_matter_endpointid_by_blemesh_addr(uint16_t blemesh_addr)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if ((current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_BLEMESH) && current_dev->dev
             && (current_dev->dev_addr.blemesh_addr == blemesh_addr)) {
            return current_dev->dev->endpoint_id;
        }
        current_dev = current_dev->next;
    }
    return 0xFFFF;
}

uint16_t app_bridge_get_blemesh_addr_by_matter_endpointid(uint16_t matter_endpointid)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if ((current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_BLEMESH) && current_dev->dev
                && (current_dev->dev->endpoint_id == matter_endpointid)) {
            return current_dev->dev_addr.blemesh_addr;
        }
        current_dev = current_dev->next;
    }
    return 0xFFFF;
}
