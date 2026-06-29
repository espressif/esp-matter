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
#include <electrical_energy_measurement.h>
#include <electrical_energy_measurement_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace electrical_energy_measurement {

namespace feature {
namespace imported_energy {
uint32_t get_id()
{
    return ImportedEnergy::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_cumulative_energy_imported(cluster, NULL, 0, 0);
    attribute::create_periodic_energy_imported(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* imported_energy */

namespace exported_energy {
uint32_t get_id()
{
    return ExportedEnergy::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_cumulative_energy_exported(cluster, NULL, 0, 0);
    attribute::create_periodic_energy_exported(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* exported_energy */

namespace cumulative_energy {
uint32_t get_id()
{
    return CumulativeEnergy::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_cumulative_energy_imported(cluster, NULL, 0, 0);
    attribute::create_cumulative_energy_exported(cluster, NULL, 0, 0);
    event::create_cumulative_energy_measured(cluster);

    return ESP_OK;
}
} /* cumulative_energy */

namespace periodic_energy {
uint32_t get_id()
{
    return PeriodicEnergy::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_periodic_energy_imported(cluster, NULL, 0, 0);
    attribute::create_periodic_energy_exported(cluster, NULL, 0, 0);
    event::create_periodic_energy_measured(cluster);

    return ESP_OK;
}
} /* periodic_energy */

} /* feature */

namespace attribute {
attribute_t *create_accuracy(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Accuracy::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_cumulative_energy_imported(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(imported_energy)) && (has_feature(cumulative_energy))), NULL);
    return esp_matter::attribute::create(cluster, CumulativeEnergyImported::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_cumulative_energy_exported(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(exported_energy)) && (has_feature(cumulative_energy))), NULL);
    return esp_matter::attribute::create(cluster, CumulativeEnergyExported::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_periodic_energy_imported(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(imported_energy)) && (has_feature(periodic_energy))), NULL);
    return esp_matter::attribute::create(cluster, PeriodicEnergyImported::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_periodic_energy_exported(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(exported_energy)) && (has_feature(periodic_energy))), NULL);
    return esp_matter::attribute::create(cluster, PeriodicEnergyExported::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_cumulative_energy_reset(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CumulativeEnergyReset::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

} /* attribute */

namespace event {
event_t *create_cumulative_energy_measured(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(cumulative_energy), NULL);
    return esp_matter::event::create(cluster, CumulativeEnergyMeasured::Id);
}

event_t *create_periodic_energy_measured(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(periodic_energy), NULL);
    return esp_matter::event::create(cluster, PeriodicEnergyMeasured::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, electrical_energy_measurement::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, electrical_energy_measurement::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ElectricalEnergyMeasurementDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterElectricalEnergyMeasurementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_accuracy(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("ImportedEnergy,ExportedEnergy",
                                       feature::imported_energy::get_id(), feature::exported_energy::get_id());
        if (feature_map & feature::imported_energy::get_id()) {
            VerifyOrReturnValue(feature::imported_energy::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::exported_energy::get_id()) {
            VerifyOrReturnValue(feature::exported_energy::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        VALIDATE_FEATURES_AT_LEAST_ONE("CumulativeEnergy,PeriodicEnergy",
                                       feature::cumulative_energy::get_id(), feature::periodic_energy::get_id());
        if (feature_map & feature::cumulative_energy::get_id()) {
            VerifyOrReturnValue(feature::cumulative_energy::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::periodic_energy::get_id()) {
            VerifyOrReturnValue(feature::periodic_energy::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterElectricalEnergyMeasurementClusterServerInitCallback,
                                                 ESPMatterElectricalEnergyMeasurementClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* electrical_energy_measurement */
} /* cluster */
} /* esp_matter */
