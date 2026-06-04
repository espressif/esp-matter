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

#include "sdkconfig.h"
#ifdef CONFIG_ESP_MATTER_ENABLE_OPTIONAL_ATTRIBUTES

#include <esp_matter_attribute.h>

namespace esp_matter {
namespace cluster {

namespace basic_information {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* basic_information */

namespace boolean_state_configuration {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* boolean_state_configuration */

namespace electrical_energy_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* electrical_energy_measurement */

namespace electrical_power_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* electrical_power_measurement */

namespace ethernet_network_diagnostics {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* ethernet_network_diagnostics */

namespace general_diagnostics {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* general_diagnostics */

namespace occupancy_sensing {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* occupancy_sensing */

namespace resource_monitoring {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* resource_monitoring */

namespace software_diagnostics {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* software_diagnostics */

namespace time_synchronization {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* time_synchronization */

namespace wifi_network_diagnostics {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* wifi_network_diagnostics */

namespace temperature_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* temperature_measurement */

namespace flow_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* flow_measurement */

namespace pressure_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* pressure_measurement */

namespace relative_humidity_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster);
} /* relative_humidity_measurement */

} /* cluster */
} /* esp_matter */

#endif /* CONFIG_ESP_MATTER_ENABLE_OPTIONAL_ATTRIBUTES */
