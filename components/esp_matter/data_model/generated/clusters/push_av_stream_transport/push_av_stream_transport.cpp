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
#include <push_av_stream_transport.h>
#include <push_av_stream_transport_ids.h>
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

static const char *TAG = "push_av_stream_transport_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace push_av_stream_transport {

namespace feature {
namespace per_zone_sensitivity {
uint32_t get_id()
{
    return PerZoneSensitivity::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* per_zone_sensitivity */

} /* feature */

namespace attribute {
attribute_t *create_supported_formats(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, SupportedFormats::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_connections(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CurrentConnections::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_allocate_push_transport(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AllocatePushTransport::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_allocate_push_transport_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AllocatePushTransportResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_deallocate_push_transport(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DeallocatePushTransport::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_modify_push_transport(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ModifyPushTransport::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_transport_status(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetTransportStatus::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_manually_trigger_transport(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ManuallyTriggerTransport::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_find_transport(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, FindTransport::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_find_transport_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, FindTransportResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

namespace event {
event_t *create_push_transport_begin(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PushTransportBegin::Id);
}

event_t *create_push_transport_end(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PushTransportEnd::Id);
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
    cluster_t *cluster = esp_matter::cluster::create(endpoint, push_av_stream_transport::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, push_av_stream_transport::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = PushAvStreamTransportDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterPushAvStreamTransportPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_supported_formats(cluster, NULL, 0, 0);
        attribute::create_current_connections(cluster, NULL, 0, 0);
        command::create_allocate_push_transport(cluster);
        command::create_allocate_push_transport_response(cluster);
        command::create_deallocate_push_transport(cluster);
        command::create_modify_push_transport(cluster);
        command::create_set_transport_status(cluster);
        command::create_manually_trigger_transport(cluster);
        command::create_find_transport(cluster);
        command::create_find_transport_response(cluster);
        /* Events */
        event::create_push_transport_begin(cluster);
        event::create_push_transport_end(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterPushAvStreamTransportClusterServerInitCallback,
                                                 ESPMatterPushAvStreamTransportClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* push_av_stream_transport */
} /* cluster */
} /* esp_matter */
