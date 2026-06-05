/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <rainmaker_controller.h>
#include <rainmaker_controller_mqtt.h>
#include <rainmaker_controller_user_api.h>

#include <cJSON.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_rmaker_auth_service.h>
#include <esp_rmaker_common_events.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_work_queue.h>
#include <esp_rmaker_standard_params.h>

#include <stdlib.h>
#include <string.h>

static const char *TAG = "rainmaker_controller";
static bool s_bridge_sync_started;
static char *s_bridge_group_id;
static char *s_subscribed_group_id;
static rainmaker_controller_config_t s_config;

static void rainmaker_controller_unsubscribe_params_topics(void);

static esp_err_t rainmaker_controller_callback(const char *node_id, const char *payload, size_t payload_len);
static void rainmaker_controller_schedule_full_sync(void);
static void rainmaker_controller_subscribe_params_topics(void);

static esp_rmaker_device_t *rainmaker_controller_service_create(const char *serv_name,
                                                                esp_rmaker_device_write_cb_t write_cb,
                                                                esp_rmaker_device_read_cb_t read_cb, void *priv_data)
{
    esp_rmaker_device_t *service = esp_rmaker_service_create(serv_name, "esp.service.rmaker-controller", priv_data);
    if (!service) {
        return nullptr;
    }

    esp_rmaker_device_add_cb(service, write_cb, read_cb);
    esp_rmaker_device_add_param(service, esp_rmaker_param_create(ESP_RMAKER_DEF_GROUP_ID_NAME, ESP_RMAKER_PARAM_GROUP_ID,
                                                                 esp_rmaker_str(""),
                                                                 PROP_FLAG_READ | PROP_FLAG_WRITE | PROP_FLAG_PERSIST));
    return service;
}

static const char *rainmaker_controller_get_stored_string_param(esp_rmaker_device_t *service, const char *param_name)
{
    esp_rmaker_param_t *param = esp_rmaker_device_get_param_by_name(service, param_name);
    if (!param) {
        return nullptr;
    }

    esp_rmaker_param_val_t *val = esp_rmaker_param_get_val(param);
    if (!val || val->type != RMAKER_VAL_TYPE_STRING || !val->val.s || val->val.s[0] == 0) {
        return nullptr;
    }
    return val->val.s;
}

static void rainmaker_controller_auth_service_event_handler([[maybe_unused]] void *arg, esp_event_base_t event_base,
                                                            int32_t event_id, [[maybe_unused]] void *event_data)
{
    if (event_base != RMAKER_AUTH_SERVICE_EVENT) {
        return;
    }

    switch (event_id) {
    case RMAKER_AUTH_SERVICE_EVENT_ENABLED:
    case RMAKER_AUTH_SERVICE_EVENT_BASE_URL_RECEIVED:
    case RMAKER_AUTH_SERVICE_EVENT_TOKEN_RECEIVED:
        rainmaker_controller_user_api_reset_login();
        rainmaker_controller_schedule_full_sync();
        break;
    default:
        break;
    }
}

static void rainmaker_controller_full_sync(void)
{
    if (!s_bridge_group_id || s_bridge_group_id[0] == 0) {
        ESP_LOGI(TAG, "RainMaker controller group id is not set; skip full sync");
        if (s_config.group_nodes_cb) {
            char empty_nodes[] = "[]";
            s_config.group_nodes_cb(empty_nodes, strlen(empty_nodes), nullptr);
        }
        return;
    }

    char *nodes_buffer = nullptr;
    esp_err_t err = rainmaker_controller_get_bridge_group_nodes(s_bridge_group_id, &nodes_buffer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get RainMaker bridge group nodes list: %d", err);
        return;
    }
    if (!nodes_buffer) {
        ESP_LOGE(TAG, "RainMaker bridge group nodes list is empty");
        return;
    }

    err = s_config.group_nodes_cb ? s_config.group_nodes_cb(nodes_buffer, strlen(nodes_buffer), s_bridge_group_id) :
          ESP_OK;
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "RainMaker full sync callback failed: %d", err);
    }
    free(nodes_buffer);
}

static void rainmaker_controller_full_sync_work_fn([[maybe_unused]] void *priv_data)
{
    rainmaker_controller_full_sync();
}

