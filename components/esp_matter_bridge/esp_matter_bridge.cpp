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
#include <esp_matter_mem.h>
#include <nvs_key_allocator.h>
#if MAX_BRIDGED_DEVICE_COUNT > 0

static const char *TAG = "esp_matter_bridge";

using namespace esp_matter;
using namespace esp_matter::endpoint;

namespace esp_matter_bridge {

static uint16_t bridged_endpoint_id_array[MAX_BRIDGED_DEVICE_COUNT];

/** Persistent Bridged Device Info **/
static esp_err_t store_device_persistent_info(device_persistent_info_t *persistent_info)
{
    esp_err_t err = ESP_OK;
    if (!persistent_info) {
        ESP_LOGE(TAG, "persistent_info cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, ESP_MATTER_BRIDGE_NAMESPACE, NVS_READWRITE,
                                  &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 ESP_MATTER_BRIDGE_NAMESPACE, err);
        return err;
    }
    uint16_t endpoint_id = persistent_info->device_endpoint_id;
    err = nvs_set_blob(handle, nvs_key_allocator::endpoint_pesistent_info(endpoint_id).KeyName(),
                       persistent_info, sizeof(device_persistent_info_t));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed on nvs_set_blob when storing device_persistent_info");
    }
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed on nvs_commit when storing device_persistent_info");
    }
    nvs_close(handle);
    return err;
}

static esp_err_t nvs_get_device_persistent_info(const char *nvs_namespace, const char *nvs_key,
                                                device_persistent_info_t *persistent_info)
{
    esp_err_t err = ESP_OK;
    nvs_handle_t handle;
    err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, nvs_namespace, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 nvs_namespace, err);
        return err;
    }
    size_t len = sizeof(device_persistent_info_t);
    err = nvs_get_blob(handle, nvs_key, persistent_info, &len);
    nvs_close(handle);
    return err;
}
static esp_err_t read_device_persistent_info(device_persistent_info_t *persistent_info, uint16_t endpoint_id)
{
    if (!persistent_info) {
        ESP_LOGE(TAG, "persistent_info cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = nvs_get_device_persistent_info(ESP_MATTER_BRIDGE_NAMESPACE,
                                                   nvs_key_allocator::endpoint_pesistent_info(endpoint_id).KeyName(),
                                                   persistent_info);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // If we don't find persistent_info key in the bridge namespace, we will try to get the persistent_info
        // with the previous key from the previous namespace.
        char nvs_namespace[16] = {0};
        snprintf(nvs_namespace, 16, "bridge_ep_%X", endpoint_id);
        err = nvs_get_device_persistent_info(nvs_namespace, "persistent_info", persistent_info);
        if (err == ESP_OK) {
            nvs_handle_t handle;
            // If we get the persistent_info with the previous key, we will erase it and store it in current namespace
            // with the new persistent_info key.
            if (nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, nvs_namespace, NVS_READWRITE,
                                        &handle) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to open %s namespace", nvs_namespace);
            } else {
                if (nvs_erase_key(handle, "persistent_info") != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to erase persistent_info");
                } else {
                    nvs_commit(handle);
                }
                nvs_close(handle);
            }
            store_device_persistent_info(persistent_info);
        }
    }
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
    err = nvs_set_blob(handle, nvs_key_allocator::endpoint_ids_array().KeyName(), bridged_endpoint_id_array,
                       sizeof(bridged_endpoint_id_array));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed on nvs_set_blob when storing bridged_endpoint_ids");
    }
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed on nvs_commit when storing bridged_endpoint_ids");
    }
    nvs_close(handle);
    return err;
}

static esp_err_t nvs_get_bridged_endpoint_ids(const char *nvs_namespace, const char *nvs_key)
{
    esp_err_t err = ESP_OK;
    nvs_handle_t handle;
    err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, nvs_namespace, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 nvs_namespace, err);
        return err;
    }
    size_t len = sizeof(bridged_endpoint_id_array);
    err = nvs_get_blob(handle, nvs_key, bridged_endpoint_id_array, &len);
    nvs_close(handle);
    return err;
}

static esp_err_t read_bridged_endpoint_ids()
{
    esp_err_t err =
        nvs_get_bridged_endpoint_ids(ESP_MATTER_BRIDGE_NAMESPACE, nvs_key_allocator::endpoint_ids_array().KeyName());
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        err = nvs_get_bridged_endpoint_ids(ESP_MATTER_BRIDGE_NAMESPACE, "ep_id_array");
        // If we don't find endpoint_ids_array in the bridge namespace, we will try to get the attribute value
        // with the previous key from the previous namespace.
        if (err == ESP_OK) {
            // If we get endpoint_ids_array with the previous key, we will erase it and store it in current namespace
            // with the new endpoint_ids_array key.
            nvs_handle_t handle;
            if (nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, ESP_MATTER_BRIDGE_NAMESPACE,
                                        NVS_READWRITE, &handle) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to open bridge namespace");
            } else {
                if (nvs_erase_key(handle, "ep_id_array") != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to erase old key");
                } else {
                    nvs_commit(handle);
                }
                nvs_close(handle);
            }
            store_bridged_endpoint_ids();
        }
    }
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
    err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, ESP_MATTER_BRIDGE_NAMESPACE, NVS_READWRITE,
                                  &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 ESP_MATTER_BRIDGE_NAMESPACE, err);
        return err;
    }
    err = nvs_erase_key(handle, nvs_key_allocator::endpoint_pesistent_info(endpoint_id).KeyName());
    nvs_commit(handle);
    nvs_close(handle);
    return err;
}

