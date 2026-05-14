// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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

#include <cJSON.h>
#include <cmath>
#include <element_types.h>
#include <esp_check.h>
#include <esp_matter_mem.h>
#include <tlv_to_json.h>
#include <lib/support/Base64.h>

#include "support/CodeUtils.h"

using namespace chip;
using chip::TLV::TLVElementType;

constexpr char TAG[] = "TlvToJson";

namespace esp_matter {

static TLVElementType get_tlv_element_type(const TLV::TLVReader &reader)
{
    return static_cast<TLVElementType>(reader.GetControlByte() & TLV::kTLVTypeMask);
}

static esp_err_t tlv_element_type_to_type_str(TLVElementType type, const char **type_str)
{
    ESP_RETURN_ON_FALSE(type_str, ESP_ERR_INVALID_ARG, TAG, "type_str cannot be NULL");
    switch (type) {
    case TLVElementType::Int8:
        *type_str = element_type::k_int8;
        return ESP_OK;
    case TLVElementType::Int16:
        *type_str = element_type::k_int16;
        return ESP_OK;
    case TLVElementType::Int32:
        *type_str = element_type::k_int32;
        return ESP_OK;
    case TLVElementType::Int64:
        *type_str = element_type::k_int64;
        return ESP_OK;
    case TLVElementType::UInt8:
        *type_str = element_type::k_uint8;
        return ESP_OK;
    case TLVElementType::UInt16:
        *type_str = element_type::k_uint16;
        return ESP_OK;
    case TLVElementType::UInt32:
        *type_str = element_type::k_uint32;
        return ESP_OK;
    case TLVElementType::UInt64:
        *type_str = element_type::k_uint64;
        return ESP_OK;
    case TLVElementType::BooleanTrue:
    case TLVElementType::BooleanFalse:
        *type_str = element_type::k_bool;
        return ESP_OK;
    case TLVElementType::FloatingPointNumber32:
        *type_str = element_type::k_float;
        return ESP_OK;
    case TLVElementType::FloatingPointNumber64:
        *type_str = element_type::k_double;
        return ESP_OK;
    case TLVElementType::UTF8String_1ByteLength:
    case TLVElementType::UTF8String_2ByteLength:
    case TLVElementType::UTF8String_4ByteLength:
    case TLVElementType::UTF8String_8ByteLength:
        *type_str = element_type::k_string;
        return ESP_OK;
    case TLVElementType::ByteString_1ByteLength:
    case TLVElementType::ByteString_2ByteLength:
    case TLVElementType::ByteString_4ByteLength:
    case TLVElementType::ByteString_8ByteLength:
        *type_str = element_type::k_bytes;
        return ESP_OK;
    case TLVElementType::Null:
        *type_str = element_type::k_null;
        return ESP_OK;
    case TLVElementType::Structure:
        *type_str = element_type::k_object;
        return ESP_OK;
    case TLVElementType::Array:
        *type_str = element_type::k_array;
        return ESP_OK;
    case TLVElementType::NotSpecified:
        *type_str = element_type::k_empty;
        return ESP_OK;
    default:
        return ESP_ERR_INVALID_ARG;
    };
}

static esp_err_t create_json_name(TLV::Tag tag, TLVElementType type, TLVElementType sub_type, char *json_name,
                                  size_t json_name_size)
{
    ESP_RETURN_ON_FALSE(tag != TLV::AnonymousTag(), ESP_ERR_INVALID_ARG, TAG, "Anonymous tag is not supported");
    ESP_RETURN_ON_FALSE(json_name, ESP_ERR_INVALID_ARG, TAG, "json name cannot be NULL");

    const char *type_str = nullptr;
    ESP_RETURN_ON_ERROR(tlv_element_type_to_type_str(type, &type_str), TAG, "Unsupported tlv element type");

    uint64_t tag_number = TLV::TagNumFromTag(tag);
    if (type == TLVElementType::Array) {
        const char *sub_type_str = nullptr;
        ESP_RETURN_ON_ERROR(tlv_element_type_to_type_str(sub_type, &sub_type_str), TAG, "Unsupported array subtype");
        snprintf(json_name, json_name_size, "%" PRIu64 ":%s-%s", tag_number, type_str, sub_type_str);
    } else {
        snprintf(json_name, json_name_size, "%" PRIu64 ":%s", tag_number, type_str);
    }
    return ESP_OK;
}

static esp_err_t encode_integer_string(int64_t value, cJSON **json)
{
    char value_str[32] = { 0 };
    snprintf(value_str, sizeof(value_str), "%" PRId64, value);
    *json = cJSON_CreateString(value_str);
    ESP_RETURN_ON_FALSE(*json, ESP_ERR_NO_MEM, TAG, "No memory");
    return ESP_OK;
}

static esp_err_t encode_unsigned_integer_string(uint64_t value, cJSON **json)
{
    char value_str[32] = { 0 };
    snprintf(value_str, sizeof(value_str), "%" PRIu64, value);
    *json = cJSON_CreateString(value_str);
    ESP_RETURN_ON_FALSE(*json, ESP_ERR_NO_MEM, TAG, "No memory");
    return ESP_OK;
}

static esp_err_t encode_byte_string(TLV::TLVReader &reader, cJSON **json)
{
    ByteSpan value;
    ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read byte string");

    char *value_str = static_cast<char *>(esp_matter_mem_calloc(BASE64_ENCODED_LEN(value.size()) + 1, sizeof(char)));
    ESP_RETURN_ON_FALSE(value_str, ESP_ERR_NO_MEM, TAG, "No memory");
    Base64Encode(value.data(), static_cast<uint16_t>(value.size()), value_str);

    *json = cJSON_CreateString(value_str);
    esp_matter_mem_free(value_str);
    return ESP_OK;
}

static esp_err_t encode_utf8_string(TLV::TLVReader &reader, cJSON **json)
{
    ByteSpan value;
    ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read byte string");

    char *value_str = static_cast<char *>(esp_matter_mem_calloc(value.size() + 1, sizeof(char)));
    ESP_RETURN_ON_FALSE(value_str, ESP_ERR_NO_MEM, TAG, "No memory");
    memcpy(value_str, value.data(), value.size());

    *json = cJSON_CreateString(value_str);
    esp_matter_mem_free(value_str);
    ESP_RETURN_ON_FALSE(*json, ESP_ERR_NO_MEM, TAG, "No memory");
    return ESP_OK;
}

static esp_err_t encode_floating_point(float value, cJSON **json)
{
    if (std::isinf(value)) {
        *json = cJSON_CreateString(value > 0 ? element_type::k_floating_point_positive_infinity
                                   : element_type::k_floating_point_negative_infinity);
    } else {
        *json = cJSON_CreateNumber(value);
    }
    ESP_RETURN_ON_FALSE(*json, ESP_ERR_NO_MEM, TAG, "No memory");
    return ESP_OK;
}

static esp_err_t encode_floating_point(double value, cJSON **json)
{
    if (std::isinf(value)) {
        *json = cJSON_CreateString(value > 0 ? element_type::k_floating_point_positive_infinity
                                   : element_type::k_floating_point_negative_infinity);
    } else {
        *json = cJSON_CreateNumber(value);
    }
    ESP_RETURN_ON_FALSE(*json, ESP_ERR_NO_MEM, TAG, "No memory");
    return ESP_OK;
}

static esp_err_t encode_tlv_node(TLV::TLVReader &reader, cJSON **json, TLVElementType &sub_type);

static esp_err_t encode_tlv_object(TLV::TLVReader &reader, cJSON **json)
{
    esp_err_t ret = ESP_OK;
    TLV::TLVType container_type;
    cJSON *json_obj = cJSON_CreateObject();
    cJSON *json_obj_child = nullptr;
    bool container_opened = false;
    CHIP_ERROR err = CHIP_NO_ERROR;

    ESP_GOTO_ON_FALSE(json_obj, ESP_ERR_NO_MEM, cleanup, TAG, "No memory");
    err = reader.EnterContainer(container_type);
    ESP_GOTO_ON_FALSE(err == CHIP_NO_ERROR, ESP_FAIL, cleanup, TAG, "Failed to enter container: %" CHIP_ERROR_FORMAT, err.Format());
    container_opened = true;

    while ((err = reader.Next()) == CHIP_NO_ERROR) {
        TLV::Tag child_tag = reader.GetTag();
        TLVElementType child_type = get_tlv_element_type(reader);
        TLVElementType child_sub_type = TLVElementType::NotSpecified;
        ESP_GOTO_ON_ERROR(encode_tlv_node(reader, &json_obj_child, child_sub_type), cleanup, TAG, "Failed to encode tlv node");

        char json_name[64] = { 0 };
        ESP_GOTO_ON_ERROR(create_json_name(child_tag, child_type, child_sub_type, json_name, sizeof(json_name)),
                          cleanup, TAG, "Failed to create json name");

        cJSON_AddItemToObject(json_obj, json_name, json_obj_child);
        json_obj_child = nullptr;
    }
    ESP_GOTO_ON_FALSE(err == CHIP_END_OF_TLV, ESP_FAIL, cleanup, TAG, "Failed to iterate container: %" CHIP_ERROR_FORMAT, err.Format());

    err = reader.ExitContainer(container_type);
    ESP_GOTO_ON_FALSE(err == CHIP_NO_ERROR, ESP_FAIL, cleanup, TAG, "Failed to exit container: %" CHIP_ERROR_FORMAT, err.Format());
    container_opened = false;

    *json = json_obj;
    json_obj = nullptr;

cleanup:
    cJSON_Delete(json_obj_child);
    cJSON_Delete(json_obj);

    if (container_opened) {
        err = reader.ExitContainer(container_type);
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Failed to exit container: %" CHIP_ERROR_FORMAT, err.Format());
        }
    }
    return ret;
}

