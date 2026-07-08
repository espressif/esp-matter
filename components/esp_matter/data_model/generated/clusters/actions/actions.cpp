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
#include <actions.h>
#include <actions_ids.h>
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
namespace actions {

namespace attribute {
attribute_t *create_action_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActionList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_endpoint_lists(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, EndpointLists::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_setup_url(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_setup_url_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound. cluster_id: 0x%08" PRIX32, actions::Id));
    return esp_matter::attribute::create(cluster, SetupURL::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_setup_url_length + 1);
}

} /* attribute */
namespace command {
command_t *create_instant_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, InstantAction::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_instant_action_with_transition(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, InstantActionWithTransition::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_start_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StartAction::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_start_action_with_duration(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StartActionWithDuration::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_stop_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StopAction::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_pause_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, PauseAction::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_pause_action_with_duration(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, PauseActionWithDuration::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_resume_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ResumeAction::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_enable_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnableAction::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_enable_action_with_duration(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnableActionWithDuration::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_disable_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DisableAction::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_disable_action_with_duration(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DisableActionWithDuration::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_state_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, StateChanged::Id);
}

event_t *create_action_failed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ActionFailed::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, actions::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, actions::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ActionsDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterActionsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_action_list(cluster, NULL, 0, 0);
        attribute::create_endpoint_lists(cluster, NULL, 0, 0);
        /* Events */
        event::create_state_changed(cluster);
        event::create_action_failed(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterActionsClusterServerInitCallback,
                                                 ESPMatterActionsClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* actions */
} /* cluster */
} /* esp_matter */
