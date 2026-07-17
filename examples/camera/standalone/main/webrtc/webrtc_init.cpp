/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "esp_cli.h"
#include "wifi_cli.h"
//  #include "app_storage.h"

#include "app_webrtc.h"
//  #include "kvs_signaling.h"
#include "kvs_peer_connection.h"
#include "esp_webrtc_time.h"
#include "esp_work_queue.h"
#include "media_stream.h"
#include "matter_signaling.h"
#include "camera-device.h"

#include "esp_console.h"

using namespace Camera;

static const char *TAG = "webrtc_init";

static void app_webrtc_event_handler(app_webrtc_event_data_t *event_data, void *user_ctx)
{
    if (event_data == NULL) {
        return;
    }

    switch (event_data->event_id) {
    case APP_WEBRTC_EVENT_INITIALIZED:
        ESP_LOGI(TAG, "[KVS Event] WebRTC Initialized.");
        break;
    case APP_WEBRTC_EVENT_DEINITIALIZING:
        ESP_LOGI(TAG, "[KVS Event] WebRTC Deinitialized.");
        break;
    case APP_WEBRTC_EVENT_SIGNALING_CONNECTING:
        ESP_LOGI(TAG, "[KVS Event] Signaling Connecting.");
        break;
    case APP_WEBRTC_EVENT_SIGNALING_CONNECTED:
        ESP_LOGI(TAG, "[KVS Event] Signaling Connected.");
        break;
    case APP_WEBRTC_EVENT_SIGNALING_DISCONNECTED:
        ESP_LOGI(TAG, "[KVS Event] Signaling Disconnected.");
        break;
    case APP_WEBRTC_EVENT_SIGNALING_DESCRIBE:
        ESP_LOGI(TAG, "[KVS Event] Signaling Describe.");
        break;
    case APP_WEBRTC_EVENT_SIGNALING_GET_ENDPOINT:
        ESP_LOGI(TAG, "[KVS Event] Signaling Get Endpoint.");
        break;
    case APP_WEBRTC_EVENT_SIGNALING_GET_ICE:
        ESP_LOGI(TAG, "[KVS Event] Signaling Get ICE.");
        break;
    case APP_WEBRTC_EVENT_PEER_CONNECTION_REQUESTED:
        ESP_LOGI(TAG, "[KVS Event] Peer Connection Requested.");
        break;
    case APP_WEBRTC_EVENT_PEER_CONNECTED:
        ESP_LOGI(TAG, "[KVS Event] Peer Connected: %s", event_data->peer_id);
        break;
    case APP_WEBRTC_EVENT_PEER_DISCONNECTED:
        ESP_LOGI(TAG, "[KVS Event] Peer Disconnected: %s", event_data->peer_id);
        break;
    case APP_WEBRTC_EVENT_STREAMING_STARTED:
        ESP_LOGI(TAG, "[KVS Event] Streaming Started for Peer: %s", event_data->peer_id);
        break;
    case APP_WEBRTC_EVENT_STREAMING_STOPPED:
        ESP_LOGI(TAG, "[KVS Event] Streaming Stopped for Peer: %s", event_data->peer_id);
        break;
    case APP_WEBRTC_EVENT_RECEIVED_OFFER:
        ESP_LOGI(TAG, "[KVS Event] Received Offer.");
        break;
    case APP_WEBRTC_EVENT_SENT_ANSWER:
        ESP_LOGI(TAG, "[KVS Event] Sent Answer.");
        break;
    case APP_WEBRTC_EVENT_ERROR:
    /* fall-through */
    case APP_WEBRTC_EVENT_SIGNALING_ERROR:
    /* fall-through */
    case APP_WEBRTC_EVENT_PEER_CONNECTION_FAILED:
        ESP_LOGE(TAG, "[KVS Event] Error Event %d: Code %d, Message: %s",
                 (int) event_data->event_id, (int) event_data->status_code, event_data->message);
        break;
    // Add cases for other events if needed
    default:
        ESP_LOGI(TAG, "[KVS Event] Unhandled Event ID: %d", (int) event_data->event_id);
        break;
    }
}

