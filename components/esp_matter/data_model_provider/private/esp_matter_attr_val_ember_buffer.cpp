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

#include "esp_matter_attr_val_ember_buffer.h"
#include <esp_matter_attribute_utils.h>
#include <app-common/zap-generated/attribute-type.h>

uint8_t get_ember_attr_type_from_val_type(esp_matter_val_type_t val_type)
{
    switch (val_type) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BOOLEAN:
        return ZCL_BOOLEAN_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_FLOAT:
    case ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT:
        return ZCL_SINGLE_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_OCTET_STRING:
        return ZCL_OCTET_STRING_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_CHAR_STRING:
        return ZCL_CHAR_STRING_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING:
        return ZCL_LONG_OCTET_STRING_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING:
        return ZCL_LONG_CHAR_STRING_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_INT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT8:
        return ZCL_INT8S_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_BITMAP8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8:
        return ZCL_BITMAP8_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_ENUM8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8:
        return ZCL_ENUM8_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_UINT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT8:
        return ZCL_INT8U_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_INT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT16:
        return ZCL_INT16S_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_BITMAP16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16:
        return ZCL_BITMAP16_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_ENUM16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM16:
        return ZCL_ENUM16_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_UINT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT16:
        return ZCL_INT16U_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_INT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT32:
        return ZCL_INT32S_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_BITMAP32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32:
        return ZCL_BITMAP32_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_UINT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT32:
        return ZCL_INT32U_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_INT64:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT64:
        return ZCL_INT64S_ATTRIBUTE_TYPE;
    case ESP_MATTER_VAL_TYPE_UINT64:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT64:
        return ZCL_INT64U_ATTRIBUTE_TYPE;
    default:
        break;
    }
    return ZCL_NO_DATA_ATTRIBUTE_TYPE;
}

uint16_t get_ember_attr_size_from_val(const esp_matter_attr_val_t &val)
{
    switch (val.type) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BOOLEAN:
    case ESP_MATTER_VAL_TYPE_INT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT8:
    case ESP_MATTER_VAL_TYPE_BITMAP8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8:
    case ESP_MATTER_VAL_TYPE_ENUM8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8:
    case ESP_MATTER_VAL_TYPE_UINT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT8:
        return 1;
    case ESP_MATTER_VAL_TYPE_INT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT16:
    case ESP_MATTER_VAL_TYPE_BITMAP16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16:
    case ESP_MATTER_VAL_TYPE_ENUM16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM16:
    case ESP_MATTER_VAL_TYPE_UINT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT16:
        return 2;
    case ESP_MATTER_VAL_TYPE_INT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT32:
    case ESP_MATTER_VAL_TYPE_BITMAP32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32:
    case ESP_MATTER_VAL_TYPE_UINT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT32:
        return 4;
    case ESP_MATTER_VAL_TYPE_INT64:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT64:
    case ESP_MATTER_VAL_TYPE_UINT64:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT64:
        return 8;

    case ESP_MATTER_VAL_TYPE_FLOAT:
    case ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT:
        return sizeof(float);
    case ESP_MATTER_VAL_TYPE_OCTET_STRING:
    case ESP_MATTER_VAL_TYPE_CHAR_STRING:
        return (val.val.a.s == UINT8_MAX ? 0 : val.val.a.s) + 1;
    case ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING:
    case ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING:
        return (val.val.a.s == UINT16_MAX ? 0 : val.val.a.s) + 1;
    default:
        break;
    }
    return 0;
}

