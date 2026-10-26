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

#include "console_utils.h"

#include <esp_matter_controller_commissioning_window_opener.h>
#include <setup_payload/ManualSetupPayloadGenerator.h>

namespace esp_matter::controller::console {

class WindowDelegate final : public controller::commissioning_window::delegate {
public:
    void OnResponse(const controller::commissioning_window::response_t &response) override
    {
        if (response.setup_payload == nullptr) {
            return;
        }
        char code[chip::kManualSetupLongCodeCharLength + 2] = {};
        chip::MutableCharSpan output(code);
        CHIP_ERROR error = chip::ManualSetupPayloadGenerator(*response.setup_payload).payloadDecimalStringRepresentation(output);
        if (error == CHIP_NO_ERROR) {
            ESP_LOGI(TAG, "Manual pairing code: %s", code);
        } else {
            ESP_LOGE(TAG, "Manual code generation failed: %" CHIP_ERROR_FORMAT, error.Format());
        }
    }
    void OnDone(CHIP_ERROR error) override
    {
        ESP_LOGI(TAG, "Window request completed: %" CHIP_ERROR_FORMAT, error.Format());
    }
};

static esp_err_t open_commissioning_window_handler(int argc, char **argv)
{
    static WindowDelegate window_delegate;
    static commissioning_window::interaction window_operation;
    if (argc == 1 && strcmp(argv[0], "status") == 0) {
        ESP_LOGI(TAG, "Window request active: %s", window_operation.active() ? "yes" : "no");
        return ESP_OK;
    }
    if (argc == 1 && strcmp(argv[0], "stop") == 0) {
        window_operation.stop();
        return ESP_OK;
    }
    VerifyOrReturnError(argc >= 3, ESP_ERR_INVALID_ARG);
    bool basic = strcmp(argv[0], "open-basic") == 0;
    VerifyOrReturnError(basic || strcmp(argv[0], "open-passcode") == 0, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(basic ? argc == 3 : argc >= 7 && argc % 2 == 1, ESP_ERR_INVALID_ARG);
    commissioning_window::request_t request(basic ? commissioning_window::method_t::basic : commissioning_window::method_t::passcode);
    chip::NodeId node_id;
    VerifyOrReturnError(parse_unsigned(argv[1], node_id) &&
                        parse_unsigned(argv[2], request.window_timeout_seconds), ESP_ERR_INVALID_ARG);
    const chip::FabricIndex fabric = active_fabric_index();
    VerifyOrReturnError(fabric != chip::kUndefinedFabricIndex, ESP_ERR_INVALID_STATE);
    request.node = chip::ScopedNodeId(node_id, fabric);
    bool iterations_set = false;
    bool discriminator_set = false;
    for (int index = 3; index < argc; index += 2) {
        auto &params = request.params.passcode;
        if (strcmp(argv[index], "--iterations") == 0 && !iterations_set) {
            VerifyOrReturnError(parse_unsigned(argv[index + 1], params.iterations), ESP_ERR_INVALID_ARG);
            iterations_set = true;
        } else if (strcmp(argv[index], "--discriminator") == 0 && !discriminator_set) {
            VerifyOrReturnError(parse_unsigned(argv[index + 1], params.discriminator), ESP_ERR_INVALID_ARG);
            discriminator_set = true;
        } else if (strcmp(argv[index], "--pin") == 0 && !params.setup_pin.HasValue()) {
            uint32_t pin;
            VerifyOrReturnError(parse_unsigned(argv[index + 1], pin), ESP_ERR_INVALID_ARG);
            params.setup_pin.SetValue(pin);
        } else {
            return ESP_ERR_INVALID_ARG;
        }
    }
    VerifyOrReturnError(basic || (iterations_set && discriminator_set), ESP_ERR_INVALID_ARG);
    return submission_result("Window request", window_operation.start(request, &window_delegate));
}

esp_err_t register_window_commands()
{
    static const command_t commands[] = {
        {
            .name = "window",
            .description = "Usage: controller window open-basic <node> <seconds> OR\n"
            "\twindow open-passcode <node> <seconds> --iterations N --discriminator N [--pin N]\n"
            "\twindow status|stop (local request only, not the remote window)",
            .handler = open_commissioning_window_handler,
        },
    };
    return register_commands(commands, sizeof(commands) / sizeof(commands[0]));
}

} // namespace esp_matter::controller::console
