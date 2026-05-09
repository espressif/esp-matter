/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter_mem.h>
#include <nvs.h>

#include <lib/support/CodeUtils.h>
#include <platform/ESP32/ScopedNvsHandle.h>

#include <app_zigbee_bridged_device.h>

#define TAG "Zigbee Bridge"

esp_err_t app_zigbee_bridged_device_t::set_dev_addr(const void *addr_ctx)
{
    if (!addr_ctx) {
        return ESP_ERR_INVALID_ARG;
    }
    if (m_dev_addr_ctx) {
        esp_matter_mem_free(m_dev_addr_ctx);
    }
    m_dev_addr_ctx = esp_matter_mem_calloc(1, sizeof(zigbee_device_addr_t));
    if (!m_dev_addr_ctx) {
        return ESP_ERR_NO_MEM;
    }
    memcpy(m_dev_addr_ctx, addr_ctx, sizeof(zigbee_device_addr_t));
    return ESP_OK;
}

bool app_zigbee_bridged_device_t::check_dev_addr(const void *addr_ctx)
{
    if (addr_ctx && m_dev_addr_ctx) {
        return memcmp(m_dev_addr_ctx, addr_ctx, sizeof(zigbee_device_addr_t)) == 0;
    }
    return false;
}

esp_err_t app_zigbee_bridged_device_t::delete_dev_addr()
{
    if (m_dev_addr_ctx) {
        esp_matter_mem_free(m_dev_addr_ctx);
        m_dev_addr_ctx = nullptr;
    }
    return ESP_OK;
}

esp_err_t app_zigbee_bridged_device_t::store_dev_addr()
{
    VerifyOrReturnValue(m_dev, ESP_ERR_INVALID_STATE);
    chip::DeviceLayer::Internal::ScopedNvsHandle scopedNvsHandle;
    CHIP_ERROR err = scopedNvsHandle.Open(ESP_MATTER_BRIDGE_NAMESPACE, NVS_READWRITE, CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %s", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 ESP_MATTER_BRIDGE_NAMESPACE, err.AsString());
        return map_matter_error(err);
    }
    uint16_t endpoint_id = esp_matter::endpoint::get_id(m_dev->endpoint);
    ESP_RETURN_ON_ERROR(nvs_set_blob(scopedNvsHandle, esp_matter_bridge::nvs_key_allocator::endpoint_dev_addr(endpoint_id).KeyName(), m_dev_addr_ctx,
                                     sizeof(zigbee_device_addr_t)), TAG, "Error storing the device address");
    ESP_RETURN_ON_ERROR(nvs_commit(scopedNvsHandle), TAG, "Error committing NVS");
    return ESP_OK;
}

esp_err_t app_zigbee_bridged_device_t::restore_dev_addr()
{
    VerifyOrReturnValue(m_dev, ESP_ERR_INVALID_STATE);
    chip::DeviceLayer::Internal::ScopedNvsHandle scopedNvsHandle;
    CHIP_ERROR err = scopedNvsHandle.Open(ESP_MATTER_BRIDGE_NAMESPACE, NVS_READONLY, CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %s", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 ESP_MATTER_BRIDGE_NAMESPACE, err.AsString());
        return map_matter_error(err);
    }
    if (m_dev_addr_ctx) {
        esp_matter_mem_free(m_dev_addr_ctx);
    }
    m_dev_addr_ctx = esp_matter_mem_calloc(1, sizeof(zigbee_device_addr_t));
    if (m_dev_addr_ctx) {
        uint16_t endpoint_id = esp_matter::endpoint::get_id(m_dev->endpoint);
        size_t read_size = sizeof(zigbee_device_addr_t);
        ESP_RETURN_ON_ERROR(nvs_get_blob(scopedNvsHandle, esp_matter_bridge::nvs_key_allocator::endpoint_dev_addr(endpoint_id).KeyName(), m_dev_addr_ctx,
                                         &read_size), TAG, "Error reading the device address");
        return ESP_OK;
    }
    return ESP_ERR_NO_MEM;
}

esp_err_t app_zigbee_bridged_device_t::erase_dev_addr()
{
    VerifyOrReturnValue(m_dev, ESP_ERR_INVALID_STATE);
    chip::DeviceLayer::Internal::ScopedNvsHandle scopedNvsHandle;
    CHIP_ERROR err = scopedNvsHandle.Open(ESP_MATTER_BRIDGE_NAMESPACE, NVS_READWRITE, CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %s", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 ESP_MATTER_BRIDGE_NAMESPACE, err.AsString());
        return map_matter_error(err);
    }
    uint16_t endpoint_id = esp_matter::endpoint::get_id(m_dev->endpoint);
    ESP_RETURN_ON_ERROR(nvs_erase_key(scopedNvsHandle, esp_matter_bridge::nvs_key_allocator::endpoint_dev_addr(endpoint_id).KeyName()),
                        TAG, "Error erasing the device address");
    ESP_RETURN_ON_ERROR(nvs_commit(scopedNvsHandle), TAG, "Error committing NVS");
    return ESP_OK;
}
