/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "app_driver.h"
#include "app_matter.h"
#include "app_constants.h"
#include "app_rainmaker.h"

#include "esp_console.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lib/shell/Engine.h"

typedef enum param_type {
    PARAM_TYPE_POWER = 0,
    PARAM_TYPE_BRIGHTNESS,
    PARAM_TYPE_MAX,
} param_type_t;

static int cli_handler(int argc, char *argv[])
{
    if (argc != 3) {
        ESP_LOGE(APP_LOG_TAG, "Incorrect arguments");
        return 0;
    }
    param_type_t param_type = (param_type_t)atoi(argv[1]);
    int value = atoi(argv[2]);

    if (param_type == PARAM_TYPE_POWER) {
        app_driver_update_and_report_power(value, APP_DRIVER_SRC_LOCAL);
    } else if (param_type == PARAM_TYPE_BRIGHTNESS) {
        app_driver_update_and_report_brightness(value, APP_DRIVER_SRC_LOCAL);
    } else {
        ESP_LOGE(APP_LOG_TAG, "Param type not handled: %d", param_type);
    }
    return 0;
}

static esp_console_cmd_t driver_cmds[] = {
    {
        .command = "driver",
        .help = "This can be used to simulate on-device control. Usage: driver <param_type> <value>",
        .hint = NULL,
        .func = cli_handler,
        .argtable = NULL
    },
};

static esp_err_t cli_init()
{
    int cmds_num = sizeof(driver_cmds) / sizeof(esp_console_cmd_t);
    int i;
    for (i = 0; i < cmds_num; i++) {
        ESP_LOGI(APP_LOG_TAG, "Registering command: %s", driver_cmds[i].command);
        esp_console_cmd_register(&driver_cmds[i]);
    }
    return ESP_OK;
}

#if CONFIG_ENABLE_CHIP_SHELL
void ChipShellTask(void *args)
{
    chip::Shell::Engine::Root().RunMainLoop();
}
#endif // CONFIG_ENABLE_CHIP_SHELL

extern "C" void app_main()
{
    // Initialize the ESP NVS layer.
    ESP_ERROR_CHECK(nvs_flash_init());

    /* Initialize and set the default params */
    app_driver_init();

    ESP_LOGI(APP_LOG_TAG, "==================================================");
    ESP_LOGI(APP_LOG_TAG, "esp-matter-rainmaker-light example starting");
    ESP_LOGI(APP_LOG_TAG, "==================================================");

    /* Initialize chip */
    ESP_ERROR_CHECK(app_matter_init());

    /* Initialize rainmaker */
    app_rmaker_init();

    app_driver_update_and_report_power(DEFAULT_POWER, APP_DRIVER_SRC_LOCAL);
    app_driver_update_and_report_brightness(DEFAULT_BRIGHTNESS, APP_DRIVER_SRC_LOCAL);
    app_driver_update_and_report_hue(DEFAULT_HUE, APP_DRIVER_SRC_LOCAL);
    app_driver_update_and_report_saturation(DEFAULT_SATURATION, APP_DRIVER_SRC_LOCAL);

    /* Register CLI commands with esp_console (indirectly, rainmaker's console). */
    cli_init();

#if CONFIG_ENABLE_CHIP_SHELL
    /* Rainmaker console is enabled. So disabling this. */
    // xTaskCreate(&ChipShellTask, "chip_shell", 2048, NULL, 5, NULL);
#endif
}
