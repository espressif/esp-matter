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
#include <ota_software_update_requestor.h>
#include <ota_software_update_requestor_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "ota_software_update_requestor_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace ota_software_update_requestor {

namespace attribute {
attribute_t *create_default_ota_providers(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, DefaultOTAProviders::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_update_possible(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, UpdatePossible::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_update_state(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, UpdateState::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(8), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_update_state_progress(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, UpdateStateProgress::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(100)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_announce_ota_provider(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AnnounceOTAProvider::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_state_transition(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, StateTransition::Id);
}

event_t *create_version_applied(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, VersionApplied::Id);
}

event_t *create_download_error(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DownloadError::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, ota_software_update_requestor::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ota_software_update_requestor::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterOtaSoftwareUpdateRequestorPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_update_possible(cluster, config->update_possible);
        attribute::create_update_state(cluster, config->update_state);
        attribute::create_update_state_progress(cluster, config->update_state_progress);
        attribute::create_default_ota_providers(cluster, NULL, 0, 0);
        /* Events */
        event::create_state_transition(cluster);
        event::create_version_applied(cluster);
        event::create_download_error(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterOtaSoftwareUpdateRequestorClusterServerInitCallback,
                                                 ESPMatterOtaSoftwareUpdateRequestorClusterServerShutdownCallback);
    }

    return cluster;
}

} /* ota_software_update_requestor */
} /* cluster */
} /* esp_matter */
