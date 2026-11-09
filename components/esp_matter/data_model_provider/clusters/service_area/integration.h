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

#include <app/clusters/service-area-server/CodegenIntegration.h>

namespace chip::app::Clusters::ServiceArea {

/**
 * Register the storage delegate for the Service Area cluster on the endpoint. The Service Area
 * cluster needs both a StorageDelegate and a Delegate: the Delegate comes from the cluster config,
 * the StorageDelegate from this call. Must be called before the endpoint is enabled (i.e. before
 * esp_matter::start()); the Instance is created when the cluster's delegate init callback runs.
 */
void SetStorageDelegate(EndpointId endpointId, StorageDelegate * storageDelegate);

/** Returns the storage delegate registered for the endpoint, or nullptr. */
StorageDelegate * GetStorageDelegate(EndpointId endpointId);

} // namespace chip::app::Clusters::ServiceArea
