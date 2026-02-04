/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_event.h>
#include <esp_matter.h>
#include <esp_matter_bridge.h>
#include <esp_rmaker_standard_types.h>
#include "esp_rmaker_standard_params.h"
#include <json_parser.h>
#include <json_generator.h>
#include <app_bridged_device.h>
#include <rainmaker_api.h>
#include <rainmaker_controller_std.h>
#include <app_network.h>
#include <common_macros.h>
#include <rainmaker_bridge.h>
#include <mdns.h>
#if CONFIG_OPENTHREAD_BORDER_ROUTER
#include <app_thread_config.h>
#include <esp_rmaker_thread_br.h>
#include <platform/ESP32/OpenthreadLauncher.h>
#ifdef CONFIG_AUTO_UPDATE_RCP
#include <esp_rcp_update.h>
#include <esp_spiffs.h>
#endif
#endif

#define INVALID_MATTER_DEVICE_TYPE 0xFFFF

static const char *TAG = "rainmaker_bridge";

using namespace chip;
using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

extern uint16_t aggregator_endpoint_id;

static esp_err_t attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, int value)
{
    esp_err_t err = ESP_OK;
    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);

    if (cluster_id == OnOff::Id) {
        if (attribute_id == OnOff::Attributes::OnOff::Id) {
            if (val.val.b != (bool)value) {
                val.val.b = (bool)value;
            } else {
                return err;
            }
        }
    } else if (cluster_id == LevelControl::Id) {
        if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
            if (val.val.u8 != REMAP_TO_RANGE(value, RMAKER_LEVEL_MAX_VALUE, MATTER_LEVEL_MAX_VALUE)) {
                val.val.u8 = REMAP_TO_RANGE(value, RMAKER_LEVEL_MAX_VALUE, MATTER_LEVEL_MAX_VALUE);
            } else {
                return err;
            }
        }
    } else if (cluster_id == ColorControl::Id) {
        if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
            if (val.val.u8 != REMAP_TO_RANGE(value, RMAKER_HUE_MAX_VALUE, MATTER_HUE_MAX_VALUE)) {
                val.val.u8 = REMAP_TO_RANGE(value, RMAKER_HUE_MAX_VALUE, MATTER_HUE_MAX_VALUE);
            } else {
                return err;
            }
        } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
            if (val.val.u8 != REMAP_TO_RANGE(value, RMAKER_SATURATION_MAX_VALUE, MATTER_SATURATION_MAX_VALUE)) {
                val.val.u8 = REMAP_TO_RANGE(value, RMAKER_SATURATION_MAX_VALUE, MATTER_SATURATION_MAX_VALUE);
            } else {
                return err;
            }
        } else if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id) {
            if (val.val.u16 != REMAP_TO_RANGE_INVERSE(value, STANDARD_TEMPERATURE_FACTOR)) {
                val.val.u16 = REMAP_TO_RANGE_INVERSE(value, STANDARD_TEMPERATURE_FACTOR);
            } else {
                return err;
            }
        }
    } else if (cluster_id == WindowCovering::Id) {
        /* windowcovering was not supported yet */
        return err;
    }

    attribute::report(endpoint_id, cluster_id, attribute_id, &val);

    return err;
}

static esp_err_t get_attribute_value_from_rainmaker_device(uint16_t endpoint_id, const char *node_id, const char *node_name)
{
    int attribute_value;
    jparse_ctx_t jctx;
    bool power;

    char* receive_buffer = esp_rainmaker_api_get_node_params(node_id);

    if (receive_buffer == NULL) {
        return ESP_FAIL;
    }

    if (json_parse_start(&jctx, receive_buffer, strlen(receive_buffer)) != 0) {
        free(receive_buffer);
        return ESP_FAIL;
    }

    if (node_name[0] != 0) {
        if (json_obj_get_object(&jctx, node_name) == 0) {
            if (json_obj_get_int(&jctx, "Brightness", &attribute_value) == 0) {
                if (attribute_value > 0) {
                    attribute_update(endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id, attribute_value);
                }
            }

            if (json_obj_get_int(&jctx, "Hue", &attribute_value) == 0) {
                attribute_update(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentHue::Id, attribute_value);
            }

            if (json_obj_get_bool(&jctx, "Power", &power) == 0) {
                attribute_value = power;
                attribute_update(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, attribute_value);
            }

            if (json_obj_get_int(&jctx, "Saturation", &attribute_value) == 0) {
                attribute_update(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentSaturation::Id, attribute_value);
            }

            if (json_obj_get_int(&jctx, "CCT", &attribute_value) == 0) {
                attribute_update(endpoint_id, ColorControl::Id, ColorControl::Attributes::ColorTemperatureMireds::Id, attribute_value);
            }
        } else {
            ESP_LOGE(TAG, "No light param found in json ");
        }
        json_obj_leave_object(&jctx);
    } else {
        /* Todo: add other device types later */
    }

    json_parse_end(&jctx);

    free(receive_buffer);

    return ESP_OK;
}

