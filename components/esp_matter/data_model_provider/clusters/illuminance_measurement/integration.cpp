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

#include <esp_matter_data_model_priv.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h>
#include <clusters/IlluminanceMeasurement/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/logging/CHIPLogging.h>
#include <lib/support/CodeUtils.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::IlluminanceMeasurement;
using namespace chip::app::Clusters::IlluminanceMeasurement::Attributes;
using namespace chip::Protocols::InteractionModel;
using namespace esp_matter;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<IlluminanceMeasurementCluster>> gServers;
} // namespace

void ESPMatterIlluminanceMeasurementClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, IlluminanceMeasurement::Id) != nullptr,
                   ChipLogError(AppServer,
                                "IlluminanceMeasurement: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));
    if (!gServers[endpointId].IsConstructed()) {
        IlluminanceMeasurementCluster::OptionalAttributeSet optionalAttributeSet(0);
        if (endpoint::is_attribute_enabled(endpointId, IlluminanceMeasurement::Id, Tolerance::Id)) {
            optionalAttributeSet.Set<Tolerance::Id>();
        }
        if (endpoint::is_attribute_enabled(endpointId, IlluminanceMeasurement::Id, LightSensorType::Id)) {
            optionalAttributeSet.Set<LightSensorType::Id>();
        }

        DataModel::Nullable<uint16_t> minMeasuredValue{};
        if (MinMeasuredValue::GetDefault(endpointId, minMeasuredValue) != Status::Success) {
            minMeasuredValue.SetNull();
        }

        DataModel::Nullable<uint16_t> maxMeasuredValue{};
        if (MaxMeasuredValue::GetDefault(endpointId, maxMeasuredValue) != Status::Success) {
            maxMeasuredValue.SetNull();
        }

        uint16_t tolerance{};
        if (optionalAttributeSet.IsSet(Tolerance::Id)) {
            VerifyOrDie(Tolerance::GetDefault(endpointId, &tolerance) == Status::Success);
        }

        DataModel::Nullable<chip::app::Clusters::IlluminanceMeasurement::LightSensorTypeEnum> lightSensorType{};
        if (optionalAttributeSet.IsSet(LightSensorType::Id)) {
            VerifyOrDie(LightSensorType::GetDefault(endpointId, lightSensorType) == Status::Success);
        }

        gServers[endpointId].Create(
            endpointId, optionalAttributeSet,
            IlluminanceMeasurementCluster::StartupConfiguration{ .minMeasuredValue = minMeasuredValue,
                                                                 .maxMeasuredValue = maxMeasuredValue,
                                                                 .tolerance        = tolerance,
                                                                 .lightSensorType  = lightSensorType });
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(
                         gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "IlluminanceMeasurement register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterIlluminanceMeasurementClusterServerShutdownCallback(EndpointId endpointId,
                                                                  ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "IlluminanceMeasurement unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}
