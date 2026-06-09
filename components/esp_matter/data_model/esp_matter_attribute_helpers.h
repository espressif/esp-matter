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

#pragma once

#include <esp_matter_data_model_priv.h>

#include <lib/core/DataModelTypes.h>
#include <lib/support/CodeUtils.h>

#include <type_traits>

namespace esp_matter {

/** Read FeatureMap from esp-matter attribute storage for the given endpoint and cluster. */
uint32_t read_feature_map_u32(chip::EndpointId endpointId, chip::ClusterId clusterId);

/**
 * Read a fixed-width scalar `T` from esp-matter attribute storage for (endpoint, cluster, attribute).
 *
 * `T` must be one of: bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t.
 *
 * Returns true and writes @p out on success. Returns false if the attribute is missing, the read fails,
 * or the stored type does not match (uint8_t accepts UINT8, ENUM8, or BITMAP8; uint16_t accepts UINT16, ENUM16, or BITMAP16;
 * uint32_t accepts UINT32 or BITMAP32).
 *
 * Uses `attribute::get_val_internal` (same path as legacy integration reads). For Interaction Model reads,
 * prefer `chip::app::Clusters::…::Attributes::…::Get` when appropriate.
 */
template<typename T>
bool read_attribute_raw_value(chip::EndpointId endpointId, chip::ClusterId clusterId, chip::AttributeId attributeId, T  &out)
{
    static_assert(std::is_same_v<T, bool> || std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t> ||
                  std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, int32_t> ||
                  std::is_same_v<T, uint32_t>,
                  "esp_matter::read_attribute_raw_value: T must be a supported scalar type");

    attribute_t *attr = attribute::get(endpointId, clusterId, attributeId);
    VerifyOrReturnValue(attr != nullptr, false);
    esp_matter_attr_val_t val;
    VerifyOrReturnValue(attribute::get_val_internal(attr, &val) == ESP_OK, false);

    if constexpr(std::is_same_v<T, bool>) {
        VerifyOrReturnValue(val.type == ESP_MATTER_VAL_TYPE_BOOLEAN, false);
        out = val.val.b;
    } else if constexpr(std::is_same_v<T, int8_t>) {
        VerifyOrReturnValue(val.type == ESP_MATTER_VAL_TYPE_INT8, false);
        out = val.val.i8;
    } else if constexpr(std::is_same_v<T, uint8_t>) {
        VerifyOrReturnValue(val.type == ESP_MATTER_VAL_TYPE_UINT8 || val.type == ESP_MATTER_VAL_TYPE_ENUM8 ||
                            val.type == ESP_MATTER_VAL_TYPE_BITMAP8,
                            false);
        out = val.val.u8;
    } else if constexpr(std::is_same_v<T, int16_t>) {
        VerifyOrReturnValue(val.type == ESP_MATTER_VAL_TYPE_INT16, false);
        out = val.val.i16;
    } else if constexpr(std::is_same_v<T, uint16_t>) {
        VerifyOrReturnValue(val.type == ESP_MATTER_VAL_TYPE_UINT16 || val.type == ESP_MATTER_VAL_TYPE_ENUM16 ||
                            val.type == ESP_MATTER_VAL_TYPE_BITMAP16,
                            false);
        out = val.val.u16;
    } else if constexpr(std::is_same_v<T, int32_t>) {
        VerifyOrReturnValue(val.type == ESP_MATTER_VAL_TYPE_INT32, false);
        out = val.val.i32;
    } else if constexpr(std::is_same_v<T, uint32_t>) {
        VerifyOrReturnValue(val.type == ESP_MATTER_VAL_TYPE_UINT32 || val.type == ESP_MATTER_VAL_TYPE_BITMAP32, false);
        out = val.val.u32;
    }
    return true;
}

} // namespace esp_matter
