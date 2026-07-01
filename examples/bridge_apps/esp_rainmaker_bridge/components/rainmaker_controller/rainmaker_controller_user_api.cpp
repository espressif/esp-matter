/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <rainmaker_controller_user_api.h>

#include <app_rmaker_user_api.h>
#include <app_rmaker_user_helper_api.h>
#include <cJSON.h>
#include <esp_check.h>
#include <esp_log.h>
#include <esp_rmaker_auth_service.h>

#include <stdlib.h>
#include <string.h>

static const char *TAG = "rainmaker_controller_user_api";
static bool s_user_api_initialized;
static bool s_user_api_logged_in;

static void rainmaker_controller_login_success_cb(void)
{
    s_user_api_logged_in = true;
    ESP_LOGI(TAG, "RainMaker user API login succeeded");
    esp_rmaker_user_auth_service_token_status_update(ESP_RMAKER_USER_AUTH_SERVICE_TOKEN_STATUS_VERIFIED);
}

static void rainmaker_controller_login_failure_cb(int error_code, const char *failed_reason)
{
    s_user_api_logged_in = false;
    ESP_LOGW(TAG, "RainMaker user API login failed: %d %s", error_code, failed_reason ? failed_reason : "");
    esp_rmaker_user_auth_service_token_status_update(ESP_RMAKER_USER_AUTH_SERVICE_TOKEN_STATUS_EXPIRED_OR_INVALID);
}

static esp_err_t rainmaker_controller_user_api_login(void)
{
    esp_err_t err = app_rmaker_user_api_login();
    if (err != ESP_OK) {
        esp_rmaker_user_auth_service_token_status_update(ESP_RMAKER_USER_AUTH_SERVICE_TOKEN_STATUS_EXPIRED_OR_INVALID);
    }
    return err;
}

static esp_err_t rainmaker_controller_get_auth_service_creds(char **base_url, char **refresh_token)
{
    esp_err_t ret = ESP_OK;
    *base_url = nullptr;
    *refresh_token = nullptr;

    ret = esp_rmaker_auth_service_get_user_token(refresh_token);
    ESP_GOTO_ON_ERROR(ret, cleanup, TAG, "RainMaker user token is not ready");

    ret = esp_rmaker_auth_service_get_base_url(base_url);
    if (ret == ESP_ERR_NOT_FOUND) {
        ret = ESP_OK;
    }
    ESP_GOTO_ON_ERROR(ret, cleanup, TAG, "Failed to get RainMaker auth service base URL");

cleanup:
    if (ret != ESP_OK) {
        free(*base_url);
        *base_url = nullptr;
        free(*refresh_token);
        *refresh_token = nullptr;
    }
    return ret;
}

esp_err_t rainmaker_controller_user_api_init_or_update(void)
{
    esp_err_t ret = ESP_OK;
    char *base_url = nullptr;
    char *refresh_token = nullptr;
    ESP_RETURN_ON_ERROR(rainmaker_controller_get_auth_service_creds(&base_url, &refresh_token), TAG,
                        "RainMaker auth service credentials are not ready");

    if (!s_user_api_initialized) {
        app_rmaker_user_api_config_t config = {
            .refresh_token = refresh_token,
            .base_url = base_url,
            .api_version = nullptr,
            .username = nullptr,
            .password = nullptr,
        };

        esp_err_t err = app_rmaker_user_api_init(&config);
        free(base_url);
        free(refresh_token);
        ESP_RETURN_ON_ERROR(err, TAG, "Failed to initialize RainMaker user API");
        s_user_api_initialized = true;
        app_rmaker_user_api_register_login_success_callback(rainmaker_controller_login_success_cb);
        app_rmaker_user_api_register_login_failure_callback(rainmaker_controller_login_failure_cb);
        return rainmaker_controller_user_api_login();
    }

    if (s_user_api_logged_in) {
        free(base_url);
        free(refresh_token);
        return ESP_OK;
    }

    if (base_url) {
        ret = app_rmaker_user_api_set_base_url(base_url);
        free(base_url);
        base_url = nullptr;
        ESP_GOTO_ON_ERROR(ret, cleanup, TAG, "Failed to update RainMaker user API base URL");
    }
    ret = app_rmaker_user_api_set_refresh_token(refresh_token);
    ESP_GOTO_ON_ERROR(ret, cleanup, TAG, "Failed to update RainMaker user API refresh token");
    free(refresh_token);
    refresh_token = nullptr;
    return rainmaker_controller_user_api_login();

cleanup:
    free(base_url);
    free(refresh_token);
    return ret;
}

static cJSON *rainmaker_controller_get_group_nodes_array(cJSON *group)
{
    if (!cJSON_IsObject(group)) {
        return nullptr;
    }

    cJSON *nodes = cJSON_GetObjectItem(group, "nodes");
    if (cJSON_IsArray(nodes)) {
        return nodes;
    }

    nodes = cJSON_GetObjectItem(group, "node_list");
    if (cJSON_IsArray(nodes)) {
        return nodes;
    }

    return nullptr;
}

