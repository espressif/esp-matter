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
#include <esp_matter_controller_utils.h>
#include <matter_controller_cluster.h>
#include <matter_controller_device_mgr.h>
#include <rmaker_rest_apis.h>

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
static TaskHandle_t s_device_mgr_task = NULL;
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
            free_matter_device_list(ret);
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

static esp_err_t update_device_list_task(void *endpoint_id_ptr)
{
    esp_err_t err = ESP_OK;
    ScopedMemoryBufferWithSize<char> endpoint_url;
    ScopedMemoryBufferWithSize<char> rainmaker_group_id;
    uint16_t endpoint_id = *(uint16_t *)endpoint_id_ptr;
    free(endpoint_id_ptr);

    endpoint_url.Calloc(ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN);
    ESP_RETURN_ON_FALSE(endpoint_url.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for endpoint_url");
    rainmaker_group_id.Calloc(ESP_MATTER_RAINMAKER_MAX_GROUP_ID_LEN);
    ESP_RETURN_ON_FALSE(rainmaker_group_id.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for rainmaker_group_id");

    ESP_RETURN_ON_ERROR(endpoint_url::get(endpoint_id, endpoint_url.Get()), TAG, "Failed to get the endpoint_url");
    ESP_RETURN_ON_ERROR(rainmaker_group_id::get(endpoint_id, rainmaker_group_id.Get()), TAG,
                        "Failed to get the rainmaker_group_id");
    const char *access_token = controller::get_current_access_token();

    matter_device_t *dev_list = NULL;
    err = fetch_matter_device_list(endpoint_url.Get(), access_token, rainmaker_group_id.Get(), &dev_list);
    if (err == ESP_ERR_TIMEOUT) {
        authorized::update(endpoint_id, false);
    }
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to get the matter device list");
    print_matter_device_list(dev_list);
    {
        scoped_device_mgr_lock lock;
        s_matter_device_list = dev_list;
    }
    if (s_device_list_update_cb) {
        s_device_list_update_cb();
    }
    return ESP_OK;
}

esp_err_t update_device_list(uint16_t endpoint_id)
{
    if (!s_task_queue) {
        ESP_LOGE(TAG, "Failed to update device list as the task queue is not initialized");
        return ESP_ERR_INVALID_STATE;
    }
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
    if (s_device_mgr_task) {
        return ESP_OK;
    }
    uint8_t fabric_index;
    bool user_noc_installed = false;
    s_device_list_update_cb = dev_list_update_cb;
    if (xTaskCreate(device_mgr_task, "device_mgr", 4096, NULL, 5, &s_device_mgr_task) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to create device mgr task");
        return ESP_ERR_NO_MEM;
    }

    ESP_RETURN_ON_ERROR(user_noc_installed::get(endpoint_id, user_noc_installed), TAG,
                        "Failed to get the user_noc_installed");
    if (user_noc_installed) {
        // get the user_noc_fabric_index and pass it to the controller.
        cluster::matter_controller::attribute::user_noc_fabric_index::get(endpoint_id, fabric_index);
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
