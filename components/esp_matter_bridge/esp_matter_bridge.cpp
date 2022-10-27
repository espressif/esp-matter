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
#include <nvs_flash.h>
#include <string.h>

#include <esp_matter_bridge.h>
#if MAX_BRIDGED_DEVICE_COUNT > 0
#define ESP_MATTER_BRIDGE_PESISTENT_INFO_KEY "persistent_info"
#define ESP_MATTER_BRIDGE_NAMESPACE "bridge"
#define ESP_MATTER_BRIDGE_ENDPOINT_ID_ARRAY_KEY "ep_id_array"

static const char *TAG = "esp_matter_bridge";

using namespace esp_matter;
using namespace esp_matter::endpoint;

namespace esp_matter_bridge {

static uint16_t bridged_endpoint_id_array[MAX_BRIDGED_DEVICE_COUNT];

/** Persistent Bridged Device Info **/
static esp_err_t store_device_persistent_info(device_t *device)
{
    esp_err_t err = ESP_OK;
    if (!device) {
        ESP_LOGE(TAG, "device cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    char namespace_name[16] = {0};
    snprintf(namespace_name, 16, "bridge_ep_%X", device->persistent_info.device_endpoint_id);
    err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, namespace_name, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 namespace_name, err);
        return err;
    }
    err = nvs_set_blob(handle, ESP_MATTER_BRIDGE_PESISTENT_INFO_KEY, &device->persistent_info,
                       sizeof(device_persistent_info_t));
    nvs_commit(handle);
    nvs_close(handle);
    return err;
}

static esp_err_t read_device_persistent_info(device_persistent_info_t *persistent_info, uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    if (!persistent_info) {
        ESP_LOGE(TAG, "persistent_info cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    char namespace_name[16] = {0};
    snprintf(namespace_name, 16, "bridge_ep_%X", endpoint_id);
    err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, namespace_name, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 namespace_name, err);
        return err;
    }
    size_t len = sizeof(device_persistent_info_t);
    err = nvs_get_blob(handle, ESP_MATTER_BRIDGE_PESISTENT_INFO_KEY, persistent_info, &len);
    nvs_close(handle);
    return err;
}

static esp_err_t store_bridged_endpoint_ids()
{
    esp_err_t err = ESP_OK;
    nvs_handle_t handle;
    err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, ESP_MATTER_BRIDGE_NAMESPACE, NVS_READWRITE,
                                  &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 ESP_MATTER_BRIDGE_NAMESPACE, err);
        return err;
    }
    err = nvs_set_blob(handle, ESP_MATTER_BRIDGE_ENDPOINT_ID_ARRAY_KEY, bridged_endpoint_id_array,
                       sizeof(bridged_endpoint_id_array));
    nvs_commit(handle);
    nvs_close(handle);
    return err;
}

static esp_err_t read_bridged_endpoint_ids()
{
    esp_err_t err = ESP_OK;
    nvs_handle_t handle;
    err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, ESP_MATTER_BRIDGE_NAMESPACE, NVS_READONLY,
                                  &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 ESP_MATTER_BRIDGE_NAMESPACE, err);
        return err;
    }
    size_t len = sizeof(bridged_endpoint_id_array);
    err = nvs_get_blob(handle, ESP_MATTER_BRIDGE_ENDPOINT_ID_ARRAY_KEY, bridged_endpoint_id_array, &len);
    nvs_close(handle);
    return err;
}

esp_err_t get_bridged_endpoint_ids(uint16_t *matter_endpoint_id_array)
{
    if (!matter_endpoint_id_array) {
        ESP_LOGE(TAG, "matter_endpoint_id_array is NULL. Failed to copy the bridged_endpoint_id_array to it");
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(matter_endpoint_id_array, bridged_endpoint_id_array, sizeof(bridged_endpoint_id_array));
    return ESP_OK;
}

esp_err_t erase_bridged_device_info(uint16_t endpoint_id)
{
    // Remove endpoint id from the endpoint id array
    esp_err_t err = ESP_OK;
    for (size_t idx = 0; idx < MAX_BRIDGED_DEVICE_COUNT; ++idx) {
        if (bridged_endpoint_id_array[idx] == endpoint_id) {
            bridged_endpoint_id_array[idx] = chip::kInvalidEndpointId;
        }
    }
    err = store_bridged_endpoint_ids();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to store the endpoint_id array");
        return err;
    }
    // Clear the persistent information of the removed endpoint
    nvs_handle_t handle;
    char namespace_name[16] = {0};
    snprintf(namespace_name, 16, "bridge_ep_%X", endpoint_id);
    err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, namespace_name, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 namespace_name, err);
        return err;
    }
    err = nvs_erase_all(handle);
    nvs_close(handle);
    return err;
}

