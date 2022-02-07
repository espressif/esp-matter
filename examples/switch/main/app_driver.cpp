/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <device.h>
#include <esp_matter.h>
#include <esp_matter_console.h>
#include <light_driver.h>

#include <app_driver.h>

static const char *TAG = "app_driver";

static esp_err_t app_driver_console_handler(int argc, char **argv)
{
    if (argc == 5 && strncmp(argv[0], "set", sizeof("set")) == 0) {
        int endpoint_id = strtol((const char *)&argv[1][2], NULL, 16);
        int cluster_id = strtol((const char *)&argv[2][2], NULL, 16);
        int attribute_id = strtol((const char *)&argv[3][2], NULL, 16);
        int value = atoi(argv[4]);

        esp_matter_attr_val_t val = esp_matter_uint8(value);

        /* Change val if bool */
        if (cluster_id == ZCL_ON_OFF_CLUSTER_ID && attribute_id == ZCL_ON_OFF_ATTRIBUTE_ID) {
            val.type = ESP_MATTER_VAL_TYPE_BOOLEAN;
            val.val.b = (bool)value;
        }
        esp_matter_attribute_update(endpoint_id, cluster_id, attribute_id, val);
    } else if (argc == 4 && strncmp(argv[0], "get", sizeof("get")) == 0) {
        int endpoint_id = strtol((const char *)&argv[1][2], NULL, 16);
        int cluster_id = strtol((const char *)&argv[2][2], NULL, 16);
        int attribute_id = strtol((const char *)&argv[3][2], NULL, 16);

        esp_matter_node_t *node = esp_matter_node_get();
        esp_matter_endpoint_t *endpoint = esp_matter_endpoint_get(node, endpoint_id);
        esp_matter_cluster_t *cluster = esp_matter_cluster_get(endpoint, cluster_id);
        esp_matter_attribute_t *attribute = esp_matter_attribute_get(cluster, attribute_id);
        esp_matter_attr_val_t val = esp_matter_attribute_get_val(attribute);
        esp_matter_attribute_val_print(endpoint_id, cluster_id, attribute_id, val);
    } else {
        ESP_LOGE(TAG, "Incorrect arguments");
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static void app_driver_register_commands()
{
    esp_matter_console_command_t command = {
        .name = "driver",
        .description = "This can be used to simulate on-device control. "
                       "Usage: matter esp driver <set|get> <endpoint_id> <cluster_id> <attribute_id> [value]. "
                       "Example1: matter esp driver set 0x1001 0x0006 0x0000 1. "
                       "Example2: matter esp driver get 0x1001 0x0006 0x0000.",
        .handler = app_driver_console_handler,
    };
    esp_matter_console_add_command(&command);
}

esp_err_t app_driver_attribute_update(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t val)
{
    esp_err_t err = ESP_OK;
   // Intentionally kept empty
 
    return err;
}

static esp_err_t app_driver_attribute_set_defaults()
{
    /* Get the default value (current value) from esp_matter and update the app_driver */
    esp_err_t err = ESP_OK;
    esp_matter_node_t *node = esp_matter_node_get();
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_get_first(node);
    while (endpoint) {
        int endpoint_id = esp_matter_endpoint_get_id(endpoint);
        esp_matter_cluster_t *cluster = esp_matter_cluster_get_first(endpoint);
        while (cluster) {
            int cluster_id = esp_matter_cluster_get_id(cluster);
            esp_matter_attribute_t *attribute = esp_matter_attribute_get_first(cluster);
            while (attribute) {
                int attribute_id = esp_matter_attribute_get_id(attribute);
                esp_matter_attr_val_t val = esp_matter_attribute_get_val(attribute);
                err |= app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, val);
                attribute = esp_matter_attribute_get_next(attribute);
            }
            cluster = esp_matter_cluster_get_next(cluster);
        }
        endpoint = esp_matter_endpoint_get_next(endpoint);
    }
    return err;
}

esp_err_t app_driver_init()
{
    // device_init();
    app_driver_attribute_set_defaults();
    app_driver_register_commands();
    return ESP_OK;
}
