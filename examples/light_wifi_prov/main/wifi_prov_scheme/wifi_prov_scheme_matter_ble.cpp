/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "wifi_prov_scheme_matter_ble.h"
#include "protocomm_matter_ble.h"

#include <cstdlib>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_wifi_types.h>
#include <protocomm.h>
#include <stdint.h>
#include <string.h>

#define TAG "WIFI_PROV"

static uint8_t custom_service_uuid[BLE_UUID128_VAL_LENGTH];
static bool custom_service_uuid_set = false;
static uint8_t *custom_manufacturer_data = nullptr;
static size_t custom_manufacturer_data_len = 0;

static esp_err_t prov_start(protocomm_t *pc, void *config)
{
    if (!pc) {
        ESP_LOGE(TAG, "Protocomm handle cannot be null");
        return ESP_ERR_INVALID_ARG;
    }

    if (!config) {
        ESP_LOGE(TAG, "Cannot start with null configuration");
        return ESP_ERR_INVALID_ARG;
    }

    protocomm_matter_ble_config_t *ble_config = (protocomm_matter_ble_config_t *)config;

#if defined(CONFIG_WIFI_PROV_BLE_BONDING)
    ble_config->ble_bonding = 1;
#endif

#if defined(CONFIG_WIFI_PROV_BLE_SEC_CONN)
    ble_config->ble_sm_sc = 1;
#endif

#if defined(CONFIG_WIFI_PROV_BLE_FORCE_ENCRYPTION)
    ble_config->ble_link_encryption = 1;
#endif

    if (protocomm_matter_ble_start(pc, ble_config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start protocomm BLE service");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t prov_stop(protocomm_t *pc)
{
    return protocomm_matter_ble_stop(pc);
}

esp_err_t wifi_prov_scheme_matter_ble_set_service_uuid(uint8_t *uuid, size_t uuid_size)
{
    if (uuid_size != sizeof(custom_service_uuid)) {
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(custom_service_uuid, uuid, sizeof(custom_service_uuid));
    custom_service_uuid_set = true;
    return ESP_OK;
}

esp_err_t wifi_prov_scheme_matter_ble_set_mfg_data(uint8_t *mfg_data, size_t mfg_data_len)
{
    if (custom_manufacturer_data) {
        free(custom_manufacturer_data);
    }
    custom_manufacturer_data = (uint8_t *)malloc(mfg_data_len);
    if (!custom_manufacturer_data) {
        return ESP_ERR_NO_MEM;
    }
    custom_manufacturer_data_len = mfg_data_len;
    memcpy(custom_manufacturer_data, mfg_data, mfg_data_len);
    return ESP_OK;
}

static void *new_config()
{
    protocomm_matter_ble_config_t *ble_config =
        (protocomm_matter_ble_config_t *)calloc(1, sizeof(protocomm_matter_ble_config_t));
    if (!ble_config) {
        ESP_LOGE(TAG, "Error allocating memory for new configuration");
        return nullptr;
    }
    const uint8_t service_uuid[16] = {
        0x07, 0xed, 0x9b, 0x2d, 0x0f, 0x06, 0x7c, 0x87, 0x9b, 0x43, 0x43, 0x6b, 0x4d, 0x24, 0x75, 0x17,
    };
    memcpy(ble_config->service_uuid, service_uuid, sizeof(ble_config->service_uuid));
    return ble_config;
}

static void delete_config(void *config)
{
    if (!config) {
        ESP_LOGE(TAG, "Cannot delete null configuration");
        return;
    }
    protocomm_matter_ble_config_t *ble_config = (protocomm_matter_ble_config_t *)config;
    for (size_t i = 0; i < ble_config->name_uuid_array_len; i++) {
        free((void *)ble_config->name_uuid_array[i].name);
    }
    free(ble_config->name_uuid_array);
    free(ble_config);
}

static esp_err_t set_config_service(void *config, const char *service_name, const char *service_key)
{
    if (!config) {
        ESP_LOGE(TAG, "Cannot set null configuration");
        return ESP_ERR_INVALID_ARG;
    }
    if (!service_name) {
        ESP_LOGE(TAG, "Service name cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    protocomm_matter_ble_config_t *ble_config = (protocomm_matter_ble_config_t *)config;
    strlcpy(ble_config->device_name, service_name, sizeof(ble_config->device_name));
    if (custom_service_uuid_set) {
        memcpy(ble_config->service_uuid, custom_service_uuid, sizeof(ble_config->service_uuid));
    }
    if (custom_manufacturer_data && custom_manufacturer_data_len) {
        size_t mfg_data_len = custom_manufacturer_data_len;
        if (mfg_data_len > (MAX_BLE_MANUFACTURER_DATA_LEN - sizeof(ble_config->device_name) - 2)) {
            ESP_LOGE(TAG, "Manufacturer data length is more than the max allowed size; expect truncated mfg_data ");
            mfg_data_len = MAX_BLE_MANUFACTURER_DATA_LEN - sizeof(ble_config->device_name) - 2;
        }
        ble_config->manufacturer_data = custom_manufacturer_data;
        ble_config->manufacturer_data_len = mfg_data_len;
    } else {
        ble_config->manufacturer_data = nullptr;
        ble_config->manufacturer_data_len = 0;
    }

    return ESP_OK;
}

static esp_err_t set_config_endpoint(void *config, const char *endpoint_name, uint16_t uuid)
{
    if (!config) {
        ESP_LOGE(TAG, "Cannot set null configuration");
        return ESP_ERR_INVALID_ARG;
    }

    if (!endpoint_name) {
        ESP_LOGE(TAG, "EP name cannot be null");
        return ESP_ERR_INVALID_ARG;
    }

    protocomm_matter_ble_config_t *ble_config = (protocomm_matter_ble_config_t *)config;

    char *copy_ep_name = strdup(endpoint_name);
    if (!copy_ep_name) {
        ESP_LOGE(TAG, "Error allocating memory for EP name");
        return ESP_ERR_NO_MEM;
    }

    protocomm_matter_ble_name_uuid_t *name_uuid_array = (protocomm_matter_ble_name_uuid_t *)realloc(
        ble_config->name_uuid_array, (ble_config->name_uuid_array_len + 1) * sizeof(protocomm_matter_ble_name_uuid_t));
    if (!name_uuid_array) {
        ESP_LOGE(TAG, "Error allocating memory for EP-UUID lookup table");
        return ESP_ERR_NO_MEM;
    }

    name_uuid_array[ble_config->name_uuid_array_len].name = copy_ep_name;
    name_uuid_array[ble_config->name_uuid_array_len].uuid = uuid;
    ble_config->name_uuid_array = name_uuid_array;
    ble_config->name_uuid_array_len += 1;
    return ESP_OK;
}

const wifi_prov_scheme_t wifi_prov_scheme_matter_ble = {.prov_start = prov_start,
                                                        .prov_stop = prov_stop,
                                                        .new_config = new_config,
                                                        .delete_config = delete_config,
                                                        .set_config_service = set_config_service,
                                                        .set_config_endpoint = set_config_endpoint,
                                                        .wifi_mode = WIFI_MODE_STA};