esp_err_t set_device_type(device_t *bridged_device, uint32_t device_type_id)
{
    if (!bridged_device) {
        ESP_LOGE(TAG, "bridged_device cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    switch (device_type_id) {
    case ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID: {
        on_off_light::config_t on_off_light_conf;
        bridged_device->endpoint = on_off_light::add(bridged_device->endpoint, &on_off_light_conf);
        break;
    }
    case ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID: {
        dimmable_light::config_t dimmable_light_conf;
        bridged_device->endpoint = dimmable_light::add(bridged_device->endpoint, &dimmable_light_conf);
        break;
    }
    case ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID: {
        color_temperature_light::config_t color_temperature_light_conf;
        bridged_device->endpoint =
            color_temperature_light::add(bridged_device->endpoint, &color_temperature_light_conf);
        break;
    }
    case ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID: {
        extended_color_light::config_t extended_color_light_conf;
        bridged_device->endpoint = extended_color_light::add(bridged_device->endpoint, &extended_color_light_conf);
        break;
    }
    default: {
        ESP_LOGE(TAG, "Unsupported bridged matter device type");
        return ESP_ERR_INVALID_ARG;
    }
    }
    return bridged_device->endpoint ? ESP_OK : ESP_FAIL;
}

static bool parent_endpoint_is_valid(node_t *node, uint16_t parent_endpoint_id)
{
    if (!node) {
        ESP_LOGE(TAG, "Node cannot be NULL");
        return false;
    }
    endpoint_t *parent_endpoint = endpoint::get(node, parent_endpoint_id);
    if (!parent_endpoint) {
        ESP_LOGE(TAG, "Parent endpoint cannot be NULL");
        return false;
    }
    uint8_t device_type_count = 0;
    uint32_t *device_type_ids_ptr = get_device_type_ids(parent_endpoint, &device_type_count);
    if (device_type_ids_ptr == NULL || device_type_count == 0) {
        ESP_LOGE(TAG, "Device type id array cannot be NULL");
        return false;
    }
    for (uint8_t i = 0; i < device_type_count; ++i) {
        if (device_type_ids_ptr[i] == esp_matter::endpoint::aggregator::get_device_type_id()) {
            return true;
        }
    }
    ESP_LOGE(TAG, "The device types of the parent endpoint must include aggregator");
    return false;
}

device_t *create_device(node_t *node, uint16_t parent_endpoint_id, uint32_t device_type_id)
{
    // Check whether the parent endpoint is valid
    if (!parent_endpoint_is_valid(node, parent_endpoint_id)) {
        ESP_LOGE(TAG, "Parent endpoint is invalid");
        return NULL;
    }

    // Create bridged device
    device_t *dev = (device_t *)calloc(1, sizeof(device_t));
    dev->node = node;
    dev->persistent_info.parent_endpoint_id = parent_endpoint_id;
    bridged_node::config_t bridged_node_config;
    dev->endpoint =
        bridged_node::create(node, &bridged_node_config, ENDPOINT_FLAG_DESTROYABLE | ENDPOINT_FLAG_BRIDGE, NULL);
    if (!(dev->endpoint)) {
        ESP_LOGE(TAG, "Could not create esp_matter endpoint for bridged device");
        free(dev);
        return NULL;
    }
    if (set_device_type(dev, device_type_id) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add the device type for the bridged device");
        remove_device(dev);
        return NULL;
    }
    endpoint_t *parent_endpoint = endpoint::get(node, parent_endpoint_id);
    if (set_parent_endpoint(dev->endpoint, parent_endpoint) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set parent endpoint for the bridged device");
        remove_device(dev);
        return NULL;
    }

    // Store the persistent information
    dev->persistent_info.device_endpoint_id = esp_matter::endpoint::get_id(dev->endpoint);
    dev->persistent_info.device_type_id = device_type_id;
    if (store_device_persistent_info(dev) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to store the persistent info for the bridged device");
        remove_device(dev);
        return NULL;
    }

    // Store the endpoint_id in endpoint_id_array
    size_t idx;
    for (idx = 0; idx < MAX_BRIDGED_DEVICE_COUNT; ++idx) {
        if (bridged_endpoint_id_array[idx] == chip::kInvalidEndpointId) {
            bridged_endpoint_id_array[idx] = dev->persistent_info.device_endpoint_id;
            break;
        }
    }
    if (idx == MAX_BRIDGED_DEVICE_COUNT) {
        ESP_LOGE(TAG, "Endpoints are used up");
        remove_device(dev);
        return NULL;
    }
    store_bridged_endpoint_ids();
    return dev;
}

device_t *resume_device(node_t *node, uint16_t device_endpoint_id)
{
    esp_err_t err = ESP_OK;
    device_persistent_info_t persistent_info;
    err = read_device_persistent_info(&persistent_info, device_endpoint_id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read the persistent info for the resumed device");
        return NULL;
    }
    if (!parent_endpoint_is_valid(node, persistent_info.parent_endpoint_id)) {
        ESP_LOGE(TAG, "Parent endpoint is invalid");
        return NULL;
    }
    device_t *dev = (device_t *)calloc(1, sizeof(device_t));
    dev->node = node;
    dev->persistent_info = persistent_info;
    bridged_node::config_t bridged_node_config;
    dev->endpoint = bridged_node::resume(node, &bridged_node_config, ENDPOINT_FLAG_DESTROYABLE | ENDPOINT_FLAG_BRIDGE,
                                         device_endpoint_id, NULL);
    if (!(dev->endpoint)) {
        ESP_LOGE(TAG, "Could not resume esp_matter endpoint for bridged device");
        free(dev);
        erase_bridged_device_info(device_endpoint_id);
        return NULL;
    }
    if (set_device_type(dev, persistent_info.device_type_id) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add the device type for the bridged device");
        remove_device(dev);
        return NULL;
    }
    endpoint_t *parent_endpoint = endpoint::get(node, persistent_info.parent_endpoint_id);
    if (set_parent_endpoint(dev->endpoint, parent_endpoint) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set parent endpoint for the bridged device");
        remove_device(dev);
        return NULL;
    }
    return dev;
}

esp_err_t remove_device(device_t *bridged_device)
{
    if (!bridged_device) {
        return ESP_ERR_INVALID_ARG;
    }
    erase_bridged_device_info(bridged_device->persistent_info.device_endpoint_id);
    esp_err_t error = endpoint::destroy(bridged_device->node, bridged_device->endpoint);
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete bridged endpoint");
    }
    free(bridged_device);
    return error;
}