/* Structure to pass peer_id to trigger offer work function */
typedef struct {
    char peer_id[256];
} trigger_offer_work_params_t;

/* Work function to trigger offer in esp_work_queue context (avoids console reentrancy) */
static void trigger_offer_work_fn(void *priv_data)
{
    trigger_offer_work_params_t *params = (trigger_offer_work_params_t *)priv_data;

    if (params == NULL) {
        ESP_LOGE(TAG, "Invalid parameters for trigger offer work function");
        return;
    }

    ESP_LOGI(TAG, "Triggering offer for peer: %s", params->peer_id);

    int result = app_webrtc_trigger_offer(params->peer_id);

    if (result == 0) {
        ESP_LOGI(TAG, "Trigger offer command completed successfully");
    } else {
        ESP_LOGE(TAG, "Trigger offer command failed with code: %d", result);
    }

    /* Free the parameters */
    free(params);
}

/* CLI handler for trigger offer command */
static int trigger_offer_cli_handler(int argc, char *argv[])
{
    if (argc != 2) {
        ESP_LOGE(TAG, "Usage: trigger-offer <peer_id>");
        return -1;
    }

    const char *peer_id = argv[1];

    /* Validate peer_id length */
    if (strlen(peer_id) >= 256) {
        ESP_LOGE(TAG, "Peer ID too long (max 255 characters)");
        return -1;
    }

    /* Allocate parameters for the work function */
    trigger_offer_work_params_t *params = (trigger_offer_work_params_t *)malloc(sizeof(trigger_offer_work_params_t));
    if (params == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for trigger offer work function");
        return -1;
    }

    /* Copy peer_id to work parameters */
    strncpy(params->peer_id, peer_id, sizeof(params->peer_id) - 1);
    params->peer_id[sizeof(params->peer_id) - 1] = '\0';

    /* Enqueue work to esp_work_queue (avoids console reentrancy issues) */
    esp_err_t err = esp_work_queue_add_task(trigger_offer_work_fn, params);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enqueue trigger offer work: %d", err);
        free(params);
        return -1;
    }

    ESP_LOGI(TAG, "Trigger offer work queued for peer: %s", peer_id);
    return 0;
}

/* Register trigger offer CLI command */
static void register_trigger_offer_cli(void)
{
    esp_console_cmd_t cmd = {
        .command = "trigger-offer",
        .help = "Trigger WebRTC offer to a peer. Usage: trigger-offer <peer_id>",
        .hint = "<peer_id>",
        .func = trigger_offer_cli_handler,
    };

    esp_err_t ret = esp_console_cmd_register(&cmd);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register trigger-offer command: %d", ret);
    } else {
        ESP_LOGI(TAG, "Registered trigger-offer command");
    }
}

