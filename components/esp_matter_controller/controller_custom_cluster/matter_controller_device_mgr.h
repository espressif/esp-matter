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

namespace esp_matter {
namespace controller {
namespace device_mgr {

#define ESP_MATTER_DEVICE_MAX_ENDPOINT 8

typedef struct endpoint_entry {
    uint16_t endpoint_id;
    uint32_t device_type_id;
} endpoint_entry_t;

typedef struct matter_device {
    uint64_t node_id;
    char rainmaker_node_id[24];
    uint8_t endpoint_count;
    endpoint_entry_t endpoints[ESP_MATTER_DEVICE_MAX_ENDPOINT];
    bool reachable;
    bool is_rainmaker_device;
    struct matter_device *next;
} matter_device_t;

typedef void (*device_list_update_callback_t)(void);

void free_device_list(matter_device_t *dev_list);

void print_device_list(matter_device_t *dev_list);

matter_device_t *get_device_list_clone();

matter_device_t *get_device_clone(uint64_t node_id);

matter_device_t *get_device_clone(char *rainmaker_node_id);

esp_err_t update_device_list(uint16_t endpoint_id);

esp_err_t init(uint16_t endpoint_id, device_list_update_callback_t dev_list_update_cb);
} // namespace device_mgr
} // namespace controller
} // namespace esp_matter
