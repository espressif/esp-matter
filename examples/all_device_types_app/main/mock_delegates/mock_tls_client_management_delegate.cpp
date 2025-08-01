/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_tls_client_management_delegate.h"

namespace chip {
namespace app {
namespace Clusters {

CHIP_ERROR MockTlsClientManagementDelegate::Init(PersistentStorageDelegate  &storage)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockTlsClientManagementDelegate::ForEachEndpoint(EndpointId matterEndpoint, FabricIndex fabric,
                                                            LoadedEndpointCallback callback)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockTlsClientManagementDelegate::FindProvisionedEndpointByID(EndpointId matterEndpoint, FabricIndex fabric,
                                                                        uint16_t endpointID, LoadedEndpointCallback callback)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

Protocols::InteractionModel::ClusterStatusCode MockTlsClientManagementDelegate::ProvisionEndpoint(
    EndpointId matterEndpoint, FabricIndex fabric,
    const TlsClientManagement::Commands::ProvisionEndpoint::DecodableType  &provisionReq, uint16_t  &endpointID)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    endpointID = 0;
    return Protocols::InteractionModel::ClusterStatusCode(Protocols::InteractionModel::Status::Success);
}

Protocols::InteractionModel::Status MockTlsClientManagementDelegate::RemoveProvisionedEndpointByID(EndpointId matterEndpoint,
                                                                                                   FabricIndex fabric,
                                                                                                   uint16_t endpointID)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

void MockTlsClientManagementDelegate::RemoveFabric(FabricIndex fabricIndex)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

CHIP_ERROR MockTlsClientManagementDelegate::MutateEndpointReferenceCount(EndpointId matterEndpoint, FabricIndex fabric,
                                                                         uint16_t endpointID, int8_t delta)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockTlsClientManagementDelegate::RootCertCanBeRemoved(EndpointId matterEndpoint, FabricIndex fabric, Tls::TLSCAID id)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockTlsClientManagementDelegate::ClientCertCanBeRemoved(EndpointId matterEndpoint, FabricIndex fabric, Tls::TLSCCDID id)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

} // namespace Clusters
} // namespace app
} // namespace chip
