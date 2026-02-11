/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string>
#include <cstring>
#include <memory>
#include <functional>
#include <esp_http_client.h>
#include <esp_log.h>
#include <cJSON.h>
#include "esp_crt_bundle.h"
#include "rainmaker_api.h"

static const char *TAG = "RM_API";

static const char* rainmaker_login_url = "/v1/login2";
static const char* rainmaker_user_info_url = "/v1/user2";
static const char* rainmaker_nodes_url = "/v1/user/nodes";
static const char* rainmaker_nodes_params_url = "/v1/user/nodes/params";
static const char* rainmaker_nodes_config_url = "/v1/user/nodes/config";
static const char* rainmaker_group_url = "/v1/user/node_group";
static const char* rainmaker_node_mapping_url = "/v1/user/nodes/mapping";
static const char* rainmaker_node_mapping_payload = "{\"secret_key\":\"%s\",\"node_id\":\"%s\",\"operation\":\"%s\"}";
static const char* rainmaker_node_mapping_payload_remove = "{\"node_id\":\"%s\",\"operation\":\"remove\"}";

#define RAINMAKER_URL_LEN 256

/* RAII wrapper for HTTP client */
class HttpClientWrapper {
public:
    explicit HttpClientWrapper(const esp_http_client_config_t &config)
        : client_(esp_http_client_init(&config)) {}

    ~HttpClientWrapper()
    {
        if (client_) {
            esp_http_client_cleanup(client_);
        }
    }

    esp_http_client_handle_t get() const
    {
        return client_;
    }
    bool is_valid() const
    {
        return client_ != nullptr;
    }

    /* Non-copyable */
    HttpClientWrapper(const HttpClientWrapper &) = delete;
    HttpClientWrapper &operator=(const HttpClientWrapper &) = delete;

private:
    esp_http_client_handle_t client_;
};

/* RAII wrapper for JSON objects */
class JsonWrapper {
public:
    explicit JsonWrapper(cJSON* json) : json_(json) {}
    ~JsonWrapper()
    {
        if (json_) {
            cJSON_Delete(json_);
        }
    }

    cJSON* get() const
    {
        return json_;
    }
    cJSON* release()
    {
        cJSON* temp = json_;
        json_ = nullptr;
        return temp;
    }

    /* Non-copyable */
    JsonWrapper(const JsonWrapper &) = delete;
    JsonWrapper &operator=(const JsonWrapper &) = delete;

private:
    cJSON* json_;
};

/* RAII wrapper for malloc'd memory */
class MallocWrapper {
public:
    explicit MallocWrapper(void* ptr) : ptr_(ptr) {}
    ~MallocWrapper()
    {
        if (ptr_) {
            free(ptr_);
        }
    }

    void* get() const
    {
        return ptr_;
    }
    void* release()
    {
        void* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }

    /* Non-copyable */
    MallocWrapper(const MallocWrapper &) = delete;
    MallocWrapper &operator=(const MallocWrapper &) = delete;

private:
    void* ptr_;
};

/* Helper function to make HTTP request */
esp_err_t RainmakerApi::MakeHttpRequest(esp_http_client_handle_t client, const char* post_data)
{
    if (!client) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = esp_http_client_open(client, post_data ? strlen(post_data) : 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        return err;
    }

    if (post_data) {
        ESP_LOGI(TAG, "Sending data: %s", post_data);
        int wlen = esp_http_client_write(client, post_data, strlen(post_data));
        if (wlen < 0) {
            ESP_LOGE(TAG, "Write failed");
            return ESP_FAIL;
        }
    }

    int content_length = esp_http_client_fetch_headers(client);
    if (content_length < 0) {
        ESP_LOGE(TAG, "Failed to fetch headers");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "HTTP request sent successfully");
    return ESP_OK;
}

/* Helper function to read HTTP response */
esp_err_t RainmakerApi::ReadHttpResponse(esp_http_client_handle_t client, char** response_data)
{
    int content_length = esp_http_client_get_content_length(client);
    if (content_length <= 0) {
        ESP_LOGE(TAG, "Invalid content length: %d", content_length);
        return ESP_FAIL;
    }

    *response_data = static_cast<char*>(malloc(content_length + 1));
    if (!*response_data) {
        ESP_LOGE(TAG, "Failed to allocate memory for response");
        return ESP_ERR_NO_MEM;
    }

    int read_len = esp_http_client_read(client, *response_data, content_length);
    if (read_len <= 0) {
        ESP_LOGE(TAG, "Failed to read response");
        free(*response_data);
        *response_data = nullptr;
        return ESP_FAIL;
    }

    (*response_data)[read_len] = '\0';
    return ESP_OK;
}

