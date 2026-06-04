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

#include "integration.h"
#include <esp_matter_data_model_priv.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/pressure-measurement-server/PressureMeasurementCluster.h>
#include <clusters/PressureMeasurement/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/logging/CHIPLogging.h>
#include <lib/support/CodeUtils.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::PressureMeasurement;
using namespace chip::app::Clusters::PressureMeasurement::Attributes;
using namespace esp_matter;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<PressureMeasurementCluster>> gServers;
} // namespace

namespace chip::app::Clusters::PressureMeasurement {

PressureMeasurementCluster * FindClusterOnEndpoint(EndpointId endpointId)
{
    auto it = gServers.find(endpointId);
    if (it == gServers.end() || !it->second.IsConstructed()) {
        return nullptr;
    }
    return &it->second.Cluster();
}

CHIP_ERROR SetMeasuredValue(EndpointId endpointId, DataModel::Nullable<int16_t> measuredValue)
{
    auto * cluster = FindClusterOnEndpoint(endpointId);
    VerifyOrReturnError(cluster != nullptr, CHIP_ERROR_NOT_FOUND);
    return cluster->SetMeasuredValue(measuredValue);
}

CHIP_ERROR SetScaledValue(EndpointId endpointId, DataModel::Nullable<int16_t> scaledValue)
{
    auto * cluster = FindClusterOnEndpoint(endpointId);
    VerifyOrReturnError(cluster != nullptr, CHIP_ERROR_NOT_FOUND);
    return cluster->SetScaledValue(scaledValue);
}

} // namespace chip::app::Clusters::PressureMeasurement

void ESPMatterPressureMeasurementClusterServerInitCallback(EndpointId endpointId)
{
    cluster_t *cluster = cluster::get(endpointId, PressureMeasurement::Id);
    VerifyOrReturn(cluster != nullptr,
                   ChipLogError(AppServer,
                                "PressureMeasurement: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));
    if (!gServers[endpointId].IsConstructed()) {
        PressureMeasurementCluster::Config config;
        esp_matter_attr_val_t val = esp_matter_invalid(nullptr);

        // MinMeasuredValue (nullable int16)
        attribute_t *attr = attribute::get(cluster, MinMeasuredValue::Id);
        if (attr && attribute::get_val_internal(attr, &val) == ESP_OK && val.type == ESP_MATTER_VAL_TYPE_NULLABLE_INT16) {
            if (val.val.i16 != INT16_MIN) {
                config.minMeasuredValue = DataModel::MakeNullable(static_cast<int16_t>(val.val.i16));
            }
        }

        // MaxMeasuredValue (nullable int16)
        val = esp_matter_invalid(nullptr);
        attr = attribute::get(cluster, MaxMeasuredValue::Id);
        if (attr && attribute::get_val_internal(attr, &val) == ESP_OK && val.type == ESP_MATTER_VAL_TYPE_NULLABLE_INT16) {
            if (val.val.i16 != INT16_MIN) {
                config.maxMeasuredValue = DataModel::MakeNullable(static_cast<int16_t>(val.val.i16));
            }
        }

        // Tolerance (optional, uint16)
        if (endpoint::is_attribute_enabled(endpointId, PressureMeasurement::Id, Tolerance::Id)) {
            val = esp_matter_invalid(nullptr);
            attr = attribute::get(cluster, Tolerance::Id);
            if (attr && attribute::get_val_internal(attr, &val) == ESP_OK && val.type == ESP_MATTER_VAL_TYPE_UINT16) {
                config.WithTolerance(val.val.u16);
            }
        }

        // Extended feature
        uint32_t feature_map = read_feature_map_u32(endpointId, PressureMeasurement::Id);
        BitFlags<PressureMeasurement::Feature> features(feature_map);
        if (features.Has(PressureMeasurement::Feature::kExtended)) {
            DataModel::Nullable<int16_t> minScaledValue;
            DataModel::Nullable<int16_t> maxScaledValue;
            int8_t scale = 0;

            val = esp_matter_invalid(nullptr);
            attr = attribute::get(cluster, MinScaledValue::Id);
            if (attr && attribute::get_val_internal(attr, &val) == ESP_OK && val.val.i16 != INT16_MIN) {
                minScaledValue = DataModel::MakeNullable(static_cast<int16_t>(val.val.i16));
            }

            val = esp_matter_invalid(nullptr);
            attr = attribute::get(cluster, MaxScaledValue::Id);
            if (attr && attribute::get_val_internal(attr, &val) == ESP_OK && val.val.i16 != INT16_MIN) {
                maxScaledValue = DataModel::MakeNullable(static_cast<int16_t>(val.val.i16));
            }

            val = esp_matter_invalid(nullptr);
            attr = attribute::get(cluster, Scale::Id);
            if (attr && attribute::get_val_internal(attr, &val) == ESP_OK) {
                scale = static_cast<int8_t>(val.val.i8);
            }

            config.WithExtendedFeature(minScaledValue, maxScaledValue, scale);

            // ScaledTolerance (optional, uint16, requires Extended)
            if (endpoint::is_attribute_enabled(endpointId, PressureMeasurement::Id, ScaledTolerance::Id)) {
                val = esp_matter_invalid(nullptr);
                attr = attribute::get(cluster, ScaledTolerance::Id);
                if (attr && attribute::get_val_internal(attr, &val) == ESP_OK && val.type == ESP_MATTER_VAL_TYPE_UINT16) {
                    config.WithScaledTolerance(val.val.u16);
                }
            }
        }

        gServers[endpointId].Create(endpointId, config);
    }

    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "PressureMeasurement register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterPressureMeasurementClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    if (it == gServers.end() || !it->second.IsConstructed()) {
        return;
    }
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "PressureMeasurement unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}