static void rainmaker_controller_schedule_full_sync(void)
{
    if (!s_bridge_sync_started) {
        ESP_LOGI(TAG, "Matter bridge not ready; defer RainMaker full sync");
        return;
    }
    if (!rainmaker_controller_is_user_api_ready()) {
        ESP_LOGI(TAG, "RainMaker user API credentials not ready; skip full sync");
        return;
    }
    if (esp_rmaker_work_queue_add_task(rainmaker_controller_full_sync_work_fn, nullptr) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to schedule RainMaker full sync");
    }
}

static bool rainmaker_controller_get_connectivity_state(const char *payload, size_t payload_len, bool *connected)
{
    cJSON *root = cJSON_ParseWithLength(payload, payload_len);
    if (!root) {
        return false;
    }

    cJSON *connectivity = cJSON_GetObjectItem(root, "Connectivity");
    cJSON *connected_item = cJSON_IsObject(connectivity) ? cJSON_GetObjectItem(connectivity, "Connected") : nullptr;
    bool found = cJSON_IsBool(connected_item);
    if (found) {
        *connected = cJSON_IsTrue(connected_item);
    }
    cJSON_Delete(root);
    return found;
}

static esp_err_t rainmaker_controller_callback(const char *node_id, const char *payload, size_t payload_len)
{
    ESP_RETURN_ON_FALSE(node_id && payload && payload_len > 0, ESP_ERR_INVALID_ARG, TAG,
                        "RainMaker controller callback ignored invalid update: node=%p payload=%p len=%u",
                        node_id, payload, payload_len);

    ESP_LOGI(TAG, "Controller received params update from node %s", node_id);
    ESP_LOGI(TAG, "Controller payload: %.*s", (int)payload_len, payload);

    bool connected = false;
    if (rainmaker_controller_get_connectivity_state(payload, payload_len, &connected)) {
        esp_err_t err = s_config.node_connectivity_cb ?
                        s_config.node_connectivity_cb(node_id, connected) : ESP_OK;
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "RainMaker connectivity update callback failed: %d", err);
            if (err == ESP_ERR_NOT_FOUND) {
                rainmaker_controller_schedule_full_sync();
            }
        }
    }

    esp_err_t err = s_config.node_params_cb ? s_config.node_params_cb(node_id, payload, payload_len) : ESP_OK;
    if (err == ESP_OK) {
        return ESP_OK;
    }

    ESP_LOGE(TAG, "RainMaker params update callback failed: %d", err);
    if (err == ESP_ERR_NOT_FOUND) {
        rainmaker_controller_schedule_full_sync();
    }
    return ESP_OK;
}

static void rainmaker_controller_subscribe_params_topics(void)
{
    if (!s_bridge_group_id || s_bridge_group_id[0] == 0) {
        ESP_LOGI(TAG, "RainMaker controller group id is not set; skip params MQTT subscription");
        rainmaker_controller_unsubscribe_params_topics();
        return;
    }

    if (s_subscribed_group_id && strcmp(s_subscribed_group_id, s_bridge_group_id) == 0) {
        ESP_LOGI(TAG, "RainMaker params topic already subscribed for group %s", s_bridge_group_id);
        return;
    }

    char topic[150] = {0};
    snprintf(topic, sizeof(topic), "node/+/params/local/%s", s_bridge_group_id);
    if (rainmaker_controller_subscribe_params_topic(topic, rainmaker_controller_callback) == ESP_OK) {
        if (s_subscribed_group_id) {
            char old_topic[150] = {0};
            snprintf(old_topic, sizeof(old_topic), "node/+/params/local/%s", s_subscribed_group_id);
            rainmaker_controller_unsubscribe_params_topic(old_topic);
            free(s_subscribed_group_id);
        }
        s_subscribed_group_id = strdup(s_bridge_group_id);
        if (!s_subscribed_group_id) {
            ESP_LOGE(TAG, "Failed to store subscribed RainMaker group id");
        }
    }
}

static void rainmaker_controller_unsubscribe_params_topics(void)
{
    if (!s_subscribed_group_id) {
        return;
    }

    char topic[150] = {0};
    snprintf(topic, sizeof(topic), "node/+/params/local/%s", s_subscribed_group_id);
    rainmaker_controller_unsubscribe_params_topic(topic);
    free(s_subscribed_group_id);
    s_subscribed_group_id = nullptr;
}

static void rainmaker_controller_event_handler([[maybe_unused]] void *arg, esp_event_base_t event_base,
                                               int32_t event_id, [[maybe_unused]] void *event_data)
{
    if (event_base == RMAKER_COMMON_EVENT && event_id == RMAKER_MQTT_EVENT_CONNECTED) {
        ESP_LOGI(TAG, "RainMaker MQTT connected event");
        rainmaker_controller_subscribe_params_topics();
    } else if (event_base == RMAKER_COMMON_EVENT && event_id == RMAKER_MQTT_EVENT_DISCONNECTED) {
        ESP_LOGW(TAG, "RainMaker MQTT disconnected event; clearing params topic subscription state");
        free(s_subscribed_group_id);
        s_subscribed_group_id = nullptr;
    }
}

