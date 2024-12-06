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
#include <esp_matter_core.h>

namespace esp_matter {
namespace cluster {

namespace identify {
void add_bounds_cb(cluster_t *cluster);
} /* identify */

namespace scenes_management {
void add_bounds_cb(cluster_t *cluster);
} /* scenes_management */

namespace on_off {
void add_bounds_cb(cluster_t *cluster);
} /* on_off */

namespace level_control {
void add_bounds_cb(cluster_t *cluster);
} /* level_control */

namespace color_control {
void add_bounds_cb(cluster_t *cluster);
} /* color_control */

namespace fan_control {
void add_bounds_cb(cluster_t *cluster);
} /* fan_control */

namespace thermostat {
void add_bounds_cb(cluster_t *cluster);
} /* thermostat */

namespace thermostat_user_interface_configuration {
void add_bounds_cb(cluster_t *cluster);
} /* thermostat_user_interface_configuration */

} /* cluster */
} /* esp_matter */
