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
#include <esp_matter_ota_provider.h>

namespace esp_matter {
namespace ota_provider {

typedef struct {
    uint16_t vendor_id;
    uint16_t product_id;
    uint32_t software_version;
    char software_version_str[SOFTWARE_VERSION_STR_MAX_LEN];
    uint16_t cd_version_number;
    uint32_t min_applicable_software_version;
    uint32_t max_applicable_software_version;
    char ota_url[OTA_URL_MAX_LEN];
    uint32_t ota_file_size;
    uint32_t lifetime;
} model_version_t;

typedef void (*fetch_ota_image_done_callback_t)(EspOtaProvider::OTAQueryStatus status, const char *imageUrl,
                                                size_t imageSize, uint32_t softwareVersion,
                                                const char *softwareVersionStr, void *ctx);

esp_err_t fetch_ota_candidate(const uint16_t vendor_id, const uint16_t product_id, const uint32_t software_version,
                              fetch_ota_image_done_callback_t callback, void *callback_args);

esp_err_t init_ota_candidates();

} // namespace ota_provider
} // namespace esp_matter
