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

#include <app/ClusterCallbacks.h>
#include <lib/core/DataModelTypes.h>

// Thread Border Router Management is a delegate-bearing cluster.
// The delegate callback (ThreadBorderRouterManagementDelegateInitCB) handles
// creating and registering the ThreadBorderRouterManagementCluster instance.
// These init/shutdown callbacks are intentionally empty stubs.
void ESPMatterThreadBorderRouterManagementClusterServerInitCallback(chip::EndpointId) {}
void ESPMatterThreadBorderRouterManagementClusterServerShutdownCallback(chip::EndpointId, ClusterShutdownType) {}

__attribute__((weak)) void MatterThreadBorderRouterManagementPluginServerInitCallback() {}
__attribute__((weak)) void MatterThreadBorderRouterManagementPluginServerShutdownCallback() {}
