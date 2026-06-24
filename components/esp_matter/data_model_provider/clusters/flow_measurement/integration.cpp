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
#include <app/clusters/flow-measurement-server/FlowMeasurementCluster.h>
#include <clusters/FlowMeasurement/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/logging/CHIPLogging.h>
#include <lib/support/CodeUtils.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::FlowMeasurement;
using namespace chip::app::Clusters::FlowMeasurement::Attributes;
using namespace esp_matter;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<FlowMeasurementCluster>> gServers;
} // namespace

namespace chip::app::Clusters::FlowMeasurement {

FlowMeasurementCluster * FindClusterOnEndpoint(EndpointId endpointId)
{
    auto it = gServers.find(endpointId);
    if (it == gServers.end() || !it->second.IsConstructed()) {
        return nullptr;
    }
    return &it->second.Cluster();
}

CHIP_ERROR SetMeasuredValue(EndpointId endpointId, DataModel::Nullable<uint16_t> measuredValue)
{
    auto * cluster = FindClusterOnEndpoint(endpointId);
    VerifyOrReturnError(cluster != nullptr, CHIP_ERROR_NOT_FOUND);
    return cluster->SetMeasuredValue(measuredValue);
}

} // namespace chip::app::Clusters::FlowMeasurement

void ESPMatterFlowMeasurementClusterServerInitCallback(EndpointId endpointId)
{
    cluster_t *cluster = cluster::get(endpointId, FlowMeasurement::Id);
    VerifyOrReturn(cluster != nullptr,
                   ChipLogError(AppServer,
                                "FlowMeasurement: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));
    if (!gServers[endpointId].IsConstructed()) {
        FlowMeasurementCluster::Config config;
        esp_matter_attr_val_t val = esp_matter_invalid(nullptr);

        attribute_t *attr = attribute::get(cluster, MinMeasuredValue::Id);
        if (attr && attribute::get_val_internal(attr, &val) == ESP_OK && val.type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT16) {
            if (val.val.u16 != UINT16_MAX) {
                config.minMeasuredValue = DataModel::MakeNullable(val.val.u16);
            }
        }

        val = esp_matter_invalid(nullptr);
        attr = attribute::get(cluster, MaxMeasuredValue::Id);
        if (attr && attribute::get_val_internal(attr, &val) == ESP_OK && val.type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT16) {
            if (val.val.u16 != UINT16_MAX) {
                config.maxMeasuredValue = DataModel::MakeNullable(val.val.u16);
            }
        }

        if (endpoint::is_attribute_enabled(endpointId, FlowMeasurement::Id, Tolerance::Id)) {
            val = esp_matter_invalid(nullptr);
            attr = attribute::get(cluster, Tolerance::Id);
            if (attr && attribute::get_val_internal(attr, &val) == ESP_OK && val.type == ESP_MATTER_VAL_TYPE_UINT16) {
                config.WithTolerance(val.val.u16);
            }
        }

        gServers[endpointId].Create(endpointId, config);
    }
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "FlowMeasurement register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterFlowMeasurementClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    if (it == gServers.end() || !it->second.IsConstructed()) {
        return;
    }
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "FlowMeasurement unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}
