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

#include <clusters/closure_dimension/integration.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/closure-dimension-server/ClosureDimensionCluster.h>
#include <clusters/ClosureDimension/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ClosureDimension;
using namespace esp_matter;

namespace {

std::unordered_map<EndpointId, LazyRegisteredServerCluster<ClosureDimensionCluster>> gServers;
std::unordered_map<EndpointId, ClosureDimensionClusterDelegate *> gDelegates;

} // namespace

namespace chip {
namespace app {
namespace Clusters {
namespace ClosureDimension {

void MatterClosureDimensionSetDelegate(EndpointId endpointId, ClosureDimensionClusterDelegate &delegate)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it == gServers.end() || !it->second.IsConstructed(),
                   ChipLogError(AppServer, "ClosureDimension: cluster already initialized; cannot set delegate"));
    gDelegates[endpointId] = &delegate;
}

} // namespace ClosureDimension
} // namespace Clusters
} // namespace app
} // namespace chip

void ESPMatterClosureDimensionClusterServerInitCallback(EndpointId endpointId)
{
    cluster_t *cluster = cluster::get(endpointId, ClosureDimension::Id);
    VerifyOrReturn(cluster != nullptr,
                   ChipLogError(AppServer,
                                "ClosureDimension: cluster missing in esp-matter data model for endpoint %u", endpointId));

    if (!gServers[endpointId].IsConstructed()) {
        ClosureDimensionClusterDelegate *delegate = nullptr;
        {
            auto it = gDelegates.find(endpointId);
            VerifyOrReturn(it != gDelegates.end() && it->second != nullptr,
                           ChipLogError(AppServer,
                                        "ClosureDimension: delegate not set for ep %u (call MatterClosureDimensionSetDelegate first)",
                                        endpointId));
            delegate = it->second;
        }

        // Read feature map from esp-matter data model
        BitFlags<ClosureDimension::Feature> features(read_feature_map_u32(endpointId, ClosureDimension::Id));

        // Build Config — feature-specific init params (resolution, stepValue, unit, etc.)
        // are MANAGED_INTERNALLY attributes. Use Config defaults; the code-driven cluster
        // initializes them properly via Startup().
        ClosureDimensionCluster::Config config(endpointId, *delegate);

        if (features.Has(Feature::kPositioning)) {
            esp_matter_attr_val_t resolution_val = esp_matter_uint16(1);
            esp_matter_attr_val_t step_val = esp_matter_uint16(1);
            attribute_t *attr = attribute::get(cluster, Attributes::Resolution::Id);
            if (attr) {
                attribute::get_val_internal(attr, &resolution_val);
            }
            attr = attribute::get(cluster, Attributes::StepValue::Id);
            if (attr) {
                attribute::get_val_internal(attr, &step_val);
            }
            config.WithPositioning(resolution_val.val.u16, step_val.val.u16);
        }
        if (features.Has(Feature::kMotionLatching)) {
            config.WithMotionLatching({});
        }
        if (features.Has(Feature::kSpeed)) {
            config.WithSpeed();
        }

        gServers[endpointId].Create(config);
    }

    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "ClosureDimension register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterClosureDimensionClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    if (it == gServers.end() || !it->second.IsConstructed()) {
        return;
    }
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "ClosureDimension unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

__attribute__((weak)) void MatterClosureDimensionPluginServerInitCallback() {}
__attribute__((weak)) void MatterClosureDimensionPluginServerShutdownCallback() {}
