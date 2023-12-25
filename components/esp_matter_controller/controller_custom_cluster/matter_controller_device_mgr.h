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
#include <matter_device.h>

namespace esp_matter {
namespace controller {
namespace device_mgr {

using endpoint_entry_t = endpoint_entry_t;
using matter_device_t = matter_device_t;

typedef void (*device_list_update_callback_t)(void);

inline void free_device_list(matter_device_t *dev_list)
{
    free_matter_device_list(dev_list);
}

inline void print_device_list(matter_device_t *dev_list)
{
    print_matter_device_list(dev_list);
}

matter_device_t *get_device_list_clone();

matter_device_t *get_device_clone(uint64_t node_id);

matter_device_t *get_device_clone(char *rainmaker_node_id);

esp_err_t update_device_list(uint16_t endpoint_id);

esp_err_t init(uint16_t endpoint_id, device_list_update_callback_t dev_list_update_cb);
} // namespace device_mgr
} // namespace controller
} // namespace esp_matter
