/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "matter_signaling.h"
#include "app_webrtc_if.h"
#include "app_webrtc.h"
#include "esp_log.h"
#include "esp_work_queue.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/idf_additions.h>
#include <freertos/queue.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <new>
#include <map>
#include <string>
#include "camera-device.h"

// Forward declarations for C++ types
using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::WebRTCTransportProvider;
using namespace Camera;

static const char *TAG = "matter_signaling";

extern CameraDevice gCameraDevice;
/**
 * @brief JSON-escape a string into a caller-provided buffer, or count the
 *        required length when @p out is NULL.
 *
 * Escapes: " \\ \b \f \n \r \t and control characters < 0x20. Returns the
 * number of bytes written (excluding the NUL terminator, which is not added).
 *
 * Used to build the KVS JSON payload directly into its final heap buffer, so
 * the (potentially multi-KB) SDP is never copied into intermediate std::string
 * temporaries. That matters here: the default heap is internal-RAM only, and a
 * throwing operator new failure aborts the process (C++ exceptions are off).
 */
static size_t json_escape_into(const char *input, char *out)
{
    size_t n = 0;
    if (!input) {
        return 0;
    }
    for (const char *p = input; *p; p++) {
        unsigned char c = (unsigned char) * p;
        const char *esc = nullptr;
        char ubuf[7];
        switch (c) {
        case '"':  esc = "\\\""; break;
        case '\\': esc = "\\\\"; break;
        case '\b': esc = "\\b";  break;
        case '\f': esc = "\\f";  break;
        case '\n': esc = "\\n";  break;
        case '\r': esc = "\\r";  break;
        case '\t': esc = "\\t";  break;
        default:
            if (c < 0x20) {
                snprintf(ubuf, sizeof(ubuf), "\\u%04x", c);
                esc = ubuf;
            }
            break;
        }
        if (esc) {
            size_t l = strlen(esc);
            if (out) {
                memcpy(out + n, esc, l);
            }
            n += l;
        } else {
            if (out) {
                out[n] = (char) c;
            }
            n += 1;
        }
    }
    return n;
}

/**
 * @brief Build "<prefix><JSON-escaped value><suffix>" into a single malloc'd,
 *        NUL-terminated buffer.
 *
 * Performs exactly one heap allocation (the buffer the caller hands to KVS as
 * webrtc_message_t::payload) and no std::string temporaries. On success returns
 * the buffer (caller frees) and sets @p out_len to the length excluding NUL.
 */
static char *build_json_payload(const char *prefix, const char *value, const char *suffix, size_t *out_len)
{
    size_t prefix_len = strlen(prefix);
    size_t suffix_len = strlen(suffix);
    size_t esc_len    = json_escape_into(value, nullptr);
    size_t total      = prefix_len + esc_len + suffix_len;

    char *buf = (char *) malloc(total + 1);
    if (buf == nullptr) {
        return nullptr;
    }
    memcpy(buf, prefix, prefix_len);
    json_escape_into(value, buf + prefix_len);
    memcpy(buf + prefix_len + esc_len, suffix, suffix_len);
    buf[total] = '\0';

    if (out_len) {
        *out_len = total;
    }
    return buf;
}

// Map to convert between Matter sessionId and WebRTC peer_client_id
// struct SessionMapping {
//     uint16_t sessionId;
//     char peerClientId[APP_WEBRTC_MAX_SIGNALING_CLIENT_ID_LEN];
// };

// Progressive ICE callback context
typedef struct {
    uint64_t customData;
    WEBRTC_STATUS(*on_ice_servers_updated)(uint64_t, uint32_t);
} matter_ice_callback_ctx_t;

/**
 * @brief Matter signaling client data structure
 */
typedef struct {
    // Configuration
    matter_signaling_config_t config;
    bool initialized;
    webrtc_signaling_state_t state;

    // Callbacks matching webrtc_signaling_client_if_t
    WEBRTC_STATUS(*on_msg_received)(uint64_t, webrtc_message_t*);
    WEBRTC_STATUS(*on_signaling_state_changed)(uint64_t, webrtc_signaling_state_t);
    WEBRTC_STATUS(*on_error)(uint64_t, WEBRTC_STATUS, char*, uint32_t);
    uint64_t user_data;

    // Session mapping: sessionId -> peerClientId
    // std::map<uint16_t, std::string> session_to_peer_map;
    // std::map<std::string, uint16_t> peer_to_session_map;
    // uint16_t next_session_id;

    // Progressive ICE server callback context
    matter_ice_callback_ctx_t ice_callback_ctx;
} MatterSignalingClientData;

