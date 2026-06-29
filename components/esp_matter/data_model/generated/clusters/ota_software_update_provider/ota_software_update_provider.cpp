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
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <esp_matter_delegate_callbacks.h>
#include <ota_software_update_provider.h>
#include <ota_software_update_provider_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace ota_software_update_provider {

namespace command {
command_t *create_query_image(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, QueryImage::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_query_image_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, QueryImageResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_apply_update_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ApplyUpdateRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_apply_update_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ApplyUpdateResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_notify_update_applied(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, NotifyUpdateApplied::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, ota_software_update_provider::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ota_software_update_provider::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = OtaSoftwareUpdateProviderDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterOtaSoftwareUpdateProviderPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        command::create_query_image(cluster);
        command::create_query_image_response(cluster);
        command::create_apply_update_request(cluster);
        command::create_apply_update_response(cluster);
        command::create_notify_update_applied(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterOtaSoftwareUpdateProviderClusterServerInitCallback,
                                                 ESPMatterOtaSoftwareUpdateProviderClusterServerShutdownCallback);
    }

    return cluster;
}

} /* ota_software_update_provider */
} /* cluster */
} /* esp_matter */
