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
#include "esp_matter_data_model.h"
#include "esp_matter_data_model_priv.h"
#include "esp_matter_data_model_provider.h"
#include <unordered_map>
#include "app/clusters/electrical-energy-measurement-server/ElectricalEnergyMeasurementCluster.h"
#include "app/server-cluster/ServerClusterInterfaceRegistry.h"
#include "clusters/ElectricalEnergyMeasurement/Enums.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<ElectricalEnergyMeasurementCluster>> gServers;

uint32_t get_feature_map(esp_matter::cluster_t *cluster)
{
    esp_matter::attribute_t *attribute = esp_matter::attribute::get(cluster, Globals::Attributes::FeatureMap::Id);
    if (attribute) {
        esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
        if (esp_matter::attribute::get_val_internal(attribute, &val) == ESP_OK &&
                val.type == ESP_MATTER_VAL_TYPE_BITMAP32) {
            return val.val.u32;
        }
    }
    return 0;
}
ElectricalEnergyMeasurementCluster::Config GetClusterConfig(EndpointId endpointId)
{
    ElectricalEnergyMeasurementCluster::Config config;
    config.endpointId = endpointId;
    esp_matter::cluster_t *cluster = esp_matter::cluster::get(endpointId, ElectricalEnergyMeasurement::Id);
    config.featureFlags = BitMask<ElectricalEnergyMeasurement::Feature>(get_feature_map(cluster));
    if (esp_matter::attribute::get(cluster, Attributes::CumulativeEnergyReset::Id)) {
        config.optionalAttributes.SetField(OptionalAttributes::kOptionalAttributeCumulativeEnergyReset, 1);
    } else {
        config.optionalAttributes.Clear(OptionalAttributes::kOptionalAttributeCumulativeEnergyReset);
    }
    return config;
}
} // namespace

namespace chip::app::Clusters::ElectricalEnergyMeasurement {

ElectricalEnergyMeasurementCluster *GetClusterInstance(EndpointId endpointId)
{
    if (gServers[endpointId].IsConstructed()) {
        return &gServers[endpointId].Cluster();
    }
    return nullptr;
}

const ElectricalEnergyMeasurement::MeasurementData *MeasurementDataForEndpoint(EndpointId endpointId)
{
    ElectricalEnergyMeasurementCluster *cluster = GetClusterInstance(endpointId);
    VerifyOrReturnValue(cluster != nullptr, nullptr);

    return cluster->GetMeasurementData();
}

CHIP_ERROR SetMeasurementAccuracy(EndpointId endpointId, const Structs::MeasurementAccuracyStruct::Type &accuracy)
{
    ElectricalEnergyMeasurementCluster *cluster = GetClusterInstance(endpointId);
    VerifyOrReturnError(cluster != nullptr, CHIP_ERROR_NOT_FOUND);
    return cluster->SetMeasurementAccuracy(accuracy);
}

CHIP_ERROR SetCumulativeReset(EndpointId endpointId,
                              const Optional<Structs::CumulativeEnergyResetStruct::Type> &cumulativeReset)
{
    ElectricalEnergyMeasurementCluster *cluster = GetClusterInstance(endpointId);
    VerifyOrReturnError(cluster != nullptr, CHIP_ERROR_NOT_FOUND);
    return cluster->SetCumulativeEnergyReset(cumulativeReset);
}

bool NotifyCumulativeEnergyMeasured(EndpointId endpointId,
                                    const Optional<Structs::EnergyMeasurementStruct::Type> &energyImported,
                                    const Optional<Structs::EnergyMeasurementStruct::Type> &energyExported)
{
    ElectricalEnergyMeasurementCluster *cluster = GetClusterInstance(endpointId);

    VerifyOrReturnValue(cluster != nullptr, false);
    VerifyOrReturnValue(cluster->Features().Has(Feature::kCumulativeEnergy), false);

    cluster->CumulativeEnergySnapshot(energyImported, energyExported);
    return true;
}

bool NotifyPeriodicEnergyMeasured(EndpointId endpointId,
                                  const Optional<Structs::EnergyMeasurementStruct::Type> &energyImported,
                                  const Optional<Structs::EnergyMeasurementStruct::Type> &energyExported)
{
    ElectricalEnergyMeasurementCluster *cluster = GetClusterInstance(endpointId);

    VerifyOrReturnValue(cluster != nullptr, false);
    VerifyOrReturnValue(cluster->Features().Has(Feature::kPeriodicEnergy), false);

    cluster->PeriodicEnergySnapshot(energyImported, energyExported);
    return true;
}

} // namespace chip::app::Clusters::ElectricalEnergyMeasurement

void ESPMatterElectricalEnergyMeasurementClusterServerInitCallback(EndpointId endpoint)
{
    if (gServers[endpoint].IsConstructed()) {
        return;
    }
    ElectricalEnergyMeasurementCluster::Config config = GetClusterConfig(endpoint);
    gServers[endpoint].Create(config);
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpoint].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register AccessControl - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterElectricalEnergyMeasurementClusterServerShutdownCallback(EndpointId endpointId,
                                                                       ClusterShutdownType shutdownType)
{
    VerifyOrReturn(gServers[endpointId].IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(
                         &gServers[endpointId].Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister AccessControl - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
    gServers[endpointId].Destroy();
}
