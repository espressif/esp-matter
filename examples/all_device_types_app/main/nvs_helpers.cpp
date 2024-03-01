/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <nvs.h>
#include <esp_log.h>
#include <helpers.h>

namespace esp_matter {
namespace nvs_helpers {
esp_err_t set_device_type_in_nvs(uint8_t device_type_index)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(CONFIG_CHIP_CONFIG_NAMESPACE_PARTITION_LABEL, "chip-config", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
	    return err;
    }
    err = nvs_set_u8(handle, "device_name", device_type_index);
    if(err == ESP_OK)
    {
        nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}

esp_err_t get_device_type_from_nvs(uint8_t *device_type_index)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(CONFIG_CHIP_CONFIG_NAMESPACE_PARTITION_LABEL, "chip-config", NVS_READONLY, &handle);
    if (err != ESP_OK) {
	    return err;
    }
    err = nvs_get_u8(handle, "device_name", device_type_index);
    nvs_close(handle);
    return err;
}
} /* namespace nvs_helpers */
} /* namepsace esp_matter */
