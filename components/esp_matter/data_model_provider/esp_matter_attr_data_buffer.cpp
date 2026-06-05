// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_matter_attr_data_buffer.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_mem.h>

#include <lib/support/CodeUtils.h>

namespace esp_matter {
namespace data_model {

CHIP_ERROR attribute_data_decode_buffer::Decode(chip::TLV::TLVReader &reader)
{
    switch (m_attr_val.get_storage_type()) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        if (reader.GetType() == chip::TLV::kTLVType_Null && m_attr_val.is_nullable()) {
            chip::app::NumericAttributeTraits<bool>::SetNull(*(uint8_t *)(&(m_attr_val.val.b)));
        } else {
            ReturnErrorOnFailure(reader.Get(m_attr_val.val.b));
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT8:
        if (reader.GetType() == chip::TLV::kTLVType_Null && m_attr_val.is_nullable()) {
            chip::app::NumericAttributeTraits<uint8_t>::SetNull(m_attr_val.val.u8);
        } else {
            ReturnErrorOnFailure(reader.Get(m_attr_val.val.u8));
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT16:
        if (reader.GetType() == chip::TLV::kTLVType_Null && m_attr_val.is_nullable()) {
            chip::app::NumericAttributeTraits<uint16_t>::SetNull(m_attr_val.val.u16);
        } else {
            ReturnErrorOnFailure(reader.Get(m_attr_val.val.u16));
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT32:
        if (reader.GetType() == chip::TLV::kTLVType_Null && m_attr_val.is_nullable()) {
            chip::app::NumericAttributeTraits<uint32_t>::SetNull(m_attr_val.val.u32);
        } else {
            ReturnErrorOnFailure(reader.Get(m_attr_val.val.u32));
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT64:
        if (reader.GetType() == chip::TLV::kTLVType_Null && m_attr_val.is_nullable()) {
            chip::app::NumericAttributeTraits<uint64_t>::SetNull(m_attr_val.val.u64);
        } else {
            ReturnErrorOnFailure(reader.Get(m_attr_val.val.u64));
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT8:
        if (reader.GetType() == chip::TLV::kTLVType_Null && m_attr_val.is_nullable()) {
            chip::app::NumericAttributeTraits<int8_t>::SetNull(m_attr_val.val.i8);
        } else {
            ReturnErrorOnFailure(reader.Get(m_attr_val.val.i8));
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT16:
        if (reader.GetType() == chip::TLV::kTLVType_Null && m_attr_val.is_nullable()) {
            chip::app::NumericAttributeTraits<int16_t>::SetNull(m_attr_val.val.i16);
        } else {
            ReturnErrorOnFailure(reader.Get(m_attr_val.val.i16));
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT32:
        if (reader.GetType() == chip::TLV::kTLVType_Null && m_attr_val.is_nullable()) {
            chip::app::NumericAttributeTraits<int32_t>::SetNull(m_attr_val.val.i32);
        } else {
            ReturnErrorOnFailure(reader.Get(m_attr_val.val.i32));
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT64:
        if (reader.GetType() == chip::TLV::kTLVType_Null && m_attr_val.is_nullable()) {
            chip::app::NumericAttributeTraits<int64_t>::SetNull(m_attr_val.val.i64);
        } else {
            ReturnErrorOnFailure(reader.Get(m_attr_val.val.i64));
        }
        break;
    case ESP_MATTER_VAL_TYPE_FLOAT:
        if (reader.GetType() == chip::TLV::kTLVType_Null && m_attr_val.is_nullable()) {
            chip::app::NumericAttributeTraits<float>::SetNull(m_attr_val.val.f);
        } else {
            ReturnErrorOnFailure(reader.Get(m_attr_val.val.f));
        }
        break;
    case ESP_MATTER_VAL_TYPE_CHAR_STRING:
    case ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING: {
        if (reader.GetType() == chip::TLV::kTLVType_Null) {
            m_attr_val.val.a.b = nullptr;
            m_attr_val.val.a.s = (m_attr_val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ? 0xFF : 0xFFFF);
            m_attr_val.val.a.t = m_attr_val.val.a.s;
            break;
        }
        uint32_t len = reader.GetLength();
        if (len > 0xFF && m_attr_val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING) {
            return CHIP_ERROR_INVALID_ARGUMENT;
        }
        // The allocated buffer will be freed in destructor
        m_attr_val.val.a.b = (uint8_t *)esp_matter_mem_calloc(len + 1, sizeof(char));
        if (!m_attr_val.val.a.b) {
            return CHIP_ERROR_NO_MEMORY;
        }
        ReturnErrorOnFailure(reader.GetString((char *)m_attr_val.val.a.b, len + 1));
        m_attr_val.val.a.s = len;
        m_attr_val.val.a.t = len + (m_attr_val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ? 1 : 2);
        break;
    }
    case ESP_MATTER_VAL_TYPE_OCTET_STRING:
    case ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING: {
        if (reader.GetType() == chip::TLV::kTLVType_Null) {
            m_attr_val.val.a.b = nullptr;
            m_attr_val.val.a.s = (m_attr_val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ? 0xFF : 0xFFFF);
            m_attr_val.val.a.t = m_attr_val.val.a.s;
            break;
        }
        uint32_t len = reader.GetLength();
        if (len > 0xFF && m_attr_val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING) {
            return CHIP_ERROR_INVALID_ARGUMENT;
        }
        // The allocated buffer will be freed in destructor
        m_attr_val.val.a.b = (uint8_t *)esp_matter_mem_calloc(len, sizeof(uint8_t));
        if (!m_attr_val.val.a.b) {
            return CHIP_ERROR_NO_MEMORY;
        }
        ReturnErrorOnFailure(reader.GetBytes(m_attr_val.val.a.b, len));
        m_attr_val.val.a.s = len;
        m_attr_val.val.a.t = len + (m_attr_val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ? 1 : 2);
        break;
    }
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR attribute_data_encode_buffer::Encode(chip::TLV::TLVWriter &writer, chip::TLV::Tag tag) const
{
    switch (m_attr_val.get_storage_type()) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        if (m_attr_val.is_nullable() && chip::app::NumericAttributeTraits<bool>::IsNullValue(m_attr_val.val.b)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
        } else {
            ReturnErrorOnFailure(writer.Put(tag, m_attr_val.val.b));
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT8:
        if (m_attr_val.is_nullable() && chip::app::NumericAttributeTraits<uint8_t>::IsNullValue(m_attr_val.val.u8)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
        } else {
            ReturnErrorOnFailure(writer.Put(tag, m_attr_val.val.u8));
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT16:
        if (m_attr_val.is_nullable() && chip::app::NumericAttributeTraits<uint16_t>::IsNullValue(m_attr_val.val.u16)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
        } else {
            ReturnErrorOnFailure(writer.Put(tag, m_attr_val.val.u16));
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT32:
        if (m_attr_val.is_nullable() && chip::app::NumericAttributeTraits<uint32_t>::IsNullValue(m_attr_val.val.u32)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
        } else {
            ReturnErrorOnFailure(writer.Put(tag, m_attr_val.val.u32));
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT64:
        if (m_attr_val.is_nullable() && chip::app::NumericAttributeTraits<uint64_t>::IsNullValue(m_attr_val.val.u64)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
        } else {
            ReturnErrorOnFailure(writer.Put(tag, m_attr_val.val.u64));
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT8:
        if (m_attr_val.is_nullable() && chip::app::NumericAttributeTraits<int8_t>::IsNullValue(m_attr_val.val.i8)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
        } else {
            ReturnErrorOnFailure(writer.Put(tag, m_attr_val.val.i8));
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT16:
        if (m_attr_val.is_nullable() && chip::app::NumericAttributeTraits<int16_t>::IsNullValue(m_attr_val.val.i16)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
        } else {
            ReturnErrorOnFailure(writer.Put(tag, m_attr_val.val.i16));
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT32:
        if (m_attr_val.is_nullable() && chip::app::NumericAttributeTraits<int32_t>::IsNullValue(m_attr_val.val.i32)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
        } else {
            ReturnErrorOnFailure(writer.Put(tag, m_attr_val.val.i32));
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT64:
        if (m_attr_val.is_nullable() && chip::app::NumericAttributeTraits<int64_t>::IsNullValue(m_attr_val.val.i64)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
        } else {
            ReturnErrorOnFailure(writer.Put(tag, m_attr_val.val.i64));
        }
        break;
    case ESP_MATTER_VAL_TYPE_FLOAT:
        if (m_attr_val.is_nullable() && chip::app::NumericAttributeTraits<float>::IsNullValue(m_attr_val.val.f)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
        } else {
            ReturnErrorOnFailure(writer.Put(tag, m_attr_val.val.f));
        }
        break;
    case ESP_MATTER_VAL_TYPE_CHAR_STRING:
    case ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING: {
        if (m_attr_val.val.a.b == nullptr &&
                m_attr_val.val.a.s == (m_attr_val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ? 0xFF : 0xFFFF)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
            break;
        }
        if (m_attr_val.val.a.b == nullptr) {
            ReturnErrorOnFailure(writer.PutString(tag, ""));
        } else {
            ReturnErrorOnFailure(writer.PutString(tag, (char *)m_attr_val.val.a.b));
        }
        break;
    }
    case ESP_MATTER_VAL_TYPE_OCTET_STRING:
    case ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING: {
        if (m_attr_val.val.a.b == nullptr &&
                m_attr_val.val.a.s == (m_attr_val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ? 0xFF : 0xFFFF)) {
            ReturnErrorOnFailure(writer.PutNull(tag));
            break;
        }
        ReturnErrorOnFailure(writer.PutBytes(tag, m_attr_val.val.a.b, m_attr_val.val.a.s));
        break;
    }
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    return CHIP_NO_ERROR;
}

} // namespace data_model
} // namespace esp_matter
