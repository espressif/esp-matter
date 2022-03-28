// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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
#include <string.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_rainmaker.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_user_mapping.h>

static const char *TAG = "esp_matter_rainmaker";

static esp_err_t esp_matter_rainmaker_console_handler(int argc, char **argv)
{
    if (argc == 3 && strncmp(argv[0], "add-user", sizeof("add-user")) == 0) {
        printf("%s: Starting user-node mapping\n", TAG);
        if (esp_rmaker_start_user_node_mapping(argv[1], argv[2]) != ESP_OK) {
            return ESP_FAIL;
        }
    } else {
        printf("%s: Invalid Usage.\n", TAG);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static void esp_matter_rainmaker_register_commands()
{
    esp_matter_console_command_t command = {
        .name = "rainmaker",
        .description = "Initiate ESP RainMaker User-Node mapping from the node. "
                       "Usage: matter esp rainmaker add-user <user_id> <secret_key>",
        .handler = esp_matter_rainmaker_console_handler,
    };
    esp_matter_console_add_command(&command);
}

#define ESP_MATTER_RAINMAKER_ENDPOINT_ID 0x0    /* Same as root node endpoint. This will always be endpoint_id 0. */
#define ESP_MATTER_RAINMAKER_CLUSTER_ID 0x131B0000   /* 0x131B == manufacturer code */
#define ESP_MATTER_RAINMAKER_STATUS_ATTRIBUTE_ID 0x0
#define ESP_MATTER_RAINMAKER_CONFIGURATION_COMMAND_ID 0x0
#define ESP_MATTER_RAINMAKER_CLUSTER_REVISION 1
#define ESP_MATTER_RAINMAKER_COMMAND_LIMIT 5    /* This command can be called 5 times per reboot */
#define ESP_MATTER_RAINMAKER_MAX_DATA_LEN 40

static esp_err_t rainmaker_status_attribute_update(bool status)
{
    int endpoint_id = ESP_MATTER_RAINMAKER_ENDPOINT_ID;
    int cluster_id = ESP_MATTER_RAINMAKER_CLUSTER_ID;
    int attribute_id = ESP_MATTER_RAINMAKER_STATUS_ATTRIBUTE_ID;
    esp_matter_attr_val_t val = esp_matter_bool(status);
    return esp_matter_attribute_update(endpoint_id, cluster_id, attribute_id, &val);
}

static void user_node_association_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id,
                                                void* event_data)
{
    /* This event handler is only for user node association status */
    if (event_base == RMAKER_EVENT) {
        if (event_id == RMAKER_EVENT_USER_NODE_MAPPING_DONE) {
            ESP_LOGI(TAG, "User node association complete. Updating the status attribute.");
            rainmaker_status_attribute_update(true);
        } else if (event_id == RMAKER_EVENT_USER_NODE_MAPPING_RESET) {
            ESP_LOGI(TAG, "User node association reset. Updating the status attribute.");
            rainmaker_status_attribute_update(false);
        }
    }
}

esp_err_t esp_matter_rainmaker_command_callback(int endpoint_id, int cluster_id, int command_id, TLVReader &tlv_data,
                                         void *priv_data)
{
    /* Return if this is not the rainmaker configuration command */
    if (endpoint_id != ESP_MATTER_RAINMAKER_ENDPOINT_ID || cluster_id != ESP_MATTER_RAINMAKER_CLUSTER_ID
        || command_id != ESP_MATTER_RAINMAKER_CONFIGURATION_COMMAND_ID) {
        return ESP_OK;
    }
    ESP_LOGI(TAG, "RainMaker configuration command callback");
    static int command_count = ESP_MATTER_RAINMAKER_COMMAND_LIMIT;
    if (command_count <= 0) {
        ESP_LOGE(TAG, "This command has reached a limit. Please reboot to try again.");
        return ESP_FAIL;
    }
    command_count--;

    /* Parse the tlv data */
    chip::CharSpan config_value;
    chip::app::DataModel::Decode(tlv_data, config_value);
    const char *data = config_value.data();
    int size = config_value.size();
    if (!data || size <= 0) {
        ESP_LOGE(TAG, "Command data not found or was not decoded correctly. The expected data is a string or the"
                 "format is \"<user_id>::<secret_key>\"");
        return ESP_FAIL;
    }

    /* The expected format of the data is "<user_id>::<secret_key>" */
    char ch = ':';
    char *check = strchr(data, (int)ch);
    if (check == NULL) {
        ESP_LOGE(TAG, "':' not found in the received data: %.*s. The expected format is \"<user_id>::<secret_key>\"",
                 size, data);
        return ESP_FAIL;
    }

    /* Get sizes */
    int user_id_index = 0;
    int user_id_len = (int)(strchr(data, (int)ch) - data);              /* (first ':') - (start of string) */
    int secret_key_index = (int)(strrchr(data, (int)ch) - data) + 1;    /* (last ':') - (start of string) + 1 */
    int secret_key_len = size - secret_key_index;
    if (user_id_len <= 0 || user_id_len >= ESP_MATTER_RAINMAKER_MAX_DATA_LEN || secret_key_len <= 0
        || secret_key_len >= ESP_MATTER_RAINMAKER_MAX_DATA_LEN) {
        ESP_LOGE(TAG, "User id or secret key length invalid: user_id_len: %d, secret_key_len: %d", user_id_len,
                 secret_key_len);
        return ESP_FAIL;
    }

    /* Copy the data. This done to make the strings NULL terminated. */
    char user_id[ESP_MATTER_RAINMAKER_MAX_DATA_LEN] = {0};
    char secret_key[ESP_MATTER_RAINMAKER_MAX_DATA_LEN] = {0};
    strncpy(user_id, &data[user_id_index], user_id_len);
    strncpy(secret_key, &data[secret_key_index], secret_key_len);
    ESP_LOGI(TAG, "user_id: %s, secret_key: %s", user_id, secret_key);

    /* Call the rainmaker API */
    if (strlen(user_id) > 0 && strlen(secret_key) > 0) {
        esp_rmaker_start_user_node_mapping(user_id, secret_key);
    }
    return ESP_OK;
}

static esp_err_t rainmaker_custom_cluster_create()
{
    /* Get the endpoint */
    esp_matter_node_t *node = esp_matter_node_get();
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_get(node, ESP_MATTER_RAINMAKER_ENDPOINT_ID);

    /* Create custom rainmaker cluster */
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ESP_MATTER_RAINMAKER_CLUSTER_ID,
                                                             ESP_MATTER_CLUSTER_FLAG_SERVER);
    esp_matter_attribute_create(cluster, ZCL_CLUSTER_REVISION_SERVER_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_uint16(ESP_MATTER_RAINMAKER_CLUSTER_REVISION));

    /* Create custom status attribute */
    /* Update the value of the attribute after esp_rmaker_node_init() is done */
    esp_matter_attribute_create(cluster, ESP_MATTER_RAINMAKER_STATUS_ATTRIBUTE_ID, ESP_MATTER_ATTRIBUTE_FLAG_NONE,
                                esp_matter_bool(false));

    /* Create custom configuration command */
    esp_matter_command_create(cluster, ESP_MATTER_RAINMAKER_CONFIGURATION_COMMAND_ID,
                              ESP_MATTER_COMMAND_FLAG_CLIENT_GENERATED | ESP_MATTER_COMMAND_FLAG_CUSTOM, NULL);
    return ESP_OK;
}

esp_err_t esp_matter_rainmaker_init()
{
    /* Add custom rainmaker cluster */
    esp_matter_rainmaker_register_commands();
    return rainmaker_custom_cluster_create();
}

esp_err_t esp_matter_rainmaker_start()
{
    /* Check user node association */
    if (esp_rmaker_user_node_mapping_get_state() == ESP_RMAKER_USER_MAPPING_DONE) {
        rainmaker_status_attribute_update(true);
    }

    /* Register an event handler and update the state later */
    esp_event_handler_register(RMAKER_EVENT, RMAKER_EVENT_USER_NODE_MAPPING_DONE,
                                &user_node_association_event_handler, NULL);
    esp_event_handler_register(RMAKER_EVENT, RMAKER_EVENT_USER_NODE_MAPPING_RESET,
                                &user_node_association_event_handler, NULL);
    return ESP_OK;
}
