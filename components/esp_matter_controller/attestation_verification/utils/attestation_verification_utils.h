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

namespace esp_matter {
namespace controller {
namespace attestation_verification {

void remove_backslash_n(char *str);

/** Convert Certificate in PEM format to DER format
 *
 * param[in] pem certificate in PEM format
 * param[out] der_buf output certificate in DER format
 * param[out] der_len length of DER certificate
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t convert_pem_to_der(const char *pem, uint8_t *der_buf, size_t *der_len);

} // namespace attestation_verification
} // namespace controller
} // namespace esp_matter