/* Helper function to handle HTTP response and check for authentication errors */
esp_err_t RainmakerApi::HandleHttpResponse(esp_http_client_handle_t client, char** response_data,
                                           std::function<esp_err_t()> retry_func)
{
    int status_code = esp_http_client_get_status_code(client);
    esp_err_t err = ReadHttpResponse(client, response_data);

    if (err != ESP_OK) {
        return err;
    }

    if (!*response_data) {
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Status code: %d, response_data: %s", status_code, *response_data);

    /* Handle unauthorized error */
    if (status_code == 401 && strstr(*response_data, "Unauthorized")) {
        ESP_LOGE(TAG, "Access token expired, attempting re-login");
        free(*response_data);
        *response_data = nullptr;

        if (retry_func) {
            return retry_func();
        }
        return ESP_ERR_INVALID_STATE;
    }

    return (status_code == 200 || status_code == 207) ? ESP_OK : ESP_FAIL;
}

/* Helper function to create JSON payload with refresh token */
static char* CreateRefreshTokenPayload(const std::string &refresh_token)
{
    JsonWrapper root(cJSON_CreateObject());
    if (!root.get()) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return nullptr;
    }

    cJSON_AddStringToObject(root.get(), "refreshtoken", refresh_token.c_str());
    return cJSON_PrintUnformatted(root.get());
}

/* Helper function to parse login response */
static esp_err_t ParseLoginResponse(const char* response_data, std::string &access_token)
{
    JsonWrapper response(cJSON_Parse(response_data));
    if (!response.get()) {
        ESP_LOGE(TAG, "Failed to parse response JSON");
        return ESP_FAIL;
    }

    cJSON *status = cJSON_GetObjectItem(response.get(), "status");
    if (!status || !status->valuestring || strcmp(status->valuestring, "success") != 0) {
        cJSON *description = cJSON_GetObjectItem(response.get(), "description");
        ESP_LOGE(TAG, "Login failed: %s",
                 description && description->valuestring ? description->valuestring : "Unknown error");
        return ESP_FAIL;
    }

    cJSON *access_token_json = cJSON_GetObjectItem(response.get(), "accesstoken");
    if (!access_token_json || !access_token_json->valuestring) {
        ESP_LOGE(TAG, "No access token in response");
        return ESP_FAIL;
    }

    access_token = access_token_json->valuestring;
    ESP_LOGI(TAG, "Access token saved successfully");
    return ESP_OK;
}

/* Helper function to set common HTTP headers */
static void SetCommonHeaders(esp_http_client_handle_t client, const std::string &access_token,
                             bool is_json_content = false)
{
    if (!access_token.empty()) {
        esp_http_client_set_header(client, "Authorization", access_token.c_str());
    }
    esp_http_client_set_header(client, "accept", "application/json");
    if (is_json_content) {
        esp_http_client_set_header(client, "Content-Type", "application/json");
    }
}

RainmakerApi::RainmakerApi() : access_token_(""), refresh_token_(""), base_url_(""), user_id_("") {}

RainmakerApi::~RainmakerApi()
{
    access_token_.clear();
    refresh_token_.clear();
    base_url_.clear();
    user_id_.clear();
}

esp_err_t RainmakerApi::Login(void)
{
    if (refresh_token_.empty()) {
        ESP_LOGE(TAG, "Refresh token not available");
        return ESP_ERR_INVALID_STATE;
    }

    char url[RAINMAKER_URL_LEN];
    snprintf(url, sizeof(url), "%s%s", base_url_.c_str(), rainmaker_login_url);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    /* Create JSON payload */
    MallocWrapper post_data(CreateRefreshTokenPayload(refresh_token_));
    if (!post_data.get()) {
        ESP_LOGE(TAG, "Failed to create post data");
        return ESP_ERR_NO_MEM;
    }

    /* Set headers */
    SetCommonHeaders(client.get(), "", true);
    esp_http_client_set_post_field(client.get(), static_cast<char*>(post_data.get()),
                                   strlen(static_cast<char*>(post_data.get())));

    esp_err_t err = MakeHttpRequest(client.get(), static_cast<char*>(post_data.get()));
    if (err != ESP_OK) {
        return err;
    }

    int status_code = esp_http_client_get_status_code(client.get());
    ESP_LOGI(TAG, "HTTP POST Status = %d", status_code);

    if (status_code != 200) {
        ESP_LOGE(TAG, "HTTP POST request failed with status code: %d", status_code);
        return ESP_FAIL;
    }

    char* response_data = nullptr;
    err = ReadHttpResponse(client.get(), &response_data);
    if (err != ESP_OK) {
        return err;
    }

    MallocWrapper response_wrapper(response_data);
    ESP_LOGD(TAG, "Response data: %s", response_data);

    return ParseLoginResponse(response_data, access_token_);
}

