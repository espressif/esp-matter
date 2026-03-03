/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_tls_certificate_management_delegate.h"

namespace chip {
namespace app {
namespace Clusters {

Protocols::InteractionModel::Status
MockTlsCertificateManagementDelegate::ProvisionRootCert(EndpointId matterEndpoint, FabricIndex fabric,
                                                        const ProvisionRootCertificateType  &provisionReq, Tls::TLSCAID  &outCaid)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    outCaid = 0;
    return Protocols::InteractionModel::Status::Success;
}

CHIP_ERROR MockTlsCertificateManagementDelegate::LoadedRootCerts(EndpointId matterEndpoint, FabricIndex fabric,
                                                                 LoadedRootCertificateCallback loadedCallback) const
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockTlsCertificateManagementDelegate::RootCertsForFabric(EndpointId matterEndpoint, FabricIndex fabric,
                                                                    RootCertificateListCallback loadedCallback) const
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockTlsCertificateManagementDelegate::FindRootCert(EndpointId matterEndpoint, FabricIndex fabric, Tls::TLSCAID id,
                                                              LoadedRootCertificateCallback loadedCallback) const
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR MockTlsCertificateManagementDelegate::LookupRootCertByFingerprint(EndpointId matterEndpoint, FabricIndex fabric,
                                                                             const ByteSpan  &fingerprint,
                                                                             LoadedRootCertificateCallback loadedCallback) const
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR MockTlsCertificateManagementDelegate::LookupRootCert(EndpointId matterEndpoint, FabricIndex fabric,
                                                                const ByteSpan  &cert,
                                                                LoadedRootCertificateCallback loadedCallback) const
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

Protocols::InteractionModel::Status MockTlsCertificateManagementDelegate::RemoveRootCert(EndpointId matterEndpoint,
                                                                                         FabricIndex fabric, Tls::TLSCAID id)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockTlsCertificateManagementDelegate::GenerateClientCsr(EndpointId matterEndpoint,
                                                                                            FabricIndex fabric,
                                                                                            const ClientCsrType  &request,
                                                                                            GeneratedCsrCallback loadedCallback) const
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Failure;
}

Protocols::InteractionModel::Status
MockTlsCertificateManagementDelegate::ProvisionClientCert(EndpointId matterEndpoint, FabricIndex fabric,
                                                          const ProvisionClientCertificateType  &provisionReq)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

CHIP_ERROR MockTlsCertificateManagementDelegate::LoadedClientCerts(EndpointId matterEndpoint, FabricIndex fabric,
                                                                   LoadedClientCertificateCallback loadedCallback) const
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockTlsCertificateManagementDelegate::ClientCertsForFabric(EndpointId matterEndpoint, FabricIndex fabric,
                                                                      ClientCertificateListCallback loadedCallback) const
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockTlsCertificateManagementDelegate::FindClientCert(EndpointId matterEndpoint, FabricIndex fabric, Tls::TLSCCDID id,
                                                                LoadedClientCertificateCallback loadedCallback) const
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR MockTlsCertificateManagementDelegate::LookupClientCertByFingerprint(EndpointId matterEndpoint, FabricIndex fabric,
                                                                               const ByteSpan  &fingerprint,
                                                                               LoadedClientCertificateCallback loadedCallback) const
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR MockTlsCertificateManagementDelegate::LookupClientCert(EndpointId matterEndpoint, FabricIndex fabric,
                                                                  const ByteSpan  &certificate,
                                                                  LoadedClientCertificateCallback loadedCallback) const
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

Protocols::InteractionModel::Status MockTlsCertificateManagementDelegate::RemoveClientCert(EndpointId matterEndpoint,
                                                                                           FabricIndex fabric, Tls::TLSCCDID id)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

} // namespace Clusters
} // namespace app
} // namespace chip
