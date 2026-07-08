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
#include <webrtc_transport_provider.h>
#include <webrtc_transport_provider_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace webrtc_transport_provider {

namespace attribute {
attribute_t *create_current_sessions(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CurrentSessions::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_solicit_offer(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SolicitOffer::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_solicit_offer_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SolicitOfferResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_provide_offer(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProvideOffer::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_provide_offer_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProvideOfferResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_provide_answer(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProvideAnswer::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_provide_ice_candidates(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProvideICECandidates::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_end_session(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EndSession::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, webrtc_transport_provider::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, webrtc_transport_provider::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_current_sessions(cluster, NULL, 0, 0);
        command::create_solicit_offer(cluster);
        command::create_solicit_offer_response(cluster);
        command::create_provide_offer(cluster);
        command::create_provide_offer_response(cluster);
        command::create_provide_answer(cluster);
        command::create_provide_ice_candidates(cluster);
        command::create_end_session(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterWebRTCTransportProviderClusterServerInitCallback,
                                                 ESPMatterWebRTCTransportProviderClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* webrtc_transport_provider */
} /* cluster */
} /* esp_matter */
