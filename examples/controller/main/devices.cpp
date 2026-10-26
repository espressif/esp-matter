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

#include "devices.h"

#include <esp_log.h>
#include <esp_matter_console.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_pairing_command.h>
#include <lib/support/BytesToHex.h>
#include <platform/KeyValueStoreManager.h>

#include <lib/support/CHIPArgParser.hpp>
#include <inttypes.h>
#include <string.h>

using namespace chip;
using namespace esp_matter::controller;

namespace {

const char *TAG = "devices";

constexpr uint32_t kStorageVersion = 1;
constexpr const char *kStorageKey = "controller/devices";

device_list_t s_devices = {};
bool s_registry_available = false;

int find_device(NodeId node)
{
    for (uint32_t i = 0; i < s_devices.count; ++i) {
        if (s_devices.nodes[i] == node) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

esp_err_t save_devices(const device_list_t &devices)
{
    CHIP_ERROR error = DeviceLayer::PersistedStorage::KeyValueStoreMgr().Put(kStorageKey, &devices, sizeof(devices));
    if (error != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Saving devices failed: %" CHIP_ERROR_FORMAT, error.Format());
        return ESP_FAIL;
    }
    s_devices = devices;
    s_registry_available = true;
    return ESP_OK;
}

esp_err_t load_devices()
{
    auto *commissioner = matter_controller_client::get_instance().get_commissioner();
    device_list_t devices = {};
    devices.version = kStorageVersion;

    size_t size = sizeof(devices);
    CHIP_ERROR error = DeviceLayer::PersistedStorage::KeyValueStoreMgr().Get(kStorageKey, &devices, sizeof(devices), &size);
    if (error == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND) {
        s_devices = devices;
        s_registry_available = true;
        return ESP_OK;
    }
    if (error != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Loading devices failed: %" CHIP_ERROR_FORMAT, error.Format());
        return ESP_FAIL;
    }

    if (size != sizeof(devices) || devices.version != kStorageVersion || devices.count > kMaxDevices) {
        return ESP_ERR_INVALID_VERSION;
    }
    for (uint32_t i = 0; i < devices.count; ++i) {
        if (!IsOperationalNodeId(devices.nodes[i]) || devices.nodes[i] == commissioner->GetNodeId()) {
            return ESP_ERR_INVALID_ARG;
        }
        for (uint32_t j = 0; j < i; ++j) {
            if (devices.nodes[i] == devices.nodes[j]) {
                return ESP_ERR_INVALID_ARG;
            }
        }
    }

    s_devices = devices;
    s_registry_available = true;
    return ESP_OK;
}

esp_err_t save_device(NodeId node)
{
    if (find_device(node) >= 0) {
        return ESP_OK;
    }
    if (s_devices.count == kMaxDevices) {
        return ESP_ERR_NO_MEM;
    }

    device_list_t devices = s_devices;
    devices.nodes[devices.count++] = node;
    return save_devices(devices);
}

class pairing_delegate final : public pairing::delegate {
public:
    void OnProgress(pairing::phase_t phase) override
    {
        ESP_LOGI(TAG, "Pairing phase: %u", static_cast<unsigned>(phase));
    }

    void OnDone(const pairing::completion_t &result) override
    {
        if (result.error != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Pairing node 0x%016" PRIx64 " failed: %" CHIP_ERROR_FORMAT,
                     result.node_id, result.error.Format());
            return;
        }

        esp_err_t err = save_device(result.node_id);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Node 0x%016" PRIx64 " commissioned but NOT saved: %s",
                     result.node_id, esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "Saved commissioned node=0x%016" PRIx64, result.node_id);
        }
    }
};

pairing_delegate s_pairing_delegate;
pairing::interaction s_pairing;

bool parse_node(const char *text, NodeId &node)
{
    return text[0] >= '0' && text[0] <= '9' && ArgParser::ParseInt(text, node, text[0] == '0' && (text[1] == 'x' || text[1] == 'X') ? 16 : 10) &&
           IsOperationalNodeId(node) && node != matter_controller_client::get_instance().get_commissioner()->GetNodeId();
}

