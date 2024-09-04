// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
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
#include <esp_matter_console.h>

namespace esp_matter {
namespace console {

// Should be called after esp_matter::start.
// Should not coexist with application operations, such as adding matter nodes when ZigBee devices are connected.
esp_err_t bridge_register_commands();

} // namespace console
} // namespace esp_matter
