/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>

#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter_mem.h>
#include <nvs.h>

#include <lib/support/CodeUtils.h>
#include <platform/ESP32/ScopedNvsHandle.h>

#include <app_dali_bridged_device.h>
#include <app_priv.h>

#define TAG "DALI Bridge"

/* Previous revisions stored {addr, color_caps} in this namespace. restore_dev_addr()
 * still reads it once and copies the blob into the common bridge address key. */
#define DALI_LEGACY_NVS_NAMESPACE "dali_light"

typedef struct {
    uint8_t addr;
    uint8_t color_caps;
} dali_legacy_nvs_info_t;

static esp_err_t read_legacy_dali_addr(uint16_t endpoint_id, dali_device_addr_t *out)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, DALI_LEGACY_NVS_NAMESPACE,
                                            NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }
    char key[16];
    snprintf(key, sizeof(key), "ep_%u", endpoint_id);
    dali_legacy_nvs_info_t info = {};
    size_t len = sizeof(info);
    err = nvs_get_blob(handle, key, &info, &len);
    nvs_close(handle);
    if (err != ESP_OK) {
        return err;
    }
    if (len != sizeof(info) || info.addr >= 64) {
        return ESP_ERR_INVALID_SIZE;
    }
    out->short_addr = info.addr;
    out->color_caps = info.color_caps;
    return ESP_OK;
}

static void erase_legacy_dali_addr(uint16_t endpoint_id)
{
    nvs_handle_t handle;
    if (nvs_open_from_partition(CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME, DALI_LEGACY_NVS_NAMESPACE, NVS_READWRITE,
                                &handle) != ESP_OK) {
        return;
    }
    char key[16];
    snprintf(key, sizeof(key), "ep_%u", endpoint_id);
    if (nvs_erase_key(handle, key) == ESP_OK) {
        nvs_commit(handle);
    }
    nvs_close(handle);
}

esp_err_t app_dali_bridged_device_t::set_dev_addr(const void *addr_ctx)
{
    if (!addr_ctx) {
        return ESP_ERR_INVALID_ARG;
    }
    const dali_device_addr_t *addr = (const dali_device_addr_t *)addr_ctx;
    if (addr->short_addr >= 64) {
        return ESP_ERR_INVALID_ARG;
    }
    if (m_dev_addr_ctx) {
        esp_matter_mem_free(m_dev_addr_ctx);
    }
    m_dev_addr_ctx = esp_matter_mem_calloc(1, sizeof(dali_device_addr_t));
    if (!m_dev_addr_ctx) {
        return ESP_ERR_NO_MEM;
    }
    memcpy(m_dev_addr_ctx, addr_ctx, sizeof(dali_device_addr_t));
    return ESP_OK;
}

bool app_dali_bridged_device_t::check_dev_addr(const void *addr_ctx)
{
    if (addr_ctx && !m_dev_addr_ctx && m_priv_data) {
        return ((const dali_device_addr_t *)addr_ctx)->short_addr ==
               ((const dali_light_ctx_t *)m_priv_data)->dali_addr;
    }
    if (addr_ctx && m_dev_addr_ctx) {
        const dali_device_addr_t *incoming = (const dali_device_addr_t *)addr_ctx;
        const dali_device_addr_t *stored = (const dali_device_addr_t *)m_dev_addr_ctx;
        return incoming->short_addr == stored->short_addr;
    }
    return false;
}

esp_err_t app_dali_bridged_device_t::delete_dev_addr()
{
    if (m_dev_addr_ctx) {
        esp_matter_mem_free(m_dev_addr_ctx);
        m_dev_addr_ctx = nullptr;
    }
    return ESP_OK;
}

esp_err_t app_dali_bridged_device_t::store_dev_addr()
{
    VerifyOrReturnValue(m_dev && m_dev_addr_ctx, ESP_ERR_INVALID_STATE);
    chip::DeviceLayer::Internal::ScopedNvsHandle scopedNvsHandle;
    CHIP_ERROR err = scopedNvsHandle.Open(ESP_MATTER_BRIDGE_NAMESPACE, NVS_READWRITE,
                                          CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %s", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 ESP_MATTER_BRIDGE_NAMESPACE, err.AsString());
        return map_matter_error(err);
    }
    uint16_t endpoint_id = esp_matter::endpoint::get_id(m_dev->endpoint);
    ESP_RETURN_ON_ERROR(nvs_set_blob(scopedNvsHandle,
                                     esp_matter_bridge::nvs_key_allocator::endpoint_dev_addr(endpoint_id).KeyName(),
                                     m_dev_addr_ctx, sizeof(dali_device_addr_t)),
                        TAG, "Error storing the device address");
    ESP_RETURN_ON_ERROR(nvs_commit(scopedNvsHandle), TAG, "Error committing NVS");
    return ESP_OK;
}

