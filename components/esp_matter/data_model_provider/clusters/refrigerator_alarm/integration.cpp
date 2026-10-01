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

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/ClusterCallbacks.h>
#include <app/clusters/alarm-base-server/Delegate.h>
#include <app/clusters/refrigerator-alarm-server/RefrigeratorAlarmCluster.h>
#include <clusters/RefrigeratorAlarm/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::Protocols::InteractionModel;
using namespace esp_matter;

namespace {

// Forwards to the application delegate once one is set; the cluster requires a delegate
// reference at construction, before the app can provide one. The base class defaults
// (accept every request) apply until then.
class AlarmDelegateWrapper final : public AlarmBase::Delegate {
public:
    void SetWrapped(AlarmBase::Delegate *wrapped)
    {
        mWrapped = wrapped;
    }

    bool ModifyEnabledAlarms(AlarmBase::AlarmMap mask) override
    {
        VerifyOrReturnValue(mWrapped != nullptr, true);
        return mWrapped->ModifyEnabledAlarms(mask);
    }

    bool ResetAlarms(AlarmBase::AlarmMap alarms) override
    {
        VerifyOrReturnValue(mWrapped != nullptr, true);
        return mWrapped->ResetAlarms(alarms);
    }

private:
    AlarmBase::Delegate *mWrapped = nullptr;
};

struct RefrigeratorAlarmEntry {
    AlarmDelegateWrapper delegate;
    LazyRegisteredServerCluster<RefrigeratorAlarmCluster> server;
};

std::unordered_map<EndpointId, RefrigeratorAlarmEntry> gServers;

} // namespace

void ESPMatterRefrigeratorAlarmClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, RefrigeratorAlarm::Id) != nullptr,
                   ChipLogError(AppServer, "RefrigeratorAlarm: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));

    auto &entry = gServers[endpointId];
    if (!entry.server.IsConstructed()) {
        AlarmBase::AlarmMap supported{};
        BitMask<RefrigeratorAlarm::AlarmBitmap> supportedDefault{};
        if (RefrigeratorAlarm::Attributes::Supported::GetDefault(endpointId, supportedDefault) == Status::Success) {
            supported = AlarmBase::AlarmMap(supportedDefault.Raw());
        }

        // RefrigeratorAlarm has no features and does not support ModifyEnabledAlarms.
        RefrigeratorAlarmCluster::Config config{
            .delegate = entry.delegate,
            .feature = BitFlags<AlarmBase::Feature>(),
            .supported = supported,
            .supportsModifyEnabledAlarms = false,
        };

        entry.server.Create(endpointId, config);

        auto &clusterServer = entry.server.Cluster();

        BitMask<RefrigeratorAlarm::AlarmBitmap> maskDefault{};
        if (RefrigeratorAlarm::Attributes::Mask::GetDefault(endpointId, maskDefault) == Status::Success) {
            clusterServer.SetMask(AlarmBase::AlarmMap(maskDefault.Raw()));
        }

        BitMask<RefrigeratorAlarm::AlarmBitmap> stateDefault{};
        if (RefrigeratorAlarm::Attributes::State::GetDefault(endpointId, stateDefault) == Status::Success) {
            clusterServer.SetState(AlarmBase::AlarmMap(stateDefault.Raw()));
        }
    }

    CHIP_ERROR err = data_model::provider::get_instance().registry().Register(entry.server.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register RefrigeratorAlarm on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterRefrigeratorAlarmClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.server.IsConstructed());

    CHIP_ERROR err = data_model::provider::get_instance().registry().Unregister(&it->second.server.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister RefrigeratorAlarm on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }

    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.server.Destroy();
        gServers.erase(it);
    }
}

__attribute__((weak)) void MatterRefrigeratorAlarmPluginServerInitCallback() {}
__attribute__((weak)) void MatterRefrigeratorAlarmPluginServerShutdownCallback() {}

namespace chip::app::Clusters::RefrigeratorAlarm {

RefrigeratorAlarmCluster * FindClusterOnEndpoint(EndpointId endpointId)
{
    auto it = gServers.find(endpointId);
    if (it == gServers.end() || !it->second.server.IsConstructed()) {
        return nullptr;
    }
    return &it->second.server.Cluster();
}

void SetDefaultDelegate(EndpointId endpointId, AlarmBase::Delegate *delegate)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end() && it->second.server.IsConstructed(),
                   ChipLogError(Zcl, "RefrigeratorAlarm cluster on endpoint %u not found", endpointId));
    it->second.delegate.SetWrapped(delegate);
}

} // namespace chip::app::Clusters::RefrigeratorAlarm