static esp_err_t plugin_init_callback_endpoint(endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    ESP_LOGI(TAG, "Cluster plugin init for the new added endpoint");
    cluster_t *cluster = cluster::get_first(endpoint);
    while (cluster) {
        /* Plugin server init callback */
        cluster::plugin_server_init_callback_t plugin_server_init_callback =
            cluster::get_plugin_server_init_callback(cluster);
        if (plugin_server_init_callback) {
            plugin_server_init_callback();
        }
        cluster = cluster::get_next(cluster);
    }
    return ESP_OK;
}

static bridge_device_type_callback_t device_type_callback;

esp_err_t set_device_type(device_t *bridged_device, uint32_t device_type_id, void *priv_data)
{
    esp_err_t err;

    if (!bridged_device) {
        ESP_LOGE(TAG, "bridged_device cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    err = device_type_callback(bridged_device->endpoint, device_type_id, priv_data);
    if (err != ESP_OK)
        return err;
    return plugin_init_callback_endpoint(bridged_device->endpoint);
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

device_t *create_device(node_t *node, uint16_t parent_endpoint_id, uint32_t device_type_id, void *priv_data)
{
    // Check whether the parent endpoint is valid
    if (!parent_endpoint_is_valid(node, parent_endpoint_id)) {
        ESP_LOGE(TAG, "Parent endpoint is invalid");
        return NULL;
    }

    // Create bridged device
    device_t *dev = (device_t *)esp_matter_mem_calloc(1, sizeof(device_t));
    dev->node = node;
    dev->persistent_info.parent_endpoint_id = parent_endpoint_id;
    bridged_node::config_t bridged_node_config;
    dev->endpoint =
        bridged_node::create(node, &bridged_node_config, ENDPOINT_FLAG_DESTROYABLE | ENDPOINT_FLAG_BRIDGE, priv_data);
    if (!(dev->endpoint)) {
        ESP_LOGE(TAG, "Could not create esp_matter endpoint for bridged device");
        esp_matter_mem_free(dev);
        return NULL;
    }
    if (set_device_type(dev, device_type_id, priv_data) != ESP_OK) {
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
    if (store_device_persistent_info(&dev->persistent_info) != ESP_OK) {
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

device_t *resume_device(node_t *node, uint16_t device_endpoint_id, void *priv_data)
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
    device_t *dev = (device_t *)esp_matter_mem_calloc(1, sizeof(device_t));
    dev->node = node;
    dev->persistent_info = persistent_info;
    bridged_node::config_t bridged_node_config;
    dev->endpoint = bridged_node::resume(node, &bridged_node_config, ENDPOINT_FLAG_DESTROYABLE | ENDPOINT_FLAG_BRIDGE,
                                         device_endpoint_id, priv_data);
    if (!(dev->endpoint)) {
        ESP_LOGE(TAG, "Could not resume esp_matter endpoint for bridged device");
        esp_matter_mem_free(dev);
        erase_bridged_device_info(device_endpoint_id);
        return NULL;
    }
    if (set_device_type(dev, persistent_info.device_type_id, priv_data) != ESP_OK) {
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
    esp_matter_mem_free(bridged_device);
    return error;
}

esp_err_t initialize(node_t *node, bridge_device_type_callback_t device_type_cb)
{
    if (!node) {
        ESP_LOGE(TAG, "node could not be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    
    if (!device_type_cb) {
        ESP_LOGE(TAG, "device_type_callback cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    device_type_callback = device_type_cb;

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

// TODO: Add a factory_reset_cb_register so that when we call esp_matter::factory_reset, we can erase other
// namespaces/partitions.
esp_err_t factory_reset()
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, ESP_MATTER_BRIDGE_NAMESPACE,
                                            NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %d", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 ESP_MATTER_BRIDGE_NAMESPACE, err);
        return err;
    }
    err = nvs_erase_all(handle);
    nvs_commit(handle);
    nvs_close(handle);
    for (size_t idx = 0; idx < MAX_BRIDGED_DEVICE_COUNT; ++idx) {
        bridged_endpoint_id_array[idx] = chip::kInvalidEndpointId;
    }
    return err;
}

} // namespace esp_matter_bridge

#endif // MAX_BRIDGED_DEVICE_COUNT > 0
