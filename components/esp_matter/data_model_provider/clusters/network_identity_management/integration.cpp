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

// NetworkIdentityManagement requires a NetworkIdentityStorage, NetworkIdentityKeystore,
// and AuthenticatorDriver from the application. The upstream CodegenIntegration.h states:
// "Applications using the code-gen data model should use RegisteredServerCluster or
//  LazyRegisteredServerCluster to manage a NetworkIdentityManagementCluster instance."
//
// This file provides the plugin init stubs. The application must instantiate and register
// the cluster itself using LazyRegisteredServerCluster<NetworkIdentityManagementCluster>.

#include <app/ClusterCallbacks.h>
#include <app/clusters/network-identity-management-server/NetworkIdentityManagementCluster.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;
using namespace chip::app;

void ESPMatterNetworkIdentityManagementClusterServerInitCallback(EndpointId) {}
void ESPMatterNetworkIdentityManagementClusterServerShutdownCallback(EndpointId, ClusterShutdownType) {}

void MatterNetworkIdentityManagementPluginServerInitCallback() {}
void MatterNetworkIdentityManagementPluginServerShutdownCallback() {}
