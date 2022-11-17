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
#include <nvs.h>
#include <string.h>

#include <app_bridged_device.h>

// The bridge app can be used only when MAX_BRIDGED_DEVICE_COUNT > 0
#if defined(MAX_BRIDGED_DEVICE_COUNT) && MAX_BRIDGED_DEVICE_COUNT > 0
#define APP_BRIDGE_BRIDGED_DEVICE_ADDR_KEY "dev_addr"
#define APP_BRIDGE_BRIDGED_DEVICE_TYPE_KEY "dev_type"

using namespace esp_matter;

static const char *TAG = "app_bridged_device";
static app_bridged_device_t *g_bridged_device_list = NULL;
static uint8_t g_current_bridged_device_count = 0;

/** Persistent Bridged Device Info **/

static esp_err_t app_bridge_store_bridged_device_info(app_bridged_device_t *bridged_device)
{
    esp_err_t err = ESP_OK;
    if (!bridged_device) {
        ESP_LOGE(TAG, "bridged device cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    nvs_handle_t handle;
    char namespace_name[16] = {0};
    snprintf(namespace_name, 16, "bridge_ep_%X", bridged_device->dev->persistent_info.device_endpoint_id);
    err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, namespace_name, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 namespace_name, err);
        return err;
    }
    err = nvs_set_blob(handle, APP_BRIDGE_BRIDGED_DEVICE_ADDR_KEY, &bridged_device->dev_addr,
                       sizeof(app_bridged_device_address_t));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error storing the device address");
    }
    err = nvs_set_blob(handle, APP_BRIDGE_BRIDGED_DEVICE_TYPE_KEY, &bridged_device->dev_type,
                        sizeof(app_bridged_device_type_t));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error storing the device type");
    }
    nvs_commit(handle);
    nvs_close(handle);
    return err;
}

static esp_err_t app_bridge_read_bridged_device_info(app_bridged_device_type_t *device_type,
                                                     app_bridged_device_address_t *device_addr,
                                                     uint16_t matter_endpoint_id)
{
    esp_err_t err = ESP_OK;
    if (!device_type || !device_addr) {
        ESP_LOGE(TAG, "device_type or device_addr cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    nvs_handle_t handle;
    char namespace_name[16] = {0};
    snprintf(namespace_name, 16, "bridge_ep_%X", matter_endpoint_id);
    err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, namespace_name, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 namespace_name, err);
        return err;
    }
    size_t len = sizeof(app_bridged_device_address_t);
    err = nvs_get_blob(handle, APP_BRIDGE_BRIDGED_DEVICE_ADDR_KEY, device_addr, &len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading the device address");
    }
    len = sizeof(app_bridged_device_type_t);
    err = nvs_get_blob(handle, APP_BRIDGE_BRIDGED_DEVICE_TYPE_KEY, device_type, &len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading the device type");
    }
    nvs_close(handle);
    return err;
}

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
app_bridged_device_t *app_bridge_create_bridged_device(node_t *node, uint16_t parent_endpoint_id,
                                                       uint32_t matter_device_type_id,
                                                       app_bridged_device_type_t bridged_device_type,
                                                       app_bridged_device_address_t bridged_device_address)
{
    if (g_current_bridged_device_count >= MAX_BRIDGED_DEVICE_COUNT) {
        ESP_LOGE(TAG, "The device list is full, Could not add a zigbee bridged device");
        return NULL;
    }
    app_bridged_device_t *new_dev = (app_bridged_device_t *)calloc(1, sizeof(app_bridged_device_t));
    new_dev->dev = esp_matter_bridge::create_device(node, parent_endpoint_id, matter_device_type_id);
    if (!(new_dev->dev)) {
        ESP_LOGE(TAG, "Failed to create the bridged device");
        free(new_dev);
        return NULL;
    }

    new_dev->dev_type = bridged_device_type;
    new_dev->dev_addr = bridged_device_address;
    new_dev->next = g_bridged_device_list;
    g_bridged_device_list = new_dev;
    g_current_bridged_device_count++;

    if (ESP_OK != app_bridge_store_bridged_device_info(new_dev)) {
        ESP_LOGW(TAG, "Failed to store the bridged device information");
    }

    // Enable the created endpoint
    esp_matter::endpoint::enable(new_dev->dev->endpoint);

    return new_dev;
}

