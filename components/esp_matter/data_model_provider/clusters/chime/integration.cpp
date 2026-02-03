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

#include "integration.h"
#include "esp_matter_data_model_provider.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Chime;

namespace chip::app::Clusters::Chime {
ChimeServer::~ChimeServer()
{
    RETURN_SAFELY_IGNORED esp_matter::data_model::provider::get_instance().registry().Unregister(&(mCluster.Cluster()));
}

CHIP_ERROR ChimeServer::Init()
{
    return esp_matter::data_model::provider::get_instance().registry().Register(mCluster.Registration());
}
} // namespace chip::app::Clusters
void ESPMatterChimeClusterServerInitCallback(EndpointId) {}
void ESPMatterChimeClusterServerShutdownCallback(EndpointId, ClusterShutdownType) {}

// Stub callbacks for ZAP generated code
void MatterChimePluginServerInitCallback() {}
void MatterChimePluginServerShutdownCallback() {}
