/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <string.h>

#include <esp_rmaker_core.h>
#include <esp_rmaker_ota.h>
#include <esp_rmaker_schedule.h>
#include <esp_rmaker_standard_devices.h>
#include <esp_rmaker_standard_params.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_user_mapping.h>

#include <app_rainmaker.h>
#include <esp_matter.h>
#include <esp_matter_console.h>

static const char *TAG = "app_rainmaker";
extern int light_endpoint_id;

#define DEFAULT_LIGHT_NAME "Light"

static esp_err_t app_rainmaker_console_handler(int argc, char **argv)
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

static void app_rainmaker_register_commands()
{
    esp_matter_console_command_t command = {
        .name = "rainmaker",
        .description = "Initiate ESP RainMaker User-Node mapping from the node. "
                       "Usage: matter esp rainmaker add-user <user_id> <secret_key>",
        .handler = app_rainmaker_console_handler,
    };
    esp_matter_console_add_command(&command);
}

static const char *app_rainmaker_get_device_name_from_id(int endpoint_id)
{
    if (endpoint_id == light_endpoint_id) {
        return DEFAULT_LIGHT_NAME;
    }
    return NULL;
}

static const char *app_rainmaker_get_device_type_from_id(int device_type_id)
{
    if (device_type_id == ESP_MATTER_COLOR_DIMMABLE_LIGHT_DEVICE_TYPE_ID) {
        return ESP_RMAKER_DEVICE_LIGHTBULB;
    }
    return NULL;
}

static int app_rainmaker_get_endpoint_id_from_name(const char *device_name)
{
    if (strcmp(device_name, DEFAULT_LIGHT_NAME) == 0) {
        return light_endpoint_id;
    }
    return 0;
}

static const char *app_rainmaker_get_param_name_from_id(int cluster_id, int attribute_id)
{
    if (cluster_id == ZCL_ON_OFF_CLUSTER_ID) {
        if (attribute_id == ZCL_ON_OFF_ATTRIBUTE_ID) {
            return ESP_RMAKER_DEF_POWER_NAME;
        }
    } else if (cluster_id == ZCL_LEVEL_CONTROL_CLUSTER_ID) {
        if (attribute_id == ZCL_CURRENT_LEVEL_ATTRIBUTE_ID) {
            return ESP_RMAKER_DEF_BRIGHTNESS_NAME;
        }
    } else if (cluster_id == ZCL_COLOR_CONTROL_CLUSTER_ID) {
        if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID) {
            return ESP_RMAKER_DEF_HUE_NAME;
        } else if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID) {
            return ESP_RMAKER_DEF_SATURATION_NAME;
        }
    }
    return NULL;
}

static const char *app_rainmaker_get_param_type_from_id(int cluster_id, int attribute_id)
{
    if (cluster_id == ZCL_ON_OFF_CLUSTER_ID) {
        if (attribute_id == ZCL_ON_OFF_ATTRIBUTE_ID) {
            return ESP_RMAKER_PARAM_POWER;
        }
    } else if (cluster_id == ZCL_LEVEL_CONTROL_CLUSTER_ID) {
        if (attribute_id == ZCL_CURRENT_LEVEL_ATTRIBUTE_ID) {
            return ESP_RMAKER_PARAM_BRIGHTNESS;
        }
    } else if (cluster_id == ZCL_COLOR_CONTROL_CLUSTER_ID) {
        if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID) {
            return ESP_RMAKER_PARAM_HUE;
        } else if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID) {
            return ESP_RMAKER_PARAM_SATURATION;
        }
    }
    return NULL;
}

static const char *app_rainmaker_get_param_ui_type_from_id(int cluster_id, int attribute_id)
{
    if (cluster_id == ZCL_ON_OFF_CLUSTER_ID) {
        if (attribute_id == ZCL_ON_OFF_ATTRIBUTE_ID) {
            return ESP_RMAKER_UI_TOGGLE;
        }
    } else if (cluster_id == ZCL_LEVEL_CONTROL_CLUSTER_ID) {
        if (attribute_id == ZCL_CURRENT_LEVEL_ATTRIBUTE_ID) {
            return ESP_RMAKER_UI_SLIDER;
        }
    } else if (cluster_id == ZCL_COLOR_CONTROL_CLUSTER_ID) {
        if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID) {
            return ESP_RMAKER_UI_SLIDER;
        } else if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID) {
            return ESP_RMAKER_UI_SLIDER;
        }
    }
    return NULL;
}

