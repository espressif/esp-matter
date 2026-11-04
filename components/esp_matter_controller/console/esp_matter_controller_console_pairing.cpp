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

#include <esp_matter_controller_client.h>
#include <esp_matter_controller_pairing_command.h>
#include <lib/support/BytesToHex.h>

namespace esp_matter::controller::console {

class PairingDelegate final : public controller::pairing::delegate {
public:
    void OnProgress(controller::pairing::phase_t phase) override
    {
        ESP_LOGI(TAG, "Pairing phase: %u", static_cast<unsigned>(phase));
    }
    void OnCommissioningStage(chip::Controller::CommissioningStage stage) override
    {
        ESP_LOGI(TAG, "Commissioning stage: %s", chip::Controller::StageToString(stage));
    }
    void OnDone(const controller::pairing::completion_t &result) override
    {
        ESP_LOGI(TAG, "Pairing node 0x%" PRIx64 " completed: %" CHIP_ERROR_FORMAT,
                 result.node_id, result.error.Format());
        if (result.details != nullptr && result.details->failedStage.HasValue()) {
            ESP_LOGE(TAG, "Failed commissioning stage: %s",
                     chip::Controller::StageToString(result.details->failedStage.Value()));
        }
        if (result.details != nullptr && result.details->networkCommissioningStatus.HasValue()) {
            ESP_LOGE(TAG, "Network commissioning status: %u",
                     static_cast<unsigned>(result.details->networkCommissioningStatus.Value()));
        }
    }
};

static void console_unpair_complete(chip::NodeId node, CHIP_ERROR error)
{
    ESP_LOGI(TAG, "Unpair node 0x%" PRIx64 " completed: %" CHIP_ERROR_FORMAT, node, error.Format());
}

static esp_err_t controller_unpair_handler(int argc, char **argv)
{
    chip::NodeId node;
    VerifyOrReturnError(argc == 1 && parse_unsigned(argv[0], node) && chip::IsOperationalNodeId(node), ESP_ERR_INVALID_ARG);
    return matter_controller_client::get_instance().unpair(node, console_unpair_complete);
}

static esp_err_t controller_pairing_handler(int argc, char **argv)
{
    static PairingDelegate delegate;
    static pairing::interaction operation;
    if (argc == 1 && strcmp(argv[0], "status") == 0) {
        ESP_LOGI(TAG, "Pairing active: %s", operation.active() ? "yes" : "no");
        return ESP_OK;
    }
    if (argc == 1 && strcmp(argv[0], "cancel") == 0) {
        return submission_result("Pairing cancellation", operation.cancel());
    }
    if (argc == 1 && strcmp(argv[0], "stop") == 0) {
        operation.stop();
        return ESP_OK;
    }
    pairing::request_t request;
#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
    auto *udc = matter_controller_client::get_instance().get_discovery_controller();
    auto *peer = udc->GetUDCClientState();
    bool from_udc = argc > 0 && strcmp(argv[0], "udc") == 0;
    if (from_udc) {
        if (argc == 2 && strcmp(argv[1], "status") == 0) {
            if (peer == nullptr) {
                ESP_LOGI(TAG, "No pending UDC peer");
            } else {
                char address[chip::Transport::PeerAddress::kMaxToStringSize];
                peer->GetPeerAddress().ToString(address);
                ESP_LOGI(TAG, "UDC peer: %s (%s)", peer->GetInstanceName(), address);
            }
            return ESP_OK;
        }
        VerifyOrReturnError(argc == 3 && peer != nullptr && parse_unsigned(argv[1], request.node_id), ESP_ERR_INVALID_ARG);
        pairing::address_t address{peer->GetPeerAddress(), 0};
        VerifyOrReturnError(parse_unsigned(argv[2], address.pin), ESP_ERR_INVALID_ARG);
        request.set_address(address);
    } else
#endif // CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
    {
        VerifyOrReturnError(argc >= 2 && parse_unsigned(argv[0], request.node_id), ESP_ERR_INVALID_ARG);
        int index = 2;
        if (strcmp(argv[1], "--address") == 0) {
            VerifyOrReturnError(argc >= 3, ESP_ERR_INVALID_ARG);
            chip::Inet::IPAddress ip;
            chip::Inet::InterfaceId interface;
            VerifyOrReturnError(chip::Inet::IPAddress::FromString(argv[2], ip, interface), ESP_ERR_INVALID_ARG);
            request.set_address({chip::Transport::PeerAddress::UDP(ip, CHIP_PORT, interface), 0});
            index = 3;
        } else {
            request.params.setup_code.payload = argv[1];
        }
        uint8_t dataset[chip::Controller::CommissioningParameters::kMaxThreadDatasetLen];
        bool transport_set = false;
        bool port_set = false;
        while (index < argc) {
            VerifyOrReturnError(index + 1 < argc, ESP_ERR_INVALID_ARG);
            const char *option = argv[index++];
            const char *value = argv[index++];
            if (strcmp(option, "--wifi") == 0 && !request.credentials.wifi.HasValue()) {
                VerifyOrReturnError(index < argc, ESP_ERR_INVALID_ARG);
                request.credentials.wifi.SetValue(pairing::wifi_credentials_t{
                    chip::ByteSpan(reinterpret_cast<const uint8_t *>(value), strlen(value)),
                    chip::ByteSpan(reinterpret_cast<const uint8_t *>(argv[index]), strlen(argv[index]))});
                ++index;
            } else if (strcmp(option, "--thread") == 0 && !request.credentials.thread_dataset.HasValue()) {
                size_t size = chip::Encoding::HexToBytes(value, strlen(value), dataset, sizeof(dataset));
                VerifyOrReturnError(size != 0, ESP_ERR_INVALID_ARG);
                request.credentials.thread_dataset.SetValue(chip::ByteSpan(dataset, size));
            } else if (request.method == pairing::method_t::setup_code && strcmp(option, "--transport") == 0 && !transport_set) {
                if (strcmp(value, "auto") == 0) {
                    request.params.setup_code.transport = pairing::transport_t::automatic;
                } else if (strcmp(value, "ble") == 0) {
                    request.params.setup_code.transport = pairing::transport_t::ble;
                } else if (strcmp(value, "on-network") == 0) {
                    request.params.setup_code.transport = pairing::transport_t::on_network;
                } else {
                    return ESP_ERR_INVALID_ARG;
                }
                transport_set = true;
            } else if (request.method == pairing::method_t::address && strcmp(option, "--pin") == 0 && request.params.address.pin == 0) {
                VerifyOrReturnError(parse_unsigned(value, request.params.address.pin) && request.params.address.pin != 0, ESP_ERR_INVALID_ARG);
            } else if (request.method == pairing::method_t::address && strcmp(option, "--port") == 0 && !port_set) {
                uint16_t port;
                VerifyOrReturnError(parse_unsigned(value, port) && port != 0, ESP_ERR_INVALID_ARG);
                request.params.address.peer.SetPort(port);
                port_set = true;
            } else if (request.method == pairing::method_t::address && strcmp(option, "--interface") == 0 &&
                       !request.params.address.peer.GetInterface().IsPresent()) {
                chip::Inet::InterfaceId interface;
                VerifyOrReturnError(chip::Inet::InterfaceId::InterfaceNameToId(value, interface) == CHIP_NO_ERROR && interface.IsPresent(), ESP_ERR_INVALID_ARG);
                request.params.address.peer.SetInterface(interface);
            } else {
                return ESP_ERR_INVALID_ARG;
            }
        }
        /* start() copies credential spans before their stack buffers expire. */
        return submission_result("Pairing", operation.start(request, pairing::options_t(), &delegate));
    }
#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
    CHIP_ERROR error = operation.start(request, pairing::options_t(), &delegate);
    if (error == CHIP_NO_ERROR) {
        peer->Reset();
        udc->ResetState();
    }
    return submission_result("UDC pairing", error);
#endif // CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
}

esp_err_t register_pairing_commands()
{
    static const command_t commands[] = {
        {
            .name = "pair",
            .description = "Usage: controller pair <node> <code> [--transport auto|ble|on-network] [--wifi SSID PASSWORD] [--thread HEX] OR\n"
            "\tpair <node> --address IP --pin PIN [--port N] [--interface NAME] OR\n"
            "\tpair status|cancel|stop\n"
#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
            "\tpair udc status OR pair udc <node> <pin>\n"
#endif // CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
            "\tstop detaches observation; it does not undo remote commissioning.",
            .handler = controller_pairing_handler,
        },
        {
            .name = "unpair",
            .description = "Usage: controller unpair <node>",
            .handler = controller_unpair_handler,
        },
    };
    return register_commands(commands, sizeof(commands) / sizeof(commands[0]));
}

} // namespace esp_matter::controller::console
