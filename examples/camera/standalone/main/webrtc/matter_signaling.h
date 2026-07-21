/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __MATTER_SIGNALING_H__
#define __MATTER_SIGNALING_H__

#include "app_webrtc_if.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Configuration for Matter signaling client
 */
typedef struct {
    // Pointer to WebRTCProviderManager instance (C++ object)
    void *provider_manager;
} matter_signaling_config_t;

/**
 * @brief Get the Matter signaling client interface
 *
 * This returns a pointer to the Matter signaling interface that can be used
 * with app_webrtc_init to enable Matter-based signaling.
 *
 * @return Pointer to the Matter signaling client interface
 */
webrtc_signaling_client_if_t* getMatterSignalingClientInterface(void);

/**
 * @brief Handle offer received from Matter controller
 *
 * This function is called by WebRTCProviderManager when an offer is received
 * from the Matter controller. It converts the Matter offer to a WebRTC message
 * and delivers it to the signaling client.
 *
 * @param sessionId Matter session ID
 * @param sdpOffer SDP offer string
 * @return WEBRTC_STATUS code
 */
WEBRTC_STATUS matter_signaling_handle_offer(uint16_t sessionId, const char *sdpOffer);

/**
 * @brief Handle answer received from Matter controller
 *
 * This function is called by WebRTCProviderManager when an answer is received
 * from the Matter controller. It converts the Matter answer to a WebRTC message
 * and delivers it to the signaling client.
 *
 * @param sessionId Matter session ID
 * @param sdpAnswer SDP answer string
 * @return WEBRTC_STATUS code
 */
WEBRTC_STATUS matter_signaling_handle_answer(uint16_t sessionId, const char *sdpAnswer);

/**
 * @brief Handle ICE candidate received from Matter controller
 *
 * This function is called by WebRTCProviderManager when ICE candidates are received
 * from the Matter controller. It converts the Matter ICE candidates to WebRTC messages
 * and delivers them to the signaling client.
 *
 * @param sessionId Matter session ID
 * @param candidate ICE candidate string
 * @param mid SDP mid string (optional)
 * @return WEBRTC_STATUS code
 */
WEBRTC_STATUS matter_signaling_handle_ice_candidate(uint16_t sessionId, const char *candidate, const char *mid);

/**
 * @brief Send WebRTC message via Matter WebRTCProviderManager (C++ bridge)
 *
 * This is a C++ bridge function that allows the C-based matter_signaling.cpp
 * to call C++ methods on WebRTCProviderManager.
 *
 * @param provider_manager_ptr Pointer to WebRTCProviderManager instance
 * @param sessionId Matter session ID
 * @param pMessage WebRTC message to send
 * @return WEBRTC_STATUS code
 */
WEBRTC_STATUS matter_signaling_send_message_via_matter(
    void *provider_manager_ptr,
    uint16_t sessionId,
    webrtc_message_t *pMessage);

#ifdef __cplusplus
}
#endif

#endif /* __MATTER_SIGNALING_H__ */
