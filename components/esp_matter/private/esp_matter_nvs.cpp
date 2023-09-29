// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_err.h>
#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_mem.h>

#define ESP_MATTER_NVS_PART_NAME CONFIG_ESP_MATTER_NVS_PART_NAME

namespace esp_matter {
namespace attribute {

const char * TAG = "mtr_nvs";

esp_err_t get_val_from_nvs(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t & val)
{
    /* Get keys */
    char nvs_namespace[16] = {0};
    char attribute_key[16] = {0};
    snprintf(nvs_namespace, 16, "endpoint_%" PRIX16 "", endpoint_id); /* endpoint_id */
    snprintf(attribute_key, 16, "%" PRIX32 ":%" PRIX32 "", cluster_id, attribute_id); /* cluster_id:attribute_id */

    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, nvs_namespace, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }
    ESP_LOGD(TAG, "read attribute from nvs: endpoint_id-0x%" PRIx16 ", cluster_id-0x%" PRIx32 ", attribute_id-0x%" PRIx32 "",
             endpoint_id, cluster_id, attribute_id);
    if (val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_ARRAY) {
        size_t len = 0;
        if ((err = nvs_get_blob(handle, attribute_key, NULL, &len)) == ESP_OK) {
            uint8_t *buffer = (uint8_t *)esp_matter_mem_calloc(1, len);
            if (!buffer) {
                err = ESP_ERR_NO_MEM;
            } else {
                nvs_get_blob(handle, attribute_key, buffer, &len);
                val.val.a.b = buffer;
                val.val.a.n = len;
                val.val.a.t = len + (val.val.a.t - val.val.a.s);
                val.val.a.s = len;
            }
        }
    } else {
        // Handling how to get attributes in NVS based on config option.
#if CONFIG_ESP_MATTER_NVS_USE_COMPACT_ATTR_STORAGE
        // This switch case handles primitive data types
        switch (val.type)
        {
            case ESP_MATTER_VAL_TYPE_BOOLEAN:
            {
                uint8_t b_val;
                if ((err = nvs_get_u8(handle, attribute_key, &b_val)) == ESP_OK)
                {
                    val.val.b = (b_val != 0);
                }
                break;
            }

            case ESP_MATTER_VAL_TYPE_INTEGER:
            case ESP_MATTER_VAL_TYPE_NULLABLE_INTEGER:
            {
                err = nvs_get_i32(handle, attribute_key, reinterpret_cast<int32_t *>(&val.val.i));
                break;
            }

            // no nvs api to read float, since it is stored as blob, reading as blob
            case ESP_MATTER_VAL_TYPE_FLOAT:
            case ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT:
            {
                size_t length = sizeof(val.val.f);
                err = nvs_get_blob(handle, attribute_key, &val.val.f, &length);
                break;
            }

            case ESP_MATTER_VAL_TYPE_INT8:
            case ESP_MATTER_VAL_TYPE_NULLABLE_INT8:
            {
                err = nvs_get_i8(handle, attribute_key, &val.val.i8);
                break;
            }

            case ESP_MATTER_VAL_TYPE_UINT8:
            case ESP_MATTER_VAL_TYPE_ENUM8:
            case ESP_MATTER_VAL_TYPE_BITMAP8:
            case ESP_MATTER_VAL_TYPE_NULLABLE_UINT8:
            case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8:
            case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8:
            {
                err = nvs_get_u8(handle, attribute_key, &val.val.u8);
                break;
            }

            case ESP_MATTER_VAL_TYPE_INT16:
            case ESP_MATTER_VAL_TYPE_NULLABLE_INT16:
            {
                err = nvs_get_i16(handle, attribute_key, &val.val.i16);
                break;
            }

            case ESP_MATTER_VAL_TYPE_UINT16:
            case ESP_MATTER_VAL_TYPE_BITMAP16:
            case ESP_MATTER_VAL_TYPE_NULLABLE_UINT16:
            case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16:
            {
                err = nvs_get_u16(handle, attribute_key, &val.val.u16);
                break;
            }

            case ESP_MATTER_VAL_TYPE_INT32:
            case ESP_MATTER_VAL_TYPE_NULLABLE_INT32:
            {
                err = nvs_get_i32(handle, attribute_key, &val.val.i32);
                break;
            }

            case ESP_MATTER_VAL_TYPE_UINT32:
            case ESP_MATTER_VAL_TYPE_BITMAP32:
            case ESP_MATTER_VAL_TYPE_NULLABLE_UINT32:
            case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32:
            {
                err = nvs_get_u32(handle, attribute_key, &val.val.u32);
                break;
            }

            case ESP_MATTER_VAL_TYPE_INT64:
            case ESP_MATTER_VAL_TYPE_NULLABLE_INT64:
            {
                err = nvs_get_i64(handle, attribute_key, &val.val.i64);
                break;
            }

            case ESP_MATTER_VAL_TYPE_UINT64:
            case ESP_MATTER_VAL_TYPE_NULLABLE_UINT64:
            {
                err = nvs_get_u64(handle, attribute_key, &val.val.u64);
                break;
            }

            default:
            {
                // handle the case where the type is not recognized
                err = ESP_ERR_INVALID_ARG;
                ESP_LOGE(TAG, "Invalid attribute type: %u", val.type);
                break;
            }
        }
#else
        size_t len = sizeof(esp_matter_attr_val_t);
        err = nvs_get_blob(handle, attribute_key, &val, &len);
#endif // CONFIG_ESP_MATTER_NVS_USE_COMPACT_ATTR_STORAGE
    }
    nvs_close(handle);
    return err;
}

esp_err_t store_val_in_nvs(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, const esp_matter_attr_val_t & val)
{
    char nvs_namespace[16] = {0};
    char attribute_key[16] = {0};
    snprintf(nvs_namespace, 16, "endpoint_%" PRIX16 "", endpoint_id); /* endpoint_id */
    snprintf(attribute_key, 16, "%" PRIX32 ":%" PRIX32 "", cluster_id, attribute_id); /* cluster_id:attribute_id */

    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, nvs_namespace, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }
    ESP_LOGD(TAG, "Store attribute in nvs: endpoint_id-0x%" PRIx16 ", cluster_id-0x%" PRIx32 ", attribute_id-0x%" PRIx32 "",
             endpoint_id, cluster_id, attribute_id);
    if (val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_ARRAY) {
        /* Store only if value is not NULL */
        if (val.val.a.b) {
            err = nvs_set_blob(handle, attribute_key, val.val.a.b, val.val.a.s);
            nvs_commit(handle);
        } else {
            err = ESP_OK;
        }
    } else {
        // Handling how to store attributes in NVS based on config option.
#if CONFIG_ESP_MATTER_NVS_USE_COMPACT_ATTR_STORAGE
        // This switch case handles primitive data types
        switch (val.type)
        {
            case ESP_MATTER_VAL_TYPE_BOOLEAN:
            {
                err = nvs_set_u8(handle, attribute_key, val.val.b != 0);
                break;
            }

            case ESP_MATTER_VAL_TYPE_INTEGER:
            case ESP_MATTER_VAL_TYPE_NULLABLE_INTEGER:
            {
                err = nvs_set_i32(handle, attribute_key, val.val.i);
                break;
            }

            // no nvs api to store float, storing as blob
            case ESP_MATTER_VAL_TYPE_FLOAT:
            case ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT:
            {
                err = nvs_set_blob(handle, attribute_key, &val.val.f, sizeof(val.val.f));
                break;
            }

            case ESP_MATTER_VAL_TYPE_INT8:
            case ESP_MATTER_VAL_TYPE_NULLABLE_INT8:
            {
                err = nvs_set_i8(handle, attribute_key, val.val.i8);
                break;
            }

            case ESP_MATTER_VAL_TYPE_UINT8:
            case ESP_MATTER_VAL_TYPE_ENUM8:
            case ESP_MATTER_VAL_TYPE_BITMAP8:
            case ESP_MATTER_VAL_TYPE_NULLABLE_UINT8:
            case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8:
            case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8:
            {
                err = nvs_set_u8(handle, attribute_key, val.val.u8);
                break;
            }

            case ESP_MATTER_VAL_TYPE_INT16:
            case ESP_MATTER_VAL_TYPE_NULLABLE_INT16:
            {
                err = nvs_set_i16(handle, attribute_key, val.val.i16);
                break;
            }

            case ESP_MATTER_VAL_TYPE_UINT16:
            case ESP_MATTER_VAL_TYPE_BITMAP16:
            case ESP_MATTER_VAL_TYPE_NULLABLE_UINT16:
            case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16:
            {
                err = nvs_set_u16(handle, attribute_key, val.val.u16);
                break;
            }

            case ESP_MATTER_VAL_TYPE_INT32:
            case ESP_MATTER_VAL_TYPE_NULLABLE_INT32:
            {
                err = nvs_set_i32(handle, attribute_key, val.val.i32);
                break;
            }

            case ESP_MATTER_VAL_TYPE_UINT32:
            case ESP_MATTER_VAL_TYPE_BITMAP32:
            case ESP_MATTER_VAL_TYPE_NULLABLE_UINT32:
            case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32:
            {
                err = nvs_set_u32(handle, attribute_key, val.val.u32);
                break;
            }

            case ESP_MATTER_VAL_TYPE_INT64:
            case ESP_MATTER_VAL_TYPE_NULLABLE_INT64:
            {
                err = nvs_set_i64(handle, attribute_key, val.val.i64);
                break;
            }

            case ESP_MATTER_VAL_TYPE_UINT64:
            case ESP_MATTER_VAL_TYPE_NULLABLE_UINT64:
            {
                err = nvs_set_u64(handle, attribute_key, val.val.u64);
                break;
            }

            default:
            {
                // handle the case where the type is not recognized
                err = ESP_ERR_INVALID_ARG;
                ESP_LOGE(TAG, "Invalid attribute type: %u", val.type);
                break;
            }
        }
#else
        err = nvs_set_blob(handle, attribute_key, &val, sizeof(esp_matter_attr_val_t));
#endif // CONFIG_ESP_MATTER_NVS_USE_COMPACT_ATTR_STORAGE

        nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}

} // namespace attribute
} // namespace esp_matter
