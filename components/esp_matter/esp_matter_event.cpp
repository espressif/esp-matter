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

#include <platform/DeviceControlServer.h>

static const char *TAG = "esp_matter_event";

using chip::DeviceLayer::DeviceControlServer;
using chip::DeviceLayer::SwitchDeviceControlDelegate;

/** cluster: DoorLock */
esp_err_t esp_matter_event_send_door_lock_alarm()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_door_state_change()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_lock_operation()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_lock_operation_error()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_lock_user_change()
{
    /* Not implemented */
    return ESP_OK;
}

/** cluster: PumpConfigurationAndControl */ 
esp_err_t esp_matter_event_send_supply_voltage_low()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_supply_voltage_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_power_missing_phase()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_system_pressure_low()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_system_pressure_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_dry_running()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_motor_temperature_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_pump_motor_fatal_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_electronic_temperature_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_pump_blocked()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_sensor_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_electronic_non_fatal_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_electronic_fatal_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_general_fault()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_leakage()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_air_detection()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t esp_matter_event_send_turbine_operation()
{
    /* Not implemented */
    return ESP_OK;
}

/* cluster: Switch */
static SwitchDeviceControlDelegate *get_switch_delegate()
{
    SwitchDeviceControlDelegate *delegate = DeviceControlServer::DeviceControlSvr().GetSwitchDelegate();
    if (!delegate) {
        ESP_LOGE(TAG, "No delegate registered to handle switch events");
    }
    return delegate;
}

esp_err_t esp_matter_event_send_switch_latched(uint8_t new_position)
{
    SwitchDeviceControlDelegate *delegate = get_switch_delegate();
    if (!delegate) {
        return ESP_FAIL;
    }
    delegate->OnSwitchLatched(new_position);
    return ESP_OK;
}

esp_err_t esp_matter_event_send_initial_press(uint8_t new_position)
{
    SwitchDeviceControlDelegate *delegate = get_switch_delegate();
    if (!delegate) {
        return ESP_FAIL;
    }
    delegate->OnInitialPressed(new_position);
    return ESP_OK;
}

esp_err_t esp_matter_event_send_long_press(uint8_t new_position)
{
    SwitchDeviceControlDelegate *delegate = get_switch_delegate();
    if (!delegate) {
        return ESP_FAIL;
    }
    delegate->OnLongPressed(new_position);
    return ESP_OK;
}

esp_err_t esp_matter_event_send_short_release(uint8_t previous_position)
{
    SwitchDeviceControlDelegate *delegate = get_switch_delegate();
    if (!delegate) {
        return ESP_FAIL;
    }
    delegate->OnShortReleased(previous_position);
    return ESP_OK;
}

esp_err_t esp_matter_event_send_long_release(uint8_t previous_position)
{
    SwitchDeviceControlDelegate *delegate = get_switch_delegate();
    if (!delegate) {
        return ESP_FAIL;
    }
    delegate->OnLongReleased(previous_position);
    return ESP_OK;
}

esp_err_t esp_matter_event_send_multi_press_ongoing(uint8_t new_position, uint8_t count)
{
    SwitchDeviceControlDelegate *delegate = get_switch_delegate();
    if (!delegate) {
        return ESP_FAIL;
    }
    delegate->OnMultiPressOngoing(new_position, count);
    return ESP_OK;
}

esp_err_t esp_matter_event_send_multi_press_complete(uint8_t new_position, uint8_t count)
{
    SwitchDeviceControlDelegate *delegate = get_switch_delegate();
    if (!delegate) {
        return ESP_FAIL;
    }
    delegate->OnMultiPressComplete(new_position, count);
    return ESP_OK;
}
