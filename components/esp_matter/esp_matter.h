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

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_err.h>
#include <stdbool.h>
#include <stdint.h>

#define REMAP_TO_RANGE(value, from, to) value *to / from

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
    /** NULL terminated JSON Object string Eg. {"name":"value"} */
    ESP_MATTER_VAL_TYPE_OBJECT,
    /** NULL terminated JSON Array string Eg. [1,2,3] */
    ESP_MATTER_VAL_TYPE_ARRAY,
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
} esp_matter_val_t;

/* ESP Matter Attribute Value */
typedef struct {
    /** Type of Value */
    esp_matter_val_type_t type;
    /** Actual value. Depends on the type */
    esp_matter_val_t val;
} esp_matter_attr_val_t;

/** Callback for attribute value changed
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
typedef esp_err_t (*esp_matter_attribute_callback_t)(const char *endpoint, const char *attribute,
                                                     esp_matter_attr_val_t val, void *priv_data);

/**
 * Initialise a Boolean value
 *
 * @param[in] bval Initialising value.
 *
 * @return Value structure.
 */
esp_matter_attr_val_t esp_matter_bool(bool bval);

/**
 * Initialise an Integer value
 *
 * @param[in] ival Initialising value.
 *
 * @return Value structure.
 */
esp_matter_attr_val_t esp_matter_int(int ival);

/**
 * Initialise a Float value
 *
 * @param[in] fval Initialising value.
 *
 * @return Value structure.
 */
esp_matter_attr_val_t esp_matter_float(float fval);

/**
 * Initialise a String value
 *
 * @param[in] sval Initialising value.
 *
 * @return Value structure.
 */
esp_matter_attr_val_t esp_matter_str(const char *sval);

/**
 * Initialise a json object value
 *
 * @note the object will not be validated internally. it is the application's
 * responsibility to ensure that the object is a valid json object.
 * eg. esp_matter_obj("{\"name\":\"value\"}");
 *
 * @param[in] val Initialising value
 *
 * @return Value structure
 */
esp_matter_attr_val_t esp_matter_obj(const char *val);

/**
 * Initialise a json array value
 *
 * @note the array will not be validated internally. it is the application's
 * responsibility to ensure that the array is a valid json array.
 * eg. esp_matter_array("[1,2,3]");
 *
 * @param[in] val Initialising value
 *
 * @return Value structure
 */
esp_matter_attr_val_t esp_matter_array(const char *val);

/** Add callback
 *
 * Add a new callback. The callback will then be called if the value of any attributes have been changed by some other
 * callback.
 *
 * @param[in] name Callback name
 * @param[in] callback Callback for value change
 * @param[in] priv_data (Optional) Private data associated with the callback. This will be passed to the callback.
 * It should stay allocated throughout the lifetime of the device.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_matter_attribute_callback_add(const char *name, esp_matter_attribute_callback_t callback,
                                            void *priv_data);

/** Remove callback
 *
 * Remove a previously added callback
 *
 * @param[in] name Callback name
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_matter_attribute_callback_remove(const char *name);

/** Notify other callbacks
 *
 * Notify other callbacks about the change in the value of any attribute for the endpoint
 *
 * @param[in] name Callback name
 * @param[in] endpoint Endpoint name
 * @param[in] attribute Attribute name
 * @param[in] val Attribute value
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_matter_attribute_notify(const char *name, const char *endpoint, const char *attribute,
                                      esp_matter_attr_val_t val);

/** Initialize ESP Matter
 *
 * This initializes the handling of different callbacks
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_matter_init();

#ifdef __cplusplus
}
#endif
