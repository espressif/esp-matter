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
#include <app/SafeAttributePersistenceProvider.h>
#include <app/clusters/mode-base-server/CodegenIntegration.h>
#include <app/clusters/mode-base-server/mode-base-cluster-objects.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/IntrusiveList.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/DiagnosticDataProvider.h>

#include <optional>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ModeBase::Attributes;
using chip::Protocols::InteractionModel::Status;

namespace chip::app::Clusters::ModeBase {

namespace {

IntrusiveList<Instance> gModeBaseInstances;

} // namespace

IntrusiveList<Instance>  &GetModeBaseInstanceList()
{
    return gModeBaseInstances;
}

Instance::Instance(Delegate * aDelegate, EndpointId aEndpointId, ClusterId aClusterId, uint32_t aFeature) :
    mDelegate(aDelegate), mClusterPath(aEndpointId, aClusterId), mFeature(aFeature)
{}

Instance::~Instance()
{
    Shutdown();
}

CHIP_ERROR Instance::Init()
{
    // Endpoint re-enable invokes the delegate init callback again. The cluster survives a
    // non-permanent shutdown (no shutdown callback is registered for mode clusters), so a
    // second Init() has nothing to re-create or re-register.
    VerifyOrReturnError(!mCluster.IsConstructed(), CHIP_NO_ERROR);

    std::optional<uint32_t> clusterRevision;
    for (const auto  &entry : kAliasedClusters) {
        if (entry.id == mClusterPath.mClusterId) {
            clusterRevision = entry.revision;
            break;
        }
    }
    VerifyOrReturnError(clusterRevision.has_value(), CHIP_ERROR_INVALID_ARGUMENT);

    // No aliased ModeBase cluster supports StartUpMode or the OnOff feature (spec).
    // TODO: if one ever enables kOnOff, derive onOffValueForStartUp from StartUpOnOff via
    //       esp-matter's attribute store (upstream uses ember static-ZAP metadata we lack).
    VerifyOrReturnError(!HasFeature(ModeBase::Feature::kOnOff), CHIP_ERROR_INCORRECT_STATE);
    const bool onOffValueForStartUp = false;

    VerifyOrReturnError(mDelegate != nullptr, CHIP_ERROR_INCORRECT_STATE);
    mDelegate->SetInstance(this);
    ReturnErrorOnFailure(mDelegate->Init());

    SafeAttributePersistenceProvider * safeAttributePersistenceProvider = GetSafeAttributePersistenceProvider();
    VerifyOrReturnError(safeAttributePersistenceProvider != nullptr, CHIP_ERROR_INCORRECT_STATE);

    DeviceLayer::DiagnosticDataProvider  &diagnosticDataProvider = DeviceLayer::GetDiagnosticDataProvider();

    ModeBaseCluster::Config config{ .feature                          = mFeature,
                                    .optionalAttributeSet             = mOptionalAttributeSet,
                                    .appDelegate                      = *mDelegate,
                                    .onOffValueForStartUp             = onOffValueForStartUp,
                                    .safeAttributePersistenceProvider = *safeAttributePersistenceProvider,
                                    .diagnosticDataProvider           = diagnosticDataProvider,
                                    .clusterRevision                  = clusterRevision.value() };
    mCluster.Create(mClusterPath.mEndpointId, mClusterPath.mClusterId, config);
    RegisterThisInstance();
    return esp_matter::data_model::provider::get_instance().registry().Register(mCluster.Registration());
}

Status Instance::UpdateCurrentMode(uint8_t aNewMode)
{
    VerifyOrDie(mCluster.IsConstructed());
    return mCluster.Cluster().UpdateCurrentMode(aNewMode);
}

Status Instance::UpdateStartUpMode(DataModel::Nullable<uint8_t> aNewStartUpMode)
{
    VerifyOrDie(mCluster.IsConstructed());
    return mCluster.Cluster().UpdateStartUpMode(aNewStartUpMode);
}

Status Instance::UpdateOnMode(DataModel::Nullable<uint8_t> aNewOnMode)
{
    VerifyOrDie(mCluster.IsConstructed());
    return mCluster.Cluster().UpdateOnMode(aNewOnMode);
}

uint8_t Instance::GetCurrentMode() const
{
    VerifyOrDie(mCluster.IsConstructed());
    return mCluster.Cluster().GetCurrentMode();
}

DataModel::Nullable<uint8_t> Instance::GetStartUpMode() const
{
    VerifyOrDie(mCluster.IsConstructed());
    return mCluster.Cluster().GetStartUpMode();
}

DataModel::Nullable<uint8_t> Instance::GetOnMode() const
{
    VerifyOrDie(mCluster.IsConstructed());
    return mCluster.Cluster().GetOnMode();
}

void Instance::ReportSupportedModesChange()
{
    VerifyOrDie(mCluster.IsConstructed());
    mCluster.Cluster().ReportSupportedModesChange();
}

bool Instance::IsSupportedMode(uint8_t mode)
{
    VerifyOrDie(mCluster.IsConstructed());
    return mCluster.Cluster().IsSupportedMode(mode);
}

CHIP_ERROR Instance::GetModeValueByModeTag(uint16_t modeTag, uint8_t  &value)
{
    VerifyOrDie(mCluster.IsConstructed());
    return mCluster.Cluster().GetModeValueByModeTag(modeTag, value);
}

void Instance::RegisterThisInstance()
{
    if (!GetModeBaseInstanceList().Contains(this)) {
        GetModeBaseInstanceList().PushBack(this);
    }
}

void Instance::UnregisterThisInstance()
{
    GetModeBaseInstanceList().Remove(this);
}

void Instance::Shutdown()
{
    if (mDelegate != nullptr) {
        mDelegate->SetInstance(nullptr);
    }
    UnregisterThisInstance();
    if (mCluster.IsConstructed()) {
        LogErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Unregister(
                              &mCluster.Cluster(), ClusterShutdownType::kPermanentRemove));
        mCluster.Destroy();
    }
}

} // namespace chip::app::Clusters::ModeBase
