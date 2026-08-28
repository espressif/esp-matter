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

#include <app/ClusterCallbacks.h>
#include <app/clusters/operational-state-server/CodegenIntegration.h>
#include <app/clusters/operational-state-server/OperationalStateCluster.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OperationalState;

namespace chip {
namespace app {
namespace Clusters {
namespace OperationalState {

Instance::Instance(Delegate * aDelegate, EndpointId aEndpointId, const OperationalStateCluster::Config  &config) :
    mDelegate(aDelegate), mOwnedStorage(Platform::MakeUnique<detail::OperationalInstanceBase>(aDelegate, aEndpointId, config)),
    mCluster(mOwnedStorage->mCluster.Cluster()), mRegPtr(&mOwnedStorage->mCluster.Registration())
{
    aDelegate->SetInstance(this);
}

Instance::Instance(OperationalStateCluster  &cluster, ServerClusterRegistration  &registration, Delegate * aDelegate) :
    mDelegate(aDelegate), mCluster(cluster), mRegPtr(&registration)
{
    aDelegate->SetInstance(this);
}

Instance::~Instance()
{
    if (mRegistered) {
        ChipLogError(AppServer, "OperationalState::Instance destroyed without Shutdown(); shutting down now.");
        Shutdown();
    }
    if (mDelegate) {
        mDelegate->SetInstance(nullptr);
    }
}

CHIP_ERROR Instance::Init()
{
    VerifyOrReturnError(!mRegistered, CHIP_NO_ERROR);
    ReturnErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Register(*mRegPtr));
    mRegistered = true;
    return CHIP_NO_ERROR;
}

void Instance::Shutdown()
{
    VerifyOrReturn(mRegistered);
    mRegistered = false;
    LogErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Unregister(
                          &mCluster, ClusterShutdownType::kPermanentRemove));
}

CHIP_ERROR Instance::SetCurrentPhase(const DataModel::Nullable<uint8_t>  &aPhase)
{
    return Cluster().SetCurrentPhase(aPhase);
}

CHIP_ERROR Instance::SetOperationalState(uint8_t aOpState)
{
    return Cluster().SetOperationalState(aOpState);
}

DataModel::Nullable<uint8_t> Instance::GetCurrentPhase() const
{
    return Cluster().GetCurrentPhase();
}

uint8_t Instance::GetCurrentOperationalState() const
{
    return Cluster().GetCurrentOperationalState();
}

void Instance::GetCurrentOperationalError(GenericOperationalError  &error) const
{
    Cluster().GetCurrentOperationalError(error);
}

void Instance::UpdateCountdownTimeFromDelegate()
{
    Cluster().UpdateCountdownTimeFromDelegate();
}

void Instance::OnOperationalErrorDetected(const Structs::ErrorStateStruct::Type  &aError)
{
    Cluster().OnOperationalErrorDetected(aError);
}

void Instance::OnOperationCompletionDetected(uint8_t aCompletionErrorCode,
                                             const Optional<DataModel::Nullable<uint32_t>>  &aTotalOperationalTime,
                                             const Optional<DataModel::Nullable<uint32_t>>  &aPausedTime)
{
    Cluster().OnOperationCompletionDetected(aCompletionErrorCode, aTotalOperationalTime, aPausedTime);
}

void Instance::ReportOperationalStateListChange()
{
    Cluster().ReportOperationalStateListChange();
}

void Instance::ReportPhaseListChange()
{
    Cluster().ReportPhaseListChange();
}

bool Instance::IsSupportedPhase(uint8_t aPhase)
{
    return Cluster().IsSupportedPhase(aPhase);
}

bool Instance::IsSupportedOperationalState(uint8_t aState)
{
    return Cluster().IsSupportedOperationalState(aState);
}

} // namespace OperationalState
} // namespace Clusters
} // namespace app
} // namespace chip
