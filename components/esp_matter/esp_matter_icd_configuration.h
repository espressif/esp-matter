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

#include <esp_err.h>
#include <optional>

namespace esp_matter {
namespace icd {

typedef struct config {
    bool enable_icd_server;
    std::optional<uint32_t> fast_interval_ms;
    std::optional<uint32_t> slow_interval_ms;
    std::optional<uint32_t> active_mode_duration_ms;
    std::optional<uint32_t> idle_mode_duration_s;
    std::optional<uint32_t> active_threshold_ms;
} config_t;

/** Get whether ICD server is enabled for Matter end-device
 */
bool get_icd_server_enabled();

/** Set ICD configuration data
 * 
 * This function allows the user to enable or disable the ICD server and configure its settings at runtime, before creating the Matter data model.
 */
esp_err_t set_configuration_data(config_t *config);

} // namespace icd
} // namespace esp_matter
