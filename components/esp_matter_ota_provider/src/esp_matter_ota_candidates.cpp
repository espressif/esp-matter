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

#include <algorithm>
#include <esp_check.h>
#include <esp_crt_bundle.h>
#include <esp_err.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <esp_matter_mem.h>
#include <esp_matter_ota_candidates.h>
#include <esp_matter_ota_provider.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <functional>
#include <json_parser.h>

#include <lib/support/ScopedBuffer.h>

#include <string.h>
#include "core/DataModelTypes.h"

using chip::Platform::ScopedMemoryBufferWithSize;

namespace esp_matter {
namespace ota_provider {

static constexpr char TAG[] = "ota_provider";
#if CONFIG_ESP_MATTER_OTA_PROVIDER_DCL_MAINNET
static constexpr char dcl_rest_url[] = "https://on.dcl.csa-iot.org/dcl/model/versions";
#elif CONFIG_ESP_MATTER_OTA_PROVIDER_DCL_TESTNET
static constexpr char dcl_rest_url[] = "https://on.test-net.dcl.csa-iot.org/dcl/model/versions";
#endif
static constexpr size_t max_ota_candidate_count = CONFIG_ESP_MATTER_MAX_OTA_CANDIDATES_COUNT;

static model_version_t *_ota_candidates_cache[max_ota_candidate_count];
static QueueHandle_t _ota_candidate_task_queue = NULL;
#ifdef CONFIG_ESP_MATTER_OTA_CANDIDATES_UPDATE_PERIODICALLY
static esp_timer_handle_t _ota_candidates_update_timer = NULL;
#endif

typedef struct {
    uint16_t vendor_id;
    uint16_t product_id;
    uint32_t software_version;
    fetch_ota_image_done_callback_t callback;
    void *callback_args;
} ota_candidate_fetch_action_t;

static bool _is_ota_candidate_valid(model_version_t *model, uint32_t current_software_version)
{
    return model->software_version > current_software_version &&
        model->max_applicable_software_version >= current_software_version &&
        model->min_applicable_software_version <= current_software_version;
}

static int _search_ota_candidate(uint16_t vendor_id, uint16_t product_id, uint32_t software_ver)
{
    for (size_t index = 0; index < max_ota_candidate_count; ++index) {
        model_version_t *cur_model = _ota_candidates_cache[index];
        if (cur_model) {
            if (cur_model->vendor_id == vendor_id && cur_model->product_id == product_id) {
                if (_is_ota_candidate_valid(cur_model, software_ver)) {
                    return index;
                } else {
                    // This candidate is not valid, expire it.
                    esp_matter_mem_free(cur_model);
                    _ota_candidates_cache[index] = nullptr;
                    return -1;
                }
            }
        }
    }
    return -1;
}

static size_t _find_empty_ota_candidates()
{
    uint8_t oldest_candidate_lifetime = 0;
    size_t oldest_candidate_index = 0;
    for (size_t index = 0; index < max_ota_candidate_count; ++index) {
        if (!_ota_candidates_cache[index]) {
            return index;
        } else {
            if (oldest_candidate_lifetime < _ota_candidates_cache[index]->lifetime) {
                oldest_candidate_lifetime = _ota_candidates_cache[index]->lifetime;
                oldest_candidate_index = index;
            }
        }
    }
    // expire the oldest candidate
    esp_matter_mem_free(_ota_candidates_cache[oldest_candidate_index]);
    _ota_candidates_cache[oldest_candidate_index] = nullptr;
    return oldest_candidate_index;
}

static void _increase_ota_candidates_lifetime()
{
    for (size_t index = 0; index < max_ota_candidate_count; ++index) {
        if (_ota_candidates_cache[index]) {
            _ota_candidates_cache[index]->lifetime++;
        }
    }
}

static esp_err_t _query_software_version_array(const uint16_t vendor_id, const uint16_t product_id,
                                               uint32_t **software_version_array, size_t &software_version_count)
{
    if (!software_version_array) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;
    int sw_ver_count = 0, sw_ver_index = 0, sw_ver_tmp;
    ;
    char url[100];
    snprintf(url, sizeof(url), "%s/%d/%d", dcl_rest_url, vendor_id, product_id);
    esp_http_client_config_t config = {
        .url = url,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .buffer_size = 1024,
        .skip_cert_common_name_check = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = NULL;
    ScopedMemoryBufferWithSize<char> http_payload;
    int http_len, http_status_code;
    jparse_ctx_t jctx;

    client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Failed to initialise HTTP Client.");
        return ESP_ERR_NO_MEM;
    }
    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "accept", "application/json"), cleanup, TAG,
                      "Failed to set http header accept");
    ESP_GOTO_ON_ERROR(esp_http_client_set_method(client, HTTP_METHOD_GET), cleanup, TAG, "Failed to set http method");

