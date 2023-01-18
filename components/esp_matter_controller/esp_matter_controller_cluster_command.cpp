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

#include <controller/CommissioneeDeviceProxy.h>
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
#include <esp_matter_commissioner.h>
#else
#include <app/server/Server.h>
#endif
#include <esp_matter_controller_cluster_command.h>
#include <esp_matter_controller_utils.h>

using namespace chip::app::Clusters;
static const char *TAG = "cluster_command";

namespace esp_matter {
namespace controller {

namespace clusters {

namespace on_off {

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    if (command_data->command_data_count != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    switch (command_data->command_id) {
    case OnOff::Commands::On::Id:
        return esp_matter::cluster::on_off::command::send_on(remote_device, remote_endpoint_id);
    case OnOff::Commands::Off::Id:
        return esp_matter::cluster::on_off::command::send_off(remote_device, remote_endpoint_id);
    case OnOff::Commands::Toggle::Id:
        return esp_matter::cluster::on_off::command::send_toggle(remote_device, remote_endpoint_id);
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace on_off

namespace level_control {

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case LevelControl::Commands::Move::Id:
        if (command_data->command_data_count != 4) {
            ESP_LOGE(TAG, "The command date should in following order: move_mode, rate, option_mask, option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::level_control::command::send_move(
            remote_device, remote_endpoint_id,
            /* move_mode */ string_to_uint8(command_data->command_data_str[0]),
            /* rate*/ string_to_uint8(command_data->command_data_str[1]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[2]),
            /* option_override*/ string_to_uint8(command_data->command_data_str[3]));
        break;
    case LevelControl::Commands::MoveToLevel::Id:
        if (command_data->command_data_count != 4) {
            ESP_LOGE(
                TAG,
                "The command date should in following order: level, transition_time, option_mask, option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::level_control::command::send_move_to_level(
            remote_device, remote_endpoint_id,
            /* level */ string_to_uint8(command_data->command_data_str[0]),
            /* transition_time */ string_to_uint16(command_data->command_data_str[1]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[2]),
            /* option_override */string_to_uint8(command_data->command_data_str[3]) );
        break;
    case LevelControl::Commands::Step::Id:
        if (command_data->command_data_count != 5) {
            ESP_LOGE(TAG,
                     "The command date should in following order: step_mode, step_size, transition_time, option_mask, "
                     "option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::level_control::command::send_step(
            remote_device, remote_endpoint_id,
            /* step_mode */ string_to_uint8(command_data->command_data_str[0]),
            /* step_size */ string_to_uint8(command_data->command_data_str[1]),
            /* transition_time */ string_to_uint16(command_data->command_data_str[2]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[3]),
            /* option_override */ string_to_uint8(command_data->command_data_str[4]));
        break;
    case LevelControl::Commands::Stop::Id:
        if (command_data->command_data_count != 2) {
            ESP_LOGE(TAG, "The command date should in following order: option_mask, option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::level_control::command::send_stop(
            remote_device, remote_endpoint_id,
            /* option_mask */ string_to_uint8(command_data->command_data_str[0]),
            /* option_override */ string_to_uint8(command_data->command_data_str[1]));
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace level_control

namespace color_control {

static esp_err_t send_command(command_data_t *command_data, peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    switch (command_data->command_id) {
    case ColorControl::Commands::MoveToHue::Id:
        if (command_data->command_data_count != 5) {
            ESP_LOGE(TAG,
                     "The command date should in following order: hue, direction, transition_time, option_mask, "
                     "option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::color_control::command::send_move_to_hue(
            remote_device, remote_endpoint_id,
            /* hue */ string_to_uint8(command_data->command_data_str[0]),
            /* direction */ string_to_uint8(command_data->command_data_str[1]),
            /* transition_time */ string_to_uint16(command_data->command_data_str[2]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[3]),
            /* option_override */ string_to_uint8(command_data->command_data_str[4]));
        break;
    case ColorControl::Commands::MoveToSaturation::Id:
        if (command_data->command_data_count != 4) {
            ESP_LOGE(TAG,
                     "The command date should in following order: saturation, transition_time, option_mask, "
                     "option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::color_control::command::send_move_to_saturation(
            remote_device, remote_endpoint_id,
            /* saturation */ string_to_uint8(command_data->command_data_str[0]),
            /* transition_time */ string_to_uint16(command_data->command_data_str[1]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[2]),
            /* option_override */ string_to_uint8(command_data->command_data_str[3]));
        break;
    case ColorControl::Commands::MoveToHueAndSaturation::Id:
        if (command_data->command_data_count != 5) {
            ESP_LOGE(TAG,
                     "The command date should in following order: hue, saturation, transition_time, option_mask, "
                     "option_override");
            return ESP_ERR_INVALID_ARG;
        }
        return esp_matter::cluster::color_control::command::send_move_to_hue_and_saturation(
            remote_device, remote_endpoint_id,
            /* hue */ string_to_uint8(command_data->command_data_str[0]),
            /* saturation */ string_to_uint8(command_data->command_data_str[1]),
            /* transition_time */ string_to_uint16(command_data->command_data_str[2]),
            /* option_mask */ string_to_uint8(command_data->command_data_str[3]),
            /* option_override */ string_to_uint8(command_data->command_data_str[4]));
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace color_control

} // namespace clusters

void cluster_command::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr, SessionHandle &sessionHandle)
{
    cluster_command *cmd = reinterpret_cast<cluster_command *>(context);
    chip::OperationalDeviceProxy device_proxy(&exchangeMgr, sessionHandle);
    switch (cmd->m_command_data->cluster_id) {
    case OnOff::Id:
        clusters::on_off::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case LevelControl::Id:
        clusters::level_control::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    case ColorControl::Id:
        clusters::color_control::send_command(cmd->m_command_data, &device_proxy, cmd->m_endpoint_id);
        break;
    default:
        break;
    }
    chip::Platform::Delete(cmd);
    return;
}

void cluster_command::on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    cluster_command *cmd = reinterpret_cast<cluster_command *>(context);
    chip::Platform::Delete(cmd);
    return;
}

esp_err_t cluster_command::send_command()
{
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    if (CHIP_NO_ERROR ==
        commissioner::get_device_commissioner()->GetConnectedDevice(m_node_id, &on_device_connected_cb,
                                                                    &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#else
    chip::Server *server = &(chip::Server::GetInstance());
    server->GetCASESessionManager()->FindOrEstablishSession(ScopedNodeId(m_node_id, /* fabric index */ 1),
                                                            &on_device_connected_cb, &on_device_connection_failure_cb);
    return ESP_OK;
#endif
    chip::Platform::Delete(this);
    return ESP_FAIL;
}

esp_err_t send_invoke_cluster_command(uint64_t node_id, uint16_t endpoint_id, int cmd_data_argc, char **cmd_data_argv)
{
    command_data_t *command_data = (command_data_t *)calloc(1, sizeof(command_data_t));
    if (!command_data) {
        ESP_LOGE(TAG, "Failed to alloc memory for command data");
        return ESP_ERR_NO_MEM;
    }
    command_data->cluster_id = string_to_uint32(cmd_data_argv[0]);
    command_data->command_id = string_to_uint32(cmd_data_argv[1]);
    command_data->command_data_count = cmd_data_argc - 2;

    for (size_t idx = 0; idx < command_data->command_data_count; idx++) {
        strncpy(command_data->command_data_str[idx], cmd_data_argv[2 + idx],
                strnlen(cmd_data_argv[2 + idx], k_max_command_data_str_len));
        command_data->command_data_str[idx][controller::k_max_command_data_str_len - 1] = 0;
    }
    cluster_command *cmd = chip::Platform::New<cluster_command>(node_id, endpoint_id, command_data);
    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for cluster_command");
        return ESP_ERR_NO_MEM;
    }

    return cmd->send_command();
}

} // namespace controller
} // namespace esp_matter
