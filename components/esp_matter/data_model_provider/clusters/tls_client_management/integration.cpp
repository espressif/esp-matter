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

#include <app/ClusterCallbacks.h>
#include <app/clusters/tls-client-management-server/TLSClientManagementCluster.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server/Server.h>
#include <clusters/TlsClientManagement/Attributes.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/core/CHIPConfig.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::TlsClientManagement;

namespace {

struct EndpointState {
    TLSClientManagementDelegate * delegate = nullptr;
    Tls::CertificateTableImpl * certTable  = nullptr;
    Tls::CertificateTableImpl defaultCertTable;
    LazyRegisteredServerCluster<TLSClientManagementCluster> cluster;
};

std::unordered_map<EndpointId, EndpointState> gState;

} // namespace

void ESPMatterTlsClientManagementClusterServerInitCallback(EndpointId endpointId)
{
    auto it = gState.find(endpointId);
    if (it == gState.end() || it->second.delegate == nullptr) {
        ChipLogError(AppServer, "TlsClientManagement: no delegate set for endpoint %u — call SetDelegate() first",
                     endpointId);
        return;
    }
    auto &state = it->second;

    if (!state.cluster.IsConstructed()) {
        Tls::CertificateTableImpl &certTable = state.certTable ? *state.certTable : state.defaultCertTable;

        LogErrorOnFailure(certTable.SetEndpoint(endpointId));

        uint8_t maxProvisioned = CHIP_CONFIG_TLS_MAX_PROVISIONED_ENDPOINTS;
        esp_matter::read_attribute_raw_value(endpointId, TlsClientManagement::Id, Attributes::MaxProvisioned::Id, maxProvisioned);

        TLSClientManagementCluster::Context context{ Server::GetInstance().GetFabricTable() };
        state.cluster.Create(context, endpointId, *state.delegate, certTable, maxProvisioned);
    }

    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(state.cluster.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "TlsClientManagement: Register failed on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterTlsClientManagementClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gState.find(endpointId);
    VerifyOrReturn(it != gState.end());
    VerifyOrReturn(it->second.cluster.IsConstructed());

    LogErrorOnFailure(
        esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.cluster.Cluster(), shutdownType));
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.cluster.Destroy();
        gState.erase(it);
    }
}

void MatterTlsClientManagementPluginServerInitCallback() {}

namespace chip::app::Clusters::TlsClientManagement {

void SetDelegate(EndpointId endpointId, TLSClientManagementDelegate &delegate)
{
    gState[endpointId].delegate = &delegate;
}

void SetCertificateTable(EndpointId endpointId, Tls::CertificateTableImpl &certificate_table)
{
    gState[endpointId].certTable = &certificate_table;
}

} // namespace chip::app::Clusters::TlsClientManagement
