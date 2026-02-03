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
#include "app/AttributeAccessInterfaceRegistry.h"
#include <app/server-cluster/ServerClusterInterface.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::UnitLocalization;

namespace chip::app::Clusters::UnitLocalization {

UnitLocalizationServer & UnitLocalizationServer::Instance()
{
    static UnitLocalizationServer mInstance;
    return mInstance;
}

} // namespace chip::app::Clusters::UnitLocalization

void ESPMatterUnitLocalizationServerInitCallback(EndpointId endpointId) {}
void ESPMatterUnitLocalizationServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType) {}

void MatterUnitLocalizationPluginServerInitCallback()
{
    LogErrorOnFailure(UnitLocalizationServer::Instance().Init());
    AttributeAccessInterfaceRegistry::Instance().Register(&UnitLocalizationServer::Instance());
}

void MatterUnitLocalizationPluginServerShutdownCallback()
{
    AttributeAccessInterfaceRegistry::Instance().Unregister(&UnitLocalizationServer::Instance());
}
