/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_webrtc_transport_requestor_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace WebRTCTransportRequestor {

CHIP_ERROR MockWebRTCTransportRequestorDelegate::HandleOffer(const WebRTCSessionStruct  &session, const OfferArgs  &args)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockWebRTCTransportRequestorDelegate::HandleAnswer(const WebRTCSessionStruct  &session, const std::string  &sdpAnswer)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockWebRTCTransportRequestorDelegate::HandleICECandidates(const WebRTCSessionStruct  &session,
                                                                     const std::vector<ICECandidateStruct>  &candidates)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockWebRTCTransportRequestorDelegate::HandleEnd(const WebRTCSessionStruct  &session, WebRTCEndReasonEnum reasonCode)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

} // namespace WebRTCTransportRequestor
} // namespace Clusters
} // namespace app
} // namespace chip
