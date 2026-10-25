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
#include <media_file_management.h>
#include <media_file_management_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace media_file_management {

namespace feature {
namespace media_sharing {
uint32_t get_id()
{
    return MediaSharing::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_request_shared_files(cluster);
    command::create_get_shared_file(cluster);
    command::create_get_shared_file_response(cluster);
    command::create_offer_file(cluster);
    event::create_shared_files_added(cluster);

    return ESP_OK;
}
} /* media_sharing */

} /* feature */

namespace attribute {
attribute_t *create_total_storage(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, TotalStorage::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_available_storage(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, AvailableStorage::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_available_files(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, AvailableFiles::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_supported_mime_types(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, SupportedMimeTypes::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_add_file(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddFile::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_add_file_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddFileResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_delete_file(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DeleteFile::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_request_shared_files(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(media_sharing), NULL);
    return esp_matter::command::create(cluster, RequestSharedFiles::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_shared_file(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(media_sharing), NULL);
    return esp_matter::command::create(cluster, GetSharedFile::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_shared_file_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(media_sharing), NULL);
    return esp_matter::command::create(cluster, GetSharedFileResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_offer_file(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(media_sharing), NULL);
    return esp_matter::command::create(cluster, OfferFile::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_shared_files_added(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(media_sharing), NULL);
    return esp_matter::event::create(cluster, SharedFilesAdded::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, media_file_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, media_file_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterMediaFileManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_total_storage(cluster, config->total_storage);
        attribute::create_available_storage(cluster, config->available_storage);
        attribute::create_available_files(cluster, NULL, 0, 0);
        attribute::create_supported_mime_types(cluster, NULL, 0, 0);
        command::create_add_file(cluster);
        command::create_add_file_response(cluster);
        command::create_delete_file(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterMediaFileManagementClusterServerInitCallback,
                                                 ESPMatterMediaFileManagementClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* media_file_management */
} /* cluster */
} /* esp_matter */
