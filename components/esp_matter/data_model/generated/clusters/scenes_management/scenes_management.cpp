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
#include <scenes_management.h>
#include <scenes_management_ids.h>
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
namespace scenes_management {

namespace feature {
namespace scene_names {
uint32_t get_id()
{
    return SceneNames::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* scene_names */

} /* feature */

namespace attribute {
attribute_t *create_scene_table_size(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SceneTableSize::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_fabric_scene_info(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, FabricSceneInfo::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_add_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_add_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddSceneResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_view_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ViewScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_view_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ViewSceneResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_remove_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveSceneResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_remove_all_scenes(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveAllScenes::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_all_scenes_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveAllScenesResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_store_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StoreScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_store_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StoreSceneResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_recall_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RecallScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_scene_membership(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetSceneMembership::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_scene_membership_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetSceneMembershipResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_copy_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CopyScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_copy_scene_response(cluster_t *cluster)
{
    VerifyOrReturnValue(has_command(CopyScene, COMMAND_FLAG_ACCEPTED), NULL);
    return esp_matter::command::create(cluster, CopySceneResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, scenes_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, scenes_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterScenesManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_scene_table_size(cluster, config->scene_table_size);
        attribute::create_fabric_scene_info(cluster, NULL, 0, 0);
        command::create_add_scene(cluster);
        command::create_add_scene_response(cluster);
        command::create_view_scene(cluster);
        command::create_view_scene_response(cluster);
        command::create_remove_scene(cluster);
        command::create_remove_scene_response(cluster);
        command::create_remove_all_scenes(cluster);
        command::create_remove_all_scenes_response(cluster);
        command::create_store_scene(cluster);
        command::create_store_scene_response(cluster);
        command::create_recall_scene(cluster);
        command::create_get_scene_membership(cluster);
        command::create_get_scene_membership_response(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterScenesManagementClusterServerInitCallback,
                                                 ESPMatterScenesManagementClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* scenes_management */
} /* cluster */
} /* esp_matter */
