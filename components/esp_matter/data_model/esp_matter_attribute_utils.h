// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include "support/CodeUtils.h"
#include <esp_err.h>
#include <stdbool.h>
#include <stdint.h>

#include <app/data-model/Nullable.h>

template <typename T>
class nullable {
    /** NOTE: GetNullValue is taken from src/app/util/attribute-storage-null-handling.h */
private:
    template <typename U = T, typename std::enable_if_t<std::is_floating_point<U>::value, int> = 0>
    static constexpr T GetNullValue()
    {
        return std::numeric_limits<T>::quiet_NaN();
    }

    template <typename U = T, typename std::enable_if_t<std::is_integral<U>::value, int> = 0>
    static constexpr T GetNullValue()
    {
        return std::is_signed<T>::value ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max();
    }

    template <typename U = T, typename std::enable_if_t<std::is_enum<U>::value, int> = 0>
    static constexpr T GetNullValue()
    {
        static_assert(!std::is_signed<std::underlying_type_t<T>>::value, "Enums must be unsigned");
        return static_cast<T>(std::numeric_limits<std::underlying_type_t<T>>::max());
    }

public:
    nullable(T value)
    {
        if (chip::app::NumericAttributeTraits<T>::IsNullValue(value)) {
            chip::app::NumericAttributeTraits<T>::SetNull(val);
        } else {
            val = value;
        }
    }

    nullable()
    {
        chip::app::NumericAttributeTraits<T>::SetNull(val);
    }

    T value() const
    {
        if (is_null()) {
            return GetNullValue();
        } else {
            return val;
        }
    }

    T value_or(T ret) const
    {
        return is_null() ? ret : val;
    }

    bool is_null() const
    {
        return chip::app::NumericAttributeTraits<T>::IsNullValue(val);
    }

    void operator=(T value)
    {
        if (chip::app::NumericAttributeTraits<T>::IsNullValue(value)) {
            chip::app::NumericAttributeTraits<T>::SetNull(this->val);
        } else {
            this->val = value;
        }
    }

    void operator=(std::nullptr_t)
    {
        chip::app::NumericAttributeTraits<T>::SetNull(this->val);
    }

    static void set_null(T &val)
    {
        chip::app::NumericAttributeTraits<T>::SetNull(val);
    }

private:
    T val;
};

/** Template specialization for bool */
template <>
class nullable<bool> {
    using Traits = chip::app::NumericAttributeTraits<bool>;
    using StorageType = typename Traits::StorageType; // Resolves to uint8_t

    static_assert(std::is_same_v<StorageType, uint8_t>,
                  "nullable<bool> specialization assumes bool StorageType is uint8_t. Revisit if this changes.");

public:
    /** Constructors */
    nullable()
    {
        Traits::SetNull(storage);
    }
    nullable(std::nullptr_t)
    {
        Traits::SetNull(storage);
    }
    constexpr nullable(bool v)
        : storage(static_cast<StorageType>(v))
    {
    }

    /** Observers */
    constexpr bool is_null() const
    {
        return Traits::IsNullValue(storage);
    }

    /**
     * @brief Gets the stored bool value.
     *
     * Returns the boolean interpretation of the internal storage.
     * @warning Returns `true` when `is_null()` is true. Check `is_null()` first
     *          if the distinction between null and `true` is important.
     *
     * Example:
     * ```
     * nullable<bool> v_true(true);   // v_true.value() == true
     * nullable<bool> v_false(false); // v_false.value() == false
     * nullable<bool> v_null;         // v_null.value() == true (because internal null 0xFF is non-zero)
     * ```
     * @return `true` if storage is non-zero (includes null state), `false` if storage is zero.
     */
    constexpr bool value() const
    {
        return static_cast<bool>(storage);
    }

    constexpr bool value_or(bool d) const
    {
        return is_null() ? d : static_cast<bool>(storage);
    }

    /** Modifiers */
    void operator=(std::nullptr_t)
    {
        Traits::SetNull(storage);
    }
    void operator=(bool v)
    {
        storage = static_cast<StorageType>(v);
    }

