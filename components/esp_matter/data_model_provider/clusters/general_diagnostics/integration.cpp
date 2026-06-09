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

#include "integration.h"
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_data_model_priv.h>

#include <app/ClusterCallbacks.h>
#include <app/InteractionModelEngine.h>
#include <app/clusters/general-diagnostics-server/GeneralDiagnosticsCluster.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <clusters/GeneralDiagnostics/ClusterId.h>
#include <clusters/GeneralDiagnostics/Enums.h>
#include <lib/support/BitFlags.h>
#include <lib/support/CodeUtils.h>

#include <app/server/Server.h>
#include <platform/DiagnosticDataProvider.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace esp_matter;

namespace {

struct cluster_server {
    cluster_server()
        : isFullConfigurable(false)
    {
    }
    ~cluster_server()
    {
        if (isFullConfigurable && fullConfigurableServer.IsConstructed()) {
            fullConfigurableServer.Destroy();
        } else if (!isFullConfigurable && server.IsConstructed()) {
            server.Destroy();
        }
    }

    bool isFullConfigurable;
    union {
        LazyRegisteredServerCluster<GeneralDiagnosticsClusterFullConfigurable> fullConfigurableServer;
        LazyRegisteredServerCluster<GeneralDiagnosticsCluster> server;
    };
};

cluster_server gServer;

bool IsAttributeEnabled(EndpointId endpointId, AttributeId attributeId)
{
    return endpoint::is_attribute_enabled(endpointId, GeneralDiagnostics::Id, attributeId);
}

bool IsCommandEnabled(EndpointId endpointId, CommandId commandId, uint16_t commandFlag)
{
    cluster_t *cluster = cluster::get(endpointId, GeneralDiagnostics::Id);
    command_t *command = command::get(cluster, commandId, commandFlag);
    return command != nullptr;
}

bool IsClusterEnabled(EndpointId endpointId, ClusterId clusterId)
{
    cluster_t *cluster = cluster::get(endpointId, clusterId);
    return cluster != nullptr;
}

} // namespace

void ESPMatterGeneralDiagnosticsClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    if (gServer.isFullConfigurable ? !gServer.fullConfigurableServer.IsConstructed() : !gServer.server.IsConstructed()) {
        chip::app::InteractionModelEngine * interactionModel = chip::app::InteractionModelEngine::GetInstance();
        VerifyOrDie(interactionModel != nullptr);
        GeneralDiagnosticsCluster::OptionalAttributeSet attrSet;
        if (IsAttributeEnabled(endpointId, GeneralDiagnostics::Attributes::TotalOperationalHours::Id)) {
            attrSet.Set<GeneralDiagnostics::Attributes::TotalOperationalHours::Id>();
        }
        if (IsAttributeEnabled(endpointId, GeneralDiagnostics::Attributes::BootReason::Id)) {
            attrSet.Set<GeneralDiagnostics::Attributes::BootReason::Id>();
        }
        if (IsAttributeEnabled(endpointId, GeneralDiagnostics::Attributes::ActiveHardwareFaults::Id)) {
            attrSet.Set<GeneralDiagnostics::Attributes::ActiveHardwareFaults::Id>();
        }
        if (IsAttributeEnabled(endpointId, GeneralDiagnostics::Attributes::ActiveRadioFaults::Id)) {
            attrSet.Set<GeneralDiagnostics::Attributes::ActiveRadioFaults::Id>();
        }
        if (IsAttributeEnabled(endpointId, GeneralDiagnostics::Attributes::ActiveNetworkFaults::Id)) {
            attrSet.Set<GeneralDiagnostics::Attributes::ActiveNetworkFaults::Id>();
        }
        attribute_t *feature = attribute::get(endpointId, GeneralDiagnostics::Id, Globals::Attributes::FeatureMap::Id);
        esp_matter_attr_val_t feature_val;
        VerifyOrReturn(attribute::get_val_internal(feature, &feature_val) == ESP_OK &&
                       feature_val.type == ESP_MATTER_VAL_TYPE_BITMAP32);
        BitFlags<GeneralDiagnostics::Feature> featureFlags(feature_val.val.u32);

        if (IsCommandEnabled(endpointId, GeneralDiagnostics::Commands::PayloadTestRequest::Id, COMMAND_FLAG_ACCEPTED) ||
                IsClusterEnabled(endpointId, TimeSynchronization::Id)) {
            gServer.isFullConfigurable = true;
            GeneralDiagnosticsFunctionsConfig functionsConfig{
                /*
                Only consider real time if time sync cluster is actually enabled. If it's not
                enabled, this avoids likelihood of frequently reporting unusable unsynched time.
                */
                IsClusterEnabled(endpointId, TimeSynchronization::Id),
                IsCommandEnabled(endpointId, GeneralDiagnostics::Commands::PayloadTestRequest::Id, COMMAND_FLAG_ACCEPTED),
            };

            gServer.fullConfigurableServer.Create(attrSet, featureFlags, GeneralDiagnosticsCluster::Context{
                .deviceLoadStatusProvider = *interactionModel,
                .diagnosticDataProvider   = DeviceLayer::GetDiagnosticDataProvider(),
                .testEventTriggerDelegate = Server::GetInstance().GetTestEventTriggerDelegate(),
            }, functionsConfig);
        } else {
            gServer.server.Create(attrSet, featureFlags,  GeneralDiagnosticsCluster::Context{
                .deviceLoadStatusProvider = *interactionModel,
                .diagnosticDataProvider   = DeviceLayer::GetDiagnosticDataProvider(),
                .testEventTriggerDelegate = Server::GetInstance().GetTestEventTriggerDelegate(),
            });
        }
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(
                         gServer.isFullConfigurable ? gServer.fullConfigurableServer.Registration() :
                         gServer.server.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register GeneralDiagnostics - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterGeneralDiagnosticsClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    VerifyOrReturn(gServer.isFullConfigurable ? gServer.fullConfigurableServer.IsConstructed() :
                   gServer.server.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(
                         gServer.isFullConfigurable ? &gServer.fullConfigurableServer.Cluster() : &gServer.server.Cluster(),
                         shutdownType);
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        gServer.isFullConfigurable ? gServer.fullConfigurableServer.Destroy() : gServer.server.Destroy();
    }
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister GeneralDiagnostics - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void MatterGeneralDiagnosticsPluginServerInitCallback()
{
}

void MatterGeneralDiagnosticsPluginServerShutdownCallback()
{
}

namespace chip::app::Clusters::GeneralDiagnostics {
void GlobalNotifyDeviceReboot(GeneralDiagnostics::BootReasonEnum bootReason)
{
    if (gServer.isFullConfigurable && gServer.fullConfigurableServer.IsConstructed()) {
        gServer.fullConfigurableServer.Cluster().OnDeviceReboot(bootReason);
    }
    if (!gServer.isFullConfigurable && gServer.server.IsConstructed()) {
        gServer.server.Cluster().OnDeviceReboot(bootReason);
    }
}

void GlobalNotifyHardwareFaultsDetect(const DeviceLayer::GeneralFaults<DeviceLayer::kMaxHardwareFaults> &previous,
                                      const DeviceLayer::GeneralFaults<DeviceLayer::kMaxHardwareFaults> &current)
{
    if (gServer.isFullConfigurable && gServer.fullConfigurableServer.IsConstructed()) {
        gServer.fullConfigurableServer.Cluster().OnHardwareFaultsDetect(previous, current);
    }
    if (!gServer.isFullConfigurable && gServer.server.IsConstructed()) {
        gServer.server.Cluster().OnHardwareFaultsDetect(previous, current);
    }
}

void GlobalNotifyRadioFaultsDetect(const DeviceLayer::GeneralFaults<DeviceLayer::kMaxRadioFaults> &previous,
                                   const DeviceLayer::GeneralFaults<DeviceLayer::kMaxRadioFaults> &current)
{
    if (gServer.isFullConfigurable && gServer.fullConfigurableServer.IsConstructed()) {
        gServer.fullConfigurableServer.Cluster().OnRadioFaultsDetect(previous, current);
    }
    if (!gServer.isFullConfigurable && gServer.server.IsConstructed()) {
        gServer.server.Cluster().OnRadioFaultsDetect(previous, current);
    }
}

void GlobalNotifyNetworkFaultsDetect(const DeviceLayer::GeneralFaults<DeviceLayer::kMaxNetworkFaults> &previous,
                                     const DeviceLayer::GeneralFaults<DeviceLayer::kMaxNetworkFaults> &current)
{
    if (gServer.isFullConfigurable && gServer.fullConfigurableServer.IsConstructed()) {
        gServer.fullConfigurableServer.Cluster().OnNetworkFaultsDetect(previous, current);
    }
    if (!gServer.isFullConfigurable && gServer.server.IsConstructed()) {
        gServer.server.Cluster().OnNetworkFaultsDetect(previous, current);
    }
}
} // namespace chip::app::Clusters::GeneralDiagnostics