#ifdef __cplusplus
extern "C" {
#endif

void webrtc_init(void)
{
    esp_err_t ret;
    WEBRTC_STATUS status = WEBRTC_STATUS_SUCCESS;

    ESP_LOGI(TAG, "ESP32 WebRTC Example");

    // Perform the time sync
    //  esp_webrtc_time_sntp_time_sync_and_wait();

    // Register the event callback *before* init to catch all events
    if (app_webrtc_register_event_callback(app_webrtc_event_handler, NULL) != 0) {
        ESP_LOGE(TAG, "Failed to register KVS event callback.");
    }

    // Get the media capture interfaces for sending audio/video
    media_stream_video_capture_t *video_capture = media_stream_get_video_capture_if();
    media_stream_audio_capture_t *audio_capture = media_stream_get_audio_capture_if();
    media_stream_video_player_t *video_player = media_stream_get_video_player_if();
    media_stream_audio_player_t *audio_player = media_stream_get_audio_player_if();

#ifdef CONFIG_ESP_P4_CORE_BOARD
    audio_capture = NULL;
    video_player = NULL;
    audio_player = NULL;
#endif

    if (video_capture == NULL) {
        ESP_LOGW(TAG, "Video capture not available - continuing without video capture");
    }
    if (video_player == NULL) {
        ESP_LOGW(TAG, "Video player not available - continuing without video player");
    }

    if (audio_capture == NULL) {
        ESP_LOGW(TAG, "Audio capture not available - continuing without audio capture");
    }

    if (audio_player == NULL) {
        ESP_LOGW(TAG, "Audio player not available - continuing without audio player");
    }

    // Configure WebRTC app with our new simplified API
    app_webrtc_config_t app_webrtc_config = APP_WEBRTC_CONFIG_DEFAULT();

    // Essential configuration - Matter signaling
    extern CameraDevice gCameraDevice;
    static matter_signaling_config_t matter_signaling_cfg = {0};

    using namespace chip::app::Clusters::WebRTCTransportProvider;
    WebRTCProviderManager* provider_manager =
        static_cast<WebRTCProviderManager*>(&gCameraDevice.GetWebRTCProviderDelegate());
    matter_signaling_cfg.provider_manager = (void*)provider_manager;

    app_webrtc_config.signaling_client_if = getMatterSignalingClientInterface();
    app_webrtc_config.signaling_cfg = &matter_signaling_cfg;

    // Peer connection configuration - for full WebRTC functionality
    app_webrtc_config.peer_connection_if = kvs_peer_connection_if_get();

    // Media interfaces for sending (optional - NULL for signaling-only)
    app_webrtc_config.video_capture = video_capture;
    app_webrtc_config.audio_capture = audio_capture;
    // app_webrtc_config.video_player = video_player;
    // app_webrtc_config.audio_player = audio_player;

    ESP_LOGI(TAG, "Initializing WebRTC Classic Example - Full KVS Integration");
    ESP_LOGI(TAG, "  - Architecture: app_webrtc + kvs_webrtc + kvs_signaling");
    ESP_LOGI(TAG, "  - Signaling: AWS KVS WebSocket signaling");
    ESP_LOGI(TAG, "  - Peer Connections: Full KVS WebRTC peer connections");
    ESP_LOGI(TAG, "  - Role: MASTER (initiates connections)");
    ESP_LOGI(TAG, "  - Trickle ICE: enabled (faster connection setup)");
    ESP_LOGI(TAG, "  - TURN servers: enabled (better NAT traversal)");
    ESP_LOGI(TAG, "  - Audio codec: OPUS, Video codec: H264");
    ESP_LOGI(TAG, "  - Media reception: enabled for bidirectional streaming");

    // Initialize WebRTC application with simplified API
    status = app_webrtc_init(&app_webrtc_config);
    if (status != WEBRTC_STATUS_SUCCESS) {
        ESP_LOGE(TAG, "Failed to initialize WebRTC application: 0x%08" PRIx32, (uint32_t) status);
        return;
    }

    // Example: Advanced configuration APIs (optional)
    // Uncomment any of these if you need to change defaults:
    //
    // app_webrtc_set_role(WEBRTC_CHANNEL_ROLE_TYPE_VIEWER);  // Change to viewer role
    // app_webrtc_set_log_level(2);                                     // Enable DEBUG logging
    app_webrtc_enable_media_reception(true);                         // Enable receiving media
    // app_webrtc_set_ice_config(false, false);                         // Disable trickle ICE and TURN

    ESP_LOGI(TAG, "Running WebRTC application");

    // Run WebRTC application
    status = app_webrtc_run();
    if (status != WEBRTC_STATUS_SUCCESS) {
        ESP_LOGE(TAG, "WebRTC application failed: 0x%08" PRIx32, (uint32_t) status);
        app_webrtc_terminate();
    } else {
        ESP_LOGI(TAG, "WebRTC application started successfully");
    }
}

#ifdef __cplusplus
}
#endif