static uint32_t matter_get_device_type_from_rainmaker_device(const char *input_buf, size_t buf_length, const char *node_name)
{
    jparse_ctx_t jctx;
    int param_count;
    int device_count;
    char type[32];
    char param_type[32];
    uint32_t matter_device_type_id = INVALID_MATTER_DEVICE_TYPE;

    if (json_parse_start(&jctx, input_buf, buf_length) != 0) {
        return INVALID_MATTER_DEVICE_TYPE;
    }

    if (json_obj_get_array(&jctx, "devices", &device_count) == 0) {
        if (json_arr_get_object(&jctx, 0) == 0) {
            if (json_obj_get_string(&jctx, "type", type, sizeof(type)) == 0) {
                /* get device type */
                if ((strcmp(type, ESP_RMAKER_DEVICE_LIGHTBULB) == 0) || (strcmp(type, ESP_RMAKER_DEVICE_LIGHT) == 0)) {
                    matter_device_type_id = ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID;
                    if (json_obj_get_array(&jctx, "params", &param_count) == 0) {
                        for (int i = 0; i < param_count; i++) {
                            if (json_arr_get_object(&jctx, i) == 0) {
                                if (json_obj_get_string(&jctx, "type", param_type, sizeof(param_type)) == 0) {
                                    /* get param type */
                                    if (((strcmp(param_type, ESP_RMAKER_PARAM_HUE) == 0) || (strcmp(param_type, ESP_RMAKER_PARAM_SATURATION) == 0)) &&
                                            (matter_device_type_id < ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID)) {
                                        matter_device_type_id = ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID;
                                    } else if ((strcmp(param_type, ESP_RMAKER_PARAM_CCT) == 0) &&
                                               (matter_device_type_id < ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID)) {
                                        matter_device_type_id = ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID;
                                    } else if (((strcmp(param_type, ESP_RMAKER_PARAM_BRIGHTNESS) == 0) || (strcmp(param_type, ESP_RMAKER_PARAM_INTENSITY) == 0)) &&
                                               (matter_device_type_id < ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID)) {
                                        matter_device_type_id = ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID;
                                    }
                                }
                                json_arr_leave_object(&jctx);
                            }
                        }
                        json_obj_leave_array(&jctx);
                    } else {
                        ESP_LOGE(TAG, "No parameters found in json ");
                    }
                } else {
                    /* Todo: add other device types */
                    ESP_LOGW(TAG, "unsupported device type\n");
                }
            } else {
                ESP_LOGE(TAG, "No type found in json ");
            }

            if (json_obj_get_string(&jctx, "name", node_name, 32) != 0) {
                ESP_LOGE(TAG, "Get node name failed");
            }

            json_obj_leave_object(&jctx);
        } else {
            ESP_LOGE(TAG, "No devices found in array ");
        }
        json_obj_leave_array(&jctx);
    } else {
        ESP_LOGE(TAG, "No devices found in json ");
    }

    json_parse_end(&jctx);

    return matter_device_type_id;
}

static esp_err_t rainmaker_bridge_match_device(const char *node_id, const char *node_name, uint32_t device_type_id)
{
    node_t *node = node::get();

    if (app_bridge_get_matter_endpointid_by_rainmaker_node_id(node_id) == chip::kInvalidEndpointId) {
        app_bridged_device_t *bridged_device = app_bridge_create_bridged_device(node, aggregator_endpoint_id, device_type_id,
                                                                                ESP_MATTER_BRIDGED_DEVICE_TYPE_RAINMAKER,
                                                                                app_bridge_rainmaker_address(node_id, node_name), NULL);
        ESP_RETURN_ON_FALSE(bridged_device, ESP_FAIL, TAG, "Failed to create bridged device (rainmaker device)");
    } else {
        ESP_LOGI(TAG, "Bridged node for %s rainmaker device has been created", node_id);
    }

    return ESP_OK;
}

