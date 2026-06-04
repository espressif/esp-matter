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
#include <pressure_measurement.h>
#include <pressure_measurement_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "pressure_measurement_cluster";
constexpr uint16_t cluster_revision = 5;

namespace esp_matter {
namespace cluster {
namespace pressure_measurement {

namespace feature {
namespace extended {
uint32_t get_id()
{
    return Extended::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_scaled_value(cluster, config->scaled_value);
    attribute::create_min_scaled_value(cluster, config->min_scaled_value);
    attribute::create_max_scaled_value(cluster, config->max_scaled_value);
    attribute::create_scale(cluster, config->scale);

    return ESP_OK;
}
} /* extended */

} /* feature */

namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinMeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<int16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxMeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32767)));
    return attribute;
}

attribute_t *create_tolerance(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Tolerance::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(2048)));
    return attribute;
}

attribute_t *create_scaled_value(cluster_t *cluster, nullable<int16_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(extended), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ScaledValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_min_scaled_value(cluster_t *cluster, nullable<int16_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(extended), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinScaledValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32766)));
    return attribute;
}

attribute_t *create_max_scaled_value(cluster_t *cluster, nullable<int16_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(extended), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxScaledValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int16_t>(-32768)), esp_matter_attr_val(nullable<int16_t>(32767)));
    return attribute;
}

attribute_t *create_scaled_tolerance(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ScaledTolerance::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(2048)));
    return attribute;
}

attribute_t *create_scale(cluster_t *cluster, int8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(extended), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, Scale::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int8_t>(-127)), esp_matter_attr_val(static_cast<int8_t>(126)));
    return attribute;
}

} /* attribute */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, pressure_measurement::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, pressure_measurement::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterPressureMeasurementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_measured_value(cluster, config->measured_value);
        attribute::create_min_measured_value(cluster, config->min_measured_value);
        attribute::create_max_measured_value(cluster, config->max_measured_value);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterPressureMeasurementClusterServerInitCallback,
                                                 ESPMatterPressureMeasurementClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* pressure_measurement */
} /* cluster */
} /* esp_matter */
