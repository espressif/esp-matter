/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/tls-certificate-management-server/TlsCertificateManagementCluster.h>
#include <protocols/interaction_model/StatusCode.h>

namespace chip {
namespace app {
namespace Clusters {

class MockTlsCertificateManagementDelegate : public TlsCertificateManagementDelegate {
public:
    MockTlsCertificateManagementDelegate() = default;
    virtual ~MockTlsCertificateManagementDelegate() = default;

    Protocols::InteractionModel::Status ProvisionRootCert(EndpointId matterEndpoint, FabricIndex fabric,
                                                          const ProvisionRootCertificateType  &provisionReq,
                                                          Tls::TLSCAID  &outCaid) override;

    CHIP_ERROR LoadedRootCerts(EndpointId matterEndpoint, FabricIndex fabric,
                               LoadedRootCertificateCallback loadedCallback) const override;

    CHIP_ERROR RootCertsForFabric(EndpointId matterEndpoint, FabricIndex fabric,
                                  RootCertificateListCallback loadedCallback) const override;

    CHIP_ERROR FindRootCert(EndpointId matterEndpoint, FabricIndex fabric, Tls::TLSCAID id,
                            LoadedRootCertificateCallback loadedCallback) const override;

    CHIP_ERROR LookupRootCertByFingerprint(EndpointId matterEndpoint, FabricIndex fabric, const ByteSpan  &fingerprint,
                                           LoadedRootCertificateCallback loadedCallback) const override;

    CHIP_ERROR LookupRootCert(EndpointId matterEndpoint, FabricIndex fabric, const ByteSpan  &cert,
                              LoadedRootCertificateCallback loadedCallback) const override;

    Protocols::InteractionModel::Status RemoveRootCert(EndpointId matterEndpoint, FabricIndex fabric, Tls::TLSCAID id) override;

    Protocols::InteractionModel::Status GenerateClientCsr(EndpointId matterEndpoint, FabricIndex fabric,
                                                          const ClientCsrType  &request,
                                                          GeneratedCsrCallback loadedCallback) const override;

    Protocols::InteractionModel::Status ProvisionClientCert(EndpointId matterEndpoint, FabricIndex fabric,
                                                            const ProvisionClientCertificateType  &provisionReq) override;

    CHIP_ERROR LoadedClientCerts(EndpointId matterEndpoint, FabricIndex fabric,
                                 LoadedClientCertificateCallback loadedCallback) const override;

    CHIP_ERROR ClientCertsForFabric(EndpointId matterEndpoint, FabricIndex fabric,
                                    ClientCertificateListCallback loadedCallback) const override;

    CHIP_ERROR FindClientCert(EndpointId matterEndpoint, FabricIndex fabric, Tls::TLSCCDID id,
                              LoadedClientCertificateCallback loadedCallback) const override;

    CHIP_ERROR LookupClientCertByFingerprint(EndpointId matterEndpoint, FabricIndex fabric, const ByteSpan  &fingerprint,
                                             LoadedClientCertificateCallback loadedCallback) const override;

    CHIP_ERROR LookupClientCert(EndpointId matterEndpoint, FabricIndex fabric, const ByteSpan  &certificate,
                                LoadedClientCertificateCallback loadedCallback) const override;

    Protocols::InteractionModel::Status RemoveClientCert(EndpointId matterEndpoint, FabricIndex fabric, Tls::TLSCCDID id) override;

private:
    static constexpr const char * LOG_TAG = "MockTlsCertificateManagementDelegate";
};

} // namespace Clusters
} // namespace app
} // namespace chip