    static void set_null(bool &val)
    {
        chip::app::NumericAttributeTraits<bool>::SetNull(*((uint8_t *)&val));
    }

private:
    StorageType storage; // 1‑byte payload holding 0 / 1 / 0xFF
};

/* Nullable base for nullable attribute */
#define ESP_MATTER_VAL_NULLABLE_BASE 0x80

/** ESP Matter Attribute Value type */
typedef enum {
    /** Invalid */
    ESP_MATTER_VAL_TYPE_INVALID = 0,
    /** Boolean */
    ESP_MATTER_VAL_TYPE_BOOLEAN = 1,
    /** Integer. Mapped to a 32 bit signed integer, deprecated as it is duplicated with uint32 type */
    ESP_MATTER_VAL_TYPE_INTEGER __attribute__((deprecated("Use ESP_MATTER_VAL_TYPE_INT32 instead"))) = 2,
    /** Floating point number */
    ESP_MATTER_VAL_TYPE_FLOAT = 3,
    /** Array Eg. [1,2,3] */
    ESP_MATTER_VAL_TYPE_ARRAY = 4,
    /** Char String Eg. "123", Max length 0xFE */
    ESP_MATTER_VAL_TYPE_CHAR_STRING = 5,
    /** Octet String Eg. [0x01, 0x20], Max length 0xFE */
    ESP_MATTER_VAL_TYPE_OCTET_STRING = 6,
    /** 8 bit signed integer */
    ESP_MATTER_VAL_TYPE_INT8 = 7,
    /** 8 bit unsigned integer */
    ESP_MATTER_VAL_TYPE_UINT8 = 8,
    /** 16 bit signed integer */
    ESP_MATTER_VAL_TYPE_INT16 = 9,
    /** 16 bit unsigned integer */
    ESP_MATTER_VAL_TYPE_UINT16 = 10,
    /** 32 bit signed integer */
    ESP_MATTER_VAL_TYPE_INT32 = 11,
    /** 32 bit unsigned integer */
    ESP_MATTER_VAL_TYPE_UINT32 = 12,
    /** 64 bit signed integer */
    ESP_MATTER_VAL_TYPE_INT64 = 13,
    /** 64 bit unsigned integer */
    ESP_MATTER_VAL_TYPE_UINT64 = 14,
    /** 8 bit enum */
    ESP_MATTER_VAL_TYPE_ENUM8 = 15,
    /** 8 bit bitmap */
    ESP_MATTER_VAL_TYPE_BITMAP8 = 16,
    /** 16 bit bitmap */
    ESP_MATTER_VAL_TYPE_BITMAP16 = 17,
    /** 32 bit bitmap */
    ESP_MATTER_VAL_TYPE_BITMAP32 = 18,
    /** 16 bit enum */
    ESP_MATTER_VAL_TYPE_ENUM16 = 19,
    /** Long Char String, Max length 0xFFFE **/
    ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING = 20,
    /** Long Octet String, Max length 0xFFFE **/
    ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING = 21,

    /** nullable types **/
    ESP_MATTER_VAL_TYPE_NULLABLE_BOOLEAN = ESP_MATTER_VAL_TYPE_BOOLEAN + ESP_MATTER_VAL_NULLABLE_BASE,
    /* Use 2 to avoid warnings */
    ESP_MATTER_VAL_TYPE_NULLABLE_INTEGER __attribute__((deprecated("Use ESP_MATTER_VAL_TYPE_NULLABLE_INT32 instead"))) = 2 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT = ESP_MATTER_VAL_TYPE_FLOAT + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_INT8 = ESP_MATTER_VAL_TYPE_INT8 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_UINT8 = ESP_MATTER_VAL_TYPE_UINT8 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_INT16 = ESP_MATTER_VAL_TYPE_INT16 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_UINT16 = ESP_MATTER_VAL_TYPE_UINT16 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_INT32 = ESP_MATTER_VAL_TYPE_INT32 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_UINT32 = ESP_MATTER_VAL_TYPE_UINT32 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_INT64 = ESP_MATTER_VAL_TYPE_INT64 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_UINT64 = ESP_MATTER_VAL_TYPE_UINT64 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8 = ESP_MATTER_VAL_TYPE_ENUM8 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8 = ESP_MATTER_VAL_TYPE_BITMAP8 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16 = ESP_MATTER_VAL_TYPE_BITMAP16 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32 = ESP_MATTER_VAL_TYPE_BITMAP32 + ESP_MATTER_VAL_NULLABLE_BASE,
    ESP_MATTER_VAL_TYPE_NULLABLE_ENUM16 = ESP_MATTER_VAL_TYPE_ENUM16 + ESP_MATTER_VAL_NULLABLE_BASE,
} esp_matter_val_type_t;

