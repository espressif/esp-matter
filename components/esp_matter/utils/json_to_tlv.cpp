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

#include <algorithm>
#include <cJSON.h>
#include <esp_check.h>
#include <esp_matter_mem.h>
#include <json_to_tlv.h>
#include <lib/support/Base64.h>
#include <lib/support/SafeInt.h>

#include <stdlib.h>

using namespace chip;
using chip::TLV::TLVElementType;

constexpr char TAG[] = "JsonToTlv";
constexpr uint32_t k_temporary_implicit_profile_id = 0xFF01;

namespace esp_matter {

constexpr size_t k_max_json_name_len = 64;

struct element_context {
    element_context() {}
    ~element_context() {}
    char json_name[k_max_json_name_len];
    TLV::Tag tag = chip::TLV::AnonymousTag();
    TLV::TLVElementType type;
    TLV::TLVElementType sub_type;
};

static int compare_by_tag(const void *a, const void *b)
{
    const element_context *element_a = (const element_context *)a;
    const element_context *element_b = (const element_context *)b;
    if (TLV::IsContextTag(element_a->tag) == TLV::IsContextTag(element_b->tag)) {
        return (int)TLV::TagNumFromTag(element_a->tag) - (int)TLV::TagNumFromTag(element_b->tag);
    }
    return TLV::IsContextTag(element_a->tag);
}

static size_t get_char_count(const char *str, char ch)
{
    size_t ret = 0;
    if (!str) {
        return ret;
    }
    for (size_t i = 0; i < strlen(str); ++i) {
        if (ch == str[i]) {
            ret++;
        }
    }
    return ret;
}

static bool is_unsigned_integer(const char *str, size_t len)
{
    if (len == 0) {
        return false;
    }
    for (size_t i = 0; i < len; ++i) {
        if (str[i] > '9' || str[i] < '0') {
            return false;
        }
    }
    return true;
}

static esp_err_t type_str_to_tlv_element_type(const char *type_str, size_t len, TLVElementType &type)
{
    if (len == strlen(element_type::k_int8) && strncmp(type_str, element_type::k_int8, len) == 0) {
        type = TLVElementType::Int8;
        return ESP_OK;
    } else if (len == strlen(element_type::k_int16) && strncmp(type_str, element_type::k_int16, len) == 0) {
        type = TLVElementType::Int16;
        return ESP_OK;
    } else if (len == strlen(element_type::k_int32) && strncmp(type_str, element_type::k_int32, len) == 0) {
        type = TLVElementType::Int32;
        return ESP_OK;
    } else if (len == strlen(element_type::k_int64) && strncmp(type_str, element_type::k_int64, len) == 0) {
        type = TLVElementType::Int64;
        return ESP_OK;
    } else if (len == strlen(element_type::k_uint8) && strncmp(type_str, element_type::k_uint8, len) == 0) {
        type = TLVElementType::UInt8;
        return ESP_OK;
    } else if (len == strlen(element_type::k_uint16) && strncmp(type_str, element_type::k_uint16, len) == 0) {
        type = TLVElementType::UInt16;
        return ESP_OK;
    } else if (len == strlen(element_type::k_uint32) && strncmp(type_str, element_type::k_uint32, len) == 0) {
        type = TLVElementType::UInt32;
        return ESP_OK;
    } else if (len == strlen(element_type::k_uint64) && strncmp(type_str, element_type::k_uint64, len) == 0) {
        type = TLVElementType::UInt64;
        return ESP_OK;
    } else if (len == strlen(element_type::k_float) && strncmp(type_str, element_type::k_float, len) == 0) {
        type = TLVElementType::FloatingPointNumber32;
        return ESP_OK;
    } else if (len == strlen(element_type::k_double) && strncmp(type_str, element_type::k_double, len) == 0) {
        type = TLVElementType::FloatingPointNumber64;
        return ESP_OK;
    } else if (len == strlen(element_type::k_bool) && strncmp(type_str, element_type::k_bool, len) == 0) {
        type = TLVElementType::BooleanFalse;
        return ESP_OK;
    } else if (len == strlen(element_type::k_null) && strncmp(type_str, element_type::k_null, len) == 0) {
        type = TLVElementType::Null;
        return ESP_OK;
    } else if (len == strlen(element_type::k_bytes) && strncmp(type_str, element_type::k_bytes, len) == 0) {
        type = TLVElementType::ByteString_1ByteLength;
        return ESP_OK;
    } else if (len == strlen(element_type::k_string) && strncmp(type_str, element_type::k_string, len) == 0) {
        type = TLVElementType::UTF8String_1ByteLength;
        return ESP_OK;
    } else if (len == strlen(element_type::k_array) && strncmp(type_str, element_type::k_array, len) == 0) {
        type = TLVElementType::Array;
        return ESP_OK;
    } else if (len == strlen(element_type::k_object) && strncmp(type_str, element_type::k_object, len) == 0) {
        type = TLVElementType::Structure;
        return ESP_OK;
    } else if (len == strlen(element_type::k_empty) && strncmp(type_str, element_type::k_empty, len) == 0) {
        type = TLVElementType::NotSpecified;
        return ESP_OK;
    }
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t split_json_name(const char *json_name, uint64_t &tag_number, TLVElementType &type,
                                 TLVElementType &subtype)
{
    size_t split_char_count = get_char_count(json_name, ':');
    ESP_RETURN_ON_FALSE(split_char_count == 1 || split_char_count == 2, ESP_ERR_INVALID_ARG, TAG,
                        "Invalid json name format");
    ESP_RETURN_ON_FALSE(strchr(json_name, ':'), ESP_ERR_INVALID_ARG, TAG, "Invalid json name format");
    const char *tag_start = split_char_count == 1 ? json_name : strchr(json_name, ':') + 1;
    ESP_RETURN_ON_FALSE(strchr(tag_start, ':'), ESP_ERR_INVALID_ARG, TAG, "Invalid json name format");
    const char *type_start = strchr(tag_start, ':') + 1;
    size_t tag_len = type_start - tag_start - 1;
    const char *subtype_prev = strchr(type_start, '-');
    const char *subtype_start = subtype_prev ? subtype_prev + 1 : 0;
    size_t type_len = subtype_start ? subtype_start - type_start - 1 : strlen(type_start);
    size_t subtype_len = subtype_start ? strlen(subtype_start) : 0;

    ESP_RETURN_ON_FALSE(is_unsigned_integer(tag_start, tag_len), ESP_ERR_INVALID_ARG, TAG, "Not an unsigned integer");
    tag_number = strtoull(tag_start, NULL, 10);
    ESP_RETURN_ON_ERROR(type_str_to_tlv_element_type(type_start, type_len, type), TAG,
                        "Failed to convert json_type_str to tlv element type");
    ESP_RETURN_ON_FALSE(type != TLVElementType::NotSpecified, ESP_ERR_INVALID_ARG, TAG,
                        "The tlv element type cannot be no-specified");
    if (type == TLVElementType::Array) {
        ESP_RETURN_ON_FALSE(subtype_start && subtype_len != 0, ESP_ERR_INVALID_ARG, TAG, "No subtype for array");
        ESP_RETURN_ON_ERROR(type_str_to_tlv_element_type(subtype_start, subtype_len, subtype), TAG,
                            "Failed to convert json_type_str to tlv element type");
    } else {
        subtype = TLVElementType::NotSpecified;
    }
    return ESP_OK;
}

static esp_err_t internal_convert_tlv_tag(const uint64_t tag_number, TLV::Tag &tag,
                                          const uint32_t profile_id = k_temporary_implicit_profile_id)
{
    if (tag_number <= UINT8_MAX) {
        tag = TLV::ContextTag(static_cast<uint8_t>(tag_number));
    } else if (tag_number < UINT32_MAX) {
        tag = TLV::ProfileTag(profile_id, static_cast<uint32_t>(tag_number));
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static esp_err_t parse_json_name(const char *name, element_context &element_ctx, uint32_t implicit_profile_id)
{
    uint64_t tag_number = 0;
    ESP_RETURN_ON_FALSE(name, ESP_ERR_INVALID_ARG, TAG, "json name cannot be NULL");
    ESP_RETURN_ON_ERROR(split_json_name(name, tag_number, element_ctx.type, element_ctx.sub_type), TAG,
                        "Failed to parse json name");
    ESP_RETURN_ON_ERROR(internal_convert_tlv_tag(tag_number, element_ctx.tag, implicit_profile_id), TAG,
                        "Failed to convert TLV tag");
    strncpy(element_ctx.json_name, name, strlen(name));
    element_ctx.json_name[strlen(name)] = 0;
    return ESP_OK;
}

static bool is_valid_base64_str(const char *str)
{
    const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (!str) {
        return false;
    }
    size_t len = strlen(str);
    if (len % 4 != 0) {
        return false;
    }
    size_t padding_len = 0;
    if (str[len - 1] == '=') {
        padding_len++;
        if (str[len - 2] == '=') {
            padding_len++;
        }
    }
    for (size_t i = 0; i < len - padding_len; ++i) {
        if (strchr(base64_chars, str[i]) == NULL) {
            return false;
        }
    }
    return true;
}

static size_t get_object_element_count(const cJSON *json)
{
    ESP_RETURN_ON_FALSE(json->type == cJSON_Object, 0, TAG, "Invalid type");
    size_t ret = 0;
    cJSON *current_element = json->child;
    while (current_element) {
        current_element = current_element->next;
        ret++;
    }
    return ret;
}

static esp_err_t encode_tlv_element(const cJSON *val, TLV::TLVWriter &writer, const element_context &element_ctx)
{
    TLV::Tag tag = element_ctx.tag;

    switch (element_ctx.type) {
    case TLVElementType::Int8: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_Number, ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        ESP_RETURN_ON_FALSE(val->valueint <= INT8_MAX && val->valueint >= INT8_MIN, ESP_ERR_INVALID_ARG, TAG,
                            "Invalid range");
        int8_t int8_val = val->valueint;
        ESP_RETURN_ON_FALSE(writer.Put(tag, int8_val) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        break;
    }
    case TLVElementType::Int16: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_Number, ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        ESP_RETURN_ON_FALSE(val->valueint <= INT16_MAX && val->valueint >= INT16_MIN, ESP_ERR_INVALID_ARG, TAG,
                            "Invalid range");
        int16_t int16_val = val->valueint;
        ESP_RETURN_ON_FALSE(writer.Put(tag, int16_val) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        break;
    }
    case TLVElementType::Int32: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_Number, ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        int32_t int32_val = val->valueint;
        ESP_RETURN_ON_FALSE(writer.Put(tag, int32_val) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        break;
    }
    case TLVElementType::Int64: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_Number || val->type == cJSON_String, ESP_ERR_INVALID_ARG, TAG,
                            "Invalid type");
        int64_t int64_val = 0;
        if (val->type == cJSON_Number) {
            int64_val =
                (val->valueint < INT32_MAX && val->valueint > INT32_MIN) ? val->valueint : (int64_t)val->valuedouble;
        } else {
            int64_val = strtoll(val->valuestring, nullptr, 10);
        }
        ESP_RETURN_ON_FALSE(writer.Put(tag, int64_val) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        break;
    }
    case TLVElementType::UInt8: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_Number, ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        ESP_RETURN_ON_FALSE(val->valueint <= UINT8_MAX && val->valueint >= 0, ESP_ERR_INVALID_ARG, TAG,
                            "Invalid range");
        uint8_t uint8_val = val->valueint;
        ESP_RETURN_ON_FALSE(writer.Put(tag, uint8_val) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        break;
    }
    case TLVElementType::UInt16: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_Number, ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        ESP_RETURN_ON_FALSE(val->valueint <= UINT16_MAX && val->valueint >= 0, ESP_ERR_INVALID_ARG, TAG,
                            "Invalid range");
        uint16_t uint16_val = val->valueint;
        ESP_RETURN_ON_FALSE(writer.Put(tag, uint16_val) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        break;
    }
    case TLVElementType::UInt32: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_Number, ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        ESP_RETURN_ON_FALSE(val->valueint >= 0, ESP_ERR_INVALID_ARG, TAG, "Invalid range");
        uint32_t uint32_val = val->valueint < INT32_MAX ? val->valueint : (uint32_t)val->valuedouble;
        ESP_RETURN_ON_FALSE(writer.Put(tag, uint32_val) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        break;
    }
    case TLVElementType::UInt64: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_Number || val->type == cJSON_String, ESP_ERR_INVALID_ARG, TAG,
                            "Invalid type");
        uint64_t uint64_val = 0;
        if (val->type == cJSON_Number) {
            ESP_RETURN_ON_FALSE(val->valueint >= 0, ESP_ERR_INVALID_ARG, TAG, "Invalid range");
            uint64_val = val->valueint < INT32_MAX ? val->valueint : (uint64_t)val->valuedouble;
        } else {
            uint64_val = strtoull(val->valuestring, nullptr, 10);
        }
        ESP_RETURN_ON_FALSE(writer.Put(tag, uint64_val) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        break;
    }
    case TLVElementType::FloatingPointNumber32: {
        if (val->type == cJSON_Number) {
            float float_val = val->valuedouble;
            ESP_RETURN_ON_FALSE(writer.Put(tag, float_val) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        } else if (val->type == cJSON_String) {
            if (strcmp(val->valuestring, element_type::k_floating_point_positive_infinity) == 0) {
                ESP_RETURN_ON_FALSE(writer.Put(tag, std::numeric_limits<float>::infinity()) == CHIP_NO_ERROR, ESP_FAIL,
                                    TAG, "Failed to encode");
            } else if (strcmp(val->valuestring, element_type::k_floating_point_negative_infinity) == 0) {
                ESP_RETURN_ON_FALSE(writer.Put(tag, -std::numeric_limits<float>::infinity()) == CHIP_NO_ERROR, ESP_FAIL,
                                    TAG, "Failed to encode");
            } else {
                return ESP_ERR_INVALID_ARG;
            }
        } else {
            ESP_LOGE(TAG, "Invalid type");
            return ESP_ERR_INVALID_ARG;
        }
        break;
    }
    case TLVElementType::FloatingPointNumber64: {
        if (val->type == cJSON_Number) {
            double double_val = val->valuedouble;
            ESP_RETURN_ON_FALSE(writer.Put(tag, double_val) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        } else if (val->type == cJSON_String) {
            if (strcmp(val->valuestring, element_type::k_floating_point_positive_infinity) == 0) {
                ESP_RETURN_ON_FALSE(writer.Put(tag, std::numeric_limits<double>::infinity()) == CHIP_NO_ERROR, ESP_FAIL,
                                    TAG, "Failed to encode");
            } else if (strcmp(val->valuestring, element_type::k_floating_point_negative_infinity) == 0) {
                ESP_RETURN_ON_FALSE(writer.Put(tag, -std::numeric_limits<double>::infinity()) == CHIP_NO_ERROR,
                                    ESP_FAIL, TAG, "Failed to encode");
            } else {
                return ESP_ERR_INVALID_ARG;
            }
        } else {
            ESP_LOGE(TAG, "Invalid type");
            return ESP_ERR_INVALID_ARG;
        }
        break;
    }
    case TLVElementType::BooleanTrue:
    case TLVElementType::BooleanFalse: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_False || val->type == cJSON_True, ESP_ERR_INVALID_ARG, TAG,
                            "Invalid type");
        bool bool_val = (val->type == cJSON_True);
        ESP_RETURN_ON_FALSE(writer.Put(tag, bool_val) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        break;
    }
    case TLVElementType::ByteString_1ByteLength: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_String && val->valuestring, ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        size_t encoded_len = strlen(val->valuestring);
        ESP_RETURN_ON_FALSE(chip::CanCastTo<uint16_t>(encoded_len), ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        ESP_RETURN_ON_FALSE(is_valid_base64_str(val->valuestring), ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        Platform::ScopedMemoryBuffer<uint8_t> byte_str;
        byte_str.Alloc(BASE64_MAX_DECODED_LEN(static_cast<uint16_t>(encoded_len)));
        ESP_RETURN_ON_FALSE(byte_str.Get(), ESP_ERR_NO_MEM, TAG, "No memory");
        auto decoded_len = Base64Decode(val->valuestring, static_cast<uint16_t>(encoded_len), byte_str.Get());
        ESP_RETURN_ON_FALSE(writer.PutBytes(tag, byte_str.Get(), decoded_len) == CHIP_NO_ERROR, ESP_FAIL, TAG,
                            "Failed to encode");
        break;
    }
    case TLVElementType::UTF8String_1ByteLength: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_String, ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        ESP_RETURN_ON_FALSE(writer.PutString(tag, val->valuestring) == CHIP_NO_ERROR, ESP_FAIL, TAG,
                            "Failed to encode");
        break;
    }
    case TLVElementType::Null: {
        ESP_RETURN_ON_FALSE(val->type == cJSON_NULL, ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        ESP_RETURN_ON_FALSE(writer.PutNull(tag) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to encode");
        break;
    }
    case TLVElementType::Array: {
        TLV::TLVType container_type;
        esp_err_t err = ESP_OK;
        size_t array_size = cJSON_GetArraySize(val);
        ESP_RETURN_ON_FALSE(val->type == cJSON_Array, ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        if (element_ctx.sub_type == TLV::TLVElementType::NotSpecified) {
            ESP_RETURN_ON_FALSE(array_size == 0, ESP_ERR_INVALID_ARG, TAG, "Invalid array size");
        }
        ESP_RETURN_ON_FALSE(writer.StartContainer(tag, TLV::kTLVType_Array, container_type) == CHIP_NO_ERROR, ESP_FAIL,
                            TAG, "Failed to start container");
        element_context nested_element_ctx;
        nested_element_ctx.tag = TLV::AnonymousTag();
        nested_element_ctx.type = element_ctx.sub_type;
        for (size_t i = 0; i < array_size; ++i) {
            if ((err = encode_tlv_element(cJSON_GetArrayItem(val, i), writer, nested_element_ctx)) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to encode");
                writer.EndContainer(container_type);
                return err;
            }
        }
        ESP_RETURN_ON_FALSE(writer.EndContainer(container_type) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to end container");
        break;
    }
    case TLVElementType::Structure: {
        TLV::TLVType container_type;
        esp_err_t err = ESP_OK;
        ESP_RETURN_ON_FALSE(val->type == cJSON_Object, ESP_ERR_INVALID_ARG, TAG, "Invalid type");
        size_t element_count = get_object_element_count(val);
        auto element_array = std::make_unique<element_context[]>(element_count);
        ESP_RETURN_ON_FALSE(element_array.get(), ESP_ERR_NO_MEM, TAG, "No memory for element_array");
        cJSON *element = val->child;
        size_t element_idx = 0;
        while (element && element_idx < element_count) {
            ESP_RETURN_ON_ERROR(parse_json_name(element->string, element_array[element_idx], writer.ImplicitProfileId),
                                TAG, "Failed to parse json name");
            element = element->next;
            element_idx++;
        }
        qsort(element_array.get(), element_count, sizeof(element_context), compare_by_tag);
        ESP_RETURN_ON_FALSE(writer.StartContainer(tag, TLV::kTLVType_Structure, container_type) == CHIP_NO_ERROR,
                            ESP_FAIL, TAG, "Failed to start container");
        for (element_idx = 0; element_idx < element_count; ++element_idx) {
            if ((err = encode_tlv_element(cJSON_GetObjectItem(val, element_array[element_idx].json_name), writer,
                                          element_array[element_idx])) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to encode");
                writer.EndContainer(container_type);
                return err;
            }
        }
        ESP_RETURN_ON_FALSE(writer.EndContainer(container_type) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to end container");
        break;
    }
    default:
        break;
    }
    return ESP_OK;
}

esp_err_t json_to_tlv(const char *json_str, chip::TLV::TLVWriter &writer, chip::TLV::Tag tag)
{
    cJSON *json = cJSON_Parse(json_str);
    esp_err_t err =  json_to_tlv(json, writer, tag);
    cJSON_Delete(json);
    return err;
}

esp_err_t json_to_tlv(cJSON *json, chip::TLV::TLVWriter &writer, chip::TLV::Tag tag)
{
    if (!json) {
        return ESP_ERR_INVALID_ARG;
    }
    if (json->type != cJSON_Object) {
        return ESP_ERR_INVALID_ARG;
    }
    element_context element_ctx;
    element_ctx.type = TLVElementType::Structure;
    element_ctx.sub_type = TLVElementType::NotSpecified;
    element_ctx.tag = tag;
    esp_err_t err = encode_tlv_element(json, writer, element_ctx);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to encode tlv element");
    }
    return err;
}

} // namespace esp_matter
