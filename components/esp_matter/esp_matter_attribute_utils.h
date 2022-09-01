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

#include <esp_err.h>
#include <stdbool.h>
#include <stdint.h>

/** Remap attribute values
 *
 * This can be used to remap attribute values to different ranges.
 * Example: To convert the brightness value (0-255) into brightness percentage (0-100) and vice-versa.
 */
#define REMAP_TO_RANGE(value, from, to) ((value * to) / from)

/** Remap attribute values with inverse dependency
 *
 * This can be used to remap attribute values with inverse dependency to different ranges.
 * Example: To convert the temperature mireds into temperature kelvin and vice-versa where the relation between them
 * is: Mireds = 1,000,000/Kelvin.
 */
#define REMAP_TO_RANGE_INVERSE(value, factor) (factor / value)

/** ESP Matter Attribute Value type */
typedef enum {
    /** Invalid */
    ESP_MATTER_VAL_TYPE_INVALID = 0,
    /** Boolean */
    ESP_MATTER_VAL_TYPE_BOOLEAN,
    /** Integer. Mapped to a 32 bit signed integer */
    ESP_MATTER_VAL_TYPE_INTEGER,
    /** Floating point number */
    ESP_MATTER_VAL_TYPE_FLOAT,
    /** Array Eg. [1,2,3] */
    ESP_MATTER_VAL_TYPE_ARRAY,
    /** Char String Eg. "123" */
    ESP_MATTER_VAL_TYPE_CHAR_STRING,
    /** Octet String Eg. [0x01, 0x20] */
    ESP_MATTER_VAL_TYPE_OCTET_STRING,
    /** 8 bit signed integer */
    ESP_MATTER_VAL_TYPE_INT8,
    /** 8 bit unsigned integer */
    ESP_MATTER_VAL_TYPE_UINT8,
    /** 16 bit signed integer */
    ESP_MATTER_VAL_TYPE_INT16,
    /** 16 bit unsigned integer */
    ESP_MATTER_VAL_TYPE_UINT16,
    /** 32 bit signed integer */
    ESP_MATTER_VAL_TYPE_INT32,
    /** 32 bit unsigned integer */
    ESP_MATTER_VAL_TYPE_UINT32,
    /** 64 bit unsigned integer */
    ESP_MATTER_VAL_TYPE_UINT64,
    /** 8 bit enum */
    ESP_MATTER_VAL_TYPE_ENUM8,
    /** 8 bit bitmap */
    ESP_MATTER_VAL_TYPE_BITMAP8,
    /** 16 bit bitmap */
    ESP_MATTER_VAL_TYPE_BITMAP16,
    /** 32 bit bitmap */
    ESP_MATTER_VAL_TYPE_BITMAP32,
} esp_matter_val_type_t;

/** ESP Matter Value */
typedef union {
    /** Boolean */
    bool b;
    /** Integer */
    int i;
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
    int16_t i32;
    /** 32 bit unsigned integer */
    uint32_t u32;
    /** 64 bit unsigned integer */
    uint64_t u64;
    /** Array */
    struct {
        /** Buffer */
        uint8_t *b;
        /** Data size */
        uint16_t s;
        /** Data count */
        uint16_t n;
        /** Total size */
        uint16_t t;
    } a;
    /** Pointer */
    void *p;
} esp_matter_val_t;

/** ESP Matter Attribute Value */
typedef struct {
    /** Type of Value */
    esp_matter_val_type_t type;
    /** Actual value. Depends on the type */
    esp_matter_val_t val;
} esp_matter_attr_val_t;

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
esp_matter_attr_val_t esp_matter_invalid(void *val);

/** Boolean */
esp_matter_attr_val_t esp_matter_bool(bool val);

/** Integer */
esp_matter_attr_val_t esp_matter_int(int val);

/** Float */
esp_matter_attr_val_t esp_matter_float(float val);

/** 8 bit integer */
esp_matter_attr_val_t esp_matter_int8(int8_t val);

/** 8 bit unsigned integer */
esp_matter_attr_val_t esp_matter_uint8(uint8_t val);

/** 16 bit signed integer */
esp_matter_attr_val_t esp_matter_int16(int16_t val);

/** 16 bit unsigned integer */
esp_matter_attr_val_t esp_matter_uint16(uint16_t val);

/** 32 bit signed integer */
esp_matter_attr_val_t esp_matter_int32(int16_t val);

/** 32 bit unsigned integer */
esp_matter_attr_val_t esp_matter_uint32(uint32_t val);

/** 64 bit unsigned integer */
esp_matter_attr_val_t esp_matter_uint64(uint64_t val);

/** 8 bit enum */
esp_matter_attr_val_t esp_matter_enum8(uint8_t val);

/** 8 bit bitmap */
esp_matter_attr_val_t esp_matter_bitmap8(uint8_t val);

/** 16 bit bitmap */
esp_matter_attr_val_t esp_matter_bitmap16(uint16_t val);

/** 32 bit bitmap */
esp_matter_attr_val_t esp_matter_bitmap32(uint32_t val);

/** Character string */
esp_matter_attr_val_t esp_matter_char_str(char *val, uint16_t data_size);

/** Octet string */
esp_matter_attr_val_t esp_matter_octet_str(uint8_t *val, uint16_t data_size);

/** Array */
esp_matter_attr_val_t esp_matter_array(uint8_t *val, uint16_t data_size, uint16_t count);

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

/** Attribute value print
 *
 * This API prints the attribute value according to the type.
 *
 * @param[in] endpoint_id Endpoint ID of the attribute.
 * @param[in] cluster_id Cluster ID of the attribute.
 * @param[in] attribute_id Attribute ID of the attribute.
 * @param[in] val Pointer to `esp_matter_attr_val_t`. Appropriate elements should be used as per the value type.
 */
void val_print(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val);

} /* attribute */
} /* esp_matter */
