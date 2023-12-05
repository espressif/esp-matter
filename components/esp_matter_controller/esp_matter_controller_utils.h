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

#include <stdint.h>

#include <app/ConcreteAttributePath.h>
#include <app/AttributePathParams.h>
#include <app/EventHeader.h>
#include <lib/core/TLVReader.h>

namespace esp_matter {
namespace controller {
using attribute_report_cb_t = void (*)(uint64_t remote_node_id, const chip::app::ConcreteDataAttributePath &path,
                                       chip::TLV::TLVReader *data);
using event_report_cb_t = void (*)(uint64_t remote_node_id, const chip::app::EventHeader &header,
                                   chip::TLV::TLVReader *data);
using subscribe_done_cb_t = void (*)(uint64_t remote_node_id, uint32_t subscription_id);
using subscribe_failure_cb_t = void (*)(void *subscribe_command);
using read_done_cb_t = void (*)(uint64_t remote_node_id, const chip::app::AttributePathParams &path);

#if !CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
/**
 * @brief Set the fabric index of the controller.
 *        The controller should be able to send commands to the devices on this fabric.
 *
 *      This should be called after the controller is added to the fabric.
 */
void set_fabric_index(uint8_t fabric_index);

/**
 * @brief Get the fabric index of the controller.
 *
 */
uint8_t get_fabric_index();
#endif // !CONFIG_ESP_MATTER_COMMISSIONER_ENABLE

} // namespace controller
} // namespace esp_matter

int oct_str_to_byte_arr(char *oct_str, uint8_t *byte_array);

uint64_t string_to_uint64(char *str);

uint32_t string_to_uint32(char *str);

uint16_t string_to_uint16(char *str);

uint8_t string_to_uint8(char *str);

int64_t string_to_int64(char *str);

int32_t string_to_int32(char *str);

int16_t string_to_int16(char *str);

int8_t string_to_int8(char *str);

bool string_to_bool(char *str);
