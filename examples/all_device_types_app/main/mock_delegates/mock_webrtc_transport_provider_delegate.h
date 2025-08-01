/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/webrtc-transport-provider-server/WebRTCTransportProviderCluster.h>

/*
 * Mock WebRTCTransportProvider Delegate Implementation
 * This file provides a mock implementation of the WebRTCTransportProvider::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/webrtc-transport-provider-server/webrtc-transport-provider-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/camera-app/linux/include/clusters/webrtc_provider/webrtc-provider-manager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/camera-app/linux/src/clusters/webrtc_provider/webrtc-provider-manager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace WebRTCTransportProvider {

class MockWebRTCTransportProviderDelegate : public Delegate {
public:
    MockWebRTCTransportProviderDelegate() = default;
    virtual ~MockWebRTCTransportProviderDelegate() = default;

    // Delegate interface
    CHIP_ERROR HandleSolicitOffer(const OfferRequestArgs  &args, WebRTCSessionStruct  &outSession,
                                  bool  &outDeferredOffer) override;

    CHIP_ERROR HandleProvideOffer(const ProvideOfferRequestArgs  &args, WebRTCSessionStruct  &outSession) override;

    CHIP_ERROR HandleProvideAnswer(uint16_t sessionId, const std::string  &sdpAnswer) override;

    CHIP_ERROR HandleProvideICECandidates(uint16_t sessionId, const std::vector<ICECandidateStruct>  &candidates) override;

    CHIP_ERROR HandleEndSession(uint16_t sessionId, WebRTCEndReasonEnum reasonCode,
                                DataModel::Nullable<uint16_t> videoStreamID,
                                DataModel::Nullable<uint16_t> audioStreamID) override;

    CHIP_ERROR ValidateStreamUsage(StreamUsageEnum streamUsage,
                                   Optional<DataModel::Nullable<uint16_t>>  &videoStreamId,
                                   Optional<DataModel::Nullable<uint16_t>>  &audioStreamId) override;

private:
    static constexpr const char * LOG_TAG = "MockWebRTCTransportProviderDelegate";
};

} // namespace WebRTCTransportProvider
} // namespace Clusters
} // namespace app
} // namespace chip