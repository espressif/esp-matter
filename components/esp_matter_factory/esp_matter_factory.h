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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize and open the matter factory partition.
 *
 * @return ESP_OK on success or if already initialized,
 *         appropriate error code otherwise.
 */
esp_err_t esp_matter_factory_init(void);

/**
 * @brief Deinitialize and close the matter factory partition.
 */
void esp_matter_factory_deinit(void);

/**
 * @brief Get the device attestation certificate (DAC) from matter factory partition.
 *
 * @param[out]     out_buf Pointer to the buffer to store the certificate.
 * @param[in, out] buf_len The length of the buffer, and the length of the
 *                         certificate after the function returns.
 *
 * @return ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_matter_factory_get_dac_cert(uint8_t *out_buf, size_t *len);

/**
 * @brief Get the Device Attestation Public Key from matter factory partition.
 *
 * @param[out]     out_buf Pointer to the buffer to store the public key.
 * @param[in, out] buf_len The length of the buffer, and the length of the
 *                        public key after the function returns.
 *
 * @return ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_matter_factory_get_dac_public_key(uint8_t *out_buf, size_t *len);

/**
 * @brief Get the Device Attestation Private Key from matter factory partition.
 *
 * @param[out]     out_buf Pointer to the buffer to store the private key.
 * @param[in, out] buf_len The length of the buffer, and the length of the
 *
 * @return ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_matter_factory_get_dac_private_key(uint8_t *out_buf, size_t *len);

/**
 * @brief Get the Product Attestation Intermediate Certificate (PAI) from matter factory partition.
 *
 * @param[out]     out_buf Pointer to the buffer to store the certificate.
 * @param[in, out] buf_len The length of the buffer, and the length of the
 *                         certificate after the function returns.
 *
 * @return ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_matter_factory_get_pai_cert(uint8_t *out_buf, size_t *len);

/**
 * @brief Get the Certificate Declaration from matter factory partition.
 *
 * @param[out]     out_buf Pointer to the buffer to store the certificate.
 * @param[in, out] buf_len The length of the buffer, and the length of the
 *                         certificate after the function returns.
 *
 * @return ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_matter_factory_get_cert_declrn(uint8_t *out_buf, size_t *len);

#ifdef __cplusplus
}
#endif
