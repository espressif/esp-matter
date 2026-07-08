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
#include <administrator_commissioning.h>
#include <administrator_commissioning_ids.h>
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
namespace administrator_commissioning {

namespace feature {
namespace basic {
uint32_t get_id()
{
    return Basic::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_open_basic_commissioning_window(cluster);

    return ESP_OK;
}
} /* basic */

} /* feature */

namespace attribute {
attribute_t *create_window_status(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, WindowStatus::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_admin_fabric_index(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AdminFabricIndex::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_admin_vendor_id(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AdminVendorId::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_open_commissioning_window(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OpenCommissioningWindow::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_open_basic_commissioning_window(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(basic), NULL);
    return esp_matter::command::create(cluster, OpenBasicCommissioningWindow::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_revoke_commissioning(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RevokeCommissioning::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, administrator_commissioning::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, administrator_commissioning::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterAdministratorCommissioningPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_window_status(cluster, config->window_status);
        attribute::create_admin_fabric_index(cluster, config->admin_fabric_index);
        attribute::create_admin_vendor_id(cluster, config->admin_vendor_id);
        command::create_open_commissioning_window(cluster);
        command::create_revoke_commissioning(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterAdministratorCommissioningClusterServerInitCallback,
                                                 ESPMatterAdministratorCommissioningClusterServerShutdownCallback);
    }

    return cluster;
}

} /* administrator_commissioning */
} /* cluster */
} /* esp_matter */