static bool app_rainmaker_get_param_bounds_from_id(int cluster_id, int attribute_id, int *min, int *max, int *step)
{
    if (cluster_id == ZCL_ON_OFF_CLUSTER_ID) {
        if (attribute_id == ZCL_ON_OFF_ATTRIBUTE_ID) {
            return false;
        }
    } else if (cluster_id == ZCL_LEVEL_CONTROL_CLUSTER_ID) {
        if (attribute_id == ZCL_CURRENT_LEVEL_ATTRIBUTE_ID) {
            *min = 0;
            *max = 255;
            *step = 1;
            return true;
        }
    } else if (cluster_id == ZCL_COLOR_CONTROL_CLUSTER_ID) {
        if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID) {
            *min = 0;
            *max = 255;
            *step = 1;
            return true;
        } else if (attribute_id == ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID) {
            *min = 0;
            *max = 255;
            *step = 1;
            return true;
        }
    }
    return false;
}

static int app_rainmaker_get_cluster_id_from_name(const char *param_name)
{
    if (strcmp(param_name, ESP_RMAKER_DEF_POWER_NAME) == 0) {
        return ZCL_ON_OFF_CLUSTER_ID;
    } else if (strcmp(param_name, ESP_RMAKER_DEF_BRIGHTNESS_NAME) == 0) {
        return ZCL_LEVEL_CONTROL_CLUSTER_ID;
    } else if (strcmp(param_name, ESP_RMAKER_DEF_HUE_NAME) == 0) {
        return ZCL_COLOR_CONTROL_CLUSTER_ID;
    } else if (strcmp(param_name, ESP_RMAKER_DEF_SATURATION_NAME) == 0) {
        return ZCL_COLOR_CONTROL_CLUSTER_ID;
    }
    return 0;
}

static int app_rainmaker_get_attribute_id_from_name(const char *param_name)
{
    if (strcmp(param_name, ESP_RMAKER_DEF_POWER_NAME) == 0) {
        return ZCL_ON_OFF_ATTRIBUTE_ID;
    } else if (strcmp(param_name, ESP_RMAKER_DEF_BRIGHTNESS_NAME) == 0) {
        return ZCL_CURRENT_LEVEL_ATTRIBUTE_ID;
    } else if (strcmp(param_name, ESP_RMAKER_DEF_HUE_NAME) == 0) {
        return ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID;
    } else if (strcmp(param_name, ESP_RMAKER_DEF_SATURATION_NAME) == 0) {
        return ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID;
    }
    return 0;
}

static esp_rmaker_param_val_t app_rainmaker_get_rmaker_val(esp_matter_attr_val_t *val)
{
    if (val->type == ESP_MATTER_VAL_TYPE_BOOLEAN) {
        return esp_rmaker_bool(val->val.b);
    } else if (val->type == ESP_MATTER_VAL_TYPE_INTEGER) {
        return esp_rmaker_int(val->val.i);
    } else if (val->type == ESP_MATTER_VAL_TYPE_FLOAT) {
        return esp_rmaker_float(val->val.f);
    } else if (val->type == ESP_MATTER_VAL_TYPE_UINT8) {
        return esp_rmaker_int(val->val.u8);
    } else if (val->type == ESP_MATTER_VAL_TYPE_INT16) {
        return esp_rmaker_int(val->val.i16);
    } else if (val->type == ESP_MATTER_VAL_TYPE_UINT16) {
        return esp_rmaker_int(val->val.u16);
    } else {
        ESP_LOGE(TAG, "Invalid val type: %d", val->type);
    }
    return esp_rmaker_int(0);
}

static esp_matter_attr_val_t app_rainmaker_get_matter_val(esp_rmaker_param_val_t val)
{
    if (val.type == RMAKER_VAL_TYPE_BOOLEAN) {
        return esp_matter_bool(val.val.b);
    } else if (val.type == RMAKER_VAL_TYPE_INTEGER) {
        return esp_matter_int(val.val.i);
    } else if (val.type == RMAKER_VAL_TYPE_FLOAT) {
        return esp_matter_float(val.val.f);
    } else {
        ESP_LOGE(TAG, "Invalid val type: %d", val.type);
    }
    return esp_matter_int(0);
}

esp_err_t app_rainmaker_attribute_update(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t *val)
{
    const char *device_name = app_rainmaker_get_device_name_from_id(endpoint_id);
    const char *param_name = app_rainmaker_get_param_name_from_id(cluster_id, attribute_id);
    if (!device_name || !param_name) {
        ESP_LOGD(TAG, "Device name or param name not handled");
        return ESP_FAIL;
    }

    const esp_rmaker_node_t *node = esp_rmaker_get_node();
    esp_rmaker_device_t *device = esp_rmaker_node_get_device_by_name(node, device_name);
    esp_rmaker_param_t *param = esp_rmaker_device_get_param_by_name(device, param_name);
    esp_rmaker_param_val_t rmaker_val = app_rainmaker_get_rmaker_val(val);
    if (!param) {
        ESP_LOGE(TAG, "Param not found");
        return ESP_FAIL;
    }

    return esp_rmaker_param_update_and_report(param, rmaker_val);
}

