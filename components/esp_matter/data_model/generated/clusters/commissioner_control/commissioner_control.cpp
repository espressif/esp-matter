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
#include <commissioner_control.h>
#include <commissioner_control_ids.h>
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
namespace commissioner_control {

namespace attribute {
attribute_t *create_supported_device_categories(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SupportedDeviceCategories::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(SupportedDeviceCategories::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(SupportedDeviceCategories::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_request_commissioning_approval(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RequestCommissioningApproval::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_commission_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CommissionNode::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_reverse_open_commissioning_window(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ReverseOpenCommissioningWindow::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

namespace event {
event_t *create_commissioning_request_result(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, CommissioningRequestResult::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, commissioner_control::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, commissioner_control::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = CommissionerControlDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterCommissionerControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_supported_device_categories(cluster, config->supported_device_categories);
        command::create_request_commissioning_approval(cluster);
        command::create_commission_node(cluster);
        command::create_reverse_open_commissioning_window(cluster);
        /* Events */
        event::create_commissioning_request_result(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterCommissionerControlClusterServerInitCallback,
                                                 ESPMatterCommissionerControlClusterServerShutdownCallback);
    }

    return cluster;
}

} /* commissioner_control */
} /* cluster */
} /* esp_matter */
