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
#include <esp_matter_nvs.h>

#include <lib/support/Base64.h>

#define ESP_MATTER_NVS_PART_NAME CONFIG_ESP_MATTER_NVS_PART_NAME

namespace esp_matter {
namespace attribute {

const char * TAG = "mtr_nvs";

static void get_attribute_key(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, char *attribute_key)
{
     // Convert the the endpoint_id, cluster_id, attribute_id to base64 string
     uint8_t encode_buf[10] = {0};
     char base64_str[17] = {0};
     memcpy(&encode_buf[0], &endpoint_id, sizeof(endpoint_id));
     memcpy(&encode_buf[2], &cluster_id, sizeof(cluster_id));
     memcpy(&encode_buf[6], &attribute_id, sizeof(attribute_id));
     chip::Base64Encode(encode_buf, 10, base64_str);
     // The last two character must be '='
     assert(base64_str[14] == '=' && base64_str[15] == '=');
     // Copy the string before '='
     strncpy(attribute_key, base64_str, 14);
     attribute_key[14] = 0;
}

static esp_err_t nvs_store_val(const char *nvs_namespace, const char *attribute_key, const esp_matter_attr_val_t & val);
static esp_err_t nvs_erase_val(const char *nvs_namespace, const char *attribute_key);

static esp_err_t nvs_get_val(const char *nvs_namespace, const char *attribute_key, esp_matter_attr_val_t & val)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, nvs_namespace, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }

    if (val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_ARRAY) {
        size_t len = 0;
        if ((err = nvs_get_blob(handle, attribute_key, NULL, &len)) == ESP_OK) {
            // This function will only be called when recovering the non-volatile attributes during reboot
            // Add we should not decrease the size of the attribute value
            len = std::max(len, static_cast<size_t>(val.val.a.s));
            uint8_t *buffer = (uint8_t *)esp_matter_mem_calloc(1, len);
            if (!buffer) {
                err = ESP_ERR_NO_MEM;
            } else {
                val.val.a.b = buffer;
                val.val.a.n = len;
                val.val.a.t = len + (val.val.a.t - val.val.a.s);
                val.val.a.s = len;
                err = nvs_get_blob(handle, attribute_key, buffer, &len);
            }
        }

        nvs_close(handle);
        return err;
    }

    // This switch case handles primitive data types
    // if value is stored as primitive data type return it, else check if its stored as blob
    // and convert it to primitive data type
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
            nvs_close(handle);
            ESP_LOGE(TAG, "Invalid attribute type: %u", val.type);
            return ESP_ERR_INVALID_ARG;
        }
    }

    // Found the value as primitive data type
    if (err == ESP_OK) {
        nvs_close(handle);
        return err;
    }

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // Read as blob, if found, write as primitive data type
        size_t len = sizeof(esp_matter_attr_val_t);
        err = nvs_get_blob(handle, attribute_key, &val, &len);
        if (err == ESP_OK) {
            // found it as a blob, close the handle
            nvs_close(handle);

            // nvs_store_val always stores primitive value using primitive data type APIs
            err = nvs_store_val(nvs_namespace, attribute_key, val);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to store as primitive data type");
            }
        }
    }

    // There is no harm calling this function even on a closed handle
    nvs_close(handle);
    return err;
}

static esp_err_t nvs_store_val(const char *nvs_namespace, const char *attribute_key, const esp_matter_attr_val_t & val)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, nvs_namespace, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    if (val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING ||
        val.type == ESP_MATTER_VAL_TYPE_ARRAY) {
        /* Store only if value is not NULL */
        if (val.val.a.b) {
            err = nvs_set_blob(handle, attribute_key, val.val.a.b, val.val.a.s);
        } else {
            err = nvs_erase_key(handle, attribute_key);
        }
        nvs_commit(handle);
    } else {
        // This switch case handles primitive data types
        // always store values as primitive data type
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
    }
    nvs_commit(handle);
    nvs_close(handle);
    return err;
}

static esp_err_t nvs_erase_val(const char *nvs_namespace, const char *attribute_key)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, nvs_namespace, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }
    err = nvs_erase_key(handle, attribute_key);
    nvs_commit(handle);
    nvs_close(handle);
    return err;
}

esp_err_t get_val_from_nvs(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t & val)
{
    /* Get attribute key */
    char attribute_key[16] = {0};
    get_attribute_key(endpoint_id, cluster_id, attribute_id, attribute_key);

    ESP_LOGD(TAG, "read attribute from nvs: endpoint_id-0x%" PRIx16 ", cluster_id-0x%" PRIx32 ","
                  " attribute_id-0x%" PRIx32 "", endpoint_id, cluster_id, attribute_id);
    esp_err_t err = nvs_get_val(ESP_MATTER_KVS_NAMESPACE, attribute_key, val);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // If we don't find attribute key in the esp_matter_kvs namespace, we will try to get the attribute value
        // with the previous key from the previous namespace.
        char nvs_namespace[16] = {0};
        char old_attribute_key[16] = {0};
        snprintf(nvs_namespace, 16, "endpoint_%" PRIX16 "", endpoint_id); /* endpoint_id */
        snprintf(old_attribute_key, 16, "%" PRIX32 ":%" PRIX32 "", cluster_id, attribute_id); /* cluster_id:attribute_id */
        err = nvs_get_val(nvs_namespace, old_attribute_key, val);
        if (err == ESP_OK) {
            // If we get the attribute value with the previous key, we will erase it and store it in current namespace
            // with the new attribute key.
            if (nvs_erase_val(nvs_namespace, old_attribute_key) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to erase old attribute key");
            }
            if (nvs_store_val(ESP_MATTER_KVS_NAMESPACE, attribute_key, val) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to store attribute_val with new attribute key");
            }
        }
    }
    return err;
}

esp_err_t store_val_in_nvs(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, const esp_matter_attr_val_t & val)
{
    /* Get attribute key */
    char attribute_key[16] = {0};
    get_attribute_key(endpoint_id, cluster_id, attribute_id, attribute_key);
    ESP_LOGD(TAG, "Store attribute in nvs: endpoint_id-0x%" PRIx16 ", cluster_id-0x%" PRIx32 ", attribute_id-0x%" PRIx32 "",
             endpoint_id, cluster_id, attribute_id);
    return nvs_store_val(ESP_MATTER_KVS_NAMESPACE, attribute_key, val);
}

esp_err_t erase_val_in_nvs(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id)
{
    /* Get attribute key */
    char attribute_key[16] = {0};
    get_attribute_key(endpoint_id, cluster_id, attribute_id, attribute_key);
    ESP_LOGD(TAG, "Erase attribute in nvs: endpoint_id-0x%" PRIx16 ", cluster_id-0x%" PRIx32 ", attribute_id-0x%" PRIx32 "",
             endpoint_id, cluster_id, attribute_id);
    return nvs_erase_val(ESP_MATTER_KVS_NAMESPACE, attribute_key);
}

} // namespace attribute
} // namespace esp_matter
