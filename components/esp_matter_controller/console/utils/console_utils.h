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

#include <esp_matter_controller_console.h>
#include <esp_matter_console.h>
#include <esp_matter_client_target.h>
#include <esp_matter_core.h>
#include <esp_check.h>
#include <freertos/FreeRTOS.h>
#include <inttypes.h>
#include <lib/support/CHIPMem.h>
#include <lib/support/CHIPArgParser.hpp>
#include <lib/support/CodeUtils.h>
#include <platform/CHIPDeviceLayer.h>

namespace esp_matter::controller::console {

using esp_matter::console::command_t;
using chip::Platform::ScopedMemoryBufferWithSize;
constexpr const char *TAG = "controller_console";

esp_err_t register_commands(const command_t *commands, unsigned count);

esp_err_t submission_result(const char *operation, CHIP_ERROR error);

esp_err_t parse_timed_option(int argc, char **argv, uint16_t &timeout_ms);

bool parse_command_target(const char *node_arg, const char *endpoint_arg, chip::NodeId &node, chip::EndpointId &endpoint);

chip::FabricIndex active_fabric_index();

template <typename T>
bool parse_unsigned(const char *text, T &value)
{
    return text != nullptr && *text >= '0' && *text <= '9' &&
           chip::ArgParser::ParseInt(text, value, text[0] == '0' && (text[1] == 'x' || text[1] == 'X') ? 16 : 10);
}

template <typename T>
esp_err_t string_to_uint_array(const char *str, ScopedMemoryBufferWithSize<T> &array)
{
    ESP_RETURN_ON_FALSE(str != nullptr && *str != '\0', ESP_ERR_INVALID_ARG, TAG, "Empty ID list");
    size_t count = 1;
    for (const char *cursor = str; *cursor != '\0'; ++cursor) {
        count += *cursor == ',';
    }
    ESP_RETURN_ON_FALSE(array.Alloc(count), ESP_ERR_NO_MEM, TAG, "Failed to allocate IDs");
    for (size_t index = 0; index < count; ++index) {
        const char *end = strchr(str, ',');
        size_t length = end == nullptr ? strlen(str) : static_cast<size_t>(end - str);
        char number[11]; /* uint32_t in hexadecimal or decimal, plus NUL. */
        ESP_RETURN_ON_FALSE(length > 0 && length < sizeof(number),
                            ESP_ERR_INVALID_ARG, TAG, "Invalid ID");
        memcpy(number, str, length);
        number[length] = '\0';
        ESP_RETURN_ON_FALSE(parse_unsigned(number, array[index]), ESP_ERR_INVALID_ARG, TAG, "Invalid ID");
        if (end != nullptr) {
            str = end + 1;
        }
    }
    return ESP_OK;
}

template <typename Path>
static esp_err_t parse_path_columns(const char *endpoint_arg, const char *cluster_arg, const char *path_arg,
                                    ScopedMemoryBufferWithSize<Path> &paths)
{
    ScopedMemoryBufferWithSize<uint16_t> endpoint_ids;
    ScopedMemoryBufferWithSize<uint32_t> cluster_ids;
    ScopedMemoryBufferWithSize<uint32_t> path_ids;
    esp_err_t err = string_to_uint_array(endpoint_arg, endpoint_ids);
    if (err != ESP_OK) {
        return err;
    }
    err = string_to_uint_array(cluster_arg, cluster_ids);
    if (err != ESP_OK) {
        return err;
    }
    err = string_to_uint_array(path_arg, path_ids);
    if (err != ESP_OK) {
        return err;
    }
    ESP_RETURN_ON_FALSE(endpoint_ids.AllocatedSize() == cluster_ids.AllocatedSize() &&
                        endpoint_ids.AllocatedSize() == path_ids.AllocatedSize(),
                        ESP_ERR_INVALID_ARG, TAG, "Path array sizes differ");
    ESP_RETURN_ON_FALSE(paths.Alloc(endpoint_ids.AllocatedSize()), ESP_ERR_NO_MEM, TAG, "Failed to allocate paths");
    for (size_t index = 0; index < paths.AllocatedSize(); ++index) {
        paths[index] = Path(endpoint_ids[index], cluster_ids[index], path_ids[index]);
    }
    return ESP_OK;
}

} // namespace esp_matter::controller::console
