// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_crt_bundle.h>
#include <esp_err.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <esp_matter_mem.h>

#include <http_client_get.h>

constexpr char *TAG = "AttestationVerification";

typedef struct {
    on_http_data_t on_http_data;
    uint8_t *data;
    int fill_size;
    int size;
    void *ctx;
} http_info_t;

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    http_info_t *info = static_cast<http_info_t *>(evt->user_data);
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        if (!esp_http_client_is_chunked_response(evt->client)) {
            int content_len = esp_http_client_get_content_length(evt->client);
            if (info->data == nullptr && content_len) {
                info->data = (uint8_t *)esp_matter_mem_calloc(content_len + 1, 1);
                if (info->data) {
                    info->size = content_len;
                }
            }
            if (info->data && evt->data_len && info->fill_size + evt->data_len <= info->size) {
                memcpy(info->data + info->fill_size, evt->data, evt->data_len);
                info->fill_size += evt->data_len;
            }
        } else {
            // For chunked message
            if (evt->data_len > 0) {
                info->size += evt->data_len;
                uint8_t *cached_data = info->data;
                info->data = (uint8_t *)esp_matter_mem_calloc(info->size + 1, 1);
                if (info->data) {
                    if (cached_data) {
                        memcpy(info->data, cached_data, info->fill_size);
                        esp_matter_mem_free(cached_data);
                    }
                    memcpy(info->data + info->fill_size, evt->data, evt->data_len);
                    info->fill_size += evt->data_len;
                }
            }
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        if (info->fill_size && info->on_http_data) {
            http_resp_t resp = {
                .data = info->data,
                .size = info->fill_size,
            };
            info->on_http_data(&resp, info->ctx);
        }
        if (info->data) {
            esp_matter_mem_free(info->data);
        }
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        esp_http_client_set_redirection(evt->client);
        break;
    }
    return ESP_OK;
}

esp_err_t http_send_get_request(const char *url, char **headers, size_t headers_num, on_http_data_t on_http_data,
                                void *ctx)
{
    http_info_t info = {
        .on_http_data = on_http_data,
        .data = nullptr,
        .fill_size = 0,
        .size = 0,
        .ctx = ctx,
    };

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .user_data = &info,
        .skip_cert_common_name_check = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Fail to init client");
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < headers_num; ++i) {
        // Headers will be strings in the format of "<key>: <content>".
        char *dot = strchr(headers[i], ':');
        if (dot) {
            *dot = 0;
            char *cont = dot + 2;
            esp_http_client_set_header(client, headers[i], cont);
            *dot = ':';
        }
    }
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld", esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    return err;
}
