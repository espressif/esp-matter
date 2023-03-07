// Copyright 2020 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

// Utilities for building std::byte arrays from strings or integer values.
#pragma once

#include <cstddef>

// Which log level to use for pw_kvs logs.
#ifndef PW_KVS_LOG_LEVEL
#define PW_KVS_LOG_LEVEL PW_LOG_LEVEL_INFO
#endif  // PW_KVS_LOG_LEVEL

// The maximum flash alignment supported.
#ifndef PW_KVS_MAX_FLASH_ALIGNMENT
#define PW_KVS_MAX_FLASH_ALIGNMENT 256UL
#endif  // PW_KVS_MAX_FLASH_ALIGNMENT

static_assert((PW_KVS_MAX_FLASH_ALIGNMENT >= 16UL),
              "Max flash alignment is required to be at least 16");

//  Whether to remove deleted keys in heavy maintanence. This feature costs some
//  code size (>1KB) and is only necessary if arbitrary key names are used.
//  Without this feature, deleted key entries can fill the KVS, making it
//  impossible to add more keys, even though most keys are deleted.
#ifndef PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE
#define PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE 1
#endif  // PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE

namespace pw::kvs {

inline constexpr size_t kMaxFlashAlignment = PW_KVS_MAX_FLASH_ALIGNMENT;

}  // namespace pw::kvs