esp_err_t initialize(node_t *node)
{
    if (!node) {
        ESP_LOGE(TAG, "node could not be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = nvs_flash_init_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize the bridge info partition");
        return err;
    }
    // Read the bridged endpoint array
    err = read_bridged_endpoint_ids();
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "The bridged endpoint id array is not found in partition %s, Try to initialize it",
                 CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME);
        for (size_t idx = 0; idx < MAX_BRIDGED_DEVICE_COUNT; ++idx) {
            bridged_endpoint_id_array[idx] = chip::kInvalidEndpointId;
        }
        if (store_bridged_endpoint_ids() != ESP_OK) {
            ESP_LOGE(TAG, "Failed to store the initialized endpoint id array");
            return err;
        }
        return ESP_OK;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read the bridged endpoint id array");
    }
    return err;
}

// TODO: Add a factory_reset_cb_register so that when we call esp_matter::factory_reset, we can erase other namespaces/partitions.
esp_err_t factory_reset()
{
    if (read_bridged_endpoint_ids() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read the endpoint id array");
    }

    for (size_t idx = 0; idx < MAX_BRIDGED_DEVICE_COUNT; ++idx) {
        if (bridged_endpoint_id_array[idx] != chip::kInvalidEndpointId) {
            erase_bridged_device_info(bridged_endpoint_id_array[idx]);
            bridged_endpoint_id_array[idx] = chip::kInvalidEndpointId;
        }
    }
    return store_bridged_endpoint_ids();
}

} // namespace esp_matter_bridge

#endif // MAX_BRIDGED_DEVICE_COUNT > 0
