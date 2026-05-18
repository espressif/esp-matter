// Copyright 2022-2026 Espressif Systems (Shanghai) PTE LTD
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

#include <controller/CommissioneeDeviceProxy.h>
#include <cJSON.h>
#include <esp_check.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_cluster_command.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_mem.h>
#include <json_parser.h>
#include <tlv_to_json.h>

#include <app/server/Server.h>
#include <crypto/CHIPCryptoPAL.h>
#include <lib/core/Optional.h>
#include <setup_payload/ManualSetupPayloadGenerator.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>
#include <setup_payload/SetupPayload.h>

using namespace esp_matter::client;
static const char *TAG = "cluster_command";

namespace esp_matter {

namespace controller {

void cluster_command::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                              const SessionHandle &sessionHandle)
{
    cluster_command *cmd = reinterpret_cast<cluster_command *>(context);
    chip::OperationalDeviceProxy device_proxy(&exchangeMgr, sessionHandle);
    chip::app::CommandPathParams command_path = {cmd->m_endpoint_id, 0, cmd->m_cluster_id, cmd->m_command_id,
                                                 chip::app::CommandPathFlags::kEndpointIdValid
                                                };
    interaction::invoke::send_request(context, &device_proxy, command_path, cmd->m_command_data_field,
                                      cmd->on_success_cb, cmd->on_error_cb, cmd->m_timed_invoke_timeout_ms);
    chip::Platform::Delete(cmd);
    return;
}

void cluster_command::on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    cluster_command *cmd = reinterpret_cast<cluster_command *>(context);
    if (cmd->on_connect_failure_cb) {
        cmd->on_connect_failure_cb(context, peerId, error);
    }
    chip::Platform::Delete(cmd);
    return;
}

void cluster_command::default_success_fcn(void *ctx, const ConcreteCommandPath &command_path, const StatusIB &status,
                                          TLVReader *response_data)
{
    ESP_LOGI(TAG, "Send command success");
    ESP_LOGI(TAG,
             "Some commands of specific clusters will have a response which is not NullObject, so we need to handle the "
             "response data for those commands. Here we print the response data.");

    if (!response_data) {
        ESP_LOGI(TAG, "No response payload");
        return;
    }

    cJSON *decoded_json = nullptr;
    if (tlv_to_json(*response_data, &decoded_json, tlv_to_json_options { .human_readable_bytes = true }) != ESP_OK) {
        ESP_LOGW(TAG, "Failed to convert response payload to JSON");
        cJSON_Delete(decoded_json);
        return;
    }

    char *formatted_response = cJSON_Print(decoded_json);
    if (formatted_response) {
        ESP_LOGI(TAG, "Response JSON:\n%s", formatted_response);
    }

    cJSON_free(formatted_response);
    cJSON_Delete(decoded_json);
}

void cluster_command::default_error_fcn(void *ctx, CHIP_ERROR error)
{
    ESP_LOGI(TAG, "Send command failure: err :%" CHIP_ERROR_FORMAT, error.Format());
}

esp_err_t cluster_command::dispatch_group_command(void *context)
{
    esp_err_t err = ESP_OK;
    cluster_command *cmd = reinterpret_cast<cluster_command *>(context);
    uint16_t group_id = cmd->m_destination_id & 0xFFFF;
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    uint8_t fabric_index = get_fabric_index();
#else
    uint8_t fabric_index = matter_controller_client::get_instance().get_fabric_index();
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    chip::app::CommandPathParams command_path = {cmd->m_endpoint_id, group_id, cmd->m_cluster_id, cmd->m_command_id,
                                                 chip::app::CommandPathFlags::kGroupIdValid
                                                };
    err = interaction::invoke::send_group_request(fabric_index, command_path, cmd->m_command_data_field);
    chip::Platform::Delete(cmd);
    return err;
}

esp_err_t cluster_command::send_command()
{
    if (is_group_command()) {
        return dispatch_group_command(reinterpret_cast<void *>(this));
    }
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    chip::Server &server = chip::Server::GetInstance();
    server.GetCASESessionManager()->FindOrEstablishSession(ScopedNodeId(m_destination_id, get_fabric_index()),
                                                           &on_device_connected_cb, &on_device_connection_failure_cb);
    return ESP_OK;
#else
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    if (CHIP_NO_ERROR ==
            controller_instance.get_commissioner()->GetConnectedDevice(m_destination_id, &on_device_connected_cb,
                                                                       &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#else
    if (CHIP_NO_ERROR ==
            controller_instance.get_controller()->GetConnectedDevice(m_destination_id, &on_device_connected_cb,
                                                                     &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    chip::Platform::Delete(this);
    return ESP_FAIL;
}

esp_err_t send_invoke_cluster_command(uint64_t destination_id, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t command_id, const char *command_data_field,
                                      chip::Optional<uint16_t> timed_invoke_timeout_ms)
{
    cluster_command *cmd = chip::Platform::New<cluster_command>(destination_id, endpoint_id, cluster_id, command_id,
                                                                command_data_field, timed_invoke_timeout_ms);
    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for cluster_command");
        return ESP_ERR_NO_MEM;
    }

    return cmd->send_command();
}

} // namespace controller
} // namespace esp_matter