/** ESP Matter Value */
union esp_matter_val {
    /** Boolean */
    bool b;
    /** Integer (deprecated)*/
    int i __attribute__((deprecated("Use i32 instead")));
    /** Float */
    float f;
    /** 8 bit signed integer */
    int8_t i8;
    /** 8 bit unsigned integer */
    uint8_t u8;
    /** 16 bit signed integer */
    int16_t i16;
    /** 16 bit unsigned integer */
    uint16_t u16;
    /** 32 bit signed integer */
    int32_t i32;
    /** 32 bit unsigned integer */
    uint32_t u32;
    /** 64 bit signed integer */
    int64_t i64;
    /** 64 bit unsigned integer */
    uint64_t u64;
    /** Array */
    struct {
        /** Buffer */
        uint8_t *b;
        /** Data size */
        uint16_t s;
        /** Data max size */
        uint16_t max;
        /** Total size */
        uint16_t t;
    } a;
    /** Pointer */
    void *p;
    /** Constructors */
    esp_matter_val()
        : p(nullptr) {}
    esp_matter_val(void *p_val)
        : p(p_val) {}
    esp_matter_val(bool b_val)
        : b(b_val) {}
    esp_matter_val(float f_val)
        : f(f_val) {}
    esp_matter_val(int8_t i8_val)
        : i8(i8_val) {}
    esp_matter_val(int16_t i16_val)
        : i16(i16_val) {}
    esp_matter_val(int32_t i32_val)
        : i32(i32_val) {}
    esp_matter_val(int64_t i64_val)
        : i64(i64_val) {}
    esp_matter_val(uint8_t u8_val)
        : u8(u8_val) {}
    esp_matter_val(uint16_t u16_val)
        : u16(u16_val) {}
    esp_matter_val(uint32_t u32_val)
        : u32(u32_val) {}
    esp_matter_val(uint64_t u64_val)
        : u64(u64_val) {}
    esp_matter_val(nullable<bool> b_val) {
        if (b_val.is_null()) {
            nullable<bool>::set_null(b);
        } else {
            b = b_val.value();
        }
    }
    esp_matter_val(nullable<float> f_val) {
        if (f_val.is_null()) {
            nullable<float>::set_null(f);
        } else {
            f = f_val.value();
        }
    }
    esp_matter_val(nullable<int8_t> i8_val) {
        if (i8_val.is_null()) {
            nullable<int8_t>::set_null(i8);
        } else {
            i8 = i8_val.value();
        }
    }
    esp_matter_val(nullable<int16_t> i16_val) {
        if (i16_val.is_null()) {
            nullable<int16_t>::set_null(i16);
        } else {
            i16 = i16_val.value();
        }
    }
    esp_matter_val(nullable<int32_t> i32_val) {
        if (i32_val.is_null()) {
            nullable<int32_t>::set_null(i32);
        } else {
            i32 = i32_val.value();
        }
    }
    esp_matter_val(nullable<int64_t> i64_val) {
        if (i64_val.is_null()) {
            nullable<int64_t>::set_null(i64);
        } else {
            i64 = i64_val.value();
        }
    }
    esp_matter_val(nullable<uint8_t> u8_val) {
        if (u8_val.is_null()) {
            nullable<uint8_t>::set_null(u8);
        } else {
            u8 = u8_val.value();
        }
    }
    esp_matter_val(nullable<uint16_t> u16_val) {
        if (u16_val.is_null()) {
            nullable<uint16_t>::set_null(u16);
        } else {
            u16 = u16_val.value();
        }
    }
    esp_matter_val(nullable<uint32_t> u32_val) {
        if (u32_val.is_null()) {
            nullable<uint32_t>::set_null(u32);
        } else {
            u32 = u32_val.value();
        }
    }
    esp_matter_val(nullable<uint64_t> u64_val) {
        if (u64_val.is_null()) {
            nullable<uint64_t>::set_null(u64);
        } else {
            u64 = u64_val.value();
        }
    }

