// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
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

namespace esp_matter {
namespace attribute {

#define ESP_MATTER_KVS_NAMESPACE "esp_matter_kvs"

/**
 * @brief Gets the attribute value from the NVS, it generates the key based on endpoint, cluster, and attribute id.
 *
 * @param endpoint_id  Endpoint Id
 * @param cluster_id   Cluster Id
 * @param attribute_id Attribute Id
 *
 * @return ESP_OK on success, appropriate error code otherwise
 */
esp_err_t get_val_from_nvs(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t & val);

/**
 * @brief Stores the attribute value in NVS, it generates the key based on endpoint, cluster, and attribute id.
 *
 * @param endpoint_id  Endpoint Id
 * @param cluster_id   Cluster Id
 * @param attribute_id Attribute Id
 *
 * @return ESP_OK on success, appropriate error code otherwise
 */
esp_err_t store_val_in_nvs(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, const esp_matter_attr_val_t & val);

/**
 * @brief Erases the attribute value in NVS, it generates the key based on endpoint, cluster, and attribute id.
 *
 * @param endpoint_id  Endpoint Id
 * @param cluster_id   Cluster Id
 * @param attribute_id Attribute Id
 *
 * @return ESP_OK on success, appropriate error code otherwise
 */
esp_err_t erase_val_in_nvs(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id);

} // namespace attribute
} // namespace esp_matter
