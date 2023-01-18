// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_log.h>
#include <esp_matter_event.h>

#include <app/clusters/switch-server/switch-server.h>
#include <platform/DeviceControlServer.h>


using chip::DeviceLayer::DeviceControlServer;
using chip::EndpointId;
using namespace chip::app::Clusters;

namespace esp_matter {
namespace cluster {

namespace door_lock {
namespace event {

esp_err_t send_door_lock_alarm()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_door_state_change()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_lock_operation()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_lock_operation_error()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_lock_user_change()
{
    /* Not implemented */
    return ESP_OK;
}

} /* event */
} /* door_lock */

namespace pump_configuration_and_control {
namespace event {

esp_err_t send_supply_voltage_low()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_supply_voltage_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_power_missing_phase()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_system_pressure_low()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_system_pressure_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_dry_running()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_motor_temperature_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_pump_motor_fatal_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_electronic_temperature_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_pump_blocked()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_sensor_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_electronic_non_fatal_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_electronic_fatal_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_general_fault()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_leakage()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_air_detection()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_turbine_operation()
{
    /* Not implemented */
    return ESP_OK;
}

} /* event */
} /* pump_configuration_and_control */

namespace actions {
namespace event {
esp_err_t send_state_changed(EndpointId endpoint, uint16_t action_id, uint32_t invoke_id, uint8_t action_state)
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_action_failed(EndpointId endpoint, uint16_t action_id, uint32_t invoke_id, uint8_t action_state,
                             uint8_t error)
{
    /* Not implemented */
    return ESP_OK;
}
} /* event */
} /* actions */

namespace switch_cluster {
namespace event {

esp_err_t send_switch_latched(EndpointId endpoint, uint8_t new_position)
{
    SwitchServer::Instance().OnSwitchLatch(endpoint, new_position);
    return ESP_OK;
}

esp_err_t send_initial_press(EndpointId endpoint, uint8_t new_position)
{
    SwitchServer::Instance().OnInitialPress(endpoint, new_position);
    return ESP_OK;
}

esp_err_t send_long_press(EndpointId endpoint, uint8_t new_position)
{
    SwitchServer::Instance().OnLongPress(endpoint, new_position);
    return ESP_OK;
}

esp_err_t send_short_release(EndpointId endpoint, uint8_t previous_position)
{
    SwitchServer::Instance().OnShortRelease(endpoint, previous_position);
    return ESP_OK;
}

esp_err_t send_long_release(EndpointId endpoint, uint8_t previous_position)
{
    SwitchServer::Instance().OnLongRelease(endpoint, previous_position);
    return ESP_OK;
}

esp_err_t send_multi_press_ongoing(EndpointId endpoint, uint8_t new_position, uint8_t count)
{
    SwitchServer::Instance().OnMultiPressOngoing(endpoint, new_position, count);
    return ESP_OK;
}

esp_err_t send_multi_press_complete(EndpointId endpoint, uint8_t new_position, uint8_t count)
{
    SwitchServer::Instance().OnMultiPressComplete(endpoint, new_position, count);
    return ESP_OK;
}

} /* event */
} /* switch_cluster */

} /* cluster */
} /* esp_matter */