    esp_matter_val(char *chstr_buf, uint16_t chstr_len, uint16_t max_len = UINT16_MAX) {
        a.b = (uint8_t *)chstr_buf;
        a.s = chstr_len;
        a.max = max_len;
        a.t = chstr_len;
    }
    esp_matter_val(uint8_t *octet_buf, uint16_t octet_len, uint16_t max_len = UINT16_MAX) {
        a.b = (uint8_t *)octet_buf;
        a.s = octet_len;
        a.max = max_len;
        a.t = octet_len;
    }
};

typedef union esp_matter_val esp_matter_val_t;

/** ESP Matter Attribute Value */
struct esp_matter_attr_val {
    /** Type of Value */
    esp_matter_val_type_t type;
    /** Actual value. Depends on the type */
    esp_matter_val_t val;

    enum class uint_sub_type : uint8_t {
        k_none = 0,
        k_enum,
        k_bitmap,
    };

    /** Constructors */
    esp_matter_attr_val(esp_matter_val_type_t t, esp_matter_val_t v) : type(t), val(v) {}
    esp_matter_attr_val()
        : type(ESP_MATTER_VAL_TYPE_INVALID)
        , val() {}
    esp_matter_attr_val(void *p)
        : type(ESP_MATTER_VAL_TYPE_INVALID)
        , val(p) {}
    esp_matter_attr_val(bool b)
        : type(ESP_MATTER_VAL_TYPE_BOOLEAN)
        , val(b) {}
    esp_matter_attr_val(float f)
        : type(ESP_MATTER_VAL_TYPE_FLOAT)
        , val(f) {}
    esp_matter_attr_val(int8_t i8)
        : type(ESP_MATTER_VAL_TYPE_INT8)
        , val(i8) {}
    esp_matter_attr_val(int16_t i16)
        : type(ESP_MATTER_VAL_TYPE_INT16)
        , val(i16) {}
    esp_matter_attr_val(int32_t i32)
        : type(ESP_MATTER_VAL_TYPE_INT32)
        , val(i32) {}
    esp_matter_attr_val(int64_t i64)
        : type(ESP_MATTER_VAL_TYPE_INT64)
        , val(i64) {}
    esp_matter_attr_val(uint8_t u8, uint_sub_type sub_type = uint_sub_type::k_none)
        : val(u8)
    {
        if (sub_type == uint_sub_type::k_none) {
            type = ESP_MATTER_VAL_TYPE_UINT8;
        } else if (sub_type == uint_sub_type::k_enum) {
            type = ESP_MATTER_VAL_TYPE_ENUM8;
        } else {
            type = ESP_MATTER_VAL_TYPE_BITMAP8;
        }
    }
    esp_matter_attr_val(uint16_t u16, uint_sub_type sub_type = uint_sub_type::k_none)
        : val(u16)
    {
        if (sub_type == uint_sub_type::k_none) {
            type = ESP_MATTER_VAL_TYPE_UINT16;
        } else if (sub_type == uint_sub_type::k_enum) {
            type = ESP_MATTER_VAL_TYPE_ENUM16;
        } else {
            type = ESP_MATTER_VAL_TYPE_BITMAP16;
        }
    }
    esp_matter_attr_val(uint32_t u32, uint_sub_type sub_type = uint_sub_type::k_none)
        : val(u32)
    {
        VerifyOrDie(sub_type == uint_sub_type::k_none || sub_type == uint_sub_type::k_bitmap);
        if (sub_type == uint_sub_type::k_none) {
            type = ESP_MATTER_VAL_TYPE_UINT32;
        } else if (sub_type == uint_sub_type::k_bitmap) {
            type = ESP_MATTER_VAL_TYPE_BITMAP32;
        }
    }
    esp_matter_attr_val(uint64_t u64)
        : type(ESP_MATTER_VAL_TYPE_UINT64)
        , val(u64) {}

