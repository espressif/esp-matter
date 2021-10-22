// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_log.h>
#include <esp_matter.h>
#include <string.h>

#define NAME_MAX_LEN 20

static const char *TAG = "esp_matter";

typedef struct esp_matter_attr_cb {
    char name[NAME_MAX_LEN];
    esp_matter_attribute_callback_t callback;
    void *priv_data;
    struct esp_matter_attr_cb *next;
} esp_matter_attr_cb_t;

typedef struct {
    esp_matter_attr_cb_t *callbacks;
} esp_matter_t;

static esp_matter_t *esp_matter = NULL;

esp_matter_attr_val_t esp_matter_bool(bool val)
{
    esp_matter_attr_val_t attr_val = {.type = ESP_MATTER_VAL_TYPE_BOOLEAN, .val.b = val};
    return attr_val;
}

esp_matter_attr_val_t esp_matter_int(int val)
{
    esp_matter_attr_val_t attr_val = {.type = ESP_MATTER_VAL_TYPE_INTEGER, .val.i = val};
    return attr_val;
}

esp_matter_attr_val_t esp_matter_float(float val)
{
    esp_matter_attr_val_t attr_val = {.type = ESP_MATTER_VAL_TYPE_FLOAT, .val.f = val};
    return attr_val;
}

esp_matter_attr_val_t esp_matter_str(const char *val)
{
    esp_matter_attr_val_t attr_val = {.type = ESP_MATTER_VAL_TYPE_STRING, .val.s = (char *)val};
    return attr_val;
}

esp_matter_attr_val_t esp_matter_obj(const char *val)
{
    esp_matter_attr_val_t attr_val = {.type = ESP_MATTER_VAL_TYPE_OBJECT, .val.s = (char *)val};
    return attr_val;
}

esp_matter_attr_val_t esp_matter_array(const char *val)
{
    esp_matter_attr_val_t attr_val = {.type = ESP_MATTER_VAL_TYPE_ARRAY, .val.s = (char *)val};
    return attr_val;
}

esp_err_t esp_matter_attribute_notify(const char *name, const char *endpoint, const char *attribute,
                                      esp_matter_attr_val_t val)
{
    if (!esp_matter) {
        ESP_LOGE(TAG, "Init not done");
        return ESP_ERR_INVALID_STATE;
    }
    if (!name || !endpoint || !attribute) {
        ESP_LOGE(TAG, "Name or endpoint or attribute cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    /* Note: Even if a callback has not been added previously using esp_matter_attribute_callback_add(), it can still
     * call this API to notify others. If this behaviour is to be changed, a check for name can be added here. */

    /* Print */
    if (val.type == ESP_MATTER_VAL_TYPE_BOOLEAN) {
        ESP_LOGI(TAG, "********** %s changed %s's %s to %d **********", name, endpoint, attribute, val.val.b);
    } else if (val.type == ESP_MATTER_VAL_TYPE_INTEGER) {
        ESP_LOGI(TAG, "********** %s changed %s's %s to %d **********", name, endpoint, attribute, val.val.i);
    } else if (val.type == ESP_MATTER_VAL_TYPE_FLOAT) {
        ESP_LOGI(TAG, "********** %s changed %s's %s to %f **********", name, endpoint, attribute, val.val.f);
    } else if (val.type == ESP_MATTER_VAL_TYPE_STRING || val.type == ESP_MATTER_VAL_TYPE_OBJECT ||
               val.type == ESP_MATTER_VAL_TYPE_ARRAY) {
        ESP_LOGI(TAG, "********** %s changed %s's %s to %s **********", name, endpoint, attribute, val.val.s);
    } else {
        ESP_LOGI(TAG, "********** %s changed %s's %s to <invalid value> **********", name, endpoint, attribute);
        ESP_LOGW(TAG, "Not notifying other callbacks.");
        return ESP_ERR_INVALID_ARG;
    }

    /* Callback */
    esp_matter_attr_cb_t *current_callback = esp_matter->callbacks;
    while (current_callback) {
        if (strncmp(current_callback->name, name, NAME_MAX_LEN) != 0) {
            if (current_callback->callback) {
                current_callback->callback(endpoint, attribute, val, current_callback->priv_data);
            }
        }
        current_callback = current_callback->next;
    }
    return ESP_OK;
}

esp_err_t esp_matter_attribute_callback_add(const char *name, esp_matter_attribute_callback_t callback, void *priv_data)
{
    if (!esp_matter) {
        ESP_LOGE(TAG, "Init not done");
        return ESP_ERR_INVALID_STATE;
    }
    if (!name) {
        ESP_LOGE(TAG, "Name cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    /* Allocate and copy */
    esp_matter_attr_cb_t *new_callback = (esp_matter_attr_cb_t *)calloc(1, sizeof(esp_matter_attr_cb_t));
    if (new_callback == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for esp_matter_attr_cb_t");
        return ESP_ERR_NO_MEM;
    }
    strncpy(new_callback->name, name, strnlen(name, NAME_MAX_LEN));
    new_callback->callback = callback;
    new_callback->priv_data = priv_data;

    /* Add to list */
    if (esp_matter->callbacks == NULL) {
        esp_matter->callbacks = new_callback;
        return ESP_OK;
    }
    esp_matter_attr_cb_t *previous_callback = esp_matter->callbacks;
    while (previous_callback->next) {
        previous_callback = previous_callback->next;
    }
    previous_callback->next = new_callback;
    return ESP_OK;
}

esp_err_t esp_matter_attribute_callback_remove(const char *name)
{
    if (!esp_matter) {
        ESP_LOGE(TAG, "Init not done");
        return ESP_ERR_INVALID_STATE;
    }
    if (!name) {
        ESP_LOGE(TAG, "Name cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    /* Find callback */
    esp_matter_attr_cb_t *previous_callback = NULL;
    esp_matter_attr_cb_t *current_callback = esp_matter->callbacks;
    while (current_callback) {
        if (strncmp(current_callback->name, name, NAME_MAX_LEN) == 0) {
            break;
        }
        previous_callback = current_callback;
        current_callback = current_callback->next;
    }
    if (current_callback == NULL) {
        ESP_LOGE(TAG, "Could not find callback: %s", name);
        return ESP_ERR_NOT_FOUND;
    }

    /* Remove from list */
    if (previous_callback == NULL) {
        esp_matter->callbacks = current_callback->next;
    } else {
        previous_callback->next = current_callback->next;
    }

    /* Free up */
    free(current_callback);
    return ESP_OK;
}

esp_err_t esp_matter_init()
{
    if (esp_matter) {
        ESP_LOGI(TAG, "Already initialized");
        return ESP_OK;
    }
    esp_matter = (esp_matter_t *)calloc(1, sizeof(esp_matter_t));
    if (!esp_matter) {
        ESP_LOGE(TAG, "Failed to allocate memory for esp_matter_t");
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}
