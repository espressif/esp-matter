/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "app_binding_handler.h"

#include "app-common/zap-generated/ids/Clusters.h"
#include "app-common/zap-generated/ids/Commands.h"
#include "app/CommandSender.h"
#include "app/clusters/bindings/BindingManager.h"
#include "app/server/Server.h"
#include "controller/InvokeInteraction.h"
#include "lib/core/CHIPError.h"
#include <esp_matter_console.h>

#if CONFIG_ENABLE_CHIP_SHELL
#include "lib/shell/Engine.h"

using chip::Shell::Engine;
using chip::Shell::shell_command_t;
using chip::Shell::streamer_get;
using chip::Shell::streamer_printf;
#endif 

static bool sSwitchOnOffState = false;
#if CONFIG_ENABLE_CHIP_SHELL
static void toggle_switch(bool newState)
{
    sSwitchOnOffState = newState;
    chip::BindingManager::GetInstance().NotifyBoundClusterChanged(/*endpoint-id*/1, chip::app::Clusters::OnOff::Id, nullptr);
}

static esp_err_t app_switch_command_handler(int argc, char ** argv)
{
    if (argc == 1 && strcmp(argv[0], "on") == 0)
    {
        toggle_switch(true);
        return ESP_OK;
    }
    if (argc == 1 && strcmp(argv[0], "off") == 0)
    {
        toggle_switch(false);
        return ESP_OK;
    }
    if (argc == 1 && strcmp(argv[0], "toggle") == 0)
    {
        toggle_switch(!sSwitchOnOffState);
        return ESP_OK;
    }
    streamer_printf(streamer_get(), "Usage: switch [on|off|toggle]");
    return ESP_OK;
}

static void app_register_switch_commands()
{   
    esp_matter_console_command_t command = {
        .name = "switch",
        .description = "Switch commands. Usage: matter esp switch [on|off|toggle] ",
        .handler = app_switch_command_handler,
    };
    esp_matter_console_add_command(&command);
}

#endif // CONFIG_ENABLE_CHIP_SHELL

static void BoundDeviceChangedHandler(const EmberBindingTableEntry * binding, chip::DeviceProxy * peer_device, void * context)
{
    using namespace chip;
    using namespace chip::app;

    if (binding->type == EMBER_MULTICAST_BINDING)
    {
        ChipLogError(NotSpecified, "Group binding is not supported now");
        return;
    }

    if (binding->type == EMBER_UNICAST_BINDING && binding->local == 1 && binding->clusterId == Clusters::OnOff::Id)
    {
        auto onSuccess = [](const ConcreteCommandPath & commandPath, const StatusIB & status, const auto & dataResponse) {
            ChipLogProgress(NotSpecified, "OnOff command succeeds");
        };
        auto onFailure = [](CHIP_ERROR error) {
            ChipLogError(NotSpecified, "OnOff command failed: %" CHIP_ERROR_FORMAT, error.Format());
        };

        if (sSwitchOnOffState)
        {
            Clusters::OnOff::Commands::On::Type onCommand;
            Controller::InvokeCommandRequest(peer_device->GetExchangeManager(), peer_device->GetSecureSession().Value(),
                                             binding->remote, onCommand, onSuccess, onFailure);
        }
        else
        {
            Clusters::OnOff::Commands::Off::Type offCommand;
            Controller::InvokeCommandRequest(peer_device->GetExchangeManager(), peer_device->GetSecureSession().Value(),
                                             binding->remote, offCommand, onSuccess, onFailure);
        }
    }
}

esp_err_t app_binding_handler_init()
{
    chip::BindingManager::GetInstance().SetAppServer(&chip::Server::GetInstance());
    chip::BindingManager::GetInstance().RegisterBoundDeviceChangedHandler(BoundDeviceChangedHandler);
#if CONFIG_ENABLE_CHIP_SHELL
    app_register_switch_commands();
#endif
    return ESP_OK;
}