esp_err_t pair_device(int argc, char **argv)
{
    pairing::request_t request;
    if (argc < 3 || !parse_node(argv[1], request.node_id)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (find_device(request.node_id) < 0 && s_devices.count == kMaxDevices) {
        return ESP_ERR_NO_MEM;
    }

    request.params.setup_code.payload = argv[2];
    uint8_t dataset[Controller::CommissioningParameters::kMaxThreadDatasetLen];
    bool transport_set = false;

    for (int index = 3; index < argc;) {
        if (strcmp(argv[index], "--transport") == 0 && index + 1 < argc && !transport_set) {
            const char *transport = argv[index + 1];
            if (strcmp(transport, "auto") == 0) {
                request.params.setup_code.transport = pairing::transport_t::automatic;
            } else if (strcmp(transport, "ble") == 0) {
                request.params.setup_code.transport = pairing::transport_t::ble;
            } else if (strcmp(transport, "on-network") == 0) {
                request.params.setup_code.transport = pairing::transport_t::on_network;
            } else {
                return ESP_ERR_INVALID_ARG;
            }

            transport_set = true;
            index += 2;
        } else if (strcmp(argv[index], "--wifi") == 0 && index + 2 < argc && !request.credentials.wifi.HasValue()) {
            request.credentials.wifi.SetValue(pairing::wifi_credentials_t{
                ByteSpan(reinterpret_cast<const uint8_t *>(argv[index + 1]), strlen(argv[index + 1])),
                ByteSpan(reinterpret_cast<const uint8_t *>(argv[index + 2]), strlen(argv[index + 2]))});
            index += 3;
        } else if (strcmp(argv[index], "--thread") == 0 && index + 1 < argc &&
                   !request.credentials.thread_dataset.HasValue()) {
            const char *hex = argv[index + 1];
            size_t dataset_size = Encoding::HexToBytes(hex, strlen(hex), dataset, sizeof(dataset));
            if (dataset_size == 0) {
                return ESP_ERR_INVALID_ARG;
            }

            request.credentials.thread_dataset.SetValue(ByteSpan(dataset, dataset_size));
            index += 2;
        } else {
            return ESP_ERR_INVALID_ARG;
        }
    }

    CHIP_ERROR error = s_pairing.start(request, pairing::options_t(), &s_pairing_delegate);
    if (error != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Pairing submission failed: %" CHIP_ERROR_FORMAT, error.Format());
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t devices_command(int argc, char **argv)
{
    if (argc == 1 && strcmp(argv[0], "clear") == 0) {
        if (s_pairing.active()) {
            ESP_LOGW(TAG, "Stop or cancel pairing before clearing the device registry");
            return ESP_ERR_INVALID_STATE;
        }

        device_list_t devices = {};
        devices.version = kStorageVersion;
        esp_err_t err = save_devices(devices);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Device registry cleared");
        }
        return err;
    }

    if (!s_registry_available) {
        ESP_LOGW(TAG, "Device registry unavailable; run matter esp device clear");
        return ESP_ERR_INVALID_STATE;
    }

    if (argc >= 1 && strcmp(argv[0], "pair") == 0) {
        if (argc == 2 && strcmp(argv[1], "status") == 0) {
            ESP_LOGI(TAG, "Pairing active: %s", s_pairing.active() ? "yes" : "no");
            return ESP_OK;
        }

        if (argc == 2 && strcmp(argv[1], "cancel") == 0) {
            return s_pairing.cancel() == CHIP_NO_ERROR ? ESP_OK : ESP_FAIL;
        }

        if (argc == 2 && strcmp(argv[1], "stop") == 0) {
            s_pairing.stop();
            return ESP_OK;
        }

        return pair_device(argc, argv);
    }

    if (argc == 1 && strcmp(argv[0], "list") == 0) {
        ESP_LOGI(TAG, "%" PRIu32 "/%u saved devices", s_devices.count, static_cast<unsigned>(kMaxDevices));
        for (uint32_t i = 0; i < s_devices.count; ++i) {
            ESP_LOGI(TAG, "node=0x%016" PRIx64, s_devices.nodes[i]);
        }
        return ESP_OK;
    }

    NodeId node;
    if (argc != 2 || strcmp(argv[0], "remove") != 0 || !parse_node(argv[1], node)) {
        return ESP_ERR_INVALID_ARG;
    }

    int index = find_device(node);
    if (index < 0) {
        return ESP_ERR_NOT_FOUND;
    }

    device_list_t devices = s_devices;
    for (uint32_t i = static_cast<uint32_t>(index); i + 1 < devices.count; ++i) {
        devices.nodes[i] = devices.nodes[i + 1];
    }
    devices.nodes[--devices.count] = kUndefinedNodeId;
    esp_err_t error = save_devices(devices);
    if (error != ESP_OK) {
        return error;
    }
    ESP_LOGI(TAG, "Removed node=0x%016" PRIx64, node);

    error = matter_controller_client::get_instance().unpair(node, [](NodeId node_id, CHIP_ERROR err) {
        ESP_LOGI(TAG, "Unpair node 0x%" PRIx64 " completed: %" CHIP_ERROR_FORMAT, node_id, err.Format());
    });
    if (error != ESP_OK) {
        ESP_LOGW(TAG, "Peer unpair submission failed: %s", esp_err_to_name(error));
    }
    return ESP_OK;
}

} // namespace

esp_err_t devices_init()
{
    esp_err_t error = load_devices();
    if (error != ESP_OK) {
        ESP_LOGW(TAG, "Device registry unavailable (%s); run matter esp device clear", esp_err_to_name(error));
    }

    static const esp_matter::console::command_t command = {
        .name = "device",
        .description = "Example device management. Usage: matter esp device list OR\n"
        "\tdevice pair <node-id> <setup-code> [--transport auto|ble|on-network]\n"
        "\t    [--wifi <ssid> <password>] [--thread <dataset-hex>]\n"
        "\tdevice pair status|cancel|stop OR\n"
        "\tdevice remove <node-id> OR\n"
        "\tdevice clear",
        .handler = devices_command,
    };
    return esp_matter::console::add_commands(&command, 1);
}
