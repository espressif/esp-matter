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
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement::Structs;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<ElectricalEnergyMeasurementCluster>> gServers;

uint32_t get_feature_map(esp_matter::cluster_t *cluster)
{
    esp_matter::attribute_t *attribute = esp_matter::attribute::get(cluster, Globals::Attributes::FeatureMap::Id);
    if (attribute) {
        esp_matter_attr_val_t val;
        if (esp_matter::attribute::get_val_internal(attribute, &val) == ESP_OK &&
                val.type == ESP_MATTER_VAL_TYPE_BITMAP32) {
            return val.val.u32;
        }
    }
    return 0;
}

} // namespace

namespace chip::app::Clusters::ElectricalEnergyMeasurement {

ElectricalEnergyMeasurementCluster *GetClusterInstance(EndpointId endpointId)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturnValue(it != gServers.end(), nullptr);
    VerifyOrReturnValue(it->second.IsConstructed(), nullptr);
    return &it->second.Cluster();
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
    if (!gServers[endpoint].IsConstructed()) {
        const MeasurementAccuracyStruct::Type kDefaultAccuracy = {};

        esp_matter::cluster_t *cluster = esp_matter::cluster::get(endpoint, ElectricalEnergyMeasurement::Id);
        VerifyOrReturn(cluster != nullptr,
                       ChipLogError(AppServer,
                                    "ElectricalEnergyMeasurement: cluster missing in esp-matter data model for endpoint %u",
                                    endpoint));

        BitMask<ElectricalEnergyMeasurement::OptionalAttributes> optionalAttrs;
        if (esp_matter::attribute::get(cluster, Attributes::CumulativeEnergyReset::Id)) {
            optionalAttrs.SetField(OptionalAttributes::kOptionalAttributeCumulativeEnergyReset, 1);
        } else {
            optionalAttrs.Clear(OptionalAttributes::kOptionalAttributeCumulativeEnergyReset);
        }

        gServers[endpoint].Create(ElectricalEnergyMeasurementCluster::Config{
            .endpointId         = endpoint,
            .featureFlags       = BitMask<ElectricalEnergyMeasurement::Feature>(get_feature_map(cluster)),
            .optionalAttributes = optionalAttrs,
            .accuracyStruct     = kDefaultAccuracy,
        });
    }
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpoint].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer,
                     "Failed to register ElectricalEnergyMeasurement - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterElectricalEnergyMeasurementClusterServerShutdownCallback(EndpointId endpointId,
                                                                       ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer,
                     "Failed to unregister ElectricalEnergyMeasurement - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}
