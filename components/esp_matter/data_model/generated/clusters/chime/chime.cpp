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
#include <chime.h>
#include <chime_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "chime_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace chime {

namespace attribute {
attribute_t *create_installed_chime_sounds(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, InstalledChimeSounds::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_selected_chime(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SelectedChime::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, Enabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

} /* attribute */
namespace command {
command_t *create_play_chime_sound(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, PlayChimeSound::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_chime_started_playing(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ChimeStartedPlaying::Id);
}

} /* event */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, chime::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, chime::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ChimeDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterChimePluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_selected_chime(cluster, config->selected_chime);
        attribute::create_enabled(cluster, config->enabled);
        attribute::create_installed_chime_sounds(cluster, NULL, 0, 0);
        command::create_play_chime_sound(cluster);
        /* Events */
        event::create_chime_started_playing(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterChimeClusterServerInitCallback,
                                                 ESPMatterChimeClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* chime */
} /* cluster */
} /* esp_matter */
