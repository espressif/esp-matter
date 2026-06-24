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

#pragma once

#include <app/clusters/zone-management-server/CodegenIntegration.h>
#include <app/clusters/zone-management-server/Delegate.h>
#include <app/clusters/zone-management-server/ZoneManagementCluster.h>
#include <lib/core/DataModelTypes.h>

namespace chip {
namespace app {
namespace Clusters {
namespace ZoneManagement {

void MatterZoneManagementSetDelegate(EndpointId endpointId, Delegate *delegate);

void MatterZoneManagementSetConfig(EndpointId endpointId, uint8_t maxUserDefinedZones, uint8_t maxZones,
                                   uint8_t sensitivityMax, const TwoDCartesianVertexStruct &twoDCartesianMax);

void MatterZoneManagementSetFeatures(EndpointId endpointId, BitFlags<Feature> features);

} // namespace ZoneManagement
} // namespace Clusters
} // namespace app
} // namespace chip
