// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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
#include <lib/core/NodeId.h>
#include <stddef.h>
#include <stdint.h>

constexpr size_t kMaxDevices = 32;

struct device_list_t {
    uint32_t version;
    uint32_t count;
    chip::NodeId nodes[kMaxDevices];
};

// Initialize after commissioner setup, while holding the CHIP stack lock.
// Registry load errors leave only device clear available.
esp_err_t devices_init();
