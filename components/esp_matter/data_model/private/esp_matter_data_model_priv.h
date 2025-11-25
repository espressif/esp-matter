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

#include <esp_err.h>
#include <esp_matter_attribute_utils.h>
#include <app/util/attribute-storage.h>
#include <app/ConcreteCommandPath.h>

#include <esp_matter_data_model.h>

namespace esp_matter {
namespace command {
void dispatch_single_cluster_command(const chip::app::ConcreteCommandPath &command_path, chip::TLV::TLVReader &tlv_data,
                                     void *opaque_ptr);
} // command

namespace node {

esp_err_t store_min_unused_endpoint_id();

esp_err_t read_min_unused_endpoint_id();

} // namespace node
namespace endpoint {

esp_err_t enable_all();

}

namespace attribute {

/** Get the attribute value from the esp-matter storage
 *
 * @param[in] attribute Attribute handle.
 * @param[out] val Pointer to `esp_matter_attr_val_t`. Use appropriate elements as per the value type.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t get_val_internal(attribute_t *attribute, esp_matter_attr_val_t *val);

/** Set the attribute value in the esp-matter storage
 *
 * @param[in] attribute Attribute handle.
 * @param[in] val Pointer to `esp_matter_attr_val_t`. Use appropriate elements as per the value type.
 * @param[in] call_callbacks Whether to call attribute change pre/post callbacks.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t set_val_internal(attribute_t *attribute, esp_matter_attr_val_t *val, bool call_callbacks = true);

} // namespace attribute

} // namespace esp_matter