static esp_err_t write_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
                          const esp_rmaker_param_val_t val, [[maybe_unused]] void *priv_data,
                          esp_rmaker_write_ctx_t *ctx)
{
    bool from_init = ctx && ctx->src == ESP_RMAKER_REQ_SRC_INIT;

    if (strcmp(esp_rmaker_param_get_type(param), ESP_RMAKER_PARAM_GROUP_ID) == 0) {
        ESP_RETURN_ON_FALSE(val.type == RMAKER_VAL_TYPE_STRING && val.val.s, ESP_ERR_INVALID_ARG, TAG,
                            "Invalid group_id param value");
        char *group_id = strdup(val.val.s);
        ESP_RETURN_ON_FALSE(group_id, ESP_ERR_NO_MEM, TAG, "Failed to store group_id");
        free(s_bridge_group_id);
        s_bridge_group_id = group_id;
        ESP_LOGI(TAG, "Set RainMaker controller group id: %s", s_bridge_group_id);
        ESP_RETURN_ON_ERROR(from_init ? esp_rmaker_param_update(param, val) : esp_rmaker_param_update_and_report(param, val),
                            TAG, "Failed to update group_id param");
        if (!from_init) {
            rainmaker_controller_subscribe_params_topics();
            rainmaker_controller_schedule_full_sync();
        }
    }

    return ESP_OK;
}

/**
 * @brief Initialize the RainMaker bridge services
 *
 */
esp_err_t rainmaker_controller_init(esp_rmaker_node_t *node, const rainmaker_controller_config_t *config)
{
    ESP_RETURN_ON_FALSE(node, ESP_ERR_INVALID_ARG, TAG, "RainMaker node cannot be null");
    if (config) {
        s_config = *config;
    }

    esp_rmaker_device_t *device = esp_rmaker_device_create("Rainmaker Controller", "esp.device.controller", nullptr);
    esp_rmaker_device_add_param(device, esp_rmaker_name_param_create(ESP_RMAKER_DEF_NAME_PARAM, "RainmakerController"));
    ESP_RETURN_ON_ERROR(esp_rmaker_node_add_device(node, device), TAG, "Failed to add Rainmaker Controller device");

    esp_rmaker_device_t *controller_service = rainmaker_controller_service_create("RMCTLService", write_cb, nullptr, nullptr);
    ESP_RETURN_ON_FALSE(controller_service, ESP_FAIL, TAG, "Failed to create RMCTLService service");
    ESP_RETURN_ON_ERROR(esp_rmaker_node_add_device(node, controller_service), TAG, "Failed to add RMCTLService service");

    const char *stored_group_id = rainmaker_controller_get_stored_string_param(controller_service,
                                                                               ESP_RMAKER_DEF_GROUP_ID_NAME);
    if (stored_group_id) {
        s_bridge_group_id = strdup(stored_group_id);
        ESP_ERROR_CHECK(s_bridge_group_id ? ESP_OK : ESP_ERR_NO_MEM);
    }

    ESP_RETURN_ON_ERROR(esp_event_handler_register(RMAKER_AUTH_SERVICE_EVENT, ESP_EVENT_ANY_ID,
                                                   rainmaker_controller_auth_service_event_handler, nullptr),
                        TAG, "Failed to register RainMaker auth service handler");
    ESP_RETURN_ON_ERROR(esp_rmaker_auth_service_enable(), TAG, "Failed to enable RainMaker auth service");

    ESP_RETURN_ON_ERROR(esp_event_handler_register(RMAKER_COMMON_EVENT, RMAKER_MQTT_EVENT_CONNECTED,
                                                   rainmaker_controller_event_handler, nullptr),
                        TAG, "Failed to register RainMaker MQTT connected handler");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(RMAKER_COMMON_EVENT, RMAKER_MQTT_EVENT_DISCONNECTED,
                                                   rainmaker_controller_event_handler, nullptr),
                        TAG, "Failed to register RainMaker MQTT disconnected handler");
    return ESP_OK;
}

void rainmaker_controller_start(void)
{
    s_bridge_sync_started = true;
    rainmaker_controller_schedule_full_sync();
}
