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

#include "cJSON.h"
#include <esp_err.h>
#include <lib/core/TLV.h>
#include <string>

namespace esp_matter {

namespace element_type {
// Supported Data Type
const char k_int8[] = "I8";
const char k_int16[] = "I16";
const char k_int32[] = "I32";
const char k_int64[] = "I64";
const char k_uint8[] = "U8";
const char k_uint16[] = "U16";
const char k_uint32[] = "U32";
const char k_uint64[] = "U64";
const char k_bool[] = "BOOL";
const char k_float[] = "FP";
const char k_double[] = "DFP";
const char k_bytes[] = "BYT";
const char k_string[] = "STR";
const char k_null[] = "NULL";
const char k_object[] = "OBJ";
const char k_array[] = "ARR";
const char k_empty[] = "?";

const char k_floating_point_positive_infinity[] = "INF";
const char k_floating_point_negative_infinity[] = "-INF";
} // namespace element_type

/** Convert a JSON object to the given TLVWriter
 *
 * @param[in]   json_str The JSON string that represents a TLV structure
 * @param[out]  writer   The TLV output from the JSON object
 * @param[in]   tag      The TLV tag of the TLV structure
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t json_to_tlv(const char *json_str, chip::TLV::TLVWriter &writer, chip::TLV::Tag tag);

/** Convert a JSON object to the given TLVWriter
 *
 * @param[in]   json     The JSON object
 * @param[out]  writer   The TLV output from the JSON object
 * @param[in]   tag      The TLV tag of the TLV structure
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t json_to_tlv(cJSON *json, chip::TLV::TLVWriter &writer, chip::TLV::Tag tag);

} // namespace esp_matter
