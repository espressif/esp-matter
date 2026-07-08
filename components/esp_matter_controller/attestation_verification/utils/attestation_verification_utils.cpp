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

#include <esp_check.h>
#include <esp_err.h>
#include <mbedtls/x509_crt.h>
#include <string.h>

#include <attestation_verification_utils.h>

constexpr char *TAG = "attestation_verification";

namespace esp_matter {
namespace controller {
namespace attestation_verification {

void remove_backslash_n(char *str)
{
    char *src = str, *dst = str;
    while (*src) {
        if (*src == '\\' && *(src + 1) == 'n') {
            src += 2;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

esp_err_t convert_pem_to_der(const char *pem, uint8_t *der_buf, size_t *der_len)
{
    mbedtls_x509_crt cert;
    mbedtls_x509_crt_init(&cert);
    mbedtls_x509_crt_parse(&cert, (const uint8_t *)pem, strlen(pem) + 1);
    memcpy(der_buf, cert.raw.p, cert.raw.len);
    *der_len = cert.raw.len;
    mbedtls_x509_crt_free(&cert);
    return ESP_OK;
}

} // namespace attestation_verification
} // namespace controller
} // namespace esp_matter
