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

#include <esp_check.h>
#include <esp_crt_bundle.h>
#include <esp_http_client.h>
#include <esp_matter_controller_utils.h>
#include <json_generator.h>
#include <json_parser.h>
#include <matter_controller_cluster.h>
#include <matter_controller_device_mgr.h>

#include <lib/support/ScopedBuffer.h>

using chip::Platform::ScopedMemoryBufferWithSize;
using namespace esp_matter::cluster::matter_controller::attribute;

#define TAG "controller_dev_mgr"

namespace esp_matter {
namespace controller {
namespace device_mgr {

static matter_device_t *s_matter_device_list = NULL;
static device_list_update_callback_t s_device_list_update_cb = NULL;
static QueueHandle_t s_task_queue = NULL;
static SemaphoreHandle_t s_device_mgr_mutex = NULL;
typedef esp_err_t (*esp_matter_device_mgr_task_t)(void *);

typedef struct {
    esp_matter_device_mgr_task_t task;
    void *arg;
} task_post_t;

class scoped_device_mgr_lock {
public:
    scoped_device_mgr_lock()
    {
        if (s_device_mgr_mutex) {
            xSemaphoreTake(s_device_mgr_mutex, portMAX_DELAY);
        } else {
            ESP_LOGE(TAG, "device mgr lock not initialized");
        }
    }
    ~scoped_device_mgr_lock()
    {
        if (s_device_mgr_mutex) {
            xSemaphoreGive(s_device_mgr_mutex);
        } else {
            ESP_LOGE(TAG, "device mgr lock not initialized");
        }
    }
};

void free_device_list(matter_device_t *dev_list)
{
    matter_device_t *current = dev_list;
    while (current) {
        dev_list = dev_list->next;
        free(current);
        current = dev_list;
    }
}

void print_device_list(matter_device_t *dev_list)
{
    matter_device_t *dev = dev_list;
    uint8_t dev_index = 0;
    while (dev) {
        ESP_LOGI(TAG, "device %d : {", dev_index);
        ESP_LOGI(TAG, "    rainmaker_node_id: %s,", dev->rainmaker_node_id);
        ESP_LOGI(TAG, "    matter_node_id: 0x%llX,", dev->node_id);
        ESP_LOGI(TAG, "    is_rainmaker_device: %s,", dev->is_rainmaker_device ? "true" : "false");
        ESP_LOGI(TAG, "    is_online: %s,", dev->reachable ? "true" : "false");
        ESP_LOGI(TAG, "    endpoints : [");
        for (size_t i = 0; i < dev->endpoint_count; ++i) {
            ESP_LOGI(TAG, "        {");
            ESP_LOGI(TAG, "           endpoint_id: %d,", dev->endpoints[i].endpoint_id);
            ESP_LOGI(TAG, "           device_type_id: 0x%lx,", dev->endpoints[i].device_type_id);
            ESP_LOGI(TAG, "        },");
        }
        ESP_LOGI(TAG, "    ]");
        ESP_LOGI(TAG, "}");
        dev = dev->next;
        dev_index++;
    }
}

static matter_device_t *clone_device(matter_device_t *dev)
{
    matter_device_t *ret = (matter_device_t *)malloc(sizeof(matter_device_t));
    if (!ret) {
        ESP_LOGE(TAG, "Failed to allocate memory for matter device struct");
        return NULL;
    }
    memcpy(ret, dev, sizeof(matter_device_t));
    ret->next = NULL;
    return ret;
}

matter_device_t *get_device_list_clone()
{
    matter_device_t *ret = NULL;
    scoped_device_mgr_lock dev_mgr_lock;
    matter_device_t *current = s_matter_device_list;
    while (current) {
        matter_device_t *tmp = clone_device(current);
        if (!tmp) {
            free_device_list(ret);
            return NULL;
        }
        tmp->next = ret;
        ret = tmp;
        current = current->next;
    }
    return ret;
}

static matter_device_t *get_device(uint64_t node_id)
{
    matter_device_t *ret = s_matter_device_list;
    while (ret) {
        if (ret->node_id == node_id) {
            break;
        }
        ret = ret->next;
    }
    return ret;
}

static matter_device_t *get_device(char *rainmaker_node_id)
{
    if (!rainmaker_node_id) {
        return NULL;
    }
    matter_device_t *ret = s_matter_device_list;
    while (ret) {
        if (strncmp(ret->rainmaker_node_id, rainmaker_node_id,
                    strnlen(ret->rainmaker_node_id, sizeof(ret->rainmaker_node_id))) == 0) {
            break;
        }
        ret = ret->next;
    }
    return ret;
}

matter_device_t *get_device_clone(uint64_t node_id)
{
    return clone_device(get_device(node_id));
}

matter_device_t *get_device_clone(char *rainmaker_node_id)
{
    return clone_device(get_device(rainmaker_node_id));
}

static esp_err_t get_node_reachable(jparse_ctx_t *jctx, bool *value)
{
    bool value_got = false;
    if (json_obj_get_object(jctx, "status") == 0) {
        if (json_obj_get_object(jctx, "connectivity") == 0) {
            if (json_obj_get_bool(jctx, "connected", value) == 0) {
                value_got = true;
            }
            json_obj_leave_object(jctx);
        }
        json_obj_leave_object(jctx);
    }
    return value_got ? ESP_OK : ESP_FAIL;
}

static esp_err_t get_metadata(jparse_ctx_t *jctx, matter_device_t *dev)
{
    if (json_obj_get_object(jctx, "metadata") == 0) {
        int device_type = 0;
        if (json_obj_get_int(jctx, "deviceType", &device_type) == 0) {
            dev->endpoints[0].device_type_id = device_type;
            int ep_count = 0;
            int ep_id = 1;
            if (json_obj_get_array(jctx, "endpointsData", &ep_count) == 0) {
                json_arr_get_int(jctx, 1, &ep_id);
                dev->endpoints[0].endpoint_id = ep_id;
                json_obj_leave_array(jctx);
            }
            dev->endpoint_count = 1;
        }
        json_obj_get_bool(jctx, "isRainmaker", &(dev->is_rainmaker_device));
        json_obj_leave_object(jctx);
    }
    return ESP_OK;
}

static esp_err_t fetch_node_metadata(ScopedMemoryBufferWithSize<char> &endpoint_url,
                                     ScopedMemoryBufferWithSize<char> &access_token, uint16_t endpoint_id)
{
    esp_err_t ret = ESP_OK;
    matter_device_t *dev = s_matter_device_list;
    char url[200];
    int http_len, http_status_code;
    esp_http_client_config_t config = {
        .url = url,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .buffer_size = 4096,
        .buffer_size_tx = 2048,
        .skip_cert_common_name_check = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = NULL;
    jparse_ctx_t jctx;
    int node_count = 0;
    int node_index = 0;
    char id_str[24] = {0};
    int id_str_len = 0;
    ScopedMemoryBufferWithSize<char> http_payload;
    http_payload.Calloc(4096);
    ESP_RETURN_ON_FALSE(http_payload.Get(), ESP_ERR_NO_MEM, TAG, "Failed to allocate memory for http_payload");

    scoped_device_mgr_lock dev_mgr_lock;
    while (dev) {
        snprintf(url, sizeof(url), "%s/%s/%s?node_id=%s&%s", endpoint_url.Get(), HTTP_API_VERSION, "user/nodes",
                 dev->rainmaker_node_id, "node_details=true&is_matter=true");
        client = esp_http_client_init(&config);
        ESP_RETURN_ON_FALSE(client, ESP_FAIL, TAG, "Failed to initialise HTTP Client.");

        ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "accept", "application/json"), cleanup, TAG,
                          "Failed to set http header accept");
        ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "Authorization", access_token.Get()), cleanup, TAG,
                          "Failed to set http header Authorization");
        ESP_GOTO_ON_ERROR(esp_http_client_set_method(client, HTTP_METHOD_GET), cleanup, TAG,
                          "Failed to set http method");