    esp_matter_attr_val(nullable<bool> b)
        : type(ESP_MATTER_VAL_TYPE_NULLABLE_BOOLEAN)
        , val(b) {}
    esp_matter_attr_val(nullable<float> f)
        : type(ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT)
        , val(f) {}
    esp_matter_attr_val(nullable<int8_t> i8)
        : type(ESP_MATTER_VAL_TYPE_NULLABLE_INT8)
        , val(i8) {}
    esp_matter_attr_val(nullable<int16_t> i16)
        : type(ESP_MATTER_VAL_TYPE_NULLABLE_INT16)
        , val(i16) {}
    esp_matter_attr_val(nullable<int32_t> i32)
        : type(ESP_MATTER_VAL_TYPE_NULLABLE_INT32)
        , val(i32) {}
    esp_matter_attr_val(nullable<int64_t> i64)
        : type(ESP_MATTER_VAL_TYPE_NULLABLE_INT64)
        , val(i64) {}
    esp_matter_attr_val(nullable<uint8_t> u8, uint_sub_type sub_type = uint_sub_type::k_none)
        : val(u8)
    {
        if (sub_type == uint_sub_type::k_none) {
            type = ESP_MATTER_VAL_TYPE_NULLABLE_UINT8;
        } else if (sub_type == uint_sub_type::k_enum) {
            type = ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8;
        } else {
            type = ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8;
        }
    }
    esp_matter_attr_val(nullable<uint16_t> u16, uint_sub_type sub_type = uint_sub_type::k_none)
        : val(u16)
    {
        if (sub_type == uint_sub_type::k_none) {
            type = ESP_MATTER_VAL_TYPE_NULLABLE_UINT16;
        } else if (sub_type == uint_sub_type::k_enum) {
            type = ESP_MATTER_VAL_TYPE_NULLABLE_ENUM16;
        } else {
            type = ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16;
        }
    }
    esp_matter_attr_val(nullable<uint32_t> u32, uint_sub_type sub_type = uint_sub_type::k_none)
        : val(u32)
    {
        if (sub_type == uint_sub_type::k_none) {
            type = ESP_MATTER_VAL_TYPE_NULLABLE_UINT32;
        } else if (sub_type == uint_sub_type::k_bitmap) {
            type = ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32;
        } else {
            type = ESP_MATTER_VAL_TYPE_INVALID;
        }
    }
    esp_matter_attr_val(nullable<uint64_t> u64)
        : type(ESP_MATTER_VAL_TYPE_NULLABLE_UINT64)
        , val(u64) {}

    esp_matter_attr_val(char *ch_str, uint16_t data_size, bool is_long_str = false)
        : type(is_long_str ? ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING : ESP_MATTER_VAL_TYPE_CHAR_STRING)
        , val(ch_str, data_size, is_long_str ? UINT16_MAX : UINT8_MAX) {}

    esp_matter_attr_val(uint8_t *octet_str, uint16_t data_size, bool is_long_str = false)
        : type(is_long_str ? ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING : ESP_MATTER_VAL_TYPE_OCTET_STRING)
        , val(octet_str, data_size, is_long_str ? UINT16_MAX : UINT8_MAX) {}

