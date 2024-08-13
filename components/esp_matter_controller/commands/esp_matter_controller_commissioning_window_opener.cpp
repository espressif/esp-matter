// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_controller_commissioning_window_opener.h>

#include <app-common/zap-generated/cluster-objects.h>
#include <app/server/Server.h>
#include <controller/CHIPCluster.h>
#include <crypto/CHIPCryptoPAL.h>
#include <lib/core/CHIPError.h>
#include <lib/core/NodeId.h>
#include <lib/core/Optional.h>
#include <setup_payload/ManualSetupPayloadGenerator.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>

using namespace chip::app::Clusters;

#define TAG "controller"

namespace esp_matter {
namespace controller {

esp_err_t commissioning_window_opener::send_open_commissioning_window_command(uint64_t node_id, bool is_enhanced,
                                                                              uint16_t timeout, uint32_t iteration,
                                                                              uint16_t discriminator,
                                                                              uint16_t timed_invoke_timeout_ms)
{
    if (!chip::IsOperationalNodeId(node_id)) {
        return ESP_ERR_INVALID_ARG;
    }
    m_is_enhanced = is_enhanced;
    m_timout = timeout;
    m_iteration = iteration;
    m_discriminator = discriminator;
    m_timed_invoke_timeout_ms = timed_invoke_timeout_ms;
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    chip::Server &server = chip::Server::GetInstance();
    server.GetCASESessionManager()->FindOrEstablishSession(ScopedNodeId(node_id, get_fabric_index()),
                                                           &on_device_connected_cb, &on_device_connection_failure_cb);
    return ESP_OK;
#else
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    if (CHIP_NO_ERROR ==
        controller_instance.get_commissioner()->GetConnectedDevice(node_id, &on_device_connected_cb,
                                                                   &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#else
    if (CHIP_NO_ERROR ==
        controller_instance.get_controller()->GetConnectedDevice(node_id, &on_device_connected_cb,
                                                                 &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    return ESP_OK;
}
static esp_err_t generate_pase_verifier(uint32_t iteration, uint32_t &pincode, chip::MutableByteSpan &salt,
                                        chip::Crypto::Spake2pVerifier &verifier)
{
    if (chip::Crypto::DRBG_get_bytes(salt.data(), salt.size()) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to generate salt");
        return ESP_FAIL;
    }
    if (chip::PASESession::GeneratePASEVerifier(verifier, iteration, salt, true, pincode) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to generate PASE verifier");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t generate_manual_code(uint32_t pincode, uint16_t discriminator, char *manual_code_buffer,
                                      size_t buffer_size)
{
    chip::SetupPayload payload = chip::SetupPayload();
    payload.setUpPINCode = pincode;
    payload.version = 0;
    payload.discriminator.SetLongValue(discriminator);
    payload.rendezvousInformation.SetValue(chip::RendezvousInformationFlag::kOnNetwork);
    char payload_buffer[chip::QRCodeBasicSetupPayloadGenerator::kMaxQRCodeBase38RepresentationLength + 1];
    chip::MutableCharSpan manual_code(payload_buffer);
    CHIP_ERROR err = chip::ManualSetupPayloadGenerator(payload).payloadDecimalStringRepresentation(manual_code);
    if (err != CHIP_NO_ERROR || manual_code.size() >= buffer_size) {
        return ESP_FAIL;
    }
    strncpy(manual_code_buffer, manual_code.data(), manual_code.size());
    manual_code_buffer[manual_code.size()] = 0;
    return ESP_OK;
}

void commissioning_window_opener::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                                          const SessionHandle &sessionHandle)
{
    commissioning_window_opener *window_opener = reinterpret_cast<commissioning_window_opener *>(context);
    if (!window_opener) {
        return;
    }
    if (window_opener->m_is_enhanced) {
        uint8_t salt_buffer[chip::Crypto::kSpake2p_Max_PBKDF_Salt_Length];
        chip::MutableByteSpan salt = chip::MutableByteSpan(salt_buffer);
        chip::Crypto::Spake2pVerifier verifier;
        if (generate_pase_verifier(window_opener->m_iteration, window_opener->m_pincode, salt, verifier) != ESP_OK) {
            return;
        }
        chip::Crypto::Spake2pVerifierSerialized serialized_verifier;
        chip::MutableByteSpan serialized_verifier_span(serialized_verifier);
        if (verifier.Serialize(serialized_verifier_span) != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Failed to serialize the verifier");
            return;
        }
        AdministratorCommissioning::Commands::OpenCommissioningWindow::Type command_data;
        command_data.commissioningTimeout = window_opener->m_timout;
        command_data.PAKEPasscodeVerifier = serialized_verifier_span;
        command_data.discriminator = window_opener->m_discriminator;
        command_data.iterations = window_opener->m_iteration;
        command_data.salt = salt;

        chip::Controller::ClusterBase cluster(exchangeMgr, sessionHandle, window_opener->m_default_remote_endpoint_id);
        cluster.InvokeCommand(command_data, window_opener, send_command_success_callback, send_command_failure_callback,
                              chip::MakeOptional(window_opener->m_timed_invoke_timeout_ms));
    } else {
        AdministratorCommissioning::Commands::OpenBasicCommissioningWindow::Type command_data;
        command_data.commissioningTimeout = window_opener->m_timout;
        chip::Controller::ClusterBase cluster(exchangeMgr, sessionHandle, window_opener->m_default_remote_endpoint_id);
        cluster.InvokeCommand(command_data, window_opener, send_command_success_callback, send_command_failure_callback,
                              chip::MakeOptional(window_opener->m_timed_invoke_timeout_ms));
    }
}

void commissioning_window_opener::on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId,
                                                                   CHIP_ERROR error)
{
    commissioning_window_opener *window_opener = reinterpret_cast<commissioning_window_opener *>(context);
    if (window_opener) {
        ESP_LOGE(TAG, "Failed to establish CASE session for open %s commisioning window command",
                 window_opener->m_is_enhanced ? "enhanced" : "basic");
    }
}

void commissioning_window_opener::send_command_success_callback(void *context,
                                                                const chip::app::DataModel::NullObjectType &data)
{
    commissioning_window_opener *window_opener = reinterpret_cast<commissioning_window_opener *>(context);
    if (!window_opener) {
        return;
    }
    ESP_LOGI(TAG, "Open %s commissioning window finished", window_opener->m_is_enhanced ? "enhanced" : "basic");
    if (window_opener->m_is_enhanced) {
        if (window_opener->m_callback) {
            char manual_code[22];
            if (generate_manual_code(window_opener->m_pincode, window_opener->m_discriminator, manual_code,
                                     sizeof(manual_code)) == ESP_OK) {
                window_opener->m_callback(manual_code);
            }
        }
    }
}

void commissioning_window_opener::send_command_failure_callback(void *context, CHIP_ERROR error)
{
    commissioning_window_opener *window_opener = reinterpret_cast<commissioning_window_opener *>(context);
    if (window_opener) {
        ESP_LOGE(TAG, "Failed to send open %s commisioning window command",
                 window_opener->m_is_enhanced ? "enhanced" : "basic");
    }
}

} // namespace controller
} // namespace esp_matter
