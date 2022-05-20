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

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_core.h>

#include <app/clusters/bindings/BindingManager.h>
#include <zap-generated/CHIPClusters.h>

using chip::DeviceProxy;
using chip::FabricInfo;
using chip::kInvalidEndpointId;
using chip::OperationalDeviceProxy;
using chip::PeerId;
using chip::Callback::Callback;

static const char *TAG = "esp_matter_client";

namespace esp_matter {
namespace client {

static command_callback_t client_command_callback = NULL;
static void *client_command_callback_priv_data = NULL;
static bool initialize_binding_manager = false;

esp_err_t set_command_callback(command_callback_t callback, void *priv_data)
{
    client_command_callback = callback;
    client_command_callback_priv_data = priv_data;
    return ESP_OK;
}

/** TODO: Change g_remote_endpoint_id to something better. */
int g_remote_endpoint_id = kInvalidEndpointId;
void esp_matter_new_connection_success_callback(void *context, OperationalDeviceProxy *peer_device)
{
    ESP_LOGI(TAG, "New connection success");
    if (client_command_callback) {
        client_command_callback(peer_device, g_remote_endpoint_id, client_command_callback_priv_data);
    }
}

void esp_matter_new_connection_failure_callback(void *context, PeerId peerId, CHIP_ERROR error)
{
    ESP_LOGI(TAG, "New connection failure");
}

esp_err_t connect(int fabric_index, int node_id, int remote_endpoint_id)
{
    /* Get info */
    FabricInfo *fabric_info = chip::Server::GetInstance().GetFabricTable().FindFabricWithIndex(fabric_index);
    if (!fabric_info) {
        ESP_LOGE(TAG, "Couldn't find fabric info");
        return ESP_FAIL;
    }
    PeerId peer_id = fabric_info->GetPeerIdForNode(node_id);

    /* Find existing */
    DeviceProxy *peer_device = chip::Server::GetInstance().GetCASESessionManager()->FindExistingSession(peer_id);
    if (peer_device) {
        /* Callback if found */
        if (client_command_callback) {
            client_command_callback(peer_device, remote_endpoint_id, client_command_callback_priv_data);
        }
        return ESP_OK;
    }

    /* Create new connection */
    g_remote_endpoint_id = remote_endpoint_id;
    static Callback<chip::OnDeviceConnected> success_callback(esp_matter_new_connection_success_callback, NULL);
    static Callback<chip::OnDeviceConnectionFailure> failure_callback(esp_matter_new_connection_failure_callback, NULL);
    chip::Server::GetInstance().GetCASESessionManager()->FindOrEstablishSession(peer_id, &success_callback,
                                                                                &failure_callback);

    return ESP_OK;
}

static void esp_matter_command_client_binding_callback(const EmberBindingTableEntry &binding, DeviceProxy *peer_device,
                                                       void *context)
{
    if (client_command_callback) {
        client_command_callback(peer_device, binding.remote, client_command_callback_priv_data);
    }
}

esp_err_t cluster_update(int endpoint_id, int cluster_id)
{
    chip::BindingManager::GetInstance().NotifyBoundClusterChanged(endpoint_id, cluster_id, NULL);
    return ESP_OK;
}

static void __binding_manager_init(intptr_t arg)
{
    auto &server = chip::Server::GetInstance();
    struct chip::BindingManagerInitParams binding_init_params = {
        .mFabricTable = &server.GetFabricTable(),
        .mCASESessionManager = server.GetCASESessionManager(),
        .mStorage = &server.GetPersistentStorage(),
    };

    chip::BindingManager::GetInstance().Init(binding_init_params);
    chip::BindingManager::GetInstance().RegisterBoundDeviceChangedHandler(esp_matter_command_client_binding_callback);
}

void binding_manager_init()
{
    if (initialize_binding_manager) {
        chip::DeviceLayer::PlatformMgr().ScheduleWork(__binding_manager_init);
    }
}

void binding_init()
{
    initialize_binding_manager = true;
}
} /* client */

namespace cluster {
using client::peer_device_t;

static void send_command_success_callback(void *context, const chip::app::DataModel::NullObjectType &data)
{
    ESP_LOGI(TAG, "Send command success");
}

static void send_command_failure_callback(void *context, CHIP_ERROR error)
{
    ESP_LOGI(TAG, "FSend command failure");
}

namespace on_off {
namespace command {

esp_err_t send_on(peer_device_t *remote_device, int remote_endpoint_id)
{
    chip::Controller::OnOffCluster cluster;
    OnOff::Commands::On::Type command_data;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_off(peer_device_t *remote_device, int remote_endpoint_id)
{
    chip::Controller::OnOffCluster cluster;
    OnOff::Commands::Off::Type command_data;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_toggle(peer_device_t *remote_device, int remote_endpoint_id)
{
    chip::Controller::OnOffCluster cluster;
    OnOff::Commands::Toggle::Type command_data;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

} /* command */
} /* on_off */

namespace level_control {
namespace command {

esp_err_t send_move(peer_device_t *remote_device, int remote_endpoint_id, uint8_t move_mode, uint8_t rate,
                    uint8_t option_mask, uint8_t option_override)
{
    chip::Controller::LevelControlCluster cluster;
    LevelControl::Commands::Move::Type command_data;
    command_data.moveMode = (LevelControl::MoveMode)move_mode;
    command_data.rate = rate;
    command_data.optionMask = option_mask;
    command_data.optionOverride = option_override;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_move_to_level(peer_device_t *remote_device, int remote_endpoint_id, uint8_t level,
                             uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    chip::Controller::LevelControlCluster cluster;
    LevelControl::Commands::MoveToLevel::Type command_data;
    command_data.level = level;
    command_data.transitionTime = transition_time;
    command_data.optionMask = option_mask;
    command_data.optionOverride = option_override;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_move_to_level_with_on_off(peer_device_t *remote_device, int remote_endpoint_id, uint8_t level,
                                         uint16_t transition_time)
{
    chip::Controller::LevelControlCluster cluster;
    LevelControl::Commands::MoveToLevelWithOnOff::Type command_data;
    command_data.level = level;
    command_data.transitionTime = transition_time;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_move_with_on_off(peer_device_t *remote_device, int remote_endpoint_id, uint8_t move_mode, uint8_t rate)
{
    chip::Controller::LevelControlCluster cluster;
    LevelControl::Commands::MoveWithOnOff::Type command_data;
    command_data.moveMode = (LevelControl::MoveMode)move_mode;
    command_data.rate = rate;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_step(peer_device_t *remote_device, int remote_endpoint_id, uint8_t step_mode, uint8_t step_size,
                    uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    chip::Controller::LevelControlCluster cluster;
    LevelControl::Commands::Step::Type command_data;
    command_data.stepMode = (LevelControl::StepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime = transition_time;
    command_data.optionMask = option_mask;
    command_data.optionOverride = option_override;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_step_with_on_off(peer_device_t *remote_device, int remote_endpoint_id, uint8_t step_mode,
                                uint8_t step_size, uint16_t transition_time)
{
    chip::Controller::LevelControlCluster cluster;
    LevelControl::Commands::StepWithOnOff::Type command_data;
    command_data.stepMode = (LevelControl::StepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime = transition_time;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_stop(peer_device_t *remote_device, int remote_endpoint_id, uint8_t option_mask, uint8_t option_override)
{
    chip::Controller::LevelControlCluster cluster;
    LevelControl::Commands::Stop::Type command_data;
    command_data.optionMask = option_mask;
    command_data.optionOverride = option_override;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_stop_with_on_off(peer_device_t *remote_device, int remote_endpoint_id)
{
    chip::Controller::LevelControlCluster cluster;
    LevelControl::Commands::Stop::Type command_data;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

} /* command */
} /* level_control */

namespace color_control {
namespace command {

esp_err_t send_move_hue(peer_device_t *remote_device, int remote_endpoint_id, uint8_t move_mode, uint8_t rate,
                        uint8_t option_mask, uint8_t option_override)
{
    chip::Controller::ColorControlCluster cluster;
    ColorControl::Commands::MoveHue::Type command_data;
    command_data.moveMode = (ColorControl::HueMoveMode)move_mode;
    command_data.rate = rate;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_move_saturation(peer_device_t *remote_device, int remote_endpoint_id, uint8_t move_mode, uint8_t rate,
                               uint8_t option_mask, uint8_t option_override)
{
    chip::Controller::ColorControlCluster cluster;
    ColorControl::Commands::MoveSaturation::Type command_data;
    command_data.moveMode = (ColorControl::SaturationMoveMode)move_mode;
    command_data.rate = rate;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_move_to_hue(peer_device_t *remote_device, int remote_endpoint_id, uint8_t hue, uint8_t direction,
                           uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    chip::Controller::ColorControlCluster cluster;
    ColorControl::Commands::MoveToHue::Type command_data;
    command_data.hue = hue;
    command_data.direction = (ColorControl::HueDirection)direction;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_move_to_hue_and_saturation(peer_device_t *remote_device, int remote_endpoint_id, uint8_t hue,
                                          uint8_t saturation, uint16_t transition_time, uint8_t option_mask,
                                          uint8_t option_override)
{
    chip::Controller::ColorControlCluster cluster;
    ColorControl::Commands::MoveToHueAndSaturation::Type command_data;
    command_data.hue = hue;
    command_data.saturation = saturation;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_move_to_saturation(peer_device_t *remote_device, int remote_endpoint_id, uint8_t saturation,
                                  uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    chip::Controller::ColorControlCluster cluster;
    ColorControl::Commands::MoveToSaturation::Type command_data;
    command_data.saturation = saturation;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_step_hue(peer_device_t *remote_device, int remote_endpoint_id, uint8_t step_mode, uint8_t step_size,
                        uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    chip::Controller::ColorControlCluster cluster;
    ColorControl::Commands::StepHue::Type command_data;
    command_data.stepMode = (ColorControl::HueStepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_step_saturation(peer_device_t *remote_device, int remote_endpoint_id, uint8_t step_mode,
                               uint8_t step_size, uint16_t transition_time, uint8_t option_mask,
                               uint8_t option_override)
{
    chip::Controller::ColorControlCluster cluster;
    ColorControl::Commands::StepSaturation::Type command_data;
    command_data.stepMode = (ColorControl::SaturationStepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    cluster.Associate(remote_device, remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

} /* command */
} /* color_control */

} /* cluster */
} /* esp_matter */
