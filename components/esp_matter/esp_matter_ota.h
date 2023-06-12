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

#include "esp_err.h"
#include "sdkconfig.h"

/** Initialize the Matter OTA Requestor
 *
 */
esp_err_t esp_matter_ota_requestor_init(void);

/**Start the Matter OTA Requestor
 * 
 */
void esp_matter_ota_requestor_start(void);

/**
 * @brief This API initializes the handling of encrypted OTA image
 *
 * @param[in] key  null terminated RSA-3072 key in PEM format
 * @param[in] size Size of the key
 *
 * @return ESP_OK or success, appropriate error otherwise
 */
#if CONFIG_ENABLE_ENCRYPTED_OTA
esp_err_t esp_matter_ota_requestor_encrypted_init(const char *key, uint16_t size);
#endif // CONFIG_ENABLE_ENCRYPTED_OTA