static const char *rainmaker_controller_get_node_id_from_group_node(cJSON *node)
{
    if (cJSON_IsString(node)) {
        return node->valuestring;
    }
    if (!cJSON_IsObject(node)) {
        return nullptr;
    }

    cJSON *node_id = cJSON_GetObjectItem(node, "node_id");
    if (cJSON_IsString(node_id)) {
        return node_id->valuestring;
    }

    node_id = cJSON_GetObjectItem(node, "id");
    if (cJSON_IsString(node_id)) {
        return node_id->valuestring;
    }

    return nullptr;
}

static esp_err_t rainmaker_controller_parse_group_nodes(const char *groups, const char *group_id, char **nodes_buffer)
{
    esp_err_t ret = ESP_OK;
    cJSON *root = nullptr;
    cJSON *nodes = nullptr;
    cJSON *groups_array = nullptr;
    cJSON *node_ids = nullptr;
    *nodes_buffer = nullptr;

    root = cJSON_Parse(groups);
    ESP_GOTO_ON_FALSE(root, ESP_ERR_INVALID_RESPONSE, cleanup, TAG, "Failed to parse RainMaker groups response");

    groups_array = cJSON_GetObjectItem(root, "groups");
    if (cJSON_IsArray(groups_array)) {
        cJSON *group = nullptr;
        cJSON_ArrayForEach(group, groups_array) {
            cJSON *id = cJSON_GetObjectItem(group, "group_id");
            if (!group_id || (cJSON_IsString(id) && strcmp(id->valuestring, group_id) == 0)) {
                nodes = rainmaker_controller_get_group_nodes_array(group);
                break;
            }
        }
    } else {
        nodes = rainmaker_controller_get_group_nodes_array(root);
    }

    node_ids = cJSON_CreateArray();
    ESP_GOTO_ON_FALSE(node_ids, ESP_ERR_NO_MEM, cleanup, TAG, "Failed to allocate RainMaker group nodes array");

    if (cJSON_IsArray(nodes)) {
        cJSON *node = nullptr;
        cJSON_ArrayForEach(node, nodes) {
            const char *node_id = rainmaker_controller_get_node_id_from_group_node(node);
            if (node_id && node_id[0] != 0) {
                cJSON_AddItemToArray(node_ids, cJSON_CreateString(node_id));
            }
        }
    }

    *nodes_buffer = cJSON_PrintUnformatted(node_ids);
    ESP_GOTO_ON_FALSE(*nodes_buffer, ESP_ERR_NO_MEM, cleanup, TAG, "Failed to serialize RainMaker group nodes");

cleanup:
    cJSON_Delete(node_ids);
    cJSON_Delete(root);
    return ret;
}

bool rainmaker_controller_is_user_api_ready(void)
{
    char *user_token = nullptr;
    esp_err_t err = esp_rmaker_auth_service_get_user_token(&user_token);
    free(user_token);
    return err == ESP_OK;
}

void rainmaker_controller_user_api_reset_login(void)
{
    s_user_api_logged_in = false;
}

esp_err_t rainmaker_controller_get_bridge_group_nodes(const char *group_id, char **nodes_buffer)
{
    esp_err_t ret = ESP_OK;
    char *groups = nullptr;

    ESP_RETURN_ON_FALSE(group_id && group_id[0] != 0 && nodes_buffer, ESP_ERR_INVALID_ARG, TAG,
                        "Invalid RainMaker bridge group nodes arguments");

    *nodes_buffer = nullptr;
    ESP_RETURN_ON_ERROR(rainmaker_controller_user_api_init_or_update(), TAG, "RainMaker user API is not ready");
    ESP_GOTO_ON_ERROR(app_rmaker_user_helper_api_get_groups(group_id, &groups), cleanup, TAG,
                      "Failed to get RainMaker groups");
    ret = rainmaker_controller_parse_group_nodes(groups, group_id, nodes_buffer);

cleanup:
    free(groups);
    return ret;
}

esp_err_t rainmaker_controller_send_node_params(const char *payload)
{
    esp_err_t ret = ESP_OK;
    char *response = nullptr;
    int status_code = 0;
    ESP_RETURN_ON_ERROR(rainmaker_controller_user_api_init_or_update(), TAG, "RainMaker user API is not ready");

    app_rmaker_user_api_request_config_t request_config = {
        .reuse_session = false,
        .no_need_authorize = false,
        .payload_is_json = true,
        .api_type = APP_RMAKER_USER_API_TYPE_PUT,
        .api_name = "user/nodes/params",
        .api_version = nullptr,
        .api_payload = payload,
        .api_query_params = nullptr,
    };
    ret = app_rmaker_user_api_generic(&request_config, &status_code, &response);
    ESP_GOTO_ON_ERROR(ret, cleanup, TAG, "RainMaker set params request failed");

    if (status_code < 200 || status_code >= 300) {
        ESP_LOGE(TAG, "RainMaker set params failed, status=%d, response=%s", status_code, response ? response : "");
        ret = ESP_FAIL;
        goto cleanup;
    }

    ESP_LOGI(TAG, "RainMaker set params succeeded, status=%d", status_code);

cleanup:
    free(response);
    return ret;
}
