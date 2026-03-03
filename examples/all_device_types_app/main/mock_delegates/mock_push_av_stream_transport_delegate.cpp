/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_push_av_stream_transport_delegate.h"

namespace chip {
namespace app {
namespace Clusters {

Protocols::InteractionModel::Status MockPushAvStreamTransportDelegate::AllocatePushTransport(
    const PushAvStreamTransport::Structs::TransportOptionsStruct::Type  &transportOptions, const uint16_t connectionID,
    FabricIndex accessingFabricIndex)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockPushAvStreamTransportDelegate::DeallocatePushTransport(const uint16_t connectionID)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockPushAvStreamTransportDelegate::ModifyPushTransport(
    const uint16_t connectionID, const PushAvStreamTransport::TransportOptionsStorage transportOptions)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status
MockPushAvStreamTransportDelegate::SetTransportStatus(const std::vector<uint16_t> connectionIDList,
                                                      PushAvStreamTransport::TransportStatusEnum transportStatus)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockPushAvStreamTransportDelegate::ManuallyTriggerTransport(
    const uint16_t connectionID, PushAvStreamTransport::TriggerActivationReasonEnum activationReason,
    const Optional<PushAvStreamTransport::Structs::TransportMotionTriggerTimeControlStruct::Type>  &timeControl)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

bool MockPushAvStreamTransportDelegate::ValidateStreamUsage(PushAvStreamTransport::StreamUsageEnum streamUsage)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockPushAvStreamTransportDelegate::ValidateSegmentDuration(uint16_t segmentDuration,
                                                                const Optional<DataModel::Nullable<uint16_t>>  &videoStreamId)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

Protocols::InteractionModel::Status
MockPushAvStreamTransportDelegate::ValidateBandwidthLimit(PushAvStreamTransport::StreamUsageEnum streamUsage,
                                                          const Optional<DataModel::Nullable<uint16_t>>  &videoStreamId,
                                                          const Optional<DataModel::Nullable<uint16_t>>  &audioStreamId)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status
MockPushAvStreamTransportDelegate::SelectVideoStream(PushAvStreamTransport::StreamUsageEnum streamUsage, uint16_t  &videoStreamId)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    videoStreamId = 0;
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status
MockPushAvStreamTransportDelegate::SelectAudioStream(PushAvStreamTransport::StreamUsageEnum streamUsage, uint16_t  &audioStreamId)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    audioStreamId = 0;
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockPushAvStreamTransportDelegate::SetVideoStream(uint16_t videoStreamId)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockPushAvStreamTransportDelegate::SetAudioStream(uint16_t audioStreamId)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockPushAvStreamTransportDelegate::ValidateZoneId(uint16_t zoneId)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

bool MockPushAvStreamTransportDelegate::ValidateMotionZoneListSize(size_t zoneListSize)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

PushAvStreamTransport::PushAvStreamTransportStatusEnum
MockPushAvStreamTransportDelegate::GetTransportBusyStatus(const uint16_t connectionID)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return PushAvStreamTransport::PushAvStreamTransportStatusEnum::kIdle;
}

void MockPushAvStreamTransportDelegate::OnAttributeChanged(AttributeId attributeId)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

CHIP_ERROR MockPushAvStreamTransportDelegate::LoadCurrentConnections(
    std::vector<PushAvStreamTransport::TransportConfigurationStorage>  &currentConnections)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockPushAvStreamTransportDelegate::PersistentAttributesLoadedCallback()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

void MockPushAvStreamTransportDelegate::SetTLSCerts(Tls::CertificateTable::BufferedClientCert  &clientCertEntry,
                                                    Tls::CertificateTable::BufferedRootCert  &rootCertEntry)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

CHIP_ERROR MockPushAvStreamTransportDelegate::IsHardPrivacyModeActive(bool  &isActive)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    isActive = false;
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockPushAvStreamTransportDelegate::IsSoftRecordingPrivacyModeActive(bool  &isActive)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    isActive = false;
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockPushAvStreamTransportDelegate::IsSoftLivestreamPrivacyModeActive(bool  &isActive)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    isActive = false;
    return CHIP_NO_ERROR;
}

void MockPushAvStreamTransportDelegate::SetPushAvStreamTransportServer(PushAvStreamTransportServer * server)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    mServer = server;
    return;
}

} // namespace Clusters
} // namespace app
} // namespace chip