esp_err_t RainmakerApi::GetUserInfo(void)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return GetUserInfo();
        }
        return ESP_FAIL;
    }

    char url[RAINMAKER_URL_LEN];
    snprintf(url, sizeof(url), "%s%s", base_url_.c_str(), rainmaker_user_info_url);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    SetCommonHeaders(client.get(), access_token_);

    esp_err_t err = MakeHttpRequest(client.get());
    if (err != ESP_OK) {
        return ESP_FAIL;
    }

    char* response_data = nullptr;
    auto retry_func = [this]() -> esp_err_t {
        if (Login() == ESP_OK) {
            return GetUserInfo();
        }
        return ESP_FAIL;
    };

    err = HandleHttpResponse(client.get(), &response_data, retry_func);
    if (err == ESP_OK) {
        JsonWrapper response(cJSON_Parse(response_data));
        if (response.get()) {
            cJSON *user_id = cJSON_GetObjectItem(response.get(), "user_id");
            if (user_id && user_id->valuestring) {
                user_id_ = user_id->valuestring;
            }
        }
    }
    if (response_data) {
        free(response_data);
    }
    return ESP_OK;
}

esp_err_t RainmakerApi::GetNodesRecursive(const char* start_id)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return GetNodesRecursive(start_id);
        }
        return ESP_ERR_INVALID_STATE;
    }

    char url[RAINMAKER_URL_LEN];
    const char* base_params = "?node_details=true&status=true&config=false&params=true&show_tags=false&is_matter=false";

    if (start_id) {
        snprintf(url, sizeof(url), "%s%s%s&start_id=%s",
                 base_url_.c_str(), rainmaker_nodes_url, base_params, start_id);
    } else {
        snprintf(url, sizeof(url), "%s%s%s",
                 base_url_.c_str(), rainmaker_nodes_url, base_params);
    }

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    SetCommonHeaders(client.get(), access_token_);

    esp_err_t err = MakeHttpRequest(client.get());
    if (err != ESP_OK) {
        return err;
    }

    char* response_data = nullptr;
    auto retry_func = [this, start_id]() -> esp_err_t {
        if (Login() == ESP_OK) {
            return GetNodesRecursive(start_id);
        }
        return ESP_ERR_INVALID_STATE;
    };

    err = HandleHttpResponse(client.get(), &response_data, retry_func);
    if (err != ESP_OK) {
        return err;
    }

    MallocWrapper response_wrapper(response_data);
    ESP_LOGD(TAG, "Nodes response: %s", response_data);

    /* Check if there are more devices to fetch */
    JsonWrapper response_json(cJSON_Parse(response_data));
    if (response_json.get()) {
        cJSON *next_id = cJSON_GetObjectItem(response_json.get(), "next_id");
        if (next_id && next_id->valuestring) {
            ESP_LOGD(TAG, "Found next_id: %s, fetching next batch", next_id->valuestring);
            /* Recursively fetch next batch of devices */
            return GetNodesRecursive(next_id->valuestring);
        }
    }

    return ESP_OK;
}

esp_err_t RainmakerApi::RefreshNodes(void)
{
    return GetNodesRecursive(nullptr);
}

char* RainmakerApi::GetNodeList(void)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return GetNodeList();
        }
        return nullptr;
    }

    char url[RAINMAKER_URL_LEN];
    const char* base_params = "?node_details=false&status=false&config=false&params=false&show_tags=false&is_matter=false";

    snprintf(url, sizeof(url), "%s%s%s",
             base_url_.c_str(), rainmaker_nodes_url, base_params);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return nullptr;
    }

    SetCommonHeaders(client.get(), access_token_);

    esp_err_t err = MakeHttpRequest(client.get());
    if (err != ESP_OK) {
        return nullptr;
    }

    char* response_data = nullptr;
    auto retry_func = [this]() -> esp_err_t {
        if (Login() == ESP_OK) {
            return GetNodeList() ? ESP_OK : ESP_FAIL;
        }
        return ESP_FAIL;
    };

    err = HandleHttpResponse(client.get(), &response_data, retry_func);
    if (err != ESP_OK && response_data) {
        free(response_data);
        return nullptr;
    }

    return response_data;
}