        // HTTP GET Method
        ESP_GOTO_ON_ERROR(esp_http_client_open(client, 0), cleanup, TAG, "Failed to open http connection");

        http_len = esp_http_client_fetch_headers(client);
        http_status_code = esp_http_client_get_status_code(client);

        // Read Response
        if ((http_len > 0) && (http_status_code == 200)) {
            http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize());
            http_payload[http_len] = '\0';
        } else {
            http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize());
            http_payload[http_len] = '\0';
            ESP_LOGE(TAG, "Invalid response for %s", url);
            ESP_LOGE(TAG, "Status = %d, Data = %s", http_status_code, http_len > 0 ? http_payload.Get() : "None");
            if (http_status_code == 401) {
                cluster::matter_controller::attribute::authorized_attribute_update(endpoint_id, false);
            }
            ret = ESP_FAIL;
            goto close;
        }
        ESP_LOGD(TAG, "http response payload: %s", http_payload.Get());

        // Parse the http response
        ESP_GOTO_ON_FALSE(json_parse_start(&jctx, http_payload.Get(), strlen(http_payload.Get())) == 0, ESP_FAIL, close,
                          TAG, "Failed to parse the http response json on json_parse_start");
        if (json_obj_get_array(&jctx, "node_details", &node_count) == 0) {
            for (node_index = 0; node_index < node_count; ++node_index) {
                if (json_arr_get_object(&jctx, node_index) == 0) {
                    if (json_obj_get_strlen(&jctx, "id", &id_str_len) == 0 &&
                        json_obj_get_string(&jctx, "id", id_str, id_str_len + 1) == 0) {
                        id_str[id_str_len] = '\0';
                        dev = get_device(id_str);
                        if (dev) {
                            get_node_reachable(&jctx, &(dev->reachable));
                            get_metadata(&jctx, dev);
                        }
                    }
                    json_arr_leave_object(&jctx);
                }
            }
            json_obj_leave_array(&jctx);
        }
        json_parse_end(&jctx);
        dev = dev->next;
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        client = NULL;
    }
    print_device_list(s_matter_device_list);

