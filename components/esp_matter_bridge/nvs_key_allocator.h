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

#include <lib/support/DefaultStorageKeyAllocator.h>

#define ESP_MATTER_BRIDGE_NAMESPACE "bridge"

using chip::StorageKeyName;

namespace esp_matter_bridge {

namespace nvs_key_allocator {

inline StorageKeyName endpoint_ids_array()
{
    return StorageKeyName::FromConst("b/epida");
}
inline StorageKeyName endpoint_pesistent_info(uint16_t endpoint_id)
{
    return StorageKeyName::Formatted("b/%x/pi", endpoint_id);
}

} // namespace nvs_key_allocator

} // namespace esp_matter_bridge
