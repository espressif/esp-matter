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

#define REMAP_TO_RANGE(value, from, to) ((value * to) / from)

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
    /** NULL terminated string */
    ESP_MATTER_VAL_TYPE_STRING,
    /** Array Eg. [1,2,3] */
    ESP_MATTER_VAL_TYPE_ARRAY,
    /** Byte String Eg. "123" */
    ESP_MATTER_VAL_TYPE_CHAR_STRING,
    /** Byte String Eg. [0x01, 0x20] */
    ESP_MATTER_VAL_TYPE_BYTE_STRING,
    /** Octet String Eg. [0x01, 0x20] */
    ESP_MATTER_VAL_TYPE_OCTET_STRING,
    /** 8 bit signed integer */
    ESP_MATTER_VAL_TYPE_INT8,
    /** 8 bit unsigned integer */
    ESP_MATTER_VAL_TYPE_UINT8,
    /** 16 bit unsigned integer */
    ESP_MATTER_VAL_TYPE_UINT16,
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
    /** NULL terminated JSON Object string Eg. "{\"name\":\"value\"}" */
    ESP_MATTER_VAL_TYPE_JSON_OBJECT,
    /** NULL terminated JSON Array string Eg. "[1,2,3]" */
    ESP_MATTER_VAL_TYPE_JSON_ARRAY,
} esp_matter_val_type_t;

/* ESP Matter Value */
typedef union {
    /** Boolean */
    bool b;
    /** Integer */
    int i;
    /** Float */
    float f;
    /** NULL terminated string. It should stay allocated throughout the lifetime of the device. */
    char *s;
    /** 8 bit signed integer */
    int8_t i8;
    /** 8 bit unsigned integer */
    uint8_t u8;
    /** 16 bit unsigned integer */
    uint16_t u16;
    /** 32 bit unsigned integer */
    uint32_t u32;
    /** 64 bit unsigned integer */
    uint64_t u64;
    /** Array */
    struct {
        /** Buffer */
        uint8_t *b;
        /** Total size */
        uint16_t s;
        /** Total count */
        uint16_t n;
    } a;
    /** Pointer */
    void *p;
} esp_matter_val_t;

/* ESP Matter Attribute Value */
typedef struct {
    /** Type of Value */
    esp_matter_val_type_t type;
    /** Actual value. Depends on the type */
    esp_matter_val_t val;
} esp_matter_attr_val_t;

/*** Attribute val APIs ***/
/** Invalid */
esp_matter_attr_val_t esp_matter_invalid(void *val);

/** Boolean */
esp_matter_attr_val_t esp_matter_bool(bool val);

/** Integer */
esp_matter_attr_val_t esp_matter_int(int val);

/** Float */
esp_matter_attr_val_t esp_matter_float(float val);

/** String */
esp_matter_attr_val_t esp_matter_str(const char *val);

/** JSON object string */
esp_matter_attr_val_t esp_matter_json_obj(const char *val);

/** JSON array string */
esp_matter_attr_val_t esp_matter_json_array(const char *val);

/** 8 bit integer */
esp_matter_attr_val_t esp_matter_int8(int8_t val);

/** 8 bit unsigned integer */
esp_matter_attr_val_t esp_matter_uint8(uint8_t val);

/** 16 bit unsigned integer */
esp_matter_attr_val_t esp_matter_uint16(uint16_t val);

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
esp_matter_attr_val_t esp_matter_char_str(char *val, uint16_t total_size);

/** Octet string */
esp_matter_attr_val_t esp_matter_octet_str(uint8_t *val, uint16_t total_size, uint16_t count);

/** Array */
esp_matter_attr_val_t esp_matter_array(uint8_t *val, uint16_t total_size, uint16_t count);

/** Attribute update
 *
 * This API updates the attribute value
 */
esp_err_t esp_matter_attribute_update(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t val);

/** Attribute value print
 *
 * This API prints the attribute value according to the type
 */
void esp_matter_attribute_val_print(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t val);
