// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_err.h"
#include <esp_check.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <esp_matter_mem.h>
#include <esp_matter_ota_http_downloader.h>
#include <sdkconfig.h>

static constexpr char TAG[] = "ota_provider";

namespace esp_matter {
namespace ota_provider {

static bool _process_again(int status_code)
{
    switch (status_code) {
    case HttpStatus_MovedPermanently:
    case HttpStatus_Found:
    case HttpStatus_TemporaryRedirect:
    case HttpStatus_Unauthorized:
        return true;
    default:
        return false;
    }
    return false;
}

static esp_err_t _http_handle_response_code(esp_http_client_handle_t http_client, int status_code)
{
    esp_err_t err = ESP_OK;
    if (status_code == HttpStatus_MovedPermanently || status_code == HttpStatus_Found ||
        status_code == HttpStatus_TemporaryRedirect) {
        err = esp_http_client_set_redirection(http_client);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "URL redirection Failed");
            return err;
        }
    } else if (status_code == HttpStatus_Unauthorized) {
        esp_http_client_add_auth(http_client);
    } else if (status_code == HttpStatus_NotFound || status_code == HttpStatus_Forbidden) {
        ESP_LOGE(TAG, "File not found(%d)", status_code);
        return ESP_FAIL;
    } else if (status_code >= HttpStatus_BadRequest && status_code < HttpStatus_InternalError) {
        ESP_LOGE(TAG, "Client error (%d)", status_code);
        return ESP_FAIL;
    } else if (status_code >= HttpStatus_InternalError) {
        ESP_LOGE(TAG, "Server error (%d)", status_code);
        return ESP_FAIL;
    }

    char upgrade_data_buf[256];
    // process_again() returns true only in case of redirection.
    if (_process_again(status_code)) {
        while (1) {
            // In case of redirection, esp_http_client_read() is called
            // to clear the response buffer of http_client.
            int data_read = esp_http_client_read(http_client, upgrade_data_buf, sizeof(upgrade_data_buf));
            if (data_read <= 0) {
                return ESP_OK;
            }
        }
    }
    return ESP_OK;
}

static esp_err_t _http_connect(esp_http_client_handle_t http_client)
{
    esp_err_t err = ESP_OK;
    int status_code, header_ret;
    do {
        char *post_data = NULL;
        /* Send POST request if body is set.
         * Note: Sending POST request is not supported if partial_http_download
         * is enabled
         */
        int post_len = esp_http_client_get_post_field(http_client, &post_data);
        err = esp_http_client_open(http_client, post_len);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
            return err;
        }
        if (post_len) {
            int write_len = 0;
            while (post_len > 0) {
                write_len = esp_http_client_write(http_client, post_data, post_len);
                if (write_len < 0) {
                    ESP_LOGE(TAG, "Write failed");
                    return ESP_FAIL;
                }
                post_len -= write_len;
                post_data += write_len;
            }
        }
        header_ret = esp_http_client_fetch_headers(http_client);
        if (header_ret < 0) {
            return header_ret;
        }
        status_code = esp_http_client_get_status_code(http_client);
        err = _http_handle_response_code(http_client, status_code);
        if (err != ESP_OK) {
            return err;
        }
    } while (_process_again(status_code));
    return err;
}

static int _http_client_read_check_connection(esp_http_client_handle_t client, char *data, size_t size)
{
    int len = esp_http_client_read(client, data, size);
    if (len == 0 && !esp_http_client_is_complete_data_received(client) &&
        (errno == ENOTCONN || errno == ECONNRESET || errno == ECONNABORTED)) {
        return -1;
    }
    return len;
}

static int _http_client_read(esp_http_client_handle_t http_client, char *data, size_t size)
{
    int read_len = 0;
    while (read_len < size) {
        int len = _http_client_read_check_connection(http_client, data, size - read_len);
        if (esp_http_client_is_complete_data_received(http_client)) {
            ESP_LOGI(TAG, "Finish downloading");
            return read_len + len;
        } else if (len < 0) {
            ESP_LOGE(TAG, "Failed to read image");
            return read_len;
        }
        read_len += len;
    }
    return read_len;
}

static void _http_client_cleanup(esp_http_client_handle_t client)
{
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

int http_downloader_read(esp_http_client_handle_t http_client, char *buf, size_t size)
{
    if (!http_client) {
        return -1;
    }
    return _http_client_read(http_client, buf, size);
}

void http_downloader_abort(esp_http_client_handle_t http_client)
{
    if (http_client) {
        _http_client_cleanup(http_client);
    }
}

esp_err_t http_downloader_start(esp_http_client_config_t *config, esp_http_client_handle_t *http_client)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(http_client, ESP_ERR_INVALID_ARG, TAG, "http_client cannot be NULL");
    *http_client = esp_http_client_init(config);
    ESP_RETURN_ON_FALSE(*http_client, ESP_ERR_NO_MEM, TAG, "Failed to initialize http client");
    ESP_GOTO_ON_ERROR(_http_connect(*http_client), exit, TAG, "Failed to connect to HTTP server");
    return ESP_OK;
exit:
    _http_client_cleanup(*http_client);
    *http_client = nullptr;
    return ret;
}

} // namespace ota_provider
} // namespace esp_matter
