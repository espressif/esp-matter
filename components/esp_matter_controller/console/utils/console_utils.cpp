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

#include <sdkconfig.h>

#include "console_utils.h"

#ifndef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
#include <esp_matter_controller_client.h>
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

namespace esp_matter::controller::console {

static fabric_index_getter_t sFabricIndexGetter = nullptr;

void set_fabric_index_getter(fabric_index_getter_t getter)
{
    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    sFabricIndexGetter = getter;
}

bool parse_command_target(const char *node_arg, const char *endpoint_arg, chip::NodeId &node, chip::EndpointId &endpoint)
{
    if (strcmp(node_arg, "group") == 0) {
        chip::GroupId group;
        VerifyOrReturnError(parse_unsigned(endpoint_arg, group), false);
        node = chip::NodeIdFromGroupId(group);
        endpoint = chip::kInvalidEndpointId;
        return true;
    }
    return parse_unsigned(node_arg, node) && (chip::IsOperationalNodeId(node) || chip::IsGroupId(node)) &&
           parse_unsigned(endpoint_arg, endpoint);
}

esp_err_t parse_timed_option(int argc, char **argv, uint16_t &timeout_ms)
{
    timeout_ms = 0;
    if (argc == 0) {
        return ESP_OK;
    }
    ESP_RETURN_ON_FALSE(argc == 2 && strcmp(argv[0], "--timed-ms") == 0 &&
                        parse_unsigned(argv[1], timeout_ms) && timeout_ms > 0,
                        ESP_ERR_INVALID_ARG, TAG, "Expected --timed-ms <1..65535>");
    return ESP_OK;
}

esp_err_t submission_result(const char *operation, CHIP_ERROR error)
{
    if (error == CHIP_NO_ERROR) {
        return ESP_OK;
    }
    ESP_LOGE(TAG, "%s failed: %" CHIP_ERROR_FORMAT, operation, error.Format());
    if (error == CHIP_ERROR_BUSY || error == CHIP_ERROR_INCORRECT_STATE) {
        return ESP_ERR_INVALID_STATE;
    }
    if (error == CHIP_ERROR_INVALID_ARGUMENT) {
        return ESP_ERR_INVALID_ARG;
    }
    if (error == CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return error == CHIP_ERROR_NO_MEMORY ? ESP_ERR_NO_MEM : ESP_FAIL;
}

chip::FabricIndex active_fabric_index()
{
    if (sFabricIndexGetter != nullptr) {
        return sFabricIndexGetter();
    }
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    return chip::kUndefinedFabricIndex;
#else // !CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    return controller::matter_controller_client::get_instance().get_fabric_index();
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
}

} // namespace esp_matter::controller::console
