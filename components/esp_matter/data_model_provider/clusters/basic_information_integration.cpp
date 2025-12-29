// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/basic-information/BasicInformationCluster.h>
#include <lib/core/DataModelTypes.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::BasicInformation::Attributes;

namespace {
ServerClusterRegistration gRegistration(BasicInformationCluster::Instance());

bool IsAttrEnabled(uint32_t attrId)
{
    return esp_matter::attribute::get(kRootEndpointId, BasicInformation::Id, attrId);
}

BasicInformationCluster::OptionalAttributesSet GetOptionalAttrsSet()
{
    BasicInformationCluster::OptionalAttributesSet attrsSet;
    return attrsSet.Set<ManufacturingDate::Id>(IsAttrEnabled(ManufacturingDate::Id))
        .Set<PartNumber::Id>(IsAttrEnabled(PartNumber::Id))
        .Set<ProductURL::Id>(IsAttrEnabled(ProductURL::Id))
        .Set<ProductLabel::Id>(IsAttrEnabled(ProductLabel::Id))
        .Set<SerialNumber::Id>(IsAttrEnabled(SerialNumber::Id))
        .Set<LocalConfigDisabled::Id>(IsAttrEnabled(LocalConfigDisabled::Id))
        .Set<Reachable::Id>(IsAttrEnabled(Reachable::Id))
        .Set<ProductAppearance::Id>(IsAttrEnabled(ProductAppearance::Id))
        .Set<UniqueID::Id>(IsAttrEnabled(UniqueID::Id));
}
} // namespace

void ESPMatterBasicInformationClusterServerInitCallback(EndpointId endpoint)
{
    // We implement the cluster as a singleton on the root endpoint.
    VerifyOrReturn(endpoint == kRootEndpointId);

    BasicInformationCluster::Instance().OptionalAttributes() = GetOptionalAttrsSet();
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(gRegistration);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register BasicInformation - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterBasicInformationClusterServerShutdownCallback(EndpointId endpointId)
{
    // We implement the cluster as a singleton on the root endpoint.
    VerifyOrReturn(endpointId == kRootEndpointId);
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(gRegistration.serverClusterInterface);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister BasicInformation - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void MatterBasicInformationPluginServerInitCallback() {}
void MatterBasicInformationPluginServerShutdownCallback() {}
