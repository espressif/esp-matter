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

#pragma once

#include <esp_err.h>

/** 
 * NOTE: These events are currently sent on all the endpoints where the corresponding cluster exists.
 * Once this behaviour is changed, the event APIs here would have the endpoint_id as an argument.
 * Issue: https://github.com/project-chip/connectedhomeip/issues/14624            
*/

esp_err_t esp_matter_event_send_switch_latched(uint8_t new_position);
esp_err_t esp_matter_event_send_initial_press(uint8_t new_position);
esp_err_t esp_matter_event_send_long_press(uint8_t new_position);
esp_err_t esp_matter_event_send_short_release(uint8_t previous_position);
esp_err_t esp_matter_event_send_long_release(uint8_t previous_position);
esp_err_t esp_matter_event_send_multi_press_ongoing(uint8_t new_position, uint8_t count);
esp_err_t esp_matter_event_send_multi_press_complete(uint8_t new_position, uint8_t count);
