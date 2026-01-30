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

#include <esp_matter_attribute_utils.h>
#include <esp_matter_data_model.h>
#include <esp_matter_data_model_priv.h>

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/PluginApplicationCallbacks.h>
#include <app/clusters/administrator-commissioning-server/AdministratorCommissioningCluster.h>

#include <data_model_provider/esp_matter_data_model_provider.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::AdministratorCommissioning::Attributes;
namespace {
using ClusterImpl = AdministratorCommissioningWithBasicCommissioningWindowCluster;

LazyRegisteredServerCluster<ClusterImpl> gServer;
} // namespace

void ESPMatterAdministratorCommissioningClusterServerInitCallback(EndpointId endpointId)
{
    if (endpointId != kRootEndpointId) {
        return;
    }
    esp_matter::attribute_t *attribute =
        esp_matter::attribute::get(endpointId, AdministratorCommissioning::Id, Globals::Attributes::FeatureMap::Id);
    if (attribute) {
        esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
        if (esp_matter::attribute::get_val_internal(attribute, &val) == ESP_OK && val.type == ESP_MATTER_VAL_TYPE_BITMAP32) {
            gServer.Create(endpointId, BitFlags<AdministratorCommissioning::Feature>(val.val.u32));
            CHIP_ERROR err =
                esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
            if (err != CHIP_NO_ERROR) {
                ChipLogError(AppServer, "Admin Commissioning register error: %" CHIP_ERROR_FORMAT, err.Format());
            }
        }
    }
}

void ESPMatterAdministratorCommissioningClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Admin Commissioning unregister error: %" CHIP_ERROR_FORMAT, err.Format());
    }
    gServer.Destroy();
}

void MatterAdministratorCommissioningPluginServerInitCallback()
{
}
void MatterAdministratorCommissioningPluginServerShutdownCallback()
{
}