static esp_err_t rainmaker_bridge_get_param_from_device(const char* node_id, uint16_t endpoint_id)
{
    node_t *node = node::get();
    endpoint_t *dev_endpoint = endpoint::get(node, endpoint_id);
    uint8_t device_type_count = endpoint::get_device_type_count(dev_endpoint);
    uint32_t dev_type_id;
    uint8_t dev_type_ver;

    for (uint8_t i = 0; i < device_type_count; ++i) {
        if ((ESP_OK == endpoint::get_device_type_at_index(dev_endpoint, i, dev_type_id, dev_type_ver))) {
            ESP_LOGI(TAG, "Endpoint Id: %d--Node Id: %s Device Type: %ld\n", endpoint_id, node_id, dev_type_id);
            if (dev_type_id != endpoint::bridged_node::get_device_type_id()) {
                break;
            }
        }
    }

    switch (dev_type_id) {
    case ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID:
    case ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID:
    case ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID:
    case ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID: {
        get_attribute_value_from_rainmaker_device(endpoint_id, node_id, app_bridge_get_rainmaker_node_name_by_matter_endpointid(endpoint_id));
    }
    break;
    /* Todo: add other device types */
    default:
        break;
    }
    return ESP_OK;
}

static esp_err_t rainmaker_bridge_update_online_state(const char* node_id, uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    bool connection_status = false;
    attribute_t *attribute = attribute::get(endpoint_id, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::Reachable::Id);
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);

    err = esp_rainmaker_api_get_node_connection_status(node_id, &connection_status);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get node %s connection status", node_id);
    } else {
        ESP_LOGI(TAG, "node %s connection status: %d", node_id, connection_status);
        if (val.val.b != connection_status) {
            val.val.b = connection_status;
            attribute::update(endpoint_id, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::Reachable::Id, &val);
        }
    }
    return err;
}

static esp_err_t rainmaker_bridge_add_new_device(const char *node_id)
{
    uint32_t matter_device_type = 0;
    char node_name[32] = {0};

    if (node_id != NULL) {
        char* receive_buffer = esp_rainmaker_api_get_node_config(node_id);
        if (receive_buffer == NULL) {
            ESP_LOGE(TAG, "Get Node %s config failed\n", node_id);
            return ESP_FAIL;
        }
        matter_device_type = matter_get_device_type_from_rainmaker_device(receive_buffer, strlen(receive_buffer), node_name);
        if ((matter_device_type != INVALID_MATTER_DEVICE_TYPE) && (node_name[0] != 0)) {
            rainmaker_bridge_match_device(node_id, node_name, matter_device_type);
        } else {
            ESP_LOGW(TAG, "Node %s device type 0x%lx is invalid\n", node_id, matter_device_type);
            free(receive_buffer);
            return ESP_ERR_NOT_FOUND;
        }
        free(receive_buffer);
    }
    return ESP_OK;
}

static esp_err_t rainmaker_bridge_delete_device(uint16_t endpoint_id)
{
    const char* node_id = app_bridge_get_rainmaker_node_id_by_matter_endpointid(endpoint_id);

    if (node_id == NULL) {
        ESP_LOGI(TAG, "Can't find rainmaker device from ep: %d", endpoint_id);
        return ESP_FAIL;
    }

    app_bridged_device_t *bridged_device = app_bridge_get_device_by_rainmaker_node_id(node_id);

    if (bridged_device) {
        app_bridge_remove_device(bridged_device);
        ESP_LOGI(TAG, "Bridged rainmaker device removed: %s", node_id);
    } else {
        ESP_LOGI(TAG, "Bridged rainmaker device not found: %s", node_id);
    }
    return ESP_OK;
}

static void matter_check_and_remove_not_exist_device()
{
    uint16_t matter_endpoint_id_array[MAX_BRIDGED_DEVICE_COUNT];
    esp_matter_bridge::get_bridged_endpoint_ids(matter_endpoint_id_array);

    for (int i = 0; i < MAX_BRIDGED_DEVICE_COUNT; i++) {
        if (matter_endpoint_id_array[i] != chip::kInvalidEndpointId) {
            const char *node_id = app_bridge_get_rainmaker_node_id_by_matter_endpointid(matter_endpoint_id_array[i]);
            if (node_id != NULL) {
                char *buffer = esp_rainmaker_api_get_node_params(node_id);
                if (buffer == NULL) {
                    ESP_LOGI(TAG, "Remove not exist Rainmaker device Node: %s Endpoint: %d\n", node_id, matter_endpoint_id_array[i]);
                    rainmaker_bridge_delete_device(matter_endpoint_id_array[i]);
                } else {
                    free(buffer);
                }
            }
        }
    }
}