char* RainmakerApi::GetNodeConfig(const char* node_id)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return GetNodeConfig(node_id);
        }
        return nullptr;
    }

    char url[RAINMAKER_URL_LEN];
    snprintf(url, sizeof(url), "%s%s?node_id=%s", base_url_.c_str(), rainmaker_nodes_config_url, node_id);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return nullptr;
    }

    SetCommonHeaders(client.get(), access_token_);

    esp_err_t err = MakeHttpRequest(client.get());
    if (err != ESP_OK) {
        return nullptr;
    }

    char* response_data = nullptr;
    auto retry_func = [this, node_id]() -> char* {
        if (Login() == ESP_OK) {
            return GetNodeConfig(node_id);
        }
        return nullptr;
    };

    err = HandleHttpResponse(client.get(), &response_data, [&retry_func]() -> esp_err_t {
        char* result = retry_func();
        return result ? ESP_OK : ESP_ERR_INVALID_STATE;
    });

    if (err != ESP_OK && response_data) {
        free(response_data);
        return nullptr;
    }

    return response_data;
}

esp_err_t RainmakerApi::SetNodeParams(const char* payload)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return SetNodeParams(payload);
        }
        return ESP_ERR_INVALID_STATE;
    }

    char url[RAINMAKER_URL_LEN];
    snprintf(url, sizeof(url), "%s%s", base_url_.c_str(), rainmaker_nodes_params_url);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_PUT,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    SetCommonHeaders(client.get(), access_token_, true);
    esp_http_client_set_post_field(client.get(), payload, strlen(payload));

    esp_err_t err = MakeHttpRequest(client.get(), payload);
    if (err != ESP_OK) {
        return err;
    }

    char* response_data = nullptr;
    auto retry_func = [this, payload]() -> esp_err_t {
        if (Login() == ESP_OK) {
            return SetNodeParams(payload);
        }
        return ESP_ERR_INVALID_STATE;
    };

    err = HandleHttpResponse(client.get(), &response_data, retry_func);
    if (response_data) {
        free(response_data);
    }
    return err;
}

char* RainmakerApi::GetNodeParams(const char* node_id)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return GetNodeParams(node_id);
        }
        return nullptr;
    }

    char url[RAINMAKER_URL_LEN];
    snprintf(url, sizeof(url), "%s%s?node_id=%s", base_url_.c_str(), rainmaker_nodes_params_url, node_id);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return nullptr;
    }

    SetCommonHeaders(client.get(), access_token_);

    esp_err_t err = MakeHttpRequest(client.get());
    if (err != ESP_OK) {
        return nullptr;
    }

    char* response_data = nullptr;
    auto retry_func = [this, node_id]() -> char* {
        if (Login() == ESP_OK) {
            return GetNodeParams(node_id);
        }
        return nullptr;
    };

    err = HandleHttpResponse(client.get(), &response_data, [&retry_func]() -> esp_err_t {
        char* result = retry_func();
        return result ? ESP_OK : ESP_ERR_INVALID_STATE;
    });

    if (err != ESP_OK && response_data) {
        free(response_data);
        return nullptr;
    }

    return response_data;
}

esp_err_t RainmakerApi::SetRefreshToken(const char* refresh_token)
{
    if (!refresh_token) {
        ESP_LOGE(TAG, "Refresh token is null");
        return ESP_ERR_INVALID_ARG;
    }

    refresh_token_ = refresh_token;
    access_token_.clear();
    user_id_.clear();
    return ESP_OK;
}

esp_err_t RainmakerApi::SetBaseUrl(const char* base_url)
{
    if (!base_url) {
        ESP_LOGE(TAG, "Base URL is null");
        return ESP_ERR_INVALID_ARG;
    }
    base_url_ = base_url;
    return ESP_OK;
}

esp_err_t RainmakerApi::DeleteRefreshToken(void)
{
    refresh_token_.clear();
    access_token_.clear();
    user_id_.clear();
    return ESP_OK;
}

char* RainmakerApi::GetGroup(void)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return GetGroup();
        }
        return nullptr;
    }

    char url[RAINMAKER_URL_LEN];
    snprintf(url, sizeof(url), "%s%s?node_list=true", base_url_.c_str(), rainmaker_group_url);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return nullptr;
    }

    SetCommonHeaders(client.get(), access_token_);

    esp_err_t err = MakeHttpRequest(client.get());
    if (err != ESP_OK) {
        return nullptr;
    }

    char* response_data = nullptr;
    auto retry_func = [this]() -> char* {
        if (Login() == ESP_OK) {
            return GetGroup();
        }
        return nullptr;
    };

    err = HandleHttpResponse(client.get(), &response_data, [&retry_func]() -> esp_err_t {
        char* result = retry_func();
        return result ? ESP_OK : ESP_ERR_INVALID_STATE;
    });

    if (err != ESP_OK && response_data) {
        free(response_data);
        return nullptr;
    }

    return response_data;
}

