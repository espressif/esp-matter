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

#include <app/clusters/ota-requestor/ExtendedOTARequestorDriver.h>
#include <app/clusters/ota-requestor/OTARequestorUserConsentDelegate.h>
#include <platform/ESP32/OTAImageProcessorImpl.h>

typedef struct {
    // ota requestor driver
    chip::DeviceLayer::ExtendedOTARequestorDriver *driver = nullptr;
    // user consent
    chip::ota::OTARequestorUserConsentDelegate *user_consent = nullptr;
    // ota image processor
    chip::OTAImageProcessorImpl *image_processor = nullptr;
} esp_matter_ota_requestor_impl_t;

typedef struct {
    /**
     * Timeout (in seconds) for querying default OTA Providers
     * Default querying interval is 86400 seconds (24 Hrs)
     */
    uint32_t periodic_query_timeout;
    /**
     * Timer timeout (in seconds) which detects the OTA being stuck in non-idle state
     * Something went wrong and OTA Requestor is stuck in a non-idle state for too long.
     * Default timeout is 300 seconds
     */
    uint32_t watchdog_timeout;
    /**
     * Options to override the default behavior of the OTA requestor
     * Refer to esp_matter_ota_requestor_impl_t for more details.
     * If not set, default implementation is used.
     */
    const esp_matter_ota_requestor_impl_t *impl = nullptr;
} esp_matter_ota_config_t;

/**
 * Initialize the Matter OTA Requestor
 *
 * Adds the OTA requestor server cluster and ota provider client cluster to root node endpoint.
 */
esp_err_t esp_matter_ota_requestor_init(void);

/**
 * Start the Matter OTA Requestor
 */
void esp_matter_ota_requestor_start(void);

/**
 * @brief This API initializes the handling of encrypted OTA image
 *
 * @param[in] key  null terminated RSA-3072 key in PEM format.
 *                 The key buffer must remain allocated and should not be freed.
 * @param[in] size Size of the key, size shall contain the null terminator as well.
 *                 If using `strlen` then please consider adding +1 to the size.
 *
 * @return ESP_OK on success, appropriate error otherwise
 */
#if CONFIG_ENABLE_ENCRYPTED_OTA
esp_err_t esp_matter_ota_requestor_encrypted_init(const char *key, uint16_t size);
#endif // CONFIG_ENABLE_ENCRYPTED_OTA

/**
 * @brief Set the OTA Requestor configuration parameters
 *
 * @param config OTA configuration structure of type esp_matter_ota_config_t
 *
 * @return ESP_OK on success, appropriate error code otherwise
 *
 * @note Ensure that this API is called only after esp_matter::start() has been invoked.
 */
esp_err_t esp_matter_ota_requestor_set_config(const esp_matter_ota_config_t & config);