static esp_err_t rainmaker_sync_nodes(char *out_buf, size_t out_buf_len)
{
    jparse_ctx_t jctx;
    int total_count;
    char node[32];
    static uint8_t check_count = 0;
    if (json_parse_start(&jctx, out_buf, out_buf_len) != 0) {
        return ESP_FAIL;
    }

    if (json_obj_get_array(&jctx, "nodes", &total_count) == 0) {
        for (int i = 0; i < total_count; i++) {
            if (json_arr_get_string(&jctx, i, node, sizeof(node)) == 0) {
                uint16_t endpoint_id = app_bridge_get_matter_endpointid_by_rainmaker_node_id(node);
                if (endpoint_id == chip::kInvalidEndpointId) {
                    if (rainmaker_bridge_add_new_device(node) == ESP_OK) {
                        endpoint_id = app_bridge_get_matter_endpointid_by_rainmaker_node_id(node);
                    }
                }

                ESP_LOGI(TAG, "Exist node: %s--endpoint id: %d\n", node, endpoint_id);
                if (endpoint_id != chip::kInvalidEndpointId) {
                    rainmaker_bridge_get_param_from_device(node, endpoint_id);
                    rainmaker_bridge_update_online_state(node, endpoint_id);
                }
            }
        }
        json_obj_leave_array(&jctx);
    } else {
        ESP_LOGE(TAG, "No node found in json ");
    }

    json_parse_end(&jctx);

    check_count++;
    if (check_count >= 5) {
        matter_check_and_remove_not_exist_device();
        check_count = 0;
    }
    return ESP_OK;
}

esp_err_t rainmaker_bridge_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    char param_buffer[128] = {0};
    const char* node_id = app_bridge_get_rainmaker_node_id_by_matter_endpointid(endpoint_id);
    const char* node_name = app_bridge_get_rainmaker_node_name_by_matter_endpointid(endpoint_id);
    if (node_id == NULL) {
        return ESP_OK;
    }

    esp_err_t err = ESP_OK;
    if (cluster_id == OnOff::Id) {
        if (attribute_id == OnOff::Attributes::OnOff::Id) {
            snprintf(param_buffer, sizeof(param_buffer), "[{\"node_id\":\"%s\",\"payload\":{\"%s\":{\"Power\":%s}}}]", node_id, node_name, val->val.b ? "true" : "false");
        }
    } else if (cluster_id == LevelControl::Id) {
        if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
            attribute_t *attribute = attribute::get(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id);
            esp_matter_attr_val_t val_onoff = esp_matter_invalid(NULL);
            attribute::get_val(attribute, &val_onoff);
            if (val_onoff.val.b == false) {
                return ESP_OK;
            }
            snprintf(param_buffer, sizeof(param_buffer), "[{\"node_id\":\"%s\",\"payload\":{\"%s\":{\"Brightness\":%d}}}]", node_id, node_name,
                     REMAP_TO_RANGE(val->val.u8, MATTER_LEVEL_MAX_VALUE, RMAKER_LEVEL_MAX_VALUE));
        }
    } else if (cluster_id == ColorControl::Id) {
        if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
            snprintf(param_buffer, sizeof(param_buffer), "[{\"node_id\":\"%s\",\"payload\":{\"%s\":{\"Hue\":%d}}}]", node_id, node_name,
                     REMAP_TO_RANGE(val->val.u8, MATTER_HUE_MAX_VALUE, RMAKER_HUE_MAX_VALUE));
        } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
            snprintf(param_buffer, sizeof(param_buffer), "[{\"node_id\":\"%s\",\"payload\":{\"%s\":{\"Saturation\":%d}}}]", node_id, node_name,
                     REMAP_TO_RANGE(val->val.u8, MATTER_SATURATION_MAX_VALUE, RMAKER_SATURATION_MAX_VALUE));
        } else if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id) {
            snprintf(param_buffer, sizeof(param_buffer), "[{\"node_id\":\"%s\",\"payload\":{\"%s\":{\"CCT\":%d}}}]", node_id, node_name,
                     REMAP_TO_RANGE_INVERSE(val->val.u16, STANDARD_TEMPERATURE_FACTOR));
        }
    }

    if (param_buffer[0] == 0) {
        return ESP_OK;
    }

    esp_rainmaker_api_set_node_params(param_buffer);
    return err;
}

