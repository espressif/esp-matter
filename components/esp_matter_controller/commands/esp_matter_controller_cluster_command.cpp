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

#include <commands/clusters/DataModelLogger.h>
#include <controller/CommissioneeDeviceProxy.h>
#include <esp_check.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_cluster_command.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_mem.h>
#include <json_parser.h>

#include <app/server/Server.h>
#include <crypto/CHIPCryptoPAL.h>
#include <lib/core/Optional.h>
#include <setup_payload/ManualSetupPayloadGenerator.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>
#include <setup_payload/SetupPayload.h>

using namespace chip::app::Clusters;
using namespace esp_matter::cluster;
using namespace esp_matter::client;
static const char *TAG = "cluster_command";

namespace esp_matter {

namespace cluster {

template <typename CommandResponseObjectT>
esp_err_t decode_command_response(const ConcreteCommandPath &command_path, TLVReader *reader)
{
    ESP_RETURN_ON_FALSE(reader, ESP_ERR_INVALID_ARG, TAG, "reader cannot be NULL");
    ESP_RETURN_ON_FALSE(command_path.mClusterId == CommandResponseObjectT::GetClusterId() &&
                            command_path.mCommandId == CommandResponseObjectT::GetCommandId(),
                            ESP_ERR_INVALID_ARG, TAG, "Wrong command to decode");
    DataModelLogger::LogCommand(command_path, reader);
    return ESP_OK;
}

namespace group_key_management {
namespace command {

void decode_response(const ConcreteCommandPath &command_path, TLVReader *reader)
{
    if (command_path.mCommandId == GroupKeyManagement::Commands::KeySetReadResponse::Id) {
        decode_command_response<GroupKeyManagement::Commands::KeySetRead::Type::ResponseType>(command_path, reader);
    }
}

} // namespace command
} // namespace group_key_management

namespace groups {
namespace command {

void decode_response(const ConcreteCommandPath &command_path, TLVReader *reader)
{
    if (command_path.mCommandId == Groups::Commands::AddGroupResponse::Id) {
        decode_command_response<Groups::Commands::AddGroup::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == Groups::Commands::ViewGroupResponse::Id) {
        decode_command_response<Groups::Commands::ViewGroup::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == Groups::Commands::RemoveGroupResponse::Id) {
        decode_command_response<Groups::Commands::RemoveGroup::Type::ResponseType>(command_path, reader);
    }
}

} // namespace command
} // namespace groups

namespace scenes_management {
namespace command {

void decode_response(const ConcreteCommandPath &command_path, TLVReader *reader)
{
    if (command_path.mCommandId == ScenesManagement::Commands::AddSceneResponse::Id) {
        decode_command_response<ScenesManagement::Commands::AddScene::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == ScenesManagement::Commands::ViewSceneResponse::Id) {
        decode_command_response<ScenesManagement::Commands::ViewScene::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == ScenesManagement::Commands::RemoveSceneResponse::Id) {
        decode_command_response<ScenesManagement::Commands::RemoveScene::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == ScenesManagement::Commands::RemoveAllScenesResponse::Id) {
        decode_command_response<ScenesManagement::Commands::RemoveAllScenes::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == ScenesManagement::Commands::StoreSceneResponse::Id) {
        decode_command_response<ScenesManagement::Commands::StoreScene::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == ScenesManagement::Commands::GetSceneMembershipResponse::Id) {
        decode_command_response<ScenesManagement::Commands::GetSceneMembership::Type::ResponseType>(command_path,
                                                                                                    reader);
    }
}

} // namespace command
} // namespace scenes_management

namespace thermostat {
namespace command {

void decode_response(const ConcreteCommandPath &command_path, TLVReader *reader)
{
    if (command_path.mCommandId == Thermostat::Commands::GetWeeklyScheduleResponse::Id) {
        decode_command_response<Thermostat::Commands::GetWeeklySchedule::Type::ResponseType>(command_path, reader);
    }
}

} // namespace command
} // namespace thermostat

namespace door_lock {
namespace command {

void decode_response(const ConcreteCommandPath &command_path, TLVReader *reader)
{
    if (command_path.mCommandId == DoorLock::Commands::GetWeekDayScheduleResponse::Id) {
        decode_command_response<DoorLock::Commands::GetWeekDaySchedule::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == DoorLock::Commands::GetYearDayScheduleResponse::Id) {
        decode_command_response<DoorLock::Commands::GetYearDaySchedule::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == DoorLock::Commands::GetHolidayScheduleResponse::Id) {
        decode_command_response<DoorLock::Commands::GetHolidaySchedule::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == DoorLock::Commands::GetUserResponse::Id) {
        decode_command_response<DoorLock::Commands::GetUser::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == DoorLock::Commands::SetCredentialResponse::Id) {
        decode_command_response<DoorLock::Commands::SetCredential::Type::ResponseType>(command_path, reader);
    } else if (command_path.mCommandId == DoorLock::Commands::GetCredentialStatusResponse::Id) {
        decode_command_response<DoorLock::Commands::GetCredentialStatus::Type::ResponseType>(command_path, reader);
    }
}

} // namespace command
} // namespace door_lock

} // namespace cluster

namespace controller {

void cluster_command::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                              const SessionHandle &sessionHandle)
{
    cluster_command *cmd = reinterpret_cast<cluster_command *>(context);
    chip::OperationalDeviceProxy device_proxy(&exchangeMgr, sessionHandle);
    chip::app::CommandPathParams command_path = {cmd->m_endpoint_id, 0, cmd->m_cluster_id, cmd->m_command_id,
                                                 chip::app::CommandPathFlags::kEndpointIdValid};
    interaction::invoke::send_request(context, &device_proxy, command_path, cmd->m_command_data_field,
                                      cmd->on_success_cb, cmd->on_error_cb, cmd->m_timed_invoke_timeout_ms);
    chip::Platform::Delete(cmd);
    return;
}

void cluster_command::on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    cluster_command *cmd = reinterpret_cast<cluster_command *>(context);
    chip::Platform::Delete(cmd);
    return;
}

void cluster_command::default_success_fcn(void *ctx, const ConcreteCommandPath &command_path, const StatusIB &status,
                                          TLVReader *response_data)
{
    ESP_LOGI(TAG, "Send command success");
    ESP_LOGI(TAG,
             "Some commands of specific clusters will have a reponse which is not NullObject, so we need to handle the "
             "response data for those commands. Here we print the reponse data.");
    ESP_LOGI(TAG,
             "If your command's reponse is not printed here, please register another success callback when creating "
             "the cluster_command object to handle the reponse data.");
    switch (command_path.mClusterId) {
    case GroupKeyManagement::Id:
        cluster::group_key_management::command::decode_response(command_path, response_data);
        break;
    case Groups::Id:
        cluster::groups::command::decode_response(command_path, response_data);
        break;
    case ScenesManagement::Id:
        cluster::scenes_management::command::decode_response(command_path, response_data);
        break;
    case Thermostat::Id:
        cluster::thermostat::command::decode_response(command_path, response_data);
        break;
    case DoorLock::Id:
        cluster::door_lock::command::decode_response(command_path, response_data);
        break;
    default:
        break;
    }
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
                                                 chip::app::CommandPathFlags::kGroupIdValid};
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
