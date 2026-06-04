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

#pragma once

#include <algorithm>
#include <app/ConcreteClusterPath.h>
#include <app/clusters/microwave-oven-control-server/IntegrationDelegate.h>
#include <app/clusters/mode-base-server/mode-base-server.h>
#include <app/clusters/operational-state-server/operational-state-server.h>
#include <app/data-model-provider/MetadataTypes.h>
#include <clusters/MicrowaveOvenMode/Enums.h>
#include <clusters/OperationalState/ClusterId.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/ReadOnlyBuffer.h>
#include <lib/support/TypeTraits.h>

namespace esp_matter {
namespace cluster {
namespace delegate_cb {

// IntegrationDelegate for MicrowaveOvenControl that queries the OperationalState and ModeBase instances
class EspMatterMicrowaveOvenIntegrationDelegate : public chip::app::Clusters::MicrowaveOvenControl::IntegrationDelegate {
public:
    void SetDependencies(chip::app::Clusters::OperationalState::Instance *opState,
                         chip::app::Clusters::ModeBase::Instance *mode)
    {
        mOpState = opState;
        mMode = mode;
    }

    uint8_t GetCurrentOperationalState() const override
    {
        return mOpState ? mOpState->GetCurrentOperationalState() : 0;
    }

    CHIP_ERROR GetNormalOperatingMode(uint8_t &mode) const override
    {
        VerifyOrReturnError(mMode != nullptr, CHIP_ERROR_INCORRECT_STATE);
        return mMode->GetModeValueByModeTag(
                   chip::to_underlying(chip::app::Clusters::MicrowaveOvenMode::ModeTag::kNormal), mode);
    }

    bool IsSupportedMode(uint8_t mode) const override
    {
        return mMode ? mMode->IsSupportedMode(mode) : false;
    }

    bool IsSupportedOperationalStateCommand(chip::EndpointId endpointId, chip::CommandId commandId) const override
    {
        chip::ReadOnlyBufferBuilder<chip::app::DataModel::AcceptedCommandEntry> acceptedCommandsList;
        auto &provider = esp_matter::data_model::provider::get_instance();

        VerifyOrReturnValue(
            provider.AcceptedCommands(chip::app::ConcreteClusterPath(endpointId,
                                                                     chip::app::Clusters::OperationalState::Id), acceptedCommandsList) ==
            CHIP_NO_ERROR,
            false);
        auto acceptedCommands = acceptedCommandsList.TakeBuffer();

        return std::find_if(acceptedCommands.begin(), acceptedCommands.end(),
        [commandId](const chip::app::DataModel::AcceptedCommandEntry & entry) {
            return entry.commandId == commandId;
        }) != acceptedCommands.end();
    }

private:
    chip::app::Clusters::OperationalState::Instance *mOpState = nullptr;
    chip::app::Clusters::ModeBase::Instance *mMode = nullptr;
};

} // namespace delegate_cb
} // namespace cluster
} // namespace esp_matter
