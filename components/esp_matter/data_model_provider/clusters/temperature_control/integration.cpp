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

#include <esp_matter_data_model_priv.h>

#include <clusters/temperature_control/integration.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/temperature-control-server/TemperatureControlCluster.h>
#include <clusters/TemperatureControl/Attributes.h>
#include <clusters/TemperatureControl/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::TemperatureControl;
using namespace esp_matter;

namespace {

std::unordered_map<EndpointId, LazyRegisteredServerCluster<TemperatureControlCluster>> gServers;

} // namespace

void ESPMatterTemperatureControlClusterServerInitCallback(EndpointId endpointId)
{
    cluster_t *cluster = cluster::get(endpointId, TemperatureControl::Id);
    VerifyOrReturn(cluster != nullptr,
                   ChipLogError(AppServer,
                                "TemperatureControl: cluster missing in esp-matter data model for endpoint %u", endpointId));

    if (!gServers[endpointId].IsConstructed()) {
        const uint32_t rawFeatureMap = read_feature_map_u32(endpointId, TemperatureControl::Id);
        BitFlags<TemperatureControl::Feature> features(rawFeatureMap);

        int16_t temperatureSetpoint = 0;
        int16_t minTemperature      = 0;
        int16_t maxTemperature      = 0;
        int16_t step                = 0;
        uint8_t selectedTemperatureLevel = 0;

        if (features.Has(TemperatureControl::Feature::kTemperatureNumber)) {
            VerifyOrReturn(read_attribute_raw_value(endpointId, TemperatureControl::Id, Attributes::TemperatureSetpoint::Id,
                                                    temperatureSetpoint),
                           ChipLogError(AppServer, "TemperatureControl: TemperatureSetpoint read failed on ep %u", endpointId));
            VerifyOrReturn(read_attribute_raw_value(endpointId, TemperatureControl::Id, Attributes::MinTemperature::Id,
                                                    minTemperature),
                           ChipLogError(AppServer, "TemperatureControl: MinTemperature read failed on ep %u", endpointId));
            VerifyOrReturn(read_attribute_raw_value(endpointId, TemperatureControl::Id, Attributes::MaxTemperature::Id,
                                                    maxTemperature),
                           ChipLogError(AppServer, "TemperatureControl: MaxTemperature read failed on ep %u", endpointId));
            if (features.Has(TemperatureControl::Feature::kTemperatureStep)) {
                VerifyOrReturn(read_attribute_raw_value(endpointId, TemperatureControl::Id, Attributes::Step::Id, step),
                               ChipLogError(AppServer, "TemperatureControl: Step read failed on ep %u", endpointId));
            }
        }
        if (features.Has(TemperatureControl::Feature::kTemperatureLevel)) {
            VerifyOrReturn(
                read_attribute_raw_value(endpointId, TemperatureControl::Id, Attributes::SelectedTemperatureLevel::Id,
                                         selectedTemperatureLevel),
                ChipLogError(AppServer, "TemperatureControl: SelectedTemperatureLevel read failed on ep %u", endpointId));
            VerifyOrReturn(attribute::get(endpointId, TemperatureControl::Id,
                                          Attributes::SupportedTemperatureLevels::Id) != nullptr,
                           ChipLogError(AppServer, "TemperatureControl: SupportedTemperatureLevels missing on ep %u", endpointId));
        }

        TemperatureControlCluster::StartupConfiguration startupConfig{ .temperatureSetpoint      = temperatureSetpoint,
                                                                       .minTemperature           = minTemperature,
                                                                       .maxTemperature           = maxTemperature,
                                                                       .step                     = step,
                                                                       .selectedTemperatureLevel = selectedTemperatureLevel };

        gServers[endpointId].Create(endpointId, features, startupConfig);
    }
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "TemperatureControl register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterTemperatureControlClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "TemperatureControl unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

void MatterTemperatureControlPluginServerInitCallback() {}

namespace chip {
namespace app {
namespace Clusters {
namespace TemperatureControl {

SupportedTemperatureLevelsIteratorDelegate * GetDelegate()
{
    return TemperatureControlCluster::GetDelegate();
}

void SetDelegate(SupportedTemperatureLevelsIteratorDelegate * delegate)
{
    TemperatureControlCluster::SetDelegate(delegate);
}

} // namespace TemperatureControl
} // namespace Clusters
} // namespace app
} // namespace chip
