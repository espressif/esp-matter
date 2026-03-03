/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/push-av-stream-transport-server/push-av-stream-transport-delegate.h>
#include <protocols/interaction_model/StatusCode.h>

namespace chip {
namespace app {
namespace Clusters {

class MockPushAvStreamTransportDelegate : public PushAvStreamTransportDelegate {
public:
    MockPushAvStreamTransportDelegate() = default;
    virtual ~MockPushAvStreamTransportDelegate() = default;

    Protocols::InteractionModel::Status
    AllocatePushTransport(const PushAvStreamTransport::Structs::TransportOptionsStruct::Type  &transportOptions,
                          const uint16_t connectionID, FabricIndex accessingFabricIndex) override;

    Protocols::InteractionModel::Status DeallocatePushTransport(const uint16_t connectionID) override;

    Protocols::InteractionModel::Status
    ModifyPushTransport(const uint16_t connectionID,
                        const PushAvStreamTransport::TransportOptionsStorage transportOptions) override;

    Protocols::InteractionModel::Status SetTransportStatus(const std::vector<uint16_t> connectionIDList,
                                                           PushAvStreamTransport::TransportStatusEnum transportStatus) override;

    Protocols::InteractionModel::Status ManuallyTriggerTransport(
        const uint16_t connectionID, PushAvStreamTransport::TriggerActivationReasonEnum activationReason,
        const Optional<PushAvStreamTransport::Structs::TransportMotionTriggerTimeControlStruct::Type>  &timeControl) override;

    bool ValidateStreamUsage(PushAvStreamTransport::StreamUsageEnum streamUsage) override;

    bool ValidateSegmentDuration(uint16_t segmentDuration,
                                 const Optional<DataModel::Nullable<uint16_t>>  &videoStreamId) override;

    Protocols::InteractionModel::Status
    ValidateBandwidthLimit(PushAvStreamTransport::StreamUsageEnum streamUsage,
                           const Optional<DataModel::Nullable<uint16_t>>  &videoStreamId,
                           const Optional<DataModel::Nullable<uint16_t>>  &audioStreamId) override;

    Protocols::InteractionModel::Status SelectVideoStream(PushAvStreamTransport::StreamUsageEnum streamUsage,
                                                          uint16_t  &videoStreamId) override;

    Protocols::InteractionModel::Status SelectAudioStream(PushAvStreamTransport::StreamUsageEnum streamUsage,
                                                          uint16_t  &audioStreamId) override;

    Protocols::InteractionModel::Status SetVideoStream(uint16_t videoStreamId) override;

    Protocols::InteractionModel::Status SetAudioStream(uint16_t audioStreamId) override;

    Protocols::InteractionModel::Status ValidateZoneId(uint16_t zoneId) override;

    bool ValidateMotionZoneListSize(size_t zoneListSize) override;

    PushAvStreamTransport::PushAvStreamTransportStatusEnum GetTransportBusyStatus(const uint16_t connectionID) override;

    void OnAttributeChanged(AttributeId attributeId) override;

    CHIP_ERROR
    LoadCurrentConnections(std::vector<PushAvStreamTransport::TransportConfigurationStorage>  &currentConnections) override;

    CHIP_ERROR PersistentAttributesLoadedCallback() override;

    void SetTLSCerts(Tls::CertificateTable::BufferedClientCert  &clientCertEntry,
                     Tls::CertificateTable::BufferedRootCert  &rootCertEntry) override;

    CHIP_ERROR IsHardPrivacyModeActive(bool  &isActive) override;

    CHIP_ERROR IsSoftRecordingPrivacyModeActive(bool  &isActive) override;

    CHIP_ERROR IsSoftLivestreamPrivacyModeActive(bool  &isActive) override;

    void SetPushAvStreamTransportServer(PushAvStreamTransportServer * server) override;

private:
    static constexpr const char * LOG_TAG = "MockPushAvStreamTransportDelegate";
    PushAvStreamTransportServer * mServer = nullptr;
};

} // namespace Clusters
} // namespace app
} // namespace chip