// Global client instance
static MatterSignalingClientData* g_matter_client = NULL;

// Dedicated task for WebRTC callbacks with large stack
static QueueHandle_t g_webrtc_callback_queue = NULL;
static TaskHandle_t g_webrtc_callback_task = NULL;

// Work parameters for callback queue
struct callback_work_params {
    webrtc_message_t msg;
    uint64_t user_data;
    WEBRTC_STATUS(*on_msg_received)(uint64_t, webrtc_message_t*);
};

// WebRTC callback task - runs with large stack to handle deep call chains
static void webrtc_callback_task(void *pvParameters)
{
    callback_work_params params;
    ESP_LOGI(TAG, "WebRTC callback task started");

    while (1) {
        if (xQueueReceive(g_webrtc_callback_queue, &params, portMAX_DELAY) == pdTRUE) {
            if (params.on_msg_received) {
                params.on_msg_received(params.user_data, &params.msg);
            }
            // Free the payload
            if (params.msg.payload) {
                free(params.msg.payload);
            }
        }
    }
}

// Initialize the WebRTC callback task
static esp_err_t init_webrtc_callback_task(void)
{
    if (g_webrtc_callback_queue != NULL) {
        return ESP_OK; // Already initialized
    }

    // Create queue for callback work
    g_webrtc_callback_queue = xQueueCreate(10, sizeof(callback_work_params));
    if (g_webrtc_callback_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create WebRTC callback queue");
        return ESP_FAIL;
    }

    // Create task with a 16KB stack allocated in SPIRAM for WebRTC operations
    BaseType_t ret = xTaskCreateWithCaps(
                         webrtc_callback_task,
                         "webrtc_cb_task",
                         24 * 1024,  // 16KB stack
                         NULL,
                         5,  // Priority
                         &g_webrtc_callback_task,
                         MALLOC_CAP_SPIRAM
                     );

    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create WebRTC callback task");
        vQueueDelete(g_webrtc_callback_queue);
        g_webrtc_callback_queue = NULL;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "WebRTC callback task initialized with 24KB stack in SPIRAM");
    return ESP_OK;
}

/**
 * @brief Convert sessionId to peer_client_id string
 */
static void session_id_to_peer_id(uint16_t sessionId, char *peerId, size_t peerIdLen)
{
    snprintf(peerId, peerIdLen, "%u", sessionId);
}

/**
 * @brief Convert peer_client_id string to sessionId
 */
static uint16_t peer_id_to_session_id(const char *peerId)
{
    if (peerId == NULL) {
        return 0;
    }
    return (uint16_t)atoi(peerId);
}

/**
 * @brief Matter signaling init implementation
 */
