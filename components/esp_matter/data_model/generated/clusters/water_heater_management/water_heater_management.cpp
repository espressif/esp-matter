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
#include <water_heater_management.h>
#include <water_heater_management_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace water_heater_management {

namespace feature {
namespace energy_management {
uint32_t get_id()
{
    return EnergyManagement::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_tank_volume(cluster, 0);
    attribute::create_estimated_heat_required(cluster, 0);

    return ESP_OK;
}
} /* energy_management */

namespace tank_percent {
uint32_t get_id()
{
    return TankPercent::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_tank_percentage(cluster, 0);

    return ESP_OK;
}
} /* tank_percent */

} /* feature */

namespace attribute {
attribute_t *create_heater_types(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, HeaterTypes::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
}

attribute_t *create_heat_demand(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, HeatDemand::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
}

attribute_t *create_tank_volume(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(energy_management), NULL);
    return esp_matter::attribute::create(cluster, TankVolume::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

attribute_t *create_estimated_heat_required(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(energy_management), NULL);
    return esp_matter::attribute::create(cluster, EstimatedHeatRequired::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

attribute_t *create_tank_percentage(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(tank_percent), NULL);
    return esp_matter::attribute::create(cluster, TankPercentage::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

attribute_t *create_boost_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, BoostState::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
}

} /* attribute */
namespace command {
command_t *create_boost(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Boost::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_cancel_boost(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CancelBoost::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_boost_started(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BoostStarted::Id);
}

event_t *create_boost_ended(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BoostEnded::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, water_heater_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, water_heater_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = WaterHeaterManagementDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterWaterHeaterManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_heater_types(cluster, 0);
        attribute::create_heat_demand(cluster, 0);
        attribute::create_boost_state(cluster, 0);
        command::create_boost(cluster);
        command::create_cancel_boost(cluster);
        /* Events */
        event::create_boost_started(cluster);
        event::create_boost_ended(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* water_heater_management */
} /* cluster */
} /* esp_matter */
