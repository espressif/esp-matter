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

void esp_matter_connection_success_callback(void *context, ExchangeManager &exchangeMgr,
                                            const SessionHandle &sessionHandle)
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
    ESP_LOGI(TAG, "Send command failure: err: %" CHIP_ERROR_FORMAT, error.Format());
}
#if CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
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
    command_data.colorTemperatureMireds = color_temperature_mireds;
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
    command_data.colorTemperatureMireds = color_temperature_mireds;
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

esp_err_t send_trigger_effect(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t effect_identifier,
                              uint8_t effect_variant)
{
    Identify::Commands::TriggerEffect::Type command_data;
    command_data.effectIdentifier = Identify::EffectIdentifierEnum(effect_identifier);
    command_data.effectVariant = Identify::EffectVariantEnum(effect_variant);

    chip::Controller::IdentifyCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

} // namespace command
} // namespace identify

namespace group_key_management {
namespace command {

esp_err_t send_keyset_write(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                            group_keyset_struct &group_keyset)
{
    GroupKeyManagement::Commands::KeySetWrite::Type command_data;
    command_data.groupKeySet = group_keyset;

    chip::Controller::GroupKeyManagementCluster cluster(*remote_device->GetExchangeManager(),
                                                        remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_keyset_read(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t keyset_id,
                           keyset_read_callback keyset_read_cb)
{
    GroupKeyManagement::Commands::KeySetRead::Type command_data;
    command_data.groupKeySetID = keyset_id;

    chip::Controller::GroupKeyManagementCluster cluster(*remote_device->GetExchangeManager(),
                                                        remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, keyset_read_cb, send_command_failure_callback);
    return ESP_OK;
}

} // namespace command
} // namespace group_key_management

namespace groups {
namespace command {

esp_err_t send_add_group(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id, char *group_name,
                         add_group_callback add_group_cb)
{
    Groups::Commands::AddGroup::Type command_data;
    command_data.groupID = group_id;
    command_data.groupName = chip::CharSpan(group_name, strnlen(group_name, 16));

    chip::Controller::GroupsCluster cluster(*remote_device->GetExchangeManager(),
                                            remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, add_group_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_view_group(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                          view_group_callback view_group_cb)
{
    Groups::Commands::ViewGroup::Type command_data;
    command_data.groupID = group_id;

    chip::Controller::GroupsCluster cluster(*remote_device->GetExchangeManager(),
                                            remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, view_group_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_remove_group(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                            remove_group_callback remove_group_cb)
{
    Groups::Commands::RemoveGroup::Type command_data;
    command_data.groupID = group_id;

    chip::Controller::GroupsCluster cluster(*remote_device->GetExchangeManager(),
                                            remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, remove_group_cb, send_command_failure_callback);
    return ESP_OK;
}

} // namespace command
} // namespace groups

namespace scenes {
namespace command {

esp_err_t send_add_scene(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id, uint8_t scene_id,
                         uint16_t transition_time, char *scene_name, extension_field_sets &efs,
                         add_scene_callback add_scene_cb)
{
    Scenes::Commands::AddScene::Type command_data;
    command_data.groupID = group_id;
    command_data.sceneID = scene_id;
    command_data.transitionTime = transition_time;
    command_data.sceneName = chip::CharSpan(scene_name, strnlen(scene_name, 16));
    command_data.extensionFieldSets = efs;

    chip::Controller::ScenesCluster cluster(*remote_device->GetExchangeManager(),
                                            remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, add_scene_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_view_scene(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                          uint8_t scene_id, view_scene_callback view_scene_cb)
{
    Scenes::Commands::ViewScene::Type command_data;
    command_data.groupID = group_id;
    command_data.sceneID = scene_id;

    chip::Controller::ScenesCluster cluster(*remote_device->GetExchangeManager(),
                                            remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, view_scene_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_remove_scene(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                            uint8_t scene_id, remove_scene_callback remove_scene_cb)
{
    Scenes::Commands::RemoveScene::Type command_data;
    command_data.groupID = group_id;
    command_data.sceneID = scene_id;

    chip::Controller::ScenesCluster cluster(*remote_device->GetExchangeManager(),
                                            remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, remove_scene_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_remove_all_scenes(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                                 remove_all_scenes_callback remove_all_scenes_cb)
{
    Scenes::Commands::RemoveAllScenes::Type command_data;
    command_data.groupID = group_id;

    chip::Controller::ScenesCluster cluster(*remote_device->GetExchangeManager(),
                                            remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, remove_all_scenes_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_store_scene(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                           uint8_t scene_id, store_scene_callback store_scene_cb)
{
    Scenes::Commands::StoreScene::Type command_data;
    command_data.groupID = group_id;
    command_data.sceneID = scene_id;

    chip::Controller::ScenesCluster cluster(*remote_device->GetExchangeManager(),
                                            remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, store_scene_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_recall_scene(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                            uint8_t scene_id)
{
    Scenes::Commands::RecallScene::Type command_data;
    command_data.groupID = group_id;
    command_data.sceneID = scene_id;

    chip::Controller::ScenesCluster cluster(*remote_device->GetExchangeManager(),
                                            remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_get_scene_membership(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t group_id,
                                    get_scene_membership_callback get_scene_membership_cb)
{
    Scenes::Commands::GetSceneMembership::Type command_data;
    command_data.groupID = group_id;

    chip::Controller::ScenesCluster cluster(*remote_device->GetExchangeManager(),
                                            remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, get_scene_membership_cb, send_command_failure_callback);
    return ESP_OK;
}

} // namespace command
} // namespace scenes

namespace thermostat {
namespace command {

esp_err_t send_setpoint_raise_lower(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t mode,
                                    uint8_t amount)
{
    Thermostat::Commands::SetpointRaiseLower::Type command_data;
    command_data.mode = chip::app::Clusters::Thermostat::SetpointAdjustMode(mode);
    command_data.amount = amount;

    chip::Controller::ThermostatCluster cluster(*remote_device->GetExchangeManager(),
                                                remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_set_weekly_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                   uint8_t num_of_tras_for_seq, uint8_t day_of_week_for_seq, uint8_t mode_for_seq,
                                   transitions &trans)
{
    Thermostat::Commands::SetWeeklySchedule::Type command_data;
    command_data.numberOfTransitionsForSequence = num_of_tras_for_seq;
    command_data.dayOfWeekForSequence.SetRaw(day_of_week_for_seq);
    command_data.modeForSequence.SetRaw(mode_for_seq);
    command_data.transitions = trans;

    chip::Controller::ThermostatCluster cluster(*remote_device->GetExchangeManager(),
                                                remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_get_weekly_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t day_to_return,
                                   uint8_t mode_to_return, get_weekly_schedule_callback get_weekly_schedule_cb)
{
    Thermostat::Commands::GetWeeklySchedule::Type command_data;
    command_data.daysToReturn.SetRaw(day_to_return);
    command_data.modeToReturn.SetRaw(mode_to_return);

    chip::Controller::ThermostatCluster cluster(*remote_device->GetExchangeManager(),
                                                remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, get_weekly_schedule_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_clear_weekly_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    Thermostat::Commands::ClearWeeklySchedule::Type command_data;

    chip::Controller::ThermostatCluster cluster(*remote_device->GetExchangeManager(),
                                                remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

} // namespace command
} // namespace thermostat

namespace door_lock {
namespace command {

esp_err_t send_lock_door(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t timed_invoke_timeout_ms)
{
    DoorLock::Commands::LockDoor::Type command_data;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback,
                          chip::MakeOptional(timed_invoke_timeout_ms));
    return ESP_OK;
}

esp_err_t send_unlock_door(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t timed_invoke_timeout_ms)
{
    DoorLock::Commands::UnlockDoor::Type command_data;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback,
                          chip::MakeOptional(timed_invoke_timeout_ms));
    return ESP_OK;
}

esp_err_t send_unlock_with_timeout(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t timeout,
                                   uint16_t timed_invoke_timeout_ms)
{
    DoorLock::Commands::UnlockWithTimeout::Type command_data;
    command_data.timeout = timeout;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback,
                          chip::MakeOptional(timed_invoke_timeout_ms));
    return ESP_OK;
}

esp_err_t send_set_week_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t week_day_index,
                                     uint16_t user_index, uint8_t days_mask, uint8_t start_hour, uint8_t start_minute,
                                     uint8_t end_hour, uint8_t end_minute)
{
    DoorLock::Commands::SetWeekDaySchedule::Type command_data;
    command_data.weekDayIndex = week_day_index;
    command_data.userIndex = user_index;
    command_data.daysMask.SetRaw(days_mask);
    command_data.startHour = start_hour;
    command_data.startMinute = start_minute;
    command_data.endHour = end_hour;
    command_data.endMinute = end_minute;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_get_week_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t week_day_index,
                                     uint16_t user_index, get_week_day_schedule_callback success_cb)
{
    DoorLock::Commands::GetWeekDaySchedule::Type command_data;
    command_data.weekDayIndex = week_day_index;
    command_data.userIndex = user_index;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, success_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_clear_week_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                       uint8_t week_day_index, uint16_t user_index)
{
    DoorLock::Commands::ClearWeekDaySchedule::Type command_data;
    command_data.weekDayIndex = week_day_index;
    command_data.userIndex = user_index;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_set_year_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t year_day_index,
                                     uint16_t user_index, uint32_t local_start_time, uint32_t local_end_time)
{
    DoorLock::Commands::SetYearDaySchedule::Type command_data;
    command_data.yearDayIndex = year_day_index;
    command_data.userIndex = user_index;
    command_data.localStartTime = local_start_time;
    command_data.localEndTime = local_end_time;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_get_year_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t year_day_index,
                                     uint16_t user_index, get_year_day_schedule_callback success_cb)
{
    DoorLock::Commands::GetYearDaySchedule::Type command_data;
    command_data.yearDayIndex = year_day_index;
    command_data.userIndex = user_index;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, success_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_clear_year_day_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                       uint8_t year_day_index, uint16_t user_index)
{
    DoorLock::Commands::ClearYearDaySchedule::Type command_data;
    command_data.yearDayIndex = year_day_index;
    command_data.userIndex = user_index;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_set_holiday_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t holiday_index,
                                    uint32_t local_start_time, uint32_t local_end_time, uint8_t operating_mode)
{
    DoorLock::Commands::SetHolidaySchedule::Type command_data;
    command_data.holidayIndex = holiday_index;
    command_data.localStartTime = local_start_time;
    command_data.localEndTime = local_end_time;
    command_data.operatingMode = DoorLock::OperatingModeEnum(operating_mode);

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_get_holiday_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t holiday_index,
                                    get_holiday_schedule_callback success_cb)
{
    DoorLock::Commands::GetHolidaySchedule::Type command_data;
    command_data.holidayIndex = holiday_index;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, success_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_clear_holiday_schedule(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t holiday_index)
{
    DoorLock::Commands::ClearHolidaySchedule::Type command_data;
    command_data.holidayIndex = holiday_index;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_set_user(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t operation_type,
                        uint16_t user_index, char *user_name, uint32_t user_unique_id, uint8_t user_status,
                        uint8_t user_type, uint8_t credential_rule, uint16_t timed_invoke_timeout_ms)
{
    DoorLock::Commands::SetUser::Type command_data;
    command_data.operationType = DoorLock::DataOperationTypeEnum(operation_type);
    command_data.userIndex = user_index;
    if (user_name) {
        command_data.userName = chip::app::DataModel::MakeNullable(chip::CharSpan(user_name, strlen(user_name)));
    }
    command_data.userUniqueID = chip::app::DataModel::MakeNullable(user_unique_id);
    command_data.userStatus = chip::app::DataModel::MakeNullable(DoorLock::UserStatusEnum(user_status));
    command_data.userType = chip::app::DataModel::MakeNullable(DoorLock::UserTypeEnum(user_type));
    command_data.credentialRule = chip::app::DataModel::MakeNullable(DoorLock::CredentialRuleEnum(credential_rule));

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback,
                          chip::MakeOptional(timed_invoke_timeout_ms));
    return ESP_OK;
}

esp_err_t send_get_user(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t user_index,
                        get_user_callback success_cb)
{
    DoorLock::Commands::GetUser::Type command_data;
    command_data.userIndex = user_index;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, success_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_clear_user(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t user_index,
                          uint16_t timed_invoke_timeout_ms)
{
    DoorLock::Commands::ClearUser::Type command_data;
    command_data.userIndex = user_index;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback,
                          chip::MakeOptional(timed_invoke_timeout_ms));
    return ESP_OK;
}

esp_err_t send_set_credential(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t operation_type,
                              credential_struct credential, uint8_t *credential_data, size_t credential_len,
                              uint16_t user_index, uint8_t user_status, uint8_t user_type,
                              set_credential_callback success_cb, uint16_t timed_invoke_timeout_ms)
{
    DoorLock::Commands::SetCredential::Type command_data;
    command_data.operationType = DoorLock::DataOperationTypeEnum(operation_type);
    command_data.credential = credential;
    command_data.credentialData = chip::ByteSpan(credential_data, credential_len);
    command_data.userIndex = chip::app::DataModel::MakeNullable(user_index);
    command_data.userStatus = chip::app::DataModel::MakeNullable(DoorLock::UserStatusEnum(user_status));
    command_data.userType = chip::app::DataModel::MakeNullable(DoorLock::UserTypeEnum(user_type));

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, success_cb, send_command_failure_callback,
                          chip::MakeOptional(timed_invoke_timeout_ms));
    return ESP_OK;
}

esp_err_t send_get_credential_status(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                     credential_struct &credential, get_credential_status_callback success_cb)
{
    DoorLock::Commands::GetCredentialStatus::Type command_data;
    command_data.credential = credential;

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, success_cb, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_clear_credential(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                credential_struct &credential, uint16_t timed_invoke_timeout_ms)
{
    DoorLock::Commands::ClearCredential::Type command_data;
    command_data.credential = chip::app::DataModel::MakeNullable(credential);

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback,
                          chip::MakeOptional(timed_invoke_timeout_ms));
    return ESP_OK;
}

esp_err_t send_unbolt_door(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint8_t *pin_code,
                           size_t pin_code_len, uint16_t timed_invoke_timeout_ms)
{
    DoorLock::Commands::UnboltDoor::Type command_data;
    if (pin_code) {
        command_data.PINCode = chip::MakeOptional(chip::ByteSpan(pin_code, pin_code_len));
    }

    chip::Controller::DoorLockCluster cluster(*remote_device->GetExchangeManager(),
                                              remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback,
                          chip::MakeOptional(timed_invoke_timeout_ms));
    return ESP_OK;
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace command
} // namespace door_lock

namespace window_covering {
namespace command {

esp_err_t send_up_or_open(peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    WindowCovering::Commands::UpOrOpen::Type command_data;

    chip::Controller::WindowCoveringCluster cluster(*remote_device->GetExchangeManager(),
                                                    remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_down_or_close(peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    WindowCovering::Commands::DownOrClose::Type command_data;

    chip::Controller::WindowCoveringCluster cluster(*remote_device->GetExchangeManager(),
                                                    remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_stop_motion(peer_device_t *remote_device, uint16_t remote_endpoint_id)
{
    WindowCovering::Commands::StopMotion::Type command_data;

    chip::Controller::WindowCoveringCluster cluster(*remote_device->GetExchangeManager(),
                                                    remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_go_to_lift_value(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t lift_value)
{
    WindowCovering::Commands::GoToLiftValue::Type command_data;
    command_data.liftValue = lift_value;

    chip::Controller::WindowCoveringCluster cluster(*remote_device->GetExchangeManager(),
                                                    remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_go_to_lift_percentage(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                     uint16_t lift_percent100ths_value)
{
    WindowCovering::Commands::GoToLiftPercentage::Type command_data;
    command_data.liftPercent100thsValue = static_cast<chip::Percent100ths>(lift_percent100ths_value);

    chip::Controller::WindowCoveringCluster cluster(*remote_device->GetExchangeManager(),
                                                    remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_go_to_tilt_value(peer_device_t *remote_device, uint16_t remote_endpoint_id, uint16_t tilt_value)
{
    WindowCovering::Commands::GoToTiltValue::Type command_data;
    command_data.tiltValue = tilt_value;

    chip::Controller::WindowCoveringCluster cluster(*remote_device->GetExchangeManager(),
                                                    remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

esp_err_t send_go_to_tilt_percentage(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                     uint16_t tilt_percent100ths_value)
{
    WindowCovering::Commands::GoToTiltPercentage::Type command_data;
    command_data.tiltPercent100thsValue = static_cast<chip::Percent100ths>(tilt_percent100ths_value);

    chip::Controller::WindowCoveringCluster cluster(*remote_device->GetExchangeManager(),
                                                    remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback);
    return ESP_OK;
}

} // namespace command
} // namespace window_covering

namespace administrator_commissioning {
namespace command {

esp_err_t send_open_commissioning_window(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                         uint16_t commissioning_timeout, chip::MutableByteSpan &pake_passcode_verifier,
                                         uint16_t discriminator, uint32_t iterations, chip::MutableByteSpan &salt,
                                         uint16_t timed_interaction_timeout_ms)
{
    AdministratorCommissioning::Commands::OpenCommissioningWindow::Type command_data;
    command_data.commissioningTimeout = commissioning_timeout;
    command_data.PAKEPasscodeVerifier = pake_passcode_verifier;
    command_data.discriminator = discriminator;
    command_data.iterations = iterations;
    command_data.salt = salt;

    chip::Controller::AdministratorCommissioningCluster cluster(
        *remote_device->GetExchangeManager(), remote_device->GetSecureSession().Value(), remote_endpoint_id);
    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback,
                          timed_interaction_timeout_ms);
    return ESP_OK;
}

esp_err_t send_open_basic_commissioning_window(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                               uint16_t commissioning_timeout, uint16_t timed_interaction_timeout_ms)
{
    AdministratorCommissioning::Commands::OpenBasicCommissioningWindow::Type command_data;
    command_data.commissioningTimeout = commissioning_timeout;

    chip::Controller::AdministratorCommissioningCluster cluster(
        *remote_device->GetExchangeManager(), remote_device->GetSecureSession().Value(), remote_endpoint_id);

    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback,
                          timed_interaction_timeout_ms);
    return ESP_OK;
}

esp_err_t send_revoke_commissioning(peer_device_t *remote_device, uint16_t remote_endpoint_id,
                                    uint16_t timed_interaction_timeout_ms)
{
    AdministratorCommissioning::Commands::RevokeCommissioning::Type command_data;

    chip::Controller::AdministratorCommissioningCluster cluster(
        *remote_device->GetExchangeManager(), remote_device->GetSecureSession().Value(), remote_endpoint_id);

    cluster.InvokeCommand(command_data, NULL, send_command_success_callback, send_command_failure_callback,
                          timed_interaction_timeout_ms);
    return ESP_OK;
}

} // namespace command
} // namespace administrator_commissioning

#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

} // namespace cluster
} // namespace esp_matter
