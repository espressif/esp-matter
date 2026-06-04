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

#include <clusters/commissioner_control/integration.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/commissioner-control-server/CommissionerControlCluster.h>
#include <app/clusters/commissioner-control-server/Delegate.h>
#include <clusters/CommissionerControl/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::CommissionerControl;
using namespace esp_matter;

namespace {

std::unordered_map<EndpointId, LazyRegisteredServerCluster<CommissionerControlCluster>> gServers;
std::unordered_map<EndpointId, Delegate *> gDelegates;

} // namespace

namespace chip {
namespace app {
namespace Clusters {
namespace CommissionerControl {

void MatterCommissionerControlSetDelegate(EndpointId endpointId, Delegate *delegate)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it == gServers.end() || !it->second.IsConstructed(),
                   ChipLogError(AppServer, "CommissionerControl: cluster already initialized; cannot set delegate"));
    gDelegates[endpointId] = delegate;
}

} // namespace CommissionerControl
} // namespace Clusters
} // namespace app
} // namespace chip

void ESPMatterCommissionerControlClusterServerInitCallback(EndpointId endpointId)
{
    cluster_t *cluster = cluster::get(endpointId, CommissionerControl::Id);
    VerifyOrReturn(cluster != nullptr,
                   ChipLogError(AppServer,
                                "CommissionerControl: cluster missing in esp-matter data model for endpoint %u", endpointId));

    if (!gServers[endpointId].IsConstructed()) {
        Delegate *delegate = nullptr;
        {
            auto it = gDelegates.find(endpointId);
            VerifyOrReturn(it != gDelegates.end() && it->second != nullptr,
                           ChipLogError(AppServer,
                                        "CommissionerControl: delegate not set for ep %u "
                                        "(call MatterCommissionerControlSetDelegate first)",
                                        endpointId));
            delegate = it->second;
        }

        gServers[endpointId].Create(endpointId, *delegate);
    }

    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "CommissionerControl register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterCommissionerControlClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    if (it == gServers.end() || !it->second.IsConstructed()) {
        return;
    }
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "CommissionerControl unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

void MatterCommissionerControlPluginServerInitCallback() {}
void MatterCommissionerControlPluginServerShutdownCallback() {}