esp_err_t RainmakerApi::CreateGroup(const char* group_name)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return CreateGroup(group_name);
        }
        return ESP_ERR_INVALID_STATE;
    }

    char url[RAINMAKER_URL_LEN];
    snprintf(url, sizeof(url), "%s%s", base_url_.c_str(), rainmaker_group_url);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    /* Create JSON payload */
    JsonWrapper root(cJSON_CreateObject());
    if (!root.get()) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return ESP_ERR_NO_MEM;
    }

    cJSON_AddStringToObject(root.get(), "group_name", group_name);
    MallocWrapper post_data(cJSON_PrintUnformatted(root.get()));
    if (!post_data.get()) {
        ESP_LOGE(TAG, "Failed to create post data");
        return ESP_ERR_NO_MEM;
    }

    /* Set headers */
    SetCommonHeaders(client.get(), access_token_, true);
    esp_http_client_set_post_field(client.get(), static_cast<char*>(post_data.get()),
                                   strlen(static_cast<char*>(post_data.get())));

    esp_err_t err = MakeHttpRequest(client.get(), static_cast<char*>(post_data.get()));
    if (err != ESP_OK) {
        return err;
    }

    char* response_data = nullptr;
    auto retry_func = [this, group_name]() -> esp_err_t {
        if (Login() == ESP_OK) {
            return CreateGroup(group_name);
        }
        return ESP_ERR_INVALID_STATE;
    };

    err = HandleHttpResponse(client.get(), &response_data, retry_func);
    if (response_data) {
        free(response_data);
    }
    return err;
}

esp_err_t RainmakerApi::DeleteGroup(const char* group_id)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return DeleteGroup(group_id);
        }
        return ESP_ERR_INVALID_STATE;
    }

    char url[RAINMAKER_URL_LEN];
    snprintf(url, sizeof(url), "%s%s?group_id=%s", base_url_.c_str(), rainmaker_group_url, group_id);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_DELETE,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    /* Set headers */
    SetCommonHeaders(client.get(), access_token_, true);

    esp_err_t err = MakeHttpRequest(client.get());
    if (err != ESP_OK) {
        return err;
    }

    char* response_data = nullptr;
    auto retry_func = [this, group_id]() -> esp_err_t {
        if (Login() == ESP_OK) {
            return DeleteGroup(group_id);
        }
        return ESP_ERR_INVALID_STATE;
    };

    err = HandleHttpResponse(client.get(), &response_data, retry_func);
    if (response_data) {
        free(response_data);
    }
    return err;
}

esp_err_t RainmakerApi::OperateNodeToGroup(const char* node_id, const char* group_id,
                                           esp_rainmaker_api_group_operation_type_t operation_type)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return OperateNodeToGroup(node_id, group_id, operation_type);
        }
        return ESP_ERR_INVALID_STATE;
    }

    char url[RAINMAKER_URL_LEN];
    snprintf(url, sizeof(url), "%s%s?group_id=%s", base_url_.c_str(), rainmaker_group_url, group_id);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_PUT,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    /* Create JSON payload */
    JsonWrapper root(cJSON_CreateObject());
    if (!root.get()) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return ESP_ERR_NO_MEM;
    }

    const char* operation = (operation_type == ESP_RAINMAKER_API_ADD_NODE_TO_GROUP) ? "add" : "remove";
    cJSON_AddStringToObject(root.get(), "operation", operation);

    cJSON *nodes_array = cJSON_CreateArray();
    if (!nodes_array) {
        ESP_LOGE(TAG, "Failed to create nodes array");
        return ESP_ERR_NO_MEM;
    }

    cJSON_AddItemToArray(nodes_array, cJSON_CreateString(node_id));
    cJSON_AddItemToObject(root.get(), "nodes", nodes_array);

    MallocWrapper post_data(cJSON_PrintUnformatted(root.get()));
    if (!post_data.get()) {
        ESP_LOGE(TAG, "Failed to create post data");
        return ESP_ERR_NO_MEM;
    }

    /* Set headers */
    SetCommonHeaders(client.get(), access_token_, true);
    esp_http_client_set_post_field(client.get(), static_cast<char*>(post_data.get()),
                                   strlen(static_cast<char*>(post_data.get())));

    esp_err_t err = MakeHttpRequest(client.get(), static_cast<char*>(post_data.get()));
    if (err != ESP_OK) {
        return err;
    }

    char* response_data = nullptr;
    auto retry_func = [this, node_id, group_id, operation_type]() -> esp_err_t {
        if (Login() == ESP_OK) {
            return OperateNodeToGroup(node_id, group_id, operation_type);
        }
        return ESP_ERR_INVALID_STATE;
    };

    err = HandleHttpResponse(client.get(), &response_data, retry_func);
    if (response_data) {
        free(response_data);
    }
    return err;
}

