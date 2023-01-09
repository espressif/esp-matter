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

using namespace chip::app::Clusters;
using chip::BitMask;
using chip::DeviceProxy;
using chip::FabricInfo;
using chip::kInvalidEndpointId;
using chip::OperationalDeviceProxy;
using chip::OperationalSessionSetup;
using chip::ScopedNodeId;
using chip::Server;
using chip::SessionHandle;
using chip::Callback::Callback;
using chip::Messaging::ExchangeManager;

static const char *TAG = "esp_matter_client";

namespace esp_matter {
namespace client {

static command_callback_t client_command_callback = NULL;
static group_command_callback_t client_group_command_callback = NULL;
static void *command_callback_priv_data;
static bool initialize_binding_manager = false;

esp_err_t set_command_callback(command_callback_t callback, group_command_callback_t g_callback, void *priv_data)
{
    client_command_callback = callback;
    client_group_command_callback = g_callback;
    command_callback_priv_data = priv_data;
    return ESP_OK;
}

void esp_matter_connection_success_callback(void *context, ExchangeManager &exchangeMgr, SessionHandle &sessionHandle)
{
    command_handle_t *cmd_handle = static_cast<command_handle_t *>(context);
    if (!cmd_handle) {
        ESP_LOGE(TAG, "Failed to call connect_success_callback since the command handle is NULL");
        return;
    }
    ESP_LOGI(TAG, "New connection success");
    // Only unicast binding needs to establish the connection
    if (client_command_callback) {
        OperationalDeviceProxy device(&exchangeMgr, sessionHandle);
        client_command_callback(&device, cmd_handle, command_callback_priv_data);
    }
    chip::Platform::Delete(cmd_handle);
}

void esp_matter_connection_failure_callback(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    command_handle_t *cmd_handle = static_cast<command_handle_t *>(context);
    ESP_LOGI(TAG, "New connection failure");
    if (cmd_handle) {
        chip::Platform::Delete(cmd_handle);
    }
}

esp_err_t connect(uint8_t fabric_index, uint64_t node_id, command_handle_t *cmd_handle)
{
    static Callback<chip::OnDeviceConnected> success_callback(esp_matter_connection_success_callback, NULL);
    static Callback<chip::OnDeviceConnectionFailure> failure_callback(esp_matter_connection_failure_callback, NULL);

    command_handle_t *context = chip::Platform::New<command_handle_t>(cmd_handle);
    if (!context) {
        ESP_LOGE(TAG, "failed to alloc memory for the command handle");
        return ESP_ERR_NO_MEM;
    }
    success_callback.mContext = static_cast<void *>(context);
    failure_callback.mContext = static_cast<void *>(context);
    Server *server = &(chip::Server::GetInstance());
    server->GetCASESessionManager()->FindOrEstablishSession(ScopedNodeId(node_id, fabric_index), &success_callback,
                                                            &failure_callback);
    return ESP_OK;
}

esp_err_t group_command_send(uint8_t fabric_index, command_handle_t *cmd_handle)
{
    if (!cmd_handle) {
        ESP_LOGE(TAG, "command handle is null");
        return ESP_ERR_NO_MEM;
    }

    if (client_group_command_callback) {
        client_group_command_callback(fabric_index, cmd_handle, command_callback_priv_data);
    }

    return ESP_OK;
}

static void esp_matter_command_client_binding_callback(const EmberBindingTableEntry &binding,
                                                       OperationalDeviceProxy *peer_device, void *context)
{
    command_handle_t *cmd_handle = static_cast<command_handle_t *>(context);
    if (!cmd_handle) {
        ESP_LOGE(TAG, "Failed to call the binding callback since command handle is NULL");
        return;
    }
    if (binding.type == EMBER_UNICAST_BINDING && !cmd_handle->is_group && peer_device) {
        if (client_command_callback) {
            cmd_handle->endpoint_id = binding.remote;
            client_command_callback(peer_device, cmd_handle, command_callback_priv_data);
        }
    } else if (binding.type == EMBER_MULTICAST_BINDING && cmd_handle->is_group && !peer_device) {
        if (client_group_command_callback) {
            cmd_handle->group_id = binding.groupId;
            client_group_command_callback(binding.fabricIndex, cmd_handle, command_callback_priv_data);
        }
    }
}

static void esp_matter_binding_context_release(void *context)
{
    if (context) {
        chip::Platform::Delete(static_cast<command_handle_t *>(context));
    }
}

esp_err_t cluster_update(uint16_t local_endpoint_id, command_handle_t *cmd_handle)
{
    command_handle_t *context = chip::Platform::New<command_handle_t>(cmd_handle);
    if (!context) {
        ESP_LOGE(TAG, "failed to alloc memory for the command handle");
        return ESP_ERR_NO_MEM;
    }
    if (CHIP_NO_ERROR !=
        chip::BindingManager::GetInstance().NotifyBoundClusterChanged(local_endpoint_id, cmd_handle->cluster_id,
                                                                      static_cast<void *>(context))) {
        chip::Platform::Delete(context);
        ESP_LOGE(TAG, "failed to notify the bound cluster changed");
        return ESP_FAIL;
    }

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
    chip::BindingManager::GetInstance().RegisterBoundDeviceContextReleaseHandler(esp_matter_binding_context_release);
}

void binding_manager_init()
{
    if (initialize_binding_manager) {
        chip::DeviceLayer::PlatformMgr().ScheduleWork(__binding_manager_init);
        initialize_binding_manager = false;
    }
}

void binding_init()
{
    initialize_binding_manager = true;
}
} // namespace client

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

esp_err_t send_on(peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    OnOff::Commands::On::Type command_data;

    chip::Controller::OnOffCluster cluster(*remote_device->GetExchangeManager(),
                                           remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_on(uint8_t fabric_index, uint16_t group_id)
{
    OnOff::Commands::On::Type command_data;
    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_off(peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    OnOff::Commands::Off::Type command_data;

    chip::Controller::OnOffCluster cluster(*remote_device->GetExchangeManager(),
                                           remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_off(uint8_t fabric_index, uint16_t group_id)
{
    OnOff::Commands::Off::Type command_data;
    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_toggle(peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    OnOff::Commands::Toggle::Type command_data;

    chip::Controller::OnOffCluster cluster(*remote_device->GetExchangeManager(),
                                           remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_toggle(uint8_t fabric_index, uint16_t group_id)
{
    OnOff::Commands::Toggle::Type command_data;
    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

} // namespace command
} // namespace on_off

namespace level_control {
namespace command {

esp_err_t send_move(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode, uint8_t rate,
                    uint8_t option_mask, uint8_t option_override)
{
    LevelControl::Commands::Move::Type command_data;
    command_data.moveMode = (LevelControl::MoveMode)move_mode;
    command_data.rate.SetNonNull(rate);
    command_data.optionsMask.SetRaw(option_mask);
    command_data.optionsOverride.SetRaw(option_override);

    chip::Controller::LevelControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move(uint8_t fabric_index, uint16_t group_id, uint8_t move_mode, uint8_t rate, uint8_t option_mask,
                          uint8_t option_override)
{
    LevelControl::Commands::Move::Type command_data;
    command_data.moveMode = (LevelControl::MoveMode)move_mode;
    command_data.rate.SetNonNull(rate);
    command_data.optionsMask.SetRaw(option_mask);
    command_data.optionsOverride.SetRaw(option_override);

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_move_to_level(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t level,
                             uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    LevelControl::Commands::MoveToLevel::Type command_data;
    command_data.level = level;
    command_data.transitionTime.SetNonNull(transition_time);
    command_data.optionsMask.SetRaw(option_mask);
    command_data.optionsOverride.SetRaw(option_override);

    chip::Controller::LevelControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_to_level(uint8_t fabric_index, uint16_t group_id, uint8_t level, uint16_t transition_time,
                                   uint8_t option_mask, uint8_t option_override)
{
    LevelControl::Commands::MoveToLevel::Type command_data;
    command_data.level = level;
    command_data.transitionTime.SetNonNull(transition_time);
    command_data.optionsMask.SetRaw(option_mask);
    command_data.optionsOverride.SetRaw(option_override);

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_move_to_level_with_on_off(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t level,
                                         uint16_t transition_time)
{
    LevelControl::Commands::MoveToLevelWithOnOff::Type command_data;
    command_data.level = level;
    command_data.transitionTime.SetNonNull(transition_time);

    chip::Controller::LevelControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_to_level_with_on_off(uint8_t fabric_index, uint16_t group_id, uint8_t level,
                                               uint16_t transition_time)
{
    LevelControl::Commands::MoveToLevelWithOnOff::Type command_data;
    command_data.level = level;
    command_data.transitionTime.SetNonNull(transition_time);

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_move_with_on_off(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode,
                                uint8_t rate)
{
    LevelControl::Commands::MoveWithOnOff::Type command_data;
    command_data.moveMode = (LevelControl::MoveMode)move_mode;
    command_data.rate.SetNonNull(rate);

    chip::Controller::LevelControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_with_on_off(uint8_t fabric_index, uint16_t group_id, uint8_t move_mode, uint8_t rate)
{
    LevelControl::Commands::MoveWithOnOff::Type command_data;
    command_data.moveMode = (LevelControl::MoveMode)move_mode;
    command_data.rate.SetNonNull(rate);

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_step(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t step_mode, uint8_t step_size,
                    uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    LevelControl::Commands::Step::Type command_data;
    command_data.stepMode = (LevelControl::StepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime.SetNonNull(transition_time);
    command_data.optionsMask.SetRaw(option_mask);
    command_data.optionsOverride.SetRaw(option_override);

    chip::Controller::LevelControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_step(uint8_t fabric_index, uint16_t group_id, uint8_t step_mode, uint8_t step_size,
                          uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    LevelControl::Commands::Step::Type command_data;
    command_data.stepMode = (LevelControl::StepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime.SetNonNull(transition_time);
    command_data.optionsMask.SetRaw(option_mask);
    command_data.optionsOverride.SetRaw(option_override);

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_step_with_on_off(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t step_mode,
                                uint8_t step_size, uint16_t transition_time)
{
    LevelControl::Commands::StepWithOnOff::Type command_data;
    command_data.stepMode = (LevelControl::StepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime.SetNonNull(transition_time);

    chip::Controller::LevelControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_step_with_on_off(uint8_t fabric_index, uint16_t group_id, uint8_t step_mode, uint8_t step_size,
                                      uint16_t transition_time)
{
    LevelControl::Commands::StepWithOnOff::Type command_data;
    command_data.stepMode = (LevelControl::StepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime.SetNonNull(transition_time);

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_stop(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t option_mask,
                    uint8_t option_override)
{
    LevelControl::Commands::Stop::Type command_data;
    command_data.optionsMask.SetRaw(option_mask);
    command_data.optionsOverride.SetRaw(option_override);

    chip::Controller::LevelControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_stop(uint8_t fabric_index, uint16_t group_id, uint8_t option_mask, uint8_t option_override)
{
    LevelControl::Commands::Stop::Type command_data;
    command_data.optionsMask.SetRaw(option_mask);
    command_data.optionsOverride.SetRaw(option_override);

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_stop_with_on_off(peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    LevelControl::Commands::Stop::Type command_data;

    chip::Controller::LevelControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_stop_with_on_off(uint8_t fabric_index, uint16_t group_id)
{
    LevelControl::Commands::Stop::Type command_data;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

} // namespace command
} // namespace level_control

namespace color_control {
namespace command {

esp_err_t send_move_hue(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode, uint8_t rate,
                        uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveHue::Type command_data;
    command_data.moveMode = (ColorControl::HueMoveMode)move_mode;
    command_data.rate = rate;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_hue(uint8_t fabric_index, uint16_t group_id, uint8_t move_mode, uint8_t rate,
                              uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveHue::Type command_data;
    command_data.moveMode = (ColorControl::HueMoveMode)move_mode;
    command_data.rate = rate;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_move_saturation(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode,
                               uint8_t rate, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveSaturation::Type command_data;
    command_data.moveMode = (ColorControl::SaturationMoveMode)move_mode;
    command_data.rate = rate;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_saturation(uint8_t fabric_index, uint16_t group_id, uint8_t move_mode, uint8_t rate,
                                     uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveSaturation::Type command_data;
    command_data.moveMode = (ColorControl::SaturationMoveMode)move_mode;
    command_data.rate = rate;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_move_to_hue(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t hue, uint8_t direction,
                           uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveToHue::Type command_data;
    command_data.hue = hue;
    command_data.direction = (ColorControl::HueDirection)direction;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_to_hue(uint8_t fabric_index, uint16_t group_id, uint8_t hue, uint8_t direction,
                                 uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveToHue::Type command_data;
    command_data.hue = hue;
    command_data.direction = (ColorControl::HueDirection)direction;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_move_to_hue_and_saturation(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t hue,
                                          uint8_t saturation, uint16_t transition_time, uint8_t option_mask,
                                          uint8_t option_override)
{
    ColorControl::Commands::MoveToHueAndSaturation::Type command_data;
    command_data.hue = hue;
    command_data.saturation = saturation;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_to_hue_and_saturation(uint8_t fabric_index, uint16_t group_id, uint8_t hue,
                                                uint8_t saturation, uint16_t transition_time, uint8_t option_mask,
                                                uint8_t option_override)
{
    ColorControl::Commands::MoveToHueAndSaturation::Type command_data;
    command_data.hue = hue;
    command_data.saturation = saturation;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_move_to_saturation(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t saturation,
                                  uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveToSaturation::Type command_data;
    command_data.saturation = saturation;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_to_saturation(uint8_t fabric_index, uint16_t group_id, uint8_t saturation,
                                        uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveToSaturation::Type command_data;
    command_data.saturation = saturation;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_step_hue(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t step_mode, uint8_t step_size,
                        uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::StepHue::Type command_data;
    command_data.stepMode = (ColorControl::HueStepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_step_hue(uint8_t fabric_index, uint16_t group_id, uint8_t step_mode, uint8_t step_size,
                              uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::StepHue::Type command_data;
    command_data.stepMode = (ColorControl::HueStepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_step_saturation(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t step_mode,
                               uint8_t step_size, uint16_t transition_time, uint8_t option_mask,
                               uint8_t option_override)
{
    ColorControl::Commands::StepSaturation::Type command_data;
    command_data.stepMode = (ColorControl::SaturationStepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_step_saturation(uint8_t fabric_index, uint16_t group_id, uint8_t step_mode, uint8_t step_size,
                                     uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::StepSaturation::Type command_data;
    command_data.stepMode = (ColorControl::SaturationStepMode)step_mode;
    command_data.stepSize = step_size;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_move_to_color(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t color_x,
                             uint16_t color_y, uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveToColor::Type command_data;
    command_data.colorX = color_x;
    command_data.colorY = color_y;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_to_color(uint8_t fabric_index, uint16_t group_id, uint16_t color_x, uint16_t color_y,
                                   uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveToColor::Type command_data;
    command_data.colorX = color_x;
    command_data.colorY = color_y;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_move_color(peer_device_t *remote_device, uint16_t remote_endpoint_id, int16_t rate_x, int16_t rate_y,
                          uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveColor::Type command_data;
    command_data.rateX = rate_x;
    command_data.rateY = rate_y;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_color(uint8_t fabric_index, uint16_t group_id, int16_t rate_x, int16_t rate_y,
                                uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveColor::Type command_data;
    command_data.rateX = rate_x;
    command_data.rateY = rate_y;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_step_color(peer_device_t *remote_device, uint16_t remote_endpoint_id, int16_t step_x, int16_t step_y,
                          uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::StepColor::Type command_data;
    command_data.stepX = step_x;
    command_data.stepY = step_y;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_step_color(uint8_t fabric_index, uint16_t group_id, int16_t step_x, int16_t step_y,
                                uint16_t transition_time, uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::StepColor::Type command_data;
    command_data.stepX = step_x;
    command_data.stepY = step_y;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_move_to_color_temperature(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                         uint16_t color_temperature_mireds, uint16_t transition_time,
                                         uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveToColorTemperature::Type command_data;
    command_data.colorTemperature = color_temperature_mireds;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_to_color_temperature(uint8_t fabric_index, uint16_t group_id,
                                               uint16_t color_temperature_mireds, uint16_t transition_time,
                                               uint8_t option_mask, uint8_t option_override)
{
    ColorControl::Commands::MoveToColorTemperature::Type command_data;
    command_data.colorTemperature = color_temperature_mireds;
    command_data.transitionTime = transition_time;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_stop_move_step(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t option_mask,
                              uint8_t option_override)
{
    ColorControl::Commands::StopMoveStep::Type command_data;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_stop_move_step(uint8_t fabric_index, uint16_t group_id, uint8_t option_mask,
                                    uint8_t option_override)
{
    ColorControl::Commands::StopMoveStep::Type command_data;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_move_color_temperature(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t move_mode,
                                      uint16_t rate, uint16_t color_temperature_minimum_mireds,
                                      uint16_t color_temperature_maximum_mireds, uint8_t option_mask,
                                      uint8_t option_override)
{
    ColorControl::Commands::MoveColorTemperature::Type command_data;
    command_data.moveMode = static_cast<ColorControl::HueMoveMode>(move_mode);
    command_data.rate = rate;
    command_data.colorTemperatureMinimumMireds = color_temperature_minimum_mireds;
    command_data.colorTemperatureMaximumMireds = color_temperature_maximum_mireds;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_move_color_temperature(uint8_t fabric_index, uint16_t group_id, uint8_t move_mode, uint16_t rate,
                                            uint16_t color_temperature_minimum_mireds,
                                            uint16_t color_temperature_maximum_mireds, uint8_t option_mask,
                                            uint8_t option_override)
{
    ColorControl::Commands::MoveColorTemperature::Type command_data;
    command_data.moveMode = static_cast<ColorControl::HueMoveMode>(move_mode);
    command_data.rate = rate;
    command_data.colorTemperatureMinimumMireds = color_temperature_minimum_mireds;
    command_data.colorTemperatureMaximumMireds = color_temperature_maximum_mireds;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}

esp_err_t send_step_color_temperature(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t step_mode,
                                      uint16_t step_size, uint16_t transition_time,
                                      uint16_t color_temperature_minimum_mireds,
                                      uint16_t color_temperature_maximum_mireds, uint8_t option_mask,
                                      uint8_t option_override)
{
    ColorControl::Commands::StepColorTemperature::Type command_data;
    command_data.stepMode = static_cast<ColorControl::HueStepMode>(step_mode);
    command_data.stepSize = step_size;
    command_data.transitionTime = transition_time;
    command_data.colorTemperatureMinimumMireds = color_temperature_minimum_mireds;
    command_data.colorTemperatureMaximumMireds = color_temperature_maximum_mireds;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Controller::ColorControlCluster cluster(*remote_device->GetExchangeManager(),
                                                  remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_step_color_temperature(uint8_t fabric_index, uint16_t group_id, uint8_t step_mode,
                                            uint16_t step_size, uint16_t transition_time,
                                            uint16_t color_temperature_minimum_mireds,
                                            uint16_t color_temperature_maximum_mireds, uint8_t option_mask,
                                            uint8_t option_override)
{
    ColorControl::Commands::StepColorTemperature::Type command_data;
    command_data.stepMode = static_cast<ColorControl::HueStepMode>(step_mode);
    command_data.stepSize = step_size;
    command_data.transitionTime = transition_time;
    command_data.colorTemperatureMinimumMireds = color_temperature_minimum_mireds;
    command_data.colorTemperatureMaximumMireds = color_temperature_maximum_mireds;
    command_data.optionsMask = option_mask;
    command_data.optionsOverride = option_override;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}


} // namespace command
} // namespace color_control

namespace identify {
namespace command {
esp_err_t send_identify(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t identify_time)
{
    Identify::Commands::Identify::Type command_data;
    command_data.identifyTime = identify_time;

    chip::Controller::IdentifyCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t group_send_identify(uint8_t fabric_index, uint16_t group_id, uint16_t identify_time)
{
    Identify::Commands::Identify::Type command_data;
    command_data.identifyTime = identify_time;

    chip::Messaging::ExchangeManager &exchange_mgr = chip::Server::GetInstance().GetExchangeManager();

    chip::Controller::InvokeGroupCommandRequest(&exchange_mgr, fabric_index, group_id, command_data);
    return ESP_OK;
}
} // namespace command
} // namespace identify

} // namespace cluster
} // namespace esp_matter
