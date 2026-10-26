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
#include <lib/core/DataModelTypes.h>
#include <sdkconfig.h>

namespace esp_matter::controller::console {

typedef chip::FabricIndex(*fabric_index_getter_t)(void);

/**
 * @brief Set the getter for the application-selected fabric used by console commands.
 *
 * The getter runs with the CHIP stack locked and returns kUndefinedFabricIndex if no fabric is selected.
 * nullptr restores the client-only controller default; server builds require a getter.
 */
void set_fabric_index_getter(fabric_index_getter_t getter);

// Register each desired module once, before shell startup. Controller modules register their root on demand.
esp_err_t register_read_commands();
esp_err_t register_write_commands();
esp_err_t register_invoke_commands();
esp_err_t register_subscribe_commands();
esp_err_t register_window_commands();

#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
esp_err_t register_pairing_commands();
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE

#if !CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
esp_err_t register_icd_commands();
esp_err_t register_groups_commands();
#endif // !CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

} // namespace esp_matter::controller::console
