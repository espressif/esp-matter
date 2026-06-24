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

// This file provides helper APIs to create optional attributes for various Matter clusters.
// It is intended for testing purposes only and is compiled only when CONFIG_ESP_MATTER_ENABLE_OPTIONAL_ATTRIBUTES is enabled.

#include "esp_matter_optional_attribute.h"
#include <esp_log.h>
#include <esp_check.h>

#ifdef CONFIG_ESP_MATTER_ENABLE_OPTIONAL_ATTRIBUTES
static const char *TAG = "optional_attr";

namespace esp_matter {
namespace cluster {

namespace basic_information {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(basic_information::attribute::create_manufacturing_date(cluster, NULL, 0), ESP_ERR_NO_MEM, TAG, "Failed to create manufacturing_date");
    ESP_RETURN_ON_FALSE(basic_information::attribute::create_part_number(cluster, NULL, 0), ESP_ERR_NO_MEM, TAG, "Failed to create part_number");
    ESP_RETURN_ON_FALSE(basic_information::attribute::create_product_url(cluster, NULL, 0), ESP_ERR_NO_MEM, TAG, "Failed to create product_url");
    ESP_RETURN_ON_FALSE(basic_information::attribute::create_product_label(cluster, NULL, 0), ESP_ERR_NO_MEM, TAG, "Failed to create product_label");
    ESP_RETURN_ON_FALSE(basic_information::attribute::create_serial_number(cluster, NULL, 0), ESP_ERR_NO_MEM, TAG, "Failed to create serial_number");
    ESP_RETURN_ON_FALSE(basic_information::attribute::create_local_config_disabled(cluster, false), ESP_ERR_NO_MEM, TAG, "Failed to create local_config_disabled");
    ESP_RETURN_ON_FALSE(basic_information::attribute::create_reachable(cluster, true), ESP_ERR_NO_MEM, TAG, "Failed to create reachable");
    ESP_RETURN_ON_FALSE(basic_information::attribute::create_product_appearance(cluster, NULL, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create product_appearance");

    return ESP_OK;
}
} /* basic_information */

namespace boolean_state_configuration {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(boolean_state_configuration::attribute::create_default_sensitivity_level(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create default_sensitivity_level");
    ESP_RETURN_ON_FALSE(boolean_state_configuration::attribute::create_alarms_enabled(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create alarms_enabled");
    ESP_RETURN_ON_FALSE(boolean_state_configuration::attribute::create_sensor_fault(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create sensor_fault");

    return ESP_OK;
}
} /* boolean_state_configuration */

namespace electrical_energy_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(electrical_energy_measurement::attribute::create_cumulative_energy_reset(cluster, NULL, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create cumulative_energy_reset");

    return ESP_OK;
}
} /* electrical_energy_measurement */

namespace electrical_power_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_ranges(cluster, NULL, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create ranges");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_voltage(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create voltage");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_active_current(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create active_current");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_reactive_current(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create reactive_current");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_apparent_current(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create apparent_current");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_reactive_power(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create reactive_power");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_apparent_power(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create apparent_power");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_rms_voltage(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create rms_voltage");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_rms_current(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create rms_current");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_rms_power(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create rms_power");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_frequency(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create frequency");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_power_factor(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create power_factor");
    ESP_RETURN_ON_FALSE(electrical_power_measurement::attribute::create_neutral_current(cluster, nullable<int64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create neutral_current");

    return ESP_OK;
}
} /* electrical_power_measurement */

namespace ethernet_network_diagnostics {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(ethernet_network_diagnostics::attribute::create_phy_rate(cluster, nullable<uint8_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create phy_rate");
    ESP_RETURN_ON_FALSE(ethernet_network_diagnostics::attribute::create_full_duplex(cluster, nullable<bool>()), ESP_ERR_NO_MEM, TAG, "Failed to create full_duplex");
    ESP_RETURN_ON_FALSE(ethernet_network_diagnostics::attribute::create_carrier_detect(cluster, nullable<bool>()), ESP_ERR_NO_MEM, TAG, "Failed to create carrier_detect");
    ESP_RETURN_ON_FALSE(ethernet_network_diagnostics::attribute::create_time_since_reset(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create time_since_reset");

    return ESP_OK;
}
} /* ethernet_network_diagnostics */

namespace general_diagnostics {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(general_diagnostics::attribute::create_total_operational_hours(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create total_operational_hours");
    ESP_RETURN_ON_FALSE(general_diagnostics::attribute::create_boot_reason(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create boot_reason");
    ESP_RETURN_ON_FALSE(general_diagnostics::attribute::create_active_hardware_faults(cluster, NULL, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create active_hardware_faults");
    ESP_RETURN_ON_FALSE(general_diagnostics::attribute::create_active_radio_faults(cluster, NULL, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create active_radio_faults");
    ESP_RETURN_ON_FALSE(general_diagnostics::attribute::create_active_network_faults(cluster, NULL, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create active_network_faults");

    return ESP_OK;
}
} /* general_diagnostics */

namespace occupancy_sensing {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(occupancy_sensing::attribute::create_hold_time(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create hold_time");
    ESP_RETURN_ON_FALSE(occupancy_sensing::attribute::create_hold_time_limits(cluster, NULL, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create hold_time_limits");

    return ESP_OK;
}
} /* occupancy_sensing */

namespace resource_monitoring {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(resource_monitoring::attribute::create_in_place_indicator(cluster, false), ESP_ERR_NO_MEM, TAG, "Failed to create in_place_indicator");
    ESP_RETURN_ON_FALSE(resource_monitoring::attribute::create_last_changed_time(cluster, nullable<uint8_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create last_changed_time");

    return ESP_OK;
}
} /* resource_monitoring */

namespace software_diagnostics {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(software_diagnostics::attribute::create_thread_metrics(cluster, NULL, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create thread_metrics");
    ESP_RETURN_ON_FALSE(software_diagnostics::attribute::create_current_heap_free(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create current_heap_free");
    ESP_RETURN_ON_FALSE(software_diagnostics::attribute::create_current_heap_used(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create current_heap_used");

    return ESP_OK;
}
} /* software_diagnostics */

namespace time_synchronization {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(time_synchronization::attribute::create_time_source(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create time_source");
    ESP_RETURN_ON_FALSE(time_synchronization::attribute::create_trusted_time_source(cluster, NULL, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create trusted_time_source");

    return ESP_OK;
}
} /* time_synchronization */

namespace wifi_network_diagnostics {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(wifi_network_diagnostics::attribute::create_current_max_rate(cluster, nullable<uint64_t>()), ESP_ERR_NO_MEM, TAG, "Failed to create current_max_rate");

    return ESP_OK;
}
} /* wifi_network_diagnostics */

namespace temperature_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(temperature_measurement::attribute::create_tolerance(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create tolerance");

    return ESP_OK;
}
} /* temperature_measurement */

namespace flow_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(flow_measurement::attribute::create_tolerance(cluster, 0, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create tolerance");

    return ESP_OK;
}
} /* flow_measurement */

namespace pressure_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(pressure_measurement::attribute::create_tolerance(cluster, 0, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create tolerance");
    ESP_RETURN_ON_FALSE(pressure_measurement::attribute::create_scaled_tolerance(cluster, 0, 0, 0), ESP_ERR_NO_MEM, TAG, "Failed to create scaled_tolerance");

    return ESP_OK;
}
} /* pressure_measurement */

namespace relative_humidity_measurement {
esp_err_t create_optional_attributes(cluster_t *cluster)
{
    ESP_RETURN_ON_FALSE(cluster, ESP_ERR_INVALID_ARG, TAG, "Cluster cannot be NULL");

    ESP_RETURN_ON_FALSE(relative_humidity_measurement::attribute::create_tolerance(cluster, 0), ESP_ERR_NO_MEM, TAG, "Failed to create tolerance");

    return ESP_OK;
}
} /* relative_humidity_measurement */

} /* cluster */
} /* esp_matter */
#endif /* CONFIG_ESP_MATTER_ENABLE_OPTIONAL_ATTRIBUTES */