static WEBRTC_STATUS matterInit(void *signaling_cfg, void **ppSignalingClient)
{
    ESP_LOGI(TAG, "Initializing Matter signaling client");

    if (signaling_cfg == NULL || ppSignalingClient == NULL) {
        return WEBRTC_STATUS_NULL_ARG;
    }

    matter_signaling_config_t* config = (matter_signaling_config_t*)signaling_cfg;
    WebRTCTransportProvider::Delegate  &delegateRef = gCameraDevice.GetWebRTCProviderDelegate();
    WebRTCProviderManager * webrtcMgr                                = static_cast<WebRTCProviderManager *>(&delegateRef);
    config->provider_manager = webrtcMgr;
    // Allocate client data using new to ensure C++ constructors are called for std::map
    MatterSignalingClientData* client_data = new (std::nothrow) MatterSignalingClientData();
    if (client_data == NULL) {
        return WEBRTC_STATUS_NOT_ENOUGH_MEMORY;
    }

    // Initialize the structure (std::map members are already initialized by their constructors)
    client_data->config = *config;
    client_data->initialized = true;
    // Matter signaling is always "connected" - it doesn't have a connection state like KVS
    // The transport is ready to send messages immediately after initialization
    client_data->state = WEBRTC_SIGNALING_STATE_CONNECTED;
    // client_data->next_session_id = 1;
    client_data->on_msg_received = NULL;
    client_data->on_signaling_state_changed = NULL;
    client_data->on_error = NULL;
    client_data->user_data = 0;

    // Set global client reference
    g_matter_client = client_data;

    // Initialize the dedicated callback task
    if (init_webrtc_callback_task() != ESP_OK) {
        delete client_data;
        g_matter_client = NULL;
        return WEBRTC_STATUS_INTERNAL_ERROR;
    }

    // Return the client data through the output parameter
    *ppSignalingClient = (void*)client_data;

    ESP_LOGI(TAG, "Matter signaling client initialized");
    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Matter signaling connect implementation
 */
static WEBRTC_STATUS matterConnect(void *pSignalingClient)
{
    ESP_LOGI(TAG, "Connecting Matter signaling client");

    if (pSignalingClient == NULL) {
        return WEBRTC_STATUS_NULL_ARG;
    }

    MatterSignalingClientData* client_data = (MatterSignalingClientData*)pSignalingClient;

    if (!client_data->initialized) {
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    // Matter signaling is always "connected" - it doesn't have a connection state like KVS
    // The state is already set to CONNECTED during init, so just ensure it's still connected
    if (client_data->state != WEBRTC_SIGNALING_STATE_CONNECTED) {
        client_data->state = WEBRTC_SIGNALING_STATE_CONNECTED;
        // Notify state change only if state actually changed
        if (client_data->on_signaling_state_changed) {
            client_data->on_signaling_state_changed((uint64_t)client_data->user_data, WEBRTC_SIGNALING_STATE_CONNECTED);
        }
    }

    ESP_LOGI(TAG, "Matter signaling client connected (already connected)");
    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Matter signaling disconnect implementation
 */
static WEBRTC_STATUS matterDisconnect(void *pSignalingClient)
{
    ESP_LOGI(TAG, "Disconnecting Matter signaling client");

    if (pSignalingClient == NULL) {
        return WEBRTC_STATUS_NULL_ARG;
    }

    MatterSignalingClientData* client_data = (MatterSignalingClientData*)pSignalingClient;

    if (!client_data->initialized) {
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    // Clear session mappings
    // client_data->session_to_peer_map.clear();
    // client_data->peer_to_session_map.clear();

    // Set disconnected state
    client_data->state = WEBRTC_SIGNALING_STATE_DISCONNECTED;

    // Notify state change
    if (client_data->on_signaling_state_changed) {
        client_data->on_signaling_state_changed((uint64_t)client_data->user_data, WEBRTC_SIGNALING_STATE_DISCONNECTED);
    }

    ESP_LOGI(TAG, "Matter signaling client disconnected");
    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Matter signaling send message implementation
 */
static WEBRTC_STATUS matterSendMessage(void *pSignalingClient, webrtc_message_t* pMessage)
{
    ESP_LOGI(TAG, "Sending message via Matter signaling to the Controller");

    if (pSignalingClient == NULL || pMessage == NULL) {
        ESP_LOGE(TAG, "matterSendMessage: NULL argument");
        return WEBRTC_STATUS_NULL_ARG;
    }

    MatterSignalingClientData* client_data = (MatterSignalingClientData*)pSignalingClient;

    ESP_LOGI(TAG, "matterSendMessage: initialized=%d, state=%d",
             client_data->initialized, client_data->state);

    if (!client_data->initialized) {
        ESP_LOGE(TAG, "matterSendMessage: client not initialized");
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    if (client_data->state != WEBRTC_SIGNALING_STATE_CONNECTED) {
        ESP_LOGE(TAG, "matterSendMessage: client not connected (state=%d)", client_data->state);
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    // Convert peer_client_id to sessionId
    uint16_t sessionId = peer_id_to_session_id(pMessage->peer_client_id);
    // if (sessionId == 0) {
    //     ESP_LOGE(TAG, "Invalid peer_client_id: %s", pMessage->peer_client_id);
    //     return WEBRTC_STATUS_INVALID_ARG;
    // }

    // Get WebRTCProviderManager from config
    chip::app::Clusters::WebRTCTransportProvider::WebRTCProviderManager* provider_manager =
        (chip::app::Clusters::WebRTCTransportProvider::WebRTCProviderManager*)client_data->config.provider_manager;

    if (provider_manager == NULL) {
        ESP_LOGE(TAG, "WebRTCProviderManager is NULL");
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    ESP_LOGI(TAG, "Routing KVS message type=%d for sessionId=%u via Matter transport",
             pMessage->message_type, sessionId);

    // Forward KVS-generated message (answer/ICE candidates) to the Matter transport
    // via the C++ bridge. The bridge will schedule the appropriate Matter command
    // (ScheduleAnswerSend / ScheduleICECandidatesSend) on the Matter event loop.
    return matter_signaling_send_message_via_matter(provider_manager, sessionId, pMessage);
}

/**
 * @brief Matter signaling free implementation
 */
static WEBRTC_STATUS matterFree(void *pSignalingClient)
{
    ESP_LOGI(TAG, "Freeing Matter signaling client");

    if (pSignalingClient == NULL) {
        return WEBRTC_STATUS_NULL_ARG;
    }

    MatterSignalingClientData* client_data = (MatterSignalingClientData*)pSignalingClient;

    // Clear global reference
    if (g_matter_client == client_data) {
        g_matter_client = NULL;
    }

    // Free the client data (use delete since we used new)
    delete client_data;

    ESP_LOGI(TAG, "Matter signaling client freed");
    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Matter signaling set callbacks implementation
 */
static WEBRTC_STATUS matterSetCallbacks(void *pSignalingClient,
                                        uint64_t customData,
                                        WEBRTC_STATUS(*on_msg_received)(uint64_t, webrtc_message_t*),
                                        WEBRTC_STATUS(*on_signaling_state_changed)(uint64_t, webrtc_signaling_state_t),
                                        WEBRTC_STATUS(*on_error)(uint64_t, WEBRTC_STATUS, char*, uint32_t))
{
    if (pSignalingClient == NULL) {
        ESP_LOGE(TAG, "pSignalingClient is NULL");
        return WEBRTC_STATUS_NULL_ARG;
    }

    MatterSignalingClientData* client_data = (MatterSignalingClientData*)pSignalingClient;

    // Set the callbacks
    client_data->on_msg_received = on_msg_received;
    client_data->on_signaling_state_changed = on_signaling_state_changed;
    client_data->on_error = on_error;
    client_data->user_data = customData;

    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Matter signaling set role type implementation (stub)
 */
static WEBRTC_STATUS matterSetRoleType(void *pSignalingClient, webrtc_channel_role_type_t role_type)
{
    ESP_LOGI(TAG, "Setting Matter signaling role type: %d", role_type);

    if (pSignalingClient == NULL) {
        return WEBRTC_STATUS_NULL_ARG;
    }

    // Matter signaling doesn't need to handle role type differently
    ESP_LOGI(TAG, "Matter signaling role type set");
    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Matter signaling get ICE servers implementation
 */
static WEBRTC_STATUS matterGetIceServers(void *pSignalingClient, uint32_t *pIceConfigCount, void *pIceServersArray)
{
    ESP_LOGI(TAG, "Getting ICE servers via Matter signaling");

    if (pSignalingClient == NULL || pIceConfigCount == NULL || pIceServersArray == NULL) {
        return WEBRTC_STATUS_NULL_ARG;
    }

    // For Matter, ICE servers are typically provided by the Matter controller
    // For now, use default STUN server
    *pIceConfigCount = 1;

    app_webrtc_ice_server_t *dst = (app_webrtc_ice_server_t *)pIceServersArray;
    strncpy(dst[0].urls, APP_WEBRTC_DEFAULT_STUN_SERVER, APP_WEBRTC_MAX_ICE_CONFIG_URI_LEN);
    dst[0].urls[APP_WEBRTC_MAX_ICE_CONFIG_URI_LEN - 1] = '\0';
    dst[0].username[0] = '\0';
    dst[0].credential[0] = '\0';

    ESP_LOGI(TAG, "Matter signaling returning default ICE server: %s", dst[0].urls);
    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Matter signaling refresh ICE configuration (stub)
 */
static WEBRTC_STATUS matterRefreshIceConfiguration(void *pSignalingClient)
{
    if (pSignalingClient == NULL) {
        return WEBRTC_STATUS_NULL_ARG;
    }

    ESP_LOGI(TAG, "Matter signaling ICE refresh requested (not implemented)");
    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Set ICE update callback for progressive ICE server delivery
 */
static WEBRTC_STATUS matterSetIceUpdateCallback(void *pSignalingClient,
                                                uint64_t customData,
                                                WEBRTC_STATUS(*on_ice_servers_updated)(uint64_t, uint32_t))
{
    if (pSignalingClient == NULL) {
        return WEBRTC_STATUS_NULL_ARG;
    }

    MatterSignalingClientData *pClientData = (MatterSignalingClientData *)pSignalingClient;

    pClientData->ice_callback_ctx.customData = customData;
    pClientData->ice_callback_ctx.on_ice_servers_updated = on_ice_servers_updated;

    ESP_LOGD(TAG, "Matter ICE update callback set");
    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Get current signaling state
 */
static WEBRTC_STATUS matterGetState(void *pSignalingClient, webrtc_signaling_state_t *pState)
{
    if (pSignalingClient == NULL || pState == NULL) {
        return WEBRTC_STATUS_NULL_ARG;
    }

    MatterSignalingClientData *pClientData = (MatterSignalingClientData *)pSignalingClient;
    *pState = pClientData->state;
    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Get the Matter signaling client interface
 */
extern "C" webrtc_signaling_client_if_t* getMatterSignalingClientInterface(void)
{
    static webrtc_signaling_client_if_t matter_interface = {
        .init = matterInit,
        .connect = matterConnect,
        .disconnect = matterDisconnect,
        .send_message = matterSendMessage,
        .free = matterFree,
        .set_callbacks = matterSetCallbacks,
        .set_role_type = matterSetRoleType,
        .get_ice_servers = matterGetIceServers,
        .get_ice_server_by_idx = NULL, // Matter doesn't support index-based
        .is_ice_refresh_needed = NULL, // Always assume refresh is not needed for Matter
        .refresh_ice_configuration = matterRefreshIceConfiguration,
        .set_ice_update_callback = matterSetIceUpdateCallback,
        .get_state = matterGetState
    };

    return &matter_interface;
}

/**
 * @brief Handle offer received from Matter controller
 */
extern "C" WEBRTC_STATUS matter_signaling_handle_offer(uint16_t sessionId, const char *sdpOffer)
{
    ESP_LOGI(TAG, "Handling offer from Matter: sessionId=%u", sessionId);

    if (g_matter_client == NULL) {
        ESP_LOGE(TAG, "Matter offer received but g_matter_client is NULL");
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    if (!g_matter_client->initialized) {
        ESP_LOGE(TAG, "Matter offer received but client is not initialized");
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    if (g_matter_client->on_msg_received == NULL) {
        ESP_LOGE(TAG, "Matter offer received but on_msg_received callback is NULL");
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    if (sdpOffer == NULL) {
        ESP_LOGE(TAG, "Matter offer received but sdpOffer is NULL");
        return WEBRTC_STATUS_NULL_ARG;
    }

    // Convert sessionId to peer_client_id
    char peerClientId[APP_WEBRTC_MAX_SIGNALING_CLIENT_ID_LEN];
    session_id_to_peer_id(sessionId, peerClientId, sizeof(peerClientId));

    // Store mapping (use insert instead of operator[] to avoid potential issues)
    // Note: ESP-IDF doesn't enable exceptions, so we can't use try-catch
    // g_matter_client->session_to_peer_map.insert(std::make_pair(sessionId, std::string(peerClientId)));
    // g_matter_client->peer_to_session_map.insert(std::make_pair(std::string(peerClientId), sessionId));

    // Convert to webrtc_message_t
    webrtc_message_t webrtc_msg;
    memset(&webrtc_msg, 0, sizeof(webrtc_message_t));
    webrtc_msg.message_type = WEBRTC_MESSAGE_TYPE_OFFER;
    webrtc_msg.version = 1;
    strncpy(webrtc_msg.correlation_id, "matter_offer", APP_WEBRTC_MAX_CORRELATION_ID_LEN);
    webrtc_msg.correlation_id[APP_WEBRTC_MAX_CORRELATION_ID_LEN - 1] = '\0';
    strncpy(webrtc_msg.peer_client_id, peerClientId, APP_WEBRTC_MAX_SIGNALING_CLIENT_ID_LEN);
    webrtc_msg.peer_client_id[APP_WEBRTC_MAX_SIGNALING_CLIENT_ID_LEN - 1] = '\0';

    // Wrap raw SDP in the JSON format expected by KVS deserializeSessionDescriptionInit:
    //   {"type": "offer", "sdp": "<JSON-escaped SDP>"}
    // Built directly into the final heap buffer (no intermediate std::string copies).
    size_t json_len = 0;
    webrtc_msg.payload = build_json_payload("{\"type\": \"offer\", \"sdp\": \"", sdpOffer, "\"}", &json_len);
    if (webrtc_msg.payload == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for JSON SDP offer");
        return WEBRTC_STATUS_NOT_ENOUGH_MEMORY;
    }
    webrtc_msg.payload_len = json_len;   // Length without null terminator (KVS uses this)

    ESP_LOGI(TAG, "SDP offer: raw_len=%zu, json_len=%zu", strlen(sdpOffer), json_len);

    // Check for reasonable size limit (KVS has internal limits)
    if (json_len > 64 * 1024) {  // 64KB limit
        ESP_LOGE(TAG, "JSON payload too large: %zu bytes (max 64KB)", json_len);
        free(webrtc_msg.payload);
        return WEBRTC_STATUS_INVALID_ARG;
    }

    if (webrtc_msg.payload_len == 0) {
        ESP_LOGE(TAG, "Invalid JSON payload: empty");
        free(webrtc_msg.payload);
        return WEBRTC_STATUS_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Created JSON offer payload: len=%zu", json_len);

    // Schedule callback via dedicated task to avoid stack overflow
    // The callback chain (on_msg_received -> kvs_initializePeerConnection) uses a lot of stack
    callback_work_params params;
    params.msg = webrtc_msg;  // Copy the message structure
    params.user_data = g_matter_client->user_data;
    params.on_msg_received = g_matter_client->on_msg_received;

    // Send to dedicated callback task (which has 64KB stack)
    if (xQueueSend(g_webrtc_callback_queue, &params, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to queue callback to WebRTC callback task");
        free(webrtc_msg.payload);
        return WEBRTC_STATUS_INTERNAL_ERROR;
    }

    // Return success immediately - callback will be executed asynchronously
    // Note: payload will be freed by the callback task
    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Handle answer received from Matter controller
 */
extern "C" WEBRTC_STATUS matter_signaling_handle_answer(uint16_t sessionId, const char *sdpAnswer)
{
    ESP_LOGI(TAG, "Handling answer from Matter: sessionId=%u", sessionId);

    if (g_matter_client == NULL) {
        ESP_LOGE(TAG, "Matter answer received but g_matter_client is NULL");
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    if (!g_matter_client->initialized) {
        ESP_LOGE(TAG, "Matter answer received but client is not initialized");
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    if (g_matter_client->on_msg_received == NULL) {
        ESP_LOGE(TAG, "Matter answer received but on_msg_received callback is NULL");
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    if (sdpAnswer == NULL) {
        ESP_LOGE(TAG, "Matter answer received but sdpAnswer is NULL");
        return WEBRTC_STATUS_NULL_ARG;
    }

    // Convert sessionId to peer_client_id
    char peerClientId[APP_WEBRTC_MAX_SIGNALING_CLIENT_ID_LEN];
    session_id_to_peer_id(sessionId, peerClientId, sizeof(peerClientId));

    // Convert to webrtc_message_t
    webrtc_message_t webrtc_msg;
    memset(&webrtc_msg, 0, sizeof(webrtc_message_t));
    webrtc_msg.message_type = WEBRTC_MESSAGE_TYPE_ANSWER;
    webrtc_msg.version = 1;
    strncpy(webrtc_msg.correlation_id, "matter_answer", APP_WEBRTC_MAX_CORRELATION_ID_LEN);
    webrtc_msg.correlation_id[APP_WEBRTC_MAX_CORRELATION_ID_LEN - 1] = '\0';
    strncpy(webrtc_msg.peer_client_id, peerClientId, APP_WEBRTC_MAX_SIGNALING_CLIENT_ID_LEN);
    webrtc_msg.peer_client_id[APP_WEBRTC_MAX_SIGNALING_CLIENT_ID_LEN - 1] = '\0';

    // Wrap raw SDP in the JSON format expected by KVS deserializeSessionDescriptionInit:
    //   {"type": "answer", "sdp": "<JSON-escaped SDP>"}
    // Built directly into the final heap buffer (no intermediate std::string copies).
    size_t json_len = 0;
    webrtc_msg.payload = build_json_payload("{\"type\": \"answer\", \"sdp\": \"", sdpAnswer, "\"}", &json_len);
    if (webrtc_msg.payload == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for JSON SDP answer");
        return WEBRTC_STATUS_NOT_ENOUGH_MEMORY;
    }
    webrtc_msg.payload_len = json_len;

    // Schedule callback via dedicated task to avoid stack overflow
    callback_work_params params;
    params.msg = webrtc_msg;  // Copy the message structure
    params.user_data = g_matter_client->user_data;
    params.on_msg_received = g_matter_client->on_msg_received;

    // Send to dedicated callback task (which has 64KB stack)
    if (xQueueSend(g_webrtc_callback_queue, &params, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to queue callback to WebRTC callback task");
        free(webrtc_msg.payload);
        return WEBRTC_STATUS_INTERNAL_ERROR;
    }

    // Return success immediately - callback will be executed asynchronously
    // Note: payload will be freed by the callback task
    return WEBRTC_STATUS_SUCCESS;
}

/**
 * @brief Handle ICE candidate received from Matter controller
 */
extern "C" WEBRTC_STATUS matter_signaling_handle_ice_candidate(uint16_t sessionId, const char *candidate, const char *mid)
{
    ESP_LOGI(TAG, "Handling ICE candidate from Matter: sessionId=%u", sessionId);

    if (g_matter_client == NULL) {
        ESP_LOGE(TAG, "Matter ICE candidate received but g_matter_client is NULL");
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    if (!g_matter_client->initialized) {
        ESP_LOGE(TAG, "Matter ICE candidate received but client is not initialized");
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    if (g_matter_client->on_msg_received == NULL) {
        ESP_LOGE(TAG, "Matter ICE candidate received but on_msg_received callback is NULL");
        return WEBRTC_STATUS_INVALID_OPERATION;
    }

    if (candidate == NULL) {
        ESP_LOGE(TAG, "Matter ICE candidate received but candidate is NULL");
        return WEBRTC_STATUS_NULL_ARG;
    }

    // Convert sessionId to peer_client_id
    char peerClientId[APP_WEBRTC_MAX_SIGNALING_CLIENT_ID_LEN];
    session_id_to_peer_id(sessionId, peerClientId, sizeof(peerClientId));

    // Convert to webrtc_message_t
    webrtc_message_t webrtc_msg;
    memset(&webrtc_msg, 0, sizeof(webrtc_message_t));
    webrtc_msg.message_type = WEBRTC_MESSAGE_TYPE_ICE_CANDIDATE;
    webrtc_msg.version = 1;
    strncpy(webrtc_msg.correlation_id, "matter_ice", APP_WEBRTC_MAX_CORRELATION_ID_LEN);
    webrtc_msg.correlation_id[APP_WEBRTC_MAX_CORRELATION_ID_LEN - 1] = '\0';
    strncpy(webrtc_msg.peer_client_id, peerClientId, APP_WEBRTC_MAX_SIGNALING_CLIENT_ID_LEN);
    webrtc_msg.peer_client_id[APP_WEBRTC_MAX_SIGNALING_CLIENT_ID_LEN - 1] = '\0';

    // Wrap raw ICE candidate in JSON format expected by KVS deserializeRtcIceCandidateInit:
    //   {"candidate": "<JSON-escaped candidate>"}
    // Built directly into the final heap buffer (no intermediate std::string copies).
    size_t json_len = 0;
    webrtc_msg.payload = build_json_payload("{\"candidate\": \"", candidate, "\"}", &json_len);
    if (webrtc_msg.payload == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for JSON ICE candidate");
        return WEBRTC_STATUS_NOT_ENOUGH_MEMORY;
    }
    webrtc_msg.payload_len = json_len;

    // Schedule callback via dedicated task to avoid stack overflow
    callback_work_params params;
    params.msg = webrtc_msg;  // Copy the message structure
    params.user_data = g_matter_client->user_data;
    params.on_msg_received = g_matter_client->on_msg_received;

    // Send to dedicated callback task (which has 64KB stack)
    if (xQueueSend(g_webrtc_callback_queue, &params, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to queue callback to WebRTC callback task");
        free(webrtc_msg.payload);
        return WEBRTC_STATUS_INTERNAL_ERROR;
    }

    // Return success immediately - callback will be executed asynchronously
    // Note: payload will be freed by the callback task
    return WEBRTC_STATUS_SUCCESS;
}