static esp_err_t ParseNodeMappingResponse(char *response_data, char *request_id)
{
    esp_err_t err = ESP_OK;
    JsonWrapper response(cJSON_Parse(response_data));
    if (response.get()) {
        /* Parse the response to extract status and request_id */
        cJSON *status_json = cJSON_GetObjectItem(response.get(), "status");
        if (status_json && status_json->valuestring) {
            if (strcmp(status_json->valuestring, "success") == 0) {
                cJSON *request_id_json = cJSON_GetObjectItem(response.get(), "request_id");
                if (request_id_json && request_id_json->valuestring && request_id) {
                    strncpy(request_id, request_id_json->valuestring, strlen(request_id_json->valuestring));
                    request_id[strlen(request_id_json->valuestring)] = '\0';  /* Ensure null termination */
                    ESP_LOGI(TAG, "Request ID extracted: %s", request_id);
                }
            } else {
                ESP_LOGE(TAG, "Failed to set node mapping, status: %s", status_json->valuestring);
                err = ESP_FAIL;
            }
        }
    } else {
        ESP_LOGE(TAG, "Failed to parse node mapping response");
        err = ESP_FAIL;
    }
    return err;
}

esp_err_t RainmakerApi::SetNodeMapping(const char* user_id, const char* secret_key, const char* node_id,
                                       esp_rainmaker_api_node_mapping_operation_type_t operation_type, char *request_id)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return SetNodeMapping(user_id, secret_key, node_id, operation_type, request_id);
        }
        return ESP_ERR_INVALID_STATE;
    }

    char url[RAINMAKER_URL_LEN];
    snprintf(url, sizeof(url), "%s%s", base_url_.c_str(), rainmaker_node_mapping_url);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_PUT,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    const char* operation = (operation_type == ESP_RAINMAKER_API_ADD_NODE_MAPPING) ? "add" : "remove";
    char payload[256] = {0};
    if (operation_type == ESP_RAINMAKER_API_ADD_NODE_MAPPING) {
        snprintf(payload, sizeof(payload), rainmaker_node_mapping_payload, secret_key, node_id, operation);
    } else if (operation_type == ESP_RAINMAKER_API_REMOVE_NODE_MAPPING) {
        snprintf(payload, sizeof(payload), rainmaker_node_mapping_payload_remove, node_id);
    } else {
        ESP_LOGE(TAG, "Invalid operation type");
        return ESP_FAIL;
    }

    SetCommonHeaders(client.get(), access_token_, true);
    esp_http_client_set_post_field(client.get(), payload, strlen(payload));

    esp_err_t err = MakeHttpRequest(client.get(), payload);
    if (err != ESP_OK) {
        return err;
    }

    char* response_data = nullptr;
    auto retry_func = [this, user_id, secret_key, node_id, operation_type, request_id]() -> esp_err_t {
        if (Login() == ESP_OK) {
            return SetNodeMapping(user_id, secret_key, node_id, operation_type, request_id);
        }
        return ESP_ERR_INVALID_STATE;
    };

    err = HandleHttpResponse(client.get(), &response_data, retry_func);
    if (response_data) {
        if (err == ESP_OK) {
            err = ParseNodeMappingResponse(response_data, request_id);
        }
        free(response_data);
    }
    return err;
}