close:
    if (client) {
        esp_http_client_close(client);
    }
cleanup:
    if (client) {
        esp_http_client_cleanup(client);
    }
    return ret;
}

#define CONTROLLER_NODE_TYPE "Controller"

static esp_err_t fetch_node_list(ScopedMemoryBufferWithSize<char> &endpoint_url,
                                 ScopedMemoryBufferWithSize<char> &access_token,
                                 ScopedMemoryBufferWithSize<char> &rainmaker_group_id, uint16_t endpoint_id)
{
    esp_err_t ret = ESP_OK;
    char url[200];
    int http_len, http_status_code;
    ScopedMemoryBufferWithSize<char> http_payload;
    jparse_ctx_t jctx;
    int array_count = 0;
    matter_device_t *new_device_list = NULL;
    size_t node_index = 0;
    char node_type_str[32] = {0};
    int str_len = 0;

    snprintf(url, sizeof(url), "%s/%s/%s=%s&%s", endpoint_url.Get(), HTTP_API_VERSION, "user/node_group?group_id",
             rainmaker_group_id.Get(), "node_details=true&sub_groups=false&node_list=true&is_matter=true");
    esp_http_client_config_t config = {
        .url = url,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .buffer_size = 2048,
        .buffer_size_tx = 2048,
        .skip_cert_common_name_check = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    ESP_RETURN_ON_FALSE(client, ESP_FAIL, TAG, "Failed to initialise HTTP Client.");

    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "accept", "application/json"), cleanup, TAG,
                      "Failed to set http header accept");
    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "Authorization", access_token.Get()), cleanup, TAG,
                      "Failed to set http header Authorization");
    ESP_GOTO_ON_ERROR(esp_http_client_set_method(client, HTTP_METHOD_GET), cleanup, TAG, "Failed to set http method");

    // HTTP GET Method
    ESP_GOTO_ON_ERROR(esp_http_client_open(client, 0), cleanup, TAG, "Failed to open http connection");

    http_len = esp_http_client_fetch_headers(client);
    http_status_code = esp_http_client_get_status_code(client);
    http_payload.Calloc(2048);
    ESP_GOTO_ON_FALSE(http_payload.Get(), ESP_ERR_NO_MEM, close, TAG, "Failed to allocate memory for http_payload");

    // Read Response
    if ((http_len > 0) && (http_status_code == 200)) {
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize());
        http_payload[http_len] = '\0';
    } else {
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize());
        http_payload[http_len] = '\0';
        ESP_LOGE(TAG, "Invalid response for %s", url);
        ESP_LOGE(TAG, "Status = %d, Data = %s", http_status_code, http_len > 0 ? http_payload.Get() : "None");
        if (http_status_code == 401) {
            cluster::matter_controller::attribute::authorized_attribute_update(endpoint_id, false);
        }
        ret = ESP_FAIL;
        goto close;
    }
    ESP_LOGD(TAG, "http response payload: %s\n", http_payload.Get());

    // Parse http respnse
    ESP_GOTO_ON_FALSE(json_parse_start(&jctx, http_payload.Get(), strlen(http_payload.Get())) == 0, ESP_FAIL, close,
                      TAG, "Failed to parse the http response json on json_parse_start");
    if (json_obj_get_array(&jctx, "groups", &array_count) == 0 && array_count == 1) {
        if (json_arr_get_object(&jctx, 0) == 0) {
            if (json_obj_get_array(&jctx, "node_details", &array_count) == 0 && array_count > 0) {
                for (node_index = 0; node_index < array_count; ++node_index) {
                    if (json_arr_get_object(&jctx, node_index) == 0) {
                        if (json_obj_get_strlen(&jctx, "type", &str_len) == 0 &&
                            json_obj_get_string(&jctx, "type", node_type_str, str_len + 1) == 0) {
                            if (strncmp(node_type_str, CONTROLLER_NODE_TYPE, strlen(CONTROLLER_NODE_TYPE)) == 0) {
                                // Skip the controller node
                                json_arr_leave_object(&jctx);
                                continue;
                            }
                        }
                        char matter_node_id_str[17];
                        if (json_obj_get_strlen(&jctx, "matter_node_id", &str_len) == 0 &&
                            json_obj_get_string(&jctx, "matter_node_id", matter_node_id_str, str_len + 1) == 0) {
                            matter_node_id_str[str_len] = '\0';
                            matter_device_t *device_entry = (matter_device_t *)calloc(1, sizeof(matter_device_t));
                            if (!device_entry) {
                                ESP_LOGE(TAG, "Failed to alloc memory for device entry");
                                free_device_list(new_device_list);
                                ret = ESP_ERR_NO_MEM;
                                goto close;
                            }
                            device_entry->next = new_device_list;
                            new_device_list = device_entry;
                            device_entry->node_id = strtoull(matter_node_id_str, NULL, 16);
                            if (json_obj_get_strlen(&jctx, "node_id", &str_len) == 0) {
                                json_obj_get_string(&jctx, "node_id", device_entry->rainmaker_node_id, str_len + 1);
                            }
                        }
                        json_arr_leave_object(&jctx);
                    }
                }
                json_obj_leave_array(&jctx);
            }
            json_arr_leave_object(&jctx);
        }
        json_obj_leave_array(&jctx);
    }
    json_parse_end(&jctx);
    {
        scoped_device_mgr_lock dev_mgr_lock;
        free_device_list(s_matter_device_list);
        s_matter_device_list = new_device_list;
    }

