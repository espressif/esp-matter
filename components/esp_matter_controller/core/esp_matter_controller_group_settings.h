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
namespace controller {
namespace group_settings {
/** Group management functions for the client only controller **/

/**
 * Print groups information
 */
esp_err_t show_groups();

/**
 * Add a group
 *
 * @param[in] group_name Group name
 * @param[in] group_id Group ID
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t add_group(char *group_name, uint16_t group_id);

/**
 * Leave a group
 *
 * @param[in] group_id Group ID
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t remove_group(uint16_t group_id);

/**
 * Print group keysets
 */
esp_err_t show_keysets();

/**
 * Bind a group keyset to a group
 *
 * @param[in] group_id Group ID
 * @param[in] keyset_id Group Keyset ID
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t bind_keyset(uint16_t group_id, uint16_t keyset_id);

/**
 * Unbind a group keyset to a group
 *
 * @param[in] group_id Group ID
 * @param[in] keyset_id Group Keyset ID
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t unbind_keyset(uint16_t group_id, uint16_t keyset_id);

/**
 * Add a group keyset
 *
 * @param[in] keyset_id Group Keyset ID
 * @param[in] key_policy Key policy
 * @param[in] validity_time Validity time for the keyset in second
 * @param[in] epoch_key_oct_str Octet string of the Epoch key
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t add_keyset(uint16_t keyset_id, uint8_t key_policy, uint64_t validity_time, char *epoch_key_oct_str);

/**
 * Remove a group keyset
 *
 * @param[in] keyset_id Group Keyset ID
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t remove_keyset(uint16_t keyset_id);

} // namespace group_settings
} // namespace controller
} // namespace esp_matter