Status build_ember_buffer_from_attr_val(const esp_matter_attr_val_t &val, uint8_t *dataPtr, uint16_t bufferSize)
{
    switch (val.type) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BOOLEAN: {
        if (bufferSize < sizeof(bool) || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        using Traits = chip::app::NumericAttributeTraits<bool>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(*(uint8_t *)(&(val.val.b)))) {
            Traits::SetNull(*(uint8_t *)dataPtr);
        } else {
            Traits::WorkingToStorage(val.val.b, *dataPtr);
        }
        break;
    }

    case ESP_MATTER_VAL_TYPE_FLOAT:
    case ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT: {
        if (bufferSize < sizeof(float) || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        using Traits = chip::app::NumericAttributeTraits<float>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.f)) {
            Traits::SetNull(*(float *)dataPtr);
        } else {
            Traits::WorkingToStorage(val.val.f, *(float *)dataPtr);
        }
        break;
    }

    case ESP_MATTER_VAL_TYPE_OCTET_STRING:
    case ESP_MATTER_VAL_TYPE_CHAR_STRING: {
        if (bufferSize < val.val.a.t || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        uint8_t len = val.val.a.s;
        size_t header_size = sizeof(len);
        if (bufferSize < header_size) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        memcpy(dataPtr, &len, header_size);
        // UINT8_MAX is reserved for null value
        if (len < UINT8_MAX) {
            size_t max_payload = bufferSize - header_size;
            size_t copy_len = (static_cast<size_t>(len) <= max_payload) ? static_cast<size_t>(len) : max_payload;
            memcpy(dataPtr + header_size, val.val.a.b, copy_len);
        }
        break;
    }

    case ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING:
    case ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING: {
        if (bufferSize < val.val.a.t || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        size_t header_size = sizeof(val.val.a.s);
        if (bufferSize < header_size) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        memcpy(dataPtr, &val.val.a.s, header_size);
        // UINT16_MAX is reserved for null value
        if (val.val.a.s < UINT16_MAX) {
            size_t max_payload = bufferSize - header_size;
            size_t copy_len = (static_cast<size_t>(val.val.a.s) <= max_payload)
                              ? static_cast<size_t>(val.val.a.s)
                              : max_payload;
            memcpy(dataPtr + header_size, val.val.a.b, copy_len);
        }
        break;
    }

    case ESP_MATTER_VAL_TYPE_INT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT8: {
        if (bufferSize < sizeof(int8_t) || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        using Traits = chip::app::NumericAttributeTraits<int8_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.i8)) {
            Traits::SetNull(*(int8_t *)dataPtr);
        } else {
            Traits::WorkingToStorage(val.val.i8, *(int8_t *)dataPtr);
        }
        break;
    }
    case ESP_MATTER_VAL_TYPE_BITMAP8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8:
    case ESP_MATTER_VAL_TYPE_ENUM8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8:
    case ESP_MATTER_VAL_TYPE_UINT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT8: {
        if (bufferSize < sizeof(uint8_t) || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        using Traits = chip::app::NumericAttributeTraits<uint8_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.u8)) {
            Traits::SetNull(*dataPtr);
        } else {
            Traits::WorkingToStorage(val.val.u8, *dataPtr);
        }
        break;
    }

    case ESP_MATTER_VAL_TYPE_INT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT16: {
        if (bufferSize < sizeof(int16_t) || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        using Traits = chip::app::NumericAttributeTraits<int16_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.i16)) {
            Traits::SetNull(*(int16_t *)dataPtr);
        } else {
            Traits::WorkingToStorage(val.val.i16, *(int16_t *)dataPtr);
        }
        break;
    }

    case ESP_MATTER_VAL_TYPE_BITMAP16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16:
    case ESP_MATTER_VAL_TYPE_ENUM16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM16:
    case ESP_MATTER_VAL_TYPE_UINT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT16: {
        if (bufferSize < sizeof(uint16_t) || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        using Traits = chip::app::NumericAttributeTraits<uint16_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.u16)) {
            Traits::SetNull(*(uint16_t *)dataPtr);
        } else {
            Traits::WorkingToStorage(val.val.u16, *(uint16_t *)dataPtr);
        }
        break;
    }

    case ESP_MATTER_VAL_TYPE_INT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT32: {
        if (bufferSize < sizeof(int32_t) || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        using Traits = chip::app::NumericAttributeTraits<int32_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.i32)) {
            Traits::SetNull(*(int32_t *)dataPtr);
        } else {
            Traits::WorkingToStorage(val.val.i32, *(int32_t *)dataPtr);
        }
        break;
    }

    case ESP_MATTER_VAL_TYPE_BITMAP32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32:
    case ESP_MATTER_VAL_TYPE_UINT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT32: {
        if (bufferSize < sizeof(uint32_t) || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        using Traits = chip::app::NumericAttributeTraits<uint32_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.u32)) {
            Traits::SetNull(*(uint32_t *)dataPtr);
        } else {
            Traits::WorkingToStorage(val.val.u32, *(uint32_t *)dataPtr);
        }
        break;
    }

    case ESP_MATTER_VAL_TYPE_INT64:
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT64: {
        if (bufferSize < sizeof(int64_t) || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        using Traits = chip::app::NumericAttributeTraits<int64_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.i64)) {
            Traits::SetNull(*(int64_t *)dataPtr);
        } else {
            Traits::WorkingToStorage(val.val.i64, *(int64_t *)dataPtr);
        }
        break;
    }

    case ESP_MATTER_VAL_TYPE_UINT64:
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT64: {
        if (bufferSize < sizeof(uint64_t) || !dataPtr) {
            return chip::Protocols::InteractionModel::Status::ResourceExhausted;
        }
        using Traits = chip::app::NumericAttributeTraits<uint64_t>;
        if ((val.type & ESP_MATTER_VAL_NULLABLE_BASE) && Traits::IsNullValue(val.val.u64)) {
            Traits::SetNull(*(uint64_t *)dataPtr);
        } else {
            Traits::WorkingToStorage(val.val.u64, *(uint64_t *)dataPtr);
        }
        break;
    }

    default:
        return chip::Protocols::InteractionModel::Status::InvalidDataType;
    }
    return chip::Protocols::InteractionModel::Status::Success;
}
