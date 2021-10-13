/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <esp_log.h>

#include <esp_matter.h>
#include <esp_matter_standard.h>
#include <esp_matter_console.h>
#include <app_driver.h>
#include <device.h>
#include <light_driver.h>

#define APP_DRIVER_NAME "Driver"
static const char *TAG = "app_driver";

static esp_err_t app_driver_attribute_set(const char *endpoint, const char *attribute, esp_matter_attr_val_t val);
static esp_matter_attr_val_t app_driver_attribute_get(const char *endpoint, const char *attribute);

static void app_driver_print_attr_val(const char *endpoint, const char *attribute, esp_matter_attr_val_t val)
{
    switch(val.type) {
        case ESP_MATTER_VAL_TYPE_BOOLEAN:
            ESP_LOGI(TAG, "%s's %s is %d", endpoint, attribute, val.val.b);
            break;

        case ESP_MATTER_VAL_TYPE_INTEGER:
            ESP_LOGI(TAG, "%s's %s is %d", endpoint, attribute, val.val.i);
            break;

        case ESP_MATTER_VAL_TYPE_FLOAT:
            ESP_LOGI(TAG, "%s's %s is %f", endpoint, attribute, val.val.f);
            break;

        case ESP_MATTER_VAL_TYPE_STRING:
        case ESP_MATTER_VAL_TYPE_OBJECT:
        case ESP_MATTER_VAL_TYPE_ARRAY:
            ESP_LOGI(TAG, "%s's %s is %s", endpoint, attribute, val.val.s);
            break;

        default:
            ESP_LOGI(TAG, "%s's %s is <invalid value>", endpoint, attribute);
            break;
    }
}

static esp_err_t app_driver_console_handler(int argc, char** argv)
{
    if (argc == 4 && strncmp(argv[0], "set", sizeof("set")) == 0) {
        char *endpoint_name = argv[1];
        char *attribute_name = argv[2];
        int value = atoi(argv[3]);
        esp_matter_attr_val_t val = esp_matter_int(value);

        /* Change val if bool */
        if (strncmp(attribute_name, ESP_MATTER_ATTR_POWER, sizeof(ESP_MATTER_ATTR_POWER)) == 0) {
            val.type = ESP_MATTER_VAL_TYPE_BOOLEAN;
            val.val.b = (bool)value;
        }
        app_driver_attribute_set(endpoint_name, attribute_name, val);
    } else if (argc == 3 && strncmp(argv[0], "get", sizeof("get")) == 0) {
        char *endpoint_name = argv[1];
        char *attribute_name = argv[2];
        esp_matter_attr_val_t val = app_driver_attribute_get(endpoint_name, attribute_name);
        app_driver_print_attr_val(endpoint_name, attribute_name, val);
    } else {
        ESP_LOGE(TAG, "Incorrect arguments");
        return -1;
    }
    return 0;
}

static void app_driver_register_commands()
{
    esp_matter_console_command_t command= {
        .name = "driver",
        .description = "This can be used to simulate on-device control. Usage: chip esp driver <set|get> <endpoint_name> <attribute_name> [value]. Example1: chip esp driver set Light Power 1. Example2: chip esp driver get Light Power.",
        .handler = app_driver_console_handler,
    };
    esp_matter_console_add_command(&command);
}

static esp_matter_attr_val_t app_driver_attribute_get(const char *endpoint, const char *attribute)
{
    if (strncmp(endpoint, ESP_MATTER_ENDPOINT_LIGHT, sizeof(ESP_MATTER_ENDPOINT_LIGHT)) == 0) {
        if (strncmp(attribute, ESP_MATTER_ATTR_POWER, sizeof(ESP_MATTER_ATTR_POWER)) == 0) {
            return esp_matter_bool(light_driver_get_power());
        } else if (strncmp(attribute, ESP_MATTER_ATTR_BRIGHTNESS, sizeof(ESP_MATTER_ATTR_BRIGHTNESS)) == 0) {
            return esp_matter_int(light_driver_get_brightness());
        } else if (strncmp(attribute, ESP_MATTER_ATTR_HUE, sizeof(ESP_MATTER_ATTR_HUE)) == 0) {
            return esp_matter_int(light_driver_get_hue());
        } else if (strncmp(attribute, ESP_MATTER_ATTR_SATURATION, sizeof(ESP_MATTER_ATTR_SATURATION)) == 0) {
            return esp_matter_int(light_driver_get_saturation());
        } else if (strncmp(attribute, ESP_MATTER_ATTR_TEMPERATURE, sizeof(ESP_MATTER_ATTR_TEMPERATURE)) == 0) {
            return esp_matter_int(light_driver_get_temperature());
        } else {
            ESP_LOGI(TAG, "Attribute update not handled: %s", attribute);
        }
    } else {
        ESP_LOGI(TAG, "Endpoint not handled");
    }
    esp_matter_attr_val_t val = {
        .type = ESP_MATTER_VAL_TYPE_INVALID,
    };
    return val;
}

static esp_err_t app_driver_attribute_update(const char *endpoint, const char *attribute, esp_matter_attr_val_t val, void *priv_data)
{
    esp_err_t err = ESP_OK;
    if (strncmp(endpoint, ESP_MATTER_ENDPOINT_LIGHT, sizeof(ESP_MATTER_ENDPOINT_LIGHT)) == 0) {
        if (strncmp(attribute, ESP_MATTER_ATTR_POWER, sizeof(ESP_MATTER_ATTR_POWER)) == 0) {
            err = light_driver_set_power(val.val.b);
        } else if (strncmp(attribute, ESP_MATTER_ATTR_BRIGHTNESS, sizeof(ESP_MATTER_ATTR_BRIGHTNESS)) == 0) {
            err = light_driver_set_brightness(val.val.i);
        } else if (strncmp(attribute, ESP_MATTER_ATTR_HUE, sizeof(ESP_MATTER_ATTR_HUE)) == 0) {
            err = light_driver_set_hue(val.val.i);
        } else if (strncmp(attribute, ESP_MATTER_ATTR_SATURATION, sizeof(ESP_MATTER_ATTR_SATURATION)) == 0) {
            err = light_driver_set_saturation(val.val.i);
        } else if (strncmp(attribute, ESP_MATTER_ATTR_TEMPERATURE, sizeof(ESP_MATTER_ATTR_TEMPERATURE)) == 0) {
            err = light_driver_set_temperature(val.val.i);
        } else {
            ESP_LOGI(TAG, "Attribute update not handled: %s", attribute);
            err = ESP_ERR_NOT_FOUND;
        }
    } else {
        ESP_LOGI(TAG, "Endpoint not handled");
        err = ESP_ERR_NOT_FOUND;
    }
    return err;
}

static esp_err_t app_driver_attribute_set(const char *endpoint, const char *attribute, esp_matter_attr_val_t val)
{
    app_driver_attribute_update(endpoint, attribute, val, NULL);
    esp_matter_attribute_notify(APP_DRIVER_NAME, endpoint, attribute, val);
    return ESP_OK;
}

esp_err_t app_driver_init()
{
    device_init();
    esp_matter_attribute_callback_add(APP_DRIVER_NAME, app_driver_attribute_update, NULL);
    app_driver_register_commands();
    return ESP_OK;
}
