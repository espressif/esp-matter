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
namespace identification {

/** identification callback type */
typedef enum callback_type {
    /** Callback to start identification */
    START,
    /** Callback to stop identification */
    STOP,
    /** Callback to run a specific identification effect */
    EFFECT,
} callback_type_t;

/** Callback for identification
 *
 * @param[in] type callback type.
 * @param[in] endpoint_id Endpoint ID to identify.
 * @param[in] effect_id Effect ID to identify with.
 * @param[in] effect_variant Effect Variant
 * @param[in] priv_data Pointer to the private data passed while creating the endpoint.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
typedef esp_err_t (*callback_t)(callback_type_t type, uint16_t endpoint_id, uint8_t effect_id, uint8_t effect_variant,
                                    void *priv_data);

/** Set identification callback
 *
 * Set the common identification callback. Whenever an identify request is received by the device, the callback
 * will be called with the appropriate `callback_type_t`.
 *
 * @param[in] callback identification update callback.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_callback(callback_t callback);

/** Initialise identification
 *
 * This initialises the identify object on the given endpoint. This happen internally whenever the identify cluster is
 * created on an endpoint. The type in this case is taken from the identify_type attribute.
 *
 * @param[in] endpoint_id Endpoint ID to identify.
 * @param[in] identify_type The type supported by the device.
 * @param[in] effect_identifier Effect identifier, default is EMBER_ZCL_IDENTIFY_EFFECT_IDENTIFIER_BLINK
 * @param[in] effect_variant Effect variant, default is EMBER_ZCL_IDENTIFY_EFFECT_VARIANT_DEFAULT
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t init(uint16_t endpoint_id, uint8_t identify_type,
                uint8_t effect_identifier = static_cast<uint8_t>(chip::app::Clusters::Identify::EffectIdentifierEnum::kBlink),
                uint8_t effect_variant = static_cast<uint8_t>(chip::app::Clusters::Identify::EffectVariantEnum::kDefault));

} /* identification */
} /* esp_matter */