/* Callback to handle commands received from the RainMaker cloud */
static esp_err_t write_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
                          const esp_rmaker_param_val_t val, void *priv_data, esp_rmaker_write_ctx_t *ctx)
{
    if (ctx) {
        ESP_LOGI(TAG, "Received write request via : %s", esp_rmaker_device_cb_src_to_str(ctx->src));
    }

    const char *device_name = esp_rmaker_device_get_name(device);
    const char *param_name = esp_rmaker_param_get_name(param);

    int endpoint_id = app_rainmaker_get_endpoint_id_from_name(device_name);
    int cluster_id = app_rainmaker_get_cluster_id_from_name(param_name);
    int attribute_id = app_rainmaker_get_attribute_id_from_name(param_name);
    esp_matter_attr_val_t matter_val = app_rainmaker_get_matter_val(val);

    return esp_matter_attribute_update(endpoint_id, cluster_id, attribute_id, &matter_val);
}

static void app_rainmaker_device_create()
{
    const esp_rmaker_node_t *node = esp_rmaker_get_node();
    esp_matter_node_t *matter_node = esp_matter_node_get();
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_get_first(matter_node);

    /* Parse all endpoints */
    while (endpoint) {
        int endpoint_id = esp_matter_endpoint_get_id(endpoint);
        const char *device_name = app_rainmaker_get_device_name_from_id(endpoint_id);

        /* Proceed only if endpoint_id has been handled */
        if (device_name) {
            int device_type_id = esp_matter_endpoint_get_device_type_id(endpoint_id);
            const char *device_type = app_rainmaker_get_device_type_from_id(device_type_id);
            esp_rmaker_device_t *device = esp_rmaker_device_create(device_name, device_type, NULL);
            if (!device) {
                ESP_LOGE(TAG, "Could not create rainmaker device");
                continue;
            }
            esp_rmaker_device_add_cb(device, write_cb, NULL);
            esp_rmaker_node_add_device(node, device);

            esp_matter_cluster_t *cluster = esp_matter_cluster_get_first(endpoint);
            /* Parse all clusters */
            while (cluster) {
                int cluster_id = esp_matter_cluster_get_id(cluster);
                esp_matter_attribute_t *attribute = esp_matter_attribute_get_first(cluster);
                /* Parse all attributes */
                while (attribute) {
                    int attribute_id = esp_matter_attribute_get_id(attribute);
                    const char *param_name = app_rainmaker_get_param_name_from_id(cluster_id, attribute_id);
                    /* Proceed only if attribute_id corresponding to the cluster_id is handled */
                    if (param_name) {
                        const char *param_type = app_rainmaker_get_param_type_from_id(cluster_id, attribute_id);
                        const char *ui_type = app_rainmaker_get_param_ui_type_from_id(cluster_id, attribute_id);
                        int min = 0, max = 0, step = 0;
                        bool add_bounds = app_rainmaker_get_param_bounds_from_id(cluster_id, attribute_id, &min, &max,
                                                                                 &step);

                        esp_matter_attr_val_t val = esp_matter_invalid(NULL);
                        esp_matter_attribute_get_val(attribute, &val);
                        esp_rmaker_param_val_t rmaker_val = app_rainmaker_get_rmaker_val(&val);
                        esp_rmaker_param_t *param = esp_rmaker_param_create(param_name, param_type, rmaker_val,
                                                                            PROP_FLAG_READ | PROP_FLAG_WRITE);

                        /* Add additional param details */
                        if (ui_type) {
                            esp_rmaker_param_add_ui_type(param, ui_type);
                        }
                        if (add_bounds) {
                            esp_rmaker_param_add_bounds(param, esp_rmaker_int(min), esp_rmaker_int(max),
                                                        esp_rmaker_int(step));
                        }
                        esp_rmaker_device_add_param(device, param);
                    }
                    attribute = esp_matter_attribute_get_next(attribute);
                }
                cluster = esp_matter_cluster_get_next(cluster);
            }
        }
        endpoint = esp_matter_endpoint_get_next(endpoint);
    }
}

