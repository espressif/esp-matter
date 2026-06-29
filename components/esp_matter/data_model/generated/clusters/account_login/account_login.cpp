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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#include <esp_log.h>
#include <esp_matter_core.h>
#include <esp_matter.h>

#include <app-common/zap-generated/cluster-enums.h>
#include <app-common/zap-generated/callback.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <esp_matter_delegate_callbacks.h>
#include <account_login.h>
#include <account_login_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 2;

static esp_err_t esp_matter_command_callback_get_setup_pin(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
{
    chip::app::Clusters::AccountLogin::Commands::GetSetupPIN::DecodableType command_data;
    chip::app::CommandHandler *command_obj = (chip::app::CommandHandler *)opaque_ptr;
    CHIP_ERROR error = command_data.Decode(tlv_data, command_obj->GetAccessingFabricIndex());
    if (error == CHIP_NO_ERROR) {
        emberAfAccountLoginClusterGetSetupPINCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_login(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                   void *opaque_ptr)
{
    chip::app::Clusters::AccountLogin::Commands::Login::DecodableType command_data;
    chip::app::CommandHandler *command_obj = (chip::app::CommandHandler *)opaque_ptr;
    CHIP_ERROR error = command_data.Decode(tlv_data, command_obj->GetAccessingFabricIndex());
    if (error == CHIP_NO_ERROR) {
        emberAfAccountLoginClusterLoginCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_logout(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                    void *opaque_ptr)
{
    chip::app::Clusters::AccountLogin::Commands::Logout::DecodableType command_data;
    chip::app::CommandHandler *command_obj = (chip::app::CommandHandler *)opaque_ptr;
    CHIP_ERROR error = command_data.Decode(tlv_data, command_obj->GetAccessingFabricIndex());
    if (error == CHIP_NO_ERROR) {
        emberAfAccountLoginClusterLogoutCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace account_login {

namespace command {
command_t *create_get_setup_pin(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetSetupPIN::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_get_setup_pin);
}

command_t *create_get_setup_pin_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetSetupPINResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_login(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Login::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_login);
}

command_t *create_logout(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Logout::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_logout);
}

} /* command */

namespace event {
event_t *create_logged_out(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, LoggedOut::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, account_login::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, account_login::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = AccountLoginDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterAccountLoginPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        command::create_get_setup_pin(cluster);
        command::create_get_setup_pin_response(cluster);
        command::create_login(cluster);
        command::create_logout(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* account_login */
} /* cluster */
} /* esp_matter */
