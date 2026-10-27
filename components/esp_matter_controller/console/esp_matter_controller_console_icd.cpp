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
#include <esp_matter_controller_icd_client.h>
#include <crypto/CHIPCryptoPAL.h>
#include <lib/support/BytesToHex.h>

namespace esp_matter::controller::console {

using IcdKey = chip::Crypto::SensitiveDataFixedBuffer<chip::Crypto::kAES_CCM128_Key_Length>;

class IcdDelegate final : public icd_client::delegate {
public:
    void OnResponse(const icd_client::response_t &response) override
    {
        if (!response.status.IsSuccess()) {
            ESP_LOGE(TAG, "ICD response status: %" CHIP_ERROR_FORMAT, response.status.ToChipError().Format());
        } else if (response.icd_counter.HasValue()) {
            ESP_LOGI(TAG, "ICD counter: %" PRIu32, response.icd_counter.Value());
        }
    }

    void OnDone(CHIP_ERROR error) override
    {
        if (error == CHIP_NO_ERROR) {
            ESP_LOGI(TAG, "ICD request completed");
        } else {
            ESP_LOGE(TAG, "ICD request failed: %" CHIP_ERROR_FORMAT, error.Format());
        }
    }
};

static bool parse_icd_key(const char *text, IcdKey &key)
{
    return strlen(text) == key.Length() * 2 &&
           chip::Encoding::HexToBytes(text, strlen(text), key.Bytes(), key.Length()) == key.Length();
}

static CHIP_ERROR print_registered_icd(const icd_client::entry_t &entry, void *)
{
    ESP_LOGI(TAG, "%6u | %016" PRIx64 " | %016" PRIx64 " | %13" PRIu32 " | %14" PRIu32
             " | %016" PRIx64 " | %10u", static_cast<unsigned>(entry.peer_node.GetFabricIndex()),
             entry.peer_node.GetNodeId(), entry.check_in_node.GetNodeId(), entry.start_icd_counter, entry.offset,
             entry.monitored_subject, static_cast<unsigned>(entry.client_type));
    return CHIP_NO_ERROR;
}

static esp_err_t controller_icd_handler(int argc, char **argv)
{
    VerifyOrReturnError(argc > 0, ESP_ERR_INVALID_ARG);
    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    static IcdDelegate delegate;
    static icd_client::interaction operation;
    if (argc == 1 && strcmp(argv[0], "list") == 0) {
        ESP_LOGI(TAG, "Known ICDs (all fabrics):");
        ESP_LOGI(TAG, "%6s | %16s | %16s | %13s | %14s | %16s | %10s", "Fabric", "Peer Node ID", "CheckIn Node ID",
                 "Start Counter", "Counter Offset", "MonitoredSubject", "ClientType");
        return submission_result("ICD list", icd_client::for_each_registered_icd(print_registered_icd));
    }
    if (argc == 1 && strcmp(argv[0], "status") == 0) {
        ESP_LOGI(TAG, "ICD request active: %s", operation.active() ? "yes" : "no");
        return ESP_OK;
    }
    if (argc == 1 && strcmp(argv[0], "stop") == 0) {
        operation.stop();
        return ESP_OK;
    }

    const bool registering = strcmp(argv[0], "register") == 0;
    VerifyOrReturnError(registering || strcmp(argv[0], "unregister") == 0, ESP_ERR_INVALID_ARG);
    const int required_args = registering ? 7 : 3;
    VerifyOrReturnError(argc == required_args || argc == required_args + 2, ESP_ERR_INVALID_ARG);
    icd_client::request_t request = {};
    request.method = registering ? icd_client::METHOD_REGISTER : icd_client::METHOD_UNREGISTER;
    chip::NodeId check_in_node_id;
    VerifyOrReturnError(parse_unsigned(argv[1], request.node_id) && parse_unsigned(argv[2], check_in_node_id),
                        ESP_ERR_INVALID_ARG);

    IcdKey verification_key;
    const bool has_verification_key = argc == required_args + 2;
    if (has_verification_key) {
        VerifyOrReturnError(strcmp(argv[required_args], "--verification-key") == 0 &&
                            parse_icd_key(argv[required_args + 1], verification_key), ESP_ERR_INVALID_ARG);
    }
    IcdKey key;
    if (registering) {
        auto &params = request.params.registration;
        params.check_in_node_id = check_in_node_id;
        VerifyOrReturnError(parse_unsigned(argv[3], params.monitored_subject) && parse_icd_key(argv[4], key),
                            ESP_ERR_INVALID_ARG);
        params.key = key.Bytes();
        params.key_size = key.Length();
        VerifyOrReturnError(strcmp(argv[5], "--client-type") == 0, ESP_ERR_INVALID_ARG);
        using ClientType = chip::app::Clusters::IcdManagement::ClientTypeEnum;
        if (strcmp(argv[6], "permanent") == 0) {
            params.client_type = ClientType::kPermanent;
        } else if (strcmp(argv[6], "ephemeral") == 0) {
            params.client_type = ClientType::kEphemeral;
        } else {
            return ESP_ERR_INVALID_ARG;
        }
        params.verification_key = has_verification_key ? verification_key.Bytes() : nullptr;
        params.verification_key_size = has_verification_key ? verification_key.Length() : 0;
    } else {
        auto &params = request.params.unregistration;
        params.check_in_node_id = check_in_node_id;
        params.verification_key = has_verification_key ? verification_key.Bytes() : nullptr;
        params.verification_key_size = has_verification_key ? verification_key.Length() : 0;
    }
    return submission_result("ICD request", operation.start(request, &delegate));
}

esp_err_t register_icd_commands()
{
    static const command_t commands[] = {
        {
            .name = "icd",
            .description = "Usage: controller icd list|status|stop OR\n"
            "\ticd register <node> <check-in-node> <subject> <key-hex> --client-type permanent|ephemeral [--verification-key HEX] OR\n"
            "\ticd unregister <node> <check-in-node> [--verification-key HEX]\n"
            "\tKeys must contain exactly 32 hexadecimal digits.\n"
            "\tstop detaches observation; it does not undo remote registration changes.",
            .handler = controller_icd_handler,
        },
    };
    return register_commands(commands, sizeof(commands) / sizeof(commands[0]));
}

} // namespace esp_matter::controller::console
