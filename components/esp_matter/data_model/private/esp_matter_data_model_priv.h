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

namespace esp_matter {
namespace node {

esp_err_t store_min_unused_endpoint_id();

esp_err_t read_min_unused_endpoint_id();

} // namespace node
namespace endpoint {

esp_err_t enable_all();

}

namespace attribute {
esp_err_t get_data_from_attr_val(esp_matter_attr_val_t *val, EmberAfAttributeType *attribute_type,
                                 uint16_t *attribute_size, uint8_t *value);

esp_err_t get_attr_val_from_data(esp_matter_attr_val_t *val, EmberAfAttributeType attribute_type,
                                 uint16_t attribute_size, uint8_t *value,
                                 const EmberAfAttributeMetadata * attribute_metadata);
}
} // namespace esp_matter
