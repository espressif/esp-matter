/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "app_driver.h"
#include "app_matter.h"
#include "app_constants.h"

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lib/shell/Engine.h"

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
    ESP_LOGI(APP_LOG_TAG, "esp-matter-light example starting");
    ESP_LOGI(APP_LOG_TAG, "==================================================");

    /* Initialize chip */
    ESP_ERROR_CHECK(app_matter_init());

    app_driver_update_and_report_power(DEFAULT_POWER, APP_DRIVER_SRC_LOCAL);
    app_driver_update_and_report_brightness(DEFAULT_BRIGHTNESS, APP_DRIVER_SRC_LOCAL);
    app_driver_update_and_report_hue(DEFAULT_HUE, APP_DRIVER_SRC_LOCAL);
    app_driver_update_and_report_saturation(DEFAULT_SATURATION, APP_DRIVER_SRC_LOCAL);

#if CONFIG_ENABLE_CHIP_SHELL
    xTaskCreate(&ChipShellTask, "chip_shell", 2048, NULL, 5, NULL);
#endif
}
