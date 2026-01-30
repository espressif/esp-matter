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

#include "attribute_value_pair_validator.h"
#include <esp_matter_attribute_utils.h>
#include <esp_matter_data_model.h>
#include <esp_matter_data_model_priv.h>

namespace chip::scenes {

namespace {

using AttributeValuePairType = app::Clusters::ScenesManagement::Structs::AttributeValuePairStruct::Type;

template <typename Type>
typename app::NumericAttributeTraits<Type>::WorkingType ConvertDefaultValueToWorkingValue(
    const esp_matter_attr_val_t &attr_val)
{
    switch (attr_val.type & (~ESP_MATTER_VAL_NULLABLE_BASE)) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        return static_cast<typename app::NumericAttributeTraits<Type>::WorkingType>(attr_val.val.b);
    case ESP_MATTER_VAL_TYPE_INT8:
        return static_cast<typename app::NumericAttributeTraits<Type>::WorkingType>(attr_val.val.i8);
    case ESP_MATTER_VAL_TYPE_INT16:
        return static_cast<typename app::NumericAttributeTraits<Type>::WorkingType>(attr_val.val.i16);
    case ESP_MATTER_VAL_TYPE_INT32:
        return static_cast<typename app::NumericAttributeTraits<Type>::WorkingType>(attr_val.val.i32);
    case ESP_MATTER_VAL_TYPE_INT64:
        return static_cast<typename app::NumericAttributeTraits<Type>::WorkingType>(attr_val.val.i64);
    case ESP_MATTER_VAL_TYPE_UINT8:
    case ESP_MATTER_VAL_TYPE_ENUM8:
    case ESP_MATTER_VAL_TYPE_BITMAP8:
        return static_cast<typename app::NumericAttributeTraits<Type>::WorkingType>(attr_val.val.u8);
    case ESP_MATTER_VAL_TYPE_UINT16:
    case ESP_MATTER_VAL_TYPE_ENUM16:
    case ESP_MATTER_VAL_TYPE_BITMAP16:
        return static_cast<typename app::NumericAttributeTraits<Type>::WorkingType>(attr_val.val.u16);
    case ESP_MATTER_VAL_TYPE_UINT32:
    case ESP_MATTER_VAL_TYPE_BITMAP32:
        return static_cast<typename app::NumericAttributeTraits<Type>::WorkingType>(attr_val.val.u32);
    case ESP_MATTER_VAL_TYPE_UINT64:
        return static_cast<typename app::NumericAttributeTraits<Type>::WorkingType>(attr_val.val.u64);
    case ESP_MATTER_VAL_TYPE_FLOAT:
        return static_cast<typename app::NumericAttributeTraits<Type>::WorkingType>(attr_val.val.f);
    default:
        return 0;
    }
    return 0;
}
/// IsExactlyOneValuePopulated
/// @brief Helper function to verify that exactly one value is populated in a given AttributeValuePairType
/// @param AttributeValuePairType & type AttributeValuePairType to verify
/// @return bool true if only one value is populated, false otherwise
bool IsExactlyOneValuePopulated(const AttributeValuePairType &type)
{
    int count = 0;
    if (type.valueUnsigned8.HasValue())
        count++;
    if (type.valueSigned8.HasValue())
        count++;
    if (type.valueUnsigned16.HasValue())
        count++;
    if (type.valueSigned16.HasValue())
        count++;
    if (type.valueUnsigned32.HasValue())
        count++;
    if (type.valueSigned32.HasValue())
        count++;
    if (type.valueUnsigned64.HasValue())
        count++;
    if (type.valueSigned64.HasValue())
        count++;
    return count == 1;
}

/// CapAttributeValue
/// Cap the attribute value based on the attribute's min and max if they are defined,
/// or based on the attribute's size if they are not.
///
/// The TypeForMinMax template parameter determines the type to use for the
/// min/max computation.  The Type template parameter determines how the
/// resulting value is actually represented, because for booleans we
/// unfortunately end up using uint8, not an actual boolean.
///
/// @param[in] value   The value from the AttributeValuePairType that we were given.
/// @param[in] metadata  The metadata for the attribute the value is for.
///
///
///
template <typename Type, typename TypeForMinMax = Type>
void CapAttributeValue(typename app::NumericAttributeTraits<Type>::WorkingType &value, esp_matter::attribute_t *attr)
{
    using IntType = app::NumericAttributeTraits<TypeForMinMax>;
    using WorkingType = std::remove_reference_t<decltype(value)>;
    esp_matter_attr_val_t attr_val;
    if (esp_matter::attribute::get_val_internal(attr, &attr_val) != ESP_OK) {
        return;
    }

    bool is_nullable = (attr_val.type & ESP_MATTER_VAL_NULLABLE_BASE);
    WorkingType minValue = IntType::MinValue(is_nullable);
    WorkingType maxValue = IntType::MaxValue(is_nullable);

    // Check metadata for min and max values
    esp_matter_attr_bounds_t bounds;
    if (esp_matter::attribute::get_bounds(attr, &bounds)) {
        minValue = ConvertDefaultValueToWorkingValue<Type>(bounds.min);
        maxValue = ConvertDefaultValueToWorkingValue<Type>(bounds.max);
    }

    if (is_nullable && (minValue > value || maxValue < value)) {
        // If the attribute is nullable, the value can be set to NULL
        app::NumericAttributeTraits<WorkingType>::SetNull(value);
        return;
    }

    if (minValue > value) {
        value = minValue;
    } else if (maxValue < value) {
        value = maxValue;
    }
}
} // namespace
CHIP_ERROR CodegenAttributeValuePairValidator::Validate(const app::ConcreteClusterPath &clusterPath,
                                                        AttributeValuePairType &value)
{
    esp_matter::attribute_t *attr =
        esp_matter::attribute::get(clusterPath.mEndpointId, clusterPath.mClusterId, value.attributeID);
    esp_matter_attr_val_t attr_val;
    if (nullptr == attr || esp_matter::attribute::get_val_internal(attr, &attr_val) != ESP_OK) {
        return CHIP_IM_GLOBAL_STATUS(UnsupportedAttribute);
    }

    // There should never be more than one populated value in an ExtensionFieldSet
    VerifyOrReturnError(IsExactlyOneValuePopulated(value), CHIP_ERROR_INVALID_ARGUMENT);

    switch (attr_val.type & (~ESP_MATTER_VAL_NULLABLE_BASE)) {
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        VerifyOrReturnError(value.valueUnsigned8.HasValue(), CHIP_ERROR_INVALID_ARGUMENT);
        CapAttributeValue<uint8_t, bool>(value.valueUnsigned8.Value(), attr);
        break;
    case ESP_MATTER_VAL_TYPE_UINT8:
    case ESP_MATTER_VAL_TYPE_ENUM8:
    case ESP_MATTER_VAL_TYPE_BITMAP8:
        VerifyOrReturnError(value.valueUnsigned8.HasValue(), CHIP_ERROR_INVALID_ARGUMENT);
        CapAttributeValue<uint8_t>(value.valueUnsigned8.Value(), attr);
        break;
    case ESP_MATTER_VAL_TYPE_UINT16:
    case ESP_MATTER_VAL_TYPE_ENUM16:
    case ESP_MATTER_VAL_TYPE_BITMAP16:
        VerifyOrReturnError(value.valueUnsigned16.HasValue(), CHIP_ERROR_INVALID_ARGUMENT);
        CapAttributeValue<uint16_t>(value.valueUnsigned16.Value(), attr);
        break;
    case ESP_MATTER_VAL_TYPE_UINT32:
    case ESP_MATTER_VAL_TYPE_BITMAP32:
        VerifyOrReturnError(value.valueUnsigned32.HasValue(), CHIP_ERROR_INVALID_ARGUMENT);
        CapAttributeValue<uint32_t>(value.valueUnsigned32.Value(), attr);
        break;
    case ESP_MATTER_VAL_TYPE_UINT64:
        VerifyOrReturnError(value.valueUnsigned64.HasValue(), CHIP_ERROR_INVALID_ARGUMENT);
        CapAttributeValue<uint64_t>(value.valueUnsigned64.Value(), attr);
        break;
    case ESP_MATTER_VAL_TYPE_INT8:
        VerifyOrReturnError(value.valueSigned8.HasValue(), CHIP_ERROR_INVALID_ARGUMENT);
        CapAttributeValue<int8_t>(value.valueSigned8.Value(), attr);
        break;
    case ESP_MATTER_VAL_TYPE_INT16:
        VerifyOrReturnError(value.valueSigned16.HasValue(), CHIP_ERROR_INVALID_ARGUMENT);
        CapAttributeValue<int16_t>(value.valueSigned16.Value(), attr);
        break;
    case ESP_MATTER_VAL_TYPE_INT32:
        VerifyOrReturnError(value.valueSigned32.HasValue(), CHIP_ERROR_INVALID_ARGUMENT);
        CapAttributeValue<int32_t>(value.valueSigned32.Value(), attr);
        break;
    case ESP_MATTER_VAL_TYPE_INT64:
        VerifyOrReturnError(value.valueSigned64.HasValue(), CHIP_ERROR_INVALID_ARGUMENT);
        CapAttributeValue<int64_t>(value.valueSigned64.Value(), attr);
        break;
    default:
        return CHIP_IM_GLOBAL_STATUS(UnsupportedAttribute);
    }

    return CHIP_NO_ERROR;
}
} // namespace chip::scenes