static void rainmaker_bridge_task(void *pvParameters)
{
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(CONFIG_RAINMAKER_PARAMS_GET_PERIOD_MS));
        char* nodes_buffer = esp_rainmaker_api_get_nodes_list();
        if (nodes_buffer == NULL) {
            continue;
        }
        rainmaker_sync_nodes(nodes_buffer, strlen(nodes_buffer));
        free(nodes_buffer);
    }
}

static esp_err_t write_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
                          const esp_rmaker_param_val_t val, void *priv_data, esp_rmaker_write_ctx_t *ctx)
{
    if (strcmp(esp_rmaker_param_get_type(param), ESP_RMAKER_PARAM_BASE_URL) == 0) {
        if (val.type != RMAKER_VAL_TYPE_STRING || !val.val.s) {
            return ESP_ERR_INVALID_ARG;
        }
        ESP_LOGI(TAG, "Set base url: %s\n", val.val.s);
        ESP_RETURN_ON_ERROR(esp_rainmaker_api_set_base_url(val.val.s), TAG, "Failed to set base_url");
    } else if (strcmp(esp_rmaker_param_get_type(param), ESP_RMAKER_PARAM_USER_TOKEN) == 0) {
        if (val.type != RMAKER_VAL_TYPE_STRING || !val.val.s) {
            return ESP_ERR_INVALID_ARG;
        }
        ESP_LOGI(TAG, "Set user token: %s\n", val.val.s);
        ESP_RETURN_ON_ERROR(esp_rainmaker_api_set_refresh_token(val.val.s), TAG, "Failed to set user_token");
    }

    return ESP_OK;
}

#ifdef CONFIG_AUTO_UPDATE_RCP
static esp_err_t init_spiffs()
{
    esp_err_t err = ESP_OK;
    esp_vfs_spiffs_conf_t rcp_fw_conf = {.base_path = "/" CONFIG_RCP_PARTITION_NAME,
                                         .partition_label = CONFIG_RCP_PARTITION_NAME,
                                         .max_files = 10, .format_if_mount_failed = false
                                        };
    err = esp_vfs_spiffs_register(&rcp_fw_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount rcp firmware storage");
    }
    return err;
}
#endif // CONFIG_AUTO_UPDATE_RCP

/**
 * @brief Initialize the Rainmaker bridge
 *
 */
void rainmaker_init()
{
    /* Network Init */
    app_network_init();

    /* Initialize the ESP RainMaker Agent.
     * Note that this should be called after app_network_init() but before app_network_start()
     * */
    esp_rmaker_config_t rainmaker_cfg = {
        .enable_time_sync = false,
    };
    esp_rmaker_node_t *node = esp_rmaker_node_init(&rainmaker_cfg, "ESP RainMaker Device", "RainmakerController");
    if (!node) {
        ESP_LOGE(TAG, "Could not initialise node. Aborting!!!");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        abort();
    }

    esp_rmaker_device_t *device = esp_rmaker_device_create("Rainmaker Controller", ESP_RMAKER_DEVICE_CONTROLLER, NULL);
    esp_rmaker_device_add_param(device, esp_rmaker_name_param_create(ESP_RMAKER_DEF_NAME_PARAM, "RainmakerController"));
    esp_rmaker_node_add_device(node, device);
    esp_rmaker_node_add_device(node, rainmaker_controller_service_create("RainmakerCTL", write_cb, NULL, NULL));

#if CONFIG_OPENTHREAD_BORDER_ROUTER
    esp_openthread_platform_config_t thread_cfg = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()
    };
#ifdef CONFIG_AUTO_UPDATE_RCP
    ESP_ERROR_CHECK(init_spiffs());
    esp_rcp_update_config_t rcp_update_cfg = ESP_OPENTHREAD_RCP_UPDATE_CONFIG();
    esp_rmaker_thread_br_enable(&thread_cfg, &rcp_update_cfg);
#else
    esp_rmaker_thread_br_enable(&thread_cfg, NULL);
#endif // CONFIG_AUTO_UPDATE_RCP
#endif // CONFIG_OPENTHREAD_BORDER_ROUTER

    esp_rmaker_start();

    app_network_start(POP_TYPE_RANDOM);

    /* create task to get node and params from rainmaker side */
    xTaskCreate(rainmaker_bridge_task, "rainmaker_main", CONFIG_RAINMAKER_TASK_STACK_SIZE, xTaskGetCurrentTaskHandle(), 5, NULL);
}