    // HTTP GET
    ESP_GOTO_ON_ERROR(esp_http_client_open(client, 0), cleanup, TAG, "Failed to open http connection");

    // Read Response
    http_len = esp_http_client_fetch_headers(client);
    http_status_code = esp_http_client_get_status_code(client);
    http_payload.Calloc(1024);
    if ((http_len > 0) && (http_status_code == 200)) {
        ESP_GOTO_ON_FALSE(http_payload.Get(), ESP_ERR_NO_MEM, close, TAG, "Failed to alloc memory for http_payload");
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize());
        http_payload[http_len] = '\0';
    } else {
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize());
        http_payload[http_len] = '\0';
        ESP_LOGE(TAG, "Invalid response for %s", url);
        ESP_LOGE(TAG, "Status = %d, Data = %s", http_status_code, http_len > 0 ? http_payload.Get() : "None");
        ret = ESP_FAIL;
        goto close;
    }
    ESP_LOGD(TAG, "http_response:\n%s", http_payload.Get());

    // Parse the response payload
    ESP_GOTO_ON_FALSE(json_parse_start(&jctx, http_payload.Get(), http_len) == 0, ESP_FAIL, close, TAG,
                      "Failed to parse the http response json on json_parse_start");
    if (json_obj_get_object(&jctx, "modelVersions") == 0) {
        if (json_obj_get_array(&jctx, "softwareVersions", &sw_ver_count) == 0 && sw_ver_count > 0) {
            *software_version_array = (uint32_t *)esp_matter_mem_calloc(sw_ver_count, sizeof(uint32_t));
            if (*software_version_array) {
                software_version_count = sw_ver_count;
                for (sw_ver_index = 0; sw_ver_index < sw_ver_count; ++sw_ver_index) {
                    if (json_arr_get_int(&jctx, sw_ver_index, &sw_ver_tmp) == 0) {
                        (*software_version_array)[sw_ver_index] = sw_ver_tmp;
                    } else {
                        (*software_version_array)[sw_ver_index] = 0;
                    }
                }
            } else {
                ret = ESP_ERR_NO_MEM;
            }
            json_obj_leave_array(&jctx);
        } else {
            ret = ESP_FAIL;
        }
        json_obj_leave_object(&jctx);
    } else {
        ret = ESP_FAIL;
    }
    json_parse_end(&jctx);

close:
    esp_http_client_close(client);
cleanup:
    esp_http_client_cleanup(client);

    if (ret != ESP_OK) {
        if (*software_version_array) {
            esp_matter_mem_free(*software_version_array);
            *software_version_array = nullptr;
            software_version_count = 0;
        }
    }
    return ret;
}

