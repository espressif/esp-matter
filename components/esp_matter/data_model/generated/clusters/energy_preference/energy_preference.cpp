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
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <esp_matter_delegate_callbacks.h>
#include <energy_preference.h>
#include <energy_preference_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace energy_preference {

namespace feature {
namespace energy_balance {
uint32_t get_id()
{
    return EnergyBalance::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_current_energy_balance(cluster, config->current_energy_balance);
    attribute::create_energy_balances(cluster, NULL, 0, 0);
    attribute::create_energy_priorities(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* energy_balance */

namespace low_power_mode_sensitivity {
uint32_t get_id()
{
    return LowPowerModeSensitivity::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_current_low_power_mode_sensitivity(cluster, config->current_low_power_mode_sensitivity);
    attribute::create_low_power_mode_sensitivities(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* low_power_mode_sensitivity */

} /* feature */

namespace attribute {
attribute_t *create_energy_balances(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(energy_balance), NULL);
    return esp_matter::attribute::create(cluster, EnergyBalances::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_energy_balance(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(energy_balance), NULL);
    return esp_matter::attribute::create(cluster, CurrentEnergyBalance::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_energy_priorities(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(energy_balance), NULL);
    return esp_matter::attribute::create(cluster, EnergyPriorities::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_low_power_mode_sensitivities(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(low_power_mode_sensitivity), NULL);
    return esp_matter::attribute::create(cluster, LowPowerModeSensitivities::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_low_power_mode_sensitivity(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(low_power_mode_sensitivity), NULL);
    return esp_matter::attribute::create(cluster, CurrentLowPowerModeSensitivity::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

} /* attribute */

const function_generic_t function_list[] = {
    (function_generic_t)MatterEnergyPreferenceClusterServerPreAttributeChangedCallback,
};

const int function_flags = CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, energy_preference::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, energy_preference::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = EnergyPreferenceDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterEnergyPreferencePluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("EnergyBalance,LowPowerModeSensitivity",
                                       feature::energy_balance::get_id(), feature::low_power_mode_sensitivity::get_id());
        if (feature_map & feature::energy_balance::get_id()) {
            VerifyOrReturnValue(feature::energy_balance::add(cluster, &(config->features.energy_balance)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::low_power_mode_sensitivity::get_id()) {
            VerifyOrReturnValue(feature::low_power_mode_sensitivity::add(cluster, &(config->features.low_power_mode_sensitivity)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* energy_preference */
} /* cluster */
} /* esp_matter */
