/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <rainmaker_controller_mqtt.h>

#include <esp_check.h>
#include <esp_log.h>
#include <esp_rmaker_mqtt.h>

#include <stdlib.h>
#include <string.h>

static const char *TAG = "rainmaker_controller_mqtt";
static rainmaker_controller_mqtt_params_callback_t s_params_callback;

static char *rainmaker_controller_get_node_id_from_topic(const char *topic)
{
    const char *prefix = "node/";
    size_t prefix_len = strlen(prefix);
    ESP_RETURN_ON_FALSE(topic && strncmp(topic, prefix, prefix_len) == 0, nullptr, TAG,
                        "RainMaker params topic has unsupported prefix");

    const char *node_id_start = topic + prefix_len;
    const char *node_id_end = strchr(node_id_start, '/');
    ESP_RETURN_ON_FALSE(node_id_end && node_id_end != node_id_start, nullptr, TAG,
                        "RainMaker params topic has no node id");

    size_t node_id_len = node_id_end - node_id_start;
    char *node_id = static_cast<char *>(calloc(1, node_id_len + 1));
    ESP_RETURN_ON_FALSE(node_id, nullptr, TAG, "Failed to allocate RainMaker MQTT topic node id");

    memcpy(node_id, node_id_start, node_id_len);
    return node_id;
}

static void rainmaker_controller_params_mqtt_callback(const char *topic, void *payload, size_t payload_len,
                                                      [[maybe_unused]] void *priv_data)
{
    if (!topic || !payload || payload_len == 0) {
        ESP_LOGE(TAG, "RainMaker MQTT params callback ignored invalid message: topic=%p payload=%p len=%u",
                 topic, payload, payload_len);
        return;
    }

    ESP_LOGI(TAG, "RainMaker MQTT params message: topic=%s len=%u payload=%.*s",
             topic, payload_len, (int)payload_len, static_cast<const char *>(payload));
    char *node_id = rainmaker_controller_get_node_id_from_topic(topic);
    if (!node_id) {
        ESP_LOGE(TAG, "Received RainMaker params on unsupported topic: %s", topic);
        return;
    }
    ESP_LOGI(TAG, "RainMaker MQTT topic node id: %s", node_id);
    if (s_params_callback) {
        s_params_callback(node_id, static_cast<const char *>(payload), payload_len);
    }
    free(node_id);
}

esp_err_t rainmaker_controller_subscribe_params_topic(const char *topic,
                                                      rainmaker_controller_mqtt_params_callback_t callback)
{
    s_params_callback = callback;
    ESP_RETURN_ON_ERROR(esp_rmaker_mqtt_subscribe(topic, rainmaker_controller_params_mqtt_callback, RMAKER_MQTT_QOS1,
                                                  nullptr), TAG, "Failed to subscribe to RainMaker params topic %s", topic);
    ESP_LOGI(TAG, "Subscribed to RainMaker params topic: %s", topic);
    return ESP_OK;
}

esp_err_t rainmaker_controller_unsubscribe_params_topic(const char *topic)
{
    ESP_RETURN_ON_ERROR(esp_rmaker_mqtt_unsubscribe(topic), TAG, "Failed to unsubscribe from RainMaker params topic %s",
                        topic);
    ESP_LOGI(TAG, "Unsubscribed from RainMaker params topic: %s", topic);
    return ESP_OK;
}