static esp_rainmaker_api_node_mapping_status_type_t ParseNodeMappingStatusResponse(char *response_data)
{
    esp_rainmaker_api_node_mapping_status_type_t status = ESP_RAINMAKER_API_NODE_MAPPING_STATUS_INTERNAL_ERROR;
    JsonWrapper response(cJSON_Parse(response_data));
    if (response.get()) {
        cJSON *status_json = cJSON_GetObjectItem(response.get(), "request_status");
        if (status_json && status_json->valuestring) {
            if (strcmp(status_json->valuestring, "requested") == 0) {
                status = ESP_RAINMAKER_API_NODE_MAPPING_STATUS_REQUESTED;
            } else if (strcmp(status_json->valuestring, "confirmed") == 0) {
                status = ESP_RAINMAKER_API_NODE_MAPPING_STATUS_CONFIRMED;
            } else if (strcmp(status_json->valuestring, "timeout") == 0) {
                status = ESP_RAINMAKER_API_NODE_MAPPING_STATUS_TIMEOUT;
            } else if (strcmp(status_json->valuestring, "discarded") == 0) {
                status = ESP_RAINMAKER_API_NODE_MAPPING_STATUS_DISCARDED;
            } else {
                ESP_LOGE(TAG, "Unknown node mapping status: %s", status_json->valuestring);
                status = ESP_RAINMAKER_API_NODE_MAPPING_STATUS_INTERNAL_ERROR;
            }
        }
    } else {
        ESP_LOGE(TAG, "Failed to parse node mapping status response");
        status = ESP_RAINMAKER_API_NODE_MAPPING_STATUS_INTERNAL_ERROR;
    }
    return status;
}

esp_rainmaker_api_node_mapping_status_type_t RainmakerApi::GetNodeMappingStatus(const char *request_id)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return GetNodeMappingStatus(request_id);
        }
        return ESP_RAINMAKER_API_NODE_MAPPING_STATUS_INTERNAL_ERROR;
    }

    char url[RAINMAKER_URL_LEN];
    snprintf(url, sizeof(url), "%s%s?request_id=%s", base_url_.c_str(), rainmaker_node_mapping_url, request_id);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_RAINMAKER_API_NODE_MAPPING_STATUS_INTERNAL_ERROR;
    }

    SetCommonHeaders(client.get(), access_token_);

    esp_err_t err = MakeHttpRequest(client.get());
    if (err != ESP_OK) {
        return ESP_RAINMAKER_API_NODE_MAPPING_STATUS_INTERNAL_ERROR;
    }

    char* response_data = nullptr;
    auto retry_func = [this, request_id]() -> esp_rainmaker_api_node_mapping_status_type_t {
        if (Login() == ESP_OK) {
            return GetNodeMappingStatus(request_id);
        }
        return ESP_RAINMAKER_API_NODE_MAPPING_STATUS_INTERNAL_ERROR;
    };

    esp_rainmaker_api_node_mapping_status_type_t status = ESP_RAINMAKER_API_NODE_MAPPING_STATUS_INTERNAL_ERROR;
    err = HandleHttpResponse(client.get(), &response_data, retry_func);
    if (err == ESP_OK) {
        status = ParseNodeMappingStatusResponse(response_data);
    }
    if (response_data) {
        free(response_data);
    }
    return status;
}

const std::string &RainmakerApi::GetUserId() const
{
    return user_id_;
}

esp_err_t RainmakerApi::GetNodeConnectionStatus(const char *node_id, bool *connection_status)
{
    if (access_token_.empty()) {
        ESP_LOGE(TAG, "Access token not available, need login first");
        if (Login() == ESP_OK) {
            return GetNodeConnectionStatus(node_id, connection_status);
        }
        return ESP_ERR_INVALID_STATE;
    }

    char url[RAINMAKER_URL_LEN] = {0};
    snprintf(url, sizeof(url), "%s%s?node_id=%s&node_details=false&status=true&config=false&params=false&show_tags=false&is_matter=false",
             base_url_.c_str(), rainmaker_nodes_url, node_id);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .buffer_size_tx = 2048,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    HttpClientWrapper client(config);
    if (!client.is_valid()) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_ERR_INVALID_STATE;
    }

    SetCommonHeaders(client.get(), access_token_);

    esp_err_t err = MakeHttpRequest(client.get());
    if (err != ESP_OK) {
        return ESP_ERR_INVALID_STATE;
    }

    char* response_data = nullptr;
    auto retry_func = [this, node_id, connection_status]() -> esp_err_t {
        if (Login() == ESP_OK) {
            return GetNodeConnectionStatus(node_id, connection_status);
        }
        return ESP_ERR_INVALID_STATE;
    };

    err = HandleHttpResponse(client.get(), &response_data, retry_func);
    if (err != ESP_OK) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Parse response data, extract connected field */
    if (!response_data) {
        ESP_LOGE(TAG, "No response data received");
        return ESP_ERR_INVALID_RESPONSE;
    }

    JsonWrapper root(cJSON_Parse(response_data));
    if (!root.get()) {
        ESP_LOGE(TAG, "Failed to parse response JSON");
        free(response_data);
        return ESP_ERR_INVALID_RESPONSE;
    }

    cJSON *node_details = cJSON_GetObjectItem(root.get(), "node_details");
    if (!node_details || !cJSON_IsArray(node_details)) {
        ESP_LOGE(TAG, "No node_details object in response");
        free(response_data);
        return ESP_ERR_INVALID_RESPONSE;
    }

    cJSON *node = cJSON_GetArrayItem(node_details, 0);
    if (!node) {
        ESP_LOGE(TAG, "No node object in node_details array");
        free(response_data);
        return ESP_ERR_INVALID_RESPONSE;
    }

    cJSON *status = cJSON_GetObjectItem(node, "status");
    if (!status) {
        ESP_LOGE(TAG, "No status object in node");
        free(response_data);
        return ESP_ERR_INVALID_RESPONSE;
    }

    cJSON *connectivity = cJSON_GetObjectItem(status, "connectivity");
    if (!connectivity) {
        ESP_LOGE(TAG, "No connectivity object in status");
        free(response_data);
        return ESP_ERR_INVALID_RESPONSE;
    }

    cJSON *connected = cJSON_GetObjectItem(connectivity, "connected");
    if (!connected || !cJSON_IsBool(connected)) {
        ESP_LOGE(TAG, "No connected field in connectivity");
        free(response_data);
        return ESP_ERR_INVALID_RESPONSE;
    }

    if (connection_status) {
        *connection_status = cJSON_IsTrue(connected);
    }

    free(response_data);

    return ESP_OK;
}