    esp_matter_attr_val([[ maybe_unused ]]uint8_t *array, [[ maybe_unused ]] uint16_t data_size, [[ maybe_unused ]] uint16_t count)
        : type(ESP_MATTER_VAL_TYPE_ARRAY)
        , val()
    {
        // we never store array type attribute value in our data model, so the input will be never used.
        (void)array;
        (void)data_size;
        (void)count;
    }

    bool is_nullable() const
    {
        return type & ESP_MATTER_VAL_NULLABLE_BASE;
    }

    bool is_null() const;

    /**
     * @brief Get the base type of the attribute value.
     *        The function will return the type with unset nullable bit.
     */
    esp_matter_val_type_t get_base_type() const
    {
        return (esp_matter_val_type_t)(type & (~ESP_MATTER_VAL_NULLABLE_BASE));
    }

    /**
     * @brief Get the storage type of the attribute value.
     */
    esp_matter_val_type_t get_storage_type() const
    {
        esp_matter_val_type_t base_type = get_base_type();
        if (base_type == ESP_MATTER_VAL_TYPE_ENUM8 || base_type == ESP_MATTER_VAL_TYPE_BITMAP8) {
            base_type = ESP_MATTER_VAL_TYPE_UINT8;
        } else if (base_type == ESP_MATTER_VAL_TYPE_ENUM16 || base_type == ESP_MATTER_VAL_TYPE_BITMAP16) {
            base_type = ESP_MATTER_VAL_TYPE_UINT16;
        } else if (base_type == ESP_MATTER_VAL_TYPE_BITMAP32) {
            base_type = ESP_MATTER_VAL_TYPE_UINT32;
        }
        return base_type;
    }
};

typedef struct esp_matter_attr_val esp_matter_attr_val_t;

/** ESP Matter Attribute Bounds */
typedef struct esp_matter_attr_bounds {
    /** Minimum Value */
    esp_matter_attr_val_t min;
    /** Maximum Value */
    esp_matter_attr_val_t max;
    /** TODO: Step Value might be needed here later */
} esp_matter_attr_bounds_t;

/*** Attribute val APIs ***/
/** Invalid */
inline esp_matter_attr_val_t esp_matter_invalid(void *val)
{
    return esp_matter_attr_val(val);
}
/** Boolean */
inline esp_matter_attr_val_t esp_matter_bool(bool val)
{
    return esp_matter_attr_val(val);
}
inline esp_matter_attr_val_t esp_matter_nullable_bool(nullable<bool> val)
{
    return esp_matter_attr_val(val);
}

/** Integer */
esp_matter_attr_val_t esp_matter_int(int val) __attribute__((deprecated));

esp_matter_attr_val_t esp_matter_nullable_int(nullable<int> val) __attribute__((deprecated));

/** Float */
inline esp_matter_attr_val_t esp_matter_float(float val)
{
    return esp_matter_attr_val(val);
}
inline esp_matter_attr_val_t esp_matter_nullable_float(nullable<float> val)
{
    return esp_matter_attr_val(val);
}

/** 8 bit integer */
inline esp_matter_attr_val_t esp_matter_int8(int8_t val)
{
    return esp_matter_attr_val(val);
}
inline esp_matter_attr_val_t esp_matter_nullable_int8(nullable<int8_t> val)
{
    return esp_matter_attr_val(val);
}

/** 8 bit unsigned integer */
inline esp_matter_attr_val_t esp_matter_uint8(uint8_t val)
{
    return esp_matter_attr_val(val);
}
inline esp_matter_attr_val_t esp_matter_nullable_uint8(nullable<uint8_t> val)
{
    return esp_matter_attr_val(val);
}

/** 16 bit signed integer */
inline esp_matter_attr_val_t esp_matter_int16(int16_t val)
{
    return esp_matter_attr_val(val);
}
inline esp_matter_attr_val_t esp_matter_nullable_int16(nullable<int16_t> val)
{
    return esp_matter_attr_val(val);
}

/** 16 bit unsigned integer */
inline esp_matter_attr_val_t esp_matter_uint16(uint16_t val)
{
    return esp_matter_attr_val(val);
}
inline esp_matter_attr_val_t esp_matter_nullable_uint16(nullable<uint16_t> val)
{
    return esp_matter_attr_val(val);
}

