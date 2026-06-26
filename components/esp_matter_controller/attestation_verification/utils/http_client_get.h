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

typedef struct {
    uint8_t *data;
    int size;
} http_resp_t;

typedef void (*on_http_data_t)(http_resp_t *resp, void *ctx);

esp_err_t http_send_get_request(const char *url, char **headers, size_t header_num, on_http_data_t on_http_data,
                                void *ctx);
