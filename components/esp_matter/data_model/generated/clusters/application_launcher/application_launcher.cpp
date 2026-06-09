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
#include <app/InteractionModelEngine.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <esp_matter_delegate_callbacks.h>
#include <application_launcher.h>
#include <application_launcher_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "application_launcher_cluster";
constexpr uint16_t cluster_revision = 2;

static esp_err_t esp_matter_command_callback_launch_app(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::ApplicationLauncher::Commands::LaunchApp::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfApplicationLauncherClusterLaunchAppCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stop_app(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                      void *opaque_ptr)
{
    chip::app::Clusters::ApplicationLauncher::Commands::StopApp::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfApplicationLauncherClusterStopAppCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_hide_app(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                      void *opaque_ptr)
{
    chip::app::Clusters::ApplicationLauncher::Commands::HideApp::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfApplicationLauncherClusterHideAppCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace application_launcher {

namespace feature {
namespace application_platform {
uint32_t get_id()
{
    return ApplicationPlatform::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_catalog_list(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* application_platform */

} /* feature */

namespace attribute {
attribute_t *create_catalog_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(application_platform), NULL);
    return esp_matter::attribute::create(cluster, CatalogList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_app(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CurrentApp::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_launch_app(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LaunchApp::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_launch_app);
}

command_t *create_stop_app(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StopApp::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_stop_app);
}

command_t *create_hide_app(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, HideApp::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_hide_app);
}

command_t *create_launcher_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LauncherResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, application_launcher::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, application_launcher::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ApplicationLauncherDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterApplicationLauncherPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        command::create_launch_app(cluster);
        command::create_stop_app(cluster);
        command::create_hide_app(cluster);
        command::create_launcher_response(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* application_launcher */
} /* cluster */
} /* esp_matter */
