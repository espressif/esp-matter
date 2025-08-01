/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_webrtc_transport_provider_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace WebRTCTransportProvider {

CHIP_ERROR MockWebRTCTransportProviderDelegate::HandleSolicitOffer(const OfferRequestArgs  &args, WebRTCSessionStruct  &outSession,
                                                                   bool  &outDeferredOffer)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    outDeferredOffer = false;
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockWebRTCTransportProviderDelegate::HandleProvideOffer(const ProvideOfferRequestArgs  &args, WebRTCSessionStruct  &outSession)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockWebRTCTransportProviderDelegate::HandleProvideAnswer(uint16_t sessionId, const std::string  &sdpAnswer)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockWebRTCTransportProviderDelegate::HandleProvideICECandidates(uint16_t sessionId, const std::vector<ICECandidateStruct>  &candidates)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockWebRTCTransportProviderDelegate::HandleEndSession(uint16_t sessionId, WebRTCEndReasonEnum reasonCode,
                                                                 DataModel::Nullable<uint16_t> videoStreamID,
                                                                 DataModel::Nullable<uint16_t> audioStreamID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockWebRTCTransportProviderDelegate::ValidateStreamUsage(StreamUsageEnum streamUsage,
                                                                    Optional<DataModel::Nullable<uint16_t>>  &videoStreamId,
                                                                    Optional<DataModel::Nullable<uint16_t>>  &audioStreamId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

} // namespace WebRTCTransportProvider
} // namespace Clusters
} // namespace app
} // namespace chip