esp_err_t app_bridge_initialize(node_t *node)
{
    esp_err_t err = esp_matter_bridge::initialize(node);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize the esp_matter_bridge");
        return err;
    }

    uint16_t matter_endpoint_id_array[MAX_BRIDGED_DEVICE_COUNT];
    esp_matter_bridge::get_bridged_endpoint_ids(matter_endpoint_id_array);
    for (size_t idx = 0; idx < MAX_BRIDGED_DEVICE_COUNT; ++idx) {
        if (matter_endpoint_id_array[idx] != chip::kInvalidEndpointId) {
            app_bridged_device_type_t device_type;
            app_bridged_device_address_t device_addr;
            err = app_bridge_read_bridged_device_info(&device_type, &device_addr, matter_endpoint_id_array[idx]);
            if (err != ESP_OK) {
                ESP_LOGE(TAG,
                         "Failed to read the app_bridged_device_type and app_bridged_device_address for endpoint %d",
                         matter_endpoint_id_array[idx]);
                continue;
            }
            app_bridged_device_t *new_dev = (app_bridged_device_t *)calloc(1, sizeof(app_bridged_device_t));
            if (!new_dev) {
                ESP_LOGE(TAG, "Failed to alloc memory for the resumed bridged device");
                continue;
            }
            new_dev->dev = esp_matter_bridge::resume_device(node, matter_endpoint_id_array[idx]);
            if (!(new_dev->dev)) {
                ESP_LOGE(TAG, "Failed to resume the bridged device");
                free(new_dev);
                continue;
            }
            new_dev->dev_type = device_type;
            new_dev->dev_addr = device_addr;
            new_dev->next = g_bridged_device_list;
            g_bridged_device_list = new_dev;
            g_current_bridged_device_count++;

            //Enable the resumed endpoint
            esp_matter::endpoint::enable(new_dev->dev->endpoint);
        }
    }
    return ESP_OK;
}

esp_err_t app_bridge_remove_device(app_bridged_device_t *bridged_device)
{
    esp_err_t error = ESP_OK;
    app_bridged_device_t *current_dev = NULL;
    if (!bridged_device) {
        return ESP_ERR_INVALID_ARG;
    }
    if (g_bridged_device_list == bridged_device) {
        // The delete bridged device is on the head of device list
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

    // Remove the bridged device from the node.
    error = esp_matter_bridge::remove_device(bridged_device->dev);
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete bridged device");
    }
    free(bridged_device);

    return error;
}

/** ZigBee Device APIs */
app_bridged_device_t *app_bridge_get_device_by_matter_endpointid(uint16_t matter_endpointid)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if (current_dev->dev && (esp_matter::endpoint::get_id(current_dev->dev->endpoint) == matter_endpointid)) {
            return current_dev;
        }
        current_dev = current_dev->next;
    }
    return NULL;
}

app_bridged_device_t *app_bridge_get_device_by_zigbee_shortaddr(uint16_t zigbee_shortaddr)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if (current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE && current_dev->dev &&
            current_dev->dev_addr.zigbee_shortaddr == zigbee_shortaddr) {
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
        if (current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE && current_dev->dev &&
            current_dev->dev_addr.zigbee_shortaddr == zigbee_shortaddr) {
            return esp_matter::endpoint::get_id(current_dev->dev->endpoint);
        }
        current_dev = current_dev->next;
    }
    return 0xFFFF;
}

uint16_t app_bridge_get_zigbee_shortaddr_by_matter_endpointid(uint16_t matter_endpointid)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if ((current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE) && current_dev->dev &&
            (esp_matter::endpoint::get_id(current_dev->dev->endpoint) == matter_endpointid)) {
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
        if ((current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_BLEMESH) && current_dev->dev &&
            (current_dev->dev_addr.blemesh_addr == blemesh_addr)) {
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
        if ((current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_BLEMESH) && current_dev->dev &&
            (current_dev->dev_addr.blemesh_addr == blemesh_addr)) {
            return esp_matter::endpoint::get_id(current_dev->dev->endpoint);
        }
        current_dev = current_dev->next;
    }
    return 0xFFFF;
}

uint16_t app_bridge_get_blemesh_addr_by_matter_endpointid(uint16_t matter_endpointid)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if ((current_dev->dev_type == ESP_MATTER_BRIDGED_DEVICE_TYPE_BLEMESH) && current_dev->dev &&
            (esp_matter::endpoint::get_id(current_dev->dev->endpoint) == matter_endpointid)) {
            return current_dev->dev_addr.blemesh_addr;
        }
        current_dev = current_dev->next;
    }
    return 0xFFFF;
}
#endif