#define ESP_MATTER_RAINMAKER_ENDPOINT_ID 0x0    /* Same as root node endpoint. This will always be endpoint_id 0. */
#define ESP_MATTER_RAINMAKER_CLUSTER_ID 0x131B0000   /* 0x131B == manufacturer code */
#define ESP_MATTER_RAINMAKER_STATUS_ATTRIBUTE_ID 0x0
#define ESP_MATTER_RAINMAKER_CONFIGURATION_COMMAND_ID 0x0
#define ESP_MATTER_RAINMAKER_CLUSTER_REVISION 1
#define ESP_MATTER_RAINMAKER_COMMAND_LIMIT 5    /* This command can be called 5 times per reboot */
#define ESP_MATTER_RAINMAKER_MAX_DATA_LEN 40

static esp_err_t app_rainmaker_status_attribute_update(bool status)
{
    int endpoint_id = ESP_MATTER_RAINMAKER_ENDPOINT_ID;
    int cluster_id = ESP_MATTER_RAINMAKER_CLUSTER_ID;
    int attribute_id = ESP_MATTER_RAINMAKER_STATUS_ATTRIBUTE_ID;
    esp_matter_attr_val_t val = esp_matter_bool(status);
    return esp_matter_attribute_update(endpoint_id, cluster_id, attribute_id, &val);
}

static void app_rainmaker_user_node_association_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id,
                                                              void* event_data)
{
    /* This event handler is only for user node association status */
    if (event_base == RMAKER_EVENT) {
        if (event_id == RMAKER_EVENT_USER_NODE_MAPPING_DONE) {
            ESP_LOGI(TAG, "User node association complete. Updating the status attribute.");
            app_rainmaker_status_attribute_update(true);
        } else if (event_id == RMAKER_EVENT_USER_NODE_MAPPING_RESET) {
            ESP_LOGI(TAG, "User node association reset. Updating the status attribute.");
            app_rainmaker_status_attribute_update(false);
        }
    }
}

esp_err_t app_rainmaker_command_callback(int endpoint_id, int cluster_id, int command_id, TLVReader &tlv_data,
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

esp_err_t app_rainmaker_custom_cluster_create()
{
    /* Get the endpoint */
    esp_matter_node_t *node = esp_matter_node_get();
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_get(node, ESP_MATTER_RAINMAKER_ENDPOINT_ID);

    /* Create custom rainmaker cluster */
    esp_matter_cluster_t *cluster = esp_matter_cluster_create(endpoint, ESP_MATTER_RAINMAKER_CLUSTER_ID, ESP_MATTER_CLUSTER_FLAG_SERVER);
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

esp_err_t app_rainmaker_init()
{
    /* Add custom rainmaker cluster */
    return app_rainmaker_custom_cluster_create();
}

esp_err_t app_rainmaker_start()
{
    /* Initialize the ESP RainMaker Agent.
     * Note that this should be called after app_wifi_init() but before app_wifi_start()
     * */
    esp_rmaker_config_t rainmaker_cfg = {
        .enable_time_sync = false,
    };
    esp_rmaker_node_t *node = esp_rmaker_node_init(&rainmaker_cfg, "ESP RainMaker Device", "Lightbulb");
    if (!node) {
        ESP_LOGE(TAG, "Could not initialise node.");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        return ESP_FAIL;
    }

    /* Create a device and add the relevant parameters to it */
    app_rainmaker_device_create();

    /* Enable OTA */
    esp_rmaker_ota_config_t ota_config = {
        .server_cert = ESP_RMAKER_OTA_DEFAULT_SERVER_CERT,
    };
    esp_rmaker_ota_enable(&ota_config, OTA_USING_PARAMS);

    /* Enable timezone service which will be require for setting appropriate timezone
     * from the phone apps for scheduling to work correctly.
     * For more information on the various ways of setting timezone, please check
     * https://rainmaker.espressif.com/docs/time-service.html.
     */
    esp_rmaker_timezone_service_enable();

    /* Enable scheduling. */
    esp_rmaker_schedule_enable();

    /* Check user node association */
    if (esp_rmaker_user_node_mapping_get_state() == ESP_RMAKER_USER_MAPPING_DONE) {
        app_rainmaker_status_attribute_update(true);
    }
    /* Register an event handler and update the state later */
    esp_event_handler_register(RMAKER_EVENT, RMAKER_EVENT_USER_NODE_MAPPING_DONE,
                                &app_rainmaker_user_node_association_event_handler, NULL);
    esp_event_handler_register(RMAKER_EVENT, RMAKER_EVENT_USER_NODE_MAPPING_RESET,
                                &app_rainmaker_user_node_association_event_handler, NULL);

    /* Start the ESP RainMaker Agent */
    esp_rmaker_start();

    app_rainmaker_register_commands();
    return ESP_OK;
}
