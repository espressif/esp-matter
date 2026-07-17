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

// Water heater management was migrated to code-driven WaterHeaterManagementCluster
// in CHIP v1.6. The backwards-compat Instance wrapper in upstream CodegenIntegration.cpp
// uses CodegenDataModelProvider which conflicts with esp-matter's data model provider.
//
// This file provides the Instance wrapper that uses our registry instead.
//
// TODO: Migrate to full code-driven integration pattern.

#include <app/ClusterCallbacks.h>
#include <app/clusters/water-heater-management-server/CodegenIntegration.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/core/DataModelTypes.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

namespace chip {
namespace app {
namespace Clusters {
namespace WaterHeaterManagement {

Instance::~Instance()
{
    if (mRegistered) {
        Shutdown();
    }
}

CHIP_ERROR Instance::Init()
{
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(mCluster.Registration());
    if (err == CHIP_NO_ERROR) {
        mRegistered = true;
    }
    return err;
}

void Instance::Shutdown()
{
    if (!mRegistered) {
        return;
    }
    mRegistered = false;
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(
                         &mCluster.Cluster(), ClusterShutdownType::kPermanentRemove);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister WaterHeaterManagement: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

} // namespace WaterHeaterManagement
} // namespace Clusters
} // namespace app
} // namespace chip

void ESPMatterWaterHeaterManagementClusterServerInitCallback(EndpointId) {}
void ESPMatterWaterHeaterManagementClusterServerShutdownCallback(EndpointId, ClusterShutdownType) {}

__attribute__((weak)) void MatterWaterHeaterManagementPluginServerInitCallback() {}
__attribute__((weak)) void MatterWaterHeaterManagementPluginServerShutdownCallback() {}