esp_err_t app_dali_bridged_device_t::restore_dev_addr()
{
    VerifyOrReturnValue(m_dev, ESP_ERR_INVALID_STATE);
    if (m_dev_addr_ctx) {
        esp_matter_mem_free(m_dev_addr_ctx);
        m_dev_addr_ctx = nullptr;
    }
    m_dev_addr_ctx = esp_matter_mem_calloc(1, sizeof(dali_device_addr_t));
    if (!m_dev_addr_ctx) {
        return ESP_ERR_NO_MEM;
    }

    uint16_t endpoint_id = esp_matter::endpoint::get_id(m_dev->endpoint);
    size_t read_size = sizeof(dali_device_addr_t);
    esp_err_t nvs_err = ESP_OK;
    {
        chip::DeviceLayer::Internal::ScopedNvsHandle scopedNvsHandle;
        CHIP_ERROR err = scopedNvsHandle.Open(ESP_MATTER_BRIDGE_NAMESPACE, NVS_READONLY,
                                              CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME);
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %s", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                     ESP_MATTER_BRIDGE_NAMESPACE, err.AsString());
            esp_matter_mem_free(m_dev_addr_ctx);
            m_dev_addr_ctx = nullptr;
            return map_matter_error(err);
        }
        nvs_err = nvs_get_blob(scopedNvsHandle,
                               esp_matter_bridge::nvs_key_allocator::endpoint_dev_addr(endpoint_id).KeyName(),
                               m_dev_addr_ctx, &read_size);
    }

    if (nvs_err == ESP_ERR_NVS_NOT_FOUND) {
        dali_device_addr_t legacy = {};
        esp_err_t legacy_err = read_legacy_dali_addr(endpoint_id, &legacy);
        if (legacy_err != ESP_OK) {
            esp_matter_mem_free(m_dev_addr_ctx);
            m_dev_addr_ctx = nullptr;
            return legacy_err;
        }
        memcpy(m_dev_addr_ctx, &legacy, sizeof(legacy));
        if (store_dev_addr() == ESP_OK) {
            erase_legacy_dali_addr(endpoint_id);
        } else {
            ESP_LOGW(TAG, "Restored legacy DALI address for endpoint %u but failed to migrate it", endpoint_id);
        }
        return ESP_OK;
    }
    if (nvs_err != ESP_OK) {
        esp_matter_mem_free(m_dev_addr_ctx);
        m_dev_addr_ctx = nullptr;
        ESP_LOGE(TAG, "Error reading the device address: %s", esp_err_to_name(nvs_err));
        return nvs_err;
    }
    dali_device_addr_t *addr = (dali_device_addr_t *)m_dev_addr_ctx;
    if (read_size != sizeof(dali_device_addr_t) || addr->short_addr >= 64) {
        esp_matter_mem_free(m_dev_addr_ctx);
        m_dev_addr_ctx = nullptr;
        return ESP_ERR_INVALID_SIZE;
    }
    return ESP_OK;
}

esp_err_t app_dali_bridged_device_t::erase_dev_addr()
{
    VerifyOrReturnValue(m_dev, ESP_ERR_INVALID_STATE);
    chip::DeviceLayer::Internal::ScopedNvsHandle scopedNvsHandle;
    CHIP_ERROR err = scopedNvsHandle.Open(ESP_MATTER_BRIDGE_NAMESPACE, NVS_READWRITE,
                                          CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Error opening partition %s namespace %s. Err: %s", CONFIG_ESP_MATTER_BRIDGE_INFO_PART_NAME,
                 ESP_MATTER_BRIDGE_NAMESPACE, err.AsString());
        return map_matter_error(err);
    }
    uint16_t endpoint_id = esp_matter::endpoint::get_id(m_dev->endpoint);
    ESP_RETURN_ON_ERROR(nvs_erase_key(scopedNvsHandle,
                                      esp_matter_bridge::nvs_key_allocator::endpoint_dev_addr(endpoint_id).KeyName()),
                        TAG, "Error erasing the device address");
    ESP_RETURN_ON_ERROR(nvs_commit(scopedNvsHandle), TAG, "Error committing NVS");
    erase_legacy_dali_addr(endpoint_id);
    return ESP_OK;
}