/** 32 bit signed integer */
inline esp_matter_attr_val_t esp_matter_int32(int32_t val)
{
    return esp_matter_attr_val(val);
}
inline esp_matter_attr_val_t esp_matter_nullable_int32(nullable<int32_t> val)
{
    return esp_matter_attr_val(val);
}

/** 32 bit unsigned integer */
inline esp_matter_attr_val_t esp_matter_uint32(uint32_t val)
{
    return esp_matter_attr_val(val);
}
inline esp_matter_attr_val_t esp_matter_nullable_uint32(nullable<uint32_t> val)
{
    return esp_matter_attr_val(val);
}

/** 64 bit signed integer */
inline esp_matter_attr_val_t esp_matter_int64(int64_t val)
{
    return esp_matter_attr_val(val);
}
inline esp_matter_attr_val_t esp_matter_nullable_int64(nullable<int64_t> val)
{
    return esp_matter_attr_val(val);
}

/** 64 bit unsigned integer */
inline esp_matter_attr_val_t esp_matter_uint64(uint64_t val)
{
    return esp_matter_attr_val(val);
}
inline esp_matter_attr_val_t esp_matter_nullable_uint64(nullable<uint64_t> val)
{
    return esp_matter_attr_val(val);
}

/** 8 bit enum */
inline esp_matter_attr_val_t esp_matter_enum8(uint8_t val)
{
    return esp_matter_attr_val(val, esp_matter_attr_val::uint_sub_type::k_enum);
}
inline esp_matter_attr_val_t esp_matter_nullable_enum8(nullable<uint8_t> val)
{
    return esp_matter_attr_val(val, esp_matter_attr_val::uint_sub_type::k_enum);
}

/** 16 bit enum */
inline esp_matter_attr_val_t esp_matter_enum16(uint16_t val)
{
    return esp_matter_attr_val(val, esp_matter_attr_val::uint_sub_type::k_enum);
}
inline esp_matter_attr_val_t esp_matter_nullable_enum16(nullable<uint16_t> val)
{
    return esp_matter_attr_val(val, esp_matter_attr_val::uint_sub_type::k_enum);
}

/** 8 bit bitmap */
inline esp_matter_attr_val_t esp_matter_bitmap8(uint8_t val)
{
    return esp_matter_attr_val(val, esp_matter_attr_val::uint_sub_type::k_bitmap);
}
inline esp_matter_attr_val_t esp_matter_nullable_bitmap8(nullable<uint8_t> val)
{
    return esp_matter_attr_val(val, esp_matter_attr_val::uint_sub_type::k_bitmap);
}

/** 16 bit bitmap */
inline esp_matter_attr_val_t esp_matter_bitmap16(uint16_t val)
{
    return esp_matter_attr_val(val, esp_matter_attr_val::uint_sub_type::k_bitmap);
}
inline esp_matter_attr_val_t esp_matter_nullable_bitmap16(nullable<uint16_t> val)
{
    return esp_matter_attr_val(val, esp_matter_attr_val::uint_sub_type::k_bitmap);
}

/** 32 bit bitmap */
inline esp_matter_attr_val_t esp_matter_bitmap32(uint32_t val)
{
    return esp_matter_attr_val(val, esp_matter_attr_val::uint_sub_type::k_bitmap);
}
inline esp_matter_attr_val_t esp_matter_nullable_bitmap32(nullable<uint32_t> val)
{
    return esp_matter_attr_val(val, esp_matter_attr_val::uint_sub_type::k_bitmap);
}

/** Character string */
inline esp_matter_attr_val_t esp_matter_char_str(char *val, uint16_t data_size)
{
    return esp_matter_attr_val(val, data_size);
}
inline esp_matter_attr_val_t esp_matter_long_char_str(char *val, uint16_t data_size)
{
    return esp_matter_attr_val(val, data_size, true);
}