static esp_err_t encode_tlv_array(TLV::TLVReader &reader, cJSON **json, TLVElementType &sub_type)
{
    esp_err_t ret = ESP_OK;
    TLV::TLVType container_type;
    cJSON *json_array = cJSON_CreateArray();
    cJSON *json_array_child = nullptr;
    CHIP_ERROR err = CHIP_NO_ERROR;
    bool container_opened = false;

    ESP_GOTO_ON_FALSE(json_array, ESP_ERR_NO_MEM, cleanup, TAG, "No memory");
    err = reader.EnterContainer(container_type);
    ESP_GOTO_ON_FALSE(err == CHIP_NO_ERROR, ESP_FAIL, cleanup, TAG, "Failed to enter container: %" CHIP_ERROR_FORMAT,
                      err.Format());
    container_opened = true;

    sub_type = TLVElementType::NotSpecified;
    while ((err = reader.Next()) == CHIP_NO_ERROR) {
        TLVElementType child_sub_type = TLVElementType::NotSpecified;
        TLVElementType child_type = get_tlv_element_type(reader);
        ESP_GOTO_ON_ERROR(encode_tlv_node(reader, &json_array_child, child_sub_type), cleanup, TAG, "Failed to encode tlv node");

        if (sub_type == TLVElementType::NotSpecified) {
            sub_type = child_type;
        }

        cJSON_AddItemToArray(json_array, json_array_child);
        json_array_child = nullptr;
    }
    ESP_GOTO_ON_FALSE(err == CHIP_END_OF_TLV, ESP_FAIL, cleanup, TAG, "Failed to iterate container: %" CHIP_ERROR_FORMAT,
                      err.Format());

    err = reader.ExitContainer(container_type);
    ESP_GOTO_ON_FALSE(err == CHIP_NO_ERROR, ESP_FAIL, cleanup, TAG, "Failed to exit container: %" CHIP_ERROR_FORMAT, err.Format());
    container_opened = false;

    *json = json_array;
    json_array = nullptr;

cleanup:
    cJSON_Delete(json_array_child);
    cJSON_Delete(json_array);
    if (container_opened) {
        err = reader.ExitContainer(container_type);
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Failed to exit container: %" CHIP_ERROR_FORMAT, err.Format());
        }
    }
    return ret;
}

