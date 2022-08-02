// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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

namespace esp_matter {
namespace identify {

/** Identify callback type */
typedef enum callback_type {
    /** Callback to start identify */
    START,
    /** Callback to stop identify */
    STOP,
    /** Callback to run a specific identify effect */
    EFFECT,
} callback_type_t;

/** Callback for identify
 *
 * @param[in] type callback type.
 * @param[in] endpoint_id Endpoint ID to identify.
 * @param[in] effect_id Effect ID to identify with.
 * @param[in] priv_data Pointer to the private data passed while setting the callback.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
typedef esp_err_t (*callback_t)(callback_type_t type, uint16_t endpoint_id, uint8_t effect_id, void *priv_data);

/** Set identify callback
 *
 * Set the common identify callback. Whenever an identify request is received by the device, the callback
 * will be called with the appropriate `callback_type_t`.
 *
 * @param[in] callback identify update callback.
 * @param[in] priv_data (Optional) Private data associated with the callback. This will be passed to callback. It
 * should stay allocated throughout the lifetime of the device.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_callback(callback_t callback, void *priv_data);

/** Initialise identify
 *
 * This initialises the identify object on the given endpoint. This happen internally whenever the identify cluster is
 * created on an endpoint. The type in this case is taken from the identify_type attribute.
 *
 * @param[in] endpoint_id Endpoint ID to identify.
 * @param[in] identify_type The type supported by the device.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t init(uint16_t endpoint_id, uint8_t identify_type);

} /* identify */
} /* esp_matter */
