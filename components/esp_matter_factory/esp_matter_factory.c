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
#include <nvs_flash.h>
#include <esp_matter_factory.h>

static const char *TAG = "esp_matter_factory";

static const char *NVS_KEY_DAC_CERT        = "dac_cert";
static const char *NVS_KEY_DAC_PUBLIC_KEY  = "dac_pubkey";
static const char *NVS_KEY_DAC_PRIVATE_KEY = "dac_privkey";
static const char *NVS_KEY_PAI_CERT        = "pai_cert";
static const char *NVS_KEY_CERT_DCLRN      = "cert_dclrn";

static bool initialized = false;
static nvs_handle_t factory_nvs_handle;

esp_err_t esp_matter_factory_init(void)
{
    if (initialized) {
        ESP_LOGW(TAG, "Matter Factory already initialized");
        return ESP_OK;
    }

    esp_err_t err = nvs_flash_init_partition(CONFIG_ESP_MATTER_FACTORY_PARTITION);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialized partition:%s err:0x%x", CONFIG_ESP_MATTER_FACTORY_PARTITION, err);
        return err;
    }

    err = nvs_open_from_partition(CONFIG_ESP_MATTER_FACTORY_PARTITION, CONFIG_ESP_MATTER_FACTORY_NAMESPACE,
                                  NVS_READONLY, &factory_nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to open partition:%s namespace:%s mode:NVS_READONLY err:%x",
                      CONFIG_ESP_MATTER_FACTORY_PARTITION, CONFIG_ESP_MATTER_FACTORY_NAMESPACE, err);
        nvs_flash_deinit_partition(CONFIG_ESP_MATTER_FACTORY_PARTITION);
        return err;
    }

    initialized = true;
    return ESP_OK;
}

void esp_matter_factory_deinit(void)
{
    if (initialized) {
        nvs_close(factory_nvs_handle);
        nvs_flash_deinit_partition(CONFIG_ESP_MATTER_FACTORY_PARTITION);
        initialized = false;
    }
}

/*
 * Read value from the factory partition.
 * @param [in]      key Key to read
 * @param [out]     buf Buffer to store the value
 * @param [in, out] len Length of the buf, if read is successful, it will be updated to the actual length of the value
 *
 * @return ESP_OK on success, error code otherwise
 * @return ESP_ERR_INVALID_ARG if key is NULL or len is NULL
 * @return ESP_ERR_INVALID_STATE if factory is not initialized
 */
static esp_err_t matter_factory_read_value(const char *key, uint8_t *buf, size_t *len)
{
    assert(key);

    if (buf == NULL || len == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (initialized == false)
    {
        ESP_LOGE(TAG, "Matter Factory not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = nvs_get_blob(factory_nvs_handle, key, buf, len);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read value key:%s err:%x", key, err);
    }
    return err;
}

esp_err_t esp_matter_factory_get_dac_cert(uint8_t *buf, size_t *len)
{
    return matter_factory_read_value(NVS_KEY_DAC_CERT, buf, len);
}

esp_err_t esp_matter_factory_get_pai_cert(uint8_t *buf, size_t *len)
{
    return matter_factory_read_value(NVS_KEY_PAI_CERT, buf, len);
}

esp_err_t esp_matter_factory_get_cert_declrn(uint8_t *buf, size_t *len)
{
    return matter_factory_read_value(NVS_KEY_CERT_DCLRN, buf, len);
}

esp_err_t esp_matter_factory_get_dac_public_key(uint8_t *buf, size_t *len)
{
    return matter_factory_read_value(NVS_KEY_DAC_PUBLIC_KEY, buf, len);
}

esp_err_t esp_matter_factory_get_dac_private_key(uint8_t *buf, size_t *len)
{
    return matter_factory_read_value(NVS_KEY_DAC_PRIVATE_KEY, buf, len);
}
