// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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

#include <cJSON.h>
#include <esp_err.h>
#include <lib/core/TLV.h>

namespace esp_matter {

enum class tlv_json_tag_format {
    /* Include the TLV type in each key, e.g. `"1:U8": 42` or `"2:OBJ": {...}`. */
    type_qualified,
    /* Render only the hexadecimal tag number, e.g. `"0x1": 42` or `"0x2": {...}`. */
    hexadecimal,
};

struct tlv_to_json_options {
    /* Render printable UTF-8 byte strings as text; other byte strings remain Base64-encoded. */
    bool human_readable_bytes = false;
    /* Select the format used for TLV tags in JSON object keys. */
    tlv_json_tag_format tag_format = tlv_json_tag_format::type_qualified;
};

/** Convert TLV data model payload to cJSON.
 *
 * @param[in]   reader The TLV reader positioned at the payload.
 * @param[out]  json   The JSON object output.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t tlv_to_json(chip::TLV::TLVReader &reader, cJSON **json);

/** Convert TLV data model payload to cJSON with conversion options.
 *
 * @param[in]   reader  The TLV reader positioned at the payload.
 * @param[out]  json    The JSON object output.
 * @param[in]   options Options controlling non-canonical display behavior.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t tlv_to_json(chip::TLV::TLVReader &reader, cJSON **json, const tlv_to_json_options &options);

} // namespace esp_matter