static esp_err_t _query_ota_candidate(model_version_t *model, uint32_t new_software_version,
                                      uint32_t current_software_version)
{
    if (!model) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;
    char url[128];
    snprintf(url, sizeof(url), "%s/%d/%d/%ld", dcl_rest_url, model->vendor_id, model->product_id, new_software_version);
    esp_http_client_config_t config = {
        .url = url,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .buffer_size = 1024,
        .skip_cert_common_name_check = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = NULL;
    ScopedMemoryBufferWithSize<char> http_payload;
    int http_len, http_status_code;
    int max_applicable_software_version, min_applicable_software_version, cd_version_number, string_len;
    bool software_version_valid;
    jparse_ctx_t jctx;

    client = esp_http_client_init(&config);
    ESP_RETURN_ON_FALSE(client, ESP_FAIL, TAG, "Failed to initialise HTTP Client.");
    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "accept", "application/json"), cleanup, TAG,
                      "Failed to set http header accept");
    ESP_GOTO_ON_ERROR(esp_http_client_set_method(client, HTTP_METHOD_GET), cleanup, TAG, "Failed to set http method");

    // HTTP GET
    ESP_GOTO_ON_ERROR(esp_http_client_open(client, 0), cleanup, TAG, "Failed to open http connection");

    // Read Response
    http_len = esp_http_client_fetch_headers(client);
    http_status_code = esp_http_client_get_status_code(client);
    http_payload.Calloc(1024);
    if ((http_len > 0) && (http_status_code == 200)) {
        ESP_GOTO_ON_FALSE(http_payload.Get(), ESP_ERR_NO_MEM, close, TAG, "Failed to alloc memory for http_payload");
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize());
        http_payload[http_len] = '\0';
    } else {
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize());
        http_payload[http_len] = '\0';
        ESP_LOGE(TAG, "Invalid response for %s", url);
        ESP_LOGE(TAG, "Status = %d, Data = %s", http_status_code, http_len > 0 ? http_payload.Get() : "None");
        ret = ESP_FAIL;
        goto close;
    }
    ESP_LOGD(TAG, "http_response:\n%s", http_payload.Get());

    ESP_GOTO_ON_FALSE(json_parse_start(&jctx, http_payload.Get(), http_len) == 0, ESP_FAIL, close, TAG,
                      "Failed to parse the http response json on json_parse_start");
    if (json_obj_get_object(&jctx, "modelVersion") == 0) {
        if (json_obj_get_int(&jctx, "maxApplicableSoftwareVersion", &max_applicable_software_version) == 0 &&
            json_obj_get_int(&jctx, "minApplicableSoftwareVersion", &min_applicable_software_version) == 0 &&
            json_obj_get_bool(&jctx, "softwareVersionValid", &software_version_valid) == 0 && software_version_valid &&
            max_applicable_software_version >= current_software_version &&
            min_applicable_software_version <= current_software_version &&
            new_software_version > current_software_version) {
            model->max_applicable_software_version = max_applicable_software_version;
            model->min_applicable_software_version = min_applicable_software_version;
            model->software_version = new_software_version;
            if (json_obj_get_int(&jctx, "cdVersionNumber", &cd_version_number) == 0) {
                model->cd_version_number = cd_version_number;
            }
            if (json_obj_get_strlen(&jctx, "softwareVersionString", &string_len) == 0 &&
                json_obj_get_string(&jctx, "softwareVersionString", model->software_version_str,
                                    sizeof(model->software_version_str)) == 0) {
                string_len = string_len < sizeof(model->software_version_str) - 1
                    ? string_len
                    : sizeof(model->software_version_str) - 1;
                model->software_version_str[string_len] = 0;
            }
            if (json_obj_get_strlen(&jctx, "otaUrl", &string_len) == 0 &&
                json_obj_get_string(&jctx, "otaUrl", model->ota_url, sizeof(model->ota_url)) == 0) {
                model->ota_url[string_len] = 0;
            }
        } else {
            ESP_LOGI(TAG, "This result is not valid for software version %ld, skip it", current_software_version);
            ret = ESP_ERR_NOT_FINISHED;
        }
        json_obj_leave_object(&jctx);
    } else {
        ret = ESP_FAIL;
    }
    json_parse_end(&jctx);

close:
    esp_http_client_close(client);
cleanup:
    esp_http_client_cleanup(client);
    return ret;
}

#ifdef CONFIG_ESP_MATTER_OTA_CANDIDATES_UPDATE_PERIODICALLY
static void _update_all_ota_candidates_cache()
{
    for (size_t index = 0; index < max_ota_candidate_count; ++index) {
        model_version_t *candidate = _ota_candidates_cache[index];
        if (candidate) {
            uint32_t *software_version_array = nullptr;
            size_t software_version_count;
            esp_err_t err = _query_software_version_array(candidate->vendor_id, candidate->product_id,
                                                          &software_version_array, software_version_count);
            if (err == ESP_OK && software_version_array && software_version_count > 0) {
                std::sort(&software_version_array[0], &software_version_array[software_version_count - 1],
                          std::greater<uint32_t>());
                for (size_t index = 0;
                     index < software_version_count && software_version_array[index] > candidate->software_version;
                     ++index) {
                    err = _query_ota_candidate(candidate, software_version_array[index], candidate->software_version);
                    if (err == ESP_OK) {
                        break;
                    }
                }
                esp_matter_mem_free(software_version_array);
            }
        }
    }
}

