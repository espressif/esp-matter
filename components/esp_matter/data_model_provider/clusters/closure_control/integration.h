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

#include <app/clusters/closure-control-server/ClosureControlCluster.h>
#include <app/clusters/closure-control-server/ClosureControlClusterDelegate.h>
#include <lib/core/DataModelTypes.h>

namespace chip {
namespace app {
namespace Clusters {
namespace ClosureControl {

void MatterClosureControlSetDelegate(EndpointId endpointId, ClosureControlClusterDelegate &delegate);

/// Returns the cluster instance registered on the given endpoint, nullptr otherwise
ClosureControlCluster * GetClusterInstance(EndpointId endpointId);

/// Sets the initial OverallCurrentState applied when the cluster is constructed.
/// Must be called before cluster creation (i.e. before esp_matter::start()).
void MatterClosureControlSetInitialOverallCurrentState(EndpointId endpointId,
                                                       const DataModel::Nullable<GenericOverallCurrentState> &overallCurrentState);

} // namespace ClosureControl
} // namespace Clusters
} // namespace app
} // namespace chip