/** Octet string */
inline esp_matter_attr_val_t esp_matter_octet_str(uint8_t *val, uint16_t data_size)
{
    return esp_matter_attr_val(val, data_size);
}
inline esp_matter_attr_val_t esp_matter_long_octet_str(uint8_t *val, uint16_t data_size)
{
    return esp_matter_attr_val(val, data_size, true);
}

/** Array */
inline esp_matter_attr_val_t esp_matter_array(uint8_t *val, uint16_t data_size, uint16_t count)
{
    return esp_matter_attr_val(val, data_size, count);
}

namespace esp_matter {

namespace attribute {

/** Attribute update callback type */
typedef enum callback_type {
    /** Callback before updating the value in the database */
    PRE_UPDATE,
    /** Callback after updating the value in the database */
    POST_UPDATE,
    /** Callback for reading the attribute value. This is used when the `ATTRIBUTE_FLAG_OVERRIDE` is set. */
    READ,
    /** Callback for writing the attribute value. This is used when the `ATTRIBUTE_FLAG_OVERRIDE` is set. */
    WRITE,
} callback_type_t;

/** Callback for attribute update
 *
 * @note If the callback type is `PRE_UPDATE` and an error is returned from the callback, the attribute will
 * not be updated.
 *
 * @param[in] type callback type.
 * @param[in] endpoint_id Endpoint ID of the attribute.
 * @param[in] cluster_id Cluster ID of the attribute.
 * @param[in] attribute_id Attribute ID of the attribute.
 * @param[in] val Pointer to `esp_matter_attr_val_t`. Use appropriate elements as per the value type.
 * @param[in] priv_data Pointer to the private data passed while creating the endpoint.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
typedef esp_err_t (*callback_t)(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                esp_matter_attr_val_t *val, void *priv_data);

/** Set attribute callback
 *
 * Set the common attribute update callback. Whenever an attribute managed by the application is updated, the callback
 * will be called with the appropriate `callback_type_t`.
 *
 * @param[in] callback attribute update callback.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_callback(callback_t callback);

/** Attribute update
 *
 * This API updates the attribute value.
 * After this API is called, the application gets the attribute update callback with `PRE_UPDATE`, then the
 * attribute is updated in the database, then the application get the callback with `POST_UPDATE`.
 *
 * @param[in] endpoint_id Endpoint ID of the attribute.
 * @param[in] cluster_id Cluster ID of the attribute.
 * @param[in] attribute_id Attribute ID of the attribute.
 * @param[in] val Pointer to `esp_matter_attr_val_t`. Appropriate elements should be used as per the value type.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val);

/** Attribute report
 *
 * This API reports the attribute value.
 * After this API is called, the application doesn't gets the attribute update callback with `PRE_UPDATE` or
 * `POST_UPDATE`, the attribute is updated in the database. This also marks the attribute as dirty, so that it can be
 * reported in the next subscription report.
 *
 * @param[in] endpoint_id Endpoint ID of the attribute.
 * @param[in] cluster_id Cluster ID of the attribute.
 * @param[in] attribute_id Attribute ID of the attribute.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t report(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val);

/** Attribute value print
 *
 * This API prints the attribute value according to the type.
 *
 * @param[in] endpoint_id Endpoint ID of the attribute.
 * @param[in] cluster_id Cluster ID of the attribute.
 * @param[in] attribute_id Attribute ID of the attribute.
 * @param[in] val Pointer to `esp_matter_attr_val_t`. Appropriate elements should be used as per the value type.
 * @param[in] is_read Boolean variable to indicate read or write for attributes.
 */
void val_print(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val,
               bool is_read);

/** Attribute value compare
 *
 * This API compares the two attribute values.
 *
 * @param[in] val1 Pointer to the first attribute value.
 * @param[in] val2 Pointer to the second attribute value.
 *
 * @return true if the two values are the same.
 * @return false if the two values are different.
 */
bool val_compare(const esp_matter_attr_val_t *val1, const esp_matter_attr_val_t *val2);

} // namespace attribute
} // namespace esp_matter
