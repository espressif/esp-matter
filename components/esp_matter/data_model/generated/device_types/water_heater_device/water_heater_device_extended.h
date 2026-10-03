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
#include <esp_matter_data_model.h>
#include <water_heater_device.h>
#include <water_heater_management.h>

namespace esp_matter {
namespace endpoint {
namespace water_heater {

typedef struct extended_config {
    config_t base;
    cluster::water_heater_management::feature::energy_management::config_t *energy_management;
    cluster::water_heater_management::feature::tank_percent::config_t *tank_percent;

    extended_config() : base(), energy_management(nullptr), tank_percent(nullptr) {}
} extended_config_t;

endpoint_t *create_extended(node_t *node, extended_config_t *config, uint8_t flags, void *priv_data);
} /* water_heater */
} /* endpoint */
} /* esp_matter */