static void _ota_candidates_periodic_update_handler(void *arg)
{
    ota_candidate_fetch_action_t action;
    action.vendor_id = chip::kMaxVendorId;
    if (xQueueSend(_ota_candidate_task_queue, &action, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed send search ota candidate action");
    }
}
#endif

static void _ota_candidate_fetch_handler(ota_candidate_fetch_action_t &action)
{
    model_version_t *candidate = nullptr;
    assert(action.callback);
    int candate_index = _search_ota_candidate(action.vendor_id, action.product_id, action.software_version);
    if (candate_index >= 0 && candate_index < max_ota_candidate_count && _ota_candidates_cache[candate_index]) {
        candidate = _ota_candidates_cache[candate_index];
        action.callback(EspOtaProvider::OTAQueryStatus::kUpdateAvailable, candidate->ota_url, candidate->ota_file_size,
                        candidate->software_version, candidate->software_version_str, action.callback_args);
        return;
    } else {
        // Cannot find the candidate from cache, we need to query DCL for a new candidate;
        uint32_t *software_version_array = nullptr;
        size_t software_version_count;
        esp_err_t err = _query_software_version_array(action.vendor_id, action.product_id, &software_version_array,
                                                      software_version_count);
        if (err == ESP_OK && software_version_array && software_version_count > 0) {
            // Sort the software version array
            std::sort(&software_version_array[0], &software_version_array[software_version_count - 1],
                      std::greater<uint32_t>());
            candidate = (model_version_t *)esp_matter_mem_calloc(1, sizeof(model_version_t));
            candidate->vendor_id = action.vendor_id;
            candidate->product_id = action.product_id;
            for (size_t index = 0;
                 index < software_version_count && software_version_array[index] > action.software_version; ++index) {
                err = _query_ota_candidate(candidate, software_version_array[index], action.software_version);
                if (err == ESP_OK) {
                    size_t empty_index = _find_empty_ota_candidates();
                    _increase_ota_candidates_lifetime();
                    candidate->lifetime = 0;
                    // Add this candidate to cache
                    _ota_candidates_cache[empty_index] = candidate;
                    action.callback(EspOtaProvider::OTAQueryStatus::kUpdateAvailable, candidate->ota_url,
                                    candidate->ota_file_size, candidate->software_version,
                                    candidate->software_version_str, action.callback_args);
                    esp_matter_mem_free(software_version_array);
                    return;
                }
            }
            esp_matter_mem_free(candidate);
            esp_matter_mem_free(software_version_array);
        }
    }
    // Cannot fetch the candidate
    action.callback(EspOtaProvider::OTAQueryStatus::kNotAvailable, nullptr, 0, 0, nullptr, action.callback_args);
}

static void ota_candidate_task(void *ctx)
{
    ota_candidate_fetch_action_t action;
    while (true) {
        if (xQueueReceive(_ota_candidate_task_queue, &action, portMAX_DELAY) == pdTRUE) {
            if (action.vendor_id != chip::kMaxVendorId) {
                _ota_candidate_fetch_handler(action);
            }
#ifdef CONFIG_ESP_MATTER_OTA_CANDIDATES_UPDATE_PERIODICALLY
            else {
                // If receiving an action with Max VendorId, try to update all the candidates cache.
                _update_all_ota_candidates_cache();
            }
#endif
        }
    }
    vQueueDelete(_ota_candidate_task_queue);
    vTaskDelete(NULL);
}

esp_err_t fetch_ota_candidate(const uint16_t vendor_id, const uint16_t product_id, const uint32_t software_version,
                              fetch_ota_image_done_callback_t callback, void *ctx)
{
    if (!_ota_candidate_task_queue) {
        ESP_LOGE(TAG, "Failed to search ota candidate as the task queue is not initialized");
        return ESP_ERR_NOT_FOUND;
    }
    if (!callback) {
        return ESP_ERR_INVALID_ARG;
    }
    ota_candidate_fetch_action_t action;
    action.vendor_id = vendor_id;
    action.product_id = product_id;
    action.software_version = software_version;
    action.callback = callback;
    action.callback_args = ctx;
    if (xQueueSend(_ota_candidate_task_queue, &action, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed send search ota candidate action");
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

esp_err_t init_ota_candidates()
{
    memset(_ota_candidates_cache, 0, sizeof(_ota_candidates_cache));
    if (_ota_candidate_task_queue) {
        return ESP_ERR_INVALID_STATE;
    }
    _ota_candidate_task_queue = xQueueCreate(8, sizeof(ota_candidate_fetch_action_t));
    if (!_ota_candidate_task_queue) {
        ESP_LOGE(TAG, "Failed to create ota_candidate task queue");
        return ESP_ERR_NO_MEM;
    }

    static TaskHandle_t task_handle = NULL;
    if (task_handle) {
        return ESP_ERR_INVALID_STATE;
    }
    if (xTaskCreate(ota_candidate_task, "ota_candidate", 8192, NULL, 5, NULL) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to create ota_candidate task");
        return ESP_ERR_NO_MEM;
    }
#ifdef CONFIG_ESP_MATTER_OTA_CANDIDATES_UPDATE_PERIODICALLY
    if (!_ota_candidates_update_timer) {
        // start a timer which will update the candidates cache everyday.
        esp_timer_init();
        const esp_timer_create_args_t timer_args = {
            .callback = _ota_candidates_periodic_update_handler, .arg = nullptr, .name = "ota_candidates_update_timer"};
        esp_timer_create(&timer_args, &_ota_candidates_update_timer);
        esp_timer_start_periodic(_ota_candidates_update_timer,
                                 (uint64_t)CONFIG_ESP_MATTER_OTA_CANDIDATES_UPDATE_PERIOD * 3600 * 1000 * 1000);
    }
#endif
    return ESP_OK;
}

} // namespace ota_provider
} // namespace esp_matter
