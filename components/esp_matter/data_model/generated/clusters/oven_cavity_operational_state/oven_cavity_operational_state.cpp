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
#include <oven_cavity_operational_state.h>
#include <oven_cavity_operational_state_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "oven_cavity_operational_state_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace oven_cavity_operational_state {

namespace attribute {
attribute_t *create_phase_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, PhaseList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_phase(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, CurrentPhase::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_countdown_time(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, CountdownTime::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_operational_state_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, OperationalStateList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_operational_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, OperationalState::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
}

attribute_t *create_operational_error(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, OperationalError::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_stop(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Stop::Id, COMMAND_FLAG_NONE, NULL);
}

command_t *create_start(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Start::Id, COMMAND_FLAG_NONE, NULL);
}

command_t *create_operational_command_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCommandResponse::Id, COMMAND_FLAG_NONE, NULL);
}

} /* command */

namespace event {
event_t *create_operational_error(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, OperationalError::Id);
}

event_t *create_operation_completion(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, OperationCompletion::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, oven_cavity_operational_state::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, oven_cavity_operational_state::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = OvenCavityOperationalStateDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterOvenCavityOperationalStatePluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_phase_list(cluster, NULL, 0, 0);
        attribute::create_current_phase(cluster, 0);
        attribute::create_operational_state_list(cluster, NULL, 0, 0);
        attribute::create_operational_state(cluster, 0);
        attribute::create_operational_error(cluster, NULL, 0, 0);
        /* Events */
        event::create_operational_error(cluster);
    }

    return cluster;
}

} /* oven_cavity_operational_state */
} /* cluster */
} /* esp_matter */
