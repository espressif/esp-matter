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
#include "app/clusters/electrical-energy-measurement-server/ElectricalEnergyMeasurementDelegate.h"
#include "app/server-cluster/ServerClusterInterfaceRegistry.h"
#include "clusters/ElectricalEnergyMeasurement/Enums.h"
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/DefaultTimerDelegate.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement::Structs;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<ElectricalEnergyMeasurementCluster>> gServers;

// Fallback delegate when the app does not provide one via config_t.delegate.
// The upstream Config requires a Delegate& (reference, cannot be null).
// This no-op is safe because energy readings are pushed via CumulativeEnergySnapshot /
// PeriodicEnergySnapshot rather than polled through the delegate.
class NoOpEEMDelegate : public ElectricalEnergyMeasurement::Delegate {
public:
    DataModel::Nullable<int64_t> GetCumulativeEnergyImported() override
    {
        return DataModel::NullNullable;
    }
    DataModel::Nullable<int64_t> GetCumulativeEnergyExported() override
    {
        return DataModel::NullNullable;
    }
    DataModel::Nullable<int64_t> GetPeriodicEnergyImported() override
    {
        return DataModel::NullNullable;
    }
    DataModel::Nullable<int64_t> GetPeriodicEnergyExported() override
    {
        return DataModel::NullNullable;
    }
};

NoOpEEMDelegate gNoOpDelegate;
DefaultTimerDelegate gDefaultTimerDelegate;

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

CHIP_ERROR SetMeasurementAccuracy(EndpointId endpointId, const Structs::MeasurementAccuracyStruct::Type &accuracy)
{
    ElectricalEnergyMeasurementCluster *cluster = GetClusterInstance(endpointId);
    VerifyOrReturnError(cluster != nullptr, CHIP_ERROR_NOT_FOUND);
    return cluster->SetMeasurementAccuracy(accuracy);
}

CHIP_ERROR SetCumulativeReset(EndpointId endpointId,
                              const DataModel::Nullable<Structs::CumulativeEnergyResetStruct::Type> &cumulativeReset)
{
    ElectricalEnergyMeasurementCluster *cluster = GetClusterInstance(endpointId);
    VerifyOrReturnError(cluster != nullptr, CHIP_ERROR_NOT_FOUND);
    return cluster->SetCumulativeEnergyReset(cumulativeReset);
}

bool NotifyCumulativeEnergyMeasured(EndpointId endpointId,
                                    const DataModel::Nullable<Structs::EnergyMeasurementStruct::Type> &energyImported,
                                    const DataModel::Nullable<Structs::EnergyMeasurementStruct::Type> &energyExported)
{
    ElectricalEnergyMeasurementCluster *cluster = GetClusterInstance(endpointId);

    VerifyOrReturnValue(cluster != nullptr, false);
    VerifyOrReturnValue(cluster->Features().Has(Feature::kCumulativeEnergy), false);

    cluster->CumulativeEnergySnapshot(energyImported, energyExported);
    return true;
}

bool NotifyPeriodicEnergyMeasured(EndpointId endpointId,
                                  const DataModel::Nullable<Structs::EnergyMeasurementStruct::Type> &energyImported,
                                  const DataModel::Nullable<Structs::EnergyMeasurementStruct::Type> &energyExported)
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
    static const MeasurementAccuracyStruct::Type kDefaultAccuracy = {};
    if (!gServers[endpoint].IsConstructed()) {

        esp_matter::cluster_t *cluster = esp_matter::cluster::get(endpoint, ElectricalEnergyMeasurement::Id);
        VerifyOrReturn(cluster != nullptr,
                       ChipLogError(AppServer,
                                    "ElectricalEnergyMeasurement: cluster missing in esp-matter data model for endpoint %u",
                                    endpoint));

        auto optionalAttrs = ElectricalEnergyMeasurementCluster::OptionalAttributesSet()
                             .Set<Attributes::CumulativeEnergyReset::Id>(
                                 esp_matter::attribute::get(cluster, Attributes::CumulativeEnergyReset::Id) != nullptr);

        // Use app-provided delegate (set via config_t.delegate), fall back to no-op
        void *delegate_ptr = esp_matter::cluster::get_delegate_impl(cluster);
        ElectricalEnergyMeasurement::Delegate *delegate_p =
            delegate_ptr ? static_cast<ElectricalEnergyMeasurement::Delegate *>(delegate_ptr) : &gNoOpDelegate;

        const ElectricalEnergyMeasurementCluster::Config config{
            .endpointId         = endpoint,
            .featureFlags       = BitMask<ElectricalEnergyMeasurement::Feature>(get_feature_map(cluster)),
            .optionalAttributes = optionalAttrs,
            .accuracyStruct     = kDefaultAccuracy,
            .delegate           = *delegate_p,
            .timerDelegate      = gDefaultTimerDelegate,
        };
        gServers[endpoint].Create(config);
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
