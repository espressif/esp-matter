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

#include <esp_matter_data_model.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_bridge.h>
#include <nvs.h>
#include <string.h>

#include <app_bridged_device.h>
#include "lib/core/DataModelTypes.h"

// The bridge app can be used only when MAX_BRIDGED_DEVICE_COUNT > 0
#if defined(MAX_BRIDGED_DEVICE_COUNT) && MAX_BRIDGED_DEVICE_COUNT > 0

using namespace esp_matter;

static const char *TAG = "app_bridged_device";
app_bridged_device_t *g_bridged_device_list = nullptr;
static uint8_t g_current_bridged_device_count = 0;
static create_device_callback_t g_create_device_cb = nullptr;
static free_device_callback_t g_free_device_cb = nullptr;

esp_err_t map_matter_error(CHIP_ERROR error)
{
    if (error.IsRange(chip::ChipError::Range::kPlatform)) {
        return error.GetValue();
    }
    if (error == CHIP_NO_ERROR) {
        return ESP_OK;
    }
    if (error == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND) {
        return ESP_ERR_NVS_NOT_FOUND;
    }
    if (error == CHIP_ERROR_BUFFER_TOO_SMALL) {
        return ESP_ERR_NVS_INVALID_LENGTH;
    }
    return ESP_FAIL;
}

/** Bridged Device APIs */
esp_err_t app_bridge_initialize(node_t *node, esp_matter_bridge::bridge_device_type_callback_t device_type_cb,
                                create_device_callback_t create_cb, free_device_callback_t free_cb)
{
    // Free_callback can be null if the user doesn't want to delete app_bridged_device_t app_bridge_remove_device()
    VerifyOrReturnValue(node && device_type_cb && create_cb, ESP_ERR_INVALID_ARG);
    esp_err_t err = esp_matter_bridge::initialize(node, device_type_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize the esp_matter_bridge");
        return err;
    }
    g_bridged_device_list = nullptr;
    g_current_bridged_device_count = 0;
    g_create_device_cb = create_cb;
    g_free_device_cb = free_cb;
    uint16_t matter_endpoint_id_array[MAX_BRIDGED_DEVICE_COUNT];
    esp_matter_bridge::get_bridged_endpoint_ids(matter_endpoint_id_array);
    for (size_t idx = 0; idx < MAX_BRIDGED_DEVICE_COUNT; ++idx) {
        if (matter_endpoint_id_array[idx] != chip::kInvalidEndpointId) {
            app_bridged_device_t *bridged_device = create_cb(node, matter_endpoint_id_array[idx]);
            if (!bridged_device) {
                ESP_LOGE(TAG, "Failed to resume the endpoint %u", matter_endpoint_id_array[idx]);
                return ESP_FAIL;
            }
            bridged_device->set_matter_device(esp_matter_bridge::resume_device(node, matter_endpoint_id_array[idx], bridged_device));
            VerifyOrReturnValue(bridged_device->get_matter_device(), ESP_ERR_NO_MEM);
            err = bridged_device->restore_dev_addr();
            VerifyOrReturnValue(err == ESP_OK, err);
            bridged_device->set_next(g_bridged_device_list);
            g_bridged_device_list = bridged_device;
            g_current_bridged_device_count++;
            // Enable the resumed endpoint
            esp_matter::endpoint::enable(bridged_device->get_matter_device()->endpoint);
        }
    }
    return ESP_OK;
}

esp_err_t app_bridge_create_new_device(node_t *node, uint16_t parent_endpoint_id, uint32_t matter_device_type_id,
                                       void *addr_ctx, void *priv_data)
{
    VerifyOrReturnValue(node && parent_endpoint_id != chip::kInvalidEndpointId, ESP_ERR_INVALID_ARG);
    VerifyOrReturnValue(g_create_device_cb, ESP_ERR_INVALID_STATE);
    if (g_current_bridged_device_count >= MAX_BRIDGED_DEVICE_COUNT) {
        ESP_LOGE(TAG, "The device list is full, could not add bridged device");
        return ESP_ERR_NO_MEM;
    }
    app_bridged_device_t *bridged_device = g_create_device_cb(node, chip::kInvalidEndpointId);
    if (!bridged_device) {
        ESP_LOGE(TAG, "Failed to create new endpoint");
        return ESP_FAIL;
    }

    bridged_device->set_matter_device(
        esp_matter_bridge::create_device(node, parent_endpoint_id, matter_device_type_id, bridged_device));
    if (!(bridged_device->get_matter_device())) {
        ESP_LOGE(TAG, "Failed to create the bridged device");
        g_free_device_cb(bridged_device);
        return ESP_ERR_NO_MEM;
    }
    bridged_device->set_dev_addr(addr_ctx);
    bridged_device->set_priv_data(priv_data);
    bridged_device->set_next(g_bridged_device_list);
    g_bridged_device_list = bridged_device;
    g_current_bridged_device_count++;

    if (ESP_OK != bridged_device->store_dev_addr()) {
        ESP_LOGW(TAG, "Failed to store the bridged device information");
    }
    // Enable the created endpoint
    esp_matter::endpoint::enable(bridged_device->get_matter_device()->endpoint);
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
        g_bridged_device_list = bridged_device->get_next();
    } else {
        current_dev = g_bridged_device_list;
        while (current_dev && current_dev->get_next()) {
            if (current_dev->get_next() == bridged_device) {
                break;
            }
            current_dev = current_dev->get_next();
        }
        if (current_dev && current_dev->get_next() == bridged_device) {
            current_dev->set_next(bridged_device->get_next());
        } else {
            return ESP_ERR_NOT_FOUND;
        }
    }

    bridged_device->erase_dev_addr();
    bridged_device->delete_dev_addr();

    // Remove the bridged device from the node.
    error = esp_matter_bridge::remove_device(bridged_device->get_matter_device());
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete bridged device");
    }

    if (g_free_device_cb) {
        g_free_device_cb(bridged_device);
    }
    return error;
}

app_bridged_device_t *app_bridge_get_device(const void *dev_addr)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if (current_dev->check_dev_addr(dev_addr)) {
            return current_dev;
        }
        current_dev = current_dev->get_next();
    }
    return nullptr;
}

app_bridged_device_t *app_bridge_get_device(uint16_t endpoint_id)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if (current_dev->get_matter_device() &&
                esp_matter::endpoint::get_id(current_dev->get_matter_device()->endpoint) == endpoint_id) {
            return current_dev;
        }
        current_dev = current_dev->get_next();
    }
    return nullptr;
}

uint16_t app_bridge_get_endpoint(const void *dev_addr)
{
    app_bridged_device_t *current_dev = g_bridged_device_list;
    while (current_dev) {
        if (current_dev->check_dev_addr(dev_addr) && current_dev->get_matter_device()) {
            return endpoint::get_id(current_dev->get_matter_device()->endpoint);
        }
        current_dev = current_dev->get_next();
    }
    return chip::kInvalidEndpointId;
}
#endif
