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
#include <esp_http_client.h>

namespace esp_matter {
namespace ota_provider {

struct ota_image_header_prefix {
    uint32_t file_identifier;
    uint64_t total_size;
    uint32_t header_size;
} __attribute__((packed));

typedef struct ota_image_header_prefix ota_image_header_prefix_t;

constexpr uint32_t k_ota_image_file_identifier = 0x1BEEF11E;

int http_downloader_read(esp_http_client_handle_t http_client, char *buf, size_t size);

void http_downloader_abort(esp_http_client_handle_t http_client);

esp_err_t http_downloader_start(esp_http_client_config_t *config, esp_http_client_handle_t *http_client);

} // namespace ota_provider
} // namespace esp_matter