static esp_err_t encode_tlv_node(TLV::TLVReader &reader, cJSON **json, TLVElementType &sub_type)
{
    sub_type = TLVElementType::NotSpecified;

    switch (get_tlv_element_type(reader)) {
    case TLVElementType::Int8: {
        int8_t value = 0;
        ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read int8");
        *json = cJSON_CreateNumber(value);
        break;
    }
    case TLVElementType::Int16: {
        int16_t value = 0;
        ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read int16");
        *json = cJSON_CreateNumber(value);
        break;
    }
    case TLVElementType::Int32: {
        int32_t value = 0;
        ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read int32");
        *json = cJSON_CreateNumber(value);
        break;
    }
    case TLVElementType::Int64: {
        int64_t value = 0;
        ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read int64");
        return encode_integer_string(value, json);
    }
    case TLVElementType::UInt8: {
        uint8_t value = 0;
        ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read uint8");
        *json = cJSON_CreateNumber(value);
        break;
    }
    case TLVElementType::UInt16: {
        uint16_t value = 0;
        ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read uint16");
        *json = cJSON_CreateNumber(value);
        break;
    }
    case TLVElementType::UInt32: {
        uint32_t value = 0;
        ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read uint32");
        *json = cJSON_CreateNumber(value);
        break;
    }
    case TLVElementType::UInt64: {
        uint64_t value = 0;
        ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read uint64");
        return encode_unsigned_integer_string(value, json);
    }
    case TLVElementType::BooleanFalse:
    case TLVElementType::BooleanTrue: {
        bool value = false;
        ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read bool");
        *json = cJSON_CreateBool(value);
        break;
    }
    case TLVElementType::FloatingPointNumber32: {
        float value = 0;
        ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read float");
        return encode_floating_point(value, json);
    }
    case TLVElementType::FloatingPointNumber64: {
        double value = 0;
        ESP_RETURN_ON_FALSE(reader.Get(value) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read double");
        return encode_floating_point(value, json);
    }
    case TLVElementType::UTF8String_1ByteLength:
    case TLVElementType::UTF8String_2ByteLength:
    case TLVElementType::UTF8String_4ByteLength:
    case TLVElementType::UTF8String_8ByteLength:
        return encode_utf8_string(reader, json);
    case TLVElementType::ByteString_1ByteLength:
    case TLVElementType::ByteString_2ByteLength:
    case TLVElementType::ByteString_4ByteLength:
    case TLVElementType::ByteString_8ByteLength:
        return encode_byte_string(reader, json);
    case TLVElementType::Null:
        *json = cJSON_CreateNull();
        break;
    case TLVElementType::Structure:
        return encode_tlv_object(reader, json);
    case TLVElementType::Array:
    case TLVElementType::List:
        return encode_tlv_array(reader, json, sub_type);
    default:
        ESP_LOGE(TAG, "Unsupported tlv element type: %d", static_cast<int>(get_tlv_element_type(reader)));
        return ESP_ERR_NOT_SUPPORTED;
    }

    ESP_RETURN_ON_FALSE(*json, ESP_ERR_NO_MEM, TAG, "No memory");
    return ESP_OK;
}

esp_err_t tlv_to_json(TLV::TLVReader &reader, cJSON **json)
{
    ESP_RETURN_ON_FALSE(json, ESP_ERR_INVALID_ARG, TAG, "json cannot be NULL");

    *json = nullptr;

    TLV::TLVReader reader_copy;
    reader_copy.Init(reader);

    if (reader_copy.GetType() == TLV::kTLVType_NotSpecified) {
        CHIP_ERROR chip_err = reader_copy.Next();
        if (chip_err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Failed to move tlv reader: %" CHIP_ERROR_FORMAT, chip_err.Format());
            return ESP_FAIL;
        }
    }

    TLVElementType sub_type = TLVElementType::NotSpecified;
    esp_err_t err = encode_tlv_node(reader_copy, json, sub_type);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}

} // namespace esp_matter
