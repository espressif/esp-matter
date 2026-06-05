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
#include <total_volatile_organic_compounds_concentration_measurement.h>
#include <total_volatile_organic_compounds_concentration_measurement_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "total_volatile_organic_compounds_concentration_measurement_cluster";
constexpr uint16_t cluster_revision = 4;

namespace esp_matter {
namespace cluster {
namespace total_volatile_organic_compounds_concentration_measurement {

namespace feature {
namespace numeric_measurement {
uint32_t get_id()
{
    return NumericMeasurement::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_measured_value(cluster, 0);
    attribute::create_min_measured_value(cluster, 0);
    attribute::create_max_measured_value(cluster, 0);
    attribute::create_measurement_unit(cluster, 0);

    return ESP_OK;
}
} /* numeric_measurement */

namespace level_indication {
uint32_t get_id()
{
    return LevelIndication::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_level_value(cluster, 0);

    return ESP_OK;
}
} /* level_indication */

namespace medium_level {
uint32_t get_id()
{
    return MediumLevel::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(level_indication), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* medium_level */

namespace critical_level {
uint32_t get_id()
{
    return CriticalLevel::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(level_indication), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* critical_level */

namespace peak_measurement {
uint32_t get_id()
{
    return PeakMeasurement::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(numeric_measurement), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_peak_measured_value(cluster, 0);
    attribute::create_peak_measured_value_window(cluster, 0);

    return ESP_OK;
}
} /* peak_measurement */

namespace average_measurement {
uint32_t get_id()
{
    return AverageMeasurement::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(numeric_measurement), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_average_measured_value(cluster, 0);
    attribute::create_average_measured_value_window(cluster, 0);

    return ESP_OK;
}
} /* average_measurement */

} /* feature */

namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<float> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(numeric_measurement), NULL);
    return esp_matter::attribute::create(cluster, MeasuredValue::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<float> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(numeric_measurement), NULL);
    return esp_matter::attribute::create(cluster, MinMeasuredValue::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<float> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(numeric_measurement), NULL);
    return esp_matter::attribute::create(cluster, MaxMeasuredValue::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_peak_measured_value(cluster_t *cluster, nullable<float> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(peak_measurement), NULL);
    return esp_matter::attribute::create(cluster, PeakMeasuredValue::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_peak_measured_value_window(cluster_t *cluster, uint32_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(peak_measurement), NULL);
    return esp_matter::attribute::create(cluster, PeakMeasuredValueWindow::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

attribute_t *create_average_measured_value(cluster_t *cluster, nullable<float> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(average_measurement), NULL);
    return esp_matter::attribute::create(cluster, AverageMeasuredValue::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_average_measured_value_window(cluster_t *cluster, uint32_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(average_measurement), NULL);
    return esp_matter::attribute::create(cluster, AverageMeasuredValueWindow::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

attribute_t *create_uncertainty(cluster_t *cluster, float value)
{
    return esp_matter::attribute::create(cluster, Uncertainty::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

attribute_t *create_measurement_unit(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(numeric_measurement), NULL);
    return esp_matter::attribute::create(cluster, MeasurementUnit::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
}

attribute_t *create_measurement_medium(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, MeasurementMedium::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
}

attribute_t *create_level_value(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(level_indication), NULL);
    return esp_matter::attribute::create(cluster, LevelValue::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
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
    cluster_t *cluster = esp_matter::cluster::create(endpoint, total_volatile_organic_compounds_concentration_measurement::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, total_volatile_organic_compounds_concentration_measurement::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterTotalVolatileOrganicCompoundsConcentrationMeasurementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_measurement_medium(cluster, 0);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("NumericMeasurement,LevelIndication",
                                       feature::numeric_measurement::get_id(), feature::level_indication::get_id());
        if (feature_map & feature::numeric_measurement::get_id()) {
            VerifyOrReturnValue(feature::numeric_measurement::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::level_indication::get_id()) {
            VerifyOrReturnValue(feature::level_indication::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::medium_level::get_id()) {
            VerifyOrReturnValue(feature::medium_level::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::critical_level::get_id()) {
            VerifyOrReturnValue(feature::critical_level::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::peak_measurement::get_id()) {
            VerifyOrReturnValue(feature::peak_measurement::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::average_measurement::get_id()) {
            VerifyOrReturnValue(feature::average_measurement::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* total_volatile_organic_compounds_concentration_measurement */
} /* cluster */
} /* esp_matter */
