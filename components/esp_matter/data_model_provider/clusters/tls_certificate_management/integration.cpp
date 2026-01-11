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
#include <app/clusters/tls-certificate-management-server/CertificateTableImpl.h>
#include <app/clusters/tls-certificate-management-server/TLSCertificateManagementCluster.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server/Server.h>
#include <clusters/TlsCertificateManagement/Attributes.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/core/CHIPConfig.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::TlsCertificateManagement;

namespace {

struct EndpointState {
    TLSCertificateManagementDelegate * delegate              = nullptr;
    Tls::CertificateDependencyChecker * dependencyChecker    = nullptr;
    Tls::CertificateTableImpl * certTable                    = nullptr;
    Tls::CertificateTableImpl defaultCertTable;
    LazyRegisteredServerCluster<TLSCertificateManagementCluster> cluster;
};

std::unordered_map<EndpointId, EndpointState> gState;

} // namespace

void ESPMatterTlsCertificateManagementClusterServerInitCallback(EndpointId endpointId)
{
    auto it = gState.find(endpointId);
    if (it == gState.end() || it->second.delegate == nullptr) {
        ChipLogError(AppServer, "TlsCertificateManagement: no delegate set for endpoint %u — call SetDelegate() first",
                     endpointId);
        return;
    }
    if (it->second.dependencyChecker == nullptr) {
        ChipLogError(AppServer,
                     "TlsCertificateManagement: no dependency checker set for endpoint %u — call SetDependencyChecker() first",
                     endpointId);
        return;
    }
    auto &state = it->second;

    if (!state.cluster.IsConstructed()) {
        Tls::CertificateTableImpl &certTable = state.certTable ? *state.certTable : state.defaultCertTable;

        LogErrorOnFailure(certTable.SetEndpoint(endpointId));

        uint8_t maxRootCerts   = static_cast<uint8_t>(Tls::kMaxRootCertificatesPerFabric);
        uint8_t maxClientCerts = static_cast<uint8_t>(Tls::kMaxClientCertificatesPerFabric);
        esp_matter::read_attribute_raw_value(endpointId, TlsCertificateManagement::Id,
                                             Attributes::MaxRootCertificates::Id, maxRootCerts);
        esp_matter::read_attribute_raw_value(endpointId, TlsCertificateManagement::Id,
                                             Attributes::MaxClientCertificates::Id, maxClientCerts);

        TLSCertificateManagementCluster::Context context{ Server::GetInstance().GetFabricTable() };
        state.cluster.Create(context, endpointId, *state.delegate, *state.dependencyChecker, certTable,
                             maxRootCerts, maxClientCerts);
    }

    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(state.cluster.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "TlsCertificateManagement: Register failed on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterTlsCertificateManagementClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
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

void MatterTlsCertificateManagementPluginServerInitCallback() {}

namespace chip::app::Clusters::TlsCertificateManagement {

void SetDelegate(EndpointId endpointId, TLSCertificateManagementDelegate &delegate)
{
    gState[endpointId].delegate = &delegate;
}

void SetDependencyChecker(EndpointId endpointId, Tls::CertificateDependencyChecker &checker)
{
    gState[endpointId].dependencyChecker = &checker;
}

void SetCertificateTable(EndpointId endpointId, Tls::CertificateTableImpl &certificate_table)
{
    gState[endpointId].certTable = &certificate_table;
}

} // namespace chip::app::Clusters::TlsCertificateManagement