esp_err_t esp_rainmaker_api_login(void)
{
    return RainmakerApi::GetInstance().Login();
}

esp_err_t esp_rainmaker_api_refresh_nodes(void)
{
    return ESP_OK;
}

char* esp_rainmaker_api_get_nodes_list(void)
{
    return RainmakerApi::GetInstance().GetNodeList();
}

char* esp_rainmaker_api_get_node_config(const char* node_id)
{
    return RainmakerApi::GetInstance().GetNodeConfig(node_id);
}

esp_err_t esp_rainmaker_api_set_node_params(const char* payload)
{
    return RainmakerApi::GetInstance().SetNodeParams(payload);
}

char* esp_rainmaker_api_get_node_params(const char* node_id)
{
    return RainmakerApi::GetInstance().GetNodeParams(node_id);
}

esp_err_t esp_rainmaker_api_set_refresh_token(const char* refresh_token)
{
    return RainmakerApi::GetInstance().SetRefreshToken(refresh_token);
}

esp_err_t esp_rainmaker_api_set_base_url(const char* base_url)
{
    return RainmakerApi::GetInstance().SetBaseUrl(base_url);
}

esp_err_t esp_rainmaker_api_delete_refresh_token(void)
{
    return RainmakerApi::GetInstance().DeleteRefreshToken();
}

char* esp_rainmaker_api_get_group(void)
{
    return RainmakerApi::GetInstance().GetGroup();
}

esp_err_t esp_rainmaker_api_create_group(const char* group_name)
{
    return RainmakerApi::GetInstance().CreateGroup(group_name);
}

esp_err_t esp_rainmaker_api_delete_group(const char* group_id)
{
    return RainmakerApi::GetInstance().DeleteGroup(group_id);
}

esp_err_t esp_rainmaker_api_operate_node_to_group(const char* node_id, const char* group_id,
                                                  esp_rainmaker_api_group_operation_type_t operation_type)
{
    return RainmakerApi::GetInstance().OperateNodeToGroup(node_id, group_id, operation_type);
}

esp_err_t esp_rainmaker_api_set_node_mapping(const char* user_id, const char* secret_key, const char* node_id,
                                             esp_rainmaker_api_node_mapping_operation_type_t operation_type, char *request_id)
{
    return RainmakerApi::GetInstance().SetNodeMapping(user_id, secret_key, node_id, operation_type, request_id);
}

esp_rainmaker_api_node_mapping_status_type_t esp_rainmaker_api_get_node_mapping_status(const char *request_id)
{
    return RainmakerApi::GetInstance().GetNodeMappingStatus(request_id);
}

esp_err_t esp_rainmaker_api_get_node_connection_status(const char *node_id, bool *connection_status)
{
    return RainmakerApi::GetInstance().GetNodeConnectionStatus(node_id, connection_status);
}

char* esp_rainmaker_api_get_user_id(void)
{
    const std::string &user_id = RainmakerApi::GetInstance().GetUserId();
    if (user_id.empty()) {
        return nullptr;
    }
    return strdup(user_id.c_str());
}
