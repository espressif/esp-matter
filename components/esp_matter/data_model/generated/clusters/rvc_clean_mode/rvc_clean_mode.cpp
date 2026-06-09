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
#include <app/clusters/mode-base-server/mode-base-cluster-objects.h>
#include <esp_matter_delegate_callbacks.h>
#include <rvc_clean_mode.h>
#include <rvc_clean_mode_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "rvc_clean_mode_cluster";
constexpr uint16_t cluster_revision = 5;

namespace esp_matter {
namespace cluster {
namespace rvc_clean_mode {

namespace feature {
namespace direct_mode_change {
uint32_t get_id()
{
    return DirectModeChange::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* direct_mode_change */

} /* feature */

namespace attribute {
attribute_t *create_supported_modes(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, SupportedModes::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentMode::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_change_to_mode(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ChangeToMode::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_change_to_mode_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ChangeToModeResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, rvc_clean_mode::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, rvc_clean_mode::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = RvcCleanModeDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterRvcCleanModePluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        command::create_change_to_mode(cluster);
        command::create_change_to_mode_response(cluster);
    }

    return cluster;
}

} /* rvc_clean_mode */
} /* cluster */
} /* esp_matter */
