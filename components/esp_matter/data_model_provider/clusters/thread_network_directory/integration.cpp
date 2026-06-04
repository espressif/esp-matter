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

// ThreadNetworkDirectory requires a ThreadNetworkDirectoryStorage implementation
// from the application. The upstream CodegenIntegration.h provides
// DefaultThreadNetworkDirectoryServer which bundles DefaultThreadNetworkDirectoryStorage.
//
// The application must create a DefaultThreadNetworkDirectoryServer (or equivalent)
// and call Init() to register the cluster. This file provides the ESPMatter callback
// stubs and plugin init stubs.

#include <esp_matter_data_model_priv.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/thread-network-directory-server/ThreadNetworkDirectoryCluster.h>
#include <clusters/ThreadNetworkDirectory/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;
using namespace chip::app;
using namespace esp_matter;

void ESPMatterThreadNetworkDirectoryClusterServerInitCallback(EndpointId endpointId)
{
    // ThreadNetworkDirectory is application-managed. The application must create a
    // DefaultThreadNetworkDirectoryServer and call Init().
    ChipLogProgress(AppServer,
                    "ThreadNetworkDirectory: ep %u - application must register cluster "
                    "via DefaultThreadNetworkDirectoryServer",
                    endpointId);
}

void ESPMatterThreadNetworkDirectoryClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    // Cleanup is handled by the application's DefaultThreadNetworkDirectoryServer destructor.
}

void MatterThreadNetworkDirectoryPluginServerInitCallback() {}
void MatterThreadNetworkDirectoryPluginServerShutdownCallback() {}