close:
    esp_http_client_close(client);
cleanup:
    esp_http_client_cleanup(client);
    return ret;
}

static esp_err_t update_device_list_task(void *endpoint_id_ptr)
{
    ScopedMemoryBufferWithSize<char> endpoint_url;
    ScopedMemoryBufferWithSize<char> rainmaker_group_id;
    ScopedMemoryBufferWithSize<char> access_token;
    uint16_t endpoint_id = *(uint16_t *)endpoint_id_ptr;
    free(endpoint_id_ptr);

    endpoint_url.Calloc(ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN);
    ESP_RETURN_ON_FALSE(endpoint_url.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for endpoint_url");
    rainmaker_group_id.Calloc(ESP_MATTER_RAINMAKER_MAX_GROUP_ID_LEN);
    ESP_RETURN_ON_FALSE(rainmaker_group_id.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for rainmaker_group_id");
    access_token.Calloc(ESP_MATTER_RAINMAKER_MAX_ACCESS_TOKEN_LEN);
    ESP_RETURN_ON_FALSE(access_token.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for access_token");

    ESP_RETURN_ON_ERROR(endpoint_url_attribute_get(endpoint_id, endpoint_url.Get()), TAG,
                        "Failed to get the endpoint_url");
    ESP_RETURN_ON_ERROR(rainmaker_group_id_attribute_get(endpoint_id, rainmaker_group_id.Get()), TAG,
                        "Failed to get the rainmaker_group_id");
    ESP_RETURN_ON_ERROR(access_token_attribute_get(endpoint_id, access_token.Get()), TAG,
                        "Failed to get the access_token");

    ESP_RETURN_ON_ERROR(fetch_node_list(endpoint_url, access_token, rainmaker_group_id, endpoint_id), TAG,
                        "Failed to fetch the node list");
    ESP_RETURN_ON_ERROR(fetch_node_metadata(endpoint_url, access_token, endpoint_id), TAG,
                        "Failed to fetch the node metadata");
    if (s_device_list_update_cb) {
        s_device_list_update_cb();
    }
    return ESP_OK;
}

esp_err_t update_device_list(uint16_t endpoint_id)
{
    uint16_t *endpoint_id_ptr = (uint16_t *)malloc(sizeof(uint16_t));
    *endpoint_id_ptr = endpoint_id;
    task_post_t task_post = {
        .task = update_device_list_task,
        .arg = endpoint_id_ptr,
    };
    if (xQueueSend(s_task_queue, &task_post, portMAX_DELAY) != pdTRUE) {
        free(endpoint_id_ptr);
        ESP_LOGE(TAG, "Failed send update device list task");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void device_mgr_task(void *aContext)
{
    s_task_queue = xQueueCreate(8 /* Queue Size */, sizeof(task_post_t));
    if (!s_task_queue) {
        ESP_LOGE(TAG, "Failed to create device mgr task queue");
        return;
    }
    s_device_mgr_mutex = xSemaphoreCreateRecursiveMutex();
    if (!s_device_mgr_mutex) {
        ESP_LOGE(TAG, "Failed to create device mgr lock");
        vQueueDelete(s_task_queue);
        return;
    }
    task_post_t task_post;
    while (true) {
        if (xQueueReceive(s_task_queue, &task_post, portMAX_DELAY) == pdTRUE) {
            task_post.task(task_post.arg);
        }
    }
    vQueueDelete(s_task_queue);
    vSemaphoreDelete(s_device_mgr_mutex);
    vTaskDelete(NULL);
}

esp_err_t init(uint16_t endpoint_id, device_list_update_callback_t dev_list_update_cb)
{
    uint8_t fabric_index;
    bool user_noc_installed = false;
    s_device_list_update_cb = dev_list_update_cb;
    if (xTaskCreate(device_mgr_task, "device_mgr", 4096, NULL, 5, NULL) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to create device mgr task");
        return ESP_ERR_NO_MEM;
    }

    ESP_RETURN_ON_ERROR(user_noc_installed_attribute_get(endpoint_id, user_noc_installed), TAG,
                        "Failed to get the user_noc_installed");
    if (user_noc_installed) {
        // get the user_noc_fabric_index and pass it to the controller.
        cluster::matter_controller::attribute::user_noc_fabric_index_attribute_get(endpoint_id, fabric_index);
        esp_matter::controller::set_fabric_index(fabric_index);
        // Do authorizing before update the device list
        ESP_RETURN_ON_ERROR(controller_authorize(endpoint_id), TAG, "Failed to do authorizing");
        ESP_RETURN_ON_ERROR(controller::device_mgr::update_device_list(endpoint_id), TAG,
                            "Failed to update device list");
    }
    return ESP_OK;
}
} // namespace device_mgr
} // namespace controller
} // namespace esp_matter
