/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <rainmaker_controller_node.h>
#include <rainmaker_controller_user_api.h>

#include <app_rmaker_user_helper_api.h>
#include <cJSON.h>
#include <esp_check.h>
#include <esp_log.h>
#include <esp_rmaker_work_queue.h>

#include <stdlib.h>
#include <string.h>

static const char *TAG = "rainmaker_controller_node";

static void rainmaker_controller_set_params_work_fn(void *priv_data)
{
    char *payload = static_cast<char *>(priv_data);
    if (!payload) {
        ESP_LOGE(TAG, "RainMaker set params work missing payload");
        return;
    }

    esp_err_t err = rainmaker_controller_send_node_params(payload);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set RainMaker node params: %d", err);
    }
    free(payload);
}

static esp_err_t rainmaker_controller_schedule_set_params(char *payload)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_ERROR(esp_rmaker_work_queue_add_task(rainmaker_controller_set_params_work_fn, payload), cleanup, TAG,
                      "Failed to schedule RainMaker set params work");
    return ESP_OK;

cleanup:
    free(payload);
    return ret;
}

esp_err_t rainmaker_controller_set_node_params(const char *node_id, const char *device_name,
                                               const rainmaker_controller_param_update_t *updates,
                                               size_t update_count)
{
    esp_err_t ret = ESP_OK;
    char *param_payload = nullptr;
    ESP_RETURN_ON_FALSE(node_id && node_id[0] != 0, ESP_ERR_INVALID_ARG, TAG, "RainMaker node id is empty");
    ESP_RETURN_ON_FALSE(device_name && device_name[0] != 0, ESP_ERR_INVALID_ARG, TAG, "RainMaker device name is empty");
    ESP_RETURN_ON_FALSE(updates && update_count > 0, ESP_ERR_INVALID_ARG, TAG, "RainMaker param update list is empty");

    cJSON *root = cJSON_CreateArray();
    cJSON *entry = cJSON_CreateObject();
    cJSON *payload = cJSON_CreateObject();
    cJSON *device = cJSON_CreateObject();
    ESP_GOTO_ON_FALSE(root && entry && payload && device, ESP_ERR_NO_MEM, cleanup, TAG,
                      "Failed to allocate RainMaker params JSON");

    cJSON_AddItemToArray(root, entry);
    cJSON_AddStringToObject(entry, "node_id", node_id);
    cJSON_AddItemToObject(entry, "payload", payload);
    cJSON_AddItemToObject(payload, device_name, device);

    for (size_t i = 0; i < update_count; ++i) {
        ESP_GOTO_ON_FALSE(updates[i].name && updates[i].name[0] != 0, ESP_ERR_INVALID_ARG, cleanup, TAG,
                          "RainMaker param name is empty");
        switch (updates[i].type) {
        case RAINMAKER_CONTROLLER_PARAM_TYPE_BOOL:
            cJSON_AddBoolToObject(device, updates[i].name, updates[i].value.b);
            break;
        case RAINMAKER_CONTROLLER_PARAM_TYPE_INT:
            cJSON_AddNumberToObject(device, updates[i].name, updates[i].value.i);
            break;
        case RAINMAKER_CONTROLLER_PARAM_TYPE_STRING:
            cJSON_AddStringToObject(device, updates[i].name, updates[i].value.s ? updates[i].value.s : "");
            break;
        default:
            ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, cleanup, TAG, "Unsupported RainMaker param type");
        }
    }

    param_payload = cJSON_PrintUnformatted(root);
    ESP_GOTO_ON_FALSE(param_payload, ESP_ERR_NO_MEM, cleanup, TAG, "Failed to serialize RainMaker params JSON");

    ESP_LOGI(TAG, "Set RainMaker node params: %s", param_payload);
    ret = rainmaker_controller_schedule_set_params(param_payload);
    param_payload = nullptr;
    cJSON_Delete(root);
    return ret;

cleanup:
    free(param_payload);
    cJSON_Delete(root);
    return ret;
}

esp_err_t rainmaker_controller_get_node_config(const char *node_id, char **node_config)
{
    ESP_RETURN_ON_ERROR(rainmaker_controller_user_api_init_or_update(), TAG, "RainMaker user API is not ready");
    return app_rmaker_user_helper_api_get_node_config(node_id, node_config);
}

esp_err_t rainmaker_controller_get_node_params(const char *node_id, char **node_params)
{
    ESP_RETURN_ON_ERROR(rainmaker_controller_user_api_init_or_update(), TAG, "RainMaker user API is not ready");
    return app_rmaker_user_helper_api_get_node_params(node_id, node_params);
}

esp_err_t rainmaker_controller_get_node_connection_status(const char *node_id, bool *connection_status)
{
    ESP_RETURN_ON_ERROR(rainmaker_controller_user_api_init_or_update(), TAG, "RainMaker user API is not ready");
    return app_rmaker_user_helper_api_get_node_connection_status(node_id, connection_status);
}
