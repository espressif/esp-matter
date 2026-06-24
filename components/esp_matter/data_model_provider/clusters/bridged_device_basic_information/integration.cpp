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
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/ConcreteAttributePath.h>
#include <app/EventLogging.h>
#include <clusters/BridgedDeviceBasicInformation/Attributes.h>
#include <clusters/BridgedDeviceBasicInformation/Events.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/PlatformManager.h>
#include <tracing/macros.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::BridgedDeviceBasicInformation;
using chip::DeviceLayer::PlatformMgr;
using chip::Protocols::InteractionModel::Status;

namespace {

void EmitReachableChangedEvent(intptr_t arg)
{
    VerifyOrReturn(arg != 0);
    const EndpointId endpointId = static_cast<EndpointId>(arg);
    MATTER_TRACE_INSTANT("ReachableChanged", "BridgeBasicInfo");

    bool reachable = false;
    if (Status::Success != Attributes::Reachable::GetDefault(endpointId, &reachable)) {
        ChipLogError(AppServer, "BridgedDeviceBasicInfo: ReachableChanged: failed to read Reachable on ep %u", endpointId);
        return;
    }

    Events::ReachableChanged::Type event{ reachable };
    EventNumber eventNumber;
    LogErrorOnFailure(LogEvent(event, endpointId, eventNumber));
}

void ScheduleReachableChangedEvent(EndpointId endpointId)
{
    CHIP_ERROR err = PlatformMgr().ScheduleWork(EmitReachableChangedEvent, static_cast<intptr_t>(endpointId));
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "BridgedDeviceBasicInfo: ReachableChanged: ScheduleWork failed: %" CHIP_ERROR_FORMAT,
                     err.Format());
    }
}

} // namespace

void ESPMatterBridgedDeviceBasicInformationClusterServerInitCallback(EndpointId) {}

void ESPMatterBridgedDeviceBasicInformationClusterServerShutdownCallback(EndpointId, ClusterShutdownType) {}

void MatterBridgedDeviceBasicInformationClusterServerAttributeChangedCallback(const ConcreteAttributePath
                                                                              &attributePath)
{
    VerifyOrReturn(attributePath.mClusterId == BridgedDeviceBasicInformation::Id);

    switch (attributePath.mAttributeId) {
    case Attributes::Reachable::Id:
        ScheduleReachableChangedEvent(attributePath.mEndpointId);
        break;
    default:
        break;
    }
}
