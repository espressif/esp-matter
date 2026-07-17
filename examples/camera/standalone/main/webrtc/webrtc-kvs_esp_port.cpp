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

#include "webrtc-kvs_esp_port.h"
#include "camera-device.h"
#include "webrtc-kvs_esp_port_utils.h"
#include "webrtc-transport.h"
#include "matter_signaling.h"
#include "app_webrtc.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::WebRTCTransportProvider;

using namespace Camera;

extern CameraDevice gCameraDevice;

KVSWebRTCPeerConnection::KVSWebRTCPeerConnection()
{
    std::string peerConnectionId = generateMonotonicPeerConnectionId();
    mPeerConnection              = std::make_shared<EspWebRTCPeerConnection>(peerConnectionId);
}

void KVSWebRTCPeerConnection::SetCallbacks(OnLocalDescriptionCallback onLocalDescription, OnICECandidateCallback onICECandidate,
                                           OnConnectionStateCallback onConnectionState, OnTrackCallback onTrack)
{
    // Received messages are forwarded using webrtc_bridge_send_message
}

void KVSWebRTCPeerConnection::Close()
{
    // KVSWebRTC close is handled by the KVSWebRTCManager.
}

void KVSWebRTCPeerConnection::CreateOffer(uint16_t sessionId)
{
    // Set local description in KVSWebRTC
    printf("CreateOffer: %u\n", sessionId);
    std::string peerConnectionId = this->GetPeerConnectionId();
    char peerClientId[APP_WEBRTC_MAX_SIGNALING_CLIENT_ID_LEN];
    snprintf(peerClientId, sizeof(peerClientId), "%s", peerConnectionId.c_str());
    int status = app_webrtc_trigger_offer(peerClientId);
    if (status != 0) {
        ChipLogError(Camera, "Failed to trigger offer for peer client ID: %s: %d", peerClientId, status);
        return;
    }
}

void KVSWebRTCPeerConnection::CreateAnswer()
{
    // Answer is received from KVSWebRTCManager when offer is sent and received
    // via webrtc_bridge.
}

void KVSWebRTCPeerConnection::SetRemoteDescription(const std::string  &sdp, SDPType type)
{
    // handles SDP Offer received from webrtc requestor.
    // Send SDP to KVSWebRTCManager.
    printf("SetRemoteDescription: \n%s\n", sdp.c_str());

    std::string sdp_str = std::string(sdp.begin(), sdp.end());
    std::string peerConnectionId = this->GetPeerConnectionId();
    uint16_t sessionId = static_cast<uint16_t>(std::stoi(peerConnectionId, nullptr, 16));
    if (type == SDPType::Offer) {
        WEBRTC_STATUS status = matter_signaling_handle_offer(sessionId, sdp_str.c_str());
        if (status != WEBRTC_STATUS_SUCCESS) {
            ChipLogError(Camera, "Failed to deliver offer to Matter signaling: 0x%08" PRIx32, (uint32_t)status);
            return;
        }
    } else if (type == SDPType::Answer) {
        WEBRTC_STATUS status = matter_signaling_handle_answer(sessionId, sdp_str.c_str());
        if (status != WEBRTC_STATUS_SUCCESS) {
            ChipLogError(Camera, "Failed to deliver answer to Matter signaling: 0x%08" PRIx32, (uint32_t)status);
            return;
        }
    } else {
        ChipLogError(Camera, "Invalid SDP type: %d", (int)type);
        return;
    }
}

void KVSWebRTCPeerConnection::AddRemoteCandidate(const std::string  &candidate, const std::string  &mid)
{

    // Send webrtc requestor's candidates to KVSWebRTCManager.
    printf("AddRemoteCandidate: %s\n", candidate.c_str());

    std::string candidate_str = std::string(candidate.begin(), candidate.end());
    std::string peerConnectionId = this->GetPeerConnectionId();
    uint16_t sessionId = static_cast<uint16_t>(std::stoi(peerConnectionId, nullptr, 16));
    WEBRTC_STATUS status = matter_signaling_handle_ice_candidate(sessionId, candidate_str.c_str(), mid.c_str());
    if (status != WEBRTC_STATUS_SUCCESS) {
        ChipLogError(Camera, "Failed to deliver ICE candidate to Matter signaling: 0x%08" PRIx32, (uint32_t)status);
        // Continue with existing flow even if signaling delivery fails
    }
}

std::shared_ptr<WebRTCTrack> KVSWebRTCPeerConnection::AddTrack(MediaType mediaType)
{
    // Addition of tracks is handled by the KVSWebRTCManager.
    return nullptr;
}
std::string KVSWebRTCPeerConnection::GetPeerConnectionId()
{
    return mPeerConnection->GetPeerConnectionId();
}

std::shared_ptr<WebRTCPeerConnection> CreateWebRTCPeerConnection()
{
    return std::make_shared<KVSWebRTCPeerConnection>();
}