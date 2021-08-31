/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>

#include "esp_matter.h"
#include "esp_matter_standard.h"
#include "app_driver.h"
#include "app_matter.h"
#include "app_constants.h"

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lib/shell/Engine.h"

#define APP_MAIN_NAME "Main"
static const char *TAG = "app_main";

#if CONFIG_ENABLE_CHIP_SHELL
void ChipShellTask(void *args)
{
    chip::Shell::Engine::Root().RunMainLoop();
}

CHIP_ERROR app_cli_common_handler(int argc, char** argv)
{
    /* This common handler is added to avoid adding `CHIP_ERROR` and its component requirements in other esp-matter components */
    if (argc <= 0) {
        ESP_LOGE(TAG, "Incorrect arguments");
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    if (strncmp(argv[0], "driver", sizeof("driver")) == 0) {
        app_driver_cli_handler(argc - 1, &argv[1]);
    } else {
        ESP_LOGE(TAG, "Incorrect arguments");
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    return CHIP_NO_ERROR;
}

static void app_cli_register_commands()
{
    static chip::Shell::shell_command_t cmds[] = {
        {
            .cmd_func = &app_cli_common_handler,
            .cmd_name = "esp",
            .cmd_help = "driver: This can be used to simulate on-device control. Usage: chip esp driver <set|get> <endpoint_name> <attribute_name> [value]. Example1: chip esp driver set Light Power 1. Example2: chip esp driver get Light Power.",
        },
    };
    int cmds_num = sizeof(cmds) / sizeof(chip::Shell::shell_command_t);
    chip::Shell::Engine::Root().RegisterCommands(cmds, cmds_num);
}
#endif // CONFIG_ENABLE_CHIP_SHELL

static esp_err_t app_main_attribute_update(const char *endpoint, const char *attribute, esp_matter_attr_val_t val, void *priv_data)
{
    /* Just adding this callback to notify the application */
    switch(val.type) {
        case ESP_MATTER_VAL_TYPE_BOOLEAN:
            ESP_LOGD(TAG, "%s's %s is %d", endpoint, attribute, val.val.b);
            break;

        case ESP_MATTER_VAL_TYPE_INTEGER:
            ESP_LOGD(TAG, "%s's %s is %d", endpoint, attribute, val.val.i);
            break;

        case ESP_MATTER_VAL_TYPE_FLOAT:
            ESP_LOGD(TAG, "%s's %s is %f", endpoint, attribute, val.val.f);
            break;

        case ESP_MATTER_VAL_TYPE_STRING:
        case ESP_MATTER_VAL_TYPE_OBJECT:
        case ESP_MATTER_VAL_TYPE_ARRAY:
            ESP_LOGD(TAG, "%s's %s is %s", endpoint, attribute, val.val.s);
            break;

        default:
            ESP_LOGD(TAG, "%s's %s is <invalid value>", endpoint, attribute);
            break;
    }
    return ESP_OK;
}

extern "C" void app_main()
{
    /* Initialize the ESP NVS layer */
    ESP_ERROR_CHECK(nvs_flash_init());

    /* Initialize esp_matter */
    esp_matter_init();
    esp_matter_attribute_callback_add(APP_MAIN_NAME, app_main_attribute_update, NULL);

    /* Initialize driver */
    app_driver_init();

    /* Initialize chip */
    ESP_ERROR_CHECK(app_matter_init());

    /* Set the default attribute values */
    esp_matter_attribute_notify(APP_MAIN_NAME, ESP_MATTER_ENDPOINT_LIGHT, ESP_MATTER_ATTR_POWER, esp_matter_bool(DEFAULT_POWER));
    esp_matter_attribute_notify(APP_MAIN_NAME, ESP_MATTER_ENDPOINT_LIGHT, ESP_MATTER_ATTR_BRIGHTNESS, esp_matter_int(DEFAULT_BRIGHTNESS));
    esp_matter_attribute_notify(APP_MAIN_NAME, ESP_MATTER_ENDPOINT_LIGHT, ESP_MATTER_ATTR_HUE, esp_matter_int(DEFAULT_HUE));
    esp_matter_attribute_notify(APP_MAIN_NAME, ESP_MATTER_ENDPOINT_LIGHT, ESP_MATTER_ATTR_SATURATION, esp_matter_int(DEFAULT_SATURATION));

#if CONFIG_ENABLE_CHIP_SHELL
    xTaskCreate(&ChipShellTask, "chip_shell", 2048, NULL, 5, NULL);
    app_cli_register_commands();
#endif
}
