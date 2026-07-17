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

#include <esp_matter_data_model_priv.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/ota-requestor/CodegenIntegrationInternal.h>
#include <app/clusters/ota-requestor/OTARequestorAttributes.h>
#include <app/clusters/ota-requestor/OTARequestorCluster.h>
#include <app/clusters/ota-requestor/OTARequestorInterface.h>
#include <app/server/Server.h>
#include <clusters/OtaSoftwareUpdateRequestor/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace esp_matter;

namespace {

// The OTA requestor cluster may be registered before the OTA requestor singleton is set
// (via SetRequestorInstance). This forwarder decouples the cluster from the singleton.
class OTARequestorCommandForwarder : public OTARequestorCommandInterface {
public:
    void HandleAnnounceOTAProvider(
        CommandHandler *commandObj, const ConcreteCommandPath &commandPath,
        const OtaSoftwareUpdateRequestor::Commands::AnnounceOTAProvider::DecodableType &commandData) override
    {
        if (mDestination) {
            mDestination->HandleAnnounceOTAProvider(commandObj, commandPath, commandData);
        }
    }

    void SetDestination(OTARequestorCommandInterface *destination)
    {
        mDestination = destination;
    }

private:
    OTARequestorCommandInterface *mDestination = nullptr;
};

// Forwards event generation to the cluster once it exists.
class OTAEventForwarder : public DefaultOTARequestorEventGenerator {
public:
    ~OTAEventForwarder() override = default;

    CHIP_ERROR GenerateVersionAppliedEvent(const VersionAppliedEvent &event) override
    {
        VerifyOrReturnError(mDestination, CHIP_ERROR_INCORRECT_STATE);
        return mDestination->GenerateVersionAppliedEvent(event);
    }

    CHIP_ERROR GenerateDownloadErrorEvent(const DownloadErrorEvent &event) override
    {
        VerifyOrReturnError(mDestination, CHIP_ERROR_INCORRECT_STATE);
        return mDestination->GenerateDownloadErrorEvent(event);
    }

    void SetDestination(DefaultOTARequestorEventGenerator *destination)
    {
        mDestination = destination;
    }

private:
    DefaultOTARequestorEventGenerator *mDestination = nullptr;
};

OTARequestorAttributes gAttributes;
OTARequestorCommandForwarder gCommandForwarder;
OTAEventForwarder gEventForwarder;
LazyRegisteredServerCluster<OTARequestorCluster> gServer;

void OnSetGlobalOtaRequestorInstance(OTARequestorInterface *instance)
{
    gCommandForwarder.SetDestination(instance);
}

} // namespace

namespace chip {

OTARequestorAttributes &GetOTARequestorAttributes()
{
    return gAttributes;
}

DefaultOTARequestorEventGenerator &GetDefaultOTARequestorEventGenerator()
{
    return gEventForwarder;
}

} // namespace chip

void ESPMatterOtaSoftwareUpdateRequestorClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, OtaSoftwareUpdateRequestor::Id) != nullptr,
                   ChipLogError(AppServer,
                                "OtaSoftwareUpdateRequestor: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));

    if (!gServer.IsConstructed()) {
        gInternalOnSetRequestorInstance = OnSetGlobalOtaRequestorInstance;

        gServer.Create(endpointId, gCommandForwarder, gAttributes, Server::GetInstance().GetFabricTable());
        gEventForwarder.SetDestination(&gServer.Cluster());
    }

    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "OtaSoftwareUpdateRequestor register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterOtaSoftwareUpdateRequestorClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    if (!gServer.IsConstructed()) {
        return;
    }
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "OtaSoftwareUpdateRequestor unregister failed ep %u: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        gEventForwarder.SetDestination(nullptr);
        gServer.Destroy();
    }
}

// MatterOtaSoftwareUpdateRequestorPluginServerInitCallback is defined in esp_matter_ota.cpp
void MatterOtaSoftwareUpdateRequestorPluginServerShutdownCallback() {